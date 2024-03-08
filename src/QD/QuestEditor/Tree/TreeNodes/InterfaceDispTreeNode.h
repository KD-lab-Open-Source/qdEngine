#pragma once
#include "interfaceobjecttreenode.h"

/// ���� ������������� ���������� 
class InterfaceDispTreeNode : public InterfaceObjectTreeNode
{
public:
	InterfaceDispTreeNode(qdInterfaceObjectBase* object);
	~InterfaceDispTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	//! ����� �� ������� ��� ����
	virtual bool canRename(TreeLogic& logic);
protected:
	/// �������� ������������ ������. ������ ������ ��� ���������
	bool loadScreens(ITreeNodeFactory* factory, TreeLogic& treeLogic);
};
