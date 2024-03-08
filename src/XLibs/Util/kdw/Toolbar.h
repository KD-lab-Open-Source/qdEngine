#ifndef __KDW_TOOLBAR_H_INCLUDED__
#define __KDW_TOOLBAR_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"

namespace kdw{

class ImageStore;
class CommandManager;
class CommandInstance;
class ToolbarImpl;
class KDW_API Toolbar : public _WidgetWithWindow{
public:
	Toolbar(CommandManager* commands = 0, int border = 0);
	void setImageStore(ImageStore* imageStore);

	void addButton(const char* commandName, int imageIndex);
	void addSeparator();
protected:
	void setCommands(CommandManager* commandManager);
	void updateMinimalSize();
	void onCommandInstanceStateChanged(CommandInstance* instance);
	void updateItems();

	ToolbarImpl& impl();

	bool separateNext_;
};

}

#endif
