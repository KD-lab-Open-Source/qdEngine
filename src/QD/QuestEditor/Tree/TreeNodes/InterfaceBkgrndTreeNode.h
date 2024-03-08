#pragma once
#include ".\interfaceelementtreenode.h"

/// ����� ����������
class InterfaceBkgrndTreeNode : public InterfaceElementTreeNode
{
public:
	InterfaceBkgrndTreeNode(qdInterfaceObjectBase* object);
	~InterfaceBkgrndTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
};
