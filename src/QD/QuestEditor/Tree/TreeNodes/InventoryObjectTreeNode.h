#pragma once
#include "namedobjecttreenode.h"

/// ����� ����, �������������� ���������
class InventoryObjectTreeNode : public NamedObjectTreeNode
{
public:
	InventoryObjectTreeNode(qdNamedObject* object);
	~InventoryObjectTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;

	bool showProperties(TreeLogic& logic, int iActive);
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	virtual bool canDrag(TreeLogic& logic);
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);
	virtual DROPEFFECT onDropData(TreeLogic& logic, 
		ITreeNode* dropTarget, 
		DROPEFFECT dropEffect);
	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;

	/// ������� ���������
	bool removeInventory(TreeLogic& logic, qdInventory* pInv, bool bAsk);
	/// ���������� true, ���� newName ���������
	bool nameTest(TreeLogic& logic, LPCSTR newName);
};
