#pragma once
#include ".\interfaceelementtreenode.h"

/// ����� ������������ ������
class InterfaceButtonTreeNode : public InterfaceElementTreeNode
{
public:
	InterfaceButtonTreeNode(qdInterfaceObjectBase* object);
	~InterfaceButtonTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;

	bool showProperties(TreeLogic& logic, int activePage);
protected:
	/// ��������� � ������ ��������� ������
	bool loadStates(ITreeNodeFactory* factory, TreeLogic& treeLogic);
};
