#pragma once
#include "animatedobjecttreenode.h"

/// ����� ����, �������������� ����
class MouseTreeNode : public AnimatedObjectTreeNode
{
public:
	MouseTreeNode(qdNamedObject* object);
	~MouseTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent,
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	bool showProperties(TreeLogic& logic, int iActive);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	DWORD getMenuResId() const;
	virtual bool canDrag(TreeLogic& logic);
	//! ���������� true, ���� ������� ������������, false � ��������� ������
	virtual bool canRename(TreeLogic& logic);
};
