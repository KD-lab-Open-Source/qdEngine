#include "StdAfx.h"

#include "kdw/PropertyRow.h"
#include "kdw/PropertyTree.h"
#include "kdw/PropertyTreeModel.h"

#include "kdw/HotkeyDialog.h"
#include "kdw/PopupMenu.h"
#include "kdw/Serialization.h"

#include "Serialization/SerializationFactory.h"

#include "sKey.h"
#include "kdw/Win32/Drawing.h"
#include "kdw/Win32/Window.h"

namespace kdw{

class PropertyRowHotkey : public PropertyRowImpl<sKey, PropertyRowHotkey>, public sigslot::has_slots{
public:
	PropertyRowHotkey(void* object, int size, const char* name, const char* nameAlt, const char* typeName);
	PropertyRowHotkey()
	{}
	bool onActivate(PropertyTree* tree);
	bool onActivateWidget(PropertyTree* tree, PropertyRow* hostRow);
	bool onContextMenu(PopupMenuItem& root, PropertyTree* tree);
	void onMenuClear(PropertyTreeModel* model);
	std::string valueAsString() const{ return value().toString(); }

	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_COLUMN; }
protected:
};

PropertyRowHotkey::PropertyRowHotkey(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowImpl<sKey, PropertyRowHotkey>(object, size, name, nameAlt, typeName)
{

}

bool PropertyRowHotkey::onActivateWidget(PropertyTree* tree, PropertyRow* hostRow)
{
	return onActivate(tree);
}

bool PropertyRowHotkey::onActivate(PropertyTree* tree)
{
	sKey& key = value();

	kdw::HotkeyDialog hotkeyDialog(tree, key);
	if(hotkeyDialog.showModal() == RESPONSE_OK){
		key = hotkeyDialog.get();
		tree->model()->rowChanged(this);
		return true;
	}
	else
		return false;
}

bool PropertyRowHotkey::onContextMenu(PopupMenuItem& root, PropertyTree* tree)
{
	if(!root.empty())
		root.addSeparator();
	root.add(TRANSLATE("Очистить"), tree->model()).connect(this, &PropertyRowHotkey::onMenuClear);
	return __super::onContextMenu(root, tree);
}

void PropertyRowHotkey::onMenuClear(PropertyTreeModel* model)
{
	value() = sKey();
	model->rowChanged(this);
}

DECLARE_SEGMENT(PropertyRowHotkey)
REGISTER_PROPERTY_ROW(sKey, PropertyRowHotkey);

}
