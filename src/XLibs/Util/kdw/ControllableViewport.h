#ifndef __KDW_CONTROLLABLE_VIEWPORT__
#define __KDW_CONTROLLABLE_VIEWPORT__

class cScene;
class Camera;

#include "kdw/Viewport.h"
#include "kdw/Tool.h"

namespace FT{ class Font; }

namespace kdw{

enum CoordsMode{
	COORDS_SCREEN,
	COORDS_WORLD,
	COORDS_LOCAL
};

class Sandbox;
class Model;
class ToolViewport : public Viewport{
public:
	explicit ToolViewport(cInterfaceRenderDevice* renderDevice, Model* document, bool continuousView = false);
	~ToolViewport();

	void onRedraw();
	void onInitialize();

	void onMouseButtonDown(MouseButton button);
	void onMouseButtonUp(MouseButton button);
	void onMouseMove();

	void serialize(Archive& ar);
	
	void setScene(cScene* scene);
	cScene* scene() { return scene_; }
	void setCamera(Camera* camera);
	Camera* camera() { return camera_; }

	void focusAll();
	void focusSelected();

	void setCameraPose(const Se3f& pose);
	const Se3f& cameraPose() const{ return cameraPose_; }

	float cameraDistance() const{ return cameraDistance_; }
	void setCameraDistance(float distance);

	Tool* currentTool();

	Vect2i lastMousePosition() const{ return lastMousePosition_; }

	Sandbox* sandbox(){ return sandbox_; }
	void setSandbox(Sandbox* sandbox);
	Model* document(){ return document_; }
protected:
	void updateCamera();
	void resetCameraPosition();

	Se3f cameraPose_;
	float cameraDistance_;

	bool rotating_;
	bool translating_;
	bool zooming_;

	Camera* camera_;
	cScene* scene_;
	FT::Font* font_;
	Vect2i lastMousePosition_;	

	kdw::Model* document_;
	Sandbox* sandbox_;
};

}

#endif
