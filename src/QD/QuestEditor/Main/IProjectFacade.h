#pragma once

class qdInterfaceScreen;
class qdInterfaceBackground;
class qdInterfaceButton;
class qdInterfaceSlider;
class qdInterfaceSave;
class qdInterfaceCounter;
class qdInterfaceElementState;
class qdInterfaceTextWindow;
class qdGameEnd;

class qdGameObjectStateStatic;
class qdGameObjectStateWalk;
class qdGameObjectState;
class qdGameObjectStateMask;

class qdGameObjectAnimated;
class qdGameObjectMoving;
class qdGameObjectStatic;

class qdGameScene;
class qdGameDispatcher;

class qdVideo;
class qdGridZone;

class CTreeListItem;
interface ITreeNode;

//! Режимы окончания редактирования состояния
enum eEndEditStateMode {
	EESTM_CANCEL,///< Закончить без сохранения данных
	EESTM_OK///< Закончить с сохранением данных
};

interface IProjectFacade
{
	virtual qdGameDispatcher* getGameDispatcher() const = 0;

	//! Добавляет анимированному объекту обычное состояние 
	virtual qdGameObjectStateStatic* AddEmptyStaticState(qdGameObjectAnimated* poa) = 0;
	//! Вставляет обычное состояние перед указанным
	virtual qdGameObjectStateStatic* InsertEmptyStaticState(qdGameObjectAnimated* poa, 
										int iBefore) = 0;

	//! Добавляет персонажу походочное состояние 
	virtual qdGameObjectStateWalk* AddEmptyMovementState(qdGameObjectMoving* pom) = 0;
	//! Вставляет походочное состояние перед указанным
	virtual qdGameObjectStateWalk* InsertEmptyMovementState(qdGameObjectMoving* pom, 
																int iBefore) = 0;

	//! Добавляет в сцену и в дерево анимированный объект
	virtual qdGameObjectAnimated* AddAnimated(qdGameScene* ptrScene, 
									CString const& strName, bool bShow) = 0;

	//! Добавляет в сцену и в дерево персонаж
	virtual qdGameObjectMoving* AddPersonage(qdGameScene* ptrScene, 
											CString &strName, bool bShow) = 0;
	//! Добавляет в проект и в дерево глобальный персонаж
	virtual qdGameObjectMoving* AddPersonage(qdGameDispatcher* ptrDisp,
											CString &strName, bool bShow) = 0;
	//! Добавляет в сцену и в дерево статический объект
	virtual qdGameObjectStatic* AddStatic(qdGameScene* ptrScene, 
											CString& strName, bool bShowInTree) = 0;
	//! Добавляет в проект и в дерево сцену
	virtual qdGameScene* AddScene(CString &strName, bool bShowInTree) = 0;

	//! Удаляет объект из дерева и из проекта
	virtual bool DeleteObject(class qdNamedObjectBase* ptrObj, bool bAsk) = 0;

	//! Пробегает по сценам и обновляет в дереве списки объектов
	virtual void UpdateCatalogsInScenes(qdNamedObjectType type) = 0;

	//! Обновляет каталог анимированных объектов в указанной сцене
	virtual void UpdateObjCatalogInScene(qdNamedObjectType obj_type,
										qdGameScene const* pscene) = 0;


	//! Удаляет состояние
	virtual bool RemoveState(qdGameObjectState* ptrObj, bool bAsk) = 0;
	//! Перезагружает состояния объекта в дерево
	virtual void ReloadStates(qdGameObjectAnimated* pobj) = 0;

	//! Показывает меню для объекта
	virtual void ShowCorrespondingMenu(qdNamedObjectBase* pno) = 0;

	/************************************************************************/
	/* Набор для редактора триггеров                                        */
	/************************************************************************/
	//! Вызывается на удаление объекта из триггреа. Сейчас обнволяет изображение в дереве
	virtual void ptOnRemoveObjFromTrigger(qdNamedObject* pno) = 0;
	//! После добавления объекта в триггер, обновляем его изображение в дереве
	virtual void OnAddObjToTrigger(qdNamedObject* pno) = 0;
	//! Показать диалог условий для объекта
	virtual bool ShowCondDlg(qdNamedObject* pobj) = 0;
	//! Показать диалог свойств для объекта
	virtual bool ShowProperties(qdNamedObjectBase* pno) = 0;
	/************************************************************************/
	/* Стыковка и координатная анимация                                     */
	/************************************************************************/

	//! Прекратить стыковку состояния
	virtual void EndStateStack(eEndEditStateMode mode) = 0;
	//! Прекратить редактирование КА
	virtual bool EndEditCoordAnimation(eEndEditStateMode mode) = 0;
	//! Перекратить редактирование размеров
	virtual void EndResizeState(eEndEditStateMode mode) = 0;

	//! Удалить набор ячеек из инвентори
	virtual bool DeleteInventoryCellset(class qdInventory* pinv, int icellset) = 0;

	//! Режимы запуска игры
	enum eRunMode{
		RM_NORMAL,		///< запускаем текущую сцену
		RM_TRIGGER_TEST, ///< запускаем сцену для тестинга триггеров
		RM_RUN_SCRIPT,	///< запускаем игру, как будьто бы она была запущена игроком
	};
	//! Запустить сцену
	virtual void RunScene(const CString& strSceneName, eRunMode emode) = 0;

	//! Добавить интерфейсный фон
	virtual qdInterfaceBackground* AddInterfBackgrnd(const CString& name, 
										qdInterfaceScreen* pscr) = 0;
	//! Добавить интерфейсный бегунок
	virtual qdInterfaceSlider* AddInterfSldr(const CString& name, 
										qdInterfaceScreen* pscr) = 0;
	//! Добавить окно текстов
	virtual qdInterfaceTextWindow* AddInterfTextWindow(const CString& name, 
										qdInterfaceScreen* pscr) = 0;
	//! Добавить интерфейсный счётчик
	virtual qdInterfaceCounter* AddInterfCounter(const CString& name, 
										qdInterfaceScreen* pscr) = 0;
	//! Добавить интерфейсную кнопку
	virtual qdInterfaceSave* AddInterfSave(const CString& name, 
										qdInterfaceScreen* pscr) = 0;
	//! Добавить состояние интерф. кнопке
	virtual int AddInterfBtnState(const CString& name, qdInterfaceButton* pbtn) = 0;
	//! Добавить интерфейсную кнопку
	virtual qdInterfaceButton* AddInterfBtn(const CString& name, 
										qdInterfaceScreen* pscr) = 0;
	//! Добавить конечный экран
	virtual qdGameEnd* AddGameEnd(ITreeNode* parent, CString const&, CString const&) = 0;

	//! Найти и выделить объект в дереве
	virtual bool ShowObjectInTree(const qdNamedObjectBase* p) = 0;

	//! Загрузить сцену в проект
	virtual CTreeListItem* LoadScene(qdGameScene* pgs) = 0;

	//! Сообщает является ли активным редактор сцены
	virtual bool isSceneViewActive() const = 0;
	/// Делает редактор сцен активным/неактивным
	virtual void setSceneViewActive(bool bActive) = 0;
	/// Выставляет признак модификации проекта
	virtual void SetModified(bool bModified = true) = 0;
	/// Выбирает указанную сцену
	virtual bool SelectScene(qdNamedObject* scene) = 0;
	/// Свертывает узлы всех сцен за исключением указанной
	virtual void CollapseSceneNodes(qdNamedObject const* exclude) = 0;
	/// Выставляет иконку вхождения в активную цепочку триггеров,
	/// для всех объектов входящих в qdTriggerChain
	virtual void setActiveTriggerIcon(qdTriggerChain const* chain, BOOL bSet) = 0;
	/// Выбирает указанный интерфейсный экран
	virtual bool selectInterfaceScreen(LPCTSTR screenName) = 0;

	/// Вызывается на момент окончания создания сцены
	virtual bool OnCompleteCreateZone(qdGridZone* zone) = 0;
	/// Вызывается на момент окончания создания маски
	virtual bool OnEndStateMaskCreate(qdGameObjectStateMask* state) = 0;
};