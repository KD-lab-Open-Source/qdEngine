#pragma once
#include ".\conditionalobjecttreenode.h"

/// ����� ����, ���������������� ��������� ������. 
class GameEndObjectTreeNode : public ConditionalObjectTreeNode
{
public:
	GameEndObjectTreeNode(qdNamedObject* object);
	~GameEndObjectTreeNode(void);
	virtual bool load(ITreeNodeFactory* factory,
						TreeLogic& treeLogic, 
						ITreeNode * parent, 
						qdNamedObjectBase const* owner,
						ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	virtual bool canDrag(TreeLogic& logic);
	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	/// ���������� true, ���� newName ���������
	bool nameTest(TreeLogic& logic, LPCSTR newName);
};
