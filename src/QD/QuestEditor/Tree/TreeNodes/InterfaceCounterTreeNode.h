#pragma once
#include ".\interfaceelementtreenode.h"

/// ����� ����������
class InterfaceCounterTreeNode : public InterfaceElementTreeNode
{
public:
	InterfaceCounterTreeNode(qdInterfaceObjectBase* object);
	~InterfaceCounterTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
};
