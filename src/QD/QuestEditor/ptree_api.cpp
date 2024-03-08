/********************************************************************
	created:	2002/09/06
	created:	6:9:2002   10:51
	filename: 	D:\Projects\Quest\QuestEditor\ptree_helpers.cpp
	file path:	D:\Projects\Quest\QuestEditor
	file base:	ptree_helpers
	file ext:	cpp
	Powered by:		Илюха
	
	purpose:	Вспомогательные для дерева сюда
*********************************************************************/
#include "stdafx.h"
#include "questeditor.h"

#include "qd_game_object_animated.h"
#include "qd_game_object_moving.h"
#include "qd_game_object_state.h"
#include "qd_game_dispatcher.h"
#include "qd_grid_zone.h"

#include "qd_music_track.h"
#include "qd_minigame.h"
#include "qd_video.h"
#include "qd_inventory.h"

#include "SceneTreeView.h"

#ifdef USE_NEW_PROJ
#include "Main/IProjectFacade.h"
#endif // USE_NEW_PROJ
/************************************************************************/
/* Definitions                                                          */
/************************************************************************/
#include "ptree_api.h"
#include "ptree_hlprs.hpp"
#include "ptree_loading.h"

bool ShowCondDlg(qdGameObjectState* ptrState, class qdGameDispatcher* ptrDisp);
bool CO_ShowCondDlg(qdGameDispatcher* ptrDisp, qdConditionalObject* ptrGame);

#ifdef USE_NEW_PROJ
IProjectFacade* getProject();
#endif // USE_NEW_PROJ

namespace ptree_api
{
/************************************************************************/
/* Внешний интерефейс                                                   */
/************************************************************************/
//! Добавляет анимированному объекту обычное состояние 
qdGameObjectStateStatic* AddEmptyStaticState(qdGameObjectAnimated* poa)
{
#ifdef USE_NEW_PROJ
	return getProject()->AddEmptyStaticState(poa);
#else
	std::auto_ptr<qdGameObjectStateStatic>ptrState(new qdGameObjectStateStatic);
	if(!ptrState.get())
		return NULL;
	
	if(poa->add_state(ptrState.get()))
	{
		return ptrState.release();
	}
	
	return NULL;
#endif // USE_NEW_PROJ
}

//! Вставляет обычное состояние перед указанным
qdGameObjectStateStatic* InsertEmptyStaticState(qdGameObjectAnimated* poa, 
												int iBefore)
{
#ifdef USE_NEW_PROJ
	return getProject()->InsertEmptyStaticState(poa, iBefore);
#else
	std::auto_ptr<qdGameObjectStateStatic> ptrState (new qdGameObjectStateStatic);
	if(!ptrState.get())
		return NULL;
	
	if(poa->insert_state(iBefore, ptrState.get()))
	{
		return ptrState.release();
	}
	
	return NULL;
#endif // USE_NEW_PROJ
}

//! Добавляет персонажу походочное состояние 
qdGameObjectStateWalk* AddEmptyMovementState(qdGameObjectMoving* pom)
{
#ifdef USE_NEW_PROJ
	return getProject()->AddEmptyMovementState(pom);
#else
	std::auto_ptr<qdGameObjectStateWalk> ptrState (new qdGameObjectStateWalk);
	if(!ptrState.get())
		return NULL;
	
	if(pom->add_state(ptrState.get()))
		return ptrState.release();

	return NULL;
#endif // USE_NEW_PROJ
}

//! Вставляет походочное состояние перед указанным
qdGameObjectStateWalk* InsertEmptyMovementState(qdGameObjectMoving* pom, 
												int iBef)
{
#ifdef USE_NEW_PROJ
	return getProject()->InsertEmptyMovementState(pom, iBef);
#else
	std::auto_ptr<qdGameObjectStateWalk> ptrState (new qdGameObjectStateWalk);
	if(!ptrState.get())
		return NULL;

	if(pom->insert_state(iBef, ptrState.get()))
	{
		return ptrState.release();
	}
	return NULL;
#endif // USE_NEW_PROJ
}

//! Добавляет в сцену и в дерево анимированный объект
qdGameObjectAnimated* AddAnimated(qdGameScene* ptrScene, CString& strName, 
								  bool bShowInTree)
{
#ifdef USE_NEW_PROJ
	return getProject()->AddAnimated(ptrScene, strName, bShowInTree);
#else
	return GetProjTree()->AddAnimated(ptrScene, strName, bShowInTree);
#endif // USE_NEW_PROJ
}

//! Добавляет в сцену и в дерево персонаж
qdGameObjectMoving* AddPersonage(qdGameScene* ptrScene, CString &strName, 
								 bool bShowInTree)
{
#ifdef USE_NEW_PROJ
	return getProject()->AddPersonage(ptrScene, strName, bShowInTree);
#else
	return GetProjTree()->AddPersonage(ptrScene, strName, bShowInTree, true);
#endif // USE_NEW_PROJ
}

//! Добавляет в проект и в дерево глобальный персонаж
qdGameObjectMoving* AddPersonage(qdGameDispatcher* ptrDisp, CString &strName, 
								 bool bShowInTree)
{
#ifdef USE_NEW_PROJ
	return getProject()->AddPersonage(ptrDisp, strName, bShowInTree);
#else
	return GetProjTree()->AddGlobalPersonage(ptrDisp, strName, bShowInTree);
#endif // USE_NEW_PROJ
}
//! Добавляет в сцену и в дерево статический объект
qdGameObjectStatic* AddStatic(qdGameScene* ptrScene, CString& strName, 
							  bool bShowInTree)
{
#ifdef USE_NEW_PROJ
	return getProject()->AddStatic(ptrScene, strName, bShowInTree);
#else
	return GetProjTree()->AddStatic(ptrScene, strName, bShowInTree);
#endif // USE_NEW_PROJ
}
//! Добавляет в проект и в дерево сцену
bool AddScene(CString &strName, bool bShowInTree)
{
#ifdef USE_NEW_PROJ
	return getProject()->AddScene(strName, bShowInTree);
#else
	return GetProjTree()->AddScene(strName, bShowInTree);
#endif
}

//! Удаляет объект из дерева и из проекта
bool DeleteObject(qdNamedObjectBase* ptrObj, bool bAsk)
{
#ifdef USE_NEW_PROJ
	return getProject()->DeleteObject(ptrObj, bAsk);
#else
	return GetProjTree()->DeleteObject(ptrObj, bAsk);
#endif // USE_NEW_PROJ
}

//! Пробегает по сценам и обновляет в дереве списки объектов
void UpdateCatalogsInScenes(qdNamedObjectType type)
{
#ifdef USE_NEW_PROJ
	getProject()->UpdateCatalogsInScenes(type);
#else
	GetProjTree()->UpdateCatalogsInScenes(type);
#endif // USE_NEW_PROJ
}

//! Обновляет каталог анимированных объектов в указанной сцене
void UpdateObjCatalogInScene(qdNamedObjectType obj_type,qdGameScene const* pscene)
{
#ifdef USE_NEW_PROJ
	getProject()->UpdateObjCatalogInScene(obj_type, pscene);
#else
	GetProjTree()->UpdateObjCatalogInScene(obj_type, pscene);
#endif // USE_NEW_PROJ
}

//! Удаляет состояние
bool RemoveState(qdGameObjectState* ptrObj, bool bAsk)
{
#ifdef USE_NEW_PROJ
	return getProject()->RemoveState(ptrObj, bAsk);
#else
	return GetProjTree()->RemoveState(ptrObj, bAsk);
#endif // USE_NEW_PROJ
}

//! Перезагружает состояния объекта в дерево
void ReloadStates(qdGameObjectAnimated* pobj)
{
#ifdef USE_NEW_PROJ
	getProject()->ReloadStates(pobj);
#else
	ptree_loading::ReloadStates(GetProjTree()->GetTreeCtrl(),
				GetProjTree()->FindObjectItem(pobj), pobj);
#endif // USE_NEW_PROJ
}

//! Показывает меню для объекта
void ShowCorrespondingMenu(qdNamedObjectBase* pno)
{
#ifdef USE_NEW_PROJ
	getProject()->ShowCorrespondingMenu(pno);
#else
	GetProjTree()->ShowCorrespondingMenu(pno);
#endif // USE_NEW_PROJ
}

/************************************************************************/
/* Набор для редактора триггеров                                        */
/************************************************************************/
//! Вызывается на удаление объекта из триггреа. Сейчас обнволяет изображение в дереве
void ptOnRemoveObjFromTrigger(qdNamedObject* pno)
{
#ifdef USE_NEW_PROJ
	getProject()->ptOnRemoveObjFromTrigger(pno);
#else
	GetProjTree()->OnRemoveObjFromTrigger(pno);
#endif // USE_NEW_PROJ
}

//! После добавления объекта в триггер, обновляем его изображение в дереве
void OnAddObjToTrigger(qdNamedObject* pno)
{
#ifdef USE_NEW_PROJ
	getProject()->OnAddObjToTrigger(pno);
#else
	GetProjTree()->OnAddObjToTrigger(pno);
#endif // USE_NEW_PROJ
}

//! Показать диалог условий для объекта
bool ShowCondDlg(qdNamedObject* pobj)
{
#ifdef USE_NEW_PROJ
	return getProject()->ShowCondDlg(pobj);
#else
	switch(pobj->named_object_type()) {
	case QD_NAMED_OBJECT_OBJ_STATE:
		return ::ShowCondDlg(static_cast<qdGameObjectState*>(pobj), 
			GetProjTree()->GetGameDispatcher());
	default:
		return ::CO_ShowCondDlg(GetProjGD(), dynamic_cast<qdConditionalObject*>(pobj));
	}  
#endif // USE_NEW_PROJ
}

//! Показать диалог свойств для объекта
bool ShowProperties(qdNamedObjectBase* pno)
{
#ifdef USE_NEW_PROJ
	return getProject()->ShowProperties(pno);
#else
	return GetProjTree()->ShowProperties(pno, 0);
#endif // USE_NEW_PROJ
}

//! Прекратить стыковку состояния
void EndStateStack(eEndEditStateMode mode)
{
#ifdef USE_NEW_PROJ
	getProject()->EndStateStack(mode);
#else
	GetProjTree()->EndStateStack(mode == EESTM_CANCEL);
#endif // USE_NEW_PROJ
}

//! Прекратить редактирование КА
bool EndEditCoordAnimationMode(eEndEditStateMode mode)
{
#ifdef USE_NEW_PROJ
	return getProject()->EndEditCoordAnimation(mode);
#else
	return GetProjTree()->EndEditCoordAnimationMode(mode == EESTM_CANCEL);
#endif // USE_NEW_PROJ
}

//! Перекратить редактирование размеров
void EndResizeState(eEndEditStateMode mode){
#ifdef USE_NEW_PROJ
	getProject()->EndResizeState(mode);
#else
	GetProjTree()->EndEditStateDims(mode == EESTM_OK);
#endif // USE_NEW_PROJ
}

//! Удалить набор ячеек из инвентори
bool DeleteInventoryCellset(qdInventory* pinv, int icellset){
#ifdef USE_NEW_PROJ
	return getProject()->DeleteInventoryCellset(pinv, icellset);
#else
	return GetProjTree()->DeleteInventoryCellset(pinv, icellset);
#endif // USE_NEW_PROJ
}

//! Запустить сцену
void RunScene(const CString& strSceneName, eRunMode emode)
{
#ifdef USE_NEW_PROJ
	getProject()->RunScene(strSceneName, emode);
#else
	return GetProjTree()->RunScene(strSceneName, emode);
#endif // USE_NEW_PROJ
}

#ifndef USE_NEW_PROJ
//! Возвращает диспетчер проекта
qdGameDispatcher* GetProjGD(){
	return GetProjTree()->GetGameDispatcher();
};
#endif // USE_NEW_PROJ

//! Добавить интерфейсный фон
qdInterfaceBackground* AddInterfBackgrnd(const CString& name, qdInterfaceScreen* pscr)
{
#ifdef USE_NEW_PROJ
	return getProject()->AddInterfBackgrnd(name, pscr);
#else
	return GetProjTree()->AddInterfBack(pscr, NULL, name);
#endif // USE_NEW_PROJ
}

//! Добавить интерфейсный бегунок
qdInterfaceSlider* AddInterfSldr(const CString& name, qdInterfaceScreen* pscr)
{
#ifdef USE_NEW_PROJ
	return getProject()->AddInterfSldr(name, pscr);
#else
	return GetProjTree()->AddInterfSldr(pscr, NULL, name);
#endif // USE_NEW_PROJ
}

//! Добавить окно текстов
qdInterfaceTextWindow* AddInterfTextWindow(const CString& name, qdInterfaceScreen* pscr)
{
#ifdef USE_NEW_PROJ
	return getProject()->AddInterfTextWindow(name, pscr);
#else
	return GetProjTree()->AddInterfTextWindow(pscr, NULL, name);
#endif // USE_NEW_PROJ
}

//! Добавить интерфейсную кнопку
qdInterfaceButton* AddInterfBtn(const CString& name, qdInterfaceScreen* pscr)
{
#ifdef USE_NEW_PROJ
	return getProject()->AddInterfBtn(name, pscr);
#else
	return GetProjTree()->AddInterfBtn(pscr, NULL, name);
#endif // USE_NEW_PROJ
}

//! Добавить состояние интерф. кнопке
int AddInterfBtnState(const CString& name, qdInterfaceButton* pbtn){
#ifdef USE_NEW_PROJ
	return getProject()->AddInterfBtnState(name, pbtn);
#else
	return GetProjTree()->AddInterfBtnState(pbtn, NULL, name);
#endif // USE_NEW_PROJ
}

//! Добавить интерф. сейв
qdInterfaceSave* AddInterfSave(const CString& name, qdInterfaceScreen* pscr)
{
#ifdef USE_NEW_PROJ
	return getProject()->AddInterfSave(name, pscr);
#else
	return GetProjTree()->AddInterfSave(pscr, NULL, name);
#endif // USE_NEW_PROJ
}

//! Найти и выделить объект в дереве
bool ShowObjectInTree(const qdNamedObjectBase* p)
{
#ifdef USE_NEW_PROJ
	return getProject()->ShowObjectInTree(p);
#else
	return GetProjTree()->ShowObjectInTree(p);
#endif // USE_NEW_PROJ
}

//! Загрузить сцену в проект
#ifdef USE_NEW_PROJ
CTreeListItem* LoadScene(qdGameScene* pgs)
{
	return getProject()->LoadScene(pgs);
}
#else
HTREEITEM LoadScene(qdGameScene* pgs)
{
	HTREEITEM hParent = GetProjTree()->FindObjectItem(GetProjGD());
	return GetProjTree()->LoadScene(hParent, pgs);
}
#endif // USE_NEW_PROJ

//! Добавить конечный экран
#ifdef USE_NEW_PROJ
qdGameEnd* AddGameEnd(ITreeNode* parent, CString const& name, CString const& scrName)
{
	return getProject()->AddGameEnd(parent, name, scrName);
}
#else
qdGameEnd* AddGameEnd(HTREEITEM hParent, CString const& name, CString const& scrName)
{
	return GetProjTree()->AddGameEnd(hParent, name, scrName);
}
#endif // USE_NEW_PROJ

}//namespace ptree_api