/********************************************************************
created:	2002/09/11
created:	11:9:2002   11:00
filename: 	D:\Projects\Quest\QuestEditor\ptree_add.cpp
file path:	D:\Projects\Quest\QuestEditor
file base:	ptree_add
file ext:	cpp
Powerd by:	Илюха

  purpose:	Функции обеспечивающие добавление в проект
  и в дерево
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
/* Внешние определения                                                  */
/************************************************************************/
void ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID = -1);
void ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID = -1);

qdAnimation* CreateNewAnimation(HWND hPar, qdGameDispatcherBase* ptrDisp);

bool AskUserForName(CString& strName);
const CString AskUserForName(const CString& strDef);

/************************************************************************/
/* Глобальные функции                                                   */
/************************************************************************/

namespace ptree_add
{
/*!
 	Возвращает позицию персонажа. Чтобы он был поверх других объектов
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
 	Модифицирует глубину объекта. Каждому из 3х типов объектов сцены соответствует 
	своя глубина
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

//! Загружает заначения по умолчанию для сетки
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

//! По типу объекта возвращает имя по умолчанию
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

//! Хранитель текущей папки для миниигр
CurDirSafer& MGCurDirSafer(){
	static CurDirSafer _s_cur_dir;
	return _s_cur_dir;
}

//! Хранитель текущей папки для музыкальных треков
CurDirSafer& MTCurDirSafer(){
	static CurDirSafer _s_cur_dir;
	return _s_cur_dir;
}

CurDirSafer& FontCurDirSafer(){
	static CurDirSafer safer;
	return safer;
}
//! Хранитель текущей папки для интерфейса
CurDirSafer& INTERFCurDirSafer(){
	static CurDirSafer _s_cur_dir;
	return _s_cur_dir;
}

//! Получает имя длл для миниигры
const CString MGGetDLLName(){
	CString strFilter((LPTSTR)IDS_OPEN_MINIGAME_DLL_FILTER);
	CString strTitle((LPTSTR)IDS_OPEN_MINIGAME_DLL_TITLE);
	return disk_op::Get1DiskFileName(MGCurDirSafer, strFilter, _T("*.dll"), strTitle);
}

//! Получает имя конфиг-файла для миниигры
const CString MGGetCNFGName(){
	CString strFilter((LPTSTR)IDS_OPEN_MINIGAME_CONFIG_FILTER);
	CString strExt((LPTSTR)IDS_OPEN_MINIGAME_CONFIG_EXT);
	CString strTitle((LPTSTR)IDS_OPEN_MINIGAME_CONFIG_TITLE);
	return disk_op::Get1DiskFileName(MGCurDirSafer, strFilter, strExt, strTitle);
}

//! Получает имя файла для муз. трека
const CString GetMusicTrackFileName(){
	CString strFilter((LPTSTR)IDS_OPEN_MUSIC_TRACK_FILTER);
	CString strExt((LPTSTR)IDS_OPEN_MUSIC_TRACK_EXT);
	CString strTitle((LPTSTR)IDS_OPEN_MUSIC_TRACK_TITLE);
	return disk_op::Get1DiskFileName(MTCurDirSafer, strFilter, strExt, strTitle);
}

//! Получает имя файла шрифта
const CString GetFontFileName(){
	CString strFilter((LPTSTR)IDS_OPEN_FONT_FILE_FILTER);
	CString strExt((LPTSTR)IDS_OPEN_FONT_FILE_EXT);
	CString strTitle((LPTSTR)IDS_OPEN_FONT_FILE_TITLE);
	return disk_op::Get1DiskFileName(FontCurDirSafer, strFilter, strExt, strTitle);
}
//! Получает имя файла анимации
const CString GetAnimFileName(){
	CString strTitle((LPTSTR)IDS_OSD_CAP_SEL_ANIM);
	CString strFilter((LPTSTR)IDS_OPEN_ANIMATION_FILTER);
	CString strExt((LPTSTR)IDS_OPEN_ANIMATION_EXT);
	return disk_op::Get1DiskFileName(INTERFCurDirSafer, strFilter, strExt, strTitle);
}

//! \brief Возвращает глубину, которую надо установить, да бы интерфейсный элемент был 
//! поверх фона
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
Класс-темплейт для добавления интерфейсных элементов в интерфейсный экран. Как то - 
кнопок, ползунков, фонов...
Работает так: создает новый элемент, выбирает уникальное имя для него, добавляет в 
список интерфейсных элементов.
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
 	На основе \a strName получает уникальное имя, затем создает сцену, добавляет её в
	диспетчер(\a ptrDisp) и устанавливает значения по умолчанию для сетки
	(ptree_add::SetSceneNetDefaults())
	\param ptrDisp - диспетчер игры, куда будет добавлена новая сцена
	\param strName - имя новой сцены. Изменяется, если такое имя уже есть
	\return указатель на новый объект, либо NULL - в случае неудачи
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
 	Выбирает из списка походок первую походку. Добавляет походочное состояние 
	qdGameObjectStateWalk(ptree_api::AddEmptyMovementState()), назначает ему(состоянию) 
	походку. Выравнивает размеры персонажа(qdGameObjectMoving::auto_bound()) и 
	положение на сетке(qdGameObjectMoving::adjust_z())
	\return \a true - если удалось вставить.
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
 	Создает объект "GameObj" и назначает ему позицию (0, 0, 0)
	\return Указатель на созданный объект
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
 	Передает управление SceneTreeView::CreateGameObject(DWORD dwErrMsg)
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
 	Передает управление SceneTreeView::CreateGameObject(DWORD dwErrMsg)
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
 	Передает управление SceneTreeView::CreateGameObject(DWORD dwErrMsg)
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
 	Создает статический объект(SceneTreeView::CreateStatic()). Добавляет его в сцену(
	SceneTreeView::AddSceneObj()). Запускает диалог выбора изображения(
	StaticObjPage::SelectPicture()). Если изображение выбрали, то назначает его объекту,
	подгоняет под размер объекта размеры сцены. Выравнивает размеры битмапа в
	редактре сцены. Если картинка НЕ выбрана, то удаляет созданный объект
	\param hSceneItem	- Узел сцены, в которую будет добавлен объект
	\param ptrScene		- Сцена, в которую будет добавлен объект
	\return \a true - если объект был создан и добавлен
*/
bool SceneTreeView::CreateBackground(HTREEITEM hSceneItem, qdGameScene* ptrScene)
{
	//по имени находим каталог, в котором храняться все статические объекты
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
 	Открывает диалог выбора изображения(StaticObjPage::SelectPicture()).  Если указаны, 
	то: создает статический объект(
	SceneTreeView::CreateStatic()); добавляет его в сцену(SceneTreeView::AddSceneObj());
	назначает ему изображение; кладет объект в список отсортированный по площади объекта.
	Все это выполняется в цикле. Цикл прерывается, если узображения для объектов не были
	указаны. После выхода из цикла функция пробегает по составленному списку объектов
	и назначает им глубину в зависимостити от площади. Чем больше площадь, тем больше 
	глубина
	\param hSceneItem - Узел сцены, в которую добавляются объекты
	\retval true - всегда
*/
bool SceneTreeView::AddStatics(HTREEITEM hSceneItem)
{
	CTreeCtrl& tree = GetTreeCtrl();
	qdGameScene* ptrScene = (qdGameScene*)tree.GetItemData(hSceneItem);
	
	//по имени находим каталог, в котором храняться все статические объекты
	CString str;
	str.LoadString(IDS_STATIC_OBJS_NODE);
	HTREEITEM hCatalog = ptree_helpers::FindTreeItemByText(GetTreeCtrl(), hSceneItem,str);
	ASSERT(hCatalog);

	CStringVect_t vecFiles;
	std::multimap<int, qdGameObject*> objSort;
	CString strOpenDlgTitle((LPTSTR)IDS_OSD_CAP_SEL_STAT_OBJS);
	while (true) {
		vecFiles.clear();
		//выбираем картинку
		CString picture = 
			StaticObjPage::SelectPicture(this->m_hWnd, strOpenDlgTitle, &vecFiles);
		if(picture.IsEmpty())
			break;
		
		CStringVect_t::iterator _itr = vecFiles.begin(), _end = vecFiles.end();
		for(; _itr != _end; ++_itr)
		{
			CString strName = disk_op::GetFileName(*_itr);
			ASSERT(!strName.IsEmpty());
			//создаём объект и добавляем его в сцену


			std::auto_ptr<qdGameObjectStatic> pobj(CreateStatic());
			if (!pobj.get())
				break;
			if(!AddSceneObj(pobj.get(), hCatalog, strName, II_STATIC_OBJ_NODE))
				break;

			//загружаем картинку
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
	Передает управление SceneTreeView::AddScene(HTREEITEM hItemParent, CString& strName).
	Если \a bShowNew = true, то выделяет узел новой сцены
	\param strName	- имя новой сцены
	\param bShowNew	- переносить или нет выделение на новый узел
	\return \a true - если добавилась удачно
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
 	Создаёт сцену и добавляет её в диспетчер игры
	(SceneTreeView::CreateScene(qdGameDispatcher*, CString&)).
	Загружает сцену в дерево(SceneTreeView::LoadScene(HTREEITEM hPar, qdGameScene* pscene)
	\param hItemParent	- родительский узел в дереве
	\param strName		- имя новой сцены
	\return описатель узла сцены в дереве, NULL в случае неудачи
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
	добавляет объект в сцену(SceneTreeView::AddToScene(qdGameScene* ptrScene, qdGameObject* ptrObj, const CString& strName)), 
	получая уникальное имя, в дерево(SceneTreeView::AddGOToTree(HTREEITEM hItemParent, qdGameObject* ptr, UINT uiImage)).
	После вставки показывает узел объекта.
	Используется при добавлении нескольких объектов
	\param ptr			- укзатель на объект
	\param hItemParent	- родительский узел в дереве
	\param strName		- имя. Если содержит пустую строку, то из 
							ресурсов загружается имя по умолчанию
	\param uiImage		- номер картики для узла объекта
	\return \a true - если удалось вставить
*/
bool SceneTreeView::AddSceneObj(qdGameObject* ptr, HTREEITEM hItemParent, 
								const CString &strName,UINT uiImage)
{
	CTreeCtrl& wndTree = GetTreeCtrl();
	HTREEITEM hSceneItem = wndTree.GetParentItem(hItemParent);
	ASSERT(hSceneItem);
	qdGameScene* ptrScene = (qdGameScene*)wndTree.GetItemData(hSceneItem);
	ASSERT(!IsBadReadPtr(ptrScene, sizeof(qdGameScene)));
	//добавляет, получая уникальное имя
	AddToScene(ptrScene, ptr, strName);
	if (HTREEITEM hItem = AddGOToTree(hItemParent, ptr, uiImage))
		ShowItem(hItem);
	
	SetProjChange();
	return true;
}

/*!
 	Тестирует нет ли в сцене объекта с таким же именем. Если есть заменяет имя на
	уникальное и назначает его объекту. Добавляет объект в сцену.
	\param ptrScene - Сцена
	\param ptrObj	- Объект
	\param strName  - Имя объекта
	\retval true - если добавлися успешно
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
 	Находит каталог для вставки объекта(ptree_helpers::FindTreeItemByText()) и 
	передает управление SceneTreeView::AddGOToTree(HTREEITEM hItemParent, 
	qdGameObject* ptr, UINT uiImage)

	\param hSceneItem	- Узел сцены, которой принадлежит объект
	\param uiCatNameID	- Ид ресурсной строки, содержащей имя каталога, в который 
							будет добавлен объект
	\param ptr			-  Объект
	\param uiImage		- Изображение для узла объекта
	\return Описатель узла объекта
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
 	Вставляет объект в дерево(ptree_loading::InsertNamedObject()). Если это не
	статический объект, то загружает его состояния в дерево(ptree_loading::LoadStates())
	\param hItemParent	- Родительский узел в дереве
	\param ptr			- Объект для вставки
	\param uiImage		- Изображение для узла объекта
	\return Описатель узла объекта
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
	Вызывает SceneTreeView::AddPersonage (qdGameScene* ptrScene, 
	const CString &strName, bool bShow, bool bAdd2Tree) для добавления персонажа.
	SceneTreeView::InsertFirstGaitToPersonage (qdGameObjectMoving* ptrPers) - для
	вставки походки.
	\param ptrScene - сцена куда добвляем персонажа.
	\param strName  - имя персонажа
 	\return Указатель на созданный объект
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
 	Создает персонаж(SceneTreeView::CreatePersonage()), добавляет его в список 
	глобальных объектов и в дерево, делает копии персонажа в сценах(
	SceneTreeView::GlobalObjToScenes(qdGameObject const* pobj)), обновляет содержимое
	каталогов персонажей в сценах
	\param ptrDisp	- Указатель на диспетчер игры
	\param strName	- Имя персонажа
	\param bShow	- Переводить ли выделение на узел добавленного обоъекта
	\return Указатель на созданный объект
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
			//Нашли каталог глобальных персонажей
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
 	В зависимости от типа глобальноно объекта вызывает 
	SceneTreeView::AddPersonage(qdGameScene* ptrScene, const CString &strName, 
	bool bShow, bool bAdd2Tree), SceneTreeView::AddAnimated(
	qdGameScene* ptrScene, const CString &strName, bool bShow) или 
	SceneTreeView::AddStatic(qdGameScene* ptrScene, const CString &strName, bool bShow)
	\param pscene	- сцена, куда будет добавлена копия
	\param pobj		- объект, копия которого будет создаваться
	\return Указатель на созданную копию объекта. NULL в случае неудачи
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
 	Созадает персонаж, добавляет его в сцену(SceneTreeView::AddToScene(
		qdGameScene* ptrScene, qdGameObject* ptrObj, const CString& strName)), если 
	требуется, то добавляет объект в дерево. Если персонаж копия глобального, то
	объеденяем его с глобальным объектом
	\param	ptrScene	- Сцена, куда добавляем объект
	\param  strName		- Имя нового персонажа
	\param	bShow		- Переносить ли выделение на узел добавленного объекта
	\param	bAdd2Tree	- Добавлять объект в дерево
	\return Указатель на вновь созданный объкт
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
 	Создает объект (SceneTreeView::CreateAnimated()), добавляет в сцену
	(SceneTreeView::AddToScene(qdGameScene* ptrScene, qdGameObject* ptrObj, 
	const CString& strName). Вставляет объект в дерево. Если надо переводит на 
	узел объекта выделение.
	\param ptrScene - сцена, в которую добавится объект
	\param strName	- имя нового объекта
	\param	bShow	- Переносить ли выделение на узел добавленного объекта
	\return Указатль на созданный объект
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
	Создает объект(SceneTreeView::CreateStatic()), добавляет его в сцену и в дерево.
	Если надо выделяет узел
	\param ptrScene - сцена, в которую добавится объект
	\param strName	- имя нового объекта
	\param	bShow	- Переносить ли выделение на узел добавленного объекта
	\return Указатль на созданный объект
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
 	Сохраняет текущую директорию(при выходе из функции она будет восстановлена).
	Показыает диалог выбора файла. Если файл выбран, то получает относительный путь от
	каталога проекта(r_p::RelativePath()). Сохраняет текущую директорию.
	Вызывает SceneTreeView::AddVideoFile() для добавления файла в проект
	\retval true - если видео было добавлено
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
 	Создает видео объект (qdVideo). Назначает ему имя по имени файла, и добавляет
	видео ролик в игровой диспетчер. В дерево добавления не происходт
	\param strFileName - имя файла видео-ролика.
	\retval true - если удалось добавить видео-ролик
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
/* Инвентори                                                            */
/************************************************************************/

/*!
 	Запрашивает у пользователя имена файлов(StaticObjPage::SelectPicture()), 
	хранящих изображения ячеек. Добавляет указанные типы диспетчер. Если
	добавление прошло удачно, спрашиваем "Добавлять ли еще"
 */
void SceneTreeView::AddInventoryCellTypes()
{
	CStringVect_t vecFiles;
	CString strOpenDlgTitle((LPTSTR)IDS_OSD_CAP_SEL_CELL_TYPE);
	int nCountAdded = 0;

	while (true) {
		vecFiles.clear();
		//выбираем картинку
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
 	Создает объект инвентори. Если режим создания(\a mode) ICM_USE_EXISTING_CELL_TYPES,
	то в инвентори для каждого типа ячеек добавляется набор ячеек, состоящий из одной
	ячейки.	Наборы располагаются, так чтобы не перескаться
	\param mode - рижим создания.
	\return	Указатель на вновь созданый объект, либо NULL в случае неудачи
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
				//типа переходим на новую строчку
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
 	Находит узел-каталог для инвентори.Вставляет туда \a pinv
	(ptree_helperes::InsertNamedObject()). Убирает чекбокс(SceneTreeView::SetNoCheck())
	и делает узел видимым(SceneTreeView::ShowItem())
	\param pinv - объект инвентори
	\return описатель узла в дереве, либо NULL в случае неудачи
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
 	Получает уникальное имя на основе strName, назначает его объекту
	(qdNamedObject::set_name()). Добавляет инвентори в диспетчер игры
	(qdGameDispatcher::add_inventory());
	\param pinv		- объект инвентори
	\param strName	- имя для нового инвентори
	\return \a true - если pinv не NULL и удачно вставили в диспетчер игры
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
/* Миниигры                                                             */
/************************************************************************/
/*!
	\param strName			- Имя нового объекта
	\param strDllName		- Имя файла длл
	\param strConfigName	- Имя файла конфиг-файла
	\return Укзатель на созданный объект. 	
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
 	Запрашивает у пользователя имя файла длл(ptree_add::MGGetDLLName()). Запрашивает
	у пользователя имя конфиг-файла(ptree_add::MGGetCNFGName()). Запрашивает имя(
	bool AskUserForName (CString& strName)). Создает миниигру. Вставляет её в дерево(
	SceneTreeView::InsertMGToTree()). Добавляет в список миниигр. Добавляет состояния 
	выигрыша и проигрыша к миниигре(SceneTreeView::AddSimpleMGState())
	\return \a true - если все удалось 
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
	\param pgame - Указатель на миниигру
 	\return \a true - если состояния добавилсь успешно
*/
bool SceneTreeView::AddSimpleMGState(qdMiniGame* pgame){

/*
	//первое простое будет проигрышем. Очков 0 имя "Проигрыш"
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
/* Цепочки триггеров                                                    */
/************************************************************************/
/*!
 	Создает цепочку (SceneTreeView::CreateTriggerChain()). Задает ей уникальное имя.
	Добавляет в список цепочек триггеров. Вставляет цепочку в дерево.
	\param hPar		- Родительский узел в дереве
	\param strName	- Имя цепочки триггеров
	\param bShow	- Переносить ли выделение, на добавленный узел
	\return Указатель на добавленный объект
*/
qdTriggerChain* SceneTreeView::AddTriggerChain(HTREEITEM hPar, 
											   const CString& strName,
											   bool bShow)
{
	std::auto_ptr<qdTriggerChain> res (CreateTriggerChain());
	if (!res.get()) {
		return NULL;
	}

	//получаем имя для новой цепочки
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
 	Создает объект qdTriggerChain и делает корневой узел активным
	\return Указатель на созданный объект.
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
/* Зоны на сетке                                                        */
/************************************************************************/
/*!
 	Находит узел сетки в текущей сцене. Запрашивает у пользователя имя объекта
	(bool AskUserForName (CString& strName)), добавляет зону в список зон сцены. 
	Вставляет объект в дерево.
	\param ptrZone - зона
	\return \a true - если добавление удалось
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
/* Музыкальные треки                                                    */
/************************************************************************/
/*!
	\param strName - имя музыкального трека
	\return Указатель на созданный объект
*/
std::auto_ptr<qdMusicTrack> SceneTreeView::CreateMusicTrack(const CString& strName){
	std::auto_ptr<qdMusicTrack> ptr(new qdMusicTrack);
	if (ptr.get() == NULL)
		return ptr;
	ptr->set_name(strName);
	return ptr;
}

/*!
	Запрашивает у пользователя файл муз. трека(ptree_add::GetMusicTrackFileName())
	Запрашивает у пользователя имя муз. трека(bool AskUserForName (CString& strName)).
	Создает муз. трек и добавляет его в дерево
	\param pscene		- Сцена, куда будет добавлен муз. трек
	\param hMTCatalog	- Узел каталога музыкальных треков
	\return \a true	- если добавление прошло успешно
*/
bool SceneTreeView::AddMusicTrack(qdGameScene* pscene, HTREEITEM hMTCatalog)
{
	CString strFileName = GetMusicTrackFileName();
	if (strFileName.IsEmpty())
		return false;

	CString strMTrackName = disk_op::GetFileName(strFileName);
	if (!AskUserForName(strMTrackName)) 
		return false;

	//получаем уникальное имя
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
	Создает объект qdFontInfo с указанным именем
	\return NULL - если не удалось создать
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

	//получаем уникальное имя
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

	//получаем уникальное имя
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

	//получаем уникальное имя
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
/* Создание интерфейса                                                  */
/************************************************************************/

const DWORD INTERF_BACK_DEPTH = 30;
const DWORD INTERF_BTN_DEPTH = 15;
const DWORD INTERF_SAVE_DEPTH = 15;
const DWORD INTERF_SLIDER_DEPTH = 15;
const DWORD INTERF_TEXT_WINDOW_DEPTH = 15;
const DWORD INTERF_COUNTER_DEPTH = 15;

/*!
 	Создает объект "интерфейсный экран" и добавляет его в список экранов(
	локальный класс add_scr::operator()()). Вставляет в дерево(
	ptree_loading::InsertObject()).
	\param hItemIntef	- Родительский узел.(Узел с текстом "Интерфейс")
	\param name			- Имя нового объекта
	\return Указатель на созданный объект
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
 	Создает и добавляет объект в список элментов экрана(ptree_add::add_ele::operator()())
	Вычисляет глубину(ptree_add::GetInterfEleDepthOverBack()) и устанавилвает её. 
	Вставляет в дерево(ptree_loading::InsertObject())
	\param pscr		- Экран, в который будет добавлена кнопка
	\param hItemScr - Узел экрана в дереве
	\param name		- имя новой кнопки
	\return Указатель на созданную кнопку
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
 	Создает и добавляет объект в список элментов экрана(ptree_add::add_ele::operator()())
	Вычисляет глубину(ptree_add::GetInterfEleDepthOverBack()) и устанавилвает её. 
	Вставляет в дерево(ptree_loading::InsertObject())
	\param pscr		- Экран, в который будет добавлен фон
	\param hItemScr - Узел экрана в дереве
	\param name		- имя фона
	\return Указатель на созданный фон
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
 	Создает и добавляет объект в список элментов экрана(ptree_add::add_ele::operator()())
	Вычисляет глубину(ptree_add::GetInterfEleDepthOverBack()) и устанавилвает её. 
	Вставляет в дерево(ptree_loading::InsertObject())
	\param pscr		- Экран, в который будет добавлен сейв
	\param hItemScr - Узел экрана в дереве
	\param name		- имя сейва
	\return Указатель на созданный сейв
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
 	Создает и добавляет объект в список элментов экрана(ptree_add::add_ele::operator()())
	Вычисляет глубину(ptree_add::GetInterfEleDepthOverBack()) и устанавилвает её. 
	Вставляет в дерево(ptree_loading::InsertObject())
	\param pscr		- Экран, в который будет добавлен регулятор
	\param hItemScr - Узел экрана в дереве
	\param name		- имя нового регулятора
	\return Указатель на созданный регулятор
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
 	Создает и добавляет объект в список элментов экрана(ptree_add::add_ele::operator()())
	Вычисляет глубину(ptree_add::GetInterfEleDepthOverBack()) и устанавилвает её. 
	Вставляет в дерево(ptree_loading::InsertObject())
	\param pscr		- Экран, в который будет добавлен регулятор
	\param hItemScr - Узел экрана в дереве
	\param name		- имя нового регулятора
	\return Указатель на созданный регулятор
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
 	Создает и добавляет состояние(локальный класс add_state). Вставляет состояние
	в дерево(ptree_loading::InsertObject())
	\return Порядковый номер состояния.
	\retval -1 В случае неудачи
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
 	Получает уникальное имя на основе \a name. Создает объект и назначает имя
	интерфейсного экрана(\a scrName).
	\param name		- имя конечного экрана
	\param scrName	- задает имя интерфейсного экрана
	\return указатель на вновь созданный объект. NULL в случае неудачи
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
 	Создает объект "Конечный экран"(SceneTreeView::CreateGameEnd(CString const& name, 
	CString const& scrName)). Вставляет его в дерево(ptree_loading::InsertObject()).
	Добавляет в список конечных экранов
	\param hParent	- Родительский узел в дереве
	\param name		- имя нового объекта
	\param scrName	- Имя интерфесного экрана
	\return Указатель на соданный объект
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
 	Создает новый объект и назначает ему имя.
	\param name - имя счетчика
	\return указатель на новый объект. NULL в случае неудачи
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
 	Если \a name - пустая строка, то запрашивает имя у пользовтеля(
	bool AskUserForName (CString& strName)). Создает объект "Счетчик"(
	SceneTreeView::CreateCounter(CString const& name)). Вставляет его в дерево(
	ptree_loading::InsertObject()).Добавляет в список счетчиков. Запускает 
	диалог свойств(SceneTreeView::ShowCorrespondingPropDlg(qdInterfaceObjectBase* ptrObj,
	int iActive)). 
	\param hParent	- Родительский узел в дереве
	\param name		- Имя нового объекта
	\return	Указатель на новый объект
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