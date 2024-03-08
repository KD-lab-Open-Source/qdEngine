#include "StdAfx.h"
#include "kdw/Serialization.h"
#include "kdw/PropertyRow.h"
#include "kdw/PropertyTree.h"
#include "kdw/PropertyTreeModel.h"
#include "kdw/FileDialog.h"
#include "kdw/PopupMenu.h"
#include "kdw/ContentUtil.h"
#include "FileUtils/FileUtils.h"
#include "Serialization/SerializationFactory.h"
#include "Serialization/GenericFileSelector.h"
#include "Serialization/ResourceSelector.h"
#include "kdw/Win32/Drawing.h"
#include "kdw/Win32/Window.h"

namespace kdw{

class PropertyRowFileSelector : public PropertyRowImpl<GenericFileSelector, PropertyRowFileSelector>, public sigslot::has_slots{
public:
	PropertyRowFileSelector(void* object, int size, const char* name, const char* nameAlt, const char* typeName);
	PropertyRowFileSelector()
	{}
	bool activateOnAdd() const{ return true; }
	bool onActivate(PropertyTree* tree);
	bool onActivateWidget(PropertyTree* tree, PropertyRow* hostRow);
	bool onContextMenu(PopupMenuItem& root, PropertyTree* tree);
	void onMenuClear(PropertyTreeModel* model);

	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, kdw::PropertyRow* hostRow);
	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_COLUMN; }
	std::string valueAsString() const{ return static_cast<const char*>(value()); }
protected:
};

void PropertyRowFileSelector::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, kdw::PropertyRow* hostRow)
{
	if(multiValue())
		Win32::drawEdit(dc, widgetRect, " ... ", Win32::defaultFont(), true, true);
	else
		Win32::drawEdit(dc, widgetRect, value(), Win32::defaultFont(), true);
}

PropertyRowFileSelector::PropertyRowFileSelector(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowImpl<GenericFileSelector, PropertyRowFileSelector>(object, size, name, nameAlt, typeName)
{

}


bool PropertyRowFileSelector::onActivateWidget(PropertyTree* tree, PropertyRow* hostRow)
{
	return onActivate(tree);
}

bool PropertyRowFileSelector::onActivate(PropertyTree* tree)
{
	GenericFileSelector& selector = value();
	string title = std::string("(") + selector.filter() + ")";
	const char* masks[] = { title.c_str(), selector.filter(), 0 };


	kdw::FileDialog fileDialog(tree, selector.save(), masks,
							   selector.initialDir(), selector);
	if(fileDialog.showModal()){
		selector.setFileName(fileDialog.fileName());
		tree->model()->rowChanged(this);
		return true;
	}
	else
		return false;
}

bool PropertyRowFileSelector::onContextMenu(PopupMenuItem& root, PropertyTree* tree)
{
	if(!root.empty())
		root.addSeparator();
	root.add(TRANSLATE("Очистить"), tree->model()).connect(this, &PropertyRowFileSelector::onMenuClear);
	return __super::onContextMenu(root, tree);
}

void PropertyRowFileSelector::onMenuClear(PropertyTreeModel* model)
{
	value().setFileName("");
	model->rowChanged(this);
}

DECLARE_SEGMENT(PropertyRowFileSelector)
REGISTER_PROPERTY_ROW(GenericFileSelector, PropertyRowFileSelector);

class PropertyRowResourceSelector : public PropertyRowImpl<ResourceSelector, PropertyRowResourceSelector>, public sigslot::has_slots{
public:
	PropertyRowResourceSelector(void* object, int size, const char* name, const char* nameAlt, const char* typeName);
	PropertyRowResourceSelector()
	{}
	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);
	bool activateOnAdd() const{ return true; }
	bool onActivate(PropertyTree* tree);
	bool onActivateWidget(PropertyTree* tree, PropertyRow* hostRow);
	bool onContextMenu(PopupMenuItem& root, PropertyTree* tree);
	void onMenuClear(PropertyTreeModel* model);

	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_COLUMN; }
	std::string valueAsString() const{ return static_cast<const char*>(value()); }
protected:
};

PropertyRowResourceSelector::PropertyRowResourceSelector(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowImpl<ResourceSelector, PropertyRowResourceSelector>(object, size, name, nameAlt, typeName)
{

}

void PropertyRowResourceSelector::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow)
{
	if(multiValue())
		Win32::drawEdit(dc, widgetRect, " ... ", Win32::defaultFont(), true, true);
	else
		Win32::drawEdit(dc, widgetRect, value(), Win32::defaultFont(), true);
}

bool PropertyRowResourceSelector::onActivateWidget(PropertyTree* tree, PropertyRow* hostRow)
{
	return onActivate(tree);
}

bool PropertyRowResourceSelector::onActivate(PropertyTree* tree)
{
	ResourceSelector& selector = value();
	string title = std::string("(") + selector.options().filter.c_str() + ")";
	const char* masks[] = { title.c_str(), selector.options().filter.c_str(), 0 };

	std::string fileName = selectAndCopyResource(selector.options().initialDir.c_str(),
												 masks,
												 ::extractFileName(selector).c_str(),
                                                 selector.options().title.c_str(), tree);
	if(!fileName.empty()){
		selector.setFileName(fileName.c_str());
		tree->model()->rowChanged(this);
		return true;
	}
	return false;
}

bool PropertyRowResourceSelector::onContextMenu(PopupMenuItem& root, PropertyTree* tree)
{
	if(!root.empty())
		root.addSeparator();
	root.add(TRANSLATE("Очистить"), tree->model()).connect(this, &PropertyRowResourceSelector::onMenuClear);
	return __super::onContextMenu(root, tree);
}

void PropertyRowResourceSelector::onMenuClear(PropertyTreeModel* model)
{
	value().setFileName("");
	model->rowChanged(this);
}

REGISTER_PROPERTY_ROW(ResourceSelector, PropertyRowResourceSelector);


class PropertyRowModelSelector : public PropertyRowImpl<ModelSelector, PropertyRowModelSelector>, public sigslot::has_slots{
public:
	PropertyRowModelSelector(void* object, int size, const char* name, const char* nameAlt, const char* typeName);
	PropertyRowModelSelector()
	{}
	bool activateOnAdd() const{ return true; }
	bool onActivate(PropertyTree* tree);
	bool onActivateWidget(PropertyTree* tree, PropertyRow* hostRow);
	bool onContextMenu(PopupMenuItem& root, PropertyTree* tree);
	void onMenuClear(PropertyTreeModel* model);
	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);

	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_COLUMN; }
	std::string valueAsString() const{ return static_cast<const char*>(value()); }
protected:
};

PropertyRowModelSelector::PropertyRowModelSelector(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowImpl<ModelSelector, PropertyRowModelSelector>(object, size, name, nameAlt, typeName)
{

}

bool PropertyRowModelSelector::onActivateWidget(PropertyTree* tree, PropertyRow* hostRow)
{
	return onActivate(tree);
}

bool PropertyRowModelSelector::onActivate(PropertyTree* tree)
{
	ModelSelector& selector = value();
	string title = std::string("(") + selector.options().filter.c_str() + ")";
	const char* masks[] = { title.c_str(), selector.options().filter.c_str(), 0 };

	std::string fileName = selectAndCopyModel(selector.options().initialDir.c_str(),
												 masks,
												 ::extractFileName(selector).c_str(),
                                                 selector.options().title.c_str(), tree);
	if(!fileName.empty()){
		selector.setFileName(fileName.c_str());
		tree->model()->rowChanged(this);
		return true;
	}
	return false;
}

void PropertyRowModelSelector::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow)
{
	if(multiValue())
		Win32::drawEdit(dc, widgetRect, " ... ", Win32::defaultFont(), true, true);
	else
		Win32::drawEdit(dc, widgetRect, value(), Win32::defaultFont(), true);
}

bool PropertyRowModelSelector::onContextMenu(PopupMenuItem& root, PropertyTree* tree)
{
	if(!root.empty())
		root.addSeparator();
	root.add(TRANSLATE("Очистить"), tree->model()).connect(this, &PropertyRowModelSelector::onMenuClear);
	return __super::onContextMenu(root, tree);
}

void PropertyRowModelSelector::onMenuClear(PropertyTreeModel* model)
{
	value().setFileName("");
	model->rowChanged(this);
}

REGISTER_PROPERTY_ROW(ModelSelector, PropertyRowModelSelector);

}
