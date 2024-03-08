#pragma once
#include "NamedObjectTreeNode.h"


class GameObjectTreeNode : public NamedObjectTreeNode
{
public:
	GameObjectTreeNode(qdNamedObject* object);
	~GameObjectTreeNode(void);
	virtual bool onCheckNode(TreeLogic& logic, bool bCheck);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! \brief ����������, ����� ����� ������� �������, ������������ � ������,
	// ��� ��� ��������(��������, ����� ��������� ������������� ������. 
	//��� �������, ����� ��������� �����)
	virtual bool deleteAsSubObject(TreeLogic& logic);
	//! ���������� true, ���� ������� ������������, false � ��������� ������
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
	//! ����� �� ������� ��� ����
	virtual bool canRename(TreeLogic& logic);
protected:
	//! ������� ������ �� ����� � ������������ ������ ���������� ��������
	virtual bool removeObject(TreeLogic& logic);
	bool nameTest(TreeLogic& logic, LPCTSTR newName);
};
