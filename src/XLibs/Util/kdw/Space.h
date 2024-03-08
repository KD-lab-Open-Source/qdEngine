#ifndef __KDW_SPACE_H_INCLUDED__
#define __KDW_SPACE_H_INCLUDED__

#include "Serialization/Factory.h"
#include <string>
#include "XTL/sigslot.h"

#include "kdw/MenuBar.h"
#include "kdw/VBox.h"
#include "kdw/SpaceHeader.h"

template<class BaseType, class FactoryArg>
class SerializationFactory;

namespace kdw{

class Workspace;
class Widget;
class CommandManager;
class ScrolledWindow;
class HBox;
class Space : public ShareHandleBase, public sigslot::has_slots{
public:
	typedef SerializationFactory<Space, FactoryArg0<Space> > Factory;
	Space();
	virtual ~Space();

	virtual void add(Widget* widget);
	virtual Widget* widget();

	void addToHeader(Widget* widget);

	void setMenu(const char* menuPath);
	virtual Space* spaceByPoint(Vect2i point);
	virtual bool selfSplit(Vect2i screenPoint) { return false; }
	Space* clone();
	virtual void replaceSpace(Space* oldSpace, Space* newSpace);
	virtual bool isLeaf() const{ return true; }

	virtual void serialize(Archive& ar);
	
	void setPosition(float position){ position_ = position; }
	float position() const{ return position_; }

	Widget*	findWidget();
	Workspace* findWorkspace();

	Space* parent() { return parent_; }
	virtual void setParent(Space* space);

	CommandManager& commands() { return *commandManager_; };
protected:
	void rebuildWidgets();

	ShareHandle<VBox> vbox_;
	ShareHandle<SpaceHeader> header_;
	ShareHandle<MenuBar> menuBar_;
	ShareHandle<Widget> widget_;
	HBox* headerBox_;
	ScrolledWindow* scrolledWindow_;

	typedef std::vector<ShareHandle<Widget> > Widgets;
	Widgets headerWidgets_;

	CommandManager* commandManager_;
	std::string menuPath_;

	Space* parent_;

	Space(float position);
	float position_;
};

typedef ::Factory<std::string, Space> SpaceFactory; // =)

}

#endif
