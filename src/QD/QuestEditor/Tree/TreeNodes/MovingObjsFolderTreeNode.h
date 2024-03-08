#pragma once
#include "foldertreenode.h"

/// ����� ����������
class MovingObjsFolderTreeNode : public FolderTreeNode
{
public:
	MovingObjsFolderTreeNode();
	~MovingObjsFolderTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
protected:
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;
};
