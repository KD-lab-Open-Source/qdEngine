#include "stdafx.h"
#include "QuestEditor.h"
#include ".\treelogic.h"
#include "Tree/ITreeNode.h"
#include "Tree/TreeNodeFactory.h"
#include "Tree/NodeTraits.h"

#include "Main/ObjectTraits.h"
#include "ptree_consts.h"

#include <boost/rtl.hpp>
#include <boost/tokenizer.hpp>

#include "ISceneViewFacade.h"
#include "ITriggerView.h"
#include "Main/Project.h"

#include "Tree\treecustomdrawer.h"

#include "qd_game_object_state.h"

#include "Tree\TreeNodes\ObjectStateTreeNode.h"

#include <sstream>
#include <string>
#include <fstream>

namespace
{
DWORD itemStateToNodeState(DWORD itemState)
{
	DWORD res = 0;
	if (itemState&TLIS_SELECTED)
		res |= NS_SELECTED;

	if (itemState&TLIS_FOCUSED)
		res |= NS_FOCUSED;
	return res;
}

}
/* Функции этого кода теперь выполняет TreeLogic::saveFullStruct(...)
class TreeLogic::ExpandDataSaver
{
public:
	ExpandDataSaver(std::ostream& out):out_(out)
	{
	}
	bool operator()(CTreeListItem* item){
		char const v = item->GetState(TLIS_EXPANDED)? '1' : '0';
		out_<<v;
//		out_.write(&v, sizeof(char));
		return true;
	}
private:
	std::ostream& out_;
};

class TreeLogic::ExpandDataLoader
{
public:
	ExpandDataLoader(std::istream& in, QETreeListCtrl& tree)
		: in_(in)
		, tree_(tree)
	  {
	  }
	  bool operator()(CTreeListItem* item){
		  if (in_.eof())
			  return false;
		  char expanded = '0';
		  if (!(in_>>expanded))
			  return false;

		  if (expanded == '1')
			  tree_.Expand(item, TLE_EXPAND);
		  return true;
	  }
private:
	QETreeListCtrl& tree_;
	std::istream& in_;
};
*/
/************************************************************************/
/* Драг и дроп для класса SceneTreeView                                 */
/************************************************************************/
BOOL TreeLogicDropTarget::OnDrop(CWnd* pWnd, 
								 COleDataObject* pData, 
								 DROPEFFECT dropEffect, CPoint point)
{
	UINT uiDataFormat = RegisterClipboardFormat(CString((LPTSTR)IDS_CB_FORMAT_NAME));
	ASSERT (pData->IsDataAvailable(uiDataFormat)); 

	HGLOBAL hData = pData->GetGlobalData(uiDataFormat);
	void* pdata = GlobalLock(hData);
	GlobalUnlock(hData);

	m_bDropInMe = true;

	return logic_.OnDropData(pdata, dropEffect, point);
}

DROPEFFECT TreeLogicDropTarget::OnDragEnter( CWnd* pWnd, 
											COleDataObject* pDataObject, 
											DWORD dwKeyState, CPoint point )
{
	UINT uiId = RegisterClipboardFormat(CString((LPTSTR)IDS_CB_FORMAT_NAME));
	if (pDataObject->IsDataAvailable(uiId))
		return logic_.canDrop(point);
	return DROPEFFECT_NONE;
}

DROPEFFECT TreeLogicDropTarget::OnDragOver( CWnd* pWnd, 
										   COleDataObject* pDataObject, 
										   DWORD dwKeyState, CPoint point )
{
	UINT uiId = RegisterClipboardFormat(CString((LPTSTR)IDS_CB_FORMAT_NAME));
	if (pDataObject->IsDataAvailable(uiId))
	{
		return (logic_.canDrop(point));
	}
	return DROPEFFECT_NONE;
}
DROPEFFECT TreeLogicDropTarget::OnDragScroll(CWnd* pWnd, 
											 DWORD dwKeyState, 
											 CPoint point)
{
	QETreeListCtrl& tree = logic_.getTreeListCtrl();

	CRect rectClient, rect;
	tree.GetClientRect(rectClient);
	rect = rectClient;

	rect.InflateRect(-10, -10);
	if (rectClient.PtInRect(point) && !rect.PtInRect(point))
	{
		// determine which way to scroll along both X & Y axis
		UINT nTimerID = MAKEWORD(-1, -1);
		if (point.x < rect.left)
			nTimerID = MAKEWORD(SB_LINEUP, HIBYTE(nTimerID));
		else if (point.x >= rect.right)
			nTimerID = MAKEWORD(SB_LINEDOWN, HIBYTE(nTimerID));
		if (point.y < rect.top)
			nTimerID = MAKEWORD(LOBYTE(nTimerID), SB_LINEUP);
		else if (point.y >= rect.bottom)
			nTimerID = MAKEWORD(LOBYTE(nTimerID), SB_LINEDOWN);
		ASSERT(nTimerID != MAKEWORD(-1, -1));

		tree.OnScroll(nTimerID);
		return DROPEFFECT_SCROLL;
	}
	return logic_.canDrop(point);

//	return COleDropTarget::OnDragScroll(pWnd, dwKeyState, point);
}


TreeLogic::TreeLogic(Project* project, ITreeNodeFactory* nodeFactory):
	project_(project)
, treeNodeFactory_(nodeFactory)
, sceneView_(NULL)
, tree_(NULL)
, currentSceneNode_(NULL)
, baseWnd_(NULL)
, triggerView_(NULL)
, draggedNodeParent_(NULL)
, draggedNode_(NULL)
, loadingTree_(false)
{
	customDrawer_.reset(new TreeCustomDrawer);
}

TreeLogic::~TreeLogic(void)
{
}

IViewMgr* TreeLogic::getViewMgr() const{
	return project_;
}

ITreeNodeFactory* TreeLogic::getTreeNodeFactory() const{
	return treeNodeFactory_;
}

IProjectFacade* TreeLogic::getProject() const{
	return project_;
}

QETreeListCtrl& TreeLogic::getTreeListCtrl() const{
	return *tree_;
}

ITreeNode* TreeLogic::getTreeNode(CTreeListItem const* item) const{
	return reinterpret_cast<ITreeNode*>(getTreeListCtrl().GetItemData(item));
}

ITreeNode* TreeLogic::getFocusedNode() const
{
	CTreeListItem* const item = getTreeListCtrl().GetFocusedItem();
	if (!item)
		return NULL;
	return getTreeNode(item);
}

void TreeLogic::setFocusedNode(ITreeNode* node)
{
	if (!node)
		getTreeListCtrl().SetFocusedItem(NULL);
	else
	{
		CTreeListItem* item = nodeToItem(node);
		getTreeListCtrl().SelectItem(item);
	}
}

IObjectList* TreeLogic::getObjectList() const{
	return project_->getObjectList();
}
void TreeLogic::setTreeListCtrl(QETreeListCtrl* tree)
{
	ASSERT(tree);
	tree_ = tree;
	tree->setItemDragdropNotifyListener(this);
	tree->setChangeItemLabelNotifyListener(this);
	tree->setItemOperationNotifyListener(this);
	tree->setGeneralNotifyListener(this);
	tree->setItemChangeNotifyListener(this);
	tree->SetCustomDrawNotifyListener(customDrawer_.get());

	tree->SetColumnModify(0, TLM_EDIT);

	dropTarget_.reset(new TreeLogicDropTarget(*this));
	dropTarget_->Register(tree_);	

}

void TreeLogic::setSceneView(ISceneViewFacade* sceneView){
	sceneView_ = sceneView;
}

void TreeLogic::setTriggerView(ITriggerView* view){
	triggerView_ = view;
}

ITriggerView* TreeLogic::getTriggerView() const{
	return triggerView_;
}

void TreeLogic::setBaseWnd(CWnd* wnd)
{
	baseWnd_ = wnd;
}

CWnd* TreeLogic::getBaseWnd() const{
	return baseWnd_;
}

TreeItemsIndex& TreeLogic::getSceneItemsIndex(){
	return sceneItemsIndex_;
}

//! Возвращает идекс объект-узел дерева.
TreeLogic::TreeNodeIndex const& TreeLogic::getTreeNodeIndex() const{
	return treeNodeIndex_;
}
TreeLogic::TreeNodeIndex& TreeLogic::getTreeNodeIndex(){
	return treeNodeIndex_;
}

bool TreeLogic::isSceneViewActive() const{
	return project_->isSceneViewActive();
}

void TreeLogic::setActiveSceneView(bool bActive){
	project_->setSceneViewActive(bActive);
}

qdGameDispatcher* TreeLogic::getGameDispatcher() const{
	return project_->getGameDispatcher();
}

ITreeNode* TreeLogic::getCurrentSceneNode() const{
	return currentSceneNode_;
}

void TreeLogic::setCurrentSceneNode(ITreeNode* node){
	currentSceneNode_ = node;
}

ISceneViewFacade* TreeLogic::getSceneView() const{
	return sceneView_;
}

TreeLogic::eFilterScheme TreeLogic::getFilterScheme() const{
//	return filterScheme_;
	return static_cast<TreeLogic::eFilterScheme>
		(project_->getTriggerElementFilterScheme());
}

bool TreeLogic::getShowTriggerElement() const{
	return project_->getShowTriggerElement();
}

ITreeNode* TreeLogic::getDraggedParentNode() const{
	return draggedNodeParent_;
}

//! Строит дерево проекта
bool TreeLogic::loadStructure()
{
	class LoadTreeGuard{
	public:
		LoadTreeGuard(bool *pflag):pflag_(pflag){
			*pflag_ = true;
		}
		~LoadTreeGuard(){
			*pflag_ = false;
		}
	private:
		bool *pflag_;
	} guard(&loadingTree_);

	qdGameDispatcher *gameDisp = getProject()->getGameDispatcher();
	ITreeNode* gameDispNode = treeNodeFactory_->createGameObjectNode(gameDisp).release();
	return gameDispNode->load(treeNodeFactory_, *this, TN_ROOT, NULL);
}

//! Рекурсивная часть функции для сохранения всей структуры дерева
void TreeLogic::saveFullStructRecur(std::ofstream& out, ITreeNode* child)
{
	while (child)
	{
		if (isExpanded(child))
			out.put('1');
		else
			out.put('0');
		saveFullStructRecur(out, getChildNode(child));
		child = getNextSiblingNode(child);
	}
}
//! Сохраняет всю структуру дерева в файл
void TreeLogic::saveFullStruct(const char* fname)
{
	std::ofstream out(fname);
	if (out)
	{
		ITreeNode* root_child = itemToNode(getTreeListCtrl().GetChildItem(TLI_ROOT));
		saveFullStructRecur(out, root_child);
	}
}

//! Рекурсивная часть функции для загрузки всей структуры дерева
void TreeLogic::loadFullStructRecur(std::ifstream& in, ITreeNode* child)
{
	while (child)
	{
		char c;
		in.get(c);
		if ('1' == c)
			expand(child, EC_EXPAND);
		else
			expand(child, EC_COLLAPSE);
		loadFullStructRecur(in, getChildNode(child));
		child = getNextSiblingNode(child);
	}
}
//! Грузит всю структуру дерева из файла
void TreeLogic::loadFullStruct(const char* fname)
{
	std::ifstream in(fname);
	if (in)
	{
		ITreeNode* root_child = itemToNode(getTreeListCtrl().GetChildItem(TLI_ROOT));
		loadFullStructRecur(in, root_child);
	}
}

//! Начинаем тянуть выделенные элементы левой кнопкой мыши
bool TreeLogic::onBeginDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd)
{
	QETreeListCtrl& tree = getTreeListCtrl();
	draggedNode_ = getTreeNode(tree.GetFocusedItem());
	if (!draggedNode_->canDrag(*this))
		return false;

	//m_hDraggedItemParent нам нужен, когда перетакскиваем
	//состояния под другие объекты. Так что если он будет NULL
	//ничего страшного.
	draggedNodeParent_ = isNodeUnderAnimated(draggedNode_, NULL);
	if (!draggedNodeParent_)
		draggedNodeParent_ = 
			getTreeNode(tree.GetParentItem(draggedNode_->getTreeListItem()));

	std::auto_ptr<COleDataSource> pSource(new COleDataSource);

	CString text;
	text = tree.GetItemText(draggedNode_->getTreeListItem());

	int n = text.GetLength();

	CString strCFName((LPTSTR)IDS_CB_FORMAT_NAME);
	UINT clFromatId = RegisterClipboardFormat(strCFName);

	HGLOBAL h = ::GlobalAlloc( GMEM_SHARE, 2*sizeof(qdNamedObjectBase*));
	LPVOID p = ::GlobalLock( h );
	LPBYTE pT = (LPBYTE)p;

	qdNamedObject* data = draggedNode_->getContent().gameObject;
	memcpy( pT , &data, sizeof(DWORD_PTR) );

	qdNamedObject* owner = draggedNodeParent_->getContent().gameObject;
	memcpy( pT + sizeof(qdNamedObject*), &owner, sizeof(qdNamedObject*) );
	::GlobalUnlock( h );

	pSource->CacheGlobalData( clFromatId, h );

	dropTarget_->ResetDropInMe();

	DWORD res = pSource->DoDragDrop();

	getTreeListCtrl().SetTargetItem(NULL);

	return false;
}
//! Уведомляет о том, что тянем курсор над окном
bool TreeLogic::onDragOver(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd)
{
	return true;
}
//! Уведомляет о том, что отжали кнопку мыши
bool TreeLogic::onDrop(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd)
{
	return true;
}

//! Вызывается перед началам редактирования текста.
bool TreeLogic::onBeginLabelEdit(CTreeListCtrl& source, 
							  CHANGE_LABEL_NOTIFY_INFO* pclns)
{
	return getTreeNode(pclns->pItem)->canRename(*this);
}

//! Уведомляет о том, что редактирование закончено
bool TreeLogic::onEndLabelEdit(CTreeListCtrl& source, 
							CHANGE_LABEL_NOTIFY_INFO* pclns)
{
	if (pclns->text.IsEmpty())
		return false;
	if (getTreeNode(pclns->pItem)->rename(*this, pclns->text))
	{
		getProject()->SetModified(true);
		if (isSceneViewActive())
			getSceneView()->Redraw(RM_REDRAW_CONTENT);
		else
			getTriggerView()->Redraw();
		return true;
	}
	return false;
}

void TreeLogic::onDeleteItem(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info)
{
	ITreeNode* node = reinterpret_cast<ITreeNode*>(info->item->GetData());
	node->onDeleteItem(*this);
	delete node;
}

bool TreeLogic::onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info)
{
	if (info->item)
	{
		ITreeNode* node = getTreeNode(info->item);
		if (GetAsyncKeyState(VK_SHIFT)>>15)
			showConditions(node);
		else
			showProperties(node, 0);
	}
	return false;
}

void TreeLogic::onRClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info)
{
	CTreeListItem const* item = tree.GetFocusedItem();
	if (!item)
		return;
	getTreeNode(item)->onRClick(*this);
}

void TreeLogic::reset()
{
	getTreeNodeIndex().clear();
	sceneItemsIndex_.clear();
	tree_->DeleteItem();
}

bool TreeLogic::addTreeNodeIndexEntry(DWORD_PTR key, ITreeNode* value)
{
	getTreeNodeIndex().insert(std::make_pair(key, value));
	return true;
}

void TreeLogic::removeTreeNodeIndexEntry(DWORD_PTR key, ITreeNode* value)
{
	TreeNodeIndex& index = getTreeNodeIndex();
	boost::rtl::irange<TreeNodeIndex::iterator> r(index.equal_range(key));
	for(; r; ++r)
		if ((*r).second == value)
		{
			index.erase(r.begin());
			break;
		}
}

TreeLogic::Nodes const TreeLogic::getObjectNodes(qdNamedObjectBase const* obj) const
{
	return getKeyNodes(reinterpret_cast<DWORD_PTR>(obj));
}

TreeLogic::Nodes const TreeLogic::getKeyNodes(DWORD_PTR key) const
{
	return Nodes(getTreeNodeIndex().equal_range(key));
}

void TreeLogic::setNodeCheck(ITreeNode const* node, bool check)
{
	getTreeListCtrl().SetItemCheck(nodeToItem(node), check, false);
}

void TreeLogic::showNodeCheck(ITreeNode const* node, bool show)
{
	getTreeListCtrl().ShowItemCheckBox(nodeToItem(node), show);
}

ITreeNode* TreeLogic::isNodeUnderScene(ITreeNode const* node) const{
	CTreeListItem const* item = node->getTreeListItem();
	QETreeListCtrl const& tree = getTreeListCtrl();
	do
	{
		ITreeNode* const p = getTreeNode(item);
		if (TreeNodeTraits::isSceneNode(p))
			return p;
	}
	while (item = tree.GetParentItem(item));
	return NULL;
}


/*!
	Под анимированными объектами может быть максимум 2 уровня. Если объекто под 
	анимированным, то либо сам он анимированный, либо его парент анимированный, 
	либо парент парента анимированный
*/
ITreeNode* TreeLogic::isNodeUnderAnimated(ITreeNode const* node,bool *pfIsMoving) const{
	if (pfIsMoving)
		*pfIsMoving = false;
	if (TreeNodeTraits::isAnimated(node))
	{
		if (pfIsMoving)
			*pfIsMoving = ObjectTraits::is_obj_moving(node->getContent().gameObject);
		return const_cast<ITreeNode*>(node);
	}

	node = getParentNode(node);
	if (!node)
		return NULL;
	if (TreeNodeTraits::isAnimated(node))
	{
		if (pfIsMoving)
			*pfIsMoving = ObjectTraits::is_obj_moving(node->getContent().gameObject);
		return const_cast<ITreeNode*>(node);
	}

	node = getParentNode(node);
	if (!node)
		return NULL;
	if (TreeNodeTraits::isAnimated(node))
	{
		if (pfIsMoving)
			*pfIsMoving = ObjectTraits::is_obj_moving(node->getContent().gameObject);
		return const_cast<ITreeNode*>(node);
	}

	return NULL;
}

/*!
 	Находится ли узел под каталогом состояний, если находится, то уточняем тип
	каталога. Несколько каталогов у нас у персонажа. Для него эта функция
	\param hItem	- Узел для теста
	\param pfIsWalk	- Является ли каталог каталогом походк. Может быть NULL
	\return Описатель узла каталога,если найден
*/

ITreeNode* TreeLogic::isNodeUnderStateCatalog(ITreeNode const* node, bool *pfIsWalk) const
{
	if (pfIsWalk)
		*pfIsWalk = false;

	if (TreeNodeTraits::isStaticStateCatalog(node))
		return const_cast<ITreeNode*>(node);
	else if (TreeNodeTraits::isWalkStateCatalog(node))
	{
		if (pfIsWalk)
			*pfIsWalk = true;
		return const_cast<ITreeNode*>(node);
	}

//	QETreeListCtrl const& tree = getTreeListCtrl();
//	CTreeListItem const* item = node->getTreeListItem();

//	item = tree.GetParentItem(item);
//	if (!item)
//		return NULL;

	node = getParentNode(node);
	if (!node)
		return NULL;

	if (TreeNodeTraits::isStaticStateCatalog(node))
		return const_cast<ITreeNode*>(node);
	else if (TreeNodeTraits::isWalkStateCatalog(node))
	{
		if (pfIsWalk)
			*pfIsWalk = true;
		return const_cast<ITreeNode*>(node);
	}
	return NULL;
}

ITreeNode* TreeLogic::isNodeUnderCatalog(ITreeNode const* node, DWORD catalogId)
{
	do {
		if (TreeNodeTraits::isFolderNode(node)) 
		{
			if (node->getContent().folderId == catalogId)
				return const_cast<ITreeNode*>(node);
		}
	} while(node = getParentNode(node));
	return NULL;
}

ITreeNode* TreeLogic::isNodeUnderAnimatedCatalog(ITreeNode const* node)
{
	return isNodeUnderCatalog(node, QD_NAMED_OBJECT_ANIMATED_OBJ);
}
ITreeNode* TreeLogic::isNodeUnderMovingCatalog(ITreeNode const* node)
{
	return isNodeUnderCatalog(node, QD_NAMED_OBJECT_MOVING_OBJ);
}

bool TreeLogic::showProperties(ITreeNode* node, int activePage)
{
	if (node->showProperties(*this, activePage))
	{
		node->updateItemImage(*this);
		getProject()->SetModified();
		return true;
	}
	return false;
}

bool TreeLogic::showConditions(ITreeNode* node)
{
	if (node->showConditions(*this))
	{
		node->updateItemImage(*this);
		getProject()->SetModified();
		return true;
	}
	return false;
}

//! После того, как развернули
void TreeLogic::onItemExpanded(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci)
{
	if (pci->uOldState&TLIS_EXPANDED&&getFocusedNode())
	{
		if (getTreeListCtrl().isChildItem(nodeToItem(getFocusedNode()), pci->item))
			setFocusedNode(itemToNode(pci->item));
	}
}
//! Перед изменением состояния
bool TreeLogic::onItemStateChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci)
{
	return true;
}

void TreeLogic::onItemStateChanged(ITreeNode* node, DWORD oldState, DWORD newState)
{
	bool bNeedRedraw = false;
	if (!(newState&NS_FOCUSED)&&!(oldState&NS_FOCUSED))
		return;

	if (newState&NS_FOCUSED&&!(oldState&NS_FOCUSED))
	{
		getTreeListCtrl().Invalidate();
		ITreeNode* const oldSceneNode = getCurrentSceneNode();
		setCurrentSceneNode(isNodeUnderScene(node));
		ITreeNode* const curSceneNode = getCurrentSceneNode();
		if (curSceneNode && getProject()->isSceneViewActive())
		{
			bNeedRedraw = selectScene(getCurrentSceneNode());
			if (oldSceneNode != getCurrentSceneNode())
			{
				changeSceneNodesExpanding(curSceneNode);
			}
		}
	}
	bNeedRedraw = node->onStateChange(*this, oldState, newState);
	if (newState&(NS_FOCUSED|NS_SELECTED)&&bNeedRedraw)
		sceneView_->Redraw(RM_REDRAW_CONTENT);
}

//! После изменения состояния
void TreeLogic::onItemStateChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci)
{
	DWORD newNodeState = itemStateToNodeState(pci->uNewState);
	DWORD oldNodeState = itemStateToNodeState(pci->uOldState);
	onItemStateChanged(getTreeNode(pci->item), oldNodeState, newNodeState);
}
//! Если есть стиль	TLC_CHECKBOX, то приходит после изменения состояния
void TreeLogic::onItemCheckChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci)
{
	if (loadingTree_)
		return;
	ASSERT(pci->item);
	if (getTreeNode(pci->item)->onCheckNode(*this, pci->uNewState&TLIS_CHECKED))
	{
		if (isSceneViewActive())
			sceneView_->Redraw(RM_REDRAW_CONTENT);
	}
}

bool TreeLogic::selectScene(ITreeNode* sceneNode)
{
	return getProject()->SelectScene(sceneNode->getContent().gameObject);
}

void TreeLogic::changeSceneNodesExpanding(ITreeNode const* exclude)
{
	CQuestEditorApp* pmyapp = GetMyApp();
	int bSettingVal = pmyapp->GetIntSetting(SETTING_NAME(PT_COLLAPSE_SCENES));
	if (!bSettingVal) 
		return;
	
	getProject()->CollapseSceneNodes(exclude->getContent().gameObject);
	getTreeListCtrl().Expand(exclude->getTreeListItem(), TLE_EXPAND);
}

void TreeLogic::endEditStateDims(bool bSave)
{
	getProject()->EndResizeState(bSave? EESTM_OK : EESTM_CANCEL);
}

void TreeLogic::showAnimation(qdGameObjectState* state)
{
	getSceneView()->StartAnimation(state);
}

void TreeLogic::stopAnimation(qdGameObjectState* state)
{
	getSceneView()->StopAnimation(state);
}

void TreeLogic::playStopVideo(qdVideo* video)
{
	getSceneView()->PlayStopVideo(video);
}
void TreeLogic::stopVideo(qdVideo* video)
{
	getSceneView()->StopVideo(video);
}

//! Выставляет для объектов из цепочки иконку активного триггера
void TreeLogic::setActiveTriggerIcon(qdTriggerChain const* chain, BOOL bSet)
{
	getProject()->setActiveTriggerIcon(chain, bSet);
}

void TreeLogic::setInTriggerIcon(ITreeNode* node, eTriggerIconCommand command)
{
	QETreeListCtrl& tree = getTreeListCtrl();
	ObjectStateTreeNode* state_node;
	switch(command) {
	case TIC_REMOVE:		
		tree.SetItemStateImage(node->getTreeListItem(), 0);
		state_node = dynamic_cast<ObjectStateTreeNode*>(node);
		if (state_node)
			state_node->resetDefaultStateIcon(*this);
		break;
	case TIC_SET_NOT_ACTIVE:
		tree.SetItemStateImage(node->getTreeListItem(), II_NOT_ACTIVE_TRIGGER);
		break;
	case TIC_SET_ACTIVE:
		tree.SetItemStateImage(node->getTreeListItem(), II_ACTIVE_TRIGGER);
		break;
	}
}

bool TreeLogic::selectInterfaceScreen(LPCTSTR screenName)
{
	return getProject()->selectInterfaceScreen(screenName);
}

ITreeNode* TreeLogic::getSubNodeByName(ITreeNode* parent, LPCTSTR subNodeText) const
{
	CTreeListItem *item = 
		getTreeListCtrl().getSubItemByText(nodeToItem(parent), subNodeText, 0);
	if (item)
		return getTreeNode(item);
	return NULL;
}

bool TreeLogic::showNode(ITreeNode* node) const
{
	getTreeListCtrl().SelectItem(node->getTreeListItem());
	return getTreeListCtrl().EnsureVisible(node->getTreeListItem(), 0);
}

ITreeNode* TreeLogic::getSubfolder(ITreeNode const* parent, DWORD_PTR type) const
{
	boost::rtl::irange<NodeIterator> ir(getKeyNodes(type));
	CTreeListItem* const parentItem = nodeToItem(parent);
	for(; ir; ++ir)
	{
		if (parentItem == getTreeListCtrl().GetParentItem((*ir)->getTreeListItem()))
			return *ir;
	}
	return NULL;
}

void TreeLogic::removeSubnodes(ITreeNode* node) const
{
	CTreeListItem* parent = node->getTreeListItem();
	CTreeListItem* child = getTreeListCtrl().GetChildItem(parent);
	if (!child)
		return;
	do {
		CTreeListItem* del = child;
		child = getTreeListCtrl().GetNextSiblingItem(child);
		getTreeListCtrl().DeleteItem(del);
	} while(child);
}

bool TreeLogic::isExpanded(ITreeNode const* node) const{
	return (getTreeListCtrl().GetItemState(nodeToItem(node), TLIS_EXPANDED) != 0);
}

void TreeLogic::expand(ITreeNode* node, eExpandCommand command) const{
	switch(command) {
	case EC_EXPAND:
		getTreeListCtrl().Expand(node->getTreeListItem(), TLE_EXPAND);
		break;
	case EC_COLLAPSE:
		getTreeListCtrl().Expand(node->getTreeListItem(), TLE_COLLAPSE);
		break;
	case EC_TOGGLE:
		getTreeListCtrl().Expand(node->getTreeListItem(), TLE_TOGGLE);
		break;
	}

}

//! Рекурсивная часть для expandNodeAndChilds
void TreeLogic::expandRecur(ITreeNode* child, eExpandCommand command)
{
	while (child)
	{
		// Сначала сворачиваем/раскрываем детей
		expandRecur(getChildNode(child), command);
		// Потом текущего
		expand(child, command);		
		child = getNextSiblingNode(child);
	}
}

void TreeLogic::expandNodeAndChilds(ITreeNode* node, eExpandCommand command)
{
	if (node)
	{
		// Сворачиваем/раскрываем детей
		expandRecur(getChildNode(node), command);
		// Сворачиваем/раскрываем сам узел
		expand(node, command);
	}
}

ITreeNode* TreeLogic::getParentNode(ITreeNode const* child) const
{
	CTreeListItem* item = getTreeListCtrl().GetParentItem(child->getTreeListItem());
	if (!item)
		return NULL;
	return itemToNode(item);
}

ITreeNode* TreeLogic::getChildNode(ITreeNode const* parent) const
{
	CTreeListItem* item = getTreeListCtrl().GetChildItem(nodeToItem(parent));
	if (!item)
		return NULL;
	return itemToNode(item);
}

void TreeLogic::deleteNode(ITreeNode* node)
{
	CTreeListItem* item = node->getTreeListItem();
	getTreeListCtrl().DeleteItem(item);
}

ITreeNode* TreeLogic::getPrevVisibleNode(ITreeNode* node) const
{
	CTreeListItem* item = node->getTreeListItem();
	item = getTreeListCtrl().GetPrevVisibleItem(item);
	if (!item)
		return NULL;
	return getTreeNode(item);
}

DROPEFFECT TreeLogic::canDrop(const CPoint& point)
{
	QETreeListCtrl& tree = getTreeListCtrl();
	int flags = 0;
	CTreeListItem* overItem = tree.HitTest(point,&flags);
	if (overItem&&flags&TLHT_ONITEM) 
	{
		return draggedNode_->canDrop(*this, getTreeNode(overItem));
	}
	return DROPEFFECT_NONE;
}

/*!
 	Достает из данных объект, который тащили и в зависимисти от типа объекта 
	вызываем одну из функции с префиксом odd...()
*/
BOOL TreeLogic::OnDropData(void* pData, DROPEFFECT dropEffect, 
							   const CPoint& point)
{
	ITreeNode* dropTargetNode = getDropTarget();
	if (!dropTargetNode)
		return FALSE;
	CWaitCursor wait;
	DROPEFFECT res = draggedNode_->onDropData(*this, dropTargetNode, dropEffect);
	if (res != DROPEFFECT_NONE)
		getProject()->SetModified(true);
	if (res == DROPEFFECT_MOVE) 
		deleteNode(draggedNode_);
	draggedNode_ = NULL;
	draggedNodeParent_ = NULL;
/*
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hDropTarget = tree.GetDropHilightItem();
	if(!hDropTarget)
		return FALSE;

	qdNamedObject* pno = *static_cast<qdNamedObject**>(pData);
	switch(pno->named_object_type()) {
	case QD_NAMED_OBJECT_OBJ_STATE:
		oddOwnState(static_cast<qdGameObjectState*>(pno),dropEffect, point);
		break;
	case QD_NAMED_OBJECT_MOVING_OBJ:
		if (ptree_helpers::IsGlobalObj(pno))
			oddGlobalObject(static_cast<qdGameObject*>(pno), dropEffect, point);
		else
			oddMovingObject(static_cast<qdGameObjectMoving*>(pno), dropEffect, point);
		break;
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
		oddAnimatedObject(static_cast<qdGameObjectAnimated*>(pno), dropEffect, point);
		break;
	case QD_NAMED_OBJECT_GRID_ZONE:
		oddGridZone(static_cast<qdGridZone*>(pno), dropEffect, point);
		break;
	case QD_NAMED_OBJECT_TRIGGER_CHAIN:
		oddTriggerChain(static_cast<qdTriggerChain*>(pno), dropEffect, point);
		break;
	case QD_NAMED_OBJECT_VIDEO:
		oddVideo(static_cast<qdVideo*>(pno), dropEffect, point);
		break;
	case QD_NAMED_OBJECT_INVENTORY:
		oddInventory(static_cast<qdInventory*>(pno), dropEffect, point);
		break;
	}
	tree.SelectDropTarget(NULL);
*/
	selectDropTarget(NULL);
	return TRUE;
}

ITreeNode* TreeLogic::getDropTarget() const{
	CTreeListItem* dropTarget = getTreeListCtrl().GetDropTargetItem();
	if (dropTarget)
		return getTreeNode(dropTarget);
	else
		return NULL;
}
void TreeLogic::selectDropTarget(ITreeNode const* node){
	if (node) 
		getTreeListCtrl().SetTargetItem(node->getTreeListItem());
	else
		getTreeListCtrl().SetTargetItem(NULL);
}


//! Перенаправляет вызов в Project::copyAnimatedToAnimated
bool TreeLogic::copyAnimatedToAnimated(ITreeNode* dropTarget, 
							qdGameObjectAnimated const* source,
							qdGameScene* scene)
{
	return project_->copyAnimatedToAnimated(dropTarget, source, scene);
}
//! Перенаправляет вызов в Project::copyAnimatedToMoving
bool TreeLogic::copyAnimatedToMoving(ITreeNode* dropTarget, 
						  qdGameObjectAnimated const* source,
						  qdGameScene* scene)
{
	return project_->copyAnimatedToMoving(dropTarget, source, scene);
}

//! Перенаправляет вызов в Project::copyMovingToMoving
bool TreeLogic::copyMovingToMoving(ITreeNode* dropTarget, 
						qdGameObjectMoving const* source,
						qdGameScene* scene)
{
	return project_->copyMovingToMoving(dropTarget, source, scene);
}

qdGameObjectState* TreeLogic::moveState(qdGameObjectAnimated* source, 
			   qdGameObjectAnimated* target,
			   qdGameObjectState* movedState, 
			   ITreeNode* targetNode,
			   ITreeNode* insertAfter,
			   int insertBefore)
{
	return project_->moveState(source, target, movedState, 
							targetNode, insertAfter, insertBefore);
}

qdGameObjectState* TreeLogic::copyState(qdGameObjectAnimated* source, 
			   qdGameObjectAnimated* target,
			   qdGameObjectState* copyState, 
			   ITreeNode* targetNode,
			   ITreeNode* insertAfter,
			   int insertBefore)
{
	return project_->copyState(source, target, copyState, 
		targetNode, insertAfter, insertBefore);
}

ITreeNode* TreeLogic::getPrevSiblingNode(ITreeNode const* node)
{
	QETreeListCtrl& tree = getTreeListCtrl();
	CTreeListItem* prev = tree.GetPrevSiblingItem(node->getTreeListItem());
	if (!prev)
		return NULL;
	return getTreeNode(prev);
}

ITreeNode* TreeLogic::getNextSiblingNode(ITreeNode const* node)
{
	QETreeListCtrl& tree = getTreeListCtrl();
	CTreeListItem* next = tree.GetNextSiblingItem(node->getTreeListItem());
	if (!next)
		return NULL;
	return getTreeNode(next);
}

qdGridZone* TreeLogic::moveZone(qdGameScene* scene, 
					 qdGridZone* movedZone, 
					 qdGridZone const* zoneBefore,
					 ITreeNode* nodeAfter,
					 ITreeNode* parent)
{
	return project_->moveZone(scene, movedZone, zoneBefore, nodeAfter, parent);
}

qdVideo* TreeLogic::moveVideo(qdVideo* source, qdVideo const* before, 
				   ITreeNode* insertAfter, ITreeNode* parent)
{
	return project_->moveVideo(source, before, insertAfter, parent);
}

//! Перенаправляет вызов в Project::copyInventory
qdInventory* TreeLogic::copyInventory(qdInventory const* source, ITreeNode* parent)
{
	return project_->copyInventory(source, parent);
}

void TreeLogic::setNodeText(ITreeNode* node, LPCTSTR text) const{
	getTreeListCtrl().SetItemText(node->getTreeListItem(), 0, text);
}

CString TreeLogic::getNodeText(ITreeNode* node) const{
	return getTreeListCtrl().GetItemText(nodeToItem(node));
}

/* Функции этого кода теперь выполняет TreeLogic::saveFullStruct(...) и TreeLogic::loadFullStruct(...)
void TreeLogic::loadExpandData(std::istream& in)
{
	if (!in||in.eof())
		return;
	QETreeListCtrl& tree = getTreeListCtrl();
	ExpandDataLoader expandDataLoader(in, tree);
	CTreeListItem* pFirstItem = tree.GetChildItem(tree.GetChildItem(TLI_ROOT));
	do {
		if (!expandDataLoader(pFirstItem))
			return;
		if (!in)
			return;
	} while(pFirstItem = tree.GetNextSiblingItem(pFirstItem));
}

void TreeLogic::saveExpandData(std::ostream& out)
{
	if (!out)
		return;

	QETreeListCtrl& tree = getTreeListCtrl();

	CTreeListItem* pFirstItem = tree.GetChildItem(tree.GetChildItem(TLI_ROOT));
	ExpandDataSaver expandDataSaver(out);
	if (!pFirstItem)
		return;
	do {
		if (!out)
			return;
		if (!expandDataSaver(pFirstItem))
			return;
	} while(pFirstItem = tree.GetNextSiblingItem(pFirstItem));
}

void TreeLogic::saveIni(IProjectSettings* projectSettings)
{
	CString section((LPTSTR)IDS_INI_SEC_NAME_TREE);
	projectSettings->ClearSection(section);	

	std::stringstream strm;
	saveExpandData(strm);

	CString expandKey;
	expandKey.LoadString(IDS_EXPAND_KEY);
	projectSettings->WriteStrSetting(section, expandKey, strm.str().c_str()); 

	saveFocused(projectSettings);
}

void TreeLogic::readIni(IProjectSettings* projectSettings)
{
	CString section((LPTSTR)IDS_INI_SEC_NAME_TREE);

	CString expandKey;
	expandKey.LoadString(IDS_EXPAND_KEY);
	std::string buff = projectSettings->GetStrSetting(section, expandKey, _T("0")); 

	std::stringstream strm(buff);
	loadExpandData(strm);
	loadFocused(projectSettings);
}
*/
//! Загружает узел, имевший фокус
void TreeLogic::loadFocused(IProjectSettings* projectSettings)
{
	CString section((LPTSTR)IDS_INI_SEC_NAME_TREE);
	CString key;
	key.LoadString(IDS_INI_KEY_NAME_FOCUSED_NODE);
	CString defaultValue;
	defaultValue.LoadString(IDS_GLOBAL_NODE);
	std::string str = projectSettings->GetStrSetting(section, key, defaultValue);

	typedef boost::tokenizer<boost::char_separator<TCHAR> > Tokenizer;
	boost::char_separator<TCHAR> sep(_T("\\"));
	Tokenizer t(str, sep);
	boost::rtl::crange<Tokenizer const> r(t);
	
	ITreeNode* node = TN_ROOT;
	for (; r; ++r)
	{
		node = getSubNodeByName(node, (*r).c_str());
		if (!node)
			return;
	}
	setFocusedNode(node);
	getTreeListCtrl().EnsureVisible(nodeToItem(node), 0);
}

//! Сохраняет узел, имеющий фокус
void TreeLogic::saveFocused(IProjectSettings* projectSettings)
{
	CString section((LPTSTR)IDS_INI_SEC_NAME_TREE);
	CString key;
	key.LoadString(IDS_INI_KEY_NAME_FOCUSED_NODE);
	CString defaultValue;
	defaultValue.LoadString(IDS_GLOBAL_NODE);

	ITreeNode* node = getFocusedNode();
	if (!node)
	{
		projectSettings->WriteStrSetting(section, key, defaultValue);
	}
	else
	{
		CString value = getNodeText(node);
		while (node = getParentNode(node))
		{
			value = _T("\\") + value;
			value = getNodeText(node) + value;
		} 
		projectSettings->WriteStrSetting(section, key, value);
	}
}

CTreeListItem* TreeLogic::nodeToItem(ITreeNode const *node)
{
	if (node == TN_INSERT_SORT)
		return TLI_SORT;
	if (node == TN_INSERT_LAST)
		return TLI_LAST;
	if (node == TN_INSERT_FIRST)
		return TLI_FIRST;
	if (node == TN_ROOT)
		return TLI_ROOT;
	return node->getTreeListItem();
}

ITreeNode* TreeLogic::itemToNode(CTreeListItem* item)
{
	if (!item)
		return NULL;
	if (item == TLI_SORT)
		return TN_INSERT_SORT;
	if (item == TLI_LAST)
		return TN_INSERT_LAST;
	if (item == TLI_FIRST)
		return TN_INSERT_FIRST;
	if (item == TLI_ROOT)
		return TN_ROOT;
	return reinterpret_cast<ITreeNode*>(item->GetData());
}

