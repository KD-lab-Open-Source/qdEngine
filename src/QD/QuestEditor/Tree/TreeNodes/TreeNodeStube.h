#pragma once
#include "tree\itreenode.h"
#include "Tree/ITreeNodeFactory.h"

#include "Tree/TreeLogic.h"

/// @brief ���������� ������� ITreeNode �� ���������. �������� ���������������� 
/// ����������� ���� ����� ������
class TreeNodeStube :
	public ITreeNode
{
public:
	TreeNodeStube(void);
	virtual ~TreeNodeStube(void);
	//! �������� ���� � ������
	virtual bool load(ITreeNodeFactory* factory,
		TreeLogic& treeLogic, CTreeListItem* parent, qdNamedObjectBase const* owner);
	//! ������������� ���������� 
	virtual bool reload(ITreeNodeFactory* factory, TreeLogic& logic, 
						ITreeNode const* parent, 
						qdNamedObjectBase const* owner);

	CTreeListItem* getTreeListItem() const;

	virtual void onRClick(TreeLogic& treeLogic);

	virtual bool showProperties(TreeLogic& treeLogic, int activePage);
	virtual void updateItemImage(TreeLogic& treeLogic);
	virtual bool showConditions(TreeLogic& logic);

	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool onCheckNode(TreeLogic& logic, bool bCheck);

	//! ������� ������ ������, � �������� �������� ����. ask ���������� �� 
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! \brief ����������, ����� ����� ������� �������, ������������ � ������,
	/// ��� ��� ��������(��������, ����� ��������� ������������� ������. 
	/// ��� �������, ����� ��������� �����)
	virtual bool deleteAsSubObject(TreeLogic& logic);
	//! ����� �� ������������ ����
	virtual bool canDrag(TreeLogic& logic);
	//! ����� �� ������ ���� �� ���, ��� ������� �����
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);

	virtual DROPEFFECT onDropData(TreeLogic& logic, 
							ITreeNode* dropTarget, 
							DROPEFFECT dropEffect);
	//! ���������� true, ���� ������� ������������, false � ��������� ������
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
	//! ����� �� ������� ��� ����
	virtual bool canRename(TreeLogic& logic);

	void* operator new(size_t size);
	void operator delete(void* p);

	void* operator new(size_t size, LPCTSTR file, unsigned line);
	void operator delete(void* p, LPCTSTR file, unsigned line);

protected:
	void setTreeListItem(CTreeListItem* item);
	/// ��������� ���� � ������ � ������ ������ � ������ ����� ������ � TreeLogic
	CTreeListItem* insertToTree(TreeLogic& treeLogic, 
										LPCTSTR title, 
										ITreeNode* parent, 
										UINT uiImage, 
										DWORD_PTR indexKey,
										ITreeNode* insertAfter);
	/// ������� ������ �� ������� ����� ������
	void removeIndexEntry(TreeLogic& logic, DWORD_PTR indexKey);

	//! ���������� ������������� ������� ���� ��� ����
	virtual DWORD getMenuResId() const = 0;
	//! ������������ ����, ��� ����� ��� ��������������� ����������� ����
	/**
		@param treeLogic - ������ ������
		@param pm - ���� ��������������� ����
	 */
	virtual void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;

	static bool showPropDlg(CPropertyPage** ptrPages, int iActive, UINT uiCaption);

	/// ���������� ���-���� � ���� ������
	void showCheckBox(TreeLogic& logic, bool bShow) const;
	/// ������/������� ������� � ��������
	void setCheck(TreeLogic& logic, bool bCheck) const;
	/// ���������� � ������������ ���������� ������� ������ � ������ objectName
	static bool askUserForDelete(LPCTSTR objectName);

	/// ������������ ITreeNode � CTreeListItem
	static CTreeListItem* nodeToItem(ITreeNode const *node);
	/// ������������ CTreeListItem � ITreeNode
	static ITreeNode* itemToNode(CTreeListItem* item);

private:
	/// ���� ������, ��������������� ����� �������
	CTreeListItem* item_;
};
