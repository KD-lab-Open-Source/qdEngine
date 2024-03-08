#include "stdafx.h"
#include "resource.h"
#include ".\inventoryobjecttreenode.h"

#include "InvetoryPropPage.h"
#include "ISceneViewFacade.h"

#include "menu_helpers.h"
#include "ptree_consts.h"
#include "Main/ObjectTraits.h"
#include "Main/Utils/EnumInventoryUsers.h"
#include "Main/Utils/InventorySetter.h"

#include "main/Winhelp.h"
#include "Tree/NodeTraits.h"

#include "name_op.h"

namespace {
/*!
	bAsk = true, то предупреждает пользователя, что инвентори используется.
	\param bAsk			- предупреждать или нет пользователя
	\param pInv			- удаляемый инвентори
	\param plist4Remove	- список объектов, где инвентори используется
	\return \a true - если можно удалить
*/
bool canRemoveInventory(qdGameDispatcher* gameDisp, 
						qdInventory* pInv, qdGameObjectList* plist4Remove)
{
	CString msg;
	msg.Format(IDS_DEL_INVENTORY_USED_REMOVE, pInv->name());
	if (AfxMessageBox(msg, MB_OKCANCEL) == IDOK) 
		return true;
	return false;
}

//! Переименование инвентори
bool renameInventory(qdGameDispatcher* disp, qdInventory* inventory, LPCTSTR lpszNewName)
{
	qdGameObjectList lst;
	EnumInventoryUsers::enumerate(disp, inventory, &lst);

	if (!lst.empty())
	{
		CString str;
		str.Format(IDS_ASK_FOR_RENAME_INVENTORY, inventory->name());
		switch (AfxMessageBox(str, MB_YESNOCANCEL))
		{
		case IDYES:
			{
				if (disp->rename_inventory(inventory, lpszNewName))
				{
					boost::rtl::rng::for_each(lst, InventorySetter(lpszNewName));
					return true;
				}
				return false;
			}
		case IDCANCEL:
			return false;
		}
	}
	return disp->rename_inventory(inventory, lpszNewName);
}

}
InventoryObjectTreeNode::InventoryObjectTreeNode(qdNamedObject* object)
: NamedObjectTreeNode(object)
{
}

InventoryObjectTreeNode::~InventoryObjectTreeNode(void)
{
}

bool InventoryObjectTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
								   ITreeNode * parent, qdNamedObjectBase const* owner,
								   ITreeNode* insertAfter)
{
	CTreeListItem* item = insertToTree(treeLogic,
								getObject()->name(),
								parent,
								II_INVENTORY_OBJ_NODE,
								reinterpret_cast<DWORD_PTR>(getObject()), 
								insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD InventoryObjectTreeNode::getMenuResId() const{
	return IDR_INVENTORY_OBJ_NODE;
}

void InventoryObjectTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
	qdInventory const* pi = static_cast<qdInventory const*>(getObject());
	if (pi->selected_cellset_num() == -1)
		menu_h::disableMenuItem(pm, IDC_REMOVE_CELL_SET);
	else
		menu_h::disableMenuItem(pm, IDC_REMOVE);
}

bool InventoryObjectTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<InventoryPropPage> ptrPage(new InventoryPropPage);
	ptrPage->set_data(logic.getGameDispatcher(), static_cast<qdInventory*>(getObject()));
	ptrPages[0] = ptrPage.get();
	return showPropDlg(ptrPages, iActive, IDS_PPCAP_INVENTORY_PROPERTIES);
}

bool InventoryObjectTreeNode::onStateChange(TreeLogic& logic, 
											DWORD oldState, 
											DWORD newState)
{
	if (!getObject())
		return false;
	ISceneViewFacade* sceneView = logic.getSceneView();
	if (newState&NS_FOCUSED)
	{
		sceneView->SetViewState(VS_EDIT_INVENTORY);
		sceneView->SelectInventory(static_cast<qdInventory*>(getObject()));
		return true;
	}
	else if (oldState&NS_FOCUSED)
	{
		sceneView->SelectInventory(NULL);
	}
	return false;
}

bool InventoryObjectTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!preDelete(logic, ask, statDepends))
		return false;

	if (removeInventory(logic, static_cast<qdInventory*>(getObject()), ask))
	{
		ITreeNode* prevVisible = logic.getPrevVisibleNode(this);
		delete getObject();
//		setObject(NULL);
		logic.deleteNode(this);
		logic.setFocusedNode(prevVisible);
		return true;
	}
	return false;
}

/*!
 	Ищет, где используется инвентори. В 
	объектах, где используется, сбрасывается. Удаляется из списка инвентори
	(qdGameDispatcher::remove_inventory())
	\param pInv			- удаляемый инвентори
	\param bAsk			- предупреждать или нет пользователя
	\return	\a true - если удалили.
	\remarks Физичиеского удаления объекта не происходит
*/
bool InventoryObjectTreeNode::removeInventory(TreeLogic& logic, 
											  qdInventory* pInv, 
											  bool bAsk)
{
	qdGameObjectList lst_obj;
	qdGameDispatcher *gameDisp = logic.getGameDispatcher();
	EnumInventoryUsers::enumerate(gameDisp, pInv, &lst_obj);
	if (lst_obj.empty()||!bAsk||canRemoveInventory(gameDisp, pInv, &lst_obj))
	{
		boost::rtl::rng::for_each(lst_obj, InventorySetter());
        return gameDisp->remove_inventory(pInv);
	}
	return false;
}

bool InventoryObjectTreeNode::canDrag(TreeLogic& logic){
	return true;
}

DROPEFFECT InventoryObjectTreeNode::canDrop(TreeLogic& logic, ITreeNode* overNode)
{
	if (win_h::keyPressed(VK_CONTROL))
	{
		if (TreeNodeTraits::isInventoryCatalog(overNode))
			logic.selectDropTarget(overNode);
		else if (TreeNodeTraits::isInventoryCatalog(logic.getParentNode(overNode)))
		{
			logic.selectDropTarget(logic.getParentNode(overNode));
		}
		return DROPEFFECT_COPY;
	}
	logic.selectDropTarget(NULL);
	return DROPEFFECT_NONE;
}

DROPEFFECT InventoryObjectTreeNode::onDropData(TreeLogic& logic, 
							  ITreeNode* dropTarget, 
							  DROPEFFECT dropEffect)
{
	if (!win_h::keyPressed(VK_CONTROL))
		return DROPEFFECT_NONE;
	logic.selectDropTarget(NULL);

	if (logic.copyInventory(static_cast<qdInventory*>(getObject()), dropTarget))
		return DROPEFFECT_COPY;

	return DROPEFFECT_NONE;
}

bool InventoryObjectTreeNode::canRename(TreeLogic& logic)
{
	return true;
}

bool InventoryObjectTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return renameInventory(gameDisp, static_cast<qdInventory*>(getObject()), newName);
}

bool InventoryObjectTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName)
{
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return uniq_name(newName, gameDisp->inventory_list());
}
