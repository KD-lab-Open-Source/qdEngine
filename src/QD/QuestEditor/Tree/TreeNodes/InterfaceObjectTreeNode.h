#pragma once
#include "treenodestube.h"

class qdInterfaceObjectBase;

/// ������� ����� ��� �����, ��������������� ������������ ��������
/**
	��������� ������ ��������������� ���� � ����������� ������� 
	���������������� �� �������� �������
 */
class InterfaceObjectTreeNode : public TreeNodeStube
{
public:
	InterfaceObjectTreeNode(qdInterfaceObjectBase* object);
	~InterfaceObjectTreeNode(void);
	virtual ContentClassificator getContentClassificator() const;
	virtual void onDeleteItem(TreeLogic& logic);
	NodeContent getContent() const;
protected:
	/// ���������� ��������������� ���� ������
	qdInterfaceObjectBase* getObject() const;
	/// ��������� ������, ��������������� ����
	void setObject(qdInterfaceObjectBase* obj);
	/// ���������� � ������������, ����� �� ������� ������
	bool preDelete(TreeLogic& logic, bool ask);
private:
	/// ������������ ������, ��������������� ����
	qdInterfaceObjectBase* object_;
};
