#include "stdafx.h"
#include "resource.h"
#include ".\projectmodifier.h"

#include "name_op.h"
#include "ptree_consts.h"
#include "disk_op.h"
#include "RelativePath.h"
#include "StaticObjPage.h"

#include "Project.h"
#include "GameObjectFactory.h"
#include "ISceneViewFacade.h"

#include "Tree/TreeLogic.h"
#include "Tree/ITreeNodeFactory.h"

#include "font_generator.h"

#include "qd_game_object_moving.h"
#include "qd_game_scene.h"
#include "qd_game_object_static.h"
#include "qd_game_end.h"
#include "qd_font_info.h"
#include "qd_video.h"
#include "qd_minigame.h"
#include "qd_counter.h"
#include "qd_trigger_chain.h"
#include "qd_grid_zone.h"

#include "qd_interface_screen.h"
#include "qd_interface_background.h"
#include "qd_interface_slider.h"
#include "qd_interface_text_window.h"
#include "qd_interface_save.h"
#include "qd_interface_button.h"
#include "qd_interface_counter.h"

#include "Main/ObjectTraits.h"
#include "Main/StateTraits.h"

#include "Main/Utils/RestoreImageLayout.h"

#include <boost/rtl.hpp>

const DWORD INTERF_BACK_DEPTH = 30;
const DWORD INTERF_BTN_DEPTH = 15;
const DWORD INTERF_SAVE_DEPTH = 15;
const DWORD INTERF_SLIDER_DEPTH = 15;
const DWORD INTERF_TEXT_WINDOW_DEPTH = 15;
const DWORD INTERF_COUNTER_DEPTH = 15;

void ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID = -1);
void ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID = -1);

CurDirSafer& GetVideoCurDirSafer();

bool AskUserForName(CString& strDef);
CString const AskUserForName(CString const& strDef);
BOOL AddCellSet(qdGameDispatcher* ptrDisp, qdInventory* ptrInv);

namespace
{
/*!
 	Модифицирует глубину объекта. Каждому из 3х типов объектов сцены соответствует 
	своя глубина
*/
const Vect3f getObjectStartPos(qdGameScene* ptrScene, qdNamedObjectType type, 
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

//! По типу объекта возвращает имя по умолчанию
CString getDefaultObjName(qdNamedObjectType type)
{
	UINT resID = 0;
	switch(type) {
	case QD_NAMED_OBJECT_MOVING_OBJ:
		resID = IDS_NEW_MOVING_OBJ_NAME;
		break;
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
		resID = IDS_NEW_ANIMATED_OBJ_NAME;
		break;
	case QD_NAMED_OBJECT_STATIC_OBJ:
		resID = IDS_NEW_STATIC_OBJ_NAME;
		break;
	case QD_NAMED_OBJECT_SCENE:
		resID = IDS_NEW_SCENE_NAME;
		break;
	default:
		ASSERT(("GetDefaultObjName", 0));
	}
	if(resID)
		return CString((LPTSTR)resID);
	return CString();
}

template<typename Container>
CString selectObjectName(Container const& cntr, CString name, qdNamedObjectType type)
{
	if(name.IsEmpty())
		name = getDefaultObjName(type);

	return get_unique_name(name, cntr);
}

//! \brief Возвращает глубину, которую надо установить, да бы интерфейсный элемент был 
//! поверх фона
int getInterfEleDepthOverBack(qdInterfaceScreen const* pscr, int def)
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

//! Хранитель текущей папки для миниигр
CurDirSafer& mgCurDirSafer(){
	static CurDirSafer _s_cur_dir;
	return _s_cur_dir;
}

//! Хранитель текущей папки для музыкальных треков
CurDirSafer& mtCurDirSafer(){
	static CurDirSafer _s_cur_dir;
	return _s_cur_dir;
}

CurDirSafer& fontCurDirSafer(){
	static CurDirSafer safer;
	return safer;
}
//! Хранитель текущей папки для интерфейса
CurDirSafer& interfCurDirSafer(){
	static CurDirSafer _s_cur_dir;
	return _s_cur_dir;
}

//! Получает имя длл для миниигры
const CString mgGetDLLName(){
	CString strFilter((LPTSTR)IDS_OPEN_MINIGAME_DLL_FILTER);
	CString strTitle((LPTSTR)IDS_OPEN_MINIGAME_DLL_TITLE);
	return disk_op::Get1DiskFileName(mgCurDirSafer, strFilter, _T("*.dll"), strTitle);
}

//! Получает имя конфиг-файла для миниигры
const CString mgGetCNFGName(){
	CString strFilter((LPTSTR)IDS_OPEN_MINIGAME_CONFIG_FILTER);
	CString strExt((LPTSTR)IDS_OPEN_MINIGAME_CONFIG_EXT);
	CString strTitle((LPTSTR)IDS_OPEN_MINIGAME_CONFIG_TITLE);
	return disk_op::Get1DiskFileName(mgCurDirSafer, strFilter, strExt, strTitle);
}

//! Получает имя файла для муз. трека
const CString getMusicTrackFileName(){
	CString strFilter((LPTSTR)IDS_OPEN_MUSIC_TRACK_FILTER);
	CString strExt((LPTSTR)IDS_OPEN_MUSIC_TRACK_EXT);
	CString strTitle((LPTSTR)IDS_OPEN_MUSIC_TRACK_TITLE);
	return disk_op::Get1DiskFileName(mgCurDirSafer, strFilter, strExt, strTitle);
}

//! Получает имя файла анимации
const CString getAnimFileName(){
	CString strTitle((LPTSTR)IDS_OSD_CAP_SEL_ANIM);
	CString strFilter((LPTSTR)IDS_OPEN_ANIMATION_FILTER);
	CString strExt((LPTSTR)IDS_OPEN_ANIMATION_EXT);
	return disk_op::Get1DiskFileName(interfCurDirSafer, strFilter, strExt, strTitle);
}

}// end of anonymous namespace

//! Получает имя файла шрифта
const CString getFontFileName(){
	CString strFilter((LPTSTR)IDS_OPEN_FONT_FILE_FILTER);
	CString strExt((LPTSTR)IDS_OPEN_FONT_FILE_EXT);
	CString strTitle((LPTSTR)IDS_OPEN_FONT_FILE_TITLE);
	return disk_op::Get1DiskFileName(fontCurDirSafer, strFilter, strExt, strTitle);
}


ProjectModifier::ProjectModifier(GameObjectFactory& gameObjectFactory, Project* project)
: gameObjectFactory_(gameObjectFactory)
, project_(project)
{
}

ProjectModifier::~ProjectModifier(void)
{
}

Project* ProjectModifier::getProject() const{
	return project_;
}
GameObjectFactory& ProjectModifier::getGameObjectFactory(){
	return gameObjectFactory_;
}

TreeLogic& ProjectModifier::getTreeLogic() const{
	return project_->getTreeLogic();
}

ITreeNodeFactory& ProjectModifier::getTreeNodeFactory() const{
	return project_->getTreeNodeFactory();
}

qdGameDispatcher* ProjectModifier::getGameDispatcher() const{
	return project_->getGameDispatcher();
}

qdGameObjectStatic* ProjectModifier::addStatic(qdGameScene* ptrScene,
											   CString const& strName,
											   bool bShowInTree)
{
	std::auto_ptr<qdGameObjectStatic> ptrObj( 
		getGameObjectFactory().createStatic(
			selectObjectName(ptrScene->object_list(),
					strName, QD_NAMED_OBJECT_STATIC_OBJ)));
	if(ptrObj.get())
	{
		ptrObj->set_pos(
			getObjectStartPos(ptrScene, QD_NAMED_OBJECT_STATIC_OBJ, ptrObj->R()));

		ITreeNode* sceneNode = *getTreeLogic().getObjectNodes(ptrScene);
		ITreeNode* objFolderNode = 
			getTreeLogic().getSubfolder(sceneNode, QD_NAMED_OBJECT_STATIC_OBJ);

		std::auto_ptr<ITreeNode> node = 
			getTreeNodeFactory().createGameObjectNode(ptrObj.get());
		if (!node.get())
			return NULL;

		if (ptrScene->add_object(ptrObj.get()))
		{			   

			node->load(&getTreeNodeFactory(), 
				getTreeLogic(), 
				objFolderNode, 
				ptrScene, TN_INSERT_SORT);
			if (node->getTreeListItem()&&bShowInTree)
				getTreeLogic().showNode(node.get());
			node.release();
		}
		else
			return NULL;
	}
	return ptrObj.release();
}


/*!
Создает объект (CreateAnimated()), добавляет в сцену
(AddToScene(qdGameScene* ptrScene, qdGameObject* ptrObj, 
const CString& strName). Вставляет объект в дерево. Если надо переводит на 
узел объекта выделение.
\param ptrScene - сцена, в которую добавится объект
\param strName	- имя нового объекта
\param	bShow	- Переносить ли выделение на узел добавленного объекта
\return Указатль на созданный объект
*/
qdGameObjectAnimated* ProjectModifier::addAnimated(qdGameScene* ptrScene, 
												 CString const& strName, 
												 bool bShow)
{
	std::auto_ptr<qdGameObjectAnimated> ptrObj( 
		getGameObjectFactory().createAnimated(
			selectObjectName(ptrScene->object_list(),
							strName, QD_NAMED_OBJECT_ANIMATED_OBJ)));
	if(ptrObj.get())
	{
		ptrObj->set_pos(
			getObjectStartPos(ptrScene, QD_NAMED_OBJECT_ANIMATED_OBJ, ptrObj->R()));
		ptrObj->set_default_pos(ptrObj->R());
		ITreeNode* sceneNode = *getTreeLogic().getObjectNodes(ptrScene);
		CString str;
		str.LoadString(IDS_ANIMATED_OBJS_NODE);
		ITreeNode* objFolderNode = getTreeLogic().getSubNodeByName(sceneNode, str);

		std::auto_ptr<ITreeNode> node = 
			getTreeNodeFactory().createGameObjectNode(ptrObj.get());
		if (!node.get())
			return NULL;

		if (ptrScene->add_object(ptrObj.get()))
		{			   

			node->load(&getTreeNodeFactory(), 
				getTreeLogic(), 
				objFolderNode, 
				ptrScene, TN_INSERT_SORT);
			if (node->getTreeListItem()&&bShow)
				getTreeLogic().showNode(node.get());
			node.release();
		}
		else
			return NULL;
	}
	return ptrObj.release();
}

qdGameObjectMoving* ProjectModifier::addPersonage(qdGameScene* ptrScene,
												  CString const& strName,
												  bool bShowInTree)
{
	std::auto_ptr<qdGameObjectMoving> ptrObj( 
					getGameObjectFactory().createPersonage(
											selectObjectName(
														ptrScene->object_list(), 
														strName, 
														QD_NAMED_OBJECT_MOVING_OBJ)
														)
												);
	if(ptrObj.get())
	{
		ptrObj->set_pos(
			getObjectStartPos(ptrScene, QD_NAMED_OBJECT_MOVING_OBJ, ptrObj->R()));
		ptrObj->set_default_pos(ptrObj->R());

		ITreeNode* sceneNode = *getTreeLogic().getObjectNodes(ptrScene);
		ITreeNode* objFolderNode = 
			getTreeLogic().getSubfolder(sceneNode, QD_NAMED_OBJECT_MOVING_OBJ);

		std::auto_ptr<ITreeNode> node = 
			getTreeNodeFactory().createGameObjectNode(ptrObj.get());
		if (!node.get())
			return NULL;

		if (ptrScene->add_object(ptrObj.get()))
		{		
			qdGameObject* p = 
				ObjectTraits::isCopyOfGlobal(getGameDispatcher(), ptrObj.get());
			if (p) 
				ptrScene->merge_global_objects(p);

			node->load(&getTreeNodeFactory(), 
				getTreeLogic(), 
				objFolderNode, 
				ptrScene, TN_INSERT_SORT);
			if (node->getTreeListItem()&&bShowInTree)
				getTreeLogic().showNode(node.get());
			node.release();
		}
		else
			return NULL;
	}
	return ptrObj.release();
}

qdGameObjectMoving* ProjectModifier::addGlobalPersonage(qdGameDispatcher* ptrDisp,
									   CString &strName, bool bShow)
{
	std::auto_ptr<qdGameObjectMoving> ptrObj(
		getGameObjectFactory().createPersonage(
									selectObjectName(
										ptrDisp->global_object_list(), 
										strName, 
										QD_NAMED_OBJECT_MOVING_OBJ
													)
												)
											);
	if(ptrObj.get())
	{
		ITreeNode* dispNode = *getTreeLogic().getObjectNodes(ptrDisp);
		ITreeNode* objFolderNode = 
			getTreeLogic().getSubfolder(dispNode, QD_NAMED_OBJECT_MOVING_OBJ);
		std::auto_ptr<ITreeNode> node = 
			getTreeNodeFactory().createGlobalGameObjectNode(ptrObj.get());
		if (!node.get())
			return NULL;

		if(!ptrDisp->add_global_object(ptrObj.get()))
			return NULL;
		node->load(&getTreeNodeFactory(), 
			getTreeLogic(), 
			objFolderNode, 
			ptrDisp, TN_INSERT_SORT);

		globalObjToScenes(ptrObj.get());

		if (node->getTreeListItem()&&bShow) 
			getTreeLogic().showNode(node.get());
		node.release();
	}		
	return ptrObj.release();
}

/*!
В зависимости от типа глобальноно объекта вызывает 
AddPersonage(qdGameScene* ptrScene, const CString &strName, 
bool bShow, bool bAdd2Tree), AddAnimated(
qdGameScene* ptrScene, const CString &strName, bool bShow) или 
AddStatic(qdGameScene* ptrScene, const CString &strName, bool bShow)
\param pscene	- сцена, куда будет добавлена копия
\param pobj		- объект, копия которого будет создаваться
\return Указатель на созданную копию объекта. NULL в случае неудачи
*/
qdGameObject* ProjectModifier::addGlobalObj2Scene(qdGameScene* pscene,
												qdGameObject const* pobj)
{
	int type = pobj->named_object_type();
	if (type == QD_NAMED_OBJECT_MOVING_OBJ) {
		return addPersonage(pscene, pobj->name(), false);
	}
	else if (type == QD_NAMED_OBJECT_ANIMATED_OBJ) {
		return addAnimated(pscene, pobj->name(), false);
	}
	else if (type == QD_NAMED_OBJECT_STATIC_OBJ) {
		return addStatic(pscene, pobj->name(), false);
	}
	ASSERT(0);
	return NULL;
}

/*!
Пробегает по списку сцен и проверяет: есть ли объект с таким именем в сцене.
Если нет, то создает объект-копию(AddGlobalObj2Scene(
qdGameScene* pscene, const qdGameObject* pobj)).
\param pobj - глобальный объект, копии которого будем создавать
*/
void ProjectModifier::globalObjToScenes(qdGameObject const* pobj)
{
	boost::rtl::crange<const qdGameSceneList> r(getGameDispatcher()->scene_list());
	for(; r; ++r)
	{
		qdGameScene* pscene = *r;
		if (!pscene->get_object(pobj->name())) {
			addGlobalObj2Scene(pscene, pobj);
		}
	}
}

/*!
Открывает диалог выбора изображения(StaticObjPage::SelectPicture()).  Если указаны, 
то: добавляет статический объект addStatic
назначает ему изображение; кладет объект в список отсортированный по площади объекта.
Все это выполняется в цикле. Цикл прерывается, если узображения для объектов не были
указаны. После выхода из цикла функция пробегает по составленному списку объектов
и назначает им глубину в зависимостити от площади. Чем больше площадь, тем больше 
глубина
\param hSceneItem - Узел сцены, в которую добавляются объекты
\retval true - если был добавлен хотя бы один объект
*/
bool ProjectModifier::addStatics(ITreeNode* sceneNode)
{
	bool res = false;
	qdGameScene* scene = static_cast<qdGameScene*>(sceneNode->getContent().gameObject);

	TreeLogic& logic = getTreeLogic();
	ITreeNode* folder = logic.getSubfolder(sceneNode, QD_NAMED_OBJECT_STATIC_OBJ);

	CStringVect_t vecFiles;
	std::multimap<int, qdGameObject*> objSort;
	CString strOpenDlgTitle((LPTSTR)IDS_OSD_CAP_SEL_STAT_OBJS);
	while (true) {
		vecFiles.clear();
		//выбираем картинку
		CString picture = StaticObjPage::SelectPicture(*logic.getBaseWnd(), 
										strOpenDlgTitle, &vecFiles);
		if(picture.IsEmpty())
			break;

		CStringVect_t::iterator _itr = vecFiles.begin(), _end = vecFiles.end();
		for(; _itr != _end; ++_itr)
		{
			CString strName = disk_op::GetFileName(*_itr);
			ASSERT(!strName.IsEmpty());
			//создаём объект и добавляем его в сцену


			qdGameObjectStatic* object = addStatic(scene, strName, false);
			if (!object)
				break;
			res = true;
			//загружаем картинку
			qdSprite* psprite = object->get_sprite();
			ASSERT(psprite);
			psprite->set_file(*_itr);
			object->load_resources();

			Vect2s vSize = object->screen_size();
			objSort.insert(std::multimap<int, qdGameObject*>::value_type(
				int(vSize.x)*vSize.y, object));
		}
		CString strMsg;
		strMsg.LoadString(IDS_STATICS_ADDED_SUCCESSFULL);
		CString cap;
		cap.LoadString(IDS_INFORMATION);
		if (MessageBox(logic.getBaseWnd()->m_hWnd, strMsg, cap, 
					MB_ICONINFORMATION|MB_YESNO|MB_DEFBUTTON2) == IDNO) 
			break;
	}

	std::multimap<int, qdGameObject*>::iterator beg = objSort.begin(), 
		_end = objSort.end();

	qdCamera* pcam = scene->get_camera();
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
	return res;
}

qdGameScene*	ProjectModifier::addScene(CString& name, bool bShow)
{
	TreeLogic& logic = getTreeLogic();
	qdGameDispatcher* gameDisp = getGameDispatcher();
	ITreeNode* gameDispNode = *logic.getObjectNodes(gameDisp);

	std::auto_ptr<qdGameScene> scene = 
		getGameObjectFactory().createScene(
						selectObjectName(gameDisp->scene_list(), 
										name,
										QD_NAMED_OBJECT_SCENE)
						);
	if (!scene.get())
		return NULL;

	std::auto_ptr<ITreeNode> node = 
		getTreeNodeFactory().createGameObjectNode(scene.get());

	if (!node.get())
		return NULL;
	if (gameDisp->add_scene(scene.get()))
	{
		node->load(&getTreeNodeFactory(), 
			getTreeLogic(), gameDispNode, gameDisp, TN_INSERT_SORT);

	}

	if(bShow)
	{
		if(logic.getCurrentSceneNode())
			logic.expand(logic.getCurrentSceneNode(), TreeLogic::EC_COLLAPSE);

		logic.showNode(node.get());
		logic.expand(node.get(), TreeLogic::EC_EXPAND);
	}
	node.release();
	return scene.release();
}

bool ProjectModifier::addBackground(ITreeNode* sceneNode, qdGameScene* scene)
{
	//по имени находим каталог, в котором храняться все статические объекты

	TreeLogic& logic = getTreeLogic();
	ITreeNode* staticObjs = logic.getSubfolder(sceneNode, QD_NAMED_OBJECT_STATIC_OBJ);

	CString name;
	name.LoadString(IDS_NEW_BACKGROUND_OBJ_NAME);
	qdGameObjectStatic* obj = addStatic(scene, name, false);

	CString picture = StaticObjPage::SelectPicture(logic.getBaseWnd()->m_hWnd, 
		CString((LPTSTR)IDS_OSD_CAP_SEL_BACK));
	bool res = false;
	if(!picture.IsEmpty())
	{
		qdSprite* psprite = obj->get_sprite();
		ASSERT(psprite);
		psprite->set_file(picture);
		obj->load_resources();

		Vect2s obj_size = obj->screen_size();

		qdCamera* pcam = scene->get_camera();
		pcam->set_scr_size(obj_size.x, obj_size.y);

		obj_size.x = pcam->get_scr_center_x();
		obj_size.y = pcam->get_scr_center_y();

		obj_size = pcam->scr2rscr(obj_size);
		obj->set_pos(pcam->rscr2global(obj_size, 
			scene->get_camera()->get_R()+DEF_BACKGROUND_DEPTH));

		getProject()->getSceneView()->AdjustBitmapSize();
		res = true;
		getProject()->SetModified(true);
	}
	else
		getProject()->DeleteObject(obj, false);
	return res;
}

ITreeNode* ProjectModifier::addInterfaceElement(qdInterfaceElement* element, 
										  qdInterfaceScreen* screen,
										  int defaultDepth)
{
	ITreeNodePtr node = getTreeNodeFactory().createInterfaceObjectNode(element);
	if (!node.get())
		return NULL;
	if (!screen->add_element(element))
		return NULL;

	element->set_owner(screen);
	element->set_screen_depth(getInterfEleDepthOverBack(screen, defaultDepth));

	ITreeNode* screenNode = *getTreeLogic().getObjectNodes(screen);
	assert(screenNode);
	if (!node->load(&getTreeNodeFactory(), getTreeLogic(), 
		screenNode, screen, TN_INSERT_SORT))
	{
		screen->remove_element(element);
		return NULL;
	}
	return node.release();
}

qdInterfaceSlider* ProjectModifier::addInterfaceSlider(CString const& name, 
										  qdInterfaceScreen* pscr)
{
	std::auto_ptr<qdInterfaceSlider> res(
			getGameObjectFactory().createInterfaceSlider(
								get_unique_name(name, pscr->element_list())));
	ITreeNode* node = addInterfaceElement(res.get(), pscr, INTERF_SLIDER_DEPTH);
	if (!node)
		return NULL;
	getTreeLogic().expand(node, TreeLogic::EC_EXPAND);
	getProject()->SetModified(true);
	return res.release();
}

qdInterfaceBackground* ProjectModifier::addInterfaceBackground(CString const& name, 
															qdInterfaceScreen* pscr)
{
	std::auto_ptr<qdInterfaceBackground> res(
		getGameObjectFactory().createInterfaceBackground(
						get_unique_name(name, pscr->element_list())));
	if (!addInterfaceElement(res.get(), pscr, INTERF_BACK_DEPTH))
		return NULL;

	getProject()->SetModified(true);
	return res.release();
}

qdInterfaceScreen*	ProjectModifier::addInterfaceScreen(ITreeNode* parent, 
														CString const& name)
{

	qdInterfaceDispatcher *pd = qdInterfaceDispatcher::get_dispatcher();
	std::auto_ptr<qdInterfaceScreen> ps = 
		getGameObjectFactory().createInterfaceScreen(
						get_unique_name(name, pd->screen_list()));
	if (!ps.get())
		return NULL;

	ITreeNodePtr node = getTreeNodeFactory().createInterfaceObjectNode(ps.get());
	if (!node.get())
		return NULL;
	if (!pd->add_screen(ps.get()))
		return NULL;
	ps->set_owner(pd);
	if (!node->load(&getTreeNodeFactory(), 
					getTreeLogic(), parent, pd, TN_INSERT_SORT))
	{
		pd->remove_screen(ps.get());
		return NULL;
	}
	node.release();
	return ps.release();
}

qdInterfaceTextWindow* ProjectModifier::addInterfaceTextWindow(CString const& name, 
											  qdInterfaceScreen* pscr)
{
	std::auto_ptr<qdInterfaceTextWindow> res(
		getGameObjectFactory().createInterfaceTextWindow(
		get_unique_name(name, pscr->element_list())));
	ITreeNode* node = addInterfaceElement(res.get(), pscr, INTERF_TEXT_WINDOW_DEPTH);
	if (!node)
		return NULL;
	getTreeLogic().expand(node, TreeLogic::EC_EXPAND);
	getProject()->SetModified(true);
	return res.release();
}

qdInterfaceCounter* ProjectModifier::addInterfaceCounter(CString const& name, 
											  qdInterfaceScreen* pscr)
{
	std::auto_ptr<qdInterfaceCounter> res(
		getGameObjectFactory().createInterfaceCounter(
		get_unique_name(name, pscr->element_list())));
	ITreeNode* node = addInterfaceElement(res.get(), pscr, INTERF_COUNTER_DEPTH);
	if (!node)
		return NULL;
	getTreeLogic().expand(node, TreeLogic::EC_EXPAND);
	getProject()->SetModified(true);
	return res.release();
}

qdInterfaceSave* ProjectModifier::addInterfaceSave(CString const&name, 
												   qdInterfaceScreen* pscr)
{
	std::auto_ptr<qdInterfaceSave> res(
			getGameObjectFactory().createInterfaceSave(
								get_unique_name(name, pscr->element_list())));
	ITreeNode* node = addInterfaceElement(res.get(), pscr, INTERF_SAVE_DEPTH);
	if (!node)
		return NULL;
	getTreeLogic().expand(node, TreeLogic::EC_EXPAND);
	getProject()->SetModified(true);
	return res.release();
}

qdInterfaceButton* ProjectModifier::addInterfaceButton(CString const&name, 
													   qdInterfaceScreen* pscr)
{
	std::auto_ptr<qdInterfaceButton> res(
		getGameObjectFactory().createInterfaceButton(
		get_unique_name(name, pscr->element_list())));
	ITreeNode* node = addInterfaceElement(res.get(), pscr, INTERF_BTN_DEPTH);
	if (!node)
		return NULL;
	getTreeLogic().expand(node, TreeLogic::EC_EXPAND);
	getProject()->SetModified(true);
	return res.release();
}

int ProjectModifier::addInterfButtonState(qdInterfaceButton* pb, CString const& name)
{
	qdInterfaceElementState st;
	st.set_name(name);
	
	if (!pb->add_state(st))
		return -1;
	int iState = pb->num_states()-1;

	ITreeNode* buttonNode = *getTreeLogic().getObjectNodes(pb);
	ASSERT(buttonNode);

	ITreeNodePtr node = 
		getTreeNodeFactory().createInterfaceObjectNode(pb->get_state(iState));
	if (!node.get()
		||!node->load(&getTreeNodeFactory(), 
					getTreeLogic(), 
					buttonNode,
					pb, TN_INSERT_SORT))
	{
		pb->erase_state(iState);
		return -1;
	}
	getTreeLogic().expand(node.get(), TreeLogic::EC_EXPAND);
	node.release();
	return iState;
}

qdGameEnd*	ProjectModifier::addGameEnd(ITreeNode* parent, 
					   CString const& name, 
					   CString const& screenName)
{
	qdGameDispatcher* gameDisp = getGameDispatcher();
	std::auto_ptr<qdGameEnd> res = getGameObjectFactory().createGameEnd(
								get_unique_name(name, gameDisp->game_end_list()));
	if (!res.get())
		return NULL;
	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(res.get());
	if (!node.get())
		return NULL;

	res->set_interface_screen(screenName);
	if (!gameDisp->add_game_end(res.get()))
		return NULL;

	if (!node->load(&getTreeNodeFactory(), getTreeLogic(), parent,
		parent->getContent().interfaceObject, TN_INSERT_SORT))
	{
		gameDisp->remove_game_end(res.get());
		return NULL;
	}
	node.release();
	return res.release();
}

qdFontInfo*	ProjectModifier::addFont(ITreeNode* parent)
{
	CString strFileName = getFontFileName();
	if (strFileName.IsEmpty())
		return false;

	CString strName = disk_op::GetFileName(strFileName);
	if (!AskUserForName(strName)) 
		return false;

	qdGameDispatcher* gameDisp = getGameDispatcher();
	//получаем уникальное имя
	strName = get_unique_name(strName, gameDisp->fonts_list());
	std::auto_ptr<qdFontInfo> p(getGameObjectFactory().createFont(strName));
	if (!p.get())
		return false;

	strFileName.Replace(_T(".idx"), _T(".tga"));

	p->set_font_file_name(static_cast<LPCTSTR>(strFileName));
	p->set_type(gameDisp->get_unique_font_info_type());

	if (!gameDisp->add_font_info(p.get()))
		return NULL;

	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(p.get());

	if (!node.get()||!node->load(&getTreeNodeFactory(), getTreeLogic(),
		parent, gameDisp, TN_INSERT_SORT))
	{
		gameDisp->remove_font_info(p.get());
		return NULL;
	}

	getTreeLogic().expand(parent, TreeLogic::EC_EXPAND);
	getProject()->SetModified(true);
	node.release();
	return p.release();
}

qdFontInfo*	ProjectModifier::createFont(ITreeNode* parent)
{
	CString strName;
	strName.LoadString(IDS_NEW_FONT_NAME);
	if (!AskUserForName(strName)) 
		return false;

	qdGameDispatcher* gameDisp = getGameDispatcher();
	//получаем уникальное имя
	strName = get_unique_name(strName, gameDisp->fonts_list());
	std::auto_ptr<qdFontInfo> p(getGameObjectFactory().createFont(strName));
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
	p->set_type(gameDisp->get_unique_font_info_type());

	if (!gameDisp->add_font_info(p.get()))
		return NULL;

	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(p.get());

	if (!node.get()||!node->load(&getTreeNodeFactory(), getTreeLogic(),
		parent, gameDisp, TN_INSERT_SORT))
	{
		gameDisp->remove_font_info(p.get());
		return NULL;
	}

	getTreeLogic().expand(parent, TreeLogic::EC_EXPAND);
	getProject()->SetModified(true);
	node.release();
	return p.release();
}

bool ProjectModifier::addVideos(ITreeNode* parent)
{
	bool res = false;

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
				ShowError(getTreeLogic().getBaseWnd()->m_hWnd, IDS_EM_CNNT_GET_REL_PATH);
				return res;
			}
			GetVideoCurDirSafer().save_current();

			if(addVideo(rel_path, parent))
				res = true;
		}
	}

	return res;
}

/*!
Создает видео объект (qdVideo). Назначает ему имя по имени файла, и добавляет
видео ролик в игровой диспетчер и загружает в дерево
\param strFileName - имя файла видео-ролика.
\param parent - узел, под который будем добавлять видео ролик
\retval !NULL - если удалось добавить видео-ролик
*/
qdVideo* ProjectModifier::addVideo(const CString &strFileName, ITreeNode* parent)
{
	qdGameDispatcher* gameDisp = getGameDispatcher();
	std::auto_ptr<qdVideo> ptrVideo (
		getGameObjectFactory().createVideo(
			get_unique_name(disk_op::GetFileName(strFileName), gameDisp->video_list())));
	if(!ptrVideo.get())
		return false;

	ptrVideo->set_file_name(strFileName);
	if(!gameDisp->add_video(ptrVideo.get()))
		return false;

	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(ptrVideo.get());
	if (!node.get()
		||!node->load(&getTreeNodeFactory(), 
						getTreeLogic(), 
						parent,
						gameDisp, TN_INSERT_SORT))
	{
		gameDisp->remove_video(ptrVideo.get());
		return NULL;
	}
	node.release();
	return ptrVideo.release();
}

qdMiniGame* ProjectModifier::addMinigame(ITreeNode* parent)
{
	const CString strDllName = mgGetDLLName();
	if (strDllName.IsEmpty())
		return false;

	CString strName((LPTSTR)IDS_NEW_MINIGAME_NAME);
	if(!AskUserForName(strName))
		return false;

	std::auto_ptr<qdMiniGame> pmg(getGameObjectFactory().createMiniGame(strName));
	if (!pmg.get())
		return NULL;

	pmg->set_dll_name(strDllName);

	qdGameDispatcher* gameDisp = getGameDispatcher();
	if (!gameDisp->add_minigame(pmg.get()))
		return NULL;


	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(pmg.get());
	if (!node.get()
		||!node->load(&getTreeNodeFactory(), 
					getTreeLogic(),
					parent,
					gameDisp, TN_INSERT_SORT))
	{
		gameDisp->remove_minigame(pmg.get());
		return NULL;
	}

	getTreeLogic().showNode(node.get());
	node.release();
	return pmg.release();
}

qdCounter* ProjectModifier::addCounter(ITreeNode* parent)
{
	CString name;
	name.LoadString(IDS_NEW_COUNTER_NAME);
	if (!AskUserForName(name))
		return NULL;

	qdGameDispatcher* gameDisp = getGameDispatcher();
	name = get_unique_name(name, gameDisp->counter_list());

	std::auto_ptr<qdCounter> ptr(getGameObjectFactory().createCounter(name));
	if (!gameDisp->add_counter(ptr.get()))
		return NULL;

	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(ptr.get());
	if (!node.get()
		||!node->load(&getTreeNodeFactory(), 
					getTreeLogic(),
					parent,
					gameDisp, TN_INSERT_SORT))
	{
		gameDisp->remove_counter(ptr.get());
		return NULL;
	}
	getProject()->ShowProperties(ptr.get());

/*	if (ptr->elements().empty())
	{
		node.release();
		getProject()->DeleteObject(ptr.release(), false);
		return NULL;
	}*/
	getTreeLogic().showNode(node.release());

	return ptr.release();
}

qdInventory* ProjectModifier::addInventory(ITreeNode* parent)
{
	CString strName((LPTSTR)IDS_INVENTORY_DEFAULT_NAME);
	if (!AskUserForName(strName))
		return NULL;

	qdGameDispatcher* gameDisp = getGameDispatcher();
	if (gameDisp->inventory_cell_types().empty())
		if (!addInventoryCellTypes())
			return NULL;


	using GameObjectFactory::eInventoryCrtMode;
	using GameObjectFactory::ICM_USE_EXISTING_CELL_TYPES;
	using GameObjectFactory::ICM_EMPTY;
	eInventoryCrtMode mode = 
		gameDisp->inventory_list().empty() ? ICM_USE_EXISTING_CELL_TYPES : ICM_EMPTY;

	std::auto_ptr<qdInventory> pi = 
		getGameObjectFactory().createInventory(gameDisp, mode);
	if (!pi.get())
		return NULL;

	pi->set_name(get_unique_name(strName, gameDisp->inventory_list()));

	if (!gameDisp->add_inventory(pi.get()))
		return NULL;
	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(pi.get());

	if (!node.get()
		||!node->load(&getTreeNodeFactory(), 
						getTreeLogic(),
						parent,
						gameDisp, TN_INSERT_SORT))
	{
		gameDisp->remove_inventory(pi.get());
		return NULL;
	}

	getTreeLogic().showNode(node.get());

	node.release();
	//если создали первый инв., то назначаем его всем персонажам.
	if (gameDisp->inventory_list().size() == 1)
		getProject()->setInventoryToAllPersonages(pi->name());
	else
	{
		addCellSet(*pi);
		if (pi->cell_sets().empty())
			getProject()->DeleteObject(pi.release(), false);
	}
	return pi.release();
}

bool ProjectModifier::addCellSet(qdInventory& inventory)
{
	return (AddCellSet(getGameDispatcher(), &inventory) != TRUE);
}

/*!
Запрашивает у пользователя имена файлов(StaticObjPage::SelectPicture()), 
хранящих изображения ячеек. Добавляет указанные типы диспетчер. Если
добавление прошло удачно, спрашиваем "Добавлять ли еще"
*/
bool ProjectModifier::addInventoryCellTypes()
{
	CStringVect_t vecFiles;
	CString strOpenDlgTitle((LPTSTR)IDS_OSD_CAP_SEL_CELL_TYPE);
	int nCountAdded = 0;

	qdGameDispatcher* gameDisp = getGameDispatcher();
	bool res = false;
	while (true) {
		vecFiles.clear();
		//выбираем картинку
		CString picture = 
			StaticObjPage::SelectPicture(getTreeLogic().getBaseWnd()->m_hWnd, 
											strOpenDlgTitle, &vecFiles);
		if(picture.IsEmpty())
			break;

		boost::rtl::crange<CStringVect_t const> r(vecFiles);
		for(; r; ++r)
		{
			qdInventoryCellType ct(gameDisp->get_unique_inventory_cell_type());

			ct.set_sprite_file(*r);
			
			if (const qdInventoryCellType* pct = gameDisp->add_inventory_cell_type(ct))
			{
				pct->load_resources();
				++nCountAdded;
			}
		}
		CString strMsg;
		strMsg.LoadString(IDS_CELL_TYPES_ADDED_SUCCESSFULL);
		if (::MessageBox(getTreeLogic().getBaseWnd()->m_hWnd, 
			strMsg, NULL, MB_ICONINFORMATION|MB_YESNO|MB_DEFBUTTON2) == IDNO) 
			break;
	}
	return (nCountAdded != 0);
}


qdTriggerChain* ProjectModifier::addTriggerChain(ITreeNode* parent, CString const& name)
{
	qdGameDispatcher* gameDisp = getGameDispatcher();
	std::auto_ptr<qdTriggerChain> p(
		getGameObjectFactory().createTriggerChain(
					get_unique_name(name, gameDisp->trigger_chain_list())));
	if (!p.get())
		return NULL;

	if(!gameDisp->add_trigger_chain(p.get()))
		return NULL;

	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(p.get());
	if (!node.get()
		||!node->load(&getTreeNodeFactory(), getTreeLogic(), 
					parent, gameDisp, TN_INSERT_SORT))
	{
		gameDisp->remove_trigger_chain(p.get());
		return NULL;
	}
	node.release();
	return p.release();
}

ITreeNode* ProjectModifier::addMusicTrack(qdGameScene* scene, ITreeNode* musicFolder)
{
	CString strFileName = getMusicTrackFileName();
	if (strFileName.IsEmpty())
		return NULL;

	CString name = disk_op::GetFileName(strFileName);
	if (!AskUserForName(name)) 
		return NULL;

	//получаем уникальное имя
	name = get_unique_name(name, scene->music_track_list());
	std::auto_ptr<qdMusicTrack> pmt(getGameObjectFactory().createMusicTrack(name));
	if (!pmt.get())
		return NULL;
	pmt->toggle_cycle(true);
	pmt->set_file_name(strFileName);
	if (!scene->add_music_track(pmt.get()))
		return NULL;

	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(pmt.get());
	if (!node.get()
		||!node->load(&getTreeNodeFactory(), 
					getTreeLogic(), 
					musicFolder,
					scene, TN_INSERT_SORT))
	{
		scene->remove_music_track(pmt.get());
		return NULL;
	}
	pmt.release();
	return node.release();
}

/*!
 	Находит узел сетки в текущей сцене. Запрашивает у пользователя имя объекта
	(bool AskUserForName (CString& strName)), добавляет зону в список зон сцены. 
	Вставляет объект в дерево.
	\param ptrZone - зона
	\return \a true - если добавление удалось
*/
ITreeNode* ProjectModifier::addGridZone(ITreeNode* sceneNode, qdGridZone* zone)
{
	if(zone)
	{
		CString str;
		str.LoadString(IDS_NEW_GRID_ZONE_NAME);
		if(!AskUserForName(str))
			return false;

		ITreeNode* netNode = 
			getTreeLogic().getSubfolder(sceneNode, QD_NAMED_OBJECT_GRID_ZONE);

		qdGameScene* scene = 
			static_cast<qdGameScene*>(sceneNode->getContent().gameObject);

		str = get_unique_name(str, scene->grid_zone_list());

		zone->set_name(str);
		
		if (!scene->add_grid_zone(zone))
			return NULL;

		ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(zone);
		if (!node.get()
			||!node->load(&getTreeNodeFactory(),
					getTreeLogic(),
					netNode,
					sceneNode->getContent().gameObject, TN_INSERT_SORT))
		{
			scene->remove_grid_zone(zone);
			return NULL;
		}
		return node.release();
	}
	return NULL;
}

ITreeNode* ProjectModifier::addMaskState(qdGameObjectAnimated* obj,
										 qdGameObjectStateMask* state)
{
	CString str;
	str.Format(IDS_MASK_NAME, state->parent_name());
	str = AskUserForName((const CString&)str);
	str = get_unique_name(str, obj->state_vector());
	state->set_name(str);
	if (!obj->add_state(state))
		return NULL;

	ITreeNode* objNode = *getTreeLogic().getObjectNodes(obj);
	assert(objNode);
	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(state);
	if (!node.get()||
		!node->load(&getTreeNodeFactory(),
					getTreeLogic(),
					objNode,
					obj, TN_INSERT_SORT))
	{
		obj->remove_state(state);
		return NULL;
	}

	obj->set_default_state();
	return node.release();
}

qdGameObjectAnimated* ProjectModifier::addAnimated(ITreeNode* parent, 
													qdGameObjectAnimated const* source, 
													qdGameScene* ptrScene)
{
	std::auto_ptr<qdGameObjectAnimated> object(
		getGameObjectFactory().createAnimated(source));
	if (!object.get())
		return NULL;

	CString const name = get_unique_name(source->name(), ptrScene->object_list());
	object->set_name(name);

	std::auto_ptr<ITreeNode> node = 
		getTreeNodeFactory().createGameObjectNode(object.get());
	if (!node.get())
		return NULL;

	if (ptrScene->add_object(object.get()))
	{			   

		node->load(&getTreeNodeFactory(), 
			getTreeLogic(), 
			parent, 
			ptrScene, TN_INSERT_SORT);
		node.release();
	}
	else
		return NULL;

	return object.release();
}

//! Создает объект копируя его с исходного
qdGameObjectMoving* ProjectModifier::addPersonage(ITreeNode* parent, 
													qdGameObjectAnimated const* source, 
													qdGameScene* ptrScene)
{
	std::auto_ptr<qdGameObjectMoving> object(
		getGameObjectFactory().createPersonage(source));
	if (!object.get())
		return NULL;

	CString const name = get_unique_name(source->name(), ptrScene->object_list());
	object->set_name(name);

	std::auto_ptr<ITreeNode> node = 
		getTreeNodeFactory().createGameObjectNode(object.get());
	if (!node.get())
		return NULL;

	if (ptrScene->add_object(object.get()))
	{			   
		RestoreImageLayout::restore(const_cast<qdGameObjectAnimated*>(source),
									object.get());
		node->load(&getTreeNodeFactory(), 
			getTreeLogic(), 
			parent, 
			ptrScene, TN_INSERT_SORT);
		node.release();
	}
	else
		return NULL;

	return object.release();
}
//! Создает объект копируя его с исходного
qdGameObjectMoving* ProjectModifier::addPersonage(ITreeNode* parent, 
													qdGameObjectMoving const* source, 
													qdGameScene* ptrScene)
{
	std::auto_ptr<qdGameObjectMoving> object(
		getGameObjectFactory().createPersonage(source));
	if (!object.get())
		return NULL;

	CString const name = get_unique_name(source->name(), ptrScene->object_list());
	object->set_name(name);

	std::auto_ptr<ITreeNode> node = 
		getTreeNodeFactory().createGameObjectNode(object.get());
	if (!node.get())
		return NULL;

	if (ptrScene->add_object(object.get()))
	{			   

		node->load(&getTreeNodeFactory(), 
			getTreeLogic(), 
			parent, 
			ptrScene, TN_INSERT_SORT);
		node.release();
	}
	else
		return NULL;

	return object.release();
}

qdGameObjectState* ProjectModifier::moveState(qdGameObjectAnimated* source, 
			   qdGameObjectAnimated* target,
			   qdGameObjectState* movedState, 
			   ITreeNode* targetNode,
			   ITreeNode* insertAfter,
			   int insertBefore)
{

	int movedStateIndex = StateTraits::getStateIndex(source->state_vector(), movedState);

	if (!source->remove_state(movedState))
		return NULL;

	qdGameObjectState * ptrInsState = movedState;


	if(!target->insert_state(insertBefore, ptrInsState))
	{
		if (movedStateIndex > source->state_vector().size()) 
			source->insert_state(movedStateIndex, ptrInsState);
		else
			source->add_state(ptrInsState);

		return NULL;
	}

	bool restoreGlobalFlag = false;
	if(StateTraits::hasGlobalFlag(ptrInsState))
	{
		restoreGlobalFlag = true;
		ptrInsState->drop_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_GLOBAL_OWNER);
	}

	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(ptrInsState);
	if (!node.get()
		||!node->load(&getTreeNodeFactory(),
		getTreeLogic(), targetNode, target, insertAfter))
	{
		if (restoreGlobalFlag)
			ptrInsState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_GLOBAL_OWNER);

		if (movedStateIndex > source->state_vector().size()) 
			source->insert_state(movedStateIndex, ptrInsState);
		else
			source->add_state(ptrInsState);
		target->remove_state(ptrInsState);
		return NULL;
	}

//	if (ObjectTraits::is_obj_moving(source))
//		RestoreImageLayout::restore(movedState, target, ptrInsState);

	node.release();
	return ptrInsState;
}

qdGameObjectState* ProjectModifier::copyState(qdGameObjectAnimated* source, 
			   qdGameObjectAnimated* target,
			   qdGameObjectState* copyState, 
			   ITreeNode* targetNode,
			   ITreeNode* insertAfter,
			   int insertBefore)
{
	std::auto_ptr<qdGameObjectState> ptrInsState(copyState->clone());

	if (!ptrInsState.get()) 
		return NULL;

	if(StateTraits::hasGlobalFlag(ptrInsState.get()))
		ptrInsState->drop_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_GLOBAL_OWNER);

//	//если не переносим внутри одного и того же объекта, 
//	//то проверяем уникальность имени
	CString strName = get_unique_name(copyState->name(),target->state_vector());
	ptrInsState->set_name(strName);

	if (ObjectTraits::isGlobal(*target)) 
		ptrInsState->coords_animation()->clear();

	if(!target->insert_state(insertBefore, ptrInsState.get()))
		return NULL;

	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(ptrInsState.get());
	if (!node.get()
		||!node->load(&getTreeNodeFactory(),
						getTreeLogic(), targetNode, target, insertAfter))
	{
		target->remove_state(ptrInsState.get());
		return NULL;
	}

	if (ObjectTraits::is_obj_moving(target))
		RestoreImageLayout::restore(copyState, target, ptrInsState.get());

	node.release();
	return ptrInsState.release();
}

qdGridZone* ProjectModifier::moveZone(qdGameScene* scene, qdGridZone* moved, 
					 qdGridZone const* zoneBefore, 
					 ITreeNode* insertAfter, 
					 ITreeNode* parent)
{
	if (!scene->remove_grid_zone(moved))
		return NULL;


	scene->insert_grid_zone(moved, zoneBefore);

	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(moved);
	if (!node.get()
		||!node->load(&getTreeNodeFactory(), 
						getTreeLogic(), 
						parent, 
						scene, 
						insertAfter))
		return NULL;
	node.release();
	return moved;
}

qdVideo* ProjectModifier::moveVideo(qdVideo* source, 
				   qdVideo const* before, 
				   ITreeNode* insertAfter,
				   ITreeNode* parent)
{
	qdGameDispatcher* gameDisp = getGameDispatcher();
	if (gameDisp->is_video_playing(source)) 
	{
		gameDisp->stop_video();
		gameDisp->close_video();
	}

	if (!gameDisp->remove_video(source))
		return NULL;


	gameDisp->add_video(source, before);

	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(source);
	if (!node.get()
		||!node->load(&getTreeNodeFactory(), 
						getTreeLogic(), 
						parent, 
						gameDisp, 
						insertAfter))
		return NULL;
	node.release();
	return source;
}

qdInventory* ProjectModifier::copyInventory(qdInventory const* source, ITreeNode* parent)
{
	std::auto_ptr<qdInventory> object = getGameObjectFactory().createInventory(source);
	if (!object.get())
		return NULL;

	qdGameDispatcher* gameDisp = getGameDispatcher();

	object->set_name(get_unique_name(source->name(), gameDisp->inventory_list()));

	if (!gameDisp->add_inventory(object.get()))
		return NULL;
	ITreeNodePtr node = getTreeNodeFactory().createGameObjectNode(object.get());
	if (!node.get()
		||!node->load(&getTreeNodeFactory(), getTreeLogic(), parent, gameDisp))
	{
		gameDisp->remove_inventory(object.get());
		return NULL;
	}
	node.release();
	return object.release();
}

