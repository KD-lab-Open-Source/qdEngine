#pragma once
#include "foldertreenode.h"

/// ����� ����������� ������
class MusicFolderTreeNode : public FolderTreeNode
{
public:
	MusicFolderTreeNode();
	~MusicFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
};
