/********************************************************************
created:	2002/09/11
created:	11:9:2002   12:20
filename: 	D:\Projects\Quest\QuestEditor\ptree_menu.cpp
file path:	D:\Projects\Quest\QuestEditor
file base:	ptree_menu
file ext:	cpp
Powerd by:	�����

  purpose:	������� ������ � ����������� ����, � ��� �� ������� ��� �
  ������� ���� ���������
*********************************************************************/

#include "stdafx.h"
#include "QuestEditor.h"

#include "qd_named_object.h"
#include "qd_music_track.h"
#include "qd_game_object_static.h"
#include "qd_game_object_animated.h"
#include "qd_game_object_moving.h"
#include "qd_game_object_state.h"
#include "qd_game_scene.h"
#include "qd_inventory.h"
#include "qd_game_dispatcher.h"

#include "scenetreeview.h"
#include "ptree_hlprs.hpp"
#include "ptree_loading.h"
#include "mainfrm.h"
#include "menu_helpers.h"

#include "MovementTypeManager.h"

/************************************************************************/
/* ���������� �������                                                   */
/************************************************************************/
namespace{
//! �� ��������� ������ ���� ������� ��� �������
int FindMenuItem(CMenu* Menu, LPCTSTR MenuString)
{
	ASSERT(Menu);
	ASSERT(::IsMenu(Menu->GetSafeHmenu()));
	
	int count = Menu->GetMenuItemCount();
	for (int i = 0; i < count; i++)
	{
		CString str;
		if (Menu->GetMenuString(i, str, MF_BYPOSITION) &&
			(strcmp(str, MenuString) == 0))
			return i;
	}
	
	return -1;
}
//! ��������� ������ ���� ��������� � ��������� � ���� ���������
void disableAnimationMenuItems(CMenu* pm)
{
	menu_h::disableMenuItem(pm, IDC_STATE_DIMS);
	menu_h::disableMenuItem(pm, IDC_STATE_HAS_BOUND);
	menu_h::disableMenuItem(pm, IDC_BTN_AUTO_BND);
	menu_h::disableMenuItem(pm, IDC_STACK_STATE);
	menu_h::disableMenuItem(pm, IDC_COORDS_ANIMATION);
//	menu_h::disableMenuItem(pm, IDC_MAKE_BOUND_AS_FIRST);
}

/*!
 	������������ ��������� ������ ���� IDC_SELECT_MOVEMENT_TYPE ���� ���������, 
	�������� � ���� �������� �������� ���� ��������.
*/
void ModifySelectMovementCaption(CMenu *pm, qdGameObjectStateWalk const* pstate)
{
	CString str;
	pm->GetMenuString(IDC_SELECT_MOVEMENT_TYPE, str, MF_BYCOMMAND);
	MovementTypeManager mgr;
	str += _T(" - ");
	str += mgr[pstate->movement_type()]->description();
	MENUITEMINFO mi;
	mi.cbSize = sizeof (MENUITEMINFO); // must fill up this field
	mi.fMask = MIIM_STATE;             // get the state of the menu item
	pm->GetMenuItemInfo(IDC_SELECT_MOVEMENT_TYPE, &mi);

	pm->ModifyMenu(IDC_SELECT_MOVEMENT_TYPE, MF_BYCOMMAND|MF_STRING,
					IDC_SELECT_MOVEMENT_TYPE, str);
	pm->EnableMenuItem(IDC_SELECT_MOVEMENT_TYPE,  mi.fState|MF_BYCOMMAND);
}

void DisableAllMenuItems(CMenu* pm)
{
	int count = pm->GetMenuItemCount();
	for(int i = 0; i < count; ++i)
		pm->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
}

}
//////////////////////////////////////////////////////////////////////////

/*!
 	������� ���� � ������, ��������������� ����� �������. � ������� ����������
	SceneTreeView::ShowCorrespondingMenu(HTREEITEM hItem, bool bIsCatalog,
	qdNamedObjectBase const* pno, bool bIsGlobal).
*/
void SceneTreeView::ShowCorrespondingMenu(const qdNamedObjectBase* pobj)
{
	ShowCorrespondingMenu(FindObjectItem(pobj), false, pobj, false);
}

/*!
 	���������� ����������� ����, ������ �� ���������� ����������.
	���������� ������ �� ���� ������� ������������� ����
	(ptree_helpers::GetCorrspondingMenuResID()(������������ ���)).
	������������ ����(�������, ��������� ������, ������ ������) ���, ����� ��� 
	��������������� �������� ��������� ���������(ModifyMenu()). � ���������� ���.

	\param hItem		- ����, ��� �������� ���� �������� ����
	\param bIsCatalog	- �������� �� ���� ����-���������
	\param pno			- ������, ������� ��������� � ���� \a hItem
	\param bIsGlobal	- �������� �� ������ ����������
	\remarks ������ ��������������� ����(�� ����� �������� UpdateCommandUI(...) ����
	����������� �� �� ������. ���������, ��� ���� TrackPopupMenu(...) ���������� 
	� �������� ����, ����������� ���������, �� Main Frame, �� UpdateCommandUI ������ ��
	��������. ���������� ���, ��� ����� ����, ��� ���� �������� ����� ���� ��� ������
	�����������. �� � ����� � ������, ��� ��� ����� �������� �������. ������ ��� ��� 
	�������� � ����� ����� � ���� ���
*/
void SceneTreeView::ShowCorrespondingMenu(HTREEITEM hItem, bool bIsCatalog,
										qdNamedObjectBase const* pno, bool bIsGlobal)
{

	DWORD dwMenuResID = -1;
	const qdInterfaceObjectBase* piob = NULL;
	if (!bIsCatalog&&(piob = dynamic_cast<const qdInterfaceObjectBase*>(pno)))
	{
		dwMenuResID = ptree_helpers::GetCorrspondingMenuResID(piob);
	}
	else
	{
		qdNamedObject const* const pobj = static_cast<const qdNamedObject*>(pno);

		qdNamedObjectType type = (bIsCatalog ? 
						static_cast<qdNamedObjectType>((int)(pobj)) : 
						static_cast<qdNamedObjectType>(pobj->named_object_type()));
		dwMenuResID = ptree_helpers::GetCorrspondingMenuResID(bIsCatalog, type, bIsGlobal);
	}
	
	if(dwMenuResID != -1)
	{
		CMenu menu;
		menu.LoadMenu(dwMenuResID);
		CPoint point;
		GetCursorPos(&point);
		CMenu *pm = menu.GetSubMenu(0);
		
		ModifyMenu(pno, hItem, dwMenuResID, pm);
		pm->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
	}
}

/*!
 	������� �� ���� ������. ���������� �������� �� ���� ���� ��������� � �������� 
	���������� SceneTreeView::ShowCorrespondingMenu(HTREEITEM hItem, bool bIsCatalog,
	qdNamedObjectBase const* pno, bool bIsGlobal)
	\param hItem - ����, ��� �������� ���� �������� ����
*/
void SceneTreeView::ShowCorrespondingMenu(HTREEITEM hItem)
{
	CTreeCtrl& tree = GetTreeCtrl();
	DWORD_PTR dwData = tree.GetItemData(hItem);
	qdNamedObjectBase * pno = reinterpret_cast<qdNamedObjectBase*>(dwData);
	
	//���� ��� ������ ���������, ������ ������ � ����,
	//������� ���� ����������������� ����:). ��.: "�����", "�������" � �.�.
	bool bIsObj = ptree_helpers::is_obj(dwData);
	
	ShowCorrespondingMenu(hItem, !bIsObj, pno, bIsObj&&ptree_helpers::IsGlobalObj(pno));
}

/*!
 	� ����������� �� \a dwMenuId ���������� ���� �� ������� � ��������� "mm"
	\param pobj		- ������ ���������� � \a hItem
	\param hItem	- ����, ��� �������� ���������� ����
	\param dwMenuId - id ������� ����, ��������� �� ����������� ���� � \a pm
	\param pm		- ��������� �� ����, ������� ����� ��������������
*/
void SceneTreeView::ModifyMenu(qdNamedObjectBase const* pobj,
							   HTREEITEM hItem, DWORD dwMenuId, CMenu* pm)
{
	switch(dwMenuId) {
	case IDR_STATE_NODE:
		mmStateNode(hItem, static_cast<const qdNamedObject*>(pobj), pm);
		break;
	case  IDR_STATIC_OBJ_NODE:
		mmStaticObjNode(hItem, static_cast<const qdNamedObject*>(pobj), pm);
		break;
	case IDR_MOVING_OBJ_NODE:
		mmMovingObjNode(hItem, static_cast<const qdNamedObject*>(pobj), pm);
		break;
	case IDR_ANIMATED_OBJ_NODE:
		mmAnimatedObjNode(hItem, static_cast<const qdNamedObject*>(pobj), pm);
		break;
	case IDR_INVENTORY_OBJ_NODE:
		mmInventoryObjNode(hItem, static_cast<const qdNamedObject*>(pobj), pm);
		break;
	case IDR_MUSIC_TRACK_NODE:
		mmMusicTrackNode(hItem, static_cast<const qdNamedObject*>(pobj), pm);
		break;
	case IDR_STATIC_OBJS_NODE:
		mmStaticObjsNode(hItem, static_cast<const qdNamedObject*>(pobj), pm);
		break;
	case IDR_ANIMATED_OBJS_NODE:
		mmAnimatedOjbsNode(hItem, static_cast<const qdNamedObject*>(pobj), pm);
		break;
	case IDR_MOVING_OBJS_NODE:
		mmMovingObjsNode(hItem, static_cast<const qdNamedObject*>(pobj), pm);
		break;
	case IDR_GRID_ZONE_NODE:
		mmGridZoneNode(hItem, static_cast<const qdNamedObject*>(pobj), pm);
		break;
	case IDR_VIDEO_NODE:
		mmVideoNode(hItem, static_cast<const qdNamedObject*>(pobj), pm);
		break;
	case IDR_INVENTORY_OBJS_NODE:
		mmInventoryObjsNode(hItem, static_cast<const qdNamedObject*>(pobj), pm);
		break;
	case IDR_SCENE_NODE:
		mmSceneNode(hItem, static_cast<const qdNamedObject*>(pobj), pm);
		break;
	}
}

void SceneTreeView::mmStateNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm)
{
	const qdGameObjectState* pstate = static_cast<const qdGameObjectState*>(pobj);
	if (pstate->owner()->named_object_type() == QD_NAMED_OBJECT_MOUSE_OBJ)
	{
		pm->RemoveMenu(IDC_COORDS_ANIMATION, MF_BYCOMMAND);
		pm->RemoveMenu(IDC_BIND_OBJ_CENTER, MF_BYCOMMAND);
		pm->RemoveMenu(IDC_BTN_AUTO_BND, MF_BYCOMMAND);
		pm->RemoveMenu(IDC_STATE_HAS_BOUND, MF_BYCOMMAND);
		pm->RemoveMenu(IDC_STATE_DIMS, MF_BYCOMMAND);
		pm->RemoveMenu(IDC_MAKE_BOUND_AS_FIRST, MF_BYCOMMAND);
		pm->RemoveMenu(IDC_SELECT_MOVEMENT_TYPE, MF_BYCOMMAND);
		//������� ���������
		pm->RemoveMenu(5, MF_BYPOSITION);
		pm->RemoveMenu(5, MF_BYPOSITION);
		if (!m_IsSceneViewActive||!m_ptrGameDisp->get_active_scene()) 
			menu_h::disableMenuItem(pm, IDC_STACK_STATE);
		pm->RemoveMenu(IDC_STATE_CONDITION, MF_BYCOMMAND);
	}
	else
	{
		bool bDisableAll = false;
		if (ptree_helpers::HasGlobalFlag(pstate)) {
			//��������� ���������� ������������� �����. 
			//���� �������� � ���������� ��������� ��� ���������� �� ���������,
			//�� ��������� ������ ���.
			//���� � ��� ������(�.�. ��� ������ ����������� ��������� � �����),
			//�� ������������ 

			HTREEITEM hSceneItem = 
				ptree_helpers::IsObjUnderScene(GetTreeCtrl(), hItem);
			if (hSceneItem) 
				bDisableAll = true;
			else
				menu_h::disableMenuItem(pm, IDC_COORDS_ANIMATION);
		}

		if (bDisableAll) 
			DisableAllMenuItems(pm);
		else{
			//��������� ���������
			qdGameScene* pActiveScene = m_ptrGameDisp->get_active_scene();
			//���� ��������� �� ����������� �������� �����, �� ���������� ���. ����������
			if (pActiveScene != static_cast<qdGameScene*>(pobj->ref_owner()->owner())) 
			{
				if (!pActiveScene||!m_IsSceneViewActive) {
					menu_h::disableMenuItem(pm, IDC_STATE_DIMS);
					menu_h::disableMenuItem(pm, IDC_STATE_HAS_BOUND);
					menu_h::disableMenuItem(pm, IDC_BTN_AUTO_BND);
					menu_h::disableMenuItem(pm, IDC_MAKE_BOUND_AS_FIRST);
					menu_h::disableMenuItem(pm, IDC_STACK_STATE);
				}
				menu_h::disableMenuItem(pm, IDC_COORDS_ANIMATION);
				menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES);
				menu_h::disableMenuItem(pm, IDC_BIND_OBJ_CENTER);
			}
			else{
				if (pstate->state_type() == qdGameObjectState::STATE_MASK) 
				{
					disableAnimationMenuItems(pm);
					menu_h::disableMenuItem(pm, IDC_MAKE_BOUND_AS_FIRST);
					menu_h::disableMenuItem(pm, IDC_CANCEL_REMOVING_EDGES);
				}
				else
				{
					if (!m_IsSceneViewActive){
						disableAnimationMenuItems(pm);
					}
					if (pstate->state_type() == qdGameObjectState::STATE_STATIC)
					{
						qdGameObjectStateStatic const* p = 
							static_cast<qdGameObjectStateStatic const*>(pstate);
						if (!p->animation())
						{
							disableAnimationMenuItems(pm);
							menu_h::disableMenuItem(pm, IDC_BIND_OBJ_CENTER);
							menu_h::disableMenuItem(pm, IDC_EDIT_STATE_ANIMATION);
							menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES);
						}
					}
					qdGameObjectAnimated* const owner = 
						static_cast<qdGameObjectAnimated*>(pobj->owner());
					if(qdGameObjectState* const pdef_state = owner->get_default_state()){
						if (!pdef_state->has_bound())
							menu_h::disableMenuItem(pm, IDC_MAKE_BOUND_AS_FIRST);
					}
				}
			}

			if (pstate->state_type() != qdGameObjectState::STATE_STATIC)
			{
				menu_h::disableMenuItem(pm, IDC_EDIT_STATE_ANIMATION);
				menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES);
				menu_h::disableMenuItem(pm, IDC_BIND_OBJ_CENTER);
			}
		}
		if (pstate->state_type() != qdGameObjectState::STATE_MASK)
			menu_h::checkMenuItem(pm, IDC_STATE_HAS_BOUND, pstate->has_bound());
		if (pstate->state_type() != qdGameObjectState::STATE_WALK)
			pm->RemoveMenu(IDC_SELECT_MOVEMENT_TYPE, MF_BYCOMMAND);
		else
			ModifySelectMovementCaption(pm, 
				static_cast<qdGameObjectStateWalk const*>(pstate));
	}
}

void SceneTreeView::mmMovingObjNode(HTREEITEM hItem, const qdNamedObject* pobj, 
										CMenu* pm)
{
	menu_h::checkMenuItem(pm, IDC_NOT_CONTROLED_PERS, 
		pobj->check_flag(QD_OBJ_NON_PLAYER_PERSONAGE_FLAG));

	if (m_ptrGameDisp->get_active_scene() != static_cast<qdGameScene*>(pobj->owner()))
		menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES_IN_STATES);
}

void SceneTreeView::mmAnimatedObjNode(HTREEITEM hItem, const qdNamedObject* pobj, 
																			CMenu* pm)
{
	menu_h::checkMenuItem(pm, IDC_DISABLE_MOUSE, 
		pobj->check_flag(QD_OBJ_DISABLE_MOUSE_FLAG));

	if (m_ptrGameDisp->get_active_scene() != static_cast<qdGameScene*>(pobj->owner()))
		menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES_IN_STATES);
}

void SceneTreeView::mmStaticObjNode(HTREEITEM hItem, const qdNamedObject* pobj, 
																		CMenu* pm)
{
	if (m_ptrGameDisp->get_active_scene() != static_cast<qdGameScene*>(pobj->owner()))
		menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES);

	if (!m_IsSceneViewActive)
		menu_h::disableMenuItem(pm, IDC_CREATE_MASK);
}

void SceneTreeView::mmInventoryObjNode(HTREEITEM hItem, const qdNamedObject* pobj, 
																			 CMenu* pm)
{
	const qdInventory* pi = static_cast<const qdInventory*>(pobj);
	if (pi->selected_cellset_num() == -1)
		menu_h::disableMenuItem(pm, IDC_REMOVE_CELL_SET);
	else
		menu_h::disableMenuItem(pm, IDC_REMOVE);
}

void SceneTreeView::mmMusicTrackNode(HTREEITEM hItem, const qdNamedObject* pobj, 
									 CMenu* pm)
{
	const qdMusicTrack* pmt = static_cast<const qdMusicTrack*>(pobj);
	menu_h::checkMenuItem(pm, IDC_MUSIC_TRACK_CYCLED, pmt->is_cycled());
}

void SceneTreeView::mmStaticObjsNode(HTREEITEM hItem, const qdNamedObject* pobj, 
																		CMenu* pm)
{
	if(m_hCurrScene == NULL||!m_IsSceneViewActive)
		menu_h::disableMenuItem(pm, IDC_CRT_STATIC_OBJ);
}
void SceneTreeView::mmAnimatedOjbsNode(HTREEITEM hItem, const qdNamedObject* pobj, 
																		CMenu* pm)
{
	if(m_hCurrScene == NULL||!m_IsSceneViewActive)
		DisableAllMenuItems(pm);
	// ��� ������ �������� �������������, ������������ ������
	menu_h::enableMenuItem(pm, IDC_COLLAPSE_ALL);
	menu_h::enableMenuItem(pm, IDC_EXPAND_ALL);
}
void SceneTreeView::mmMovingObjsNode(HTREEITEM hItem, const qdNamedObject* pobj, 
																		 CMenu* pm)
{
	if(m_ptrGameDisp == NULL||!m_IsSceneViewActive)
		menu_h::disableMenuItem(pm, IDC_CRT_MOVING_OBJ);
}
void SceneTreeView::mmGridZoneNode(HTREEITEM hItem, const qdNamedObject* pobj, 
																		CMenu* pm)
{
	if(m_hCurrScene == NULL||!m_IsSceneViewActive)
		menu_h::disableMenuItem(pm, IDC_CRT_GRID_ZONE);
}
void SceneTreeView::mmVideoNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm)
{
	if(!m_IsSceneViewActive){
		menu_h::disableMenuItem(pm, IDC_STOP_VIDEO);
		menu_h::disableMenuItem(pm, IDC_PLAY_STOP_VIDEO);
	}
}
void SceneTreeView::mmInventoryObjsNode(HTREEITEM hItem, const qdNamedObject* pobj, 
										CMenu* pm)
{
	if(!m_IsSceneViewActive)
		menu_h::disableMenuItem(pm, IDC_CRT_NEW_INVENTORY);
}

void SceneTreeView::mmSceneNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm)
{
	if(!m_IsSceneViewActive)
	{
		CMenu* psub = pm->GetSubMenu(0);
		if (psub){
			menu_h::disableMenuItem(psub, IDC_CRT_STATIC_OBJ);
			menu_h::disableMenuItem(psub, IDC_CRT_ANIMATED_OBJ);
			menu_h::disableMenuItem(psub, IDC_CRT_MOVING_OBJ);
		}
		menu_h::disableMenuItem(pm, IDC_SOUND_LIST);
	}

	if (static_cast<const qdGameScene*>(pobj)!=m_ptrGameDisp->get_active_scene())
		menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES_FOR_ALL_OBJS);
}
