/********************************************************************
	created:	2002/09/11
	created:	11:9:2002   16:16
	filename: 	D:\Projects\Quest\QuestEditor\ptree_hlprs.cpp
	file path:	D:\Projects\Quest\QuestEditor
	file base:	ptree_hlprs
	file ext:	cpp
	Powerd by:	Илюха
	
	purpose:	Не inline функции
*********************************************************************/

#include "stdafx.h"
#include "questeditor.h"

#include "qd_grid_zone.h"
#include "qd_game_object_static.h"
#include "qd_game_object_animated.h"
#include "qd_game_object_moving.h"
#include "qd_game_object_mouse.h"
#include "qd_game_dispatcher.h"
#include "qd_interface_object_base.h"
#include "qd_interface_button.h"
#include "qd_interface_slider.h"
#include "qd_interface_background.h"
#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"
#include "qd_interface_save.h"
#include "qd_trigger_chain.h"
#include "qd_interface_text_window.h"

#include "SceneTreeView.h"
#include "relativepath.h"

#include "ptree_hlprs.hpp"

#include <typeinfo>
namespace ptree_helpers
{
BOOL cfALL(qdGameObject*){	
	return TRUE;
}
	
HTREEITEM FindTreeItemByText(CTreeCtrl& tree, HTREEITEM hPar, LPCTSTR strText)
{
	HTREEITEM hChild = tree.GetChildItem(hPar);
	if(!hChild)
		return NULL;
	do {
		if(tree.GetItemText(hChild)==strText)
			return hChild;
		hChild = tree.GetNextSiblingItem(hChild);
	} while(hChild);
	
	return NULL;
}

CString GetFolderNodeName(qdNamedObjectType __t)
{
	CString res;
	DWORD resID = 0;
	switch(__t){
	case QD_NAMED_OBJECT_GENERIC:
	case QD_NAMED_OBJECT_MOUSE_OBJ:
		ASSERT(0);
		break;
	case QD_NAMED_OBJECT_ANIMATION:
		resID = IDS_ANIMATIONS_NODE;
		break;
	case QD_NAMED_OBJECT_COORDS_ANIMATION:
		resID = IDS_COORDS_ANIMATIONS_NODE;
		break;
	case QD_NAMED_OBJECT_STATIC_OBJ:
		resID = IDS_STATIC_OBJS_NODE;
		break;
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
		resID = IDS_ANIMATED_OBJS_NODE;
		break;
	case QD_NAMED_OBJECT_MOVING_OBJ:
		resID = IDS_MOVING_OBJS_NODE;
		break;
	case QD_NAMED_OBJECT_SCENE:
		resID = IDS_SCENES_NODE;
		break;
	case QD_NAMED_OBJECT_DISPATCHER:
		resID = IDS_GLOBAL_NODE;
		break;
	case QD_NAMED_OBJECT_GRID_ZONE:
		resID = IDS_NET_NODE_NAME;
		break;
	case QD_NAMED_OBJECT_INVENTORY:
		resID = IDS_INVENTORY_OBJS_NODE;
		break;
	case QD_NAMED_OBJECT_MUSIC_TRACK:
		resID = IDS_MUSIC_TRACKS_NODE;
		break;
	case QD_NAMED_OBJECT_TRIGGER_CHAIN:
		resID = IDS_TRIGGERS_NODE;
		break;
	case QD_NAMED_OBJECT_VIDEO:
		resID = IDS_VIDEO_CATALOG;
		break;
	case QD_NAMED_OBJECT_GAME_END:
		resID = IDS_GAME_ENDS_NODE_NAME;
		break;
	case QD_NAMED_OBJECT_COUNTER:
		resID = IDS_COUNTERS_NODE_NAME;
		break;
	default:
		ASSERT(0);
	};
	if(res)
		res.LoadString(resID);
	return res;
}

bool is_dispacher(qdNamedObject* pno){
	ASSERT(is_obj(reinterpret_cast<DWORD_PTR>(pno)));
	
	qdNamedObjectType type = (qdNamedObjectType)pno->named_object_type();
	return (type == QD_NAMED_OBJECT_DISPATCHER
		||type == QD_NAMED_OBJECT_SCENE);
}

HTREEITEM GetDispatcherNode(CTreeCtrl& tree, HTREEITEM hItem)
{
	ASSERT(!IsItemUnderInterface(tree, hItem));
	while (!is_dispacher((qdNamedObject*)tree.GetItemData(hItem))) {
		hItem = tree.GetParentItem(hItem);
	}
	return hItem;
}

HTREEITEM IsObjUnderScene(CTreeCtrl& tree, HTREEITEM hItem)
{
	do {
		if (is_scene(tree, hItem))
			return hItem;
	} while(hItem = tree.GetParentItem(hItem));
	return NULL;
}

HTREEITEM IsItemUnderInterface(CTreeCtrl& tree, HTREEITEM hItem)
{
	do {
		if (is_interface_root(tree, hItem)) 		
			return hItem;
	} while(hItem = tree.GetParentItem(hItem));
	return NULL;
}

void DeleteSubItems(CTreeCtrl& tree, HTREEITEM hItem)
{
	HTREEITEM hFirst = tree.GetChildItem(hItem);
	while(hFirst)
	{
		tree.DeleteItem(hFirst);
		hFirst = tree.GetChildItem(hItem);
	} 
}

void DeleteSubItemsExlude(CTreeCtrl& tree, HTREEITEM hPar, HTREEITEM hExcl)
{
	HTREEITEM hFirst = tree.GetChildItem(hPar);
	while(hFirst)
	{
		if (hExcl != hFirst)
			tree.DeleteItem(hFirst);
		hFirst = tree.GetChildItem(hPar);
	} 
}

HTREEITEM GetStateCatalog(CTreeCtrl& tree, HTREEITEM hObjItem,
						  int type, int  obj_type)
{
	HTREEITEM hCat = hObjItem;
	
	switch(type) {
	case qdGameObjectState::STATE_STATIC:
	case qdGameObjectState::STATE_MASK:
		{
			if (obj_type == QD_NAMED_OBJECT_MOVING_OBJ) 
			{
				CString strCatName((LPTSTR)IDS_STOPS_CATALOG);
				hCat = ptree_helpers::FindTreeItemByText(tree, hObjItem, strCatName);
				if (!hCat) {
					hCat = tree.InsertItem(strCatName, hObjItem);
					
					tree.SetItemImage(hCat, II_STATES_CATALOG, II_STATES_CATALOG);
					tree.SetItemData(hCat, QE_STATE_CATALOG);
					tree.SetItemState(hCat, 0, TVIS_STATEIMAGEMASK);	
				}
			}
		}
		break;
	case qdGameObjectState::STATE_WALK:
		{
			CString strCatName((LPTSTR)IDS_GAITS_CATALOG);
			hCat = ptree_helpers::FindTreeItemByText(tree, hObjItem, strCatName);
			if (!hCat) {
				hCat = tree.InsertItem(strCatName, hObjItem);
				tree.SetItemImage(hCat, II_STATES_CATALOG, II_STATES_CATALOG);
				tree.SetItemData(hCat, QE_MOVEMENT_STATE_CATALOG);
				tree.SetItemState(hCat, 0, TVIS_STATEIMAGEMASK);	
			}
		}
		break;
	}
	return hCat;		
}

DWORD GetCorrspondingMenuResID(const qdInterfaceObjectBase* pobj)
{
	const type_info& ti = typeid(*pobj);
	if (typeid(qdInterfaceButton)==ti) 
	{
		return IDR_INTERFACE_BTN_NODE;
	}
	else if (typeid(qdInterfaceSlider)==ti) 
	{
		return IDR_INTERFACE_ELE_NODE;
	}
	else if (typeid(qdInterfaceBackground)==ti) 
	{
		return IDR_INTERFACE_BACK_NODE;
	}
	else if (ti == typeid(qdInterfaceSave))
	{
		return IDR_INTERFACE_ELE_NODE;
	}
	else if (typeid(qdInterfaceScreen)==ti) 
	{
		return IDR_INTERFACE_SCREEN_NODE;
	}
	else if (typeid(qdInterfaceDispatcher)==ti) 
	{
		return IDR_INTERFACE_DISPATCHER;
	}
	else if (typeid(qdInterfaceElementState)==ti)
	{
		return IDR_INTERFACE_ELE_STATE_NODE;
	}
	else if (typeid(qdInterfaceTextWindow) == ti)
	{
		return IDR_INTERFACE_TEXT_WINDOW_NODE;
	}
	return -1;
}

DWORD GetCorrspondingMenuResID(bool bIsCont, qdNamedObjectType type, 
									bool bIsCopyOfGlobal)
{
	DWORD dwMenuResID = -1;
	switch(type) {
	case QD_NAMED_OBJECT_DISPATCHER://глобальный узел
		dwMenuResID = IDR_GLOBAL_NODE;
		break;
	case QD_NAMED_OBJECT_STATIC_OBJ:
		dwMenuResID = bIsCont ? IDR_STATIC_OBJS_NODE : IDR_STATIC_OBJ_NODE;
		break;
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
		dwMenuResID = bIsCont ? IDR_ANIMATED_OBJS_NODE : IDR_ANIMATED_OBJ_NODE;
		break;
	case QD_NAMED_OBJECT_MOVING_OBJ:
		if (bIsCopyOfGlobal) 
			dwMenuResID = bIsCont ? IDR_MOVING_OBJS_NODE : IDR_GLOBAL_MOVING_OBJ_NODE;
		else
			dwMenuResID = bIsCont ? IDR_MOVING_OBJS_NODE : IDR_MOVING_OBJ_NODE;
		break;
	case QD_NAMED_OBJECT_SCENE:
		dwMenuResID = bIsCont ? IDR_SCENES_NODE : IDR_SCENE_NODE;
		break;
	case QD_NAMED_OBJECT_ANIMATION:
		dwMenuResID = bIsCont ? IDR_ANIMATIONS_NODE : IDR_ANIMATION_NODE;
		break;
	case QD_NAMED_OBJECT_OBJ_STATE:
		dwMenuResID = IDR_STATE_NODE;
		break;
	case QD_NAMED_OBJECT_MOUSE_OBJ:
		dwMenuResID = IDR_MOUSE_NODE;
		break;
	case QD_NAMED_OBJECT_VIDEO:
		dwMenuResID = bIsCont ? IDR_VIDEOS_NODE : IDR_VIDEO_NODE;
		break;
	case QD_NAMED_OBJECT_GRID_ZONE:
		dwMenuResID = bIsCont?IDR_NET_NODE : IDR_GRID_ZONE_NODE;
		break;
	case QE_MOVEMENT_STATE_CATALOG:
		ASSERT(bIsCont);
		dwMenuResID = IDR_MOVING_OBJ_GAITS_NODE;
		break;
	case QE_STATE_CATALOG:
		ASSERT(bIsCont);
		dwMenuResID = IDR_MOVING_OBJ_ANIMATION_NODE;
		break;
	case QD_NAMED_OBJECT_TRIGGER_CHAIN:
		dwMenuResID = bIsCont? IDR_TRIGGERS_NODE : IDR_TRIGGER_NODE;
		break;
	case QD_NAMED_OBJECT_INVENTORY:
		dwMenuResID = bIsCont? IDR_INVENTORY_OBJS_NODE 
							: IDR_INVENTORY_OBJ_NODE;
		break;
	case QD_NAMED_OBJECT_MINIGAME:
		dwMenuResID = bIsCont? IDR_MINIGAMES_NODE
							: IDR_MINIGAME_OBJ_NODE;
		break;
	case QD_NAMED_OBJECT_MUSIC_TRACK:
		dwMenuResID = bIsCont? IDR_MUSIC_TRACKS_NODE
							: IDR_MUSIC_TRACK_NODE;
		break;
	case QD_NAMED_OBJECT_GRID_ZONE_STATE:
		dwMenuResID = IDR_GRID_ZONE_STATE_NODE;
		break;
	case QD_NAMED_OBJECT_GAME_END:
		dwMenuResID = bIsCont ? IDR_GAME_ENDS_NODE
								: IDR_GAME_END_NODE;
		break;
	case QD_NAMED_OBJECT_COUNTER:
		dwMenuResID = bIsCont ? IDR_COUNTERS_NODE
								: IDR_COUNTER_NODE;
		break;
	case QD_NAMED_OBJECT_FONT_INFO:
		dwMenuResID = bIsCont ? IDR_FONTS_NODE
								:  IDR_FONT_NODE;
		break;
	}
	return dwMenuResID;
}

qdGameScene* GetSceneByItem(CTreeCtrl& tree, HTREEITEM hItem)
{
	if (!hItem) 
		return NULL;
	
	hItem = IsObjUnderScene(tree, hItem);
	qdGameScene* ps = NULL;
	if(hItem)
		ps = reinterpret_cast<qdGameScene*>(tree.GetItemData(hItem));
	return ps;
}

bool is_interface_root(CTreeCtrl& tree, HTREEITEM hItem)
{
	DWORD data = tree.GetItemData(hItem);
	if (!is_obj(data))
		return false;

	std::pair<qdNamedObjectBase*, bool> r = is_any_interf_obj(data);
	if (!r.second)
		return false;
	qdInterfaceDispatcher * const p = qdInterfaceDispatcher::get_dispatcher();
	return (static_cast<qdNamedObjectBase*>(p)==r.first);
}

int get_state_index(const qdGameObjectStateVector& v, const qdGameObjectState*ps)
{
	qdGameObjectStateVector::const_iterator r = std::find(v.begin(), v.end(), ps);
	if (r == v.end())
		return -1;
	return std::distance(v.begin(), r);
}


bool IsContainerObj(CTreeCtrl& tree, HTREEITEM hItem)
{
	DWORD dwData = tree.GetItemData(hItem);
	if (!is_obj(dwData))
		return true;

	std::pair<qdNamedObjectBase*, bool> r = is_any_interf_obj(dwData);
	if (r.second)
	{
		if (is_interface_root(tree, hItem))
			return true;
		return false;
	}
	else
	{
		return 
			(static_cast<qdNamedObject*>(r.first)->named_object_type()
				==QD_NAMED_OBJECT_DISPATCHER);
	}
}

std::auto_ptr<CStringArray>	CreateTreePath(const qdInterfaceObjectBase* pobj)
{
	//максимальный уровень вложенности в нашем дереве(с запасом)
	const int MAX_TREE_MULTIPLICITY = 10;
	std::auto_ptr<CStringArray> res(new CStringArray);
	res->SetSize(MAX_TREE_MULTIPLICITY);	
	int _count = 0;
	for(int i = 0; (i < MAX_TREE_MULTIPLICITY)&&pobj; pobj = pobj->owner())
	{
		if (typeid(qdInterfaceDispatcher)==typeid(*pobj)) 
		{
			res->SetAt(_count, CString((LPTSTR)IDS_INTERFACE_NODE));
			++_count;

			res->SetAt(_count, ptree_helpers::GetFolderNodeName(QD_NAMED_OBJECT_DISPATCHER));
			++_count;
			res->SetSize(_count);
			return res;
		}
		res->SetAt(_count, pobj->name());
		++_count;
	}
	return std::auto_ptr<CStringArray>();
}

std::auto_ptr<CStringArray>	CreateTreePath(const qdNamedObject* pobj)
{
	//максимальный уровень вложенности в нашем дереве(с запасом)
	const int MAX_TREE_MULTIPLICITY = 10;

	std::auto_ptr<CStringArray> res(new CStringArray);
	res->SetSize(MAX_TREE_MULTIPLICITY);	

	int _count = 0;
	for(int i = 0; (i < MAX_TREE_MULTIPLICITY)&&pobj; pobj = pobj->ref_owner())
	{
		qdNamedObjectType type = (qdNamedObjectType)pobj->named_object_type();
		if(type == QD_NAMED_OBJECT_DISPATCHER)
		{
			res->SetAt(_count, GetFolderNodeName(type));
			++_count;
		}
		else
		{
			res->SetAt(_count, pobj->name());
			++_count;
			//добавляем имена родителей, исключая локацию
			//если это не сцена и не локация, добавляем имя узла каталога
			if(type != QD_NAMED_OBJECT_SCENE
				&&type != QD_NAMED_OBJECT_MINIGAME
				&&type != QD_NAMED_OBJECT_MOUSE_OBJ)
			{
				if (type != QD_NAMED_OBJECT_OBJ_STATE
					&&type != QD_NAMED_OBJECT_GRID_ZONE_STATE) {
					CString fnd = GetFolderNodeName(type);
					if(!fnd.IsEmpty())
					{
						res->SetAt(_count, fnd);
						++_count;
					}
					if (type == QD_NAMED_OBJECT_GAME_END)
						res->SetAt(_count++, CString((LPTSTR)IDS_INTERFACE_NODE));
				}
				else if(pobj->owner()->named_object_type() 
												== QD_NAMED_OBJECT_MOVING_OBJ)
				{
					CString str;
					if(IsMovementState(static_cast<const qdGameObjectState*>(pobj)))
						str.LoadString(IDS_GAITS_CATALOG);
					else
						str.LoadString(IDS_STOPS_CATALOG);

					res->SetAt(_count, str);
					++_count;
				}
			}
		}
	}
	res->SetSize(_count);
	
	return res;
}

int SelAnimObjPic(const qdGameObject* pa){
	if (pa->check_flag(QD_OBJ_DISABLE_MOUSE_FLAG)) 
		return II_DISABLE_MOUSE_ANIMATED_OBJ_NODE;
	else
		return II_ANIMATED_OBJ_NODE;
}

int SelStaticObjPic(const qdGameObject* ps){
	if (ps->check_flag(QD_OBJ_DISABLE_MOVEMENT_FLAG))
		return II_NOT_MOV_STATIC_OBJ_NODE;
	return II_STATIC_OBJ_NODE;
}

int SelPic4MouseState(const qdGameObjectMouse* pmouse, 
											const qdGameObjectState* pstate)
{
	int const id = pmouse->default_cursor(qdGameObjectMouse::OBJECT_CURSOR);
	int const id2 = pmouse->default_cursor(qdGameObjectMouse::ZONE_CURSOR);
	int const id3 = pmouse->default_cursor(qdGameObjectMouse::MAIN_MENU_CURSOR);
	int const id4 = pmouse->default_cursor(qdGameObjectMouse::INGAME_INTERFACE_CURSOR);
	int const id5 = pmouse->default_cursor(qdGameObjectMouse::INVENTORY_OBJECT_CURSOR);
	int const state_id = get_state_index(pmouse->state_vector(), pstate);

	if (!state_id)
		return II_CURSOR_FOR_ALL_NODE;

	if (id&&id == id2&&state_id == id) 
		return II_CURSOR_FOR_DYN_OBJ_AND_ZONES;
	else
	{
		if (id&&id == state_id)
			return II_CURSOR_FOR_DYN_OBJ;

		if (id2&&id2 == state_id)
			return II_CURSOR_FOR_ZONES;
		if (id3&&id3 == state_id)
			return II_CURSOR_FOR_MAIN_MENU;
		if (id4&&id4 == state_id)
			return II_CURSOR_FOR_INGAME_INTERF;
		if (id5&&id5 == state_id)
			return II_CURSOR_FOR_OBJ_IN_INVENTORY;
	}
	return II_CURSOR_NODE;
}

int SelStatePic(const qdGameObjectState* ps, const qdGameObjectAnimated* pobj)
{
	if (ptree_helpers::is_obj_mouse(pobj))
		return ptree_helpers::SelPic4MouseState(
							static_cast<const qdGameObjectMouse*>(pobj), ps);
	else
	{
		if (!ps->coords_animation()->is_empty())
			return II_CA_STATE_NODE;
		else if (IsGlobalState4Obj(ps, pobj)) 
			return II_GLOBAL_STATE_NODE;
		else if (ptree_helpers::IsGlobalObj(pobj)&&!ptree_helpers::is_obj_mouse(pobj)) 
			return II_GLOBAL_OBJ_STATE_NODE;
		else if (ps->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_INVENTORY))
			return II_INVENTORY_STATE_NODE;
		else if (ps->has_text())
			return II_DIALOG_STATE_NODE;
		else if (ps->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_MOVE_TO_INVENTORY))
			return II_STATE4INVENTORY_NODE;
	}
	return II_STATE_NODE;
}

int SelGZonePic(qdGridZone const* pgz)
{
	if (pgz->check_flag(qdGridZone::ZONE_EXIT_FLAG))
		return II_GZONE_NEED_CUR_NODE;
	else
		return II_ZONE_NODE;
}

bool is_interface_screen(CTreeCtrl& tree, HTREEITEM hItem)
{
	DWORD data = tree.GetItemData(hItem);
	if (!is_obj(data)) 
		return false;
	std::pair<qdNamedObjectBase*, bool> res = is_any_interf_obj(data);
	if (res.second)
		return (typeid(qdInterfaceScreen)==typeid(*res.first));
	return false;
}

bool is_node_type(CTreeCtrl& tree, HTREEITEM hItem, DWORD dwType){
	DWORD dwData = tree.GetItemData(hItem);
	if(!is_obj(dwData))
		return false;
	std::pair<qdNamedObjectBase*, bool> res = is_any_interf_obj(dwData);
	if (!res.second)
	{
		const qdNamedObject* const pno = static_cast<qdNamedObject*>(res.first); 
		return (pno->named_object_type() == dwType);
	}
	return false;
}

bool HasGlobalFlag(const qdNamedObject* ps){
	ASSERT(ps->named_object_type() == QD_NAMED_OBJECT_OBJ_STATE);
	return (ps->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_GLOBAL_OWNER));
}

bool IsGlobalState4Obj(const qdGameObjectState* pstate, const qdGameObjectAnimated* pobj)
{
	return HasGlobalFlag(pstate)&&
		(static_cast<const qdNamedObject*>(pstate->ref_owner()) !=pobj);
}

bool HasWalkState(const qdGameObjectAnimated* pobj)
{
	typedef bool (*state_detector)(const qdGameObjectState* pstate);

	const qdGameObjectStateVector& v = pobj->state_vector();
	qdGameObjectStateVector::const_iterator res = 
		std::find_if(v.begin(), v.end(), static_cast<state_detector>(IsMovementState));
	return (res != v.end());
}

template <DWORD_PTR CatalogId>
HTREEITEM IsUnderCatalog(CTreeCtrl& tree, HTREEITEM hItem)
{
	do {
		DWORD_PTR dwData = tree.GetItemData(hItem);
		if (!is_obj(dwData))
		{
			if (dwData == CatalogId)
				return hItem;
		}
	} while(hItem = tree.GetParentItem(hItem));
	return NULL;
}

HTREEITEM IsUnderAnimatedCatalog(CTreeCtrl& tree, HTREEITEM hItem)
{
	return IsUnderCatalog<QD_NAMED_OBJECT_ANIMATED_OBJ>(tree, hItem);
}
HTREEITEM IsUnderMovingCatalog(CTreeCtrl& tree, HTREEITEM hItem)
{
	return IsUnderCatalog<QD_NAMED_OBJECT_MOVING_OBJ>(tree, hItem);
}

std::pair<qdNamedObjectBase*, bool> is_any_interf_obj(DWORD_PTR data)
{
	qdNamedObjectBase* pob = reinterpret_cast<qdNamedObjectBase*>(data);
	bool const secondValue = (dynamic_cast<qdInterfaceObjectBase*>(pob));
	return std::make_pair(pob, secondValue);
}

//! Находит в списке цепочек цепочку, с указанным именем
qdTriggerChain* findTriggerChainByName(qdTriggerChainList const& lst, LPCSTR name)
{
	qdTriggerChainList::const_iterator itr = lst.begin(), end = lst.end();
	for(; itr != end; ++itr)
	{
		if (!_tcscmp((*itr)->name(), name))
			return *itr;
	}
	return NULL;
}

bool is_dlg_state(const qdNamedObject* p){
	if(p->named_object_type() != QD_NAMED_OBJECT_OBJ_STATE)
		return false;
	return static_cast<const qdGameObjectState*>(p)->has_text();
}

//! Является ли состояние статическим
bool is_state_static(const qdGameObjectState* pstate){
	ASSERT(pstate->named_object_type() == QD_NAMED_OBJECT_OBJ_STATE);
	return (pstate->state_type() == qdGameObjectState::STATE_STATIC);
}

//! Является ли состояние походочным состоянием
bool IsMovementState(const qdGameObjectState* pno)
{
	ASSERT(pno->named_object_type() == QD_NAMED_OBJECT_OBJ_STATE);
	return (pno->state_type() == qdGameObjectState::STATE_WALK);
}

}//end of namespace