#pragma once
#include ".\interfaceelementtreenode.h"

/// ���� �����
class InterfaceSaveTreeNode : public InterfaceElementTreeNode
{
public:
	InterfaceSaveTreeNode(qdInterfaceObjectBase* object);
	~InterfaceSaveTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
};
