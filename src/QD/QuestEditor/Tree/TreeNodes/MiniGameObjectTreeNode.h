#pragma once
#include ".\namedobjecttreenode.h"

/// ����� ����, �������������� ��������
class MiniGameObjectTreeNode :
	public NamedObjectTreeNode
{
public:
	MiniGameObjectTreeNode(qdNamedObject* object);
	~MiniGameObjectTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;

	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);

	virtual bool showProperties(TreeLogic& logic, int activePage);

	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	/// ���������� true, ���� newName ���������
	bool nameTest(TreeLogic& logic, LPCSTR newName);

};
