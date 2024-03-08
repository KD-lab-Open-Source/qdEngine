#pragma once
#include "foldertreenode.h"

/// ����� ��� �� �����
class NetFolderTreeNode : public FolderTreeNode
{
public:
	NetFolderTreeNode();
	virtual ~NetFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, 
		TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;

	bool showProperties(TreeLogic& logic, int iActive);
	/// ��������� ������� ��������� �����
	virtual bool onCheckNode(TreeLogic& logic, bool bCheck);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
};
