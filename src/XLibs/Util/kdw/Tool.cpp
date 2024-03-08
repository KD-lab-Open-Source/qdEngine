#include "StdAfx.h"
#include "Render/src/Scene.h"
#include "Render/src/FT_Font.h"

#include "kdw/Tool.h"
#include "kdw/Document.h"
#include "kdw/Sandbox.h"
#include "kdw/ControllableViewport.h"
#include <string>
using std::vector;
using std::string;
#include "XMath/XMath.h"
#include "Render/src/cCamera.h"
#include "kdw/Serialization.h"
#include "Serialization/SerializationFactory.h"

#ifndef KDW_DLL

namespace kdw{

// ---------------------------------------------------------------------------

ToolManager::ToolManager()
{
	Tool::Factory& factory = Tool::Factory::instance();
	int count = factory.size();
	for(int i = 0; i < count; ++i){
		Tool* tool = factory.createByIndex(i);
		const char* name = typeid(*tool).name();
		tools_[name] = tool;
	}
}

ToolManager& ToolManager::instance()
{
	static ToolManager toolManager;
	return toolManager;
}

Tool* ToolManager::get(const char* typeName)
{
	Tools::iterator it = tools_.find(typeName);
	if(it != tools_.end())
		return it->second();
	else
		return 0;
}


// ---------------------------------------------------------------------------

TransformOptions::TransformOptions()
: space_(SPACE_VIEW)
, pose_(Se3f::ID)
, active_(false)
{	
}

void TransformOptions::setActive(bool active)
{
	active_ = active;
}

// ---------------------------------------------------------------------------
Tool::Tool()
: mouseViewport_(0)
{
}

Sandbox* Tool::sandbox()
{
	if(Model* document = Model::get())
		return document->sandbox();
	else
		return 0;
}

bool Tool::onMouseButtonDown(ToolViewport* viewport, MouseButton button )
{
	if(button != MOUSE_BUTTON_WHEEL_DOWN && button != MOUSE_BUTTON_WHEEL_UP)
		viewport->captureMouse();
	mouseViewport_ = viewport;
	return false;
}
bool Tool::onMouseButtonUp(ToolViewport* viewport, MouseButton button)
{
	if(button != MOUSE_BUTTON_WHEEL_DOWN && button != MOUSE_BUTTON_WHEEL_UP)
		viewport->releaseMouse();
	mouseViewport_ = viewport;
	return false;
}

bool Tool::onMouseMove(ToolViewport* viewport)
{
	mouseViewport_ = viewport;
	return false;
}

void Tool::onSelectionChanged()
{

}

void Tool::onActiveChanged()
{

}

// ---------------------------------------------------------------------------
ToolNavigator::ToolNavigator()
: rotating_(false)
, translating_(false)
, zooming_(false)
{

}

bool ToolNavigator::onMouseButtonDown(ToolViewport* viewport, MouseButton button)
{
	if(button == MOUSE_BUTTON_WHEEL_DOWN){
		transformCamera(viewport, 0.0f, 0.0f, 0.0f, 1.25f);
	}
	else if(button == MOUSE_BUTTON_WHEEL_UP){
		transformCamera(viewport, 0.0f, 0.0f, 0.0f, 0.8f);
	}
	else if(button == kdw::MOUSE_BUTTON_LEFT){
		rotating_ = true;
	}
	else if(button == kdw::MOUSE_BUTTON_MIDDLE){
		translating_ = true;
	}
	else if(button == kdw::MOUSE_BUTTON_RIGHT){
		zooming_ = true;
	}


	if(rotating_ || translating_ || zooming_)
		viewport->captureMouse();
	return true;
}

bool ToolNavigator::onMouseButtonUp(ToolViewport* viewport, MouseButton button)
{
	if(button == kdw::MOUSE_BUTTON_LEFT){
		rotating_ = false;
	}
	else if(button == kdw::MOUSE_BUTTON_MIDDLE){
		translating_ = false;
	}
	else if(button == kdw::MOUSE_BUTTON_RIGHT){
		zooming_ = false;
	}

	if(!rotating_ && !translating_ && !zooming_)
		viewport->releaseMouse();
	return true;
}

bool ToolNavigator::onMouseMove(ToolViewport* viewport)
{
	float cameraDistance = viewport->cameraDistance();
	Vect2i delta = viewport->mousePosition() - viewport->lastMousePosition();
	float pixelSize = 1.0f / max(1.0f, min(viewport->size().x * 0.5f, viewport->size().y * 0.5f));
	if(rotating_){
		float deltaPsi = delta.x * M_PI * pixelSize * 0.5f;
		float deltaTheta = delta.y * M_PI * pixelSize * 0.5f;
		transformCamera(viewport, deltaPsi, deltaTheta, 0.0f, 1.0f);
	}
	if(translating_){

		Se3f cameraPose = viewport->cameraPose();
		
		Vect3f translationDelta = 0.5f * delta.y * viewport->camera()->GetWorldJ() * pixelSize * cameraDistance
			                    - 0.5f * delta.x * viewport->camera()->GetWorldI() * pixelSize * cameraDistance;
		cameraPose.trans() += translationDelta;

		viewport->setCameraPose(cameraPose);

		transformCamera(viewport, 0.0f, 0.0f, 0.0f, 1.0f);
	}
	if(zooming_){
		transformCamera(viewport, 0.0f, 0.0f, 0.0f, 1.0f + -pixelSize * delta.y * 0.5f);
	}
	return true;
}

void ToolNavigator::transformCamera(ToolViewport* viewport, float deltaPsi, float deltaTheta, float deltaRoll, float dscale)
{	
	Se3f cameraPose = viewport->cameraPose();
	float cameraDistance = viewport->cameraDistance();

	Vect3f cameraDirection(0.0f, 0.0f, cameraDistance);
	cameraPose.rot().xform(cameraDirection);
	Vect3f cameraTarget = cameraPose.trans() + cameraDirection;
	QuatF rotation;
	rotation.mult(cameraPose.rot(), QuatF(deltaPsi, Vect3f(0.0f, 1.0f, 0.0f)));
	rotation.mult(rotation, QuatF(deltaTheta, Vect3f(1.0f, 0.0f, 0.0f)));
	cameraPose.rot().mult(rotation, QuatF(deltaRoll, Vect3f(0.0f, 0.0f, 1.0f)));
	cameraPose.rot().normalize();
	
	Vect3f rotationOffset(0.0f, 0.0f, cameraDistance * dscale);
	cameraPose.rot().xform(rotationOffset);
	cameraPose.trans() = cameraTarget - rotationOffset;
	cameraDistance *= dscale;

	viewport->setCameraPose(cameraPose);
	viewport->setCameraDistance(cameraDistance);

}



// ---------------------------------------------------------------------------

ToolSelect::ToolSelect()
: selectStartPoint_(Vect2i::ZERO)
, drag_(false)
{

}

void ToolSelect::render(ToolViewport* viewport, SandboxRenderer* renderer)
{
	if(drag_ && viewport == mouseViewport()){
		Vect2i start = selectStartPoint_;
		Vect2i end = viewport->mousePosition();
		renderer->drawRect(start, end, Color4c(255, 255, 0, 255));
	}
	//renderer->drawLine();
}

bool ToolSelect::onMouseButtonDown(ToolViewport* viewport, MouseButton button)
{
	Sandbox* sandbox = viewport->sandbox();
	if(button == MOUSE_BUTTON_LEFT){
		selectStartPoint_ = viewport->mousePosition();
		drag_ = true;
	}
	return __super::onMouseButtonDown(viewport, button);
}
Vect2f screenToView(ToolViewport* viewport, Vect2i screen)
{
	return Vect2f(float(screen.x) / viewport->size().x, float(screen.y) / viewport->size().y) - Vect2f(0.5f, 0.5f);
}


struct NearestToyFinder : ToyVisitor{
	NearestToyFinder(Vect3f point, Toy*& result, Vect3f rayStart, Vect3f rayEnd)
	: result_(result)
	, point_(point)
	, rayStart_(rayStart)
	, rayEnd_(rayEnd)
	, nearestDistance2_(10e9f)
	{}

	bool operator()(Toy& toy){
		sBox6f box;
		if(toy.intersectedWith(rayStart_, rayEnd_)){
			float distance2 = point_.distance2(toy.globalPose().trans());
			if(distance2 < nearestDistance2_){
				nearestDistance2_ = distance2;
				result_ = &toy;
			}
		}
		return toy.visit(*this);
	}

	Vect3f point_;
	Vect3f rayStart_;
	Vect3f rayEnd_;
	Toy*& result_;
	float nearestDistance2_;
};


struct PyramidSelector : ToyVisitor{
	PyramidSelector(const SelectionPyramid& pyramid, Sandbox* sandbox)
	: pyramid_(pyramid)
	, sandbox_(sandbox)
	{}
	bool operator()(Toy& toy){
		if(toy.intersectedWith(pyramid_))
			sandbox_->select(&toy);
		return toy.visit(*this);
	}

	const SelectionPyramid& pyramid_;
	Sandbox* sandbox_;
};

bool ToolSelect::onMouseButtonUp(ToolViewport* viewport, MouseButton button)
{
	drag_ = false;
	Sandbox* sandbox = viewport->sandbox();
	Vect2i mousePosition = viewport->mousePosition();
	if(selectStartPoint_ == mousePosition){ // click
		Vect3f pos;
		Vect3f dir;
		viewport->camera()->GetWorldRay(screenToView(viewport, mousePosition), pos, dir);
		Vect3f end = pos + dir * 1e6f;

		sandbox->deselectAll();

		Toy* nearestToy = 0;
		NearestToyFinder finder(viewport->cameraPose().trans(), nearestToy, pos, end);
		sandbox->root()->visit(finder);
		if(nearestToy != 0)
			sandbox->select(nearestToy);
		else
			sandbox->deselectAll();
		sandbox->onSelectionChanged();
		sandbox->onActiveChanged();
	}
	else{
		SelectionPyramid pyramid;
		pyramid.rectangle = Rectf(Vect2f(selectStartPoint_), Vect2f(mousePosition - selectStartPoint_));
		pyramid.rectangle.validate();
		pyramid.worldToView = viewport->camera()->matViewProjScr;

		int minX = min(mousePosition.x, selectStartPoint_.x);
		int maxX = max(mousePosition.x, selectStartPoint_.x);
		int minY = min(mousePosition.y, selectStartPoint_.y);
		int maxY = max(mousePosition.y, selectStartPoint_.y);

		Vect2i screenPoints[] = {
			Vect2i(minX, minY),
			Vect2i(minX, maxY),
			Vect2i(maxX, maxY),
			Vect2i(maxX, minY)
		};
		for(int i = 0; i < 4; ++i){
			Vect3f pos;
			Vect3f dir;
			viewport->camera()->GetWorldRay(screenToView(viewport, screenPoints[i]), pos, dir);
			pyramid.nearPlane[i] = pos;
			pyramid.farPlane[i] = pos + dir * 1e3f;
		}



		sandbox->deselectAll();
		sandbox->root()->visit(PyramidSelector(pyramid, sandbox));
		sandbox->onSelectionChanged();
		sandbox->onActiveChanged();
	}
	return __super::onMouseButtonUp(viewport, button);
}

bool ToolSelect::onMouseMove(ToolViewport* viewport, MouseButton button)
{
	return false;
}




// ---------------------------------------------------------------------------

ToolTransform::ToolTransform()
{

}

// ---------------------------------------------------------------------------
ToolTranslate::ToolTranslate()
: grabbedAxis_(-1)
, translation_(Vect3f::ZERO)
, tempTranslation_(Vect3f::ZERO)
, transformAxis_(Vect3f::K)
, grabbed_(false)
{
}


struct CenterObtainer : ToyVisitor{
	CenterObtainer(bool selectedOnly = false)
	: center_(Vect3f::ZERO)
	, count_(0)
	, selectedOnly_(selectedOnly)
	{
	}
	bool operator()(Toy& toy){
		if(!selectedOnly_ || toy.selected()){
			center_ += toy.globalPose().trans();
			++count_;
		}
		toy.visit(*this);
		return true;
	}
	Vect3f center() const{
		if(count_)
			return center_ * 1.0f / count_;
		else
			return Vect3f::ZERO;
	}
	int count() const{ return count_; }

	bool selectedOnly_;
	int count_;
	Vect3f center_;
};

void ToolTranslate::updateTransformOptions()
{
	transformOptions().setActive(false);
	CenterObtainer obtainer(true);
	sandbox()->root()->visit(obtainer);
	if(obtainer.count() > 0)
		transformOptions().setActive(true);
	transformOptions_.setPose(Se3f(transformOptions_.pose().rot(), obtainer.center()));
}

float ToolTranslate::widgetSize(ToolViewport* viewport)
{
	return 10.0f;
}

bool linePeek(const Vect2f& start, const Vect2f& end, const Vect2f& point, float distance)
{
	Vect2f direction = end - start;
	float len = direction.norm();
	if(len > FLT_EPS){
		direction *= 1.0f / len;
		
		float t = direction.dot(point - start);
		if(t < -distance || t > len + distance)
			return false;

		Vect2f normal(-direction.y, direction.x);
		t = normal.dot(point - start);
		if(t > distance || t < -distance)
			return false;
	}
	else{
		if(point.x < start.x - distance || point.x > start.x + distance)
			return false;
		if(point.y < start.y - distance || point.y > start.y + distance)
			return false;
	}
	return true;
}

bool ToolTranslate::axisHovered(ToolViewport* viewport, int index)
{
	xassert(index >= 0 && index < 3);
	if(grabbed_){
		if(grabbedAxis_ != -1)
			return lockAxis_ ? index != grabbedAxis_ : index == grabbedAxis_;
		else
			return true;
	}

	Se3f pose = transformOptions().pose();

	Vect3f bases[3] = { Vect3f::I, Vect3f::J, Vect3f::K };
	Vect3f axisStart = Vect3f::ZERO;
	pose.xformPoint(axisStart);
	viewport->camera()->matViewProjScr.xformCoord(axisStart);
	for(int i = 0; i < 3; ++i){
		bases[i] *= widgetSize(viewport);
		pose.xformPoint(bases[i]);
		viewport->camera()->matViewProjScr.xformCoord(bases[i]);
	}

	for(int i = 0; i < 3; ++i)
		if(linePeek(axisStart, bases[i], Vect2f(viewport->mousePosition()), 2.0f))
			return i == index;
	for(int i = 0; i < 3; ++i)
		if(linePeek(axisStart, bases[i], Vect2f(viewport->mousePosition()), 15.0f))
			return i == index;
	return false;
}

bool ToolTranslate::onMouseButtonDown(ToolViewport* viewport, MouseButton button)
{
	grabbedAxis_ = -1;
	Vect3f axices[3] = { Vect3f::I, Vect3f::J, Vect3f::K };
	for(int i = 0; i < 3; ++i){
		if(axisHovered(viewport, i)){
			grabbedAxis_ = i;
			transformAxis_ = axices[i];
			lockAxis_ = button != MOUSE_BUTTON_LEFT;
			break;
		}
	}
	if(grabbedAxis_ == -1){
		transformAxis_ = viewport->camera()->GetWorldK();
		lockAxis_ = true;
	}
	grabbed_ = true;
	justStarted_ = true;
	return __super::onMouseButtonDown(viewport, button);
}

void ToolTranslate::applyTranslation(ToolViewport* viewport)
{
	Toys selection = sandbox()->selection();
	Toys::iterator it;
	FOR_EACH(selection, it){
		Toy* toy = *it;
		Se3f pose = toy->globalPose();
		pose.trans() += translation_ - tempTranslation_;
		toy->setGlobalPose(pose);
		if(!translation_.eq(Vect3f::ZERO))
			toy->signalMoved().emit(toy);
	}
	
	translation_ = Vect3f::ZERO;
	tempTranslation_ = Vect3f::ZERO;
	updateTransformOptions();
}

void ToolTranslate::previewTranslation(ToolViewport* viewport)
{
	Toys selection = sandbox()->selection();
	Toys::iterator it;
	FOR_EACH(selection, it){
		Toy* toy = *it;
		Se3f pose = toy->globalPose();
		pose.trans() += translation_ - tempTranslation_;
		if(justStarted_)
			toy->signalBeforeMove().emit(toy);
		toy->setGlobalPose(pose);
	}
	tempTranslation_ = translation_;
	justStarted_ = false;
}

bool ToolTranslate::onMouseButtonUp(ToolViewport* viewport, MouseButton button)
{
	applyTranslation(viewport);
	grabbed_ = false;
	return __super::onMouseButtonUp(viewport, button);
}

Vect3f ToolTranslate::transformPlane(ToolViewport* viewport)
{
	if(!lockAxis_){
		Vect3f cameraDirection = viewport->camera()->GetWorldK();
		Vect3f axis = transformAxis_;
		Vect3f perp = cameraDirection.postcross(axis);
		Vect3f normal = axis.postcross(perp);
		normal.normalize();
		return normal;
	}
	else
		return transformAxis_;
}

bool intersectLineWithPlane(Vect3f& result, const Vect3f& start, const Vect3f& end, const Vect3f& normal)
{
	const float epsilon = 1e-6f;
	Vect3f pn = end - start;

	float n_pn = dot(normal, pn);
	if (n_pn >- epsilon && n_pn < epsilon)
		return false;
	float t = dot(normal, (-start)) / n_pn;
	if(t < 0.0f || t > 1.0f)
		return false;

	result = pn * t + start;
	return true;
}


Vect3f ToolTranslate::restrictTransform(Vect3f delta)
{
	if(lockAxis_)
		return delta - transformAxis_ * transformAxis_.dot(delta);
	else
		return transformAxis_ * transformAxis_.dot(delta);

}


bool ToolTranslate::onMouseMove(ToolViewport* viewport)
{
	if(grabbed_){
		Vect3f normal = transformPlane(viewport);

		Vect3f point, lastPoint;

		Vect3f rayStart;
		Vect3f rayDirection;
		viewport->camera()->GetWorldRay(screenToView(viewport, viewport->mousePosition()), rayStart, rayDirection);
		
		Vect3f center = transformOptions().pose().trans();

	   	if(!intersectLineWithPlane(point, rayStart - center,
								   rayStart + rayDirection * 1e14f - center, normal))
			return __super::onMouseMove(viewport);
		viewport->camera()->GetWorldRay(screenToView(viewport, viewport->lastMousePosition()), rayStart, rayDirection);
	   	if(!intersectLineWithPlane(lastPoint, rayStart - center,
								   rayStart + rayDirection * 1e14f - center, normal))
			return __super::onMouseMove(viewport);

		Vect3f delta = point - lastPoint;
		translation_ += restrictTransform(delta);
		previewTranslation(viewport);
	}
	return __super::onMouseMove(viewport);
}

void ToolTranslate::onSelectionChanged()
{
	updateTransformOptions();
}

void ToolTranslate::onSelect()
{
	onSelectionChanged();
}

void ToolTranslate::render(ToolViewport* viewport, SandboxRenderer* renderer)
{
	if(!transformOptions().active())
		return;

	float size = widgetSize(viewport);

	Vect3f selectionCenter(transformOptions().pose().trans());
	
	Se3f pose = transformOptions().pose();
	Vect3f widgetX(Vect3f::I);
	Vect3f widgetY(Vect3f::J);
	Vect3f widgetZ(Vect3f::K);
	pose.xformVect(widgetX);
	pose.xformVect(widgetY);
	pose.xformVect(widgetZ);

	unsigned char oldAlpha = 50;
	unsigned char alpha = 100;
	unsigned char hoveredAlpha = 255;

	if(translation_ != Vect3f::ZERO){
		renderer->drawLine(selectionCenter, selectionCenter + widgetX * size, Color4c(255, 0, 0, oldAlpha));
		renderer->drawLine(selectionCenter, selectionCenter + widgetY * size, Color4c(0, 255, 0, oldAlpha));
		renderer->drawLine(selectionCenter, selectionCenter + widgetZ * size, Color4c(0, 0, 255, oldAlpha));
	}
                                                                                                                      
	renderer->drawLine(translation_ + selectionCenter, selectionCenter + translation_ + widgetX * size, Color4c(255, 0, 0, axisHovered(viewport, 0)?hoveredAlpha:alpha));
	renderer->drawLine(translation_ + selectionCenter, selectionCenter + translation_ + widgetY * size, Color4c(0, 255, 0, axisHovered(viewport, 1)?hoveredAlpha:alpha));
	renderer->drawLine(translation_ + selectionCenter, selectionCenter + translation_ + widgetZ * size, Color4c(0, 0, 255, axisHovered(viewport, 2)?hoveredAlpha:alpha));
}

// ---------------------------------------------------------------------------

REGISTER_CLASS(Tool, ToolNavigator, "Navigator");
REGISTER_CLASS(Tool, ToolSelect, "Select");
REGISTER_CLASS(Tool, ToolTranslate, "Translate");

// ---------------------------------------------------------------------------
ToolViewport::ToolViewport(cInterfaceRenderDevice* renderDevice, Model* document, bool continuousUpdate)
: kdw::Viewport(renderDevice, continuousUpdate)
, document_(document)
, cameraPose_(QuatF::ID, Vect3f(0.0f, 100.0f, 0.0f))
, cameraDistance_(100.0f)

, camera_(0)
, scene_(0)
, sandbox_(0)

, font_(0)
{
}

ToolViewport::~ToolViewport()
{
	delete font_;
    RELEASE(camera_);
}

void ToolViewport::setSandbox(Sandbox* sandbox)
{
	sandbox_ = sandbox;
}



Tool* ToolViewport::currentTool()
{
	return document()->currentTool();
}


void ToolViewport::onInitialize()
{
	font_ = FT::fontManager().createFont("Scripts\\Resource\\fonts\\default.ttf", 12);
	updateCamera();
}


void ToolViewport::onRedraw()
{
	xassert(currentTool());
	xassert(document());
	xassert(document()->sandboxRenderer());
	Tool* tool = currentTool();
	SandboxRenderer* renderer = document()->sandboxRenderer();
	tool->render(this, renderer);

	camera_->SetFrustum(&Vect2f(0.5f,0.5f),&sRectangle4f(-0.5f,-0.5f,0.5f,0.5f), &Vect2f(1.f,1.f), &Vect2f(10.0f,10000.0f));
	const int offset = 10;
	const int height = 20;

	int index = 1;
	renderDevice()->SetFont(font_);
	const char* text = typeid(*tool).name();
	renderDevice()->OutText(offset, size().y - offset - height * index, text, Color4f(1.0f, 1.0f, 0, 0.5f));
	++index;
	scene_->Draw(camera_);
	//scene_->SetDeltaTime(0.0f);
	sandbox_->root()->render(renderer, sandbox_->activeToy());
}

void ToolViewport::setScene(cScene* scene)
{
	scene_ = scene;
}

void ToolViewport::setCamera(Camera* camera)
{
	camera_ = camera;
	resetCameraPosition();
}

void ToolViewport::resetCameraPosition()
{
	cameraPose_.rot().set(0.0f, 1.0f, 0.0f, 0.0f);
	cameraPose_.trans().set(1024.0f, 1024.0f, 100.0f);

	updateCamera();
}

void ToolViewport::setCameraPose(const Se3f& pose)
{
	cameraPose_ = pose;
	updateCamera();
}

void ToolViewport::setCameraDistance(float distance)
{
	cameraDistance_ = distance;
	updateCamera();
}

void ToolViewport::updateCamera()
{
	if(renderWindow()){
		renderDevice()->selectRenderWindow(renderWindow());
		MatXf m(cameraPose_);
		m.Invert();
		camera_->SetPosition(m);
		camera_->SetFrustum(&Vect2f(0.5f,0.5f),&sRectangle4f(-0.5f,-0.5f,0.5f,0.5f), &Vect2f(1.f,1.f), &Vect2f(10.0f,3000.0f));
		renderDevice()->selectRenderWindow(0);
	}
}

void ToolViewport::onMouseMove()
{
	currentTool()->onMouseMove(this);
	lastMousePosition_.x = mousePosition().x;
	lastMousePosition_.y = mousePosition().y;
}

void ToolViewport::onMouseButtonDown(MouseButton button)
{
	currentTool()->onMouseButtonDown(this, button);
	if(HotkeyContext* hotkeyContext = findHotkeyContext(this)){

	}
}

void ToolViewport::onMouseButtonUp(MouseButton button)
{
	currentTool()->onMouseButtonUp(this, button);
}


void ToolViewport::focusSelected()
{
	CenterObtainer obtainer(true);
	sandbox()->root()->visit(obtainer);

	Vect3f direction = Vect3f::K * cameraDistance_;
	cameraPose_.rot().xform(direction);
	cameraPose_.trans() = obtainer.center() - direction;
	updateCamera();
}

void ToolViewport::focusAll()
{
	CenterObtainer obtainer(false);
	sandbox()->root()->visit(obtainer);

	Vect3f direction = Vect3f::K * cameraDistance_;
	cameraPose_.rot().xform(direction);
	cameraPose_.trans() = obtainer.center() - direction;
	updateCamera();
}

void ToolViewport::serialize(Archive& ar)
{
	if(ar.filter(SERIALIZE_STATE)){
		ar.serialize(cameraPose_, "cameraPose", 0);
		ar.serialize(cameraDistance_, "cameraDistance", 0);

		if(ar.isInput())
			updateCamera();
	}
	__super::serialize(ar);
}

}

#endif // KDW_DLL
