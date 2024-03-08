#pragma once
#include "foldertreenode.h"

/// ����� ��������� �������
class GameEndFolderTreeNode :
	public FolderTreeNode
{
public:
	GameEndFolderTreeNode(void);
	~GameEndFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
};
