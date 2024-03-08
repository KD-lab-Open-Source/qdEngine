#pragma once
#include "namedobjecttreenode.h"

/// ����� ����, ���������������� ��������
class CounterObjectTreeNode : public NamedObjectTreeNode
{
public:
	CounterObjectTreeNode(qdNamedObject* object);
	~CounterObjectTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, 
		ITreeNode *parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	/// ���������� true, ���� ��� newName ���������
	bool nameTest(TreeLogic& logic, LPCSTR newName);
};
