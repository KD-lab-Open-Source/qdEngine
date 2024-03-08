#pragma once
#include "namedobjecttreenode.h"

class ConditionalObjectTreeNode :
	public NamedObjectTreeNode
{
public:
	ConditionalObjectTreeNode(qdNamedObject* object);
	~ConditionalObjectTreeNode(void);
	virtual bool showConditions(TreeLogic& logic);
	virtual bool canDrag(TreeLogic& logic);
};
