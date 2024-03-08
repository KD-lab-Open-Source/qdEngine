#pragma once

#include <map>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "IViewMgr.h"
#include "Main/IProjectFacade.h"
#include "Main/ViewActivator.h"
#include "ScaleInterfaces.h"
#include "Object List/IObjectList.h"
#include "Stat Dependencies/filterobjectdependencies.h"
#include "IAppSettingsChangeListener.h"
#include "IAppSettingsUser.h"


interface ISceneViewFacade;
interface ITreeNodeFactory;
interface ITreeNode;
interface IScaleMgr;
interface ITriggerView;
interface IRequestingStateEdit;
interface IProjectSettings;

class StructureView;
class qdGameDispatcher;
class TreeLogic;
class TreeNodeFactory;
class GameObjectFactory;
class ProjectModifier;
// Project document

typedef std::list<qdNamedObject const*> NamedObjectList;


/*!
 Основной класс. Отвечает за загрузку/сохранение проекта. Добавление/удаление
 в проект объектов
*/
class Project : public CDocument
				, public IProjectFacade
				, public IViewMgr
				, public IAppSettingsUser
				, public IAppSettingsChangeListener
{
	DECLARE_DYNCREATE(Project)

	friend class ViewActivator;
public:

	//! Возможные типы фильтров элементов в редакторе триггеров
	enum eFilterScheme{
		FS_NONE, ///< Нет фильтрации
		FS_ENTRY_FOR_DYN_OBJ, ///< Выделяются состояния динамических объектов
		FS_ENTRY_FOR_MOUSE, ///< Выделяются состояния мыши
		FS_ENTRY_FOR_PERS, ///< Выделяются состояния персонажей
		FS_ENTRY_FOR_DIALOGS,///< Выделяются диалоговые состояния
	};

	Project();
	virtual ~Project();

	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
	void setSceneView(ISceneViewFacade* sceneView);
	ISceneViewFacade* getSceneView() const;

	void setTriggerEditorView(ITriggerView* teView);
	ITriggerView* getTriggerEditorView() const;

	void setStructureView(StructureView* structureView);
	StructureView* getStructureView() const;
	
	void setScaleMngr(IScaleMgr* mngr);
	IScaleMgr* getScaleMngr() const;

	TreeLogic& getTreeLogic() const;

	qdGameDispatcher* getGameDispatcher() const;

	ITreeNodeFactory& getTreeNodeFactory() const;

	IObjectList* getObjectList() const;
	void setObjectList(IObjectList* ol);

	void OnTimer(UINT nIDEvent);
public:
	//! Добавляет анимированному объекту обычное состояние 
	virtual qdGameObjectStateStatic* AddEmptyStaticState(qdGameObjectAnimated* poa);
	//! Вставляет обычное состояние перед указанным
	virtual qdGameObjectStateStatic* InsertEmptyStaticState(qdGameObjectAnimated* poa, 
		int iBefore);

	//! Добавляет персонажу походочное состояние 
	virtual qdGameObjectStateWalk* AddEmptyMovementState(qdGameObjectMoving* pom);
	//! Вставляет походочное состояние перед указанным
	virtual qdGameObjectStateWalk* InsertEmptyMovementState(qdGameObjectMoving* pom, 
		int iBefore);

	//! Добавляет в сцену и в дерево анимированный объект
	virtual qdGameObjectAnimated* AddAnimated(qdGameScene* ptrScene, 
		CString const& strName, bool bShow);

	//! Добавляет в сцену и в дерево персонаж
	virtual qdGameObjectMoving* AddPersonage(qdGameScene* ptrScene, 
		CString &strName, bool bShow);
	//! Добавляет в проект и в дерево глобальный персонаж
	virtual qdGameObjectMoving* AddPersonage(qdGameDispatcher* ptrDisp,
		CString &strName, bool bShow);
	//! Добавляет в сцену и в дерево статический объект
	virtual qdGameObjectStatic* AddStatic(qdGameScene* ptrScene, 
		CString& strName, bool bShowInTree);
	//! Добавляет в проект и в дерево сцену
	virtual qdGameScene* AddScene(CString &strName, bool bShowInTree);

	//! Удаляет объект из дерева и из проекта
	virtual bool DeleteObject(class qdNamedObjectBase* ptrObj, bool bAsk);

	//! Пробегает по сценам и обновляет в дереве списки объектов
	virtual void UpdateCatalogsInScenes(qdNamedObjectType type);

	//! Обновляет каталог анимированных объектов в указанной сцене
	virtual void UpdateObjCatalogInScene(qdNamedObjectType obj_type,
		qdGameScene const* pscene);


	//! Удаляет состояние
	virtual bool RemoveState(qdGameObjectState* ptrObj, bool bAsk);
	//! Перезагружает состояния объекта в дерево
	virtual void ReloadStates(qdGameObjectAnimated* pobj);

	//! Показывает меню для объекта
	virtual void ShowCorrespondingMenu(qdNamedObjectBase* pno);

	/************************************************************************/
	/* Набор для редактора триггеров                                        */
	/************************************************************************/
	//! Вызывается на удаление объекта из триггреа. Сейчас обнволяет изображение в дереве
	virtual void ptOnRemoveObjFromTrigger(qdNamedObject* pno);
	//! После добавления объекта в триггер, обновляем его изображение в дереве
	virtual void OnAddObjToTrigger(qdNamedObject* pno);
	//! Показать диалог условий для объекта
	virtual bool ShowCondDlg(qdNamedObject* pobj);
	//! Показать диалог свойств для объекта
	virtual bool ShowProperties(qdNamedObjectBase* pno);
	/************************************************************************/
	/* Стыковка и координатная анимация                                     */
	/************************************************************************/
	//! Прекратить стыковку состояния
	virtual void EndStateStack(eEndEditStateMode mode);
	//! Прекратить редактирование КА
	virtual bool EndEditCoordAnimation(eEndEditStateMode mode);
	//! Перекратить редактирование размеров
	virtual void EndResizeState(eEndEditStateMode mode);

	//! Удалить набор ячеек из инвентори
	virtual bool DeleteInventoryCellset(class qdInventory* pinv, int icellset);

	//! Запустить сцену
	virtual void RunScene(const CString& strSceneName, eRunMode emode);

	//! Добавить интерфейсный фон
	virtual qdInterfaceBackground* AddInterfBackgrnd(const CString& name, 
		qdInterfaceScreen* pscr);
	//! Добавить интерфейсный бегунок
	virtual qdInterfaceSlider* AddInterfSldr(const CString& name, 
		qdInterfaceScreen* pscr);
	//! Добавить окно текстов
	virtual qdInterfaceTextWindow* AddInterfTextWindow(const CString& name, 
		qdInterfaceScreen* pscr);
	//! Добавить интерфейсный счётчик
	virtual qdInterfaceCounter* AddInterfCounter(const CString& name, 
		qdInterfaceScreen* pscr);
	//! Добавить интерфейсную кнопку
	virtual qdInterfaceSave* AddInterfSave(const CString& name, 
		qdInterfaceScreen* pscr);
	//! Добавить состояние интерф. кнопке
	virtual int AddInterfBtnState(const CString& name, qdInterfaceButton* pbtn);
	//! Добавить интерфейсную кнопку
	virtual qdInterfaceButton* AddInterfBtn(const CString& name, 
		qdInterfaceScreen* pscr);
	//! Добавить конечный экран
	virtual qdGameEnd* AddGameEnd(ITreeNode* parent, CString const&, CString const&);

	//! Найти и выделить объект в дереве
	virtual bool ShowObjectInTree(const qdNamedObjectBase* p);

	//! Загрузить сцену в проект
	virtual CTreeListItem* LoadScene(qdGameScene* pgs);

	//! Сообщает является ли активным редактор сцены
	virtual bool isSceneViewActive() const;
	virtual void setSceneViewActive(bool bActive);

	virtual void SetModified(bool bModified);

	virtual bool SelectScene(qdNamedObject* scene);
	virtual void CollapseSceneNodes(qdNamedObject const* exclude);
	virtual void setActiveTriggerIcon(qdTriggerChain const* chain, BOOL bSet);
	virtual bool selectInterfaceScreen(LPCTSTR screenName);

	virtual bool OnCompleteCreateZone(qdGridZone* zone);
	virtual bool OnEndStateMaskCreate(qdGameObjectStateMask* state);

	void setInventoryToAllPersonages(LPCTSTR inventoryName);
	LRESULT OnMenuCommand(WPARAM wParam, LPARAM lParam);

	//! Копирует анимированный объект в анимированный объект
	bool copyAnimatedToAnimated(ITreeNode* dropTarget, 
		qdGameObjectAnimated const* source,
		qdGameScene* scene);
	//! Копирует анимированный объект в персонаж
	bool copyAnimatedToMoving(ITreeNode* dropTarget, 
		qdGameObjectAnimated const* source,
		qdGameScene* scene);
	//! Копирует персонаж в персонаж
	bool copyMovingToMoving(ITreeNode* dropTarget, 
		qdGameObjectMoving const* source,
		qdGameScene* scene);

	//! Перенаправляет вызов в ProjectModifier::moveState
	qdGameObjectState* moveState(qdGameObjectAnimated* source, 
		qdGameObjectAnimated* target,
		qdGameObjectState* movedState, 
		ITreeNode* targetNode,
		ITreeNode* insertAfter,
		int insertBefore);

	//! Перенаправляет вызов в ProjectModifier::copyState
	qdGameObjectState* copyState(qdGameObjectAnimated* source, 
		qdGameObjectAnimated* target,
		qdGameObjectState* copyState, 
		ITreeNode* targetNode,
		ITreeNode* insertAfter,
		int insertBefore);

	//! Перенаправляет вызов в ProjectModifier::moveZone
	qdGridZone* moveZone(qdGameScene* scene, 
						qdGridZone* moved, 
						qdGridZone const* zoneBefore, 
						ITreeNode* insertAfter,
						ITreeNode* parent);

	//! Перенаправляет вызов в ProjectModifier::moveVideo
	qdVideo* moveVideo(qdVideo* source, qdVideo const* before, 
					ITreeNode* insertAfter, ITreeNode* parent);
	//! Перенаправляет вызыво в ProjectModifier::copyInventory
	qdInventory* copyInventory(qdInventory const* source, ITreeNode* parent);

	eFilterScheme getTriggerElementFilterScheme() const;
	bool getShowTriggerElement() const;

	//! Возвращает сцену,под которой находится узел
	qdGameScene* getSceneByNode(ITreeNode const* node) const;

	//! Запускает визард нового проекта
	void showNewProjWiz();


	virtual boost::shared_ptr<AppSettingsBasePage> getPage();
	virtual void SettingsChanged();

	void autoSave();
protected:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, 
							void* pExtra, 
							AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnProperies();
	afx_msg void onConditions();
	afx_msg void onEditTriggers();

	afx_msg void OnCrtAnimatedObj();
	afx_msg void OnCrtMovingObj();
	afx_msg void OnCrtStaticObj();
	afx_msg void OnCreateStatics();
	afx_msg void OnWaveToLocal();
	afx_msg void OnLocalToWave();
	afx_msg void OnCrtScene();
	afx_msg void OnAddInterfBackground();
	afx_msg void OnAddInterfBtnState();
	afx_msg void OnAddInterfSave();
	afx_msg void OnCreateIscreen();
	afx_msg void OnAddInterfSlider();
	afx_msg void OnAddInterfTextWindow();
	afx_msg void OnAddInterfCounter();
	afx_msg void OnAddIntrfButton();
	afx_msg void OnAddGameEnd();
	afx_msg void OnAddFont();
	afx_msg void OnCreateFont();
	afx_msg void OnAddVideo();
	afx_msg void OnCreateMinigame();
	afx_msg void OnCrtNewInventory();
	afx_msg void OnCreateCounter();
	afx_msg void OnCreateState();
	afx_msg void OnCrtTrigger();
	afx_msg void OnCrtAnimation();
	afx_msg void OnAnimationList();
	afx_msg void OnSoundList();
	afx_msg void OnEditSound();
	afx_msg void OnEditStateAnimation();
	afx_msg void OnMenuMovingsStops();
	afx_msg void OnGameSettings();
	afx_msg void OnManageCds();
	afx_msg void OnStatDependencies();
	afx_msg void OnAddCellSet();
	afx_msg void OnRemoveCellSet();
	afx_msg void OnAddMusicTrack();
	afx_msg void OnCreateMask();
	afx_msg void OnCrtGridZone();
	afx_msg void OnMusicTrackCycled();
	afx_msg void OnCrtAnimObjByMask();
	afx_msg void OnAdjustGameArea();
	afx_msg void OnRemoveEdges();
	afx_msg void OnRemoveEdgesForAllObjs();
	afx_msg void OnRemoveEdgesInStates();
	afx_msg void OnDisableMouse();
	afx_msg void OnSelectMovementType();
	afx_msg void OnStateDims();
	afx_msg void OnBtnAutoBnd();
	afx_msg void OnStateHasBound();
	afx_msg void OnMakeBoundAsFirst();
	afx_msg void OnStackState();
	afx_msg void OnCoordsAnimation();
	afx_msg void OnCancelRemovingEdges();
	afx_msg void OnPersDim();
	afx_msg void OnPersDir();
	afx_msg void OnNotControledPers();
	afx_msg void OnRunAnimationEditor();
	afx_msg void OnEditDialogs();
	afx_msg void OnLastNail();
	afx_msg void OnJoinProject();
	afx_msg void OnSelectScene();
	afx_msg void OnEditGaits();
	afx_msg void OnNetProperties();
	afx_msg void OnRunScene();
	afx_msg void OnRunScript();
	afx_msg void OnShowAllNetProjs();
	afx_msg void OnDrawAllBounds();
	afx_msg void OnToolbarDropDown(NMHDR* pNMHDR, LRESULT *plr);
	afx_msg void OnDelete();
	afx_msg void OnMgParameters();
	afx_msg void OnAppSettings();
	afx_msg void OnCollapseAll();
	afx_msg void OnExpandAll();

	afx_msg void onUpdateCmdUIScalePlus(CCmdUI* pCmdUI);
	afx_msg void onUpdateCmdUIScaleMinus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWaveToLocal(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLocalToWave(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePlayStopVideo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateStopVideo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCrtMovingObj(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNetProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCrtAnimObjs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCreateStatics(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCreateBack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditAnimation(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSound(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLastNail(CCmdUI* pCmdUI);
	afx_msg void OnUpdateJoinProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectScene(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditGaits(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRunScene(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRunScript(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowAllNetProjs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawAllBounds(CCmdUI* pCmdUI);

	afx_msg void OnTbtnEntryForDialogs();
	afx_msg void OnTbtnEntryForDynObj();
	afx_msg void OnTbtnEntryForMouse();
	afx_msg void OnTbtnEntryForPers();
	afx_msg void OnTbtnShowElement();
	afx_msg void OnUpdateTbtnEntryForDialogs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTbtnEntryForDynObj(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTbtnEntryForMouse(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTbtnEntryForPers(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTbtnShowElement(CCmdUI* pCmdUI);


	DECLARE_MESSAGE_MAP();
protected:

	void removeTmps();
	CString getTemporaryFileName();
	//! Собирает имя временного файла, который используется при автосохранении
	void makeTempFileName();

	void reset();
	void ShowAnimation();

	void setInTriggerIcon(qdTriggerChain const* chain, int command);

	//! Вызывается перед сменой окон редактора триггеров и редактора сцены.
	bool onChangingViews(bool bIsSVActiveOld, bool bIsSVActiveNew);
	//! Вызывается после смены окон редактора триггеров и редактора сцены.
	void onChangeViews(bool bIsSVActiveOld, bool bIsSVActiveNew);

	//! Метод IViewMgr
	virtual BOOL ShowSceneEditor();
	//! Метод IViewMgr
	virtual BOOL ShowTriggerEditor();

	//! Показать(активировать) редактор сцены
	void showSceneView();
	//! Показть(активировать) редактор триггеров
	void showTEView();

	GameObjectFactory& getGameObjectFactory();
	ProjectModifier& getProjectModifier();

	void showAnimationCatalogDlg(ITreeNode* focused, int eAction, bool bUniqueList);
	void showSoundCatalogDlg(ITreeNode* dispNode, bool bUniqueList);

	//! Составляет список зависимостей объекта(вхождение в условия и базовый объект КА)
	bool statObjectDependencies(qdNamedObject const* object);
	//! Отбрасывает все ссылки на подъобъекты объекта(состояния для анимированных, 
	//! все объекты для сцены), не выходящие за пределы объекта
	FilterObjectDependencies::eDependenciesBound 
		filterDepedenciesBound(qdNamedObject const* object);
	//! Заполнение списка объектов из ptrObjectsDependencies_
	void fillObjectList();

	CString const& getTempFileName() const;
	void setTempFileName(CString const& str);
	
	/* Функции этого кода теперь выполняет TreeLogic::saveFullStruct(...)
	//! Сохраняет состояние для папок объектов
	void saveSceneSubNodesState(IProjectSettings* pps);
	//! Сохраняет состояние для интерфейсных экранов
	void saveInterfaceScreensState(IProjectSettings* pps);
	*/
	//! Сохраняет скрипт, делая предварительно копию
	bool safeSaveScript(qdGameDispatcher* pdisp, LPCTSTR lpszFileName);
	//! Сериализует настройки проекта (структуру дерева проекта, настройки редактора триггеров и т.д.)
	void saveProjSettings();
	void readProjSettings();
	//! Сохраняет кто был активным редактор триггеров или редактор сцены
	void saveActiveView(IProjectSettings* pps);
	//! Сохраняет для каждой сцены метод отображения сетки
	void saveNetDrawMode(IProjectSettings* pps);
	//! Сохраняет в настройках проекта флаг showTirgger_
	void saveShowTriggerElement(IProjectSettings* pps);

	//! Считывает из настроек проекта флаг "Показывать ли элементы в триггере"
	void readShowTriggerElement(IProjectSettings* pps);
	//! Считывает и устанавливает активный редактор
	void readActiveView(IProjectSettings* pps);
	//! Считывает режимы прорисовки сетки для сцен
	void readNetDrawMode(IProjectSettings* pps);
	//! Считывает из настроек проекта состояние сетки для сцены и устанавливает его
	void adjustNetDrawMode(qdGameScene* ptrScene, LPCTSTR lpszItemName, 
		IProjectSettings* pps);
	//! Считывает состояние для папок объектов
	void readSceneSubNodesState(IProjectSettings* pps);
	//! Считывает состояние для интерфейсных экранов
	void readInterfaceScreensState(IProjectSettings* pps);

	//! Удалеяет пустоты
	void removeEdges();

	LRESULT beginEditStateDims(qdGameObjectState* pstate);
	void endEditStateDims(bool bSave);

	LRESULT beginStackState(qdGameObjectState* pstate);
	void endStateStack(bool bCancel);

	LRESULT beginCAEdit(qdGameObjectState* pstate);
	bool endEditCoordAnimationMode(bool bCancel);

	bool OnEditAnimatedDir(qdGameObjectAnimated* ptrObj);
	bool OnEditAnimatedDims(qdGameObjectAnimated* ptrObj);

	void activateScene(qdGameScene const* scene);

	//! Заполняет меню выпадающее при нажатии на стрелочку у кнопки "Т".
	void FillMenuForTriggers(CMenu*pm);
	//! Обрабатывает команды от меню сделанного FillMenuForTriggers
	void HandleEditTriggersMenu(HMENU hm, DWORD pos);

	//! Удаляет узел и объект этого узла 
	bool deleteNode(ITreeNode* node, bool ask);
	//! Переустанвливает период автосохранения
	void startAutoSavePeriod();
private:
	ISceneViewFacade* sceneView_;
	ITriggerView* triggerEditorView_;
	StructureView* structureView_;
	boost::scoped_ptr<qdGameDispatcher> gameDispatcher_;
	boost::scoped_ptr<TreeLogic> treeLogic_;
	boost::scoped_ptr<TreeNodeFactory> treeNodeFactory_;
	//! Можем ли активировать редактор сцены
	bool canActivateTriggerView_;
	ViewActivator viewActivator_;
	IScaleMgr* scaleManager_;

	//! Этот класс отвечает за создание объектов движка
	boost::scoped_ptr<GameObjectFactory> gameObjectFactory_;

	//! Этот класс отвечает за вставку в проект новых объектов
	boost::scoped_ptr<ProjectModifier> projectModifier_;
	//! Связь со списком объектов
	IObjectList* objectList_;

	//! Индекс зависимостей. Ключ объект.В списке, объекты от которых зависит
	std::map<qdNamedObject const*, NamedObjectList> *ptrObjectsDependencies_;

	///\brief сохраняем указатель на диалог,
	///из которого было затребовано либо редактирование КА
	///либо стыковка, либо редактирование боунда состояния
//	IRequestingStateEdit *m_ptrIReqStateEdit;
	//! Диалог редактирующий КА
	boost::scoped_ptr<class CoordAnimDlg>	coordAnimDlg_;
	//! Диалог помогающий в стыковке состояния
	boost::scoped_ptr<class StateCoordsStacker>  stateStackerDlg_;
	//! Диалог редактирующий размеры состояния
	boost::scoped_ptr<class StateBoundDlg> stateBoundDlg_;

	//! Предыдущее состяния SceneView
	/*!
	Сохраняем его перед запуском редактирования координатной анимации, размеров
	или стыковки, чтобы затем восстановить его
	*/
	int oldViewState_;
	/*! \brief содержит имена временных файлов, которые надо будет удалить
	в конце работы программы
	*/
	std::vector<CString> m_TmpFiles;
	//! Определяет, какие элементы в редаторе триггеров будут подсвечиваться
	eFilterScheme triggerElementFilterScheme_;
	//! Центровать ли на элементе окно редатора триггеров
	bool showTriggerElement_;

	UINT autoSaveTimer_;
	//! Имя файла, в который будет производиться автосейв
	CString tempFileName_;
};
