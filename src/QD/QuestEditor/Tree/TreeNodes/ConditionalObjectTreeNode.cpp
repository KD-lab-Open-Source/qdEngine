#include "stdafx.h"
#include ".\conditionalobjecttreenode.h"

#include "qd_conditional_object.h"

bool CO_ShowCondDlg(qdGameDispatcher* ptrDisp, qdConditionalObject* ptrGame);

ConditionalObjectTreeNode::ConditionalObjectTreeNode(qdNamedObject* object)
: NamedObjectTreeNode(object)
{
}

ConditionalObjectTreeNode::~ConditionalObjectTreeNode(void)
{
}

bool ConditionalObjectTreeNode::showConditions(TreeLogic& logic)
{
	qdConditionalObject* const obj = static_cast<qdConditionalObject*>(getObject());

	return CO_ShowCondDlg(logic.getGameDispatcher(), obj);
}

bool ConditionalObjectTreeNode::canDrag(TreeLogic& logic){
	return true;
}
