/********************************************************************
created:	2002/09/11
created:	11:9:2002   11:00
filename: 	D:\Projects\Quest\QuestEditor\ptree_add.cpp
file path:	D:\Projects\Quest\QuestEditor
file base:	ptree_add
file ext:	cpp
Powerd by:	�����

  purpose:	������� �������������� ���������� � ������
  � � ������
*********************************************************************/


#include "stdafx.h"
#include "questeditor.h"

#include "font_generator.h"

#include "qd_game_object_static.h"
#include "qd_game_object_animated.h"
#include "qd_game_object_moving.h"
#include "qd_trigger_chain.h"
#include "qd_animation_set.h"

#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"
#include "qd_interface_button.h"
#include "qd_interface_background.h"
#include "qd_interface_slider.h"
#include "qd_interface_save.h"
#include "qd_interface_text_window.h"
#include "qd_game_end.h"
#include "qd_counter.h"
#include "qd_font_info.h"

#include "qd_grid_zone.h"
#include "qd_minigame.h"
#include "qd_video.h"
#include "qd_sprite.h"
#include "qd_setup.h"
#include "qd_music_track.h"

#include "name_op.h"
#include "relativepath.h"

#include "SceneTreeView.h"

#include "staticobjpage.h"
#include "ptree_hlprs.hpp"
#include "ptree_loading.h"

#include "disk_op.h"

/************************************************************************/
/* ������� �����������                                                  */
/************************************************************************/
void ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID = -1);
void ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID = -1);

qdAnimation* CreateNewAnimation(HWND hPar, qdGameDispatcherBase* ptrDisp);

bool AskUserForName(CString& strName);
const CString AskUserForName(const CString& strDef);

/************************************************************************/
/* ���������� �������                                                   */
/************************************************************************/

namespace ptree_add
{
/*!
 	���������� ������� ���������. ����� �� ��� ������ ������ ��������
*/
Vect3f GetPersonagePos(qdGameScene* ptrScene, float z)
{
	qdCamera* ptrCam = ptrScene->get_camera();
	
	const qdGameObjectList& ol = ptrScene->object_list();
	qdGameObjectList::const_iterator _itr = ol.begin(), _end = ol.end();
	
	float min_d = 1000000.f;
	
	Vect3f pos(0, 0, z/2.f);
	for(; _itr != _end; ++_itr)
	{
		Vect3f cam_pos = ptrCam->global2camera_coord((*_itr)->R());
		if (min_d > cam_pos.z) {
			min_d = cam_pos.z;
		}
	}
	
	Vect3f obj_pos = ptrCam->global2camera_coord(pos);
	obj_pos.z = min_d - 1.f;
	
	return ptrCam->camera_coord2global(obj_pos);
}

/*!
 	������������ ������� �������. ������� �� 3� ����� �������� ����� ������������� 
	���� �������
*/
const Vect3f get_obj_start_pos(qdGameScene* ptrScene, qdNamedObjectType type, 
							   const Vect3f& creation_pos){
	qdCamera* pcam = ptrScene->get_camera();
	Vect3f pos = pcam->global2camera_coord(creation_pos);
	switch(type) {
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
		pos.z = START_DEPTH_FOR_ANIM_OBJ;
		break;
	case QD_NAMED_OBJECT_MOVING_OBJ:
		pos.z = START_DEPTH_FOR_MOVING_OBJ;
		break;
	case QD_NAMED_OBJECT_STATIC_OBJ:
		pos.z = START_DEPTH_FOR_STATIC_OBJ;
		break;
	}

	return pcam->camera_coord2global(pos);
}

//! ��������� ��������� �� ��������� ��� �����
void SetSceneNetDefaults(qdGameScene* ptrScene)
{
	qdCamera* ptrCam = ptrScene->get_camera();
	
	CQuestEditorApp* app = GetMyApp();
	ptrCam->set_focus(static_cast<float>(app->GetIntSetting(SETTING_NAME(NET_FOCUS))));
	ptrCam->rotate_and_scale(
		static_cast<float>(app->GetIntSetting(SETTING_NAME(NET_ANGLE_X))),
		static_cast<float>(app->GetIntSetting(SETTING_NAME(NET_ANGLE_Y))),
		static_cast<float>(app->GetIntSetting(SETTING_NAME(NET_ANGLE_Z))),1.f,1.f,1.f);
	
	Vect3f pos = ptrCam->get_grid_center();
	pos.z = static_cast<float>(app->GetIntSetting(SETTING_NAME(NET_VERT_OFFSET)));
	ptrCam->set_grid_center(pos);
	
	ptrCam->scale_grid(app->GetIntSetting(SETTING_NAME(NET_SX)),
		app->GetIntSetting(SETTING_NAME(NET_SY)), 
		app->GetIntSetting(SETTING_NAME(NET_CELL_SZ)),
		app->GetIntSetting(SETTING_NAME(NET_CELL_SZ)));
	
}

//! �� ���� ������� ���������� ��� �� ���������
CString GetDefaultObjName(qdGameObject* ptrGO)
{
	UINT resID = 0;
	switch(ptrGO->named_object_type()) {
	case QD_NAMED_OBJECT_MOVING_OBJ:
		resID = IDS_NEW_MOVING_OBJ_NAME;
		break;
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
		resID = IDS_NEW_ANIMATED_OBJ_NAME;
		break;
	case QD_NAMED_OBJECT_STATIC_OBJ:
		resID = IDS_NEW_STATIC_OBJ_NAME;
		break;
	default:
		ASSERT(("GetDefaultObjName", 0));
	}
	if(resID)
		return CString((LPTSTR)resID);
	return CString();
}

//! ��������� ������� ����� ��� �������
CurDirSafer& MGCurDirSafer(){
	static CurDirSafer _s_cur_dir;
	return _s_cur_dir;
}

//! ��������� ������� ����� ��� ����������� ������
CurDirSafer& MTCurDirSafer(){
	static CurDirSafer _s_cur_dir;
	return _s_cur_dir;
}

CurDirSafer& FontCurDirSafer(){
	static CurDirSafer safer;
	return safer;
}
//! ��������� ������� ����� ��� ����������
CurDirSafer& INTERFCurDirSafer(){
	static CurDirSafer _s_cur_dir;
	return _s_cur_dir;
}

//! �������� ��� ��� ��� ��������
const CString MGGetDLLName(){
	CString strFilter((LPTSTR)IDS_OPEN_MINIGAME_DLL_FILTER);
	CString strTitle((LPTSTR)IDS_OPEN_MINIGAME_DLL_TITLE);
	return disk_op::Get1DiskFileName(MGCurDirSafer, strFilter, _T("*.dll"), strTitle);
}

//! �������� ��� ������-����� ��� ��������
const CString MGGetCNFGName(){
	CString strFilter((LPTSTR)IDS_OPEN_MINIGAME_CONFIG_FILTER);
	CString strExt((LPTSTR)IDS_OPEN_MINIGAME_CONFIG_EXT);
	CString strTitle((LPTSTR)IDS_OPEN_MINIGAME_CONFIG_TITLE);
	return disk_op::Get1DiskFileName(MGCurDirSafer, strFilter, strExt, strTitle);
}

//! �������� ��� ����� ��� ���. �����
const CString GetMusicTrackFileName(){
	CString strFilter((LPTSTR)IDS_OPEN_MUSIC_TRACK_FILTER);
	CString strExt((LPTSTR)IDS_OPEN_MUSIC_TRACK_EXT);
	CString strTitle((LPTSTR)IDS_OPEN_MUSIC_TRACK_TITLE);
	return disk_op::Get1DiskFileName(MTCurDirSafer, strFilter, strExt, strTitle);
}

//! �������� ��� ����� ������
const CString GetFontFileName(){
	CString strFilter((LPTSTR)IDS_OPEN_FONT_FILE_FILTER);
	CString strExt((LPTSTR)IDS_OPEN_FONT_FILE_EXT);
	CString strTitle((LPTSTR)IDS_OPEN_FONT_FILE_TITLE);
	return disk_op::Get1DiskFileName(FontCurDirSafer, strFilter, strExt, strTitle);
}
//! �������� ��� ����� ��������
const CString GetAnimFileName(){
	CString strTitle((LPTSTR)IDS_OSD_CAP_SEL_ANIM);
	CString strFilter((LPTSTR)IDS_OPEN_ANIMATION_FILTER);
	CString strExt((LPTSTR)IDS_OPEN_ANIMATION_EXT);
	return disk_op::Get1DiskFileName(INTERFCurDirSafer, strFilter, strExt, strTitle);
}

//! \brief ���������� �������, ������� ���� ����������, �� �� ������������ ������� ��� 
//! ������ ����
int GetInterfEleDepthOverBack(qdInterfaceScreen const* pscr, int def)
{
	int res = def;
	typedef qdInterfaceScreen::element_list_t ele_lst_t;
	ele_lst_t const& lst = pscr->element_list();
	ele_lst_t::const_iterator i = lst.begin(), e = lst.end();
	for(; i != e; ++i)
	{
		qdInterfaceElement const* const pele = *i;
		if (pele->get_element_type()==qdInterfaceElement::EL_BACKGROUND
			&&pele->screen_depth()<res)
			res = pele->screen_depth();
	}
	return res-1;
}

/*!
�����-�������� ��� ���������� ������������ ��������� � ������������ �����. ��� �� - 
������, ���������, �����...
�������� ���: ������� ����� �������, �������� ���������� ��� ��� ����, ��������� � 
������ ������������ ���������.
*/
template<class EleType>
class add_ele
{
public:
	EleType* operator()(qdInterfaceScreen* ps, const CString& name){
		std::auto_ptr<EleType> pe(new EleType);
		if (!pe.get())
			return NULL;

		CString uname = get_unique_name(name, ps->element_list());
		pe->set_name(uname);
		if (ps->add_element(pe.get()))
		{
			pe->set_owner(ps);
			return pe.release();
		}
		return NULL;
	}
};

}//namespace ptree_add

using namespace ptree_add;
//////////////////////////////////////////////////////////////////////////

/*!
 	�� ������ \a strName �������� ���������� ���, ����� ������� �����, ��������� � �
	���������(\a ptrDisp) � ������������� �������� �� ��������� ��� �����
	(ptree_add::SetSceneNetDefaults())
	\param ptrDisp - ��������� ����, ���� ����� ��������� ����� �����
	\param strName - ��� ����� �����. ����������, ���� ����� ��� ��� ����
	\return ��������� �� ����� ������, ���� NULL - � ������ �������
*/
qdGameScene* SceneTreeView::CreateScene( qdGameDispatcher* ptrDisp, CString& strName)
{
	if(strName.IsEmpty())
		strName.LoadString(IDS_NEW_SCENE_NAME);
	
	strName = get_unique_name(strName, ptrDisp->scene_list());
	qdGameScene* ptrs = new qdGameScene;
	if(!ptrs)
	{
		CString str;
		str.LoadString(IDS_ERR_MSG_CAPTION);
		CString msg;
		msg.LoadString(IDS_CNNT_CRT_SCENE);
		MessageBox(msg, str, MB_OK);
		return NULL;
	}
	
	ptrs->set_name(strName);
	ptrDisp->add_scene(ptrs);
	
	SetSceneNetDefaults(ptrs);
	
	return ptrs;
}

/*!
 	�������� �� ������ ������� ������ �������. ��������� ���������� ��������� 
	qdGameObjectStateWalk(ptree_api::AddEmptyMovementState()), ��������� ���(���������) 
	�������. ����������� ������� ���������(qdGameObjectMoving::auto_bound()) � 
	��������� �� �����(qdGameObjectMoving::adjust_z())
	\return \a true - ���� ������� ��������.
*/
bool SceneTreeView::InsertFirstGaitToPersonage(qdGameObjectMoving* ptrPers)
{
	if (m_ptrGameDisp->animation_set_list().empty())
		return false;

	qdAnimationSet *ptrAS = m_ptrGameDisp->animation_set_list().front();
	if(!ptrAS)
		return false;

	qdGameObjectStateWalk* pstate = ptree_api::AddEmptyMovementState(ptrPers);
	if(!pstate)
		return false;

	pstate->animation_set_info()->set_name(ptrAS->name());
	
	ptrPers->set_default_state();
	ptrPers->auto_bound();
	ptrPers->adjust_z();
	return true;
}

/*!
 	������� ������ "GameObj" � ��������� ��� ������� (0, 0, 0)
	\return ��������� �� ��������� ������
*/

template< class GameObj>
GameObj* SceneTreeView::CreateGameObject(DWORD dwErrMsg)
{
	GameObj *pobj = new GameObj;
	if(!pobj)
	{
		ShowError(m_hWnd, dwErrMsg);
		return NULL;
	}
	pobj->set_pos(Vect3f(0, 0, 0));
	return pobj;
}

/*!
 	�������� ���������� SceneTreeView::CreateGameObject(DWORD dwErrMsg)
*/
qdGameObjectMoving* SceneTreeView::CreatePersonage()
{
/*	qdGameObjectMoving *pobj = new qdGameObjectMoving;
	if(!pobj)
	{
		ShowError(m_hWnd, IDS_CNNT_CRT_MOVING_OBJ);
		return NULL;
	}
	pobj->set_pos(Vect3f(0, 0, 0));
	return pobj;
*/
	return CreateGameObject<qdGameObjectMoving>(IDS_CNNT_CRT_MOVING_OBJ);
}

/*!
 	�������� ���������� SceneTreeView::CreateGameObject(DWORD dwErrMsg)
*/
qdGameObjectAnimated* SceneTreeView::CreateAnimated()
{
/*	qdGameObjectAnimated* pobj = new qdGameObjectAnimated;
	if(!pobj)
	{
		ShowError(m_hWnd, IDS_CNNT_CRT_ANIMATED_OBJ);
		return NULL;
	}
	pobj->set_pos(Vect3f(0, 0, 0));
	
	return pobj;
*/
	return CreateGameObject<qdGameObjectAnimated>(IDS_CNNT_CRT_ANIMATED_OBJ);
}

/*!
 	�������� ���������� SceneTreeView::CreateGameObject(DWORD dwErrMsg)
*/
qdGameObjectStatic* SceneTreeView::CreateStatic()
{
/*	qdGameObjectStatic* pobj = new qdGameObjectStatic;
	if(!pobj)
	{
		ShowError(m_hWnd, IDS_CNNT_CRT_STATIC_OBJ);
		return NULL;
	}
	pobj->set_pos(Vect3f(0, 0, 0));
	
	return pobj;
*/
	return CreateGameObject<qdGameObjectStatic>(IDS_CNNT_CRT_STATIC_OBJ);
}

/*!
 	������� ����������� ������(SceneTreeView::CreateStatic()). ��������� ��� � �����(
	SceneTreeView::AddSceneObj()). ��������� ������ ������ �����������(
	StaticObjPage::SelectPicture()). ���� ����������� �������, �� ��������� ��� �������,
	��������� ��� ������ ������� ������� �����. ����������� ������� ������� �
	�������� �����. ���� �������� �� �������, �� ������� ��������� ������
	\param hSceneItem	- ���� �����, � ������� ����� �������� ������
	\param ptrScene		- �����, � ������� ����� �������� ������
	\return \a true - ���� ������ ��� ������ � ��������
*/
bool SceneTreeView::CreateBackground(HTREEITEM hSceneItem, qdGameScene* ptrScene)
{
	//�� ����� ������� �������, � ������� ��������� ��� ����������� �������
	CString str;
	str.LoadString(IDS_STATIC_OBJS_NODE);
	HTREEITEM hCatalog = 
		ptree_helpers::FindTreeItemByText(GetTreeCtrl(), hSceneItem,str);
	
	ASSERT(hCatalog);

	qdGameObjectStatic* ptrStatObj(CreateStatic());
	if(!AddSceneObj(ptrStatObj, hCatalog, 
		CString((LPTSTR)IDS_NEW_BACKGROUND_OBJ_NAME),II_STATIC_OBJ_NODE))
		return false;
	
	CString picture = StaticObjPage::SelectPicture(this->m_hWnd, 
									CString((LPTSTR)IDS_OSD_CAP_SEL_BACK));
	bool res = false;
	if(!picture.IsEmpty())
	{
		qdSprite* psprite = ptrStatObj->get_sprite();
		ASSERT(psprite);
		psprite->set_file(picture);
		ptrStatObj->load_resources();
		
		Vect2s obj_size = ptrStatObj->screen_size();
		
		qdCamera* pcam = ptrScene->get_camera();
		pcam->set_scr_size(obj_size.x, obj_size.y);
		
		obj_size.x = pcam->get_scr_center_x();
		obj_size.y = pcam->get_scr_center_y();
		
		obj_size = pcam->scr2rscr(obj_size);
		ptrStatObj->set_pos(pcam->rscr2global(obj_size, 
			ptrScene->get_camera()->get_R()+DEF_BACKGROUND_DEPTH));
		
		AdjustBitmapSize();
		res = true;
	}
	else
		DeleteObject(ptrStatObj, false);
	Invalidate();
	return res;
}

/*!
 	��������� ������ ������ �����������(StaticObjPage::SelectPicture()).  ���� �������, 
	��: ������� ����������� ������(
	SceneTreeView::CreateStatic()); ��������� ��� � �����(SceneTreeView::AddSceneObj());
	��������� ��� �����������; ������ ������ � ������ ��������������� �� ������� �������.
	��� ��� ����������� � �����. ���� �����������, ���� ����������� ��� �������� �� ����
	�������. ����� ������ �� ����� ������� ��������� �� ������������� ������ ��������
	� ��������� �� ������� � ������������� �� �������. ��� ������ �������, ��� ������ 
	�������
	\param hSceneItem - ���� �����, � ������� ����������� �������
	\retval true - ������
*/
bool SceneTreeView::AddStatics(HTREEITEM hSceneItem)
{
	CTreeCtrl& tree = GetTreeCtrl();
	qdGameScene* ptrScene = (qdGameScene*)tree.GetItemData(hSceneItem);
	
	//�� ����� ������� �������, � ������� ��������� ��� ����������� �������
	CString str;
	str.LoadString(IDS_STATIC_OBJS_NODE);
	HTREEITEM hCatalog = ptree_helpers::FindTreeItemByText(GetTreeCtrl(), hSceneItem,str);
	ASSERT(hCatalog);

	CStringVect_t vecFiles;
	std::multimap<int, qdGameObject*> objSort;
	CString strOpenDlgTitle((LPTSTR)IDS_OSD_CAP_SEL_STAT_OBJS);
	while (true) {
		vecFiles.clear();
		//�������� ��������
		CString picture = 
			StaticObjPage::SelectPicture(this->m_hWnd, strOpenDlgTitle, &vecFiles);
		if(picture.IsEmpty())
			break;
		
		CStringVect_t::iterator _itr = vecFiles.begin(), _end = vecFiles.end();
		for(; _itr != _end; ++_itr)
		{
			CString strName = disk_op::GetFileName(*_itr);
			ASSERT(!strName.IsEmpty());
			//������ ������ � ��������� ��� � �����


			std::auto_ptr<qdGameObjectStatic> pobj(CreateStatic());
			if (!pobj.get())
				break;
			if(!AddSceneObj(pobj.get(), hCatalog, strName, II_STATIC_OBJ_NODE))
				break;

			//��������� ��������
			qdSprite* psprite = pobj->get_sprite();
			ASSERT(psprite);
			psprite->set_file(*_itr);
			pobj->load_resources();
			
			Vect2s vSize = pobj->screen_size();
			objSort.insert(std::multimap<int, qdGameObject*>::value_type(
											int(vSize.x)*vSize.y, pobj.release()));
		}
		CString strMsg;
		strMsg.LoadString(IDS_STATICS_ADDED_SUCCESSFULL);
		if (MessageBox(strMsg, NULL, MB_ICONINFORMATION|MB_YESNO|MB_DEFBUTTON2) == IDNO) 
			break;
	}
	
	std::multimap<int, qdGameObject*>::iterator beg = objSort.begin(), 
		_end = objSort.end();

	qdCamera* pcam = ptrScene->get_camera();
	Vect2s scrCenter;
	float fDepth = START_DEPTH_FOR_STATIC_OBJ;
	for(;beg != _end; ++beg)
	{
		scrCenter.x = (short)pcam->get_scr_center_x();
		scrCenter.y = (short)pcam->get_scr_center_y();
		
		scrCenter = pcam->scr2rscr(scrCenter);
		beg->second->set_pos(pcam->rscr2global(scrCenter, fDepth));
		fDepth += 1.f;
	}
	return true;
}

/*!
	�������� ���������� SceneTreeView::AddScene(HTREEITEM hItemParent, CString& strName).
	���� \a bShowNew = true, �� �������� ���� ����� �����
	\param strName	- ��� ����� �����
	\param bShowNew	- ���������� ��� ��� ��������� �� ����� ����
	\return \a true - ���� ���������� ������
*/
bool SceneTreeView::AddScene(CString& strName, bool bShowNew)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hPar = tree.GetChildItem(TVI_ROOT);
	HTREEITEM hScene = AddScene(hPar, strName);
	if (!hScene)
		return false;

	if(bShowNew)
	{
		if(m_hCurrScene)
			tree.Expand(m_hCurrScene, TVE_COLLAPSE);
		ShowItem(hScene);
		tree.Expand(hScene, TVE_EXPAND);
	}
	
	Invalidate();
	return true;
}

/*!
 	������ ����� � ��������� � � ��������� ����
	(SceneTreeView::CreateScene(qdGameDispatcher*, CString&)).
	��������� ����� � ������(SceneTreeView::LoadScene(HTREEITEM hPar, qdGameScene* pscene)
	\param hItemParent	- ������������ ���� � ������
	\param strName		- ��� ����� �����
	\return ��������� ���� ����� � ������, NULL � ������ �������
*/
HTREEITEM SceneTreeView::AddScene(HTREEITEM hItemParent, CString& strName)
{
	CTreeCtrl& wndTree = GetTreeCtrl();
	
	qdGameScene* ptrs = CreateScene(m_ptrGameDisp, strName);
	
	HTREEITEM hItem = LoadScene(hItemParent, ptrs);
	if(!hItem)
		return NULL;
	SetProjChange();
	return hItem;
}

/*!
	��������� ������ � �����(SceneTreeView::AddToScene(qdGameScene* ptrScene, qdGameObject* ptrObj, const CString& strName)), 
	������� ���������� ���, � ������(SceneTreeView::AddGOToTree(HTREEITEM hItemParent, qdGameObject* ptr, UINT uiImage)).
	����� ������� ���������� ���� �������.
	������������ ��� ���������� ���������� ��������
	\param ptr			- �������� �� ������
	\param hItemParent	- ������������ ���� � ������
	\param strName		- ���. ���� �������� ������ ������, �� �� 
							�������� ����������� ��� �� ���������
	\param uiImage		- ����� ������� ��� ���� �������
	\return \a true - ���� ������� ��������
*/
bool SceneTreeView::AddSceneObj(qdGameObject* ptr, HTREEITEM hItemParent, 
								const CString &strName,UINT uiImage)
{
	CTreeCtrl& wndTree = GetTreeCtrl();
	HTREEITEM hSceneItem = wndTree.GetParentItem(hItemParent);
	ASSERT(hSceneItem);
	qdGameScene* ptrScene = (qdGameScene*)wndTree.GetItemData(hSceneItem);
	ASSERT(!IsBadReadPtr(ptrScene, sizeof(qdGameScene)));
	//���������, ������� ���������� ���
	AddToScene(ptrScene, ptr, strName);
	if (HTREEITEM hItem = AddGOToTree(hItemParent, ptr, uiImage))
		ShowItem(hItem);
	
	SetProjChange();
	return true;
}

/*!
 	��������� ��� �� � ����� ������� � ����� �� ������. ���� ���� �������� ��� ��
	���������� � ��������� ��� �������. ��������� ������ � �����.
	\param ptrScene - �����
	\param ptrObj	- ������
	\param strName  - ��� �������
	\retval true - ���� ��������� �������
*/
bool SceneTreeView::AddToScene(qdGameScene* ptrScene, qdGameObject* ptrObj, 
							   const CString& strName)
{
	CString str(strName);
	if(str.IsEmpty())
		str= GetDefaultObjName(ptrObj);
	
	str = get_unique_name(str, ptrScene->object_list());
	ptrObj->set_name(str);
	return static_cast<bool>(ptrScene->add_object(ptrObj));
}

/*!
 	������� ������� ��� ������� �������(ptree_helpers::FindTreeItemByText()) � 
	�������� ���������� SceneTreeView::AddGOToTree(HTREEITEM hItemParent, 
	qdGameObject* ptr, UINT uiImage)

	\param hSceneItem	- ���� �����, ������� ����������� ������
	\param uiCatNameID	- �� ��������� ������, ���������� ��� ��������, � ������� 
							����� �������� ������
	\param ptr			-  ������
	\param uiImage		- ����������� ��� ���� �������
	\return ��������� ���� �������
*/
HTREEITEM SceneTreeView::AddGOToTree2(HTREEITEM hSceneItem, UINT uiCatNameID, 
									  qdGameObject* ptr, UINT uiImage)
{
	HTREEITEM hCat = ptree_helpers::FindTreeItemByText(GetTreeCtrl(), hSceneItem, 
					CString((LPTSTR)uiCatNameID));
	if(!hCat)
		return NULL;
	
	return AddGOToTree(hCat, ptr, uiImage);
}

/*!
 	��������� ������ � ������(ptree_loading::InsertNamedObject()). ���� ��� ��
	����������� ������, �� ��������� ��� ��������� � ������(ptree_loading::LoadStates())
	\param hItemParent	- ������������ ���� � ������
	\param ptr			- ������ ��� �������
	\param uiImage		- ����������� ��� ���� �������
	\return ��������� ���� �������
*/
HTREEITEM SceneTreeView::AddGOToTree(HTREEITEM hItemParent, qdGameObject* ptr, 
											UINT uiImage)
{
	CTreeCtrl& wndTree = GetTreeCtrl();
	HTREEITEM hItem = InsertNamedObject(wndTree, hItemParent, ptr, uiImage);
	if(!hItem)
		return NULL;
	
	if(ptr->named_object_type() != QD_NAMED_OBJECT_STATIC_OBJ)
	{
		ASSERT(ptr->owner() != m_ptrGameDisp);
		LoadStates(wndTree, hItem, ptr);
		wndTree.Expand(hItem, TVE_EXPAND);
	}
	
	return hItem;
}

/*!
	�������� SceneTreeView::AddPersonage (qdGameScene* ptrScene, 
	const CString &strName, bool bShow, bool bAdd2Tree) ��� ���������� ���������.
	SceneTreeView::InsertFirstGaitToPersonage (qdGameObjectMoving* ptrPers) - ���
	������� �������.
	\param ptrScene - ����� ���� �������� ���������.
	\param strName  - ��� ���������
 	\return ��������� �� ��������� ������
*/
qdGameObjectMoving* SceneTreeView::AddPersWith1stGait(qdGameScene* ptrScene, 
													  const CString &strName)
{
	qdGameObjectMoving* res = AddPersonage(ptrScene, CString(strName), 
											false, false);
	if (!res) 
		return NULL;

	InsertFirstGaitToPersonage(res);
	res->set_pos(GetPersonagePos(ptrScene, res->bound().z/2.f));
	return res;
}

/*!
 	������� ��������(SceneTreeView::CreatePersonage()), ��������� ��� � ������ 
	���������� �������� � � ������, ������ ����� ��������� � ������(
	SceneTreeView::GlobalObjToScenes(qdGameObject const* pobj)), ��������� ����������
	��������� ���������� � ������
	\param ptrDisp	- ��������� �� ��������� ����
	\param strName	- ��� ���������
	\param bShow	- ���������� �� ��������� �� ���� ������������ ��������
	\return ��������� �� ��������� ������
*/
qdGameObjectMoving* SceneTreeView::AddGlobalPersonage(qdGameDispatcher* ptrDisp, 
														  CString &strName, 
														  bool bShow)
{
	std::auto_ptr<qdGameObjectMoving> ptrObj (CreatePersonage());
	if(ptrObj.get())
	{
		if(strName.IsEmpty())
			strName = GetDefaultObjName(ptrObj.get());
		
		strName = get_unique_name(strName, ptrDisp->global_object_list());
		ptrObj->set_name(strName);
		ptrObj->set_pos(Vect3f(0, 0, 0));

		if(ptrDisp->add_global_object(ptrObj.get()))
		{
			SetProjChange();
			qdGameObjectMoving* ptrMO = ptrObj.release();
			
			CTreeCtrl& tree = GetTreeCtrl();
			//����� ������� ���������� ����������
			HTREEITEM hGlobMObjs = 
				ptree_helpers::FindTreeItemByText(tree, tree.GetRootItem(),
										CString((LPTSTR)IDS_MOVING_OBJS_NODE));
			
			HTREEITEM hItem = InsertNamedObject(tree,hGlobMObjs, ptrMO, 
				II_MOVING_OBJ_NODE);
			if (hItem&&bShow) {
				ShowItem(hItem);
			}
			GlobalObjToScenes(ptrMO);
			UpdateCatalogsInScenes(QD_NAMED_OBJECT_MOVING_OBJ);
			return ptrMO;
		}
	}		
	return NULL;
}

/*!
 	� ����������� �� ���� ����������� ������� �������� 
	SceneTreeView::AddPersonage(qdGameScene* ptrScene, const CString &strName, 
	bool bShow, bool bAdd2Tree), SceneTreeView::AddAnimated(
	qdGameScene* ptrScene, const CString &strName, bool bShow) ��� 
	SceneTreeView::AddStatic(qdGameScene* ptrScene, const CString &strName, bool bShow)
	\param pscene	- �����, ���� ����� ��������� �����
	\param pobj		- ������, ����� �������� ����� �����������
	\return ��������� �� ��������� ����� �������. NULL � ������ �������
*/
qdGameObject* SceneTreeView::AddGlobalObj2Scene(qdGameScene* pscene,
												qdGameObject const* pobj)
{
	int type = pobj->named_object_type();
	if (type == QD_NAMED_OBJECT_MOVING_OBJ) {
		return AddPersonage(pscene, pobj->name(), false, true);
	}
	else if (type == QD_NAMED_OBJECT_ANIMATED_OBJ) {
		return AddAnimated(pscene, pobj->name(), false);
	}
	else if (type == QD_NAMED_OBJECT_STATIC_OBJ) {
		return AddStatic(pscene, pobj->name(), false);
	}
	ASSERT(0);
	return NULL;
}

/*!
 	�������� ��������, ��������� ��� � �����(SceneTreeView::AddToScene(
		qdGameScene* ptrScene, qdGameObject* ptrObj, const CString& strName)), ���� 
	���������, �� ��������� ������ � ������. ���� �������� ����� �����������, ��
	���������� ��� � ���������� ��������
	\param	ptrScene	- �����, ���� ��������� ������
	\param  strName		- ��� ������ ���������
	\param	bShow		- ���������� �� ��������� �� ���� ������������ �������
	\param	bAdd2Tree	- ��������� ������ � ������
	\return ��������� �� ����� ��������� �����
*/
qdGameObjectMoving* SceneTreeView::AddPersonage(qdGameScene* ptrScene, 
										const CString &strName, 
										bool bShow, 
										bool bAdd2Tree)
{
	std::auto_ptr<qdGameObjectMoving> ptrObj (CreatePersonage());
	if(ptrObj.get())
	{
		ptrObj->set_pos(
			get_obj_start_pos(ptrScene, QD_NAMED_OBJECT_MOVING_OBJ, ptrObj->R()));
		ptrObj->set_default_pos(ptrObj->R());

		if(AddToScene(ptrScene, ptrObj.get(), strName))
		{
			if (qdGameObject* p = 
					ptree_helpers::IsCopyOfGlobal(m_ptrGameDisp, ptrObj->name())) 
				ptrScene->merge_global_objects(p);

			if (bAdd2Tree) {
				HTREEITEM hSceneItem = FindObjectItem(ptrScene);
				ASSERT(hSceneItem);
				
				HTREEITEM hItem = AddGOToTree2(hSceneItem, IDS_MOVING_OBJS_NODE, 
												ptrObj.get(), II_MOVING_OBJ_NODE);
				ASSERT(hItem);
				
				if(hItem&&bShow)
					ShowItem(hItem);
			}
			SetProjChange();
		}
		else
			return NULL;
	}
	return ptrObj.release();
}

/*!
 	������� ������ (SceneTreeView::CreateAnimated()), ��������� � �����
	(SceneTreeView::AddToScene(qdGameScene* ptrScene, qdGameObject* ptrObj, 
	const CString& strName). ��������� ������ � ������. ���� ���� ��������� �� 
	���� ������� ���������.
	\param ptrScene - �����, � ������� ��������� ������
	\param strName	- ��� ������ �������
	\param	bShow	- ���������� �� ��������� �� ���� ������������ �������
	\return �������� �� ��������� ������
*/
qdGameObjectAnimated* SceneTreeView::AddAnimated(qdGameScene* ptrScene, 
												 const CString& strName, 
												 bool bShow)
{
	std::auto_ptr<qdGameObjectAnimated> ptrObj( CreateAnimated());
	if(ptrObj.get())
	{
		ptrObj->set_pos(
			get_obj_start_pos(ptrScene, QD_NAMED_OBJECT_ANIMATED_OBJ, ptrObj->R()));
		ptrObj->set_default_pos(ptrObj->R());
		if(AddToScene(ptrScene, ptrObj.get(), strName))
		{
			HTREEITEM hSceneItem = FindObjectItem(ptrScene);
			ASSERT(hSceneItem);
			
			HTREEITEM hItem = AddGOToTree2(hSceneItem, IDS_ANIMATED_OBJS_NODE, 
				ptrObj.get(), II_ANIMATED_OBJ_NODE);
			ASSERT(hItem);
			
			if(hItem&&bShow)
				ShowItem(hItem);
			SetProjChange();
		}
		else
			return NULL;
	}
	return ptrObj.release();
}

/*!
	������� ������(SceneTreeView::CreateStatic()), ��������� ��� � ����� � � ������.
	���� ���� �������� ����
	\param ptrScene - �����, � ������� ��������� ������
	\param strName	- ��� ������ �������
	\param	bShow	- ���������� �� ��������� �� ���� ������������ �������
	\return �������� �� ��������� ������
*/
qdGameObjectStatic* SceneTreeView::AddStatic(qdGameScene* ptrScene, 
											 const CString& strName, 
											 bool bShow)
{
	std::auto_ptr<qdGameObjectStatic> ptrObj( CreateStatic());
	if(ptrObj.get())
	{
		ptrObj->set_pos(
			get_obj_start_pos(ptrScene, QD_NAMED_OBJECT_STATIC_OBJ, ptrObj->R()));
		if(AddToScene(ptrScene, ptrObj.get(), strName))
		{
			HTREEITEM hSceneItem = FindObjectItem(ptrScene);
			ASSERT(hSceneItem);
			
			HTREEITEM hItem = AddGOToTree2(hSceneItem, IDS_STATIC_OBJS_NODE, 
				ptrObj.get(), II_STATIC_OBJ_NODE);
			ASSERT(hItem);
			
			if(hItem&&bShow)
				ShowItem(hItem);
			SetProjChange();
		}
		else
			return NULL;
	}
	return ptrObj.release();
}

CurDirSafer& GetVideoCurDirSafer();
/*!
 	��������� ������� ����������(��� ������ �� ������� ��� ����� �������������).
	��������� ������ ������ �����. ���� ���� ������, �� �������� ������������� ���� ��
	�������� �������(r_p::RelativePath()). ��������� ������� ����������.
	�������� SceneTreeView::AddVideoFile() ��� ���������� ����� � ������
	\retval true - ���� ����� ���� ���������
*/
bool SceneTreeView::AddVideo()
{
	CurDirGuard __cdg(&GetVideoCurDirSafer());
	
	CString str2;
	str2.LoadString(IDS_OPEN_VIDEO_FILTER);
	CString strTitle((LPTSTR)IDS_OSD_CAP_SEL_VIDEO);
	
	CFileDialog dlg(TRUE, NULL, NULL, 
		OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, str2);
	CString curDir = CurDirSafer::GetCurrentDirectory();
	dlg.m_ofn.lpstrInitialDir = curDir;
	dlg.m_ofn.lpstrTitle = strTitle;
	if(dlg.DoModal() == IDOK)
	{
		CString strCurrDir = __cdg.get_saved();
		strCurrDir += _T("\\");
		
		CString rel_path;
		POSITION pos = dlg.GetStartPosition();
		while(pos)
		{
			if((!r_p::RelativePath(dlg.GetNextPathName(pos), strCurrDir, rel_path)))
			{
				ShowError(m_hWnd, IDS_EM_CNNT_GET_REL_PATH);
				return false;
			}
			GetVideoCurDirSafer().save_current();

			if(!AddVideoFile(rel_path))
				return false;
		}
		return true;
	}
	
	return false;
}

/*!
 	������� ����� ������ (qdVideo). ��������� ��� ��� �� ����� �����, � ���������
	����� ����� � ������� ���������. � ������ ���������� �� ���������
	\param strFileName - ��� ����� �����-������.
	\retval true - ���� ������� �������� �����-�����
*/
bool SceneTreeView::AddVideoFile(const CString &strFileName)
{
	std::auto_ptr<qdVideo> ptrVideo (new qdVideo);
	if(!ptrVideo.get())
		return false;
	
	ptrVideo->set_name(get_unique_name(disk_op::GetFileName(strFileName), 
		m_ptrGameDisp->video_list()));
	ptrVideo->set_file_name(strFileName);
	if(!m_ptrGameDisp->add_video(ptrVideo.get()))
		return false;
	ptrVideo.release();
	return true;
}


/************************************************************************/
/* ���������                                                            */
/************************************************************************/

/*!
 	����������� � ������������ ����� ������(StaticObjPage::SelectPicture()), 
	�������� ����������� �����. ��������� ��������� ���� ���������. ����
	���������� ������ ������, ���������� "��������� �� ���"
 */
void SceneTreeView::AddInventoryCellTypes()
{
	CStringVect_t vecFiles;
	CString strOpenDlgTitle((LPTSTR)IDS_OSD_CAP_SEL_CELL_TYPE);
	int nCountAdded = 0;

	while (true) {
		vecFiles.clear();
		//�������� ��������
		CString picture = StaticObjPage::SelectPicture(this->m_hWnd, 
			strOpenDlgTitle, &vecFiles);
		if(picture.IsEmpty())
			break;
		
		CStringVect_t::iterator _itr = vecFiles.begin(), _end = vecFiles.end();
		for(; _itr != _end; ++_itr)
		{
			qdInventoryCellType ct(m_ptrGameDisp->
								get_unique_inventory_cell_type());

			ct.set_sprite_file(*_itr);
			if(const qdInventoryCellType* pct = 
				m_ptrGameDisp->add_inventory_cell_type(ct))
			{
				pct->load_resources();
				++nCountAdded;
			}
			SetProjChange();
		}
		CString strMsg;
		strMsg.LoadString(IDS_CELL_TYPES_ADDED_SUCCESSFULL);
		if (MessageBox(strMsg, NULL, MB_ICONINFORMATION|MB_YESNO|MB_DEFBUTTON2) == IDNO) 
			break;
	}
}

/*!
 	������� ������ ���������. ���� ����� ��������(\a mode) ICM_USE_EXISTING_CELL_TYPES,
	�� � ��������� ��� ������� ���� ����� ����������� ����� �����, ��������� �� �����
	������.	������ �������������, ��� ����� �� �����������
	\param mode - ����� ��������.
	\return	��������� �� ����� �������� ������, ���� NULL � ������ �������
*/
qdInventory* SceneTreeView::CreateInventory(eInventoryCrtMode mode){
	std::auto_ptr<qdInventory> p(new qdInventory);
	if (mode == ICM_USE_EXISTING_CELL_TYPES){
		typedef qdInventoryCellTypeVector::const_iterator ci_t;
		const Vect2s init_pos(0, 0);
		const Vect2s init_size(1,1);

		Vect2s scr_pos(init_pos);
		int bottom = 0;
		const qdInventoryCellTypeVector& v = 
						m_ptrGameDisp->inventory_cell_types();
		ci_t beg = v.begin(), end = v.end();
		for(; beg != end; ++beg){
			const qdInventoryCellType& ct = *beg;
			qdInventoryCellSet* ps = 
				p->add_cell_set(init_pos, init_size, ct);
			Vect2i size = ps->cell_size();
			if (scr_pos.x + size.x > qdGameConfig::get_config().screen_sx())
			{
				//���� ��������� �� ����� �������
				scr_pos.x = 0;
				scr_pos.y = bottom;
			}

			ps->set_screen_pos(Vect2s(scr_pos.x + (size.x>>1),
											scr_pos.y + (size.y>>1)));
			size = ps->screen_size();
			scr_pos.x += size.x;
			bottom = (std::max)(bottom, scr_pos.y + size.y);
		}
	}
	return p.release();
}

/*!
 	������� ����-������� ��� ���������.��������� ���� \a pinv
	(ptree_helperes::InsertNamedObject()). ������� �������(SceneTreeView::SetNoCheck())
	� ������ ���� �������(SceneTreeView::ShowItem())
	\param pinv - ������ ���������
	\return ��������� ���� � ������, ���� NULL � ������ �������
*/
HTREEITEM SceneTreeView::InsertInventoryToTree(qdInventory* pinv)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hRoot = tree.GetRootItem();
	HTREEITEM hInventories = ptree_helpers::FindTreeItemByText(tree, hRoot, 
								CString((LPTSTR)IDS_INVENTORY_OBJS_NODE));

	ASSERT(hInventories);
	HTREEITEM hres = 
		InsertNamedObject(tree, hInventories, pinv, II_INVENTORY_OBJ_NODE);
	if (hres)
	{
		SetNoCheck(hres);
		ShowItem(hres);
	}
	return hres;
}

/*!
 	�������� ���������� ��� �� ������ strName, ��������� ��� �������
	(qdNamedObject::set_name()). ��������� ��������� � ��������� ����
	(qdGameDispatcher::add_inventory());
	\param pinv		- ������ ���������
	\param strName	- ��� ��� ������ ���������
	\return \a true - ���� pinv �� NULL � ������ �������� � ��������� ����
*/
bool SceneTreeView::InsertInvToGD(qdInventory* pinv, CString strName)
{
	if (pinv)
	{
		strName = get_unique_name(strName, m_ptrGameDisp->inventory_list());
		pinv->set_name(strName);
		
		return m_ptrGameDisp->add_inventory(pinv);
	}
	return false;
}
/************************************************************************/
/* ��������                                                             */
/************************************************************************/
/*!
	\param strName			- ��� ������ �������
	\param strDllName		- ��� ����� ���
	\param strConfigName	- ��� ����� ������-�����
	\return �������� �� ��������� ������. 	
*/
qdMiniGame* SceneTreeView::CreateMiniGame(const CString& strName,
										  const CString& strDllName, 
										  const CString& strConfigName)
{
	std::auto_ptr<qdMiniGame> res(new qdMiniGame);
	if (!res.get())
		return NULL;

	CString strRealName = get_unique_name(strName, m_ptrGameDisp->minigame_list());
	res->set_name(strRealName);
	res->set_dll_name(strDllName);
//	res->set_config_file(strConfigName);

	return res.release();
}

/*!
 	����������� � ������������ ��� ����� ���(ptree_add::MGGetDLLName()). �����������
	� ������������ ��� ������-�����(ptree_add::MGGetCNFGName()). ����������� ���(
	bool AskUserForName (CString& strName)). ������� ��������. ��������� � � ������(
	SceneTreeView::InsertMGToTree()). ��������� � ������ �������. ��������� ��������� 
	�������� � ��������� � ��������(SceneTreeView::AddSimpleMGState())
	\return \a true - ���� ��� ������� 
*/
bool SceneTreeView::AddMiniGame(){
	HTREEITEM hSelected = GetTreeCtrl().GetSelectedItem();
	if (!hSelected) 
		return false;

	const CString strDllName = MGGetDLLName();
	if (strDllName.IsEmpty())
		return false;

	const CString strConfigName = MGGetCNFGName();

	CString strName((LPTSTR)IDS_NEW_MINIGAME_NAME);
	if(!AskUserForName(strName))
		return false;

	std::auto_ptr<qdMiniGame> pmg(CreateMiniGame(strName, strDllName, strConfigName));

	if (HTREEITEM hItem = InsertMGToTree(hSelected, pmg.get())){
		if (!m_ptrGameDisp->add_minigame(pmg.get()))
		{
			RemoveItem(hItem, false);
			return false;
		}
		ShowItem(hItem);
	}
	AddSimpleMGState(pmg.get());

	SetProjChange();
	pmg.release();
	return true;
}

/*!
	\param pgame - ��������� �� ��������
 	\return \a true - ���� ��������� ��������� �������
*/
bool SceneTreeView::AddSimpleMGState(qdMiniGame* pgame){

/*
	//������ ������� ����� ����������. ����� 0 ��� "��������"
	CString strStateName;
	if(!strStateName.LoadString(IDS_MG_STATE_NAME_LOSS))
		return false;
	qdMiniGameState state;
	state.set_name(strStateName);
	state.set_score_level(0);
	pgame->add_state(state);

	if(!strStateName.LoadString(IDS_MG_STATE_NAME_WIN))
		return false;
	state.set_name(strStateName);
	state.set_score_level(1);
	pgame->add_state(state);
	return true;
*/
	return false;
}
/************************************************************************/
/* ������� ���������                                                    */
/************************************************************************/
/*!
 	������� ������� (SceneTreeView::CreateTriggerChain()). ������ �� ���������� ���.
	��������� � ������ ������� ���������. ��������� ������� � ������.
	\param hPar		- ������������ ���� � ������
	\param strName	- ��� ������� ���������
	\param bShow	- ���������� �� ���������, �� ����������� ����
	\return ��������� �� ����������� ������
*/
qdTriggerChain* SceneTreeView::AddTriggerChain(HTREEITEM hPar, 
											   const CString& strName,
											   bool bShow)
{
	std::auto_ptr<qdTriggerChain> res (CreateTriggerChain());
	if (!res.get()) {
		return NULL;
	}

	//�������� ��� ��� ����� �������
	CString strUniqName(strName);
	if (strUniqName.IsEmpty()) {
		strUniqName.LoadString(IDS_NEW_TRIGGER_CHAIN_NAME);
	}
	strUniqName = get_unique_name(strUniqName, 
		m_ptrGameDisp->trigger_chain_list());
	
	res->set_name(strUniqName);
	if(!m_ptrGameDisp->add_trigger_chain(res.get()))
		return NULL;
	
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.InsertItem(res->name(), hPar);
	if(hItem)
	{
		tree.SetItemData(hItem, (DWORD)res.get());
		tree.SetItemImage(hItem, II_TRIGGER_NODE, II_TRIGGER_NODE);
		SetNoCheck(hItem);
		
		if (bShow) {
			ShowItem(hItem);
		}
	}
	SetProjChange();
	return res.release();
}

/*!
 	������� ������ qdTriggerChain � ������ �������� ���� ��������
	\return ��������� �� ��������� ������.
*/
qdTriggerChain* SceneTreeView::CreateTriggerChain()
{
	qdTriggerChain *pobj = new qdTriggerChain;
	if(!pobj)
	{
		ShowError(m_hWnd, IDS_CNNT_CRT_TRIGGER_CHAIN);
		return NULL;
	}
	pobj->root_element()->make_active(true);
	return pobj;
}

/************************************************************************/
/* ���� �� �����                                                        */
/************************************************************************/
/*!
 	������� ���� ����� � ������� �����. ����������� � ������������ ��� �������
	(bool AskUserForName (CString& strName)), ��������� ���� � ������ ��� �����. 
	��������� ������ � ������.
	\param ptrZone - ����
	\return \a true - ���� ���������� �������
*/
bool SceneTreeView::AddGridZone2Tree(qdGridZone* ptrZone)
{
	if(ptrZone)
	{
		CTreeCtrl& tree = GetTreeCtrl();
		HTREEITEM hItem = 
			ptree_helpers::FindTreeItemByText(tree, m_hCurrScene, 
											CString((LPTSTR)IDS_NET_NODE_NAME));
		ASSERT(hItem);
		
		qdGameScene* ptrScene = 
			reinterpret_cast<qdGameScene*>(tree.GetItemData(m_hCurrScene));
		CString str;
		str.LoadString(IDS_NEW_GRID_ZONE_NAME);
		str = get_unique_name(str, ptrScene->grid_zone_list());

		if(!AskUserForName(str))
			return false;

		ptrZone->set_name(str);
		
		if (ptrScene->add_grid_zone(ptrZone)) {
			if (hItem = InsertGZ2Tree(tree,hItem, ptrZone, II_ZONE_NODE, TVI_LAST))
			{
				ShowItem(hItem);
				Invalidate(FALSE);
				SetProjChange();
				return true;
			}
		}
	}
	return false;
}

/************************************************************************/
/* ����������� �����                                                    */
/************************************************************************/
/*!
	\param strName - ��� ������������ �����
	\return ��������� �� ��������� ������
*/
std::auto_ptr<qdMusicTrack> SceneTreeView::CreateMusicTrack(const CString& strName){
	std::auto_ptr<qdMusicTrack> ptr(new qdMusicTrack);
	if (ptr.get() == NULL)
		return ptr;
	ptr->set_name(strName);
	return ptr;
}

/*!
	����������� � ������������ ���� ���. �����(ptree_add::GetMusicTrackFileName())
	����������� � ������������ ��� ���. �����(bool AskUserForName (CString& strName)).
	������� ���. ���� � ��������� ��� � ������
	\param pscene		- �����, ���� ����� �������� ���. ����
	\param hMTCatalog	- ���� �������� ����������� ������
	\return \a true	- ���� ���������� ������ �������
*/
bool SceneTreeView::AddMusicTrack(qdGameScene* pscene, HTREEITEM hMTCatalog)
{
	CString strFileName = GetMusicTrackFileName();
	if (strFileName.IsEmpty())
		return false;

	CString strMTrackName = disk_op::GetFileName(strFileName);
	if (!AskUserForName(strMTrackName)) 
		return false;

	//�������� ���������� ���
	strMTrackName = get_unique_name(strMTrackName, pscene->music_track_list());
	std::auto_ptr<qdMusicTrack> pmt(CreateMusicTrack(strMTrackName));
	if (!pmt.get())
		return false;
	pmt->toggle_cycle(true);
	pmt->set_file_name(strFileName);
	if (HTREEITEM hItem = 
		InsertNamedObject(GetTreeCtrl(), hMTCatalog, pmt.get(), II_MUSIC_TRACK_NODE))
	{
		SetNoCheck(hItem);
		if (!pscene->add_music_track(pmt.get()))
		{
			RemoveItem(hItem, false);
			return false;
		}
		pmt.release();
	}
	SetProjChange();
	Invalidate();
	return true;
}

/*!
	������� ������ qdFontInfo � ��������� ������
	\return NULL - ���� �� ������� �������
*/
std::auto_ptr<qdFontInfo> SceneTreeView::CreateFont(CString const& strName)
{
	std::auto_ptr<qdFontInfo> res(new qdFontInfo);
	if (!res.get())
		return res;

	res->set_name(strName);
	return res;
}

bool SceneTreeView::AddFont(HTREEITEM hCatalog)
{
	CString strFileName = GetFontFileName();
	if (strFileName.IsEmpty())
		return false;

	CString strName = disk_op::GetFileName(strFileName);
	if (!AskUserForName(strName)) 
		return false;

	//�������� ���������� ���
	strName = get_unique_name(strName, m_ptrGameDisp->fonts_list());
	std::auto_ptr<qdFontInfo> p(CreateFont(strName));
	if (!p.get())
		return false;
	strFileName.Replace(_T(".idx"), _T(".tga"));
	p->set_font_file_name(static_cast<LPCTSTR>(strFileName));

	p->set_type(m_ptrGameDisp->get_unique_font_info_type());

	if (HTREEITEM hItem = 
		InsertNamedObject(GetTreeCtrl(), hCatalog, p.get(), II_FONT_NODE))
	{
		SetNoCheck(hItem);
		if (!m_ptrGameDisp->add_font_info(p.get()))
		{
			RemoveItem(hItem, false);
			return false;
		}
		p.release();
		GetTreeCtrl().Expand(hCatalog, TVE_EXPAND);
	}
	SetProjChange();
	Invalidate();
	return true;
}

bool SceneTreeView::CreateFont(HTREEITEM hCatalog)
{
	CString strName = LoadString(IDS_NEW_FONT_NAME);
	if (!AskUserForName(strName)) 
		return false;

	//�������� ���������� ���
	strName = get_unique_name(strName, m_ptrGameDisp->fonts_list());
	std::auto_ptr<qdFontInfo> p(CreateFont(strName));
	if (!p.get())
		return false;

	CString strFileName = "Resource\\Fonts\\";
	strFileName += strName;

	FontGenerator font_gen;
	if(!font_gen.generate(strFileName))
		return false;

	strFileName += ".idx";

	strFileName.Replace(_T(".idx"), _T(".tga"));
	p->set_font_file_name(static_cast<LPCTSTR>(strFileName));
	p->set_type(m_ptrGameDisp->get_unique_font_info_type());

	if (HTREEITEM hItem = 
		InsertNamedObject(GetTreeCtrl(), hCatalog, p.get(), II_FONT_NODE))
	{
		SetNoCheck(hItem);
		if (!m_ptrGameDisp->add_font_info(p.get()))
		{
			RemoveItem(hItem, false);
			return false;
		}
		p.release();
		GetTreeCtrl().Expand(hCatalog, TVE_EXPAND);
	}
	SetProjChange();
	Invalidate();
	return true;
}

bool SceneTreeView::Font(HTREEITEM hCatalog)
{
	CString strFileName = GetFontFileName();
	if (strFileName.IsEmpty())
		return false;

	CString strName = disk_op::GetFileName(strFileName);
	if (!AskUserForName(strName)) 
		return false;

	//�������� ���������� ���
	strName = get_unique_name(strName, m_ptrGameDisp->fonts_list());
	std::auto_ptr<qdFontInfo> p(CreateFont(strName));
	if (!p.get())
		return false;
	strFileName.Replace(_T(".idx"), _T(".tga"));
	p->set_font_file_name(static_cast<LPCTSTR>(strFileName));

	p->set_type(m_ptrGameDisp->get_unique_font_info_type());

	if (HTREEITEM hItem = 
		InsertNamedObject(GetTreeCtrl(), hCatalog, p.get(), II_FONT_NODE))
	{
		SetNoCheck(hItem);
		if (!m_ptrGameDisp->add_font_info(p.get()))
		{
			RemoveItem(hItem, false);
			return false;
		}
		p.release();
		GetTreeCtrl().Expand(hCatalog, TVE_EXPAND);
	}
	SetProjChange();
	Invalidate();
	return true;
}

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* �������� ����������                                                  */
/************************************************************************/

const DWORD INTERF_BACK_DEPTH = 30;
const DWORD INTERF_BTN_DEPTH = 15;
const DWORD INTERF_SAVE_DEPTH = 15;
const DWORD INTERF_SLIDER_DEPTH = 15;
const DWORD INTERF_TEXT_WINDOW_DEPTH = 15;
const DWORD INTERF_COUNTER_DEPTH = 15;

/*!
 	������� ������ "������������ �����" � ��������� ��� � ������ �������(
	��������� ����� add_scr::operator()()). ��������� � ������(
	ptree_loading::InsertObject()).
	\param hItemIntef	- ������������ ����.(���� � ������� "���������")
	\param name			- ��� ������ �������
	\return ��������� �� ��������� ������
*/
qdInterfaceScreen* SceneTreeView::AddInterfScr(HTREEITEM hItemIntef, const CString& name)
{
	class add_scr{
	public:
		qdInterfaceScreen* operator()(const CString& name)
		{
			std::auto_ptr<qdInterfaceScreen> scr(new qdInterfaceScreen);
			qdInterfaceDispatcher *pd = qdInterfaceDispatcher::get_dispatcher();
			CString uname = get_unique_name(name, pd->screen_list());
			scr->set_name(uname);
			if (pd->add_screen(scr.get()))
			{
				scr->set_owner(pd);
				return scr.release();
			}
			return NULL;
		}
	};

	std::auto_ptr<qdInterfaceScreen> ps(add_scr()(name));
	if (!ps.get())
		return false;

	qdInterfaceDispatcher *pd = qdInterfaceDispatcher::get_dispatcher();
	if (!hItemIntef)
		hItemIntef = FindObjectItem(pd);

	ASSERT(hItemIntef);

	const HTREEITEM hRes = ptree_loading::InsertObject(GetTreeCtrl(), hItemIntef, ps.get(), 
		II_INTERFACE_SCEREEN_NODE, TVI_SORT);
	if (!hRes)
	{
		pd->remove_screen(ps.get());
		return NULL;
	}

	return ps.release();
}

/*!
 	������� � ��������� ������ � ������ �������� ������(ptree_add::add_ele::operator()())
	��������� �������(ptree_add::GetInterfEleDepthOverBack()) � ������������� �. 
	��������� � ������(ptree_loading::InsertObject())
	\param pscr		- �����, � ������� ����� ��������� ������
	\param hItemScr - ���� ������ � ������
	\param name		- ��� ����� ������
	\return ��������� �� ��������� ������
*/
qdInterfaceButton* SceneTreeView::AddInterfBtn(qdInterfaceScreen* pscr, 
											   HTREEITEM hItemScr, const CString& name)
{
	typedef ptree_add::add_ele<qdInterfaceButton> add_btn;

	std::auto_ptr<qdInterfaceButton> pb(add_btn()(pscr, name));
	if (!pb.get())
		return NULL;

	pb->set_screen_depth(GetInterfEleDepthOverBack(pscr, INTERF_BTN_DEPTH));

	if (!hItemScr)
		hItemScr = FindObjectItem(pscr);
	ASSERT(hItemScr);

	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItemBtn = ptree_loading::InsertObject(tree,	hItemScr,
		pb.get(), II_INTERFACE_ELEMENT_NODE);
	if (!hItemBtn)
	{
		pscr->remove_element(pb.get());
		return NULL;
	}
	tree.SetItemState(hItemBtn, INDEXTOSTATEIMAGEMASK(pb->is_visible()?2:1), 
							TVIS_STATEIMAGEMASK);
	tree.Expand(hItemScr, TVE_EXPAND);
	return pb.release();
}

/*!
 	������� � ��������� ������ � ������ �������� ������(ptree_add::add_ele::operator()())
	��������� �������(ptree_add::GetInterfEleDepthOverBack()) � ������������� �. 
	��������� � ������(ptree_loading::InsertObject())
	\param pscr		- �����, � ������� ����� �������� ���
	\param hItemScr - ���� ������ � ������
	\param name		- ��� ����
	\return ��������� �� ��������� ���
*/
qdInterfaceBackground* SceneTreeView::AddInterfBack(qdInterfaceScreen* pscr, 
										HTREEITEM hItemScr, const CString& name)
{
	typedef ptree_add::add_ele<qdInterfaceBackground> add_back;
	std::auto_ptr<qdInterfaceBackground> pb(add_back()(pscr, name));
	if (!pb.get())
		return NULL;

	pb->set_screen_depth(GetInterfEleDepthOverBack(pscr, INTERF_BACK_DEPTH));

	if (!hItemScr)
		hItemScr = FindObjectItem(pscr);
	ASSERT(hItemScr);

	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hitem = ptree_loading::InsertObject(tree,
				hItemScr, pb.get(), II_INTERFACE_ELEMENT_NODE);
	if (!hitem)
	{
		pscr->remove_element(pb.get());
		return NULL;
	}
	tree.SetItemState(hitem, INDEXTOSTATEIMAGEMASK(pb->is_visible()?2:1), 
		TVIS_STATEIMAGEMASK);
	tree.Expand(hItemScr, TVE_EXPAND);
	return pb.release();
}

/*!
 	������� � ��������� ������ � ������ �������� ������(ptree_add::add_ele::operator()())
	��������� �������(ptree_add::GetInterfEleDepthOverBack()) � ������������� �. 
	��������� � ������(ptree_loading::InsertObject())
	\param pscr		- �����, � ������� ����� �������� ����
	\param hItemScr - ���� ������ � ������
	\param name		- ��� �����
	\return ��������� �� ��������� ����
*/
qdInterfaceSave* SceneTreeView::AddInterfSave(qdInterfaceScreen* pscr, 
							   HTREEITEM hItemScr, const CString& name)
{
	typedef ptree_add::add_ele<qdInterfaceSave> add_save;
	std::auto_ptr<qdInterfaceSave> ps(add_save()(pscr, name));
	if (!ps.get())
		return NULL;

	ps->set_screen_depth(GetInterfEleDepthOverBack(pscr, INTERF_SAVE_DEPTH));

	if (!hItemScr)
		hItemScr = FindObjectItem(pscr);
	ASSERT(hItemScr);

	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hitem = ptree_loading::InsertObject(tree,
		hItemScr, ps.get(), II_INTERFACE_ELEMENT_NODE);
	if (!hitem)
	{
		pscr->remove_element(ps.get());
		return NULL;
	}
	tree.SetItemState(hitem, INDEXTOSTATEIMAGEMASK(ps->is_visible()?2:1), 
		TVIS_STATEIMAGEMASK);
	tree.Expand(hItemScr, TVE_EXPAND);
	return ps.release();
}

/*!
 	������� � ��������� ������ � ������ �������� ������(ptree_add::add_ele::operator()())
	��������� �������(ptree_add::GetInterfEleDepthOverBack()) � ������������� �. 
	��������� � ������(ptree_loading::InsertObject())
	\param pscr		- �����, � ������� ����� �������� ���������
	\param hItemScr - ���� ������ � ������
	\param name		- ��� ������ ����������
	\return ��������� �� ��������� ���������
*/
qdInterfaceSlider* SceneTreeView::AddInterfSldr(qdInterfaceScreen* pscr, 
								HTREEITEM hItemScr, const CString& name)
{
	typedef ptree_add::add_ele<qdInterfaceSlider> add_sldr;
	std::auto_ptr<qdInterfaceSlider> pb(add_sldr()(pscr, name));
	if (!pb.get())
		return NULL;

	pb->set_screen_depth(GetInterfEleDepthOverBack(pscr, INTERF_SLIDER_DEPTH));
	if (!hItemScr)
		hItemScr = FindObjectItem(pscr);
	ASSERT(hItemScr);

	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hitem = ptree_loading::InsertObject(tree, hItemScr, 
							pb.get(), II_INTERFACE_ELEMENT_NODE);
	if (!hitem)
	{
		pscr->remove_element(pb.get());
		return NULL;
	}
	tree.SetItemState(hitem, INDEXTOSTATEIMAGEMASK(pb->is_visible()?2:1), 
							TVIS_STATEIMAGEMASK);
	tree.Expand(hItemScr, TVE_EXPAND);
	return pb.release();
}

/*!
 	������� � ��������� ������ � ������ �������� ������(ptree_add::add_ele::operator()())
	��������� �������(ptree_add::GetInterfEleDepthOverBack()) � ������������� �. 
	��������� � ������(ptree_loading::InsertObject())
	\param pscr		- �����, � ������� ����� �������� ���������
	\param hItemScr - ���� ������ � ������
	\param name		- ��� ������ ����������
	\return ��������� �� ��������� ���������
*/
qdInterfaceTextWindow* SceneTreeView::AddInterfTextWindow(qdInterfaceScreen* pscr, 
								HTREEITEM hItemScr, const CString& name)
{
	typedef ptree_add::add_ele<qdInterfaceTextWindow> add_element;
	std::auto_ptr<qdInterfaceTextWindow> pb(add_element()(pscr, name));
	if (!pb.get())
		return NULL;

	pb->set_screen_depth(GetInterfEleDepthOverBack(pscr, INTERF_TEXT_WINDOW_DEPTH));
	if (!hItemScr)
		hItemScr = FindObjectItem(pscr);
	ASSERT(hItemScr);

	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hitem = ptree_loading::InsertObject(tree, hItemScr, 
							pb.get(), II_INTERFACE_ELEMENT_NODE);
	if (!hitem)
	{
		pscr->remove_element(pb.get());
		return NULL;
	}
	tree.SetItemState(hitem, INDEXTOSTATEIMAGEMASK(pb->is_visible()?2:1), 
							TVIS_STATEIMAGEMASK);
	tree.Expand(hItemScr, TVE_EXPAND);
	return pb.release();
}


/*!
 	������� � ��������� ���������(��������� ����� add_state). ��������� ���������
	� ������(ptree_loading::InsertObject())
	\return ���������� ����� ���������.
	\retval -1 � ������ �������
*/
int SceneTreeView::AddInterfBtnState(qdInterfaceButton* pb, 
		HTREEITEM hItemBtn, const CString& name)
{
	class add_state
	{
		qdInterfaceButton* m_pButton;
	public:
		add_state():m_pButton(NULL){
		}
		int operator()(const CString& name, qdInterfaceButton* pb){
			qdInterfaceElementState st;
			st.set_name(name);
			if (pb->add_state(st))
			{
				return pb->num_states()-1;
			}
			return -1;
		}
	};
	int iState = add_state()(name, pb);
	if (iState == -1)
		return -1;
	if (!hItemBtn)
		hItemBtn = FindObjectItem(pb);
	ASSERT(hItemBtn);

	const qdInterfaceElementState* pst = 
		static_cast<qdInterfaceButton const*>(pb)->get_state(iState);

	if (!ptree_loading::InsertObject(GetTreeCtrl(), 
				hItemBtn, pst, II_INTERFACE_ELEMENT_STATE))
	{
		pb->erase_state(iState);
		return -1;
	}
	GetTreeCtrl().Expand(hItemBtn, TVE_EXPAND);
	return iState;
}

/*!
 	�������� ���������� ��� �� ������ \a name. ������� ������ � ��������� ���
	������������� ������(\a scrName).
	\param name		- ��� ��������� ������
	\param scrName	- ������ ��� ������������� ������
	\return ��������� �� ����� ��������� ������. NULL � ������ �������
*/
qdGameEnd* SceneTreeView::CreateGameEnd(CString const& name, CString const& scrName)
{
	std::auto_ptr<qdGameEnd> ptr(new qdGameEnd);
	if (!ptr.get())
		return NULL;
	CString const uniqName = get_unique_name(name, m_ptrGameDisp->game_end_list());
	ptr->set_name(uniqName);
	ptr->set_interface_screen(scrName);
	return ptr.release();
}

/*!
 	������� ������ "�������� �����"(SceneTreeView::CreateGameEnd(CString const& name, 
	CString const& scrName)). ��������� ��� � ������(ptree_loading::InsertObject()).
	��������� � ������ �������� �������
	\param hParent	- ������������ ���� � ������
	\param name		- ��� ������ �������
	\param scrName	- ��� ������������ ������
	\return ��������� �� �������� ������
*/
qdGameEnd* SceneTreeView::AddGameEnd(HTREEITEM hParent, 
					CString const& name, CString const& scrName)
{
	std::auto_ptr<qdGameEnd> pge ( CreateGameEnd(name, scrName));
	if (!pge.get())
		return NULL;
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = 
		ptree_loading::InsertObject(tree, hParent, pge.get(), II_GAME_END_NODE);
	if (!hItem)
		return NULL;

	if (!m_ptrGameDisp->add_game_end(pge.get())){
		tree.DeleteItem(hItem);
		return NULL;
	}
//	tree.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(pge.get()));
	return pge.release();
}
//////////////////////////////////////////////////////////////////////////
/*!
 	������� ����� ������ � ��������� ��� ���.
	\param name - ��� ��������
	\return ��������� �� ����� ������. NULL � ������ �������
*/
qdCounter* SceneTreeView::CreateCounter(CString const& name)
{
	std::auto_ptr<qdCounter> ptr(new qdCounter);
	if (ptr.get())
	{
		ptr->set_name(name);
	}
	return ptr.release();
}

/*!
 	���� \a name - ������ ������, �� ����������� ��� � �����������(
	bool AskUserForName (CString& strName)). ������� ������ "�������"(
	SceneTreeView::CreateCounter(CString const& name)). ��������� ��� � ������(
	ptree_loading::InsertObject()).��������� � ������ ���������. ��������� 
	������ �������(SceneTreeView::ShowCorrespondingPropDlg(qdInterfaceObjectBase* ptrObj,
	int iActive)). 
	\param hParent	- ������������ ���� � ������
	\param name		- ��� ������ �������
	\return	��������� �� ����� ������
*/
qdCounter* SceneTreeView::AddCounter(HTREEITEM hParent, CString name)
{
	if (name.IsEmpty()&&!AskUserForName(
			(name = CString(reinterpret_cast<LPTSTR>(IDS_NEW_COUNTER_NAME)))))
		return NULL;
	name = get_unique_name(name, m_ptrGameDisp->counter_list());
	std::auto_ptr<qdCounter> ptr(CreateCounter(name));
	CTreeCtrl& tree = GetTreeCtrl();

	HTREEITEM hItem = 
		ptree_loading::InsertObject(tree, hParent, ptr.get(), II_COUNTER_NODE);
	if (!hItem)
		return NULL;
	if (!m_ptrGameDisp->add_counter(ptr.get()))
	{
		tree.DeleteItem(hItem);
		return NULL;
	}
	ShowCorrespondingPropDlg(ptr.get(), 0, true);
	if (ptr->elements().empty())
	{
		DeleteObject(ptr.release(), false);
		return NULL;
	}
	return ptr.release();
}