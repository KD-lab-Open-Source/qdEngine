#pragma once
#include ".\interfaceelementtreenode.h"

/// ���� ���� � �������
class InterfaceTextWndTreeNode : public InterfaceElementTreeNode
{
public:
	InterfaceTextWndTreeNode(qdInterfaceObjectBase* object);
	~InterfaceTextWndTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
};
