#pragma once
#include "foldertreenode.h"

/// ����� ����������� ��������� ���������
class StaticStatesFolderTreeNode : public FolderTreeNode
{
public:
	StaticStatesFolderTreeNode();
	~StaticStatesFolderTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
};
