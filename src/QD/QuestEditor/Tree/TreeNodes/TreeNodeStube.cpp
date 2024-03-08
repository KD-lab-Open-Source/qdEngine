#include "stdafx.h"
#include "resource.h"
#include ".\treenodestube.h"

#include "MyPropertySheet.h"

#include <boost/pool/pool.hpp>
typedef boost::pool<> TreeNodePool;
TreeNodePool pool(1, 256);

ITreeNode::~ITreeNode(){}

TreeNodeStube::TreeNodeStube(void):
  item_(NULL)
{
}

TreeNodeStube::~TreeNodeStube(void)
{
}

CTreeListItem* TreeNodeStube::getTreeListItem() const{
	return item_;
}

void TreeNodeStube::setTreeListItem(CTreeListItem* item){
	item_ = item;
}

bool TreeNodeStube::load(ITreeNodeFactory* factory,
						TreeLogic& treeLogic, 
						CTreeListItem* parent, 
						qdNamedObjectBase const* owner)
{
	return false;
}

//! Перезагружает содержимое 
bool TreeNodeStube::reload(ITreeNodeFactory* factory, 
						   TreeLogic& logic, 
						   ITreeNode const* parent, 
						   qdNamedObjectBase const* owner)
{
	return false;
}

CTreeListItem* TreeNodeStube::insertToTree(TreeLogic& treeLogic, 
											LPCTSTR title, 
											ITreeNode* parent, 
											UINT uiImage, 
											DWORD_PTR indexKey,
											ITreeNode* insertAfter)
{
	QETreeListCtrl& tree = treeLogic.getTreeListCtrl();
	CTreeListItem* res = tree.InsertItem(title, 
										nodeToItem(parent), 
										nodeToItem(insertAfter));
	if (res)
	{
		setTreeListItem(res);
		tree.SetItemImage(res, uiImage);
		tree.SetItemData(res, reinterpret_cast<DWORD_PTR>(this));
		treeLogic.addTreeNodeIndexEntry(indexKey, this);
	}
	return res;
}

void TreeNodeStube::removeIndexEntry(TreeLogic& logic, DWORD_PTR indexKey)
{
	logic.removeTreeNodeIndexEntry(indexKey, this);
}

void TreeNodeStube::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
}

void TreeNodeStube::onRClick(TreeLogic& treeLogic)
{

	CMenu menu;
	menu.LoadMenu(getMenuResId());
	CPoint point;
	GetCursorPos(&point);
	CMenu *pm = menu.GetSubMenu(0);

	modifyMenu(treeLogic, pm);
	pm->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, 
						point.x, point.y, treeLogic.getBaseWnd());
}

bool TreeNodeStube::showProperties(TreeLogic& treeLogic, int activePage)
{
	return false;
}

bool TreeNodeStube::showPropDlg(CPropertyPage** ptrPages, int iActive, UINT uiCaption)
{
	CMyPropertySheet propSheet(ptrPages, iActive, uiCaption);
	int res = propSheet.DoModal();
	return (res == IDOK);
}

void TreeNodeStube::updateItemImage(TreeLogic& treeLogic)
{

}

bool TreeNodeStube::showConditions(TreeLogic& logic)
{
	return false;
}

bool TreeNodeStube::onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState)
{
	return false;
}

bool TreeNodeStube::onCheckNode(TreeLogic& logic, bool bCheck)
{
	return false;
}


void TreeNodeStube::showCheckBox(TreeLogic& logic, bool bShow) const{
	ASSERT(getTreeListItem());
	logic.getTreeListCtrl().ShowItemCheckBox(getTreeListItem(), bShow);
}

void TreeNodeStube::setCheck(TreeLogic& logic, bool bCheck) const{
	ASSERT(getTreeListItem());
	logic.getTreeListCtrl().SetItemCheck(getTreeListItem(), bCheck, false);
}

bool TreeNodeStube::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	_ASSERTE((_T("deleteObject not implemented"), 0));
	return false;
}

bool TreeNodeStube::askUserForDelete(LPCTSTR objectName)
{
	CString strMsg;
	strMsg.Format(IDS_DEL_OBJECT, objectName);

	return (AfxMessageBox(strMsg,MB_YESNO|MB_ICONQUESTION) == IDYES);
}

//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
// при его удаление(сотояния, когда удаляется анимированный объект. 
//Все объекты, когда удаляется сцена)
bool TreeNodeStube::deleteAsSubObject(TreeLogic& logic)
{
	return false;
}

//! Можно ли перетягивать узел
bool TreeNodeStube::canDrag(TreeLogic& logic){
	return false;
}

//! Можно ли кинуть узел на тот, над которым тянем
DROPEFFECT TreeNodeStube::canDrop(TreeLogic& logic, ITreeNode* overNode)
{
	return DROPEFFECT_NONE;
}

DROPEFFECT TreeNodeStube::onDropData(TreeLogic& logic, 
							   ITreeNode* dropTarget, 
							   DROPEFFECT dropEffect)
{
	return DROPEFFECT_NONE;
}

CTreeListItem* TreeNodeStube::nodeToItem(ITreeNode const *node)
{
	return TreeLogic::nodeToItem(node);
}

ITreeNode* TreeNodeStube::itemToNode(CTreeListItem* item)
{
	return TreeLogic::itemToNode(item);
}

//! Возвращает true, если удалось переимновать, false в противном случае
bool TreeNodeStube::rename(TreeLogic& logic, LPCTSTR newName)
{
	return false;
}

//! Можно ли сменить имя узлу
bool TreeNodeStube::canRename(TreeLogic& logic){
	return false;
}

void* TreeNodeStube::operator new(size_t size)
{
//	return TreeNodeSingletonPool::malloc(size);
	return pool.ordered_malloc(size);
}

void TreeNodeStube::operator delete(void* p)
{
//	return TreeNodeSingletonPool::free(p);
	return pool.ordered_free(p);
}

void* TreeNodeStube::operator new(size_t size, LPCTSTR file, unsigned line)
{
	return TreeNodeStube::operator new(size);
}

void TreeNodeStube::operator delete(void* p, LPCTSTR file, unsigned line)
{
	TreeNodeStube::operator delete(p);
}




