#pragma once
#include "foldertreenode.h"

/// ����� ���������
class TriggerFolderTreeNode : public FolderTreeNode
{
public:
	TriggerFolderTreeNode(void);
	~TriggerFolderTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, 
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
};
