#pragma once
#include "foldertreenode.h"

/// ����� ���������
class CounterFolderTreeNode : public FolderTreeNode
{
public:
	CounterFolderTreeNode(void);
	~CounterFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
};
