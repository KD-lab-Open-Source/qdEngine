/********************************************************************
	created:	2002/08/15
	created:	15:8:2002   13:39
	filename: 	D:\Projects\Quest\QuestEditor\ptree_helpers.h
	file path:	D:\Projects\Quest\QuestEditor
	file base:	ptree_helpers
	file ext:	h
	author:		Илюха
	
	purpose:	Набор глобальных функций для работы с деровом
*********************************************************************/
#ifndef __PTREE_API_H__
#define __PTREE_API_H__

#include "Main/IProjectFacade.h"

class SceneTreeView;
class qdInterfaceScreen;
class qdInterfaceBackground;
class qdInterfaceButton;
class qdInterfaceSlider;
class qdInterfaceSave;
class qdInterfaceElementState;
class qdInterfaceTextWindow;
class qdGameEnd;

class qdGameObjectStateStatic;
class qdGameObjectStateWalk;
class qdGameObjectState;

class qdGameObjectAnimated;
class qdGameObjectMoving;
class qdGameObjectStatic;

class qdGameScene;
class qdGameDispatcher;

interface ITreeNode;
class CTreeListItem;
namespace ptree_api
{
//! Добавляет анимированному объекту обычное состояние 
qdGameObjectStateStatic* AddEmptyStaticState(qdGameObjectAnimated* poa);
//! Вставляет обычное состояние перед указанным
qdGameObjectStateStatic* InsertEmptyStaticState(qdGameObjectAnimated* poa, int iBefore);

//! Добавляет персонажу походочное состояние 
qdGameObjectStateWalk* AddEmptyMovementState(qdGameObjectMoving* pom);
//! Вставляет походочное состояние перед указанным
qdGameObjectStateWalk* InsertEmptyMovementState(qdGameObjectMoving* pom, int iBefore);

//! Добавляет в сцену и в дерево анимированный объект
qdGameObjectAnimated* AddAnimated(qdGameScene* ptrScene, CString& strName, bool bShow);
//! Добавляет в сцену и в дерево персонаж
qdGameObjectMoving* AddPersonage(qdGameScene* ptrScene, CString &strName, bool bShow);
//! Добавляет в проект и в дерево глобальный персонаж
qdGameObjectMoving* AddPersonage(qdGameDispatcher* ptrDisp,CString &strName, bool bShow);
//! Добавляет в сцену и в дерево статический объект
qdGameObjectStatic* AddStatic(qdGameScene* ptrScene, CString& strName, bool bShowInTree);
//! Добавляет в проект и в дерево сцену
bool AddScene(CString &strName, bool bShowInTree);

//! Удаляет объект из дерева и из проекта
bool DeleteObject(class qdNamedObjectBase* ptrObj, bool bAsk);


//! Пробегает по сценам и обновляет в дереве списки объектов
void UpdateCatalogsInScenes(qdNamedObjectType type);
//! Обновляет каталог анимированных объектов в указанной сцене
void UpdateObjCatalogInScene(qdNamedObjectType obj_type,qdGameScene const* pscene);


//! Удаляет состояние
bool RemoveState(qdGameObjectState* ptrObj, bool bAsk);
//! Перезагружает состояния объекта в дерево
void ReloadStates(qdGameObjectAnimated* pobj);

#ifndef USE_NEW_PROJ
//! Указатель на класс менеджер дерева
SceneTreeView* GetProjTree();
#endif // USE_NEW_PROJ
//! Показывает меню для объекта
void ShowCorrespondingMenu(qdNamedObjectBase* pno);

/************************************************************************/
/* Набор для редактора триггеров                                        */
/************************************************************************/
//! Вызывается на удаление объекта из триггреа. Сейчас обнволяет изображение в дереве
void ptOnRemoveObjFromTrigger(qdNamedObject* pno);
//! После добавления объекта в триггер, обновляем его изображение в дереве
void OnAddObjToTrigger(qdNamedObject* pno);
//! Показать диалог условий для объекта
bool ShowCondDlg(qdNamedObject* pobj);
//! Показать диалог свойств для объекта
bool ShowProperties(qdNamedObjectBase* pno);
/************************************************************************/
/* Стыковка и координатная анимация                                     */
/************************************************************************/
//! Режимы окончания редактирования состояния
//enum eEndEditStateMode {
//	EESTM_CANCEL,///< Закончить без сохранения данных
//	EESTM_OK///< Закончить с сохранением данных
//};
using ::eEndEditStateMode;
using ::EESTM_CANCEL;
using ::EESTM_OK;

//! Прекратить стыковку состояния
void EndStateStack(eEndEditStateMode mode);
//! Прекратить редактирование КА
bool EndEditCoordAnimationMode(eEndEditStateMode mode);
//! Перекратить редактирование размеров
void EndResizeState(eEndEditStateMode mode);

//! Удалить набор ячеек из инвентори
bool DeleteInventoryCellset(class qdInventory* pinv, int icellset);
	
//! Режимы запуска игры
//enum eRunMode{
//		RM_NORMAL,		///< запускаем текущую сцену
//		RM_TRIGGER_TEST, ///< запускаем сцену для тестинга триггеров
//		RM_RUN_SCRIPT,	///< запускаем игру, как будьто бы она была запущена игроком
//};
using IProjectFacade::eRunMode;
using IProjectFacade::RM_NORMAL;
using IProjectFacade::RM_TRIGGER_TEST;
using IProjectFacade::RM_RUN_SCRIPT;

//! Запустить сцену
void RunScene(const CString& strSceneName, eRunMode emode);

//! Возвращает диспетчер проекта
class qdGameDispatcher* GetProjGD();

//! Добавить интерфейсный фон
qdInterfaceBackground* AddInterfBackgrnd(const CString& name, qdInterfaceScreen* pscr);
//! Добавить интерфейсный бегунок
qdInterfaceSlider* AddInterfSldr(const CString& name, qdInterfaceScreen* pscr);
//! Добавить окно текстов
qdInterfaceTextWindow* AddInterfTextWindow(const CString& name, qdInterfaceScreen* pscr);
//! Добавить интерфейсную кнопку
qdInterfaceSave* AddInterfSave(const CString& name, qdInterfaceScreen* pscr);
//! Добавить состояние интерф. кнопке
int AddInterfBtnState(const CString& name, qdInterfaceButton* pbtn);
//! Добавить интерфейсную кнопку
qdInterfaceButton* AddInterfBtn(const CString& name, qdInterfaceScreen* pscr);
//! Добавить конечный экран
#ifdef USE_NEW_PROJ
qdGameEnd* AddGameEnd(ITreeNode*, CString const&, CString const&);
#else
qdGameEnd* AddGameEnd(HTREEITEM, CString const&, CString const&);
#endif // USE_NEW_PROJ

//! Найти и выделить объект в дереве
bool ShowObjectInTree(const qdNamedObjectBase* p);

//! Загрузить сцену в проект
#ifdef USE_NEW_PROJ
__declspec(deprecated) CTreeListItem* LoadScene(qdGameScene* pgs);
#else
HTREEITEM LoadScene(qdGameScene* pgs);
#endif // USE_NEW_PROJ
}//namespace ptree_api

#endif //__PTREE_API_H__