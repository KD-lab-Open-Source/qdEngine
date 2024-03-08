#include "StdAfx.h"
#include "Render/Inc/IRenderDevice.h"
#include "kdw/Sandbox.h"
#include "kdw/Document.h"
#include "kdw/Tool.h"

namespace kdw{

SandboxRenderer::SandboxRenderer(Sandbox* sandbox, cInterfaceRenderDevice* renderDevice)
: sandbox_(sandbox)
, renderDevice_(renderDevice)
{
	xassert(renderDevice_);
}

void SandboxRenderer::drawLine(const Vect2i& start, const Vect2i& end, Color4c color)
{
	renderDevice_->DrawLine(start.x, start.y, end.x, end.y, color);
}

void SandboxRenderer::drawRect(const Vect2i& start, const Vect2i& end, Color4c color)
{
    drawLine(start, Vect2i(start.x, end.y), color);
    drawLine(start, Vect2i(end.x, start.y), color);
    drawLine(end, Vect2i(start.x, end.y), color);
    drawLine(end, Vect2i(end.x, start.y), color);
}

void SandboxRenderer::drawLine(const Vect3f& start, const Vect3f& end, Color4c color)
{
	renderDevice_->DrawLine(start, end, color);
}

void SandboxRenderer::drawCircle(const Se3f& pose, const Vect3f& size, Color4c color)
{
	const int segments = 64;
	Vect3f prev = Vect3f(size.x, 0.0f, 0.0f);
	pose.xformPoint(prev);
	for(int i = 1; i <= segments; ++i){
		float angle = float(i) / float(segments) * M_PI * 2.0f;
		Vect3f point(size.x * cosf(angle), size.y * sinf(angle), 0.0f);
		pose.xformPoint(point);
		drawLine(prev, point, color);
		prev = point;
	}
}

void SandboxRenderer::drawBox(const Se3f& pose, const Vect3f& size, Color4c color)
{
	Vect3f points[] = {
		Vect3f(size.x, size.y, -size.z) * 0.5f,
		Vect3f(-size.x, size.y, -size.z) * 0.5f,
		Vect3f(-size.x, -size.y, -size.z) * 0.5f,
		Vect3f(size.x, -size.y, -size.z) * 0.5f,

		Vect3f(size.x, size.y, size.z) * 0.5f,
		Vect3f(-size.x, size.y, size.z) * 0.5f,
		Vect3f(-size.x, -size.y, size.z) * 0.5f,
		Vect3f(size.x, -size.y, size.z) * 0.5f
	};

	for(int i = 0; i < 8; ++i){
		points[i] = pose.trans() + pose.rot().xform(points[i]);
	}

	drawLine(points[0], points[1], color);
	drawLine(points[1], points[2], color);
	drawLine(points[2], points[3], color);
	drawLine(points[3], points[0], color);

	drawLine(points[4], points[5], color);
	drawLine(points[5], points[6], color);
	drawLine(points[6], points[7], color);
	drawLine(points[7], points[4], color);

	drawLine(points[0], points[4], color);
	drawLine(points[1], points[5], color);
	drawLine(points[2], points[6], color);
	drawLine(points[3], points[7], color);
}

// ---------------------------------------------------------------------------

Sandbox::Sandbox()
: activeToy_(0)
, root_(Se3f::ID, Vect3f::ID, TOY_SHAPE_NONE)
{
}

void Sandbox::toyDetached(Toy* toy)
{
	if(activeToy_ == toy){
		activeToy_ = 0;
		onActiveChanged();
	}
}

Toys Sandbox::selection()
{
	Toys selected;
	root_.getSelected(selected);
	return selected;
}

void Sandbox::select(Toy* toy, bool selected)
{
	if(selected != toy->selected())
		toy->setSelected(selected);

	if(selected){
		if(activeToy_ != toy)
			activeToy_ = toy;
	}
	else{
		if(activeToy_ == toy)
			activeToy_ = 0;
	}
}

void Sandbox::deselectAll()
{
	root_.deselectAll();
	activeToy_ = 0;
}

void Sandbox::onActiveChanged()
{
	if(activeToy_)
		activeToy_->onActivate();
	if(Model::get()){
		Tool* tool = Model::get()->currentTool();
		tool->onActiveChanged();
	}
}

void Sandbox::onSelectionChanged()
{
	if(Model::get()){
		Tool* tool = Model::get()->currentTool();
		tool->onSelectionChanged();
	}
}

// ---------------------------------------------------------------------------

ToyLink::ToyLink(Toy* start, Toy* end)
: start_(start)
, end_(end)
{

}
        
void ToyLink::render(SandboxRenderer* renderer)
{
	if(start_ && end_){
		renderer->drawLine(start_->globalPose().trans(), end_->globalPose().trans(), Color4c(200, 200, 200, 150));
	}
}

// ---------------------------------------------------------------------------
Toy::Toy(const Se3f& pose, const Vect3f& size, ToyShape widget)
: pose_(pose)
, globalPose_(pose)
, size_(size)
, selected_(false)
, widget_(widget)
, showWidget_(true)
, parent_(0)
{
}

Toy::~Toy()
{
	while(!children_.empty())
		detach(children_.front());
}

void Toy::detach(Toys& childs)
{
	Toys::iterator it;

	FOR_EACH(childs, it){
		Toy* toy = *it;
		detach(toy);
	}
}

void Toy::detach(Toy* child)
{
	Children::iterator it = std::find(children_.begin(), children_.end(), child);
	xassert(it != children_.end());
	if(it != children_.end()){
		ShareHandle<Toy> ref(child);
		children_.erase(it);
		child->setParent(0);
	}
}

void Toy::setGlobalPose(const Se3f& pose, bool signal)
{
	if(parent()){
		Se3f pos(parent()->pose());
		pos.invert();
		setPose(pose * pos , signal);
	}
	else{
		setPose(pose, signal);
	}
}

void Toy::setPose(const Se3f& pose, bool signal)
{
	pose_ = pose;
	if(parent())
		globalPose_ = parent()->globalPose() * pose;
	else
		globalPose_ = pose_;
	if(signal)
		signalMove_.emit(this);
	Children::iterator it;
	FOR_EACH(children_, it){
		Toy* toy = *it;
		toy->setParentPose(pose_);
	}
}


void Toy::attach(Toys& toys)
{
	Toys::iterator it;
	FOR_EACH(toys, it){
		Toy* toy = *it;
		attach(toy);
	}
}

void Toy::attach(Toy* child)
{
	xassert(!child->attached());
	xassert(std::find(children_.begin(), children_.end(), child) == children_.end());
	children_.push_back(child);
	child->setParent(this);
	child->setParentPose(globalPose_);
}

void Toy::setParentPose(const Se3f& pose)
{
	globalPose_ = pose * pose_;
	Children::iterator it;
	FOR_EACH(children_, it){
		Toy* toy = *it;
		toy->setParentPose(globalPose_);
	}
}

void Toy::setSize(Vect3f size, bool signal)
{
	size_ = size;
	if(signal)
		signalScale_.emit(this);
}

void Toy::getBoundBox(sBox6f& box) const
{
	box.xmin() = -size_.x * 0.5f;
	box.xmax() =  size_.x * 0.5f;
	box.ymin() = -size_.y * 0.5f;
	box.ymax() =  size_.y * 0.5f;
	box.zmin() = -size_.z * 0.5f;
	box.zmax() =  size_.z * 0.5f;
}


bool Toy::intersectedWith(const Vect3f& lineStart, const Vect3f& lineEnd) const
{
	if(widget_ == TOY_SHAPE_NONE)
		return false;
	sBox6f box;
	getBoundBox(box);
	return box.isCrossOrInside(globalPose_, lineStart, lineEnd);
}


bool lineIntersected(const Vect2f& start1, const Vect2f& end1, const Vect2f& start2, const Vect2f& end2)
{
	if(start1.eq(end1) || start2.eq(end2))
		return false;
	Vect2f direction1(end1 - start1);
	Vect2f normal1(-direction1.y, direction1.x);
	normal1.normalize(1.0f);
	
	float x1 = (start2 - start1).dot(normal1);
	float x2 = (end2 - start1).dot(normal1);
	if(x1 > x2)
		std::swap(x1, x2);
    
	if(x1 > 0.0f || x2 < 0.0f)
		return false;

	Vect2f direction2(end2 - start2);
	Vect2f normal2(-direction2.y, direction2.x);
	normal2.normalize(1.0f);
	
	float y1 = (start1 - start2).dot(normal2);
	float y2 = (end1 - start2).dot(normal2);
	if(y1 > y2)
		std::swap(y1, y2);
    
	if(y1 > 0.0f || y2 < 0.0f)
		return false;

	return true;
}

bool SelectionPyramid::includePoints(const Vect3f* points, int numPoints) const
{
	for(int i = 0; i < numPoints; ++i){
		Vect3f screenPoint;
		worldToView.xformCoord(points[i], screenPoint);
		if(rectangle.point_inside(Vect2f(screenPoint)))
			return true;
	}
	return false;
}

bool SelectionPyramid::intersectEdges(const Vect3f* edgePoints, int numEdges) const
{
	Vect2f rectEdges[4] = {
		rectangle.left_top(),
		rectangle.right_top(),
		rectangle.right_bottom(),
		rectangle.left_bottom()
	};

	for(int i = 0; i < numEdges; ++i){
		Vect3f screenStart, screenEnd;
		if(edgePoints[i * 2] != edgePoints[i * 2 + 1]){
			worldToView.xformCoord(edgePoints[i * 2], screenStart);
			worldToView.xformCoord(edgePoints[i * 2 + 1], screenEnd);

			if(lineIntersected(rectEdges[0], rectEdges[1], Vect2f(screenStart), Vect2f(screenEnd)))
				return true;
			if(lineIntersected(rectEdges[1], rectEdges[2], Vect2f(screenStart), Vect2f(screenEnd)))
				return true;
			if(lineIntersected(rectEdges[2], rectEdges[3], Vect2f(screenStart), Vect2f(screenEnd)))
				return true;
			if(lineIntersected(rectEdges[3], rectEdges[0], Vect2f(screenStart), Vect2f(screenEnd)))
				return true;
		}
	}
	return false;
}

bool Toy::intersectedWith(const SelectionPyramid& p) const
{
	switch(widget_){
	case TOY_SHAPE_NONE:
		return false;
	case TOY_SHAPE_BOX:
	{
		Vect3f points[] = {
			Vect3f(-0.5f, -0.5f, -0.5f),
			Vect3f( 0.5f, -0.5f, -0.5f),
			Vect3f( 0.5f,  0.5f, -0.5f),
			Vect3f(-0.5f,  0.5f, -0.5f),

			Vect3f(-0.5f, -0.5f,  0.5f),
			Vect3f( 0.5f, -0.5f,  0.5f),
			Vect3f( 0.5f,  0.5f,  0.5f),
			Vect3f(-0.5f,  0.5f,  0.5f),

			Vect3f(-0.5f, -0.5f,  0.5f),
			Vect3f( 0.5f, -0.5f,  0.5f),
			Vect3f( 0.5f,  0.5f,  0.5f),
			Vect3f(-0.5f,  0.5f,  0.5f),
		};
		for(int i = 0; i < 12; ++i)
			pose_.xformPoint(points[i]);
		if(p.includePoints(points, 12))
			return true;
	
		Vect3f edges[] = {
			Vect3f(-0.5f, -0.5f, -0.5f), Vect3f( 0.5f, -0.5f, -0.5f),
			Vect3f( 0.5f, -0.5f, -0.5f), Vect3f( 0.5f,  0.5f, -0.5f),
			Vect3f( 0.5f,  0.5f, -0.5f), Vect3f(-0.5f,  0.5f, -0.5f),
			Vect3f(-0.5f,  0.5f, -0.5f), Vect3f(-0.5f, -0.5f, -0.5f),

			Vect3f(-0.5f, -0.5f,  0.5f), Vect3f( 0.5f, -0.5f,  0.5f),
			Vect3f( 0.5f, -0.5f,  0.5f), Vect3f( 0.5f,  0.5f,  0.5f),
			Vect3f( 0.5f,  0.5f,  0.5f), Vect3f(-0.5f,  0.5f,  0.5f),
			Vect3f(-0.5f,  0.5f,  0.5f), Vect3f(-0.5f, -0.5f,  0.5f),

			Vect3f(-0.5f, -0.5f,  0.5f), Vect3f(-0.5f, -0.5f, -0.5f),
			Vect3f( 0.5f, -0.5f,  0.5f), Vect3f( 0.5f, -0.5f, -0.5f),
			Vect3f( 0.5f,  0.5f,  0.5f), Vect3f( 0.5f,  0.5f, -0.5f),
			Vect3f(-0.5f,  0.5f,  0.5f), Vect3f(-0.5f,  0.5f, -0.5f),
		};
		for(int i = 0; i < 24; ++i)
			pose_.xformPoint(edges[i]);
		if(p.intersectEdges(edges, 12))
			return true;

	}
	case TOY_SHAPE_CIRCLE_XY:
	{
		const int segments = 64;
		Vect3f points[segments];
		Vect3f edges[segments * 2];
		points[0] = Vect3f(size_.x, 0.0f, 0.0f);
		edges[0] = points[0];
		globalPose_.xformPoint(points[0]);
		for(int i = 1; i < segments; ++i){
			float angle = float(i) / float(segments) * M_PI * 2.0f;
			points[i] = Vect3f(size_.x * cosf(angle), size_.y * sinf(angle), 0.0f);
			globalPose_.xformPoint(points[i]);
			edges[(i - 1) * 2] = points[i - 1];
			edges[(i - 1) * 2 + 1] = points[i];
		}
		if(p.includePoints(points, segments))
			return true;
		if(p.intersectEdges(edges, segments))
			return true;
		}
        break;
	}
	return false;
}

void Toy::setSelected(bool selected)
{
	selected_ = selected;
}

void Toy::deselectAll()
{
	setSelected(false);
	Children::iterator it;
	FOR_EACH(children_, it){
		Toy* child = *it;
		child->deselectAll();
	}
}

void Toy::getSelected(Toys& toys)
{
	if(selected())
		toys.push_back(this);
	Children::iterator it;
	FOR_EACH(children_, it){
		Toy* child = *it;
		child->getSelected(toys);
	}
}

void Toy::onActivate()
{
	ShareHandle<Toy> ref(this);
	signalActivate_.emit(this);
}

bool Toy::visit(ToyVisitor& visitor)
{
	Children::iterator it;
	FOR_EACH(children_, it){
		Toy* child = *it;
		visitor(*child);
	}
	return true;
}


void Toy::setParent(Toy* parent)
{
	parent_ = parent;
}

void Toy::render(SandboxRenderer* renderer, Toy* activeToy)
{	
	bool active = this == activeToy;
	Color4c color;
	
	if(active)
		color = Color4c(255, 255, 255, 255);
	else{
		if(selected_)
			color = Color4c(255, 255, 128, 255);
		else
			color = Color4c(128, 128, 128, 255);
	}

	if(showWidget_){
		switch(widget_){
		case TOY_SHAPE_BOX:
			renderer->drawBox(globalPose_, size_, color);
			break;
		case TOY_SHAPE_CIRCLE_XY:
			renderer->drawCircle(globalPose_, size_, color);
			break;
		case TOY_SHAPE_GRID_XY:
			{
				const int num = 20;
				const float step = 10.0f;
				float mn=-num*step, mx=num*step;

				for(int x = -num; x <= num; x++){
					Vect3f pn,px;
					pn.set(x*step ,mn , 0);
					px.set(pn.x,mx , 0);
					globalPose_.xformPoint(pn);
					globalPose_.xformPoint(px);

					renderer->drawLine(pn, px, Color4c(196,196,196,128));
				}

				for(int y = -num; y <= num; y++){
					Vect3f pn,px;
					pn.set(mn ,y*step,0);
					px.set(mx ,pn.y, 0);

					globalPose_.xformPoint(pn);
					globalPose_.xformPoint(px);
					renderer->drawLine(pn,px,Color4c(196,196,196,128));
				}

				Vect3f pn,px;
				pn.set(0,0, 0);
				px.set(mx ,pn.y, 0);

				globalPose_.xformPoint(pn);
				globalPose_.xformPoint(px);
				renderer->drawLine(pn,px,Color4c(255,0,0,128));

				pn.set(0,0,0);
				px.set(pn.x,mx, 0);

				globalPose_.xformPoint(pn);
				globalPose_.xformPoint(px);
				renderer->drawLine(pn,px,Color4c(0,255,0,128));
			}
			break;
		}
		if(link_)
			link_->render(renderer);
	}

	Children::iterator it;
	FOR_EACH(children_, it){
		Toy* toy = *it;
		toy->render(renderer, activeToy);
	}
}

}
