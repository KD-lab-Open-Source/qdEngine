#include "stdafx.h"
#include ".\interfaceobjecttreenode.h"

InterfaceObjectTreeNode::InterfaceObjectTreeNode(qdInterfaceObjectBase* object)
: object_(object)
{
}

InterfaceObjectTreeNode::~InterfaceObjectTreeNode(void)
{
}

qdInterfaceObjectBase* InterfaceObjectTreeNode::getObject() const{
	return object_;
}

void InterfaceObjectTreeNode::setObject(qdInterfaceObjectBase* obj){
	object_ = NULL;
}

ContentClassificator InterfaceObjectTreeNode::getContentClassificator() const{
	return CC_INTERFACE_OBJECT;
}

void InterfaceObjectTreeNode::onDeleteItem(TreeLogic& logic)
{
	removeIndexEntry(logic, reinterpret_cast<DWORD_PTR>(getObject()));
}

NodeContent InterfaceObjectTreeNode::getContent() const{
	NodeContent res;
	res.interfaceObject = getObject();
	return res;
}

bool InterfaceObjectTreeNode::preDelete(TreeLogic& logic, bool ask)
{
	if (ask&&!askUserForDelete(getObject()->name()))
		return false;

	return true;
}
