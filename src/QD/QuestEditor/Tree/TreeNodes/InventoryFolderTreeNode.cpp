#include "stdafx.h"
#include "resource.h"
#include ".\inventoryfoldertreenode.h"

#include "ISceneViewFacade.h"

#include "InvetoryPropPage.h"

#include "menu_helpers.h"

#include "ptree_consts.h"

InventoryFolderTreeNode::InventoryFolderTreeNode(void)
: FolderTreeNode(QD_NAMED_OBJECT_INVENTORY)
{
}

InventoryFolderTreeNode::~InventoryFolderTreeNode(void)
{
}

bool InventoryFolderTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic, 
								   ITreeNode * parent, qdNamedObjectBase const* owner,
								   ITreeNode* insertAfter)
{
	CString str;
	str.LoadString(IDS_INVENTORY_OBJS_NODE);
	CTreeListItem* item = insertToTree(treeLogic,
									str, parent, 
									II_INVENTORY_OBJS_NODE, 
									getFolderId(), insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD InventoryFolderTreeNode::getMenuResId() const{
	return IDR_INVENTORY_OBJS_NODE;
}

void InventoryFolderTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
	if(!treeLogic.isSceneViewActive())
		menu_h::disableMenuItem(pm, IDC_CRT_NEW_INVENTORY);
}

bool InventoryFolderTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<InventoryPropPage> ptrPage(new InventoryPropPage);
	ptrPage->set_data(logic.getGameDispatcher(), NULL);
	ptrPages[0] = ptrPage.get();
	return showPropDlg(ptrPages, iActive, IDS_PPCAP_INVENTORY_PROPERTIES);
}

bool InventoryFolderTreeNode::onStateChange(TreeLogic& logic, 
											DWORD oldState, 
											DWORD newState)
{
	if (!logic.isSceneViewActive())
		return false;
	ISceneViewFacade* sceneView = logic.getSceneView();
	if (newState&NS_FOCUSED)
	{
		sceneView->SetViewState(VS_NONE);
		return true;
	}
	return false;
}
