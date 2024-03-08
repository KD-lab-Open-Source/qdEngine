#ifndef __KDW_SANDBOX_H_INCLUDED__
#define __KDW_SANDBOX_H_INCLUDED__

#include "XTL/sigslot.h"
#include "XMath/XMath.h"
#include "XMath/Mat4f.h"
#include "XMath/Box6f.h"
#include "XMath/Colors.h"
#include "XTL\Rect.h"

class cInterfaceRenderDevice;

namespace kdw{
class Sandbox;
class SandboxRenderer {
public:
	SandboxRenderer(Sandbox* sandbox, cInterfaceRenderDevice* renderDevice);

	void drawBox(const Se3f& pose, const Vect3f& size, Color4c color);
	void drawCircle(const Se3f& pose, const Vect3f& size, Color4c color);
	void drawLine(const Vect3f& start, const Vect3f& end, Color4c color);
	void drawLine(const Vect2i& start, const Vect2i& end, Color4c color);
	void drawRect(const Vect2i& start, const Vect2i& end, Color4c color);

protected:
	Sandbox* sandbox_;
	cInterfaceRenderDevice* renderDevice_;
};

enum ToyShape{
	TOY_SHAPE_NONE,
	TOY_SHAPE_BOX,
	TOY_SHAPE_CIRCLE_XY,
	TOY_SHAPE_PLANE_XY,
	TOY_SHAPE_GRID_XY
};

struct SelectionPyramid{
	bool includePoints(const Vect3f* points, int numPoints) const;
    bool intersectEdges(const Vect3f* points, int numEdges) const;

	Rectf rectangle;
	Mat4f worldToView;
	Vect3f nearPlane[4];
	Vect3f farPlane[4];
};

class Toy;
class ToyLink : public ShareHandleBase{
public:
	ToyLink(Toy* start, Toy* end);
        
	void render(SandboxRenderer* renderer);
protected:
	Toy* start_;
	Toy* end_;
};

class Toy;

class ToyVisitor{
public:
	virtual bool operator()(Toy& toy) = 0;
};

typedef std::vector<ShareHandle<Toy> > Toys;
class Toy : public ShareHandleBase, public sigslot::has_slots{
public:
	Toy(const Se3f& pose = Se3f::ID, const Vect3f& size = Vect3f::ID, ToyShape = TOY_SHAPE_BOX);
	virtual ~Toy();

	void attach(Toys& toys);
	void attach(Toy* child);
	bool attached() const{ return parent_ != 0; }
	void detach(Toys& toys);
	void detach(Toy* child);
	virtual void render(SandboxRenderer* renderer, Toy* activeToy);

	void setShowWidget(bool show);
	bool showWidget() const{ return showWidget_; }

	virtual void setPose(const Se3f& pose, bool signal = true);
	virtual void setGlobalPose(const Se3f& pose, bool signal = true);
	virtual void setParentPose(const Se3f& pose);
	const Se3f& pose() const{ return pose_; }
	const Se3f& globalPose() const{ return globalPose_; }

	virtual void setSize(Vect3f size, bool signal = true);
	const Vect3f size() const{ return size_; }

	void getBoundBox(sBox6f& box) const;

	bool intersectedWith(const Vect3f& lineStart, const Vect3f& lineEnd) const;
	bool intersectedWith(const SelectionPyramid& pyramid) const;

	bool selected() const{ return selected_; }
	void setSelected(bool selected);
	void deselectAll();
	void getSelected(Toys& toys);

	void onActivate();
	virtual bool visit(ToyVisitor& visitor);

	sigslot::signal1<Toy*>& signalMove(){ return signalMove_; }
	sigslot::signal1<Toy*>& signalMoved(){ return signalMoved_; }
	sigslot::signal1<Toy*>& signalBeforeMove(){ return signalBeforeMove_; }
	sigslot::signal1<Toy*>& signalScale(){ return signalScale_; }
	sigslot::signal1<Toy*>& signalActivate(){ return signalActivate_; }

	void setLink(ToyLink* link){ link_ = link; }
	Toy* parent() { return parent_; }
	virtual void setParent(Toy* parent);
protected:
	sigslot::signal1<Toy*> signalBeforeMove_;
	sigslot::signal1<Toy*> signalMove_;
	sigslot::signal1<Toy*> signalMoved_;
	sigslot::signal1<Toy*> signalScale_;
	sigslot::signal1<Toy*> signalActivate_;

	Vect3f size_;
	// local
	Se3f pose_;
	// global
	Se3f globalPose_;

	bool showWidget_;
	bool selected_;
	Toy* parent_;
	ToyShape widget_;
	ShareHandle<ToyLink> link_;

	typedef std::vector<ShareHandle<Toy> > Children;
	Children children_;
};


class Sandbox{
public:
	Sandbox();

	void select(Toy* toy, bool selected = true);

	Toy* activeToy(){ return activeToy_; }

	void deselectAll();
	void onSelectionChanged();
	void onActiveChanged();

	void toyDetached(Toy* toy);

	Toys selection();
	Toy* root(){ return &root_; }
protected:
	sigslot::signal0 signalActiveChanged_;

	Toy root_;
	Toy* activeToy_;
};

}
#endif
