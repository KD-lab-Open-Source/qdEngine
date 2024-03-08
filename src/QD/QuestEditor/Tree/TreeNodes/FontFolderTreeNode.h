#pragma once
#include "foldertreenode.h"

/// ����� ������
class FontFolderTreeNode : public FolderTreeNode
{
public:
	FontFolderTreeNode(void);
	~FontFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int activePage);
};
