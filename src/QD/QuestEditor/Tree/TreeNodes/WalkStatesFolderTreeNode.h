#pragma once
#include "foldertreenode.h"

/// ����� ���������� ��������� ���������
class WalkStatesFolderTreeNode : public FolderTreeNode
{
public:
	WalkStatesFolderTreeNode();
	~WalkStatesFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
};
