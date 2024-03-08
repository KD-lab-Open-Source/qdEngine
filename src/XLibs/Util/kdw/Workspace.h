#ifndef __KDW_WORKSPACE_H_INCLUDED__
#define __KDW_WORKSPACE_H_INCLUDED__

#include <vector>

#include "kdw/Space.h"
#include "kdw/_WidgetWithWindow.h"
#include "kdw/HSplitter.h"
#include "kdw/VSplitter.h"

class Archive;

namespace kdw{

class Workspace;
class RootSpace : public Space{
public:
	explicit RootSpace(Workspace* workspace)
	: Space()
	, workspace_(workspace)
	{
	}

	void serialize(Archive& ar);
	void replaceSpace(Space* oldSpace, Space* newSpace);

	void reparent();
	void setSpace(Space* space);
	Widget* widget();

	Space* spaceByPoint(Vect2i screenPoint);
	Workspace* workspace() { return workspace_; }
protected:
	ShareHandle<Space> child_;
	Workspace* workspace_;
};

class SpaceBox : public Space{
public:
	bool isLeaf() const{ return false; }
	void serialize(Archive& ar);

	Space* spaceByPoint(Vect2i screenPoint);
	void replaceSpace(Space* oldSpace, Space* newSpace);
	void removeSpace(int index);
	kdw::Widget* widget() { return splitter_; }
	bool selfSplit(Vect2i point);
	virtual bool vertical() const = 0;
	void setParent(Space* parent);
protected:
	typedef std::vector<ShareHandle<Space> > Spaces;
	Spaces spaces_;
	ShareHandle<Splitter> splitter_;
};

class SpaceHBox : public SpaceBox{
public:
	SpaceHBox();
	SpaceHBox(Space* oldSpace, Vect2i screenSplitPoint);
	bool vertical() const { return false; }
};


class SpaceVBox : public SpaceBox{
public:
	SpaceVBox();
	SpaceVBox(Space* oldSpace, Vect2i screenSplitPoint);
	bool vertical() const { return true; }
};

class WorkspaceImpl;

class Workspace : public _ContainerWithWindow{
public:
	Workspace(int border = 0);
	~Workspace();

	void serialize(Archive& ar);

	void setSpace(Space* space);
	void setRootWidget(Widget* widget);
	void visitChildren(WidgetVisitor& visitor) const;

	void startSplit();
	
	void setStateFile(const char* fileName);
	void saveState();
	void revertState();
protected:
	friend class WorkspaceImpl;
	WorkspaceImpl* impl();
	void _arrangeChildren();
	void _setParent(Container* container);
	void _setFocus();
	Widget* _nextWidget(Widget* last, FocusDirection direction);

	void loadState(const char* fileName);
	void saveState(const char* fileName);

	std::string stateFile_;

	//ShareHandle<Space> root_;
	RootSpace rootSpace_;
	ShareHandle<Widget> rootWidget_;
};

}

#endif
