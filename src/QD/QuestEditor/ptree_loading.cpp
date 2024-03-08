/********************************************************************
created:	2002/09/06
created:	6:9:2002   11:16
filename: 	D:\Projects\Quest\QuestEditor\ptree_loading.cpp
file path:	D:\Projects\Quest\QuestEditor
file base:	ptree_loading
file ext:	cpp
Powerd by:	�����

  purpose:	����� ������� �������� ������ �������
*********************************************************************/

#include "stdafx.h"
#include "questeditor.h"
#include <list>

#include "qd_music_track.h"
#include "qd_minigame.h"
#include "qd_trigger_chain.h"
#include "qd_game_object_moving.h"
#include "qd_game_object_mouse.h"
#include "qd_video.h"
#include "qd_grid_zone.h"
#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"
#include "qd_font_info.h"

#include "snd_dispatcher.h"

#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"
#include "qd_interface_element.h"
#include "qd_interface_button.h"

#include "scenetreeview.h"

#include "ptree_hlprs.hpp"
#include "ptree_loading.h"
#include "GameEndLoader.h"
#include "CounterLoader.h"
#include <functional>
#include <boost/utility.hpp>
#include <boost/rtl.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************/
/* ���������� �������                                                   */
/************************************************************************/
namespace ptree_loading{
using namespace ptree_helpers;
//! ������� ����������������� ��� ������������ ���������
class all_walk{
public:
	inline bool operator()(const qdGameObjectState* ps) const{
		return (ps->state_type() == qdGameObjectState::STATE_WALK);
	}
};

//! ������� ����������������� ��� ���������� ���������
class all_not_walk{
public:
	
	inline bool operator()(const qdGameObjectState* ps) const{
		return (ps->state_type() != qdGameObjectState::STATE_WALK);
	}
};

//! ������� ������ �� �����������������
class all_states{
public:
	inline bool operator()(const qdGameObjectState* ps) const{
		return true;
	}
};

bool own_walk::operator()(const qdGameObjectState* ps) const{
	return (!ptree_helpers::IsGlobalState4Obj(ps, powner)
		&&ps->state_type() == qdGameObjectState::STATE_WALK);
}

bool own_not_walk::operator()(const qdGameObjectState* ps) const{
	return (!ptree_helpers::IsGlobalState4Obj(ps, powner)
		&&ps->state_type() != qdGameObjectState::STATE_WALK);
}

BOOL SetInTrigger(CTreeCtrl& tree, HTREEITEM hItem, BOOL bInTrg, int id){

	tree.SetItemState(hItem, bInTrg ? TVIS_BOLD : 0, TVIS_BOLD);
	return tree.SetItemState(hItem, 
		INDEXTOSTATEIMAGEMASK((bInTrg ? 3+id : 0)), TVIS_STATEIMAGEMASK);
}	

bool LoadBitmaps(CImageList* ptrIL)
{
	ptrIL->Create(16, 16, ILC_MASK|ILC_COLOR32, 55, 4);
	CBitmap bitmap;
	
	for (UINT nID = 180; nID <= 199; nID++)  
	{
		bitmap.LoadBitmap(nID);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();
	}

	bitmap.LoadBitmap(IDB_INVENTORY_STATE_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	
	bitmap.LoadBitmap(IDB_DIALOG_STATE_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	
	bitmap.LoadBitmap(IDB_MINIGAME_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_MINIGAMESTATE_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_MINIGAMES_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_MUSIC_TRACKS_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_MUSIC_TRACK_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_GZONE_STATE_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_INTERFACE_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_INTERFACE_SCREEN_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_INTERFACE_ELEMENT_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_CURSOR_FOR_DYN_OBJ);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_CURSOR_FOR_ZONES);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_CURSOR_FOR_DYN_OBJ_AND_ZONES);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_CURSOR_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_GZONE_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_VIDEOS_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_VIDEO_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_INVENTORY_OBJS_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_INVENTORY_OBJ_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_DISABLE_MOUSE_ANIMATED_OBJ_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_NOT_MOV_STATIC_OBJ_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_INTERFACE_ELEMENT_STATE_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_CURSOR_FOR_ALL_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_GZONE_NEED_CUR_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_STATE4INVENTORY_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_CURSOR_FOR_MAIN_MENU);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_CURSOR_FOR_INGAME_INTERF);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_GAME_END_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_GAME_ENDS_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_COUNTER_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_COUNTERS_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_DIALOGS_FRAME_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_FONTS_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_FONT_NODE);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_CURSOR_FOR_OBJ_IN_INVENTORY);
	ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_BMP_ITEM_STATE_OBJ_VISIBLE);
	pil->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	pil->SetOverlayImage(II_DEFAULT_STATE, II_DEFAULT_STATE_OVERLAY);

	return true;
}

bool LoadStateBitmaps(CImageList* pil)
{
	pil->Create(16, 16, ILC_MASK, 0, 4);

	CBitmap bitmap;

	bitmap.LoadBitmap(IDB_BMP_ITEM_STATE_NULL_BMP);
	pil->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	
	bitmap.LoadBitmap(IDB_BMP_ITEM_STATE_OBJ_NOT_VISIBLE);
	pil->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	
	bitmap.LoadBitmap(IDB_BMP_ITEM_STATE_OBJ_VISIBLE);
	pil->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_BMP_ITEM_STATE_STATE_IN_TGR);
	pil->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	
	bitmap.LoadBitmap(IDB_BMP_ITEM_STATE_STATE_IN_NOT_ACTIVE_TGR);
	pil->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	return true;
}

/*!
 	\param wndTree		- ������ �������
	\param hPar			- ������������ ����
	\param id			- �� ������ �������� � ������ ��� ����
	\param type			- ������� ������ ���� ����� ��������� � ��������
	\param uiImage		- �� ����������� ��� ����
	\param hItemAfter	- ����, ����� �������� ���� �������� ����������� ����
*/
HTREEITEM CrtTypedNode(CTreeCtrl& wndTree, HTREEITEM hPar, DWORD id, DWORD type,
					   UINT uiImage, HTREEITEM hItemAfter)
{
	CString str;
	str.LoadString(id);
	HTREEITEM hRes = NULL;
	hRes = wndTree.InsertItem(str, hPar, hItemAfter);
	if(hRes)
	{
		wndTree.SetItemData(hRes, type);
		wndTree.SetItemImage(hRes, uiImage, uiImage);
		wndTree.SetItemState(hRes, 0, TVIS_STATEIMAGEMASK);	
	}
	return hRes;
}

/*!
	�������� ������ � ������. ������������� ����������� ��� ����. ������ � ������ ����
	��������� �� ������
 	\param tree			- ������ �������
	\param hParent		- ������������ ����
	\param po			- ������ ��� �������
	\param uiImage		- ����������� ��� ����
 	\param hInsertAfter	- �������� ���� ������� ����� ����� ����
*/
HTREEITEM InsertObject(CTreeCtrl& tree, HTREEITEM hParent, const qdNamedObjectBase* po,
											 UINT uiImage, HTREEITEM hInsertAfter/* = TVI_LAST*/)
{
	LPCTSTR name = po->name();
	HTREEITEM hItem = tree.InsertItem(name ? name : _T(""), hParent, hInsertAfter);
	if(hItem)
	{
		tree.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(po));
		tree.SetItemImage(hItem, uiImage, uiImage);
	}
	return hItem;
}

/*!
	�������� InsertObject(CTreeCtrl& tree, HTREEITEM hParent, const qdNamedObjectBase* po,
	UINT uiImage, HTREEITEM hInsertAfter). ������������� ��������� �������� � �����������
		�� ���� ����� ������ ��� ���
 	\param tree			- ������ �������
	\param hParent		- ������������ ����
	\param pno			- ������ ��� �������
	\param uiImage		- ����������� ��� ����
 	\param hInsertAfter	- �������� ���� ������� ����� ����� ����
*/
HTREEITEM InsertNamedObject(CTreeCtrl& tree, HTREEITEM hPar, 
						const qdNamedObject* pno, UINT uiImage, HTREEITEM hInsertAfter)
{
	HTREEITEM hItem = InsertObject(tree, hPar, pno, uiImage, hInsertAfter);
	if(hItem)
		tree.SetItemState(hItem, 
			INDEXTOSTATEIMAGEMASK(!pno->check_flag(QD_OBJ_HIDDEN_FLAG)?2:1), 
			TVIS_STATEIMAGEMASK);

	return hItem ;
}

/*!
	�������� ��� ��������� �����������. ��������� ��� � ������. ���� ���������
	������������, �� ������������� ��� �������, ��� ��� � ��������(���� ����)
 	\param tree			- ������ �������
	\param hPar			- ������������ ����
 	\param pobj			- ������-�������� ���������
	\param ptrState		- ������ ��� �������
	\param uiImage		- ����������� ��� ����
 	\param hInsertAfter	- �������� ���� ������� ����� ����� ����
*/

HTREEITEM InsertState(CTreeCtrl& tree, HTREEITEM hPar, 
					  const qdGameObjectAnimated* pobj, 
					  qdGameObjectState* ptrState, UINT uiImage, 
						HTREEITEM hInsertAfter)
{
	if (uiImage == static_cast<UINT>(-1)) 
		uiImage = ptree_helpers::SelStatePic(ptrState, pobj);

	HTREEITEM hItem = 
		InsertNamedObject(tree, hPar, ptrState, uiImage, hInsertAfter);
	if (hItem) 
	{
		id(ptrState->is_default())
			tree.SetItemState(hItem, INDEXTOOVERLAYMASK(II_DEFAULT_STATE_OVERLAY), TVIS_OVERLAYMASK);
		else
			tree.SetItemState(hItem, 0, TVIS_OVERLAYMASK);

		tree.SetItemState(hItem, 0, TVIS_STATEIMAGEMASK);	
		if (uiImage != II_GLOBAL_STATE_NODE) {
			SetInTrigger(tree, hItem, ptrState->is_in_triggers(), II_NOT_ACTIVE_TRIGGER);
		}
	}
	return hItem;
}


//! ��������� � ��������� ������� ��� ��������� ���������� �������, ���������������
//! ������� \a _if
template<class Pred>
bool LoadStates2(CTreeCtrl& tree, HTREEITEM hStateCatalog, 
				 const qdGameObjectAnimated* pobj,
				 const qdGameObjectStateVector& vec, Pred _if)
{
	qdGameObjectStateVector::const_iterator _i = vec.begin(), _E = vec.end();
	while((_i = std::find_if(_i, _E, _if)) != _E)
	{
		InsertState(tree, hStateCatalog, pobj, 
			*_i, static_cast<UINT>(-1), TVI_LAST);
		++_i;
	}
	return true;
}

bool aoLoadStates(CTreeCtrl& tree, HTREEITEM hObjItem, 
					   qdGameObjectAnimated* pobj)
{
	return LoadStates2(tree, hObjItem, 
		pobj, pobj->state_vector(), all_states());
}

/*!
 	��������� ��� ���������� ��������� � ������� �������.
 	\param hWalkStatesCatalog - ������� ������� ��� ���������
*/ 
bool moLoadWalkStates(CTreeCtrl& tree, HTREEITEM hWalkStatesCatalog,
					  qdGameObjectMoving const* pobj)
{
	return LoadStates2(tree, hWalkStatesCatalog, pobj,
		pobj->state_vector(), all_walk());
}

/*!
 	��������� ��� ������������ ��������� � �������.
 	\param hNotWalkStatesCatalog - ������� ������� ��� ���������
*/ 
bool moLoadNotWalkStates(CTreeCtrl& tree, HTREEITEM hNotWalkStatesCatalog,
						 qdGameObjectMoving const* pobj)
{
	return LoadStates2(tree, hNotWalkStatesCatalog, pobj,
		pobj->state_vector(), all_not_walk());
}

bool moLoadStates(CTreeCtrl& tree, HTREEITEM hObjItem, qdGameObjectMoving* pobj)
{
	CString str((LPTSTR)IDS_STOPS_CATALOG);
	HTREEITEM hCatAnim = tree.InsertItem(str, hObjItem);
	
	tree.SetItemImage(hCatAnim, II_STATES_CATALOG, II_STATES_CATALOG);
	tree.SetItemData(hCatAnim, QE_STATE_CATALOG);
	tree.SetItemState(hCatAnim, 0, TVIS_STATEIMAGEMASK);	
	
	moLoadNotWalkStates(tree, hCatAnim, pobj);
	
	str.LoadString(IDS_GAITS_CATALOG);
	HTREEITEM hCatGait = tree.InsertItem(str, hObjItem);
	
	tree.SetItemImage(hCatGait, II_STATES_CATALOG, II_STATES_CATALOG);
	tree.SetItemData(hCatGait, QE_MOVEMENT_STATE_CATALOG);
	tree.SetItemState(hCatGait, 0, TVIS_STATEIMAGEMASK);	
		
	moLoadWalkStates(tree, hCatGait, pobj);

	tree.Expand(hCatGait, TVE_EXPAND);
	tree.Expand(hCatAnim, TVE_EXPAND);
	return true;
}

/*!
 	� ����������� �� ���� ������� �������� ptree_loading::aoLoadStates() ���
	ptree_loading::moLoadStates()
*/
bool LoadStates(CTreeCtrl& tree, HTREEITEM hObjItem, qdGameObject* pobj)
{
	if (ptree_helpers::is_obj_animated(pobj)||ptree_helpers::is_obj_mouse(pobj)) 
		return aoLoadStates(tree, hObjItem, static_cast<qdGameObjectAnimated*>(pobj));
	else if (ptree_helpers::is_obj_moving(pobj)) 
		return moLoadStates(tree, hObjItem, static_cast<qdGameObjectMoving*>(pobj));

	return false;
}

/*!
 	������� ��� ������� �������� � ��������� ��������� ������
*/
void moReloadWalkStates(CTreeCtrl& tree, HTREEITEM hCat,
						   const qdGameObjectMoving* pobj)
{
	ptree_helpers::DeleteSubItems(tree, hCat);
	moLoadWalkStates(tree, hCat, pobj);
}

/*!
 	������� ��� ������� �������� � ��������� ��������� ������
*/
void moReloadNotWalkStates(CTreeCtrl& tree, HTREEITEM hCat,
						   const qdGameObjectMoving* pobj)
{
	ptree_helpers::DeleteSubItems(tree, hCat);
	moLoadNotWalkStates(tree, hCat, pobj);
}

/*!
 	������� ��� ������� �������� � ��������� ��������� ������
*/
void moReloadStates(CTreeCtrl& tree, HTREEITEM hObjItem,
					qdGameObjectMoving* pobj)
{
	HTREEITEM hCat = ptree_helpers::GetStateCatalog(tree, hObjItem,
		qdGameObjectState::STATE_STATIC, QD_NAMED_OBJECT_MOVING_OBJ);
	ASSERT(hCat);
	moReloadNotWalkStates(tree, hCat, pobj);
	tree.Expand(hCat, TVE_EXPAND);
	
	hCat = ptree_helpers::GetStateCatalog(tree, hObjItem,
		qdGameObjectState::STATE_WALK, QD_NAMED_OBJECT_MOVING_OBJ);
	
	ASSERT(hCat);
	moReloadWalkStates(tree, hCat, pobj);

	tree.Expand(hCat, TVE_EXPAND);
}

/*!
 	������� ��� ������� �������� � ��������� ��������� ������
*/
void aoReloadStates(CTreeCtrl& tree, HTREEITEM hObjItem, 
					qdGameObjectAnimated* pobj)
{
	ptree_helpers::DeleteSubItems(tree, hObjItem);
	aoLoadStates(tree, hObjItem, pobj);
}

/*!
	� ����������� �� ���� ������� �������� ��� ptree_loading::aoReloadStates()
	��� ptree_loading::moReloadStates()
*/
void ReloadStates(CTreeCtrl& tree, HTREEITEM hObjItem,
				  qdGameObjectAnimated* pobj)
{
	if (ptree_helpers::is_obj_animated(pobj)||ptree_helpers::is_obj_mouse(pobj)) {
		aoReloadStates(tree, hObjItem, pobj);
	}
	else if (ptree_helpers::is_obj_moving(pobj)) {
		moReloadStates(tree, hObjItem, static_cast<qdGameObjectMoving*>(pobj));
	} 
}

/*!
 	� ����������� �� ������������ ���������� �������� ��� 
	ptree_loading::moReloadWalkStates() ��� ptree_loading::moReloadNotWalkStates()
	��� ptree_loading::aoReloadStates()
*/
void ReloadStateCatalog(CTreeCtrl& tree, HTREEITEM hCat, 
							qdGameObjectAnimated* pobj, bool bIsWalk)
{
	if (pobj->named_object_type()==QD_NAMED_OBJECT_MOVING_OBJ) {
		if(bIsWalk)
			moReloadWalkStates(tree, hCat, static_cast<qdGameObjectMoving*>(pobj));
		else
			moReloadNotWalkStates(tree, hCat, static_cast<qdGameObjectMoving*>(pobj));
	}
	else if (pobj->named_object_type() == QD_NAMED_OBJECT_ANIMATED_OBJ
		||pobj->named_object_type() == QD_NAMED_OBJECT_MOUSE_OBJ) 
		aoReloadStates(tree, hCat, pobj);
	
}

}//namespace ptree_loading
using namespace ptree_loading;
/************************************************************************/
/* ������� ����� ������                                                 */
/************************************************************************/

/*!
 	������� ���������� ����� �������, ����� ���� �������� ���� ����������� �� ��������
	\param hParent	- ������������ ���� � ������
	\param pmg		- ��������� �� ��������
*/
HTREEITEM SceneTreeView::InsertMGToTree(HTREEITEM hParent, const qdMiniGame* pmg)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hInsAfter = TVI_LAST;
	TreeItemIndex_t::iterator itr = 
		m_mapMinigameIndex.insert(TreeItemIndex_t::value_type(pmg->name(), NULL)).first;

	if (itr != m_mapMinigameIndex.begin()) {
		TreeItemIndex_t::iterator prev = boost::prior(itr);
		hInsAfter = prev->second;
	}
	else
	{
		TreeItemIndex_t::iterator next = boost::next(itr);
		if (next != m_mapMinigameIndex.end()) {
			hInsAfter = tree.GetPrevSiblingItem(next->second);
		}
	}
	ASSERT (itr != m_mapMinigameIndex.end());

	HTREEITEM hItem = tree.InsertItem(pmg->name(), hParent, hInsAfter);
	if(!hItem)
		return NULL;
	tree.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(pmg));
	tree.SetItemImage(hItem, II_MINIGAME_NODE, II_MINIGAME_NODE);
	SetNoCheck(hItem);

	itr->second = hItem;

	return hItem;
}

/*!
	�������� InsertMGToTree() ��� ������� �������� � ������
	\param hParent - ������������ ���� � ������
	\param pg - ��������
	\return true - ���� ������� ��������
*/
bool SceneTreeView::LoadMinigame(HTREEITEM hParent, qdMiniGame const* pg){
	HTREEITEM hGame = InsertMGToTree(hParent, pg);

	if(!hGame)
		return false;
	
	return true;
}

/*!
 	������� ���� ������� ��� ������� � ��� ������ �������� �������� ������� 
	�������� LoadMinigame().
	\param hParent - ������������ ���� � ������
	\param mgl - ������ �������
	\return true - ���� ������� ���������
 */
bool SceneTreeView::LoadMinigames(HTREEITEM hParent, const qdMiniGameList& mgl)
{
	typedef qdMiniGameList::const_iterator ci_t;

	HTREEITEM hMGs = CrtTypedNode(GetTreeCtrl(), hParent, IDS_MINIGAMES_NODE, 
							QD_NAMED_OBJECT_MINIGAME, II_MINIGAMES_NODE, TVI_FIRST);


	ci_t itr = mgl.begin(), end = mgl.end();
	for(; itr != end; ++itr){
		LoadMinigame(hMGs, *itr);
	}
	GetTreeCtrl().Expand(hMGs, TVE_EXPAND);

	return true;
}

/*!
 	������� ���� ������� ��� ����������� ������ � ��� ������� ����� �������� ������� 
	�������� LoadMusicTrack().
	\param hParent - ������������ ���� � ������
	\param mtl - ������ ����������� ������
	\return true - ���� ������� ���������
 */
bool SceneTreeView::LoadMusicTracks(HTREEITEM hParent, const qdMusicTrackList& mtl)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hMTs = CrtTypedNode(tree, hParent, IDS_MUSIC_TRACKS_NODE, 
						QD_NAMED_OBJECT_MUSIC_TRACK, II_MUSIC_TRACKS_NODE, TVI_LAST);
	if (!hMTs)
		return false;

	qdMusicTrackList::const_iterator i = mtl.begin(), e = mtl.end();
	for(; i != e; ++i)
		LoadMusicTrack(hMTs, *i);
	tree.Expand(hMTs, TVE_EXPAND);
	return true;
}

/*!
 	��������� ���� � ������, ������� InsertNamedObject() � ������� � ���� �������
 	\param hParent - ������������ ���� � ������.
 	\param pm - ����
 	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadMusicTrack(HTREEITEM hParent, qdMusicTrack * pm)
{
	HTREEITEM  hMT = InsertNamedObject(GetTreeCtrl(),hParent, pm, II_MUSIC_TRACK_NODE);
	if (!hMT)
		return false;
	SetNoCheck(hMT);
	SetInTrigger(GetTreeCtrl(), hMT, pm->is_in_triggers(), II_NOT_ACTIVE_TRIGGER);
	return true;
}

/*!
 	��������������� ��������� ������� � ������. ������������� �� �� ���������,
	������������� ����������� ��� �����, ��������� ��������� ������������ �������� 
	� ����������
	\param wndTree - ������ �� CTreeCtrl, � ������� ��� ������������
	\param hStaticObj - ������������ ���� � ������ ��� ����������� ��������
	\param hAnimatedObj - ������������ ���� � ������ ��� ������������� ��������
	\param hMovingObj - ������������ ���� � ������ ��� ���������
	\param gol - ������ ��������, ������� � ����� ���������
	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadGameObjects(CTreeCtrl& wndTree, 
									const qdGameObjectList& gol,
									HTREEITEM hStaticObj, 
									HTREEITEM hAnimatedObj,
									HTREEITEM hMovingObj)
{
	qdGameObjectList::const_iterator _itr = gol.begin(),_end = gol.end();
	HTREEITEM hPar = NULL;
	UINT uiImage = 0;
	for(; _itr != _end; ++_itr)
	{
		qdGameObject* pgo = *_itr;
		int type = pgo->named_object_type();
		switch(type) {
		case QD_NAMED_OBJECT_STATIC_OBJ:
			ASSERT(hStaticObj);
			hPar = hStaticObj;
			uiImage = ptree_helpers::SelStaticObjPic(pgo);
			break;
		case QD_NAMED_OBJECT_ANIMATED_OBJ:
			ASSERT(hAnimatedObj);
			hPar = hAnimatedObj;
			uiImage = ptree_helpers::SelAnimObjPic(pgo);
			break;
		case QD_NAMED_OBJECT_MOVING_OBJ:
			ASSERT(hMovingObj);
			hPar = hMovingObj;
			uiImage = II_MOVING_OBJ_NODE;
			break;
		default:
			continue;
		}
		HTREEITEM hObj = InsertNamedObject(wndTree, hPar, pgo, uiImage);
		if(!hObj)
			return false;
		
		LoadStates(wndTree, hObj, pgo);

		wndTree.Expand(hObj, TVE_EXPAND);
	}

	return true;
}
/*!
 	������� ����-��������, � ������� ����� ��������� ������� ���������������� ����
	\param wndTree - ������ �� CTreeCtrl, � ������� ��� ������������
	\param hParentItem - ������������ ���� � ������. ���� �����
	\param gol - ������ ��������, ������� � ����� ���������
	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadGameObjects(CTreeCtrl& wndTree, HTREEITEM hParentItem,
									const qdGameObjectList& gol)
{
	//������ ����, ����� ����� ����������� ����
	//������ �� �����
	HTREEITEM hStaticObj = CrtTypedNode(wndTree, hParentItem, 
		IDS_STATIC_OBJS_NODE, QD_NAMED_OBJECT_STATIC_OBJ, II_STATIC_OBJS_NODE);
	if(!hStaticObj)
		return false;
	
	HTREEITEM hAnimatedObj = CrtTypedNode(wndTree, hParentItem, 
						IDS_ANIMATED_OBJS_NODE, QD_NAMED_OBJECT_ANIMATED_OBJ, 
						II_ANIMATED_OBJS_NODE);
	if(!hAnimatedObj)
		return false;
	
	HTREEITEM hMovingObj = CrtTypedNode(wndTree, hParentItem, 
		IDS_MOVING_OBJS_NODE, QD_NAMED_OBJECT_MOVING_OBJ, II_MOVING_OBJS_NODE);
	if(!hMovingObj)
		return false;
	
	LoadGameObjects(wndTree, gol, hStaticObj, hAnimatedObj, hMovingObj);
	
	wndTree.Expand(hStaticObj, TVE_EXPAND);
	wndTree.Expand(hAnimatedObj, TVE_EXPAND);
	wndTree.Expand(hMovingObj, TVE_EXPAND);
	
	return true;
}
/*!
 	������� �����-�������� � �������� 
	bool LoadGameObjects(CTreeCtrl& wndTree, const qdGameObjectList& gol,
		HTREEITEM hStaticObj, HTREEITEM hAnimatedObj, HTREEITEM hMovingObj)
	��� �������� ���������� ��������
	\param hParent - ������������ ���� � ������.
	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadGlobalGameObjects(HTREEITEM hParent)
{
	CTreeCtrl& tree = GetTreeCtrl();
	
	HTREEITEM hMovingObj = CrtTypedNode(tree, hParent, 
		IDS_MOVING_OBJS_NODE, QD_NAMED_OBJECT_MOVING_OBJ, II_MOVING_OBJS_NODE,
		TVI_FIRST);
	if(!hMovingObj)
		return false;
	
	LoadGameObjects(tree, m_ptrGameDisp->global_object_list(), 
						NULL, NULL, hMovingObj);
	
	tree.Expand(hMovingObj, TVE_EXPAND);
	
	return true;
}

/*!
 	������� ����-������� ��� �����-������� � �������� LoadVideo() ��� �� ��������
	\param hParent - ������������ ���� � ������.
	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadVideo(HTREEITEM hParent)
{
	CTreeCtrl &tree = GetTreeCtrl();
	CString str((LPTSTR)IDS_VIDEO_CATALOG);
	
	HTREEITEM hCat = CrtTypedNode(tree, hParent, 
		IDS_VIDEO_CATALOG, QD_NAMED_OBJECT_VIDEO, II_VIDEO_CATALOG, TVI_FIRST);
	return LoadVideo(hCat, m_ptrGameDisp->video_list());
}

/*!
 	��������� �����-����� � ������
 	\param hPar - ������������ ���� � ������. 
 	\param vl - ������ ��������, ������� � ����� ���������
 	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadVideo(HTREEITEM hPar, const qdVideoList& vl)
{
	CTreeCtrl& tree = GetTreeCtrl();
	boost::rtl::crange<qdVideoList const> r(vl);
	for(; r; ++r)
	{
		qdVideo const& video = **r;
		HTREEITEM hItem = InsertNamedObject(tree, hPar, &video, II_VIDEO_NODE, TVI_SORT);
		SetNoCheck(hItem);
		SetInTrigger(tree, hItem, video.is_in_triggers(), II_NOT_ACTIVE_TRIGGER);
	}

	return true;
}

//! ���������� ����� ��� �������� ������ ������
bool SceneTreeView::LoadFonts(HTREEITEM hParent)
{
	CTreeCtrl &tree = GetTreeCtrl();

	HTREEITEM hCat = CrtTypedNode(tree, hParent, 
		IDS_FONTS_CATALOG, QD_NAMED_OBJECT_FONT_INFO, II_FONTS_CATALOG, TVI_FIRST);
	return LoadFonts(hCat, m_ptrGameDisp->fonts_list());
}
//! ��������� ������ ������
bool SceneTreeView::LoadFonts(HTREEITEM hPar, qdFontInfoList const& l)
{
	CTreeCtrl& tree = GetTreeCtrl();
	boost::rtl::crange<qdFontInfoList const> r(l);
	for(; r; ++r)
	{
		qdFontInfo const* fi = *r;
		HTREEITEM hItem = InsertNamedObject(tree, hPar, fi, II_FONT_NODE, TVI_SORT);
		SetNoCheck(hItem);
	}
	tree.Expand(hPar, TVE_EXPAND);
	return true;
}

/*!
 	������� ������� �������� �������. ��������� ���������� ������� �������� ���������
	������ �������
	\param ptrGD - ��������� �� ��������� ����
	\return ���������� ��������� �������� ����
*/
bool SceneTreeView::LoadStructTree(qdGameDispatcher* ptrGD)
{
	CWaitCursor __wait__;
	ASSERT(ptrGD);

	//������ ������ ���� ������
	ASSERT(!GetTreeCtrl().GetCount());
	m_ptrGameDisp = ptrGD;

	if(sndDispatcher* p = sndDispatcher::get_dispatcher())
		p -> disable();

	CString str;
	str.LoadString(IDS_GLOBAL_NODE);
	
	HTREEITEM hItem = GetTreeCtrl().InsertItem(str);
	GetTreeCtrl().SetItemData(hItem, reinterpret_cast<DWORD_PTR>(ptrGD));
	GetTreeCtrl().SetItemImage(hItem, II_GLOBAL_NODE, II_GLOBAL_NODE);

	GetTreeCtrl().SetItemState(hItem, 0, TVIS_STATEIMAGEMASK);	
	
	
	bool res = LoadScenes(hItem, m_ptrGameDisp);

	LoadGlobalGameObjects(hItem);

	lioLoadHelper(hItem, ptrGD->trigger_chain_list(), IDS_TRIGGERS_NODE, 
		QD_NAMED_OBJECT_TRIGGER_CHAIN, II_TRIGGERS_NODE, 
		II_TRIGGER_NODE,TVI_FIRST);

	LoadMouse(hItem, m_ptrGameDisp->mouse_object());

	LoadInventory(hItem);

	LoadCounters(hItem);

	LoadMinigames(hItem, m_ptrGameDisp->minigame_list());
	LoadVideo(hItem);
	LoadFonts(hItem);
	LoadInterface(hItem);

	
	GetTreeCtrl().SelectItem(hItem);
	GetTreeCtrl().Expand(hItem, TVE_EXPAND);

	return res;
}

/*!
 	��������� ������ ���� �� ���������� ���� � ������� �������
	LoadScene(HTREEITEM hPar, qdGameScene* pgs).
	\param hParent - ������������ ���� � ������. 
	\param ptrDisp - ��������� �� ��������� ����
	\return \a true ���� ������� ���������

 */
bool SceneTreeView::LoadScenes(HTREEITEM hParentItem, qdGameDispatcher* ptrDisp)
{
	CTreeCtrl& wndTree = GetTreeCtrl();
	//������ ���� ����� ��� ��� ����
	HTREEITEM hGenItem = hParentItem;
	qdGameSceneList::const_iterator _itr = ptrDisp->scene_list().begin(),
									_end = ptrDisp->scene_list().end();
	
	for(; _itr != _end; ++_itr)
	{
		qdGameScene* pgs = *_itr;
		HTREEITEM hScene = LoadScene(hGenItem, pgs);
		if(!hScene)
			return false;
	}
	return true;
}

/*!
 	��������� ����, ��� ����� �� ��� ���������� �� �������� ����� ����� ����.
	��������� ���������� ����� � ������(SceneTreeView::LoadSceneObjs())
	\param hPar - ������������ ���� � ������.
	\param pgs	- ��������� �� �����.
	\return ���� �����
*/
HTREEITEM SceneTreeView::LoadScene(HTREEITEM hPar, qdGameScene* pgs)
{
	CTreeCtrl& wndTree = GetTreeCtrl();
	HTREEITEM hInsAfter = TVI_LAST;
	TreeItemIndex_t::iterator itr = 
		m_mapSceneIndex.insert(TreeItemIndex_t::value_type(pgs->name(), NULL)).first;
	
	if (itr != m_mapSceneIndex.begin()) {
		hInsAfter = boost::prior(itr)->second;
	}
	else
	{
		TreeItemIndex_t::iterator next(boost::next(itr));
		if (next != m_mapSceneIndex.end()) {
			hInsAfter = wndTree.GetPrevSiblingItem(next->second);
			if (!hInsAfter)
				hInsAfter = TVI_FIRST;
		}
	}

	ASSERT (itr != m_mapSceneIndex.end());


	HTREEITEM hItem = wndTree.InsertItem(pgs->name(), hPar, hInsAfter);
	if(!hItem)
		return NULL;
	wndTree.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(pgs));
	wndTree.SetItemImage(hItem, II_SCENE_NODE, II_SCENE_NODE);

	SetNoCheck(hItem);

	if (pgs->is_in_triggers())
		SetInTrigger(wndTree, hItem, TRUE, II_NOT_ACTIVE_TRIGGER);


	itr->second = hItem;

	if(!LoadSceneObjs(hItem, pgs))
		return NULL;
	
	return hItem;
}

/*!
 	��������� ���. ����� � ������� LoadMusicTracks(), ������ ����� LoadGameObjects(),
	���� ����� - LoadGridZones()
	\param hParentItem - ������������ ���� � ������. 
	\param ptrGS - ��������� �� �����
	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadSceneObjs(HTREEITEM hParentItem, qdGameScene* ptrGS)
{
	CTreeCtrl& wndTree = GetTreeCtrl();

	LoadMusicTracks(hParentItem, ptrGS->music_track_list());
	//�������� ���� �����
	CString str;
	str.LoadString(IDS_NET_NODE_NAME);
	HTREEITEM hNetItem = wndTree.InsertItem(str, hParentItem, TVI_LAST);

	if(hNetItem)
	{
		wndTree.SetItemData(hNetItem, QD_NAMED_OBJECT_GRID_ZONE);
		wndTree.SetItemImage(hNetItem, II_NET_NODE, II_NET_NODE);
		LoadGridZones(hNetItem, ptrGS);
		wndTree.Expand(hNetItem, TVE_EXPAND);
		SetCheck(hNetItem, TRUE);
	}

	LoadGameObjects(wndTree, hParentItem, ptrGS->object_list());

	return true;
}

/*!
 	������� ���� ��� ���� (CrtTypedNode()) � ����� ������ ��� ���������
	(ptree_loading::aoLoadStates())
	\param hParentItem - ������������ ���� � ������. 
	\param ptrMouse - ��������� �� ������ ����
	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadMouse(HTREEITEM hParentItem, 
							  qdGameObjectMouse* ptrMouse)
{
	CTreeCtrl& wndTree = GetTreeCtrl();
	HTREEITEM hMouseObj = CrtTypedNode(wndTree, hParentItem,
					IDS_MOUSE_OBJ_NODE, reinterpret_cast<DWORD_PTR>(ptrMouse), 
					II_MOUSE_NODE, TVI_FIRST);
	if(!hMouseObj)
		return false;

	if (ptree_loading::aoLoadStates(wndTree, hMouseObj, ptrMouse)) {
//		wndTree.Expand(hMouseObj, TVE_EXPAND);
		return true;
	}
	return false;
}

/*!
 	��������� ���� � ������ � ������� InsertNamedObject() � ��������� ���� � �������
	InsertGZStates()
	\param tree - ������ �� CTreeCtrl, � ������� ��� ������������
	\param hParentItem - ������������ ���� � ������. 
	\param pz - ��������� �� ���� �����
	\param iItemImage - ����� �������� ��� ���� ����
	\param hInsertAfter - ���� �����, �������� ����������� 
	\return \a true ���� ������� ���������
 */
HTREEITEM SceneTreeView::InsertGZ2Tree(CTreeCtrl& tree, HTREEITEM hParentItem, 
							qdGridZone* pz, int iItemImage, HTREEITEM hInsertAfter)
{
	HTREEITEM hNode = InsertNamedObject(tree, hParentItem, pz, iItemImage, hInsertAfter);
	if(!hNode)
		return NULL;
	
	InsertGZStates(tree, hNode, pz);
	tree.Expand(hNode, TVE_EXPAND);
	return hNode;
}

/*!
	\param tree - ������ �� CTreeCtrl, � ������� ��� ������������
	\param hParent - ������������ ���� � ������. 
	\return \a true ���� ������� ���������
 */
bool SceneTreeView::InsertGZStates(CTreeCtrl& tree, HTREEITEM hZoneItem, qdGridZone* pz)
{
	HTREEITEM hRes = tree.InsertItem(qdGridZoneState::ZONE_STATE_OFF_NAME, 
							II_GZONE_STATE_NODE, II_GZONE_STATE_NODE, hZoneItem);
	if (!hRes)
		return false;

	qdGridZoneState* pst = pz->get_state(qdGridZoneState::ZONE_STATE_OFF_NAME);
	tree.SetItemData(hRes, reinterpret_cast<DWORD_PTR>(pst));
	SetInTrigger(tree, hRes, pst->is_in_triggers(), II_NOT_ACTIVE_TRIGGER);

	hRes = tree.InsertItem(qdGridZoneState::ZONE_STATE_ON_NAME, II_GZONE_STATE_NODE, 
										II_GZONE_STATE_NODE, hZoneItem);
	if (!hRes)
		return false;

	pst = pz->get_state(qdGridZoneState::ZONE_STATE_ON_NAME);
	tree.SetItemData(hRes, reinterpret_cast<DWORD_PTR>(pst));
	SetInTrigger(tree, hRes, pst->is_in_triggers(), II_NOT_ACTIVE_TRIGGER);

	return true;
}

/*!
 	��������� �� ������ ���, ���������� ����������� ��� 
	����(ptree_helpers::SelGZonePic()) � ��������� � ������ ����(InsertGZ2Tree())
	\param hParentItem - ������������ ���� � ������. 
	\param ptrScene - �����, ���� ������� ����� ���������
	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadGridZones(HTREEITEM hParentItem, qdGameScene const* ptrScene)
{
	const qdGridZoneList& zl = ptrScene->grid_zone_list();
	qdGridZoneList::const_iterator _itr = zl.begin(), _end = zl.end();

	CTreeCtrl& tree = GetTreeCtrl();
	for(; _itr != _end; ++_itr)
	{
		int const iImage = ptree_helpers::SelGZonePic(*_itr);
		if (!InsertGZ2Tree(tree, hParentItem, *_itr, iImage, TVI_LAST))
			return false;
	}
	return true;
}

/*!
 	�������� SceneTreeView::lioLoadHelper() ��� �������� ������
	\param hParent - ������������ ���� � ������. 
	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadInventory(HTREEITEM hParent)
{
	return lioLoadHelper(hParent, m_ptrGameDisp->inventory_list(),
		IDS_INVENTORY_OBJS_NODE, QD_NAMED_OBJECT_INVENTORY, 
		II_INVENTORY_OBJS_NODE, II_INVENTORY_OBJ_NODE, TVI_FIRST);
}

/*!
 	��������� ������ �������� �� ���������� ���������:
	- ������� ����-������� ��������� ����
	- ��������� ��� ���� �������, ��������� InsertObject()
	- � ������� �������
	\param hItemPar - ������������ ���� � ������. 
	\param lst - ������ ��������.(��� ������ ���� ������ ����������. 
			����� ������� ����� ������������� ����� name())
	\param strId - ������������� ������� ������. ��� ������ �������� ����� ��� 
				���� ��������
	\param ot - (object type) ��� ��������. ������������ ��� �������� ����-��������
	\param uiObjsImage - ����� ����������� ��� ����-��������
	\param uiObjImage - ����� ����������� ��� ����� ��������
	\param hInsertAfter - ���� �����, �������� ����� ��������� ����-�������
	\return \a true ���� ������� ���������
 */
template<class _List>
bool SceneTreeView::lioLoadHelper(HTREEITEM hItemPar, const _List &lst, 
								  DWORD strId, qdNamedObjectType ot, 
								  UINT uiObjsImage, UINT uiObjImage,
								  HTREEITEM hInsertAfter)
{
	CTreeCtrl& tree = GetTreeCtrl();
	
	HTREEITEM hParent = CrtTypedNode(tree, hItemPar,strId,ot, uiObjsImage, 
		hInsertAfter);
	if(!hParent)
		return false;
	
	if(lst.empty())
		return true;
	
	_List::const_iterator _itr = lst.begin(),
		_end = lst.end();
	for(; _itr != _end; ++_itr)
	{
		_List::value_type pa = *_itr;
		HTREEITEM hItem = InsertObject(tree, hParent, pa, uiObjImage, TVI_LAST);
		if (hItem) 
			SetNoCheck(hItem);
	}
	tree.Expand(hParent, TVE_EXPAND);
	return true;
}

/*!
 	��������� �������� � ������, ������� CounterLoader::load()
	\param hParent - ������������ ���� � ������. 
	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadCounters(HTREEITEM hParent)
{
	return CounterLoader::load(
		GetTreeCtrl(), hParent, m_ptrGameDisp->counter_list(), TVI_FIRST);
	return true;
}

/************************************************************************/
/* �������� ����������                                                  */
/************************************************************************/

/*!
 	��������� ������ �������� ������� ������� GameEndLoader::load().
	\param hParent - ������������ ���� � ������. 
	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadGameEnds(HTREEITEM hParent)
{
	CTreeCtrl& tree = GetTreeCtrl();
	return GameEndLoader::load(tree, hParent, m_ptrGameDisp->game_end_list(), TVI_FIRST);
}

/*!
	������ ����-������� ��� ����������. ������ � ���� ������������ ������
	(LoadInterfaceScreens()) � ������ �������� �������(LoadInterfaceScreens())
	\param hParent - ������������ ���� � ������. 
	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadInterface(HTREEITEM hParent)
{
	CTreeCtrl& tree = GetTreeCtrl();
	CString strIntrf((LPTSTR)IDS_INTERFACE_NODE);

	qdInterfaceDispatcher* pIDisp = qdInterfaceDispatcher::get_dispatcher();
	if (!pIDisp) 
		return false;

	HTREEITEM hItem = tree.InsertItem(strIntrf, II_INTERFACE_NODE, 
									II_INTERFACE_NODE, hParent, TVI_FIRST);
	if (!hItem)
		return false;
	tree.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(pIDisp));

	LoadInterfaceScreens(hItem, pIDisp);
	LoadGameEnds(hItem);
	return true;
}

/*!
 	��������� � ������ ���� ��� ������(ptree_loading::InsertObject()), � ����� 
	������ ���������� ����� (LoadInterfaceScreen())
	\param hParent - ������������ ���� � ������. 
	\param pIDisp - ��������� �� ���������-���������
	\return \a true ���� ������� ���������
 */
bool SceneTreeView::LoadInterfaceScreens(HTREEITEM hParent,
									const qdInterfaceDispatcher* pIDisp)
{
	typedef qdInterfaceDispatcher::screen_list_t list_t;
	const list_t& l = pIDisp->screen_list();
	list_t::const_iterator itr = l.begin(), end = l.end();
	CTreeCtrl& tree = GetTreeCtrl();
	for(;itr != end;++itr){
		qdInterfaceScreen* pis = *itr;
		HTREEITEM hScreen = InsertObject(tree, hParent, pis, II_INTERFACE_SCEREEN_NODE);
		if (hScreen) 
		{
			LoadInterfaceScreen(hScreen, pis);
			tree.Expand(hScreen, TVE_EXPAND);
		}
	}
	return false;
}

/*!
	��������� �� ������ ��������� ������ � ��������� �� � ������(LoadInterfaceElement()) 	
	\param hParent - ������������ ���� � ������. 
	\param psi - ��������� �� �����
	\return \a true ���� ������� ���������
*/
bool SceneTreeView::LoadInterfaceScreen(HTREEITEM hParent, qdInterfaceScreen* psi)
{
	CTreeCtrl& tree = GetTreeCtrl();
	typedef qdInterfaceScreen::element_list_t list_t;
	const	list_t& l = psi->element_list();
	list_t::const_iterator itr = l.begin(), end = l.end();
	for(;itr != end; ++itr){
		qdInterfaceElement* pEle = *itr;
		LoadInterfaceElement(hParent, pEle);
	}
	return true;
}

/*!
 	��������� ������� � ������(ptree_loading::InsertObject()). ���� ������� ������,
	�� ������ ��������� ������(LoadInterfaceBtnStates())
	\param hParent - ������������ ���� � ������. 
	\param pele - ��������� �� �������
	\return \a true ���� ������� ���������
*/
bool SceneTreeView::LoadInterfaceElement(HTREEITEM hParent, qdInterfaceElement* pele)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hElement = 
		InsertObject(tree, hParent, pele, II_INTERFACE_ELEMENT_NODE);
	if (!hElement)
		return false;
	if(hElement)
	{
		tree.SetItemState(hElement, 
			INDEXTOSTATEIMAGEMASK(pele->is_visible()?2:1), TVIS_STATEIMAGEMASK);
	}
	if (pele->get_element_type() == qdInterfaceElement::EL_BUTTON)
	{
		bool res = 
			LoadInterfaceBtnStates(hElement, static_cast<qdInterfaceButton*>(pele));
		tree.Expand(hElement, TVE_EXPAND);
		return res;
	}

	return true;
}

/*!
 	��������� �� ������ ��������� ������ � 
	��������� �� � ������(ptree_loading::InsertObject())
	\param hParent - ������������ ���� � ������. 
	\param pbtn - ��������� �� ������
	\return \a true ���� ������� ���������
*/
bool SceneTreeView::LoadInterfaceBtnStates(HTREEITEM hParent, 
									qdInterfaceButton const* pbtn)
{
	CTreeCtrl& tree = GetTreeCtrl();
	int const count = pbtn->num_states();
	for(int i = 0; i < count; ++i)
	{
		const qdInterfaceElementState* const pstate = pbtn->get_state(i);
		HTREEITEM hState = 
			InsertObject(tree, hParent, pstate, II_INTERFACE_ELEMENT_STATE);
	}
	return true;
}

