#include "stdafx.h"
#include ".\foldertreenode.h"

FolderTreeNode::FolderTreeNode(DWORD folderId):
  folderId_(folderId)
{
}

FolderTreeNode::~FolderTreeNode()
{
}

DWORD FolderTreeNode::getFolderId() const{
	return folderId_;
}

ContentClassificator FolderTreeNode::getContentClassificator() const{
	return CC_FOLDER;
}

void FolderTreeNode::onDeleteItem(TreeLogic& logic)
{
	removeIndexEntry(logic, getFolderId());
}

NodeContent FolderTreeNode::getContent() const{
	NodeContent res;
	res.folderId = getFolderId();
	return res;
}
