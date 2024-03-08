#ifndef __KDW_TOOL_H_INCLUDED__
#define __KDW_TOOL_H_INCLUDED__

#include "kdw/Viewport.h" // enum MouseButton
#include "XTL/StaticMap.h"
#include "XTL/StaticString.h"

struct sBox6f;

template<class BaseType>
class FactoryArg0;

template<class BaseType, class FactoryArg>
class SerializationFactory;

namespace kdw{

class ToolViewport;
class Sandbox;
class SandboxRenderer;

enum TransformSpace{
	SPACE_LOCAL,
	SPACE_WORLD,
	SPACE_VIEW
};

class TransformOptions{
public:
	TransformOptions();

	TransformSpace space(){ return space_; }
	void setPose(const Se3f& pose){ pose_ = pose; }
	const Se3f& pose() const{ return pose_; }
	void setActive(bool active);
	bool active() const{ return active_; }
protected:
	TransformSpace space_;
	bool activeAxices_[3];
	bool active_;
	Se3f pose_;
};

class Tool : public ShareHandleBase{
public:
	typedef SerializationFactory<Tool, FactoryArg0<Tool> > Factory;

	Tool();
	virtual ~Tool() {}
	virtual bool onMouseButtonDown(ToolViewport* viewport, MouseButton button );
	virtual bool onMouseButtonUp(ToolViewport* viewport, MouseButton button);
	virtual bool onMouseMove(ToolViewport* viewport);

	virtual void onSelect() {}
	virtual void onDeselect() {}
	virtual void onSelectionChanged();
	virtual void onActiveChanged();

	virtual void render(ToolViewport* viewport, SandboxRenderer* renderer){}
	Sandbox* sandbox();

	ToolViewport* mouseViewport(){ return mouseViewport_; }
	TransformOptions& transformOptions(){ return transformOptions_; }
protected:
	ToolViewport* mouseViewport_;

	TransformOptions transformOptions_;
};

class ToolManager{
public:
	typedef StaticMap<StaticString, ShareHandle<Tool> > Tools;

	ToolManager();

	Tool* get(const char* typeName);

	static ToolManager& instance();
protected:
	Tools tools_;
};

class ToolNavigator : public Tool{
public:
	ToolNavigator();

	bool onMouseButtonDown(ToolViewport* viewport, MouseButton button);
	bool onMouseButtonUp(ToolViewport* viewport, MouseButton button);
	bool onMouseMove(ToolViewport* viewport);

protected:
	void transformCamera(ToolViewport* viewport, float deltaPsi, float deltaTheta, float deltaRoll, float deltaScale);

	bool rotating_;
	bool translating_;
	bool zooming_;
};

class ToolSelect : public Tool{
public:
	ToolSelect();

	bool onMouseButtonDown(ToolViewport* viewport, MouseButton button);
	bool onMouseButtonUp(ToolViewport* viewport, MouseButton button);
	bool onMouseMove(ToolViewport* viewport, MouseButton button);
    
	void render(ToolViewport* viewport, SandboxRenderer* renderer);
protected:
	Vect2i selectStartPoint_;
	bool drag_;
};

class ToolTransform : public Tool{
public:
	ToolTransform();
protected:

};


class ToolTranslate : public ToolTransform, public sigslot::has_slots{
public:
	ToolTranslate();

	void render(ToolViewport* viewport, SandboxRenderer* renderer);
	void updateTransformOptions();

	void applyTranslation(ToolViewport* viewport);
	void previewTranslation(ToolViewport* viewport);
	bool onMouseButtonDown(ToolViewport* viewport, MouseButton button);
	bool onMouseButtonUp(ToolViewport* viewport, MouseButton button);
	bool onMouseMove(ToolViewport* viewport);
	void onSelectionChanged();
	void onSelect();

	// widget
	bool axisHovered(ToolViewport* viewport, int index);
	Vect3f transformPlane(ToolViewport* viewport);
	Vect3f restrictTransform(Vect3f delta);
	float widgetSize(ToolViewport* viewport);
protected:
	bool grabbed_;
	bool justStarted_;
	int grabbedAxis_;
	Vect3f translation_;
	Vect3f tempTranslation_;
	bool lockAxis_;
	Vect3f transformAxis_;
};

}

#endif
