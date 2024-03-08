#pragma once
#include "interfaceobjecttreenode.h"

/// ���� ������������� ��������
class InterfaceElementTreeNode : public InterfaceObjectTreeNode
{
public:
	InterfaceElementTreeNode(qdInterfaceObjectBase* object);
	~InterfaceElementTreeNode(void);
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! ���������� true, ���� ������� ������������, false � ��������� ������
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
	//! ����� �� ������� ��� ����
	virtual bool canRename(TreeLogic& logic);
	virtual bool onCheckNode(TreeLogic& logic, bool bCheck);
protected:
	/// ���������� true, ���� newName ���������
	bool nameTest(TreeLogic& logic, LPCTSTR newName);
};
