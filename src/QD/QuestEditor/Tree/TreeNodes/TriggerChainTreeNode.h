#pragma once
#include "namedobjecttreenode.h"

/// ���� ������� ���������
class TriggerChainTreeNode : public NamedObjectTreeNode
{
public:
	TriggerChainTreeNode(qdNamedObject* object);
	~TriggerChainTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent,
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepnds);
	virtual bool canDrag(TreeLogic& logic);
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);
	virtual DROPEFFECT onDropData(TreeLogic& logic, ITreeNode* dropTarget, 
		DROPEFFECT dropEffect);

	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	/// �������(����� �����) ��� �������� ��������� �����
	bool onSelectInTriggerView(TreeLogic& logic, bool bSelect);
	/// �������(����� �����) ��� �������� ��������� ���������
	bool onSelectInSceneView(TreeLogic& logic, bool bSelect);
	/// ��������� �������� ����� ��������
	void postDelete(TreeLogic& logic);

	/// ���������� ��������
	BOOL MergeTriggers(TreeLogic& logic, qdTriggerChain const* from, qdTriggerChain* to);
	/// ���������� true, ���� newName ���������
	bool nameTest(TreeLogic& logic, LPCSTR newName);
};
