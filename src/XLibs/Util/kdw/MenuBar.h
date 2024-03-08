#ifndef __MENU_BAR_H_INCLUDED__
#define __MENU_BAR_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"
#include <string>
#include <vector>

namespace kdw{

class MenuBarImpl;
class Command;
class CommandManager;
class KDW_API MenuBar : public _WidgetWithWindow{
public:
	MenuBar(CommandManager* commandManager, const char* path, int border = 0);
	~MenuBar();
	void set(const char* path);

	void _setPosition(const Recti& position);
	void _setParent(Container* container);
	void _updateVisibility();
protected:
	friend class MenuBarImpl;
	MenuBarImpl* impl();
	bool _focusable() const{ return false; }

	std::string path_;
};

}

#endif
