#pragma once
#include "interfaceobjecttreenode.h"

/// ���� ������������� ������
class InterfaceScreenTreeNode : public InterfaceObjectTreeNode
{
public:
	InterfaceScreenTreeNode(qdInterfaceObjectBase* object);
	~InterfaceScreenTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! ���������� true, ���� ������� ������������, false � ��������� ������
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
	//! ����� �� ������� ��� ����
	virtual bool canRename(TreeLogic& logic);
protected:
	/// ��������� ���������� ������������� ������
	bool loadContent(ITreeNodeFactory* factory, TreeLogic& treeLogic);
	/// ���������� true, ���� newName ���������
	bool nameTest(TreeLogic& logic, LPCTSTR newName) const;
};
