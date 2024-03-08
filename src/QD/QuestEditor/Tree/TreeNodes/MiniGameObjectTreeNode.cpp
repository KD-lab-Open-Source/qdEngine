#include "stdafx.h"
#include "resource.h"
#include ".\minigameobjecttreenode.h"

#include "ISceneViewFacade.h"

#include "MinigamePropPage.h"
#include "name_op.h"
#include "ptree_consts.h"
#include "qd_minigame.h"

MiniGameObjectTreeNode::MiniGameObjectTreeNode(qdNamedObject* object)
: NamedObjectTreeNode(object)
{
}

MiniGameObjectTreeNode::~MiniGameObjectTreeNode(void)
{
}

bool MiniGameObjectTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
								  ITreeNode * parent, qdNamedObjectBase const* owner,
								  ITreeNode* insertAfter)
{
	CTreeListItem* const item = insertToTree(treeLogic,
											getObject()->name(),
											parent,
											II_MINIGAME_NODE,
											reinterpret_cast<DWORD_PTR>(getObject()), 
											insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD MiniGameObjectTreeNode::getMenuResId() const{
	return IDR_MINIGAME_OBJ_NODE;
}

bool MiniGameObjectTreeNode::onStateChange(TreeLogic& logic, 
										   DWORD oldState, 
										   DWORD newState)
{
	if (!getObject())
		return false;
	if (newState&NS_FOCUSED) {
		logic.getSceneView()->SetViewState(VS_NONE);
		return true;
	} 
	return false;
}

bool MiniGameObjectTreeNode::showProperties(TreeLogic& logic, int activePage)
{
	CPropertyPage* ptrPages[] = {0, 0, };
	std::auto_ptr<MinigamePropPage> ptrPage(new MinigamePropPage);
	ptrPage->set_data(static_cast<qdMiniGame*>(getObject()), logic.getGameDispatcher());
	ptrPages[0] = ptrPage.get();
	return showPropDlg(ptrPages, activePage, IDS_PPCAP_MINIGAME_PROPERTIES);
}

bool MiniGameObjectTreeNode::canRename(TreeLogic& logic)
{
	return true;
}

bool MiniGameObjectTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return gameDisp->rename_minigame(static_cast<qdMiniGame*>(getObject()), newName);
}

bool MiniGameObjectTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName)
{
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return uniq_name(newName, gameDisp->minigame_list());
}
