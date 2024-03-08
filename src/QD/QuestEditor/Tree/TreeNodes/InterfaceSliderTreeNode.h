#pragma once
#include ".\interfaceelementtreenode.h"

/// ���� ��������
class InterfaceSliderTreeNode : public InterfaceElementTreeNode
{
public:
	InterfaceSliderTreeNode(qdInterfaceObjectBase* object);
	~InterfaceSliderTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;

	bool showProperties(TreeLogic& logic, int iActive);
};
