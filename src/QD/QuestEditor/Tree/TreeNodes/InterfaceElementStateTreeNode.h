#pragma once
#include "interfaceobjecttreenode.h"

/// ���� ��������� ������������� ��������
class InterfaceElementStateTreeNode : public InterfaceObjectTreeNode
{
public:
	InterfaceElementStateTreeNode(qdInterfaceObjectBase* object);
	~InterfaceElementStateTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! ���������� true, ���� ������� ������������, false � ��������� ������
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
	//! ����� �� ������� ��� ����
	virtual bool canRename(TreeLogic& logic);
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
protected:
	/// ��������� ������ � ������
	bool loadModes(ITreeNodeFactory* factory, TreeLogic& treeLogic) ;

	/// ������� ������, ���� � ��� ��� ������ ���������
	bool postDelete(TreeLogic& logic);
	/// ���������� true, ���� newName ���������
	bool nameTest(TreeLogic& logic, LPCTSTR newName);
};
