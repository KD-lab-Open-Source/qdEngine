// Main\Project.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"

#include "XUtil.h"

#include "NewProjMultiWizard.h"

#include "Main\Project.h"
#include "Main/GameObjectFactory.h"
#include "Main/ProjectModifier.h"
#include "Main/ObjectTraits.h"

#include "ISceneViewFacade.h"
#include "ITriggerView.h"

#include ".\project.h"
#include "StructureView.h"
#include "Tree/TreeLogic.h"
#include "Tree/TreeNodeFactory.h"
#include "Tree/NodeTraits.h"

#include <Shlwapi.h>
#include "ptree_consts.h"
#include "ptree_hlprs.hpp"

#include "qd_game_dispatcher.h"
#include "qdscr_convert.h"
#include "gdi_gr_dispatcher.h"
#include "snd_dispatcher.h"
#include "qd_game_object_moving.h"
#include "qd_trigger_chain.h"
#include "qd_game_object_static.h"
#include "qd_minigame.h"

#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"
#include "qd_interface_button.h"


#include "qd_camera.h"

#include "ptree_te_filters.h"

#include "ProgDlg.h"
#include "AddAnimStateWPg.h"
#include "AnimCatalogDlg.h"
#include "SoundCatalogDlg.h"
#include "GenStngsPP.h"
#include "AppStngsDlg.h"

#include "MoveStopDlg.h"

#include "WaveLocalization/WaveToLoc.h"

#include "disk_op.h"

#include <boost/rtl.hpp>
#include <sstream>

#include "Stat Dependencies/StatObjectDependencies.h"

#include "addintrfbackgrndwizardshow.h"
#include "addinterfsldrwizardshow.h"
#include "addinterfbtnwizshow.h"
#include "addinterfbtnstatewizshow.h"
#include "addinterfsavewizshow.h"
#include "addgameendwizardshower.h"

#include "AnimationFileNameExtractor.h"

#include "EdgeRemover.h"

#include "StateBoundDlg.h"
#include "CoordAnimDlg.h"
#include "StateCoordsStacker.h"

#include "SceneList.h"
#include "ProjectJoiner.h"
#include "ProcessLauncher.h"

#include "Main/Utils/RunDialogEditor.h"

#include "MoveStopDlg.h"

bool AskUserForName(CString& strDef);
CString const AskUserForName(CString const& strDef);

std::pair<bool, qdGameObject*> AskForObject(qdGameScene* ptrScene, 
											int type, bool bCanCrtNew);

bool EditAnimation(HWND hParent, LPCTSTR lpszFileName);
bool showCDDiskManager(qdGameDispatcher* pdispatcher);

bool SelectMovementType(qdGameObjectStateWalk* pstate);
void SaveScrLayoutOnBndEdt(qdGameObjectState* pstate, qdCamera* pcam, 
						   Vect3f const& old_bnd);
void SaveScrLayoutOnBndEdt(qdGameObjectState* pstate, qdCamera* pcam, 
						   Vect3f const& old_bnd, Vect3f const&bnd);

bool editMinigameParams(qdGameDispatcher* gameDisp, qdMiniGame* miniGame);


const UINT TM_AUTO_SAVE_TIMER		= 1;
const DWORD AUTO_SAVE_TIMER_PERIOD	= 120000;

namespace {
	bool SelectName(CString& strName){
		do {
			if(!AskUserForName(strName))
				return false;
			LPCTSTR lpszExt = PathFindExtension(strName);
			if (!lpszExt||_tcsicmp(lpszExt, _T(".qml"))){
				strName += _T(".qml");
			}

			if(!PathFileExists(strName))
				return true;
			CString msg;
			msg.Format(IDS_FILE_CONVERTER_FILE_EXIST_SELECT_NEW, 
				static_cast<LPCTSTR>(strName));
			AfxMessageBox(msg);
		} while(true);
		return true;
	}

	const CString checkFileFormat(const CString &strFileName){
		if (!qdscr_is_XML(strFileName))
		{
			CString strNewFileName(qdscr_get_XML_file_name(strFileName));
			if (PathFileExists(strNewFileName)) 
			{
				CString strMsg;
				strMsg.Format(IDS_FILE_CONVERTER_XML_FILE_EXIST_OVERWRITE,
					static_cast<LPCTSTR>(strFileName));
				int res = AfxMessageBox(strMsg, MB_YESNOCANCEL);
				switch(res) {
			case IDYES:
				if(!SelectName(strNewFileName))
					return CString();
				break;
			case IDNO:
				//ничего не делаем
				break;
			case IDCANCEL:
				return CString();
				}
			}
			if(qdscr_convert_to_XML(strFileName, strNewFileName))
				return strNewFileName;
			else
			{
				CString strMsg;
				strMsg.Format(IDS_ERR_CONVERT_TO_XML_FAILED, 
					static_cast<LPCTSTR>(strFileName));

				AfxMessageBox(strMsg);
				return CString();
			}
		}
		return strFileName;
	}
	void sceneLoadingProgressCallback(int percents, void * data)
	{
		CProgressDlg* dlg = (CProgressDlg*)data;
		dlg->SetPos(percents);
	}

	CString makeIniFileName(const CString& strFileName)
	{
		int g = strFileName.Find(_T(".qml"));
		if (g == -1) 
			g = strFileName.Find(_T(".qds"));

		ASSERT(g != -1);
		return (strFileName.Left(g) + _T(".ini"));
	}
	CString makeTreeStructFileName(const CString& strFileName)
	{
		int g = strFileName.Find(_T(".qml"));
		if (g == -1) 
			g = strFileName.Find(_T(".qds"));
		if (g == -1) 
			g = strFileName.Find(_T(".ini"));

		ASSERT(g != -1);
		return (strFileName.Left(g) + _T(".ts"));
	}

	Project* g_proejct_ = NULL;
}

IProjectFacade* getProject(){
	return g_proejct_;
}

#ifdef USE_NEW_PROJ
void SetProjChange()
{
	getProject()->SetModified(true);
}
#endif // USE_NEW_PROJ
// Project

IMPLEMENT_DYNCREATE(Project, CDocument)

Project::Project():
  sceneView_(NULL)
, triggerEditorView_(NULL)
, structureView_(NULL)
, gameDispatcher_(NULL)
, treeNodeFactory_(new TreeNodeFactory)
, scaleManager_(NULL)
, viewActivator_(this)
, objectList_(NULL)
, triggerElementFilterScheme_(FS_NONE)
, showTriggerElement_(false)
{
	treeLogic_.reset(new TreeLogic(this, treeNodeFactory_.get()));
	gameObjectFactory_.reset(new GameObjectFactory);
	projectModifier_.reset(new ProjectModifier(*gameObjectFactory_, this));
	g_proejct_ = this;
	autoSaveTimer_ = 0;
}


BOOL Project::OnNewDocument()
{
	return TRUE;
}

Project::~Project()
{
}


BEGIN_MESSAGE_MAP(Project, CDocument)
	ON_COMMAND(IDC_PROPERIES, OnProperies)
	ON_COMMAND(IDC_COND_OBJ_CONDITION, onConditions)
	ON_COMMAND(IDC_STATE_CONDITION, onConditions)
	ON_COMMAND(IDC_EDIT_TRIGGERS, onEditTriggers)
	ON_COMMAND(IDC_CRT_ANIMATED_OBJ, OnCrtAnimatedObj)
	ON_COMMAND(IDC_CRT_ANIM_OBJS, OnCrtAnimatedObj)
	ON_COMMAND(IDC_CRT_MOVING_OBJ, OnCrtMovingObj)
	ON_COMMAND(IDC_CRT_STATIC_OBJ, OnCrtStaticObj)
	ON_COMMAND(IDC_CREATE_STATICS, OnCreateStatics)
	ON_COMMAND(ID_WAVE_TO_LOCALIZATION, OnWaveToLocal)
	ON_COMMAND(ID_LOCALIZATION_TO_WAVE, OnLocalToWave)
	ON_COMMAND(IDC_RUN_ANIM_EDITOR, OnRunAnimationEditor)
	ON_COMMAND(ID_BTN_EDIT_DIALOGS, OnEditDialogs)
	ON_COMMAND(IDC_LAST_NAIL, OnLastNail)
	ON_COMMAND(ID_BUTTON_JOIN_PROJECT, OnJoinProject)
	ON_COMMAND(IDC_SEL_SCENE, OnSelectScene)
	ON_COMMAND(IDC_EDIT_GAITS, OnEditGaits)
	ON_COMMAND(IDC_NET_PROPERTIES, OnNetProperties)
	ON_COMMAND(ID_BUTTON_RUN_SCENE, OnRunScene)
	ON_COMMAND(IDC_BUTTON_RUN_SCRIPT, OnRunScript)
	ON_COMMAND(IDC_DRAW_ALL_BOUNDS, OnDrawAllBounds)
	ON_COMMAND(IDC_SHOW_ALL_NET_PROJS, OnShowAllNetProjs)

	ON_UPDATE_COMMAND_UI(ID_TBTN_SCALE_PLUS, onUpdateCmdUIScalePlus)
	ON_UPDATE_COMMAND_UI(ID_TBTN_SCALE_MINUS, onUpdateCmdUIScaleMinus)
	ON_UPDATE_COMMAND_UI(IDC_CREATE_BACK, OnUpdateCreateBack)
	ON_UPDATE_COMMAND_UI(IDC_CREATE_STATICS, OnUpdateCreateStatics)
	ON_UPDATE_COMMAND_UI(IDC_CRT_MOVING_OBJ, OnUpdateCrtMovingObj)
	ON_UPDATE_COMMAND_UI(IDC_NET_PROPERTIES, OnUpdateNetProperties)
	ON_UPDATE_COMMAND_UI(IDC_CRT_ANIM_OBJS, OnUpdateCrtAnimObjs)

	ON_UPDATE_COMMAND_UI(IDC_PLAY_STOP_VIDEO, OnUpdatePlayStopVideo)
	ON_UPDATE_COMMAND_UI(IDC_STOP_VIDEO, OnUpdateStopVideo)
	ON_UPDATE_COMMAND_UI(ID_WAVE_TO_LOCALIZATION, OnUpdateLocalToWave)
	ON_UPDATE_COMMAND_UI(ID_LOCALIZATION_TO_WAVE, OnUpdateWaveToLocal)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_SOUND, OnUpdateEditSound)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_ANIMATION, OnUpdateEditAnimation)
	ON_UPDATE_COMMAND_UI(IDC_LAST_NAIL, OnUpdateLastNail)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_JOIN_PROJECT, OnUpdateJoinProject)
	ON_UPDATE_COMMAND_UI(IDC_SEL_SCENE, OnUpdateSelectScene)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_GAITS, OnUpdateEditGaits)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_RUN_SCENE, OnUpdateRunScene)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_RUN_SCRIPT, OnUpdateRunScript)
	ON_UPDATE_COMMAND_UI(IDC_DRAW_ALL_BOUNDS, OnUpdateDrawAllBounds)
	ON_UPDATE_COMMAND_UI(IDC_SHOW_ALL_NET_PROJS, OnUpdateShowAllNetProjs)

	ON_COMMAND(IDC_CRT_SCENE, OnCrtScene)
	ON_COMMAND(IDC_ADD_INTERF_BACKGROUND, OnAddInterfBackground)
	ON_COMMAND(IDC_ADD_INTERF_BTN_STATE, OnAddInterfBtnState)
	ON_COMMAND(IDC_ADD_INTERF_SAVE, OnAddInterfSave)
	ON_COMMAND(IDC_CREATE_ISCREEN, OnCreateIscreen)
	ON_COMMAND(IDC_ADD_INTERF_SLIDER, OnAddInterfSlider)
	ON_COMMAND(IDC_ADD_INTERF_TEXT_WINDOW, OnAddInterfTextWindow)
	ON_COMMAND(IDC_ADD_INTRF_BUTTON, OnAddIntrfButton)
	ON_COMMAND(IDC_ADD_INTERF_COUNTER, OnAddInterfCounter)
	ON_COMMAND(IDC_ADD_GAME_END, OnAddGameEnd)
	ON_COMMAND(IDC_ADD_FONT, OnAddFont)
	ON_COMMAND(IDC_CREATE_FONT, OnCreateFont)
	ON_COMMAND(IDC_ADD_VIDEO, OnAddVideo)
	ON_COMMAND(IDC_CREATE_MINIGAME, OnCreateMinigame)
	ON_COMMAND(IDC_CRT_NEW_INVENTORY, OnCrtNewInventory)
	ON_COMMAND(IDC_CREATE_COUNTER, OnCreateCounter)
	ON_COMMAND(IDC_CREATE_STATE, OnCreateState)
	ON_COMMAND(IDC_CRT_TRIGGER, OnCrtTrigger)
	ON_COMMAND(IDC_CRT_ANIMATION, OnCrtAnimation)
	ON_COMMAND(IDC_ANIMATION_LIST, OnAnimationList)
	ON_COMMAND(IDC_SOUND_LIST, OnSoundList)
	ON_COMMAND(IDC_EDIT_SOUND, OnEditSound)
	ON_COMMAND(IDC_EDIT_STATE_ANIMATION, OnEditStateAnimation)
	ON_COMMAND(IDC_MENU_MOVINGS_STOPS, OnMenuMovingsStops)
	ON_COMMAND(IDC_GAME_SETTINGS, OnGameSettings)
	ON_COMMAND(ID_MANAGE_CDS, OnManageCds)
	ON_COMMAND(IDC_STAT_DEPENDENCIES, OnStatDependencies)
	ON_COMMAND(IDC_ADD_CELL_SET, OnAddCellSet)
	ON_COMMAND(IDC_REMOVE_CELL_SET, OnRemoveCellSet)
	ON_COMMAND(IDC_ADD_MUSIC_TRACK, OnAddMusicTrack)
	ON_COMMAND(IDC_CREATE_MASK, OnCreateMask)
	ON_COMMAND(IDC_CRT_GRID_ZONE, OnCrtGridZone)
	ON_COMMAND(IDC_MUSIC_TRACK_CYCLED, OnMusicTrackCycled)
	ON_COMMAND(IDC_CRT_ANIM_OBJ_BY_MASK, OnCrtAnimObjByMask)
	ON_COMMAND(IDC_ADJUST_GAME_AREA, OnAdjustGameArea)
	ON_COMMAND(IDC_REMOVE_EDGES, OnRemoveEdges)
	ON_COMMAND(IDC_REMOVE_EDGES_FOR_ALL_OBJS, OnRemoveEdgesForAllObjs)
	ON_COMMAND(IDC_REMOVE_EDGES_IN_STATES, OnRemoveEdgesInStates)
	ON_COMMAND(IDC_DISABLE_MOUSE, OnDisableMouse)
	ON_COMMAND(IDC_SELECT_MOVEMENT_TYPE, OnSelectMovementType)
	ON_COMMAND(IDC_STATE_DIMS, OnStateDims)
	ON_COMMAND(IDC_BTN_AUTO_BND, OnBtnAutoBnd)
	ON_COMMAND(IDC_STATE_HAS_BOUND, OnStateHasBound)
	ON_COMMAND(IDC_MAKE_BOUND_AS_FIRST, OnMakeBoundAsFirst)
	ON_COMMAND(IDC_STACK_STATE, OnStackState)
	ON_COMMAND(IDC_COORDS_ANIMATION, OnCoordsAnimation)
	ON_COMMAND(IDC_CANCEL_REMOVING_EDGES, OnCancelRemovingEdges)
	ON_COMMAND(IDC_PERS_DIM, OnPersDim)
	ON_COMMAND(IDC_PERS_DIR, OnPersDir)
	ON_COMMAND(IDC_NOT_CONTROLED_PERS, OnNotControledPers)
	ON_NOTIFY(TBN_DROPDOWN, 2, OnToolbarDropDown)
	ON_COMMAND(IDC_REMOVE, OnDelete)
	ON_COMMAND(IDC_MG_PARAMETERS, OnMgParameters)

	ON_COMMAND(ID_TBTN_ENTRY_FOR_DIALOGS, OnTbtnEntryForDialogs)
	ON_COMMAND(ID_TBTN_ENTRY_FOR_DYN_OBJ, OnTbtnEntryForDynObj)
	ON_COMMAND(ID_TBTN_ENTRY_FOR_MOUSE, OnTbtnEntryForMouse)
	ON_COMMAND(ID_TBTN_ENTRY_FOR_PERS, OnTbtnEntryForPers)
	ON_COMMAND(ID_TBTN_SHOW_ELEMENT, OnTbtnShowElement)
	ON_UPDATE_COMMAND_UI(ID_TBTN_ENTRY_FOR_DIALOGS, OnUpdateTbtnEntryForDialogs)
	ON_UPDATE_COMMAND_UI(ID_TBTN_ENTRY_FOR_DYN_OBJ, OnUpdateTbtnEntryForDynObj)
	ON_UPDATE_COMMAND_UI(ID_TBTN_ENTRY_FOR_MOUSE, OnUpdateTbtnEntryForMouse)
	ON_UPDATE_COMMAND_UI(ID_TBTN_ENTRY_FOR_PERS, OnUpdateTbtnEntryForPers)
	ON_UPDATE_COMMAND_UI(ID_TBTN_SHOW_ELEMENT, OnUpdateTbtnShowElement)

	ON_COMMAND(IDC_APP_SETTINGS, OnAppSettings)
	ON_COMMAND(IDC_COLLAPSE_ALL, OnCollapseAll)
	ON_COMMAND(IDC_EXPAND_ALL, OnExpandAll)
	END_MESSAGE_MAP()


// Project diagnostics

#ifdef _DEBUG
void Project::AssertValid() const
{
	CDocument::AssertValid();
}

void Project::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Project serialization

void Project::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

Project::eFilterScheme Project::getTriggerElementFilterScheme() const{
	return triggerElementFilterScheme_;
}

IObjectList* Project::getObjectList() const{
	return objectList_;
}
void Project::setObjectList(IObjectList* ol){
	objectList_ = ol;
}

TreeLogic& Project::getTreeLogic() const{
	return *treeLogic_;
}

ITreeNodeFactory& Project::getTreeNodeFactory() const{
	return *treeNodeFactory_;
}

ISceneViewFacade* Project::getSceneView() const{
	return sceneView_;
}
ITriggerView* Project::getTriggerEditorView() const{
	return triggerEditorView_;
}

StructureView* Project::getStructureView() const{
	return structureView_;
}

qdGameDispatcher* Project::getGameDispatcher() const{
	return gameDispatcher_.get();
}

bool Project::getShowTriggerElement() const{
	return showTriggerElement_;
}

void Project::setSceneView(ISceneViewFacade* sceneView){
	sceneView_ = sceneView;
	if (sceneView)
	{
		sceneView->setProject(this);
		treeLogic_->setSceneView(sceneView_);
	}
}

void Project::setTriggerEditorView(ITriggerView* teView){
	triggerEditorView_ = teView;
	if (treeLogic_)
		treeLogic_->setTriggerView(triggerEditorView_);
}

void Project::setStructureView(StructureView* structureView){
	structureView_ = structureView;
	ASSERT_VALID(structureView_);
	QETreeListCtrl& tree = structureView_->getTreeListCtrl();
	treeLogic_->setTreeListCtrl(&tree);
	treeLogic_->setBaseWnd(structureView_);
//	structureView_->setProject(this);
	treeLogic_->setTriggerView(triggerEditorView_);
}

ProjectModifier& Project::getProjectModifier(){
	return *projectModifier_;
}

void Project::setScaleMngr(IScaleMgr* mngr){
	scaleManager_ = mngr;
}

IScaleMgr* Project::getScaleMngr() const{
	return scaleManager_;
}

GameObjectFactory& Project::getGameObjectFactory(){
	if (!gameObjectFactory_)
		gameObjectFactory_.reset(new GameObjectFactory);
	return *gameObjectFactory_;
}

CString const& Project::getTempFileName() const{
	return tempFileName_;
}

void Project::setTempFileName(CString const& str){
	tempFileName_ = str;
}

void Project::reset()
{
	if (treeLogic_) 
		treeLogic_->reset();
	if (gameDispatcher_) 
	{
		gameDispatcher_->stop_video();
		gameDispatcher_->close_video();
	}
	if (triggerEditorView_)
		triggerEditorView_->reset();
	if (sceneView_)
		sceneView_->reset();
	gameDispatcher_.reset();
	removeTmps();
	viewActivator_ = true;
//	TreeNodeSingletonPool::release_memory();
}
// Project commands

BOOL Project::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!GetPathName().IsEmpty())
		saveProjSettings();

	reset();
	SetCurrentDirectory(disk_op::GetPathFromFullFileName(lpszPathName));
	CString fileName = checkFileFormat(lpszPathName);

	if (fileName.IsEmpty()||!CDocument::OnOpenDocument(fileName))
		return FALSE;


	gameDispatcher_.reset(new qdGameDispatcher);
	if (!gameDispatcher_)
		return FALSE;

	qdGameDispatcher::set_dispatcher(gameDispatcher_.get());
	qdGameDispatcher::free_font(grDispatcher::get_default_font());
	grDispatcher::set_default_font(qdGameDispatcher::create_font(0));

	triggerEditorView_->setGameDisp(gameDispatcher_.get());

	gameDispatcher_->load_script(fileName);

	treeLogic_->loadStructure();

	SetTitle(fileName);

	makeTempFileName();

	CString iniFileName = makeIniFileName(fileName);
	GetMyApp()->ResetProjectSettingsMgr(iniFileName);
	readProjSettings();

	// Это трюк, чтобы не писало в заголовке окна всякую гадость
	if (!m_viewList.GetSize())
		AddView(structureView_);

	return TRUE;
}

//! Добавляет анимированному объекту обычное состояние 
qdGameObjectStateStatic* Project::AddEmptyStaticState(qdGameObjectAnimated* poa)
{
	std::auto_ptr<qdGameObjectStateStatic> state(
							getGameObjectFactory().createStaticState());
	if (!state.get())
		return NULL;

	if(poa->add_state(state.get()))
		return state.release();

	return NULL;
}
//! Вставляет обычное состояние перед указанным
qdGameObjectStateStatic* Project::InsertEmptyStaticState(qdGameObjectAnimated* poa, 
														int iBefore)
{
	std::auto_ptr<qdGameObjectStateStatic> ptrState(
								getGameObjectFactory().createStaticState());
	if(!ptrState.get())
		return NULL;

	if(poa->insert_state(iBefore, ptrState.get()))
		return ptrState.release();

	return NULL;
}

//! Добавляет персонажу походочное состояние 
qdGameObjectStateWalk* Project::AddEmptyMovementState(qdGameObjectMoving* pom)
{
	std::auto_ptr<qdGameObjectStateWalk> ptrState (
								getGameObjectFactory().createWalkState());
	if(!ptrState.get())
		return NULL;

	if(pom->add_state(ptrState.get()))
		return ptrState.release();

	return NULL;
}
//! Вставляет походочное состояние перед указанным
qdGameObjectStateWalk* Project::InsertEmptyMovementState(qdGameObjectMoving* pom, 
														int iBefore)
{
	std::auto_ptr<qdGameObjectStateWalk> ptrState (
							getGameObjectFactory().createWalkState());
	if(!ptrState.get())
		return NULL;

	if(pom->insert_state(iBefore, ptrState.get()))
		return ptrState.release();

	return NULL;
}


//! Добавляет в сцену и в дерево анимированный объект
qdGameObjectAnimated* Project::AddAnimated(qdGameScene* ptrScene, 
										  CString const& strName, bool bShow)
{
	return getProjectModifier().addAnimated(ptrScene, strName, bShow);
}

//! Добавляет в сцену и в дерево персонаж
qdGameObjectMoving* Project::AddPersonage(qdGameScene* ptrScene, 
										 CString &strName, bool bShow)
{
	return getProjectModifier().addPersonage(ptrScene, strName, bShow);
}

//! Добавляет в проект и в дерево глобальный персонаж
qdGameObjectMoving* Project::AddPersonage(qdGameDispatcher* ptrDisp,
										 CString &strName, bool bShow)
{
	return getProjectModifier().addGlobalPersonage(ptrDisp, strName, bShow);
}
//! Добавляет в сцену и в дерево статический объект
qdGameObjectStatic* Project::AddStatic(qdGameScene* ptrScene, 
									  CString& strName, bool bShowInTree)
{
	return getProjectModifier().addStatic(ptrScene, strName, bShowInTree);
}

//! Добавляет в проект и в дерево сцену
qdGameScene* Project::AddScene(CString &strName, bool bShowNew)
{
	return getProjectModifier().addScene(strName, bShowNew);
}

//! Удаляет объект из дерева и из проекта
bool Project::DeleteObject(class qdNamedObjectBase* ptrObj, bool bAsk)
{
	ITreeNode* node = *getTreeLogic().getObjectNodes(ptrObj);

	return deleteNode(node, bAsk);
}


//! Пробегает по сценам и обновляет в дереве списки объектов
void Project::UpdateCatalogsInScenes(qdNamedObjectType type)
{
	ASSERT(0);
}

//! Обновляет каталог анимированных объектов в указанной сцене
void Project::UpdateObjCatalogInScene(qdNamedObjectType obj_type,
									 qdGameScene const* pscene)
{
//	ASSERT(0);
	ITreeNode* objectNode = *getTreeLogic().getObjectNodes(pscene);
	ASSERT(objectNode);
	objectNode->reload(&getTreeNodeFactory(), getTreeLogic(), NULL, NULL);
}


//! Удаляет состояние
bool Project::RemoveState(qdGameObjectState* ptrObj, bool bAsk)
{
	ASSERT(0);
	return false;
}

//! Перезагружает состояния объекта в дерево
void Project::ReloadStates(qdGameObjectAnimated* pobj)
{
	ITreeNode* objectNode = *getTreeLogic().getObjectNodes(pobj);
	ASSERT(objectNode);
	objectNode->reload(&getTreeNodeFactory(), getTreeLogic(), NULL, NULL);
}

//! Показывает меню для объекта
void Project::ShowCorrespondingMenu(qdNamedObjectBase* pno)
{
	ITreeNode* objectNode = *getTreeLogic().getObjectNodes(pno);
	ASSERT(objectNode);
	objectNode->onRClick(getTreeLogic());
}

/************************************************************************/
/* Набор для редактора триггеров                                        */
/************************************************************************/
//! Вызывается на удаление объекта из триггреа. Сейчас обнволяет изображение в дереве
void Project::ptOnRemoveObjFromTrigger(qdNamedObject* pno)
{
	TreeLogic& logic = getTreeLogic();
	boost::rtl::irange<TreeLogic::NodeIterator> r = getTreeLogic().getObjectNodes(pno);
	for (; r; ++r)
		logic.setInTriggerIcon(*r, TIC_REMOVE);
}

//! После добавления объекта в триггер, обновляем его изображение в дереве
void Project::OnAddObjToTrigger(qdNamedObject* pno)
{
	TreeLogic& logic = getTreeLogic();
	boost::rtl::irange<TreeLogic::NodeIterator> r = getTreeLogic().getObjectNodes(pno);
	for (; r; ++r)
		logic.setInTriggerIcon(*r, TIC_SET_ACTIVE);
}
//! Показать диалог условий для объекта
bool Project::ShowCondDlg(qdNamedObject* pobj)
{
	ITreeNode* objectNode = *getTreeLogic().getObjectNodes(pobj);
	ASSERT(objectNode);

	return getTreeLogic().showConditions(objectNode);
}

//! Показать диалог свойств для объекта
bool Project::ShowProperties(qdNamedObjectBase* pno)
{
	TreeLogic::Nodes nodes = getTreeLogic().getObjectNodes(pno);
	ASSERT(nodes);

	ITreeNode* objectNode = *nodes;
	ASSERT(objectNode);

	return getTreeLogic().showProperties(objectNode, 0);
}
/************************************************************************/
/* Стыковка и координатная анимация                                     */
/************************************************************************/
//! Прекратить стыковку состояния
void Project::EndStateStack(eEndEditStateMode mode)
{
	endStateStack(mode == EESTM_CANCEL);
}

//! Прекратить редактирование КА
bool Project::EndEditCoordAnimation(eEndEditStateMode mode)
{
	return endEditCoordAnimationMode(mode == EESTM_CANCEL);
}

//! Перекратить редактирование размеров
void Project::EndResizeState(eEndEditStateMode mode)
{
	endEditStateDims(mode == EESTM_OK);
}

//! Удалить набор ячеек из инвентори
bool Project::DeleteInventoryCellset(class qdInventory* pinv, int icellset)
{
	pinv->remove_cell_set(icellset);
	return true;
}

/*!
 	Из ини файла считыает имя игры(обычно qd_game.exe). Получает имя временно файла
	для скрипта, сохраняет в этот файл проект. Формирует командную строку в зависимости
	от режима и запускает процесс с этой командной строкой
	\param strSceneName - сцена, которую надо запустить.
	\param emode		- режим, в котором должна быть запущена игра
*/
void Project::RunScene(const CString& strSceneName, eRunMode emode)
{
	CString strGameExeName;
	strGameExeName.LoadString(IDS_GAME_EXE_FILE_NAME);

	TCHAR buff[MAX_PATH];
	CString strIniFileName = disk_op::GetPathFromFullFileName(disk_op::GetModulePath());
	strIniFileName += _T("QuestEditor.ini");
	GetPrivateProfileString(_T("game_exe"), _T("name"),strGameExeName, 
		buff,MAX_PATH,strIniFileName);
	strGameExeName = buff;	

	CString app_path = disk_op::GetPathFromFullFileName(disk_op::GetModulePath());
	CString strTmpFileName = getTemporaryFileName();
	if (!gameDispatcher_->save_script(strTmpFileName))
	{
		AfxMessageBox(IDS_ERR_SAVE_FAILED, MB_OK|MB_ICONERROR);
		return;
	}

	app_path += strGameExeName;


	CString comand_line = _T("\"");
	comand_line += app_path;
	comand_line += _T("\"");

	comand_line += _T(" \"");
	comand_line += strTmpFileName;
	comand_line += _T("\"");

	if (emode != IProjectFacade::RM_RUN_SCRIPT)
	{
		comand_line += _T(" -S ");
		comand_line += _T("\"");
		comand_line += strSceneName;
		comand_line += _T("\"");
	}
	if (emode == IProjectFacade::RM_TRIGGER_TEST) 
	{
		comand_line += _T(" -");
		comand_line += qdTriggerChain::debug_comline();
		comand_line += _T(" ");
		comand_line += _T(" -");
		comand_line += qdTriggerProfiler::activation_comline();
	}

	ProcessLauncher launch;
	if (!launch.createProcess(app_path, CStrBuf(comand_line)))
	{
		AfxMessageBox(IDS_CNT_LOAD_GAME_EXE, MB_OK|MB_ICONSTOP);
	}
}

//! Добавить интерфейсный фон
qdInterfaceBackground* Project::AddInterfBackgrnd(const CString& name, 
												 qdInterfaceScreen* pscr)
{
	return getProjectModifier().addInterfaceBackground(name, pscr);
}

//! Добавить интерфейсный бегунок
qdInterfaceSlider* Project::AddInterfSldr(const CString& name, 
										 qdInterfaceScreen* pscr)
{
	return getProjectModifier().addInterfaceSlider(name, pscr);
}

//! Добавить окно текстов
qdInterfaceTextWindow* Project::AddInterfTextWindow(const CString& name, 
												   qdInterfaceScreen* pscr)
{
	return getProjectModifier().addInterfaceTextWindow(name, pscr);
}

qdInterfaceCounter* Project::AddInterfCounter(const CString& name, 
												   qdInterfaceScreen* pscr)
{
	return getProjectModifier().addInterfaceCounter(name, pscr);
}

//! Добавить интерфейсную кнопку
qdInterfaceSave* Project::AddInterfSave(CString const& name, 
									   qdInterfaceScreen* pscr)
{
	return getProjectModifier().addInterfaceSave(name, pscr);
}

//! Добавить состояние интерф. кнопке
int Project::AddInterfBtnState(const CString& name, qdInterfaceButton* pbtn)
{
	return getProjectModifier().addInterfButtonState(pbtn, name);
}

//! Добавить интерфейсную кнопку
qdInterfaceButton* Project::AddInterfBtn(const CString& name, 
										qdInterfaceScreen* pscr)
{
	return getProjectModifier().addInterfaceButton(name, pscr);
}

//! Добавить конечный экран
qdGameEnd* Project::AddGameEnd(ITreeNode* parent, 
							   CString const& name, 
							   CString const& screenName)
{
	return getProjectModifier().addGameEnd(parent, name, screenName);
}

//! Найти и выделить объект в дереве
bool Project::ShowObjectInTree(const qdNamedObjectBase* p)
{
	ITreeNode* objectNode = *getTreeLogic().getObjectNodes(p);
	ASSERT(objectNode);

	return getTreeLogic().showNode(objectNode);
}

//! Загрузить сцену в проект
CTreeListItem* Project::LoadScene(qdGameScene* pgs)
{
	ASSERT(0);
	return NULL;
}

//! Сообщает является ли активным редактор сцены
bool Project::isSceneViewActive() const{
	return viewActivator_.getSceneViewActive();
}

void Project::setSceneViewActive(bool bActive){
	if (canActivateTriggerView_&&bActive)
		viewActivator_ = true;
	else if (!bActive)
		viewActivator_ = false;
}

BOOL Project::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (lpszPathName != GetPathName())
	{
		saveProjSettings();
	}
	if (!safeSaveScript(gameDispatcher_.get(), lpszPathName))
	{
		AfxMessageBox(IDS_ERR_SAVE_FAILED, MB_OK|MB_ICONERROR);
		return FALSE;
	}
	CString iniFileName = makeIniFileName(lpszPathName);
	GetMyApp()->ResetProjectSettingsMgr(iniFileName);

	saveProjSettings();
	SetModified(false);
	return TRUE;
}


BOOL Project::OnCmdMsg(UINT nID, 
					   int nCode, 
					   void* pExtra, 
					   AFX_CMDHANDLERINFO* pHandlerInfo)
{

	return __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void Project::OnProperies()
{
	ITreeNode* focused = treeLogic_->getFocusedNode();
	if (!focused)
		return;
	treeLogic_->showProperties(focused, 0);
}

void Project::SetModified(bool bModified)
{
	SetModifiedFlag(bModified);
}

void Project::onConditions()
{
	ITreeNode* focused = treeLogic_->getFocusedNode();
	if (!focused)
		return;
	treeLogic_->showConditions(focused);
}

bool Project::SelectScene(qdNamedObject* scene)
{
	if(scene != gameDispatcher_->get_active_scene())
	{
		CWaitCursor __wait__;
		sceneView_->reset();
		CProgressDlg progress;
		//Устанавливаем каллбэк, чтобы получать статус загрузки
		gameDispatcher_->
			set_scene_loading_progress_callback(sceneLoadingProgressCallback,
												(void*)&progress);
		progress.Create();
		CString strStatus((LPTSTR)CG_IDS_PROGRESS_CAPTION);
		progress.SetStatus(strStatus);

		gameDispatcher_->select_scene(static_cast<qdGameScene*>(scene));

		//сбрасываем каллбэк
		gameDispatcher_->set_loading_progress_callback(NULL,NULL);

		sceneView_->AdjustBitmapSize();
		gameDispatcher_->quant();
		gameDispatcher_->quant();
		return true;
	}
	return false;
}

void Project::CollapseSceneNodes(qdNamedObject const* exclude)
{
	boost::rtl::crange<qdGameSceneList const> r(gameDispatcher_->scene_list());
	for(; r; ++r)
	{
		if (*r != exclude)
		{

			boost::rtl::irange<
				TreeLogic::NodeIterator
			> ir(treeLogic_->getObjectNodes(*r));

			if (ir)
				treeLogic_->getTreeListCtrl().Expand(
						(*ir)->getTreeListItem(), TLE_COLLAPSE);
		}
	}
}

void Project::setActiveTriggerIcon(qdTriggerChain const* chain, BOOL bSet)
{
	boost::rtl::crange<qdTriggerChainList const>r(gameDispatcher_->trigger_chain_list());
	for(; r; ++r)
	{
		if (chain != *r)
			setInTriggerIcon(*r, bSet ? TIC_SET_NOT_ACTIVE : TIC_REMOVE);
	}
	if (chain)
		setInTriggerIcon(chain, bSet ? TIC_SET_ACTIVE : TIC_REMOVE);
}

void Project::setInTriggerIcon(qdTriggerChain const* chain, int command)
{
	boost::rtl::crange<qdTriggerElementList const> r(chain->elements_list());
	for (; r; ++r)
	{
		boost::rtl::irange<
			TreeLogic::NodeIterator
		> ir(treeLogic_->getObjectNodes((*r)->object()));

		for (;ir; ++ir)
			treeLogic_->setInTriggerIcon(*ir, static_cast<eTriggerIconCommand>(command));
	}
}

/*!
Получает выбранный узел и вызывает Project::OnSelchanged4Old(HTREEITEM hItem),
чтобы сказать, что узел перестал быть выбранным
\param bIsSVActiveOld - старое состояние редактора сцены(был ли он активен)
\param bIsSVActiveNew - новое состояние редактора сцены(станет ли он активен)
\return \retval true - окна можно поменять местами
\retval false - окна нельзя поменять местами.
*/
bool Project::onChangingViews(bool bIsSVActiveOld, bool bIsSVActiveNew)
{
	if (!triggerEditorView_->OnVisibilityChanging(
		bIsSVActiveNew? ITriggerView::VCA_HIDE : ITriggerView::VCA_SHOW))
		return false;
	ITreeNode* node = treeLogic_->getFocusedNode();
	if (!node)
		return true;
	node->onStateChange(*treeLogic_, NS_FOCUSED, 0);
//	OnSelchanged4Old(hSelected);
	return true;
}

/*!
В менеджере масштаба устанавливается масштаб соответствующий текущему редактору
(сцены или триггеров).
Получает выбранный узел и вызывает Project::OnSelchanged4New(HTREEITEM hItem),
чтобы сказать, что узел стал выбранным(см. замечания примечания). Выполняет другие
действия связанные с активацией редакторов.
\remarks При переключении редакторов эмулируется потеря узлом состояния "Выбранный",
а затем получаение этого состояния. Это делается для того, чтобы неактивном редакторе
зачистить вещи связанные с выделением, а в активном с эмулировать выделеление узла
\param bIsSVActiveOld - старое состояние редактора сцены(был ли он активен)
\param bIsSVActiveNew - новое состояние редактора сцены(станет ли он активен)
*/
void Project::onChangeViews(bool bIsSVActiveOld, bool bIsSVActiveNew)
{

	if (bIsSVActiveNew)
		scaleManager_->SetScalable(dynamic_cast<IScalable*>(sceneView_));
	else
		scaleManager_->SetScalable(dynamic_cast<IScalable*>(triggerEditorView_));
	scaleManager_->UpdateScaleInfo();

	triggerEditorView_->OnVisibilityChange(bIsSVActiveNew? 
									ITriggerView::VCA_HIDE 
									: ITriggerView::VCA_SHOW);

	ITreeNode* node = treeLogic_->getFocusedNode();
	if (!node)
		return;
	//если сейчас будет виден редактор триггеров,
	//и список триггеров пуст, то создаем триггер
	if (!bIsSVActiveNew&&gameDispatcher_->trigger_chain_list().empty()) 
	{
		ITreeNode* gameDispNode = *getTreeLogic().getObjectNodes(gameDispatcher_.get());
		ITreeNode* triggersNode = 
			getTreeLogic().getSubfolder(gameDispNode, QD_NAMED_OBJECT_TRIGGER_CHAIN);
		CString strName((LPTSTR)IDS_NEW_TRIGGER_CHAIN_NAME);
		getProjectModifier().addTriggerChain(triggersNode, strName);
/*
		CString strName((LPTSTR)IDS_NEW_TRIGGER_CHAIN_NAME);
		CString strTriggersNodeName((LPTSTR)IDS_TRIGGERS_NODE);
		CTreeCtrl& tree = GetTreeCtrl();
		HTREEITEM hItem = FindTreeItemByText(tree, 
			tree.GetRootItem(), 
			strTriggersNodeName);
		if (hItem) 
		{
			AddTriggerChain(hItem, strName, false);
			tree.Expand(hItem, TVE_EXPAND);
		}
*/
	}

	if (bIsSVActiveNew) //т.е. показался редактор сцены(SceneView)
		canActivateTriggerView_ = false;

	treeLogic_->onItemStateChanged(node, 0, NS_FOCUSED);
	if (!viewActivator_.getSceneViewActive()) 
		setActiveTriggerIcon(triggerEditorView_->ShowedTrigger(), TRUE);
//	OnSelchanged4New(hSelected);
//	if (!bIsSVActiveNew) 
//		SetTgrIcon4All(TRUE);
	if (!canActivateTriggerView_)
		canActivateTriggerView_ = true;
}

//! Метод IViewMgr
BOOL Project::ShowSceneEditor()
{
	showSceneView();
	return TRUE;
}
//! Метод IViewMgr
BOOL Project::ShowTriggerEditor()
{
	showTEView();
	return TRUE;
}

/*!
Передает управление CMainFrame::ShowSceneView()	
*/
void Project::showSceneView()
{
//	m_ptrMainFrame->ShowSceneView();
	triggerEditorView_->deactivate();
	sceneView_->activate();
}

/*!
Передает управление CMainFrame::ShowTEView()	
*/
void Project::showTEView()
{
//	m_ptrMainFrame->ShowTEView();
	sceneView_->deactivate();
	triggerEditorView_->activate();
}

void Project::setInventoryToAllPersonages(LPCTSTR inventoryName)
{
	boost::rtl::crange<qdGameSceneList const> r = getGameDispatcher()->scene_list();
	for(; r; ++r)
	{
		boost::rtl::filtered_range<
			qdGameObjectList const,
			bool (*)(qdNamedObject const*)
		> f((*r)->object_list(), ptree_helpers::is_obj_moving);

		while(f){
			static_cast<qdGameObjectMoving*>(*f)->set_inventory_name(inventoryName);
			++f;
		}
	}
}

void Project::onEditTriggers()
{
	if (isSceneViewActive()) 
	{
		const qdTriggerChainList& lst = gameDispatcher_->trigger_chain_list();
		if (lst.empty()) 
			return;

		CString str = triggerEditorView_->GetLastEditedTrigger();
		qdTriggerChain* chain = NULL;
		boost::rtl::crange<qdTriggerChainList const> r(lst);
		for(; r; ++r)
		{
			if (!_tcscmp((*r)->name(), str)){
				chain = *r;
				break;
			}
		}
		if (!chain)
			chain = lst.front();
		if (chain)
			triggerEditorView_->ShowTriggers(&chain, 1);
		viewActivator_ = false;
	}
	else
		viewActivator_ = true;	
}


void Project::onUpdateCmdUIScalePlus(CCmdUI* pCmdUI)
{
	BOOL bEnabl = TRUE;
	if (isSceneViewActive())
		bEnabl = (treeLogic_->getCurrentSceneNode()!=NULL);
	else
		bEnabl = (triggerEditorView_->ShowedTrigger()!=NULL);

	pCmdUI->Enable(bEnabl);
}

void Project::onUpdateCmdUIScaleMinus(CCmdUI* pCmdUI)
{
	BOOL bEnabl = TRUE;
	if (isSceneViewActive())
		bEnabl = (treeLogic_->getCurrentSceneNode()!=NULL);
	else
		bEnabl = (triggerEditorView_->ShowedTrigger()!=NULL);

	pCmdUI->Enable(bEnabl);
}

bool Project::selectInterfaceScreen(LPCTSTR screenName)
{
	qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
	if (!pd->selected_screen()
		||_tcscmp(pd->selected_screen()->name(), screenName))
	{
		if(pd->select_screen(screenName))
		{
			sceneView_->AdjustBitmapSize();
			return true;
		}
	}
	return false;
}

void Project::OnCrtAnimatedObj()
{
	ITreeNode* node = getTreeLogic().getCurrentSceneNode();
	ASSERT(node);

	AddAnimStateWPg pg;
	pg.set_data(gameDispatcher_.get(), 
		static_cast<qdGameScene*>(node->getContent().gameObject));
	pg.set_obj_type(QD_NAMED_OBJECT_ANIMATED_OBJ);
	pg.set_caption(CString((LPTSTR)IDS_PROPPAGE_CAPTION_ADD_DYN_OBJ));

#ifdef _AddAnimState_USE_PROP_PAGE
	CPropertyPage* pPage = &pg;
	WizardSheet ws(&pPage, 1, 0);

	ws.DoModal();
#else	
	pg.DoModal();
#endif
	if(pg.GetModified())
	{
		node->reload(&getTreeNodeFactory(), getTreeLogic(), NULL, NULL);
		SetModified(true);
		sceneView_->Redraw(RM_REDRAW_CONTENT);
	}

}

void Project::OnCrtMovingObj()
{
	TreeLogic& logic = getTreeLogic();
	ITreeNode* focused = logic.getFocusedNode();
	ITreeNode* curSceneNode = logic.getCurrentSceneNode();

	qdGameDispatcherBase* ptrObjDisp = NULL;
	if (curSceneNode) {
		ptrObjDisp = static_cast<qdGameScene*>(curSceneNode->getContent().gameObject);
	}
	else
	{
		ptrObjDisp = gameDispatcher_.get();
		curSceneNode = *logic.getObjectNodes(ptrObjDisp);
	}

	ITreeNode* folder = logic.getSubfolder(curSceneNode, QD_NAMED_OBJECT_MOVING_OBJ);

	AddAnimStateWPg pg;
	pg.set_data(gameDispatcher_.get(), ptrObjDisp);
	pg.set_obj_type(QD_NAMED_OBJECT_MOVING_OBJ);
	pg.set_use_movement();
	pg.set_caption(CString((LPTSTR)IDS_PROPPAGE_CAPTION_ADD_MOV_OBJ));

#ifdef _AddAnimState_USE_PROP_PAGE
	CPropertyPage* pPage = &pg;
	WizardSheet ws(&pPage, 1, 0);

	ws.DoModal();
#else	
	pg.DoModal();
#endif

	if(pg.GetModified())
	{
		CWaitCursor __wait__;
		curSceneNode->reload(&getTreeNodeFactory(), logic, NULL, NULL);
		SetModified(true);
		sceneView_->Redraw(RM_REDRAW_CONTENT);
	}
}

void Project::OnCrtStaticObj()
{
	ITreeNode* curScene = getTreeLogic().getCurrentSceneNode();
	if (!curScene)
		return;

	CString str;
	str.LoadString(IDS_NEW_STATIC_OBJ_NAME);

	qdGameScene* scene = static_cast<qdGameScene*>(curScene->getContent().gameObject);

	qdGameObjectStatic* ptrObj = getProjectModifier().addStatic(scene, str, true);
	if(ptrObj)
	{
		qdCamera* pcam = scene->get_camera();
		Vect2s scr_cntr;
		scr_cntr.x = pcam->get_scr_center_x();
		scr_cntr.y = pcam->get_scr_center_y();
		float fD = DEF_BACKGROUND_DEPTH;

		qdGameObject* farthest = 
			ptree_helpers::GetFarthestGO(scene, ptree_helpers::is_obj_static);

		if (farthest) {
			Vect3f v = pcam->global2camera_coord(farthest->R());
			fD = v.z - 1.f;
		}
		scr_cntr = pcam->scr2rscr(scr_cntr);
		ptrObj->set_pos(pcam->rscr2global(scr_cntr, fD));

		ShowProperties(ptrObj);
		if(!ptrObj->get_sprite()->file()
			||!_tcslen(ptrObj->get_sprite()->file()))
		{
			DeleteObject(ptrObj,false);
		}
		else
			SetModified(true);
	}

}

void Project::OnCreateStatics()
{
	ITreeNode* curScene = getTreeLogic().getCurrentSceneNode();
	if (!curScene)
		return;
	if (getProjectModifier().addStatics(curScene))
		SetModified(true);
}


void Project::OnWaveToLocal()
{
	WaveToLoc obj;
	obj.run(getTreeLogic().getBaseWnd(), *gameDispatcher_, false);
}

void Project::OnLocalToWave()
{
	WaveToLoc obj;
	obj.run(getTreeLogic().getBaseWnd(), *gameDispatcher_, true);
}


void Project::OnUpdateCrtMovingObj(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(viewActivator_.getSceneViewActive()&&gameDispatcher_);	
}

void Project::OnUpdateNetProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(getTreeLogic().getCurrentSceneNode() != NULL
		&&viewActivator_.getSceneViewActive());	
}

void Project::OnUpdateCrtAnimObjs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(getTreeLogic().getCurrentSceneNode() != NULL
		&&viewActivator_.getSceneViewActive());
}

void Project::OnUpdateEditAnimation(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(getTreeLogic().getCurrentSceneNode() != NULL
		&&viewActivator_.getSceneViewActive());
}

void Project::OnUpdateEditSound(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(getTreeLogic().getCurrentSceneNode() != NULL
		&&viewActivator_.getSceneViewActive());
}

void Project::OnUpdateCreateStatics(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(getTreeLogic().getCurrentSceneNode() != NULL
		&&viewActivator_.getSceneViewActive()&&getGameDispatcher());
}

void Project::OnUpdateCreateBack(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(getTreeLogic().getCurrentSceneNode() != NULL
		&&viewActivator_.getSceneViewActive());
}

void Project::OnUpdatePlayStopVideo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(viewActivator_.getSceneViewActive());
}

void Project::OnUpdateStopVideo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(viewActivator_.getSceneViewActive());
}

void Project::OnUpdateWaveToLocal(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(gameDispatcher_ != NULL);
}

void Project::OnUpdateLocalToWave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(gameDispatcher_ != NULL);
}

void Project::OnCrtScene()
{
	qdGameScene* scene = AddScene(CString(), true);
	if (scene)
	{
		//узел только что созданной сцены будет выбран в дереве
		ITreeNode* node = *getTreeLogic().getObjectNodes(scene);
		assert(node);

		getProjectModifier().addBackground(node, scene);
	}
}

void Project::OnAddInterfBackground()
{
	ITreeNode* screenNode = getTreeLogic().getFocusedNode();
	if (!screenNode)
		return;
	qdInterfaceScreen* ps = 
		static_cast<qdInterfaceScreen*>(screenNode->getContent().interfaceObject);
	AddIntrfBackgrndWizardShow wiz;
	if (wiz.show(getTreeLogic().getBaseWnd(), ps))
		SetModified(true);
}

void Project::OnAddInterfBtnState()
{
	ITreeNode* buttonNode = getTreeLogic().getFocusedNode();
	if (!buttonNode)
		return;
	qdInterfaceButton * button= 
		static_cast<qdInterfaceButton*>(buttonNode->getContent().interfaceObject);

	AddInterfBtnStateWizShow wiz;
	if (wiz.show(getTreeLogic().getBaseWnd(), button))
	{
		SetModified(true);
		sceneView_->Redraw(RM_REDRAW_CONTENT);
	}
}

void Project::OnAddInterfSave()
{
	ITreeNode* screenNode = getTreeLogic().getFocusedNode();
	if (!screenNode)
		return;
	qdInterfaceScreen* ps = 
		static_cast<qdInterfaceScreen*>(screenNode->getContent().interfaceObject);

	AddInterfSaveWizShow wiz;
	if (wiz.show(getTreeLogic().getBaseWnd(), ps))
	{
		SetModified(true);
		sceneView_->Redraw(RM_REDRAW_CONTENT);
	}
}

void Project::OnCreateIscreen()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	CString str((LPTSTR)IDS_NEW_INTEF_SCREEN_NAME);
	if (AskUserForName(str))
	{
		qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
		qdInterfaceScreen* pscr = getProjectModifier().addInterfaceScreen(focused, str);
		if (pscr)
		{
			pd->select_screen(pscr->name());
			SetModified(true);
			AddIntrfBackgrndWizardShow wiz;
			if (wiz.show(getTreeLogic().getBaseWnd(), pscr))
				sceneView_->Redraw(RM_REDRAW_CONTENT);
		}
	}
}

void Project::OnAddInterfSlider()
{
	ITreeNode* screenNode = getTreeLogic().getFocusedNode();
	if (!screenNode)
		return;
	qdInterfaceScreen* ps = 
		static_cast<qdInterfaceScreen*>(screenNode->getContent().interfaceObject);

	AddInterfSldrWizardShow wiz;
	if (wiz.show(getTreeLogic().getBaseWnd(), ps))
	{
		SetModified(true);
		sceneView_->Redraw(RM_REDRAW_CONTENT);
	}
}

void Project::OnAddInterfTextWindow()
{
	ITreeNode* screenNode = getTreeLogic().getFocusedNode();
	if (!screenNode)
		return;
	qdInterfaceScreen* ps = 
		static_cast<qdInterfaceScreen*>(screenNode->getContent().interfaceObject);

	CString name = AskUserForName(CString((LPTSTR)IDS_DEFAULT_TEXT_WINDOW_NAME));
	if (name.IsEmpty())
		return;


	if (!AddInterfTextWindow(name, ps))
		return;
	SetModified(true);
	sceneView_->Redraw(RM_REDRAW_CONTENT);
}

void Project::OnAddIntrfButton()
{
	ITreeNode* screenNode = getTreeLogic().getFocusedNode();
	if (!screenNode)
		return;
	qdInterfaceScreen* ps = 
		static_cast<qdInterfaceScreen*>(screenNode->getContent().interfaceObject);

	AddInterfBtnWizShow wiz;
	if (wiz.show(getTreeLogic().getBaseWnd(), ps))
	{
		SetModified(true);
		sceneView_->Redraw(RM_REDRAW_CONTENT);
	}
}

void Project::OnAddInterfCounter()
{
	ITreeNode* screenNode = getTreeLogic().getFocusedNode();
	if (!screenNode)
		return;
	qdInterfaceScreen* ps = 
		static_cast<qdInterfaceScreen*>(screenNode->getContent().interfaceObject);

	CString name = AskUserForName(CString((LPTSTR)IDS_DEFAULT_INTERF_COUNTER_NAME));
	if (name.IsEmpty())
		return;

	if (!AddInterfCounter(name, ps))
		return;
	SetModified(true);
	sceneView_->Redraw(RM_REDRAW_CONTENT);
}

void Project::OnAddGameEnd()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	AddGameEndWizardShower show;
	if (show.show(getTreeLogic().getBaseWnd(), gameDispatcher_.get(), focused))
		SetModified(true);
}

void Project::OnAddFont()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	getProjectModifier().addFont(focused);
}

void Project::OnCreateFont()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	getProjectModifier().createFont(focused);
}

void Project::OnAddVideo()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;

	if(getProjectModifier().addVideos(focused))
	{
		SetModified(true);
		getTreeLogic().expand(focused, TreeLogic::EC_EXPAND);
	}
}

void Project::OnCreateMinigame()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	if (getProjectModifier().addMinigame(focused))
	{
		getTreeLogic().expand(focused, TreeLogic::EC_EXPAND);
		SetModified(true);
		sceneView_->Redraw(RM_REDRAW_CONTENT);
	}
}

void Project::OnCrtNewInventory()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	if (getProjectModifier().addInventory(focused))
	{
		SetModified(true);
		getTreeLogic().expand(focused, TreeLogic::EC_EXPAND);
	}
}

void Project::OnCreateCounter()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;

	ITreeNode* dispNode = *getTreeLogic().getObjectNodes(getGameDispatcher());
	assert(dispNode);
	ITreeNode* counters = getTreeLogic().getSubfolder(dispNode, QD_NAMED_OBJECT_COUNTER);
	assert(counters);
	if (getProjectModifier().addCounter(counters))
	{
		SetModified(true);
		getTreeLogic().expand(counters, TreeLogic::EC_EXPAND);
	}
}

void Project::OnCreateState()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;

	bool bUseMovement = false;

	ITreeNode* objectNode = focused;
	if(TreeNodeTraits::isFolderNode(focused))
	{	//т.е. ткнули на состояния для персонажа
		bUseMovement = TreeNodeTraits::isWalkStateCatalog(focused);
		objectNode = getTreeLogic().getParentNode(focused);
	}

	qdGameObjectAnimated* ptrObj = 
		static_cast<qdGameObjectAnimated*>(objectNode->getContent().gameObject);

	AddAnimStateWPg aas;
	aas.set_data(getGameDispatcher(),
		static_cast<qdGameDispatcherBase*>(ptrObj->owner()));

	aas.set_obj(ptrObj);
#ifdef _AddAnimState_USE_PROP_PAGE
	aas.set_place(PAGE_PLACE_FIRST_LAST);
#endif
	if(ptree_helpers::is_obj_animated(ptrObj)){
		aas.set_caption(CString((LPTSTR)IDS_PROPPAGE_CAPTION_ADD_STATES_DYN_OBJ));
	}
	else if (ptree_helpers::is_obj_moving(ptrObj)) {
		aas.set_caption(CString((LPTSTR)IDS_PROPPAGE_CAPTION_ADD_STATES_MOV_OBJ));
	}
	else if (ptree_helpers::is_obj_mouse(ptrObj))
		aas.set_caption(CString((LPTSTR)IDS_PROPPAGE_CAPTION_ADD_STATES_MOUSE_OBJ));

	if(bUseMovement)
		aas.set_use_movement();

#ifdef _AddAnimState_USE_PROP_PAGE
	CPropertyPage* pPage = &aas;
	WizardSheet ws(&pPage, 1, 0);
	ws.DoModal();
#else	
	aas.DoModal();
#endif
	if(aas.GetModified())
	{
		CWaitCursor wait;
		if (ObjectTraits::isGlobal(*ptrObj))
		{
			(*getTreeLogic().getObjectNodes(getGameDispatcher()))->
				reload(&getTreeNodeFactory(), getTreeLogic(), NULL, NULL);
		}
		else
			objectNode->reload(&getTreeNodeFactory(), getTreeLogic(), NULL, NULL);

		SetModified(true);
	}
}

void Project::OnCrtTrigger()
{
	CString strName((LPTSTR)IDS_NEW_TRIGGER_CHAIN_NAME);
	if(AskUserForName(strName))
	{
		ITreeNode* gameDispNode = *getTreeLogic().getObjectNodes(getGameDispatcher());
		ITreeNode* triggers = 
			getTreeLogic().getSubfolder(gameDispNode, QD_NAMED_OBJECT_TRIGGER_CHAIN);
		ASSERT(triggers);
		qdTriggerChain* chain = getProjectModifier().addTriggerChain(triggers, strName);
		if (chain)
		{
			TreeLogic& logic = getTreeLogic();
			logic.expand(triggers, TreeLogic::EC_EXPAND);
			logic.showNode(*logic.getObjectNodes(chain));
			SetModified(true);
		}
	}
}

void Project::OnCrtAnimation()
{
	//первым делом находим диспетчер, в который и засунем новую анимацию
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	ASSERT(focused);
	showAnimationCatalogDlg(focused, GOPA_CREATE, false);
}

/*!
Достаёт из узла диспетчер. Его анимация будет загружена.
Если bUniqueList установлен в true, то список будет дополнен содержимым глобального
каталога анимации. Анимации из глобального каталога с именами, которые встречаются 
в каталоге сцены, отображаться не будут.
Настравивает объект класс каталога анимации(AnimCatalogDlg).
Отображает каталог (AnimCatalogDlg::DoModal()).
Если изменения были сделаны, то устанавливается флаг, сигнализирующий, что
проект был изменен(SetModifed())
\param hItem		- узел сцены или глобального диспетчера
\param eAction		- какое действие выполнить после появления диалога с 
содержимым каталога. См.: AnimCatalogDlgAction
\param bUniqueList	- будет список уникальным содержимым каталога сцены и глобального
каталога. Или же будут отображено только содержимое катлога
сцены
*/
void Project::showAnimationCatalogDlg(ITreeNode* focused, int eAction, bool bUniqueList)
{
	qdGameDispatcherBase* ptrGB = 
		static_cast<qdGameDispatcherBase*>(focused->getContent().gameObject);

	AnimCatalogDlg pg;
	pg.set_data(gameDispatcher_.get(), ptrGB);
	pg.set_action(eAction);
	pg.set_unique(bUniqueList);

	if(ptrGB->named_object_type() == QD_NAMED_OBJECT_SCENE)
	{
		CString strCap;
		if(bUniqueList)
		{
			strCap.Format(IDS_CAP_ANIMS_UNIQ_LIST_DLG, ptrGB->name());
			pg.set_caption(strCap);
		}
		else{
			strCap.Format(IDS_CAP_ANIMS_LIST_DLG, ptrGB->name());
			pg.set_caption(strCap);
		}
	}


	pg.DoModal();

	if(pg.is_tree_changed())
		SetModified(true);
}

void Project::OnEditStateAnimation()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	ASSERT(focused);

	//для состояний отличных от статических 
	//этот пункт должен быть недоступен
	qdNamedObject* pob = static_cast<qdNamedObject*>(focused->getContent().gameObject);
	LPCTSTR lpszFileName = AnimationFileNameExtractor::getFileName(pob);
	if (!lpszFileName) 
	{
		AfxMessageBox(IDS_ERR_NO_ANIMATION_FOUND);
		return;
	}
	CWaitCursor __wait;
	getTreeLogic().getBaseWnd()->EnableWindow(FALSE);
	::EditAnimation(*getTreeLogic().getBaseWnd(), lpszFileName);
	getTreeLogic().getBaseWnd()->EnableWindow(TRUE);
}

void Project::OnAnimationList()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	ASSERT(focused);

	ITreeNode* dispNode = getTreeLogic().isNodeUnderScene(focused);
	if (!dispNode)
		dispNode = *getTreeLogic().getObjectNodes(gameDispatcher_.get());

	showAnimationCatalogDlg(dispNode, GOPA_SHOW, true);
}

/*!
 	Достаёт из узла диспетчер. Его звуки будут загружены.
	Если bUniqueList установлен в true, то список будет дополнен содержимым глобального
	каталога звуков. Звуки из глобального каталога с именами, которые встречаются 
	в каталоге сцены, отображаться не будут.
	Настравивает объект класс каталога звуков(SoundCatalogDlg).
	Отображает каталог (SoundCatalogDlg::DoModal()).
	Если изменения были сделаны, то устанавливается флаг, сигнализирующий, что
	проект был изменен(SetProjChange())
	\param hItem		- узел сцены или глобального диспетчера
	\param bUniqueList	- будет список уникальным содержимым каталога сцены и глобального
							каталога. Или же будут отображено только содержимое катлога
							сцены
*/
void Project::showSoundCatalogDlg(ITreeNode* dispNode, bool bUniqueList)
{
	qdGameDispatcherBase* ptrGB = 
		static_cast<qdGameDispatcherBase*>(dispNode->getContent().gameObject);
	
	SoundCatalogDlg dlg;
	dlg.set_data(gameDispatcher_.get(), ptrGB);
	
	if(ptrGB->named_object_type() == QD_NAMED_OBJECT_SCENE)
	{
		CString strCap;
		if(bUniqueList)
		{
			strCap.Format(IDS_CAP_SOUNDS_UNIQ_LIST_DLG, ptrGB->name());
			dlg.set_caption(strCap);
		}
		else
		{
			strCap.Format(IDS_CAP_SOUNDS_LIST_DLG, ptrGB->name());
			dlg.set_caption(strCap);
		}
	}
	
	dlg.set_unique(bUniqueList);
	
	dlg.DoModal();
	
	if(dlg.is_proj_changed())
		SetModified(true);
	
}

void Project::OnSoundList()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	ASSERT(focused);

	ITreeNode* dispNode = getTreeLogic().isNodeUnderScene(focused);
	if (!dispNode)
		dispNode = *getTreeLogic().getObjectNodes(gameDispatcher_.get());
	showSoundCatalogDlg(dispNode, true);
}

void Project::OnEditSound()
{
	if(getTreeLogic().getCurrentSceneNode())
		showSoundCatalogDlg(getTreeLogic().getCurrentSceneNode(), false);
}

void Project::OnMenuMovingsStops()
{

	ITreeNode* focused = getTreeLogic().getFocusedNode();
	ASSERT(focused);

	qdGameDispatcherBase * ptr = 
		static_cast<qdGameDispatcherBase*>(focused->getContent().gameObject);
	ASSERT(ptr->named_object_type() == QD_NAMED_OBJECT_SCENE
		||ptr->named_object_type() == QD_NAMED_OBJECT_DISPATCHER);

	MoveStopDlg dlg;
	dlg.set_data(gameDispatcher_.get(), ptr);
	dlg.DoModal();
	if(dlg.has_change())
		SetModified(true);
}

void Project::OnGameSettings()
{
	ITreeNode* dispNode = *getTreeLogic().getObjectNodes(gameDispatcher_.get());
	if (!dispNode)
		return;
	dispNode->showProperties(getTreeLogic(), 0);
}

void Project::OnManageCds()
{
	if (showCDDiskManager(gameDispatcher_.get()))
		SetModified(true);
}

void Project::OnStatDependencies()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	CWaitCursor wait;
	qdNamedObject* obj = static_cast<qdNamedObject*>(focused->getContent().gameObject);
	statObjectDependencies(obj);
}

bool Project::statObjectDependencies(qdNamedObject const* object)
{
	std::map<qdNamedObject const*, NamedObjectList> statistic;
	ptrObjectsDependencies_ = &statistic;
	StatObjectDependencies::run(gameDispatcher_.get(), object, ptrObjectsDependencies_);
	if (!statistic.empty())
	{
		if (filterDepedenciesBound(object) == FilterObjectDependencies::ACROSS_OBJECT)
		{
			fillObjectList();
			objectList_->showObjectInList(&qdNamedObjectReference(object));
			objectList_->show();
			return true;
		}
	}
	return false;
}

//! Проверяет выходят ли зависимости объектов сцены за пределы этой сцены
FilterObjectDependencies::eDependenciesBound Project::filterDepedenciesBound(
													qdNamedObject const* object)
{
	return FilterObjectDependencies::run(object, ptrObjectsDependencies_)
										? FilterObjectDependencies::ACROSS_OBJECT 
										: FilterObjectDependencies::IN_OBJECT;
}


void Project::fillObjectList()
{
	if (!ptrObjectsDependencies_)
		return;
	getObjectList()->clearList();
	boost::rtl::crange<std::map<qdNamedObject const*, 
		NamedObjectList> > stats(*ptrObjectsDependencies_);
	for(; stats; ++stats)
	{
		boost::rtl::crange<NamedObjectList const> list((*stats).second);
		if (list)
		{
			qdNamedObjectReference objRef((*stats).first);
			HITEM objNode = getObjectList()->addObjectToList(&objRef);

			for(; list; ++list)
			{
				qdNamedObjectReference ref(*list);
				getObjectList()->addObjectToList(&ref, objNode);
			}
		}
	}
}

void Project::OnAddCellSet()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;

	qdInventory* pinv = static_cast<qdInventory*>(focused->getContent().gameObject);

	if (getProjectModifier().addCellSet(*pinv))
		SetModified(true);
	sceneView_->Redraw(RM_REDRAW_CONTENT);
}

void Project::OnRemoveCellSet()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;

	qdInventory* pinv = static_cast<qdInventory*>(focused->getContent().gameObject);
	if (DeleteInventoryCellset(pinv, pinv->selected_cellset_num()))
		sceneView_->Redraw(RM_REDRAW_CONTENT);
}

void Project::OnAddMusicTrack()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	ITreeNode* sceneNode = getTreeLogic().getCurrentSceneNode();
	assert(sceneNode);

	qdGameScene* pscene = static_cast<qdGameScene*>(sceneNode->getContent().gameObject);

	ASSERT(pscene);
	if (getProjectModifier().addMusicTrack(pscene, focused))
	{
		getTreeLogic().expand(focused, TreeLogic::EC_EXPAND);
		SetModified(true);
	}
}

void Project::OnMusicTrackCycled()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdMusicTrack* ptrack = static_cast<qdMusicTrack*>(focused->getContent().gameObject);
	ptrack->toggle_cycle(!ptrack->is_cycled());
	SetModified(true);
}

void Project::OnCreateMask()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;

	qdGameObject* ptrGO = static_cast<qdGameObject*>(focused->getContent().gameObject);

	sceneView_->SetViewState(VS_EDIT_MASK);
	sceneView_->SetSubTools(SSTE_CREATE_MASK);
	if (!sceneView_->OnCreateStateMask(ptrGO))
	{
		sceneView_->SetViewState(VS_EDIT_SCENE);
		sceneView_->SetSubTools(SSTE_NONE);
	}
}

void Project::OnCrtAnimObjByMask()
{
	sceneView_->SetViewState(VS_EDIT_MASK);
	sceneView_->SetSubTools(SSTE_CREATE_MASK);
	if (!sceneView_->OnCreateStateMask(reinterpret_cast<qdGameObject*>(-1)))
	{
		sceneView_->SetViewState(VS_EDIT_SCENE);
		sceneView_->SetSubTools(SSTE_NONE);
	}
}

void Project::OnCrtGridZone()
{
	sceneView_->SetViewState(VS_EDIT_NET);
	sceneView_->SetSubTools(SSTE_CREATE_ZONE);

	qdCamera* ptrCam = gameDispatcher_->get_active_scene()->get_camera();

	if (ptrCam->get_redraw_mode() != QDCAM_GRID_NONE) {
		ptrCam->set_redraw_mode(QDCAM_GRID_ABOVE);
	}

	sceneView_->OnCreateGridZone();
	sceneView_->Redraw(RM_REDRAW_CONTENT);
}


bool Project::OnEndStateMaskCreate(qdGameObjectStateMask* state)
{
	qdGameObjectAnimated* pao = NULL;
	if (state->owner())
	{
		//значит маску создавали кликнув в меню
		//анимированного объекта
		pao = static_cast<qdGameObjectAnimated*>(state->owner());
		if (pao == reinterpret_cast<qdGameObjectAnimated*>(-1))
		{
			pao = getProjectModifier().addAnimated(
					gameDispatcher_->get_active_scene(), 
					CString(), false);
			if (!pao)
				return false;
		}
	}
	else
	{
		std::pair<bool, qdGameObject*> res =
			AskForObject(gameDispatcher_->get_active_scene(), 
							QD_NAMED_OBJECT_ANIMATED_OBJ,  true);
		if (!res.first)
			return false;
		if (!res.second) {
			pao = getProjectModifier().addAnimated(
							gameDispatcher_->get_active_scene(), CString(), false);
			if (!pao) 
				return false;
		}
		else
			pao = static_cast<qdGameObjectAnimated*>(res.second);
	}	


	ITreeNode* stateNode = getProjectModifier().addMaskState(pao, state);
	if(stateNode)
	{
		SetModified(true);
		getTreeLogic().showNode(stateNode);
		return true;
	}
	return false;
}

bool Project::OnCompleteCreateZone(qdGridZone* zone)
{
	ITreeNode* scene = getTreeLogic().getCurrentSceneNode();
	ITreeNode* zoneNode = getProjectModifier().addGridZone(scene, zone);
	if (!zoneNode)
		return false;
	getTreeLogic().showNode(zoneNode);
	SetModified(true);
	return true;
}



bool Project::safeSaveScript(qdGameDispatcher* pdisp, LPCTSTR lpszFileName)
{
	CString str(lpszFileName);
	str += _T(".temp_save");
#ifndef _DEBUG
	try{
#endif // _DEBUG
		CString strMsg((LPTSTR)IDS_COLLECT_EXTERNAL_FILES);
		static_cast<CFrameWnd*>(AfxGetMainWnd())->SetMessageText(strMsg);

		CString collect_dir;
		collect_dir.LoadString(IDS_RESOURCE_CATALOG);				
		pdisp->collect_all_external_files(collect_dir);
		
		strMsg.LoadString(IDS_SCRIPT_SAVING);
		static_cast<CFrameWnd*>(AfxGetMainWnd())->SetMessageText(strMsg);
		if (pdisp->save_script(str))
		{
			DeleteFile(lpszFileName);
			MoveFile(str, lpszFileName);
			static_cast<CFrameWnd*>(AfxGetMainWnd())->SetMessageText("");
			return true;
		}
		static_cast<CFrameWnd*>(AfxGetMainWnd())->SetMessageText("");
#ifndef _DEBUG

	}
	catch (...) {
	}
#endif   //_DEBUG
	return false;
}

void Project::readProjSettings()
{
	IProjectSettings* pps = GetMyApp()->GetProjectSettings();

	readNetDrawMode(pps);

	triggerEditorView_->ReadIniSettings(pps);
	CString tree_struct_file = makeTreeStructFileName(GetMyApp()->GetProjIniFileName());
	getTreeLogic().loadFullStruct(tree_struct_file);
	readActiveView(pps);
	readShowTriggerElement(pps);
//	readSceneSubNodesState(pps);
//	readInterfaceScreensState(pps);
//	getTreeLogic().readIni(pps);
	triggerEditorView_->OnEndIniReading();
}

void Project::saveProjSettings()
{
	if (!gameDispatcher_ || !GetMyApp())
		return;
	IProjectSettings* pps = GetMyApp()->GetProjectSettings();
//	getTreeLogic().saveIni(pps);
	saveNetDrawMode(pps);
	saveShowTriggerElement(pps);
	triggerEditorView_->SaveIniSettings(pps);
	saveActiveView(pps);
	CString tree_struct_file = makeTreeStructFileName(GetMyApp()->GetProjIniFileName());
	getTreeLogic().saveFullStruct(tree_struct_file);
//	saveSceneSubNodesState(pps);
//	saveInterfaceScreensState(pps);
	pps->Save();
}

void Project::saveActiveView(IProjectSettings* pps)
{
	pps->WriteIntSetting(CString((LPTSTR)IDS_INI_SEC_NAME_APP),
		CString((LPTSTR)IDS_INI_KEY_NAME_ACTIVE_VIEW),
		static_cast<int>(viewActivator_.getSceneViewActive()));
}

/* Функции этого кода теперь выполняет TreeLogic::saveFullStruct(...)
void Project::saveSceneSubNodesState(IProjectSettings* pps)
{
	TreeLogic& logic = getTreeLogic();
	boost::rtl::crange<qdGameSceneList const> r(gameDispatcher_->scene_list());
	for(; r; ++r)
	{
		ITreeNode* sceneNode = *logic.getObjectNodes(*r);

		ITreeNode* node = logic.getSubfolder(sceneNode, QD_NAMED_OBJECT_MUSIC_TRACK);
		pps->WriteIntSetting((*r)->name(), CString((LPTSTR)IDS_MUSIC_TRACKS_NODE),
			logic.isExpanded(node));

		node = logic.getSubfolder(sceneNode, QD_NAMED_OBJECT_GRID_ZONE);
		pps->WriteIntSetting((*r)->name(), CString((LPTSTR)IDS_NET_NODE_NAME),
			logic.isExpanded(node));

		node = logic.getSubfolder(sceneNode, QD_NAMED_OBJECT_STATIC_OBJ);
		pps->WriteIntSetting((*r)->name(), CString((LPTSTR)IDS_STATIC_OBJ_CATALOG),
			logic.isExpanded(node));

		node = logic.getSubfolder(sceneNode, QD_NAMED_OBJECT_ANIMATED_OBJ);
		pps->WriteIntSetting((*r)->name(), CString((LPTSTR)IDS_ANIMATED_OBJS_NODE),
			logic.isExpanded(node));

		node = logic.getSubfolder(sceneNode, QD_NAMED_OBJECT_MOVING_OBJ);
		pps->WriteIntSetting((*r)->name(), CString((LPTSTR)IDS_MOVING_OBJS_NODE),
			logic.isExpanded(node));
	}
}

void Project::saveInterfaceScreensState(IProjectSettings* pps)
{
	TreeLogic& logic = getTreeLogic();
	ITreeNode* interfaceNode = 
		*logic.getObjectNodes(qdInterfaceDispatcher::get_dispatcher());
	ITreeNode* child = logic.getChildNode(interfaceNode);
	CString interfaceSection;
	interfaceSection.LoadString(IDS_INTERFACE_NODE);
	ASSERT(child);
	do {
		pps->WriteIntSetting(interfaceSection, logic.getNodeText(child),
			logic.isExpanded(child));
	} while(child = logic.getNextSiblingNode(child));
}
*/

void Project::saveNetDrawMode(IProjectSettings* pps)
{
	if (!gameDispatcher_)
		return;
	boost::rtl::crange<qdGameSceneList const> gsl = gameDispatcher_->scene_list();
	CString strKeyName((LPTSTR)IDS_INI_KEY_NAME_NET_DRAW_MODE);
	for(;gsl; ++gsl)
	{
		pps->WriteIntSetting((*gsl)->name(), strKeyName,
			(*gsl)->get_camera()->get_redraw_mode());
	}
}

//! Сохраняет в настройках проекта флаг showTirgger_
void Project::saveShowTriggerElement(IProjectSettings* pps)
{
	pps->WriteIntSetting(CString((LPTSTR)IDS_INI_SEC_NAME_APP),
		CString((LPTSTR)IDS_INI_KEY_NAME_SHOW_TRIGGER),
		static_cast<int>(showTriggerElement_));
}

/*!
	\param ptrScene		- Сцена, для сетки которой устанавливается режим
	\param lpszItemName	- Имя настройки в ини файле
	\param pps			- Указатель на менеджер настроек проекта
*/
void Project::adjustNetDrawMode(qdGameScene* ptrScene, 
									  LPCTSTR lpszItemName, 
									  IProjectSettings* pps)
{
	int iMode = pps->GetIntSetting(ptrScene->name(), lpszItemName, 1);
	ptrScene->get_camera()->set_redraw_mode(iMode);

	ITreeNode* sceneNode = *getTreeLogic().getObjectNodes(ptrScene);
	if (!sceneNode)
		return;
	ITreeNode* netNode = getTreeLogic().getSubfolder(sceneNode,QD_NAMED_OBJECT_GRID_ZONE);
	ASSERT(netNode);
	if(iMode == static_cast<int>(QDCAM_GRID_NONE))
		getTreeLogic().setNodeCheck(netNode, false);
	else
		getTreeLogic().setNodeCheck(netNode, true);
}

/*!
	Пробегает по всем сценам и вызывает AdjustNetDrawMode() для каждой
	из них
	\param pps - Указатель на менеджер настроек проекта
*/
void Project::readNetDrawMode(IProjectSettings* pps)
{
	CString strKeyName((LPTSTR)IDS_INI_KEY_NAME_NET_DRAW_MODE);

	const qdGameSceneList& gsl = gameDispatcher_->scene_list();
	qdGameSceneList::const_iterator _itr = gsl.begin(), _end = gsl.end();
	for(;_itr != _end; ++_itr)
	{
		adjustNetDrawMode(*_itr, strKeyName, pps);
	}
}

/*!
	\param pps - Указатель на менеджер настроек проекта
*/
void Project::readActiveView(IProjectSettings* pps)
{
	bool const val = static_cast<bool>(
		pps->GetIntSetting(CString((LPTSTR)IDS_INI_SEC_NAME_APP),
		CString((LPTSTR)IDS_INI_KEY_NAME_ACTIVE_VIEW), 1));
	if (!val) {
		if (!gameDispatcher_->trigger_chain_list().empty()) 
		{
			CString const str = triggerEditorView_->GetLastEditedTrigger();
			qdTriggerChainList const& lst = gameDispatcher_->trigger_chain_list();
			//! Находим по имени, если не нашли, то берем первую
			qdTriggerChain* chainToShow = 
				ptree_helpers::findTriggerChainByName(lst, str);
			if (!chainToShow)
				chainToShow = lst.front();
			triggerEditorView_->ShowTriggers(&chainToShow, 1);
		}
		viewActivator_ = false;
		setActiveTriggerIcon(triggerEditorView_->ShowedTrigger(), TRUE);
	}
	else
		viewActivator_ = true;
}

//! Считывает из настроек проекта флаг showTirgger_
void Project::readShowTriggerElement(IProjectSettings* pps)
{
	showTriggerElement_ = static_cast<bool>(
		pps->GetIntSetting(CString((LPTSTR)IDS_INI_SEC_NAME_APP),
		CString((LPTSTR)IDS_INI_KEY_NAME_SHOW_TRIGGER), 1));
}

/* Функции этого кода теперь выполняет TreeLogic::loadFullStruct(...)
//! Считывает состояние для папок объектов
void Project::readSceneSubNodesState(IProjectSettings* pps)
{
	TreeLogic& logic = getTreeLogic();
	boost::rtl::crange<qdGameSceneList const> r(gameDispatcher_->scene_list());
	for(; r; ++r)
	{
		ITreeNode* sceneNode = *logic.getObjectNodes(*r);

		ITreeNode* node = logic.getSubfolder(sceneNode, QD_NAMED_OBJECT_MUSIC_TRACK);
		int expand = 
			pps->GetIntSetting((*r)->name(), CString((LPTSTR)IDS_MUSIC_TRACKS_NODE), 0);
		logic.expand(node, static_cast<TreeLogic::eExpandCommand>(expand));

		node = logic.getSubfolder(sceneNode, QD_NAMED_OBJECT_GRID_ZONE);
		expand = pps->GetIntSetting((*r)->name(), CString((LPTSTR)IDS_NET_NODE_NAME), 0);
		logic.expand(node, static_cast<TreeLogic::eExpandCommand>(expand));

		node = logic.getSubfolder(sceneNode, QD_NAMED_OBJECT_STATIC_OBJ);
		expand = 
			pps->GetIntSetting((*r)->name(), CString((LPTSTR)IDS_STATIC_OBJ_CATALOG), 0);
		logic.expand(node, static_cast<TreeLogic::eExpandCommand>(expand));

		node = logic.getSubfolder(sceneNode, QD_NAMED_OBJECT_ANIMATED_OBJ);
		expand = pps->GetIntSetting((*r)->name(), 
			CString((LPTSTR)IDS_ANIMATED_OBJS_NODE), 0);
		logic.expand(node, static_cast<TreeLogic::eExpandCommand>(expand));

		node = logic.getSubfolder(sceneNode, QD_NAMED_OBJECT_MOVING_OBJ);
		expand = 
			pps->GetIntSetting((*r)->name(), CString((LPTSTR)IDS_MOVING_OBJS_NODE), 0);
		logic.expand(node, static_cast<TreeLogic::eExpandCommand>(expand));
	}
}

//! Считывает состояние для интерфейсных экранов
void Project::readInterfaceScreensState(IProjectSettings* pps)
{
	TreeLogic& logic = getTreeLogic();
	ITreeNode* interfaceNode = 
		*logic.getObjectNodes(qdInterfaceDispatcher::get_dispatcher());
	ITreeNode* child = logic.getChildNode(interfaceNode);
	CString interfaceSection;
	interfaceSection.LoadString(IDS_INTERFACE_NODE);
	ASSERT(child);
	do {
		int expand = pps->GetIntSetting(interfaceSection, logic.getNodeText(child), 0);
		logic.expand(child, static_cast<TreeLogic::eExpandCommand>(expand));

	} while(child = logic.getNextSiblingNode(child));
}
*/

void Project::OnAdjustGameArea()
{
	CWaitCursor _wait_;
	//Здесь на самом деле выравнивается рабочая область :-)
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdGameObjectStatic* pobj = 
		static_cast<qdGameObjectStatic*>(focused->getContent().gameObject);
	qdCamera* pcam = gameDispatcher_->get_active_scene()->get_camera();
	Vect2i old_scr_size = pcam->get_scr_size();
	Vect2i old_scr_offset = pcam->get_scr_offset();

	Vect2s obj_size = pobj->screen_size();

	pcam->set_scr_size(obj_size.x, obj_size.y);
	Vect2i sc = pcam->get_scr_center();

	pcam->set_scr_offset(pobj->screen_pos() - Vect2s(sc.x, sc.y));
	sceneView_->AdjustBitmapSize();
	sceneView_->Redraw(RM_REDRAW_CONTENT);

	if (old_scr_size != pcam->get_scr_size()||old_scr_offset != pcam->get_scr_offset()) 
			SetModified(true);
}

void Project::OnRemoveEdges()
{
	removeEdges();
}

void Project::OnRemoveEdgesForAllObjs()
{
	removeEdges();
}

void Project::OnRemoveEdgesInStates()
{
	removeEdges();
}

void Project::removeEdges()
{
	CString msg((LPTSTR)IDS_REMOVE_EDGES_QUESTION);
	if (AfxMessageBox(msg, MB_OKCANCEL) != IDOK) 
		return;

	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;

	CWaitCursor __wait__;

	EdgeRemover::remove_edges(getGameDispatcher(), 
		static_cast<qdNamedObjectBase*>(focused->getContent().gameObject));
	sceneView_->Redraw(RM_REDRAW_CONTENT);
	SetModified(true);
	DoFileSave();
}

void Project::OnDisableMouse()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdGameObjectAnimated* const pno = 
		static_cast<qdGameObjectAnimated*>(focused->getContent().gameObject);
	ASSERT(ObjectTraits::is_obj_animated(pno));
	if (pno->check_flag(QD_OBJ_DISABLE_MOUSE_FLAG))
		pno->drop_flag(QD_OBJ_DISABLE_MOUSE_FLAG);
	else
		pno->set_flag(QD_OBJ_DISABLE_MOUSE_FLAG);

	focused->updateItemImage(getTreeLogic());
}

void Project::OnSelectMovementType()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdGameObjectStateWalk* const pstate = 
		static_cast<qdGameObjectStateWalk*>(focused->getContent().gameObject);
	if (SelectMovementType(pstate))
		SetModified(true);
}

void Project::OnStateDims()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdGameObjectState* const pstate = 
		static_cast<qdGameObjectState*>(focused->getContent().gameObject);
	beginEditStateDims(pstate);
}

void Project::OnBtnAutoBnd()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdGameObjectState* const pstate = 
		static_cast<qdGameObjectState*>(focused->getContent().gameObject);

	ASSERT(pstate);

	qdGameObjectAnimated* const pao = 
		static_cast<qdGameObjectAnimated*>(pstate->owner());

	Vect3f old_bnd = pao->obj_bound();
	if (pstate->has_bound())
		old_bnd = pstate->bound();

	pstate->auto_bound();
	pstate->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);
	if (ObjectTraits::is_obj_moving(pstate->owner()))
	{
		SaveScrLayoutOnBndEdt(pstate, 
			gameDispatcher_->get_active_scene()->get_camera(), old_bnd);
		static_cast<qdGameObjectMoving*>(pstate->owner())->adjust_z();
	}
	beginEditStateDims(pstate);
	SetModified(true);
	sceneView_->Redraw(RM_REDRAW_CONTENT);
}

void Project::OnStateHasBound()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdGameObjectState* const pstate = 
		static_cast<qdGameObjectState*>(focused->getContent().gameObject);
	if (pstate->has_bound())
	{
		Vect3f old_bnd;
		if (ObjectTraits::is_obj_moving(pstate->owner()))
			old_bnd = static_cast<qdGameObjectMoving*>(pstate->owner())->bound();

		pstate->drop_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);
		if (ObjectTraits::is_obj_moving(pstate->owner()))
		{
			qdGameObjectMoving* const pobj = 
				static_cast<qdGameObjectMoving*>(pstate->owner());

			SaveScrLayoutOnBndEdt(pstate, 
				gameDispatcher_->get_active_scene()->get_camera(), old_bnd, 
				pobj->obj_bound());

			pobj->adjust_z();
		}
	}
	else
	{
		qdGameObjectAnimated* const pobj = 
			static_cast<qdGameObjectAnimated*>(pstate->owner());
		Vect3f old_bnd(pobj->obj_bound());
		pstate->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);

		if (ObjectTraits::is_obj_moving(pobj)) {
			if (pstate->bound() == qdGameObjectState::DEFAULT_BOUND) 
				pstate->set_bound(pobj->qdGameObjectAnimated::obj_bound());

			SaveScrLayoutOnBndEdt(pstate, 
				gameDispatcher_->get_active_scene()->get_camera(), 
				old_bnd);
			static_cast<qdGameObjectMoving*>(pobj)->adjust_z();
		}
		beginEditStateDims(pstate);
	}
}

void Project::OnMakeBoundAsFirst()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdGameObjectState* const pstate = 
		static_cast<qdGameObjectState*>(focused->getContent().gameObject);

	qdGameObjectAnimated* ptrObj = static_cast<qdGameObjectAnimated*>(pstate->owner());
	assert(!ptrObj->state_vector().empty());

	qdGameObjectState* ptrFirstObjectState = ptrObj->state_vector()[0];
	if (ptrFirstObjectState != pstate&&ptrFirstObjectState->has_bound())
	{
		pstate->set_bound(ptrFirstObjectState->bound());
		pstate->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);
		SetModified(true);
	}
}

void Project::OnStackState()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdGameObjectState* const pstate = 
		static_cast<qdGameObjectState*>(focused->getContent().gameObject);
	ASSERT(pstate);
	beginStackState(pstate);
}

void Project::OnCoordsAnimation()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdGameObjectState* const pstate = 
		static_cast<qdGameObjectState*>(focused->getContent().gameObject);
	beginCAEdit(pstate);
}

void Project::OnCancelRemovingEdges()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdGameObjectState* const pstate = 
		static_cast<qdGameObjectState*>(focused->getContent().gameObject);
	qdGameObjectAnimated* ptrObj = static_cast<qdGameObjectAnimated*>(pstate->owner());

	qdGameScene* scene = static_cast<qdGameScene*>(ptrObj->owner());
	qdCamera const* camera = scene->get_camera();
	Vect2i center = camera->global2scr(Vect3f(0.f, 0.f, 0.f));
	pstate->set_center_offset(center - camera->global2scr(ptrObj->R()));
	SetModified(true);
	sceneView_->Redraw(RM_REDRAW_CONTENT);
}

/*!
	Останавливает поток проигрывания анимации. Создает диалог редактирования размеров.
	Переводит редактор сцены в режим редактирования размеров. Вызывает 
	CChildView::OnEditStateDims() для увязки диалога с редактором сцены и для передачи
	редактируемог состояния
	\param pstate - Состояние, чьи размеры мы будем редактировать
	\retval TRUE - если все в порядке
	\retval FALSE - если не удалось начать стыковку
*/

LRESULT Project::beginEditStateDims(qdGameObjectState* pstate)
{
	stateBoundDlg_.reset(new StateBoundDlg);

	stateBoundDlg_->set_data(pstate);
	if (!stateBoundDlg_->Create(StateBoundDlg::IDD))
		return FALSE;

	sceneView_->PauseAnimation();

	sceneView_->SetViewState(VS_EDIT_STATE);
	sceneView_->SetSubTools(SSTE_NONE);

	CWnd* baseWnd = getTreeLogic().getBaseWnd();
	CRect r;
	baseWnd->GetWindowRect(&r);
	stateBoundDlg_->SetWindowPos(0, r.left, r.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	stateBoundDlg_->ShowWindow(SW_SHOW);

	sceneView_->OnEditStateDims(stateBoundDlg_.get(), pstate);
	return TRUE;
}

/*!
 	Останавливает процесс редактирования границ в редакторе сцены(
	CChildView::OnEndEditStateDims()) и переводит его в состояние редактирований сцены.
	Сообщает диалогу границы состояния, что процесс закончен(StateBoundDlg::EndEdit()) и
	закрываеет его. Перерисовывает редактор сцены(CChildView.Redraw). Если редактирование
	было запущено из дилога свойств, то его тоже уведомляем о прекращении работы
	\param bSave - сохранять или нет изменения
*/
void Project::endEditStateDims(bool bSave)
{
	sceneView_->OnEndEditStateDims(bSave);
	sceneView_->SetViewState(VS_EDIT_SCENE);

	qdGameObjectState* editedState = stateBoundDlg_->getEditedState();
	if (stateBoundDlg_){
		stateBoundDlg_->EndEdit(bSave?
									StateBoundDlg::EEM_SAVE:
									StateBoundDlg::EEM_DISCARD);
		stateBoundDlg_->DestroyWindow();
		stateBoundDlg_.reset(0);
	}

	if (bSave)
		SetModified(true);
	sceneView_->StartAnimation(editedState);
	sceneView_->Redraw(RM_REDRAW_CONTENT);
}

/*!
 	Останавливает поток проигрывания анимации. Создает объект диалога стыковки
	(StateCoordsStacker) и увязывает его с редактором сцены. Переводит 
	редактор сцены в состояние стыковки. Устанавливает активным состоянием для 
	объекта стыкуемое состояние
	\param pstate - Состояние, которое будем стыковать
	\retval TRUE - если все в порядке
	\retval FALSE - если не удалось начать стыковку
*/
LRESULT Project::beginStackState(qdGameObjectState* pstate)
{
	stateStackerDlg_.reset(new StateCoordsStacker);
	if (!stateStackerDlg_) 
		return FALSE;

	stateStackerDlg_->set_state(pstate);
	stateStackerDlg_->SetView(sceneView_);


	oldViewState_ = sceneView_->GetViewState();
	sceneView_->SetViewState(VS_EDIT_STACK_STATE);
	
	CWnd* baseWnd = getTreeLogic().getBaseWnd();
	if(!stateStackerDlg_->Create(StateCoordsStacker::IDD, baseWnd))
		return FALSE;

	sceneView_->PauseAnimation();
	CRect r;
	baseWnd->GetWindowRect(&r);
	stateStackerDlg_->SetWindowPos(0, r.left, r.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	stateStackerDlg_->ShowWindow(SW_SHOW);

	baseWnd->EnableWindow(FALSE);

	qdGameObjectAnimated* po = 
		static_cast<qdGameObjectAnimated*>(pstate->owner());
	po->set_state(pstate);
	ASSERT(po->get_cur_state() == pstate);
	
	return TRUE;				
}

/*!
 	Восстанавливает состояние редактора сцены в исходное. Информирует диалог стыковки
	о прекращении стыковки(StateBoundDlg::EndEdit()). . Если редактирование было запущено из 
	диалога редактирования свойств, то информирует его об окончании редактирования.
	\param bCancel - отменить/сохранить изменения
*/
void Project::endStateStack(bool bCancel)
{
	sceneView_->SetViewState(static_cast<VIEW_STATE>(oldViewState_));

	getTreeLogic().getBaseWnd()->EnableWindow(TRUE);

	qdGameObjectState* editedState = stateStackerDlg_->getStackedState();

	stateStackerDlg_->EndEdit(bCancel?StateCoordsStacker::EEM_DISCARD:
									   StateCoordsStacker::EEM_SAVE);
	if (!bCancel)
		SetModified(true);
	stateStackerDlg_->DestroyWindow();
	stateStackerDlg_.reset();
	sceneView_->StartAnimation(editedState);
}

/*!
 	Останавливает поток проигрывания анимации. Создает диалог редактирования КА и 
	увязывает его с редактором сцены. Переводит редактор сцены в режим редактирования
	КА и инициирует обновление информации о КА в редакторе сцены
	\param pstate - Состояние, чью КА мы будем редактировать
	\retval TRUE - если все в порядке
	\retval FALSE - если не удалось начать стыковку
*/
LRESULT Project::beginCAEdit(qdGameObjectState* pstate)
{
	sceneView_->PauseAnimation();

	coordAnimDlg_.reset(new CoordAnimDlg);
	if(!coordAnimDlg_)
		return FALSE;

	sceneView_->SetCoordsAnimationDlg(coordAnimDlg_.get());
	coordAnimDlg_->set_source_ca(pstate->coords_animation());
	coordAnimDlg_->SetView(sceneView_);
	
	CWnd* baseWnd = getTreeLogic().getBaseWnd();
	if(!coordAnimDlg_->Create(CoordAnimDlg::IDD, baseWnd))
		return FALSE;
	
	oldViewState_ = sceneView_->GetViewState();
	
	sceneView_->SetViewState(VS_EDIT_COORD_ANIM);
	sceneView_->UpdateCoordsAnimation();
	
	CRect r;
	baseWnd->GetWindowRect(&r);
	coordAnimDlg_->SetWindowPos(0, r.left, r.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	coordAnimDlg_->ShowWindow(SW_SHOW);

#ifdef _DEBUG
	qdGameObjectAnimated* po = static_cast<qdGameObjectAnimated*>(pstate->owner());
	ASSERT(po->get_cur_state() == pstate);
#endif
	
	baseWnd->EnableWindow(FALSE);
	return TRUE;
}


/*!
	Восстанавливает состояние редактора сцены. Информирует диалог редактирования 
	КА о прекращении редактирования(StateBoundDlg::EndEdit()). Если изменения 
	сохраняются, то обновляет изображение узла состояния
	(Project::UpdateStateImage()). Если редактирование было запущено из 
	диалога редактирования свойств, то информирует его об окончании редактирования
	\param bCancel - отменить/сохранить изменения
	\return Всегда true
*/
bool Project::endEditCoordAnimationMode(bool bCancel)
{

	sceneView_->SetViewState(static_cast<VIEW_STATE>(oldViewState_));
	coordAnimDlg_->DestroyWindow();
	coordAnimDlg_->EndEdit(bCancel? CoordAnimDlg::EEM_DISCARD: CoordAnimDlg::EEM_SAVE);
	qdGameObjectState* editedState = 
		static_cast<qdGameObjectState*>(coordAnimDlg_->get_source_ca()->owner());
	if (!bCancel)
	{
		ITreeNode * node = *getTreeLogic().getObjectNodes(editedState);
		node->updateItemImage(getTreeLogic());
		SetModified(true);
	}

	sceneView_->StartAnimation(editedState);
	coordAnimDlg_.reset();

	getTreeLogic().getBaseWnd()->EnableWindow(TRUE);
	return true;
}

void Project::OnPersDim()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdGameObjectMoving* const moving = 
		static_cast<qdGameObjectMoving*>(focused->getContent().gameObject);

	ASSERT (!ObjectTraits::isGlobal(*moving));

	OnEditAnimatedDims(moving);
}

void Project::OnPersDir()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdGameObjectMoving* const moving = 
		static_cast<qdGameObjectMoving*>(focused->getContent().gameObject);

	ASSERT (!ObjectTraits::isGlobal(*moving));

	OnEditAnimatedDir(moving);
}

void Project::OnNotControledPers()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	qdNamedObject* obj = static_cast<qdNamedObject*>(focused->getContent().gameObject);

	if (obj->check_flag(QD_OBJ_NON_PLAYER_PERSONAGE_FLAG)) 
		obj->drop_flag(QD_OBJ_NON_PLAYER_PERSONAGE_FLAG);
	else
		obj->set_flag(QD_OBJ_NON_PLAYER_PERSONAGE_FLAG);
}

/*!
 	Запускает режим редактирования размеров динамического объекта в редакторе сцены
	\param ptrObj - объект, размеры которого будем редактировать
	\return \a true - всегда
*/
bool Project::OnEditAnimatedDims(qdGameObjectAnimated* ptrObj)
{
	sceneView_->SetViewState(VS_EDIT_ANIMATED);
	sceneView_->SetSubTools(SSTE_EDIT_DIMS);
	
	sceneView_->OnEditAnimated(ptrObj);
	sceneView_->Redraw(RM_REDRAW_CONTENT);
	return true;
}

/*!
 	Запускает режим редактирования направления динамического объекта в редакторе сцены
	\param ptrObj - объект, направление которого будем редактировать
	\return \a true - всегда
	\remarks Реально направление можно выставлять только персонажам. Поэтому эта функция
	будет вызываться только для персонажей
*/
bool Project::OnEditAnimatedDir(qdGameObjectAnimated* ptrObj)
{
	sceneView_->SetViewState(VS_EDIT_ANIMATED);
	sceneView_->SetSubTools(SSTE_EDIT_DIR);
	
	sceneView_->OnEditAnimated(ptrObj);
	sceneView_->Redraw(RM_NO_CONTENT_REDRAW);
	return true;
}

void Project::OnRunAnimationEditor()
{
	CString str;
	str.LoadString(IDS_ANIMATION_EDITOR_FILE_NAME);

	CString app_path = disk_op::GetModulePath();		
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_splitpath( app_path, drive, dir, fname, ext );

	app_path = drive;
	app_path += dir;
	app_path += str;

	CString comLine = _T("\"");
	comLine += app_path;
	comLine += _T("\"");

	ProcessLauncher launch;

	if (!launch.createProcess(app_path, CStrBuf(comLine), STARTF_USESHOWWINDOW))
		AfxMessageBox(IDS_CNT_LOAD_ANIMATION_EDITOR, MB_OK|MB_ICONERROR);
}

void Project::OnEditDialogs()
{
	RunDialogEditor rde;
	if (!rde.run(GetPathName()))
		AfxMessageBox(IDS_CNT_LOAD_DIALOG_EDITOR_EXE, MB_OK|MB_ICONSTOP);
}

void Project::OnLastNail()
{
	CString tmp;
	if (IsModified())
	{
		tmp.LoadString(IDS_LAST_NAIL_PROJ_NOT_SAVED);
		if (AfxMessageBox(tmp, MB_YESNO) == IDYES)
			if (!DoFileSave())
				return;
	}

	tmp.LoadString(IDC_LAST_NAIL_EXE);
	CString app_path = disk_op::GetPathFromFullFileName(disk_op::GetModulePath());
	app_path += tmp;

	CString comand_line = _T("\"");
	comand_line += app_path;
	comand_line += _T("\"");

	comand_line += _T(" \"");
	comand_line += GetPathName();
	comand_line += _T("\"");

	ProcessLauncher launch;
	if (!launch.createProcess(app_path, CStrBuf(comand_line)))
		AfxMessageBox(IDS_CNT_LOAD_LAST_NAIL_EXE, MB_OK|MB_ICONERROR);
}

void Project::OnUpdateLastNail(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(gameDispatcher_.get() != NULL);
}

void Project::OnJoinProject()
{
	ProjectJoiner pj;
	pj.join(gameDispatcher_.get());
}

void Project::OnUpdateJoinProject(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(gameDispatcher_.get() != NULL);
}

void Project::OnSelectScene()
{
	TreeLogic& logic = getTreeLogic();
	ITreeNode* focused = logic.getFocusedNode();
	logic.setFocusedNode(NULL);

	SceneList sl;
	sl.set_data(gameDispatcher_.get());
	if(sl.DoModal() == IDOK)
	{
		qdGameScene* ptrScene = sl.get_scene();
		if(ptrScene)
			activateScene(ptrScene);
		else
			getProjectModifier().addScene(CString(), true);
		CollapseSceneNodes(gameDispatcher_->get_active_scene());
	}
	else
		logic.setFocusedNode(focused);

//		GetTreeCtrl().SelectItem(hSelected);
}

void Project::OnUpdateSelectScene(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(gameDispatcher_.get() != NULL);
}

void Project::activateScene(qdGameScene const* scene)
{
	TreeLogic& logic = getTreeLogic();
	ITreeNode* node = *logic.getObjectNodes(scene);
	if (logic.getCurrentSceneNode())
		logic.expand(node, TreeLogic::EC_COLLAPSE);
	logic.showNode(node);
	logic.expand(node, TreeLogic::EC_EXPAND);
}

void Project::OnEditGaits()
{
	MoveStopDlg dlg;
	dlg.set_data(gameDispatcher_.get(), gameDispatcher_.get());
	dlg.DoModal();
	if(dlg.has_change())
	{
		SetModified(true);
	}
}

void Project::OnUpdateEditGaits(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(gameDispatcher_.get() != NULL);
}

void Project::OnNetProperties()
{
	TreeLogic& logic = getTreeLogic();
	ITreeNode* netNode = logic.getSubfolder(
				logic.getCurrentSceneNode(), QD_NAMED_OBJECT_GRID_ZONE);

	if (!netNode)
		return;

	logic.showNode(netNode);
	netNode->showProperties(logic, 0);
}

void Project::OnRunScene()
{
	ITreeNode* scene = getTreeLogic().getCurrentSceneNode();
	if (!scene)
		return;

	RunScene(scene->getContent().gameObject->name(), IProjectFacade::RM_NORMAL);
}

void Project::OnUpdateRunScene(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(getTreeLogic().getCurrentSceneNode() != NULL);
}

void Project::OnRunScript()
{
	RunScene(CString(), IProjectFacade::RM_RUN_SCRIPT);
}

void Project::OnUpdateRunScript(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(gameDispatcher_.get() != NULL);
}

/*!
	Для каждой строки, хранящейся в \a m_TmpFiles вызываем DeleteFile
*/
void Project::removeTmps()
{
	std::for_each(m_TmpFiles.begin(), m_TmpFiles.end(), DeleteFile);
	DeleteFile(tempFileName_);
}

/*!
	Создает временный файл для скрипта в каталоге проекта. Кладет полученное имя
	в m_TmpFiles.
	\return Возвращает полное имя временного файла
*/
CString Project::getTemporaryFileName()
{
	if (!m_TmpFiles.empty()) 
	{
		if (DeleteFile(m_TmpFiles.back()))
			m_TmpFiles.pop_back();
	}
	CString res;
	CString path = disk_op::GetPathFromFullFileName(GetPathName());
	CString baseFileName = disk_op::GetFileName(GetPathName());
	do{
		res = path;
//		res += _T("\\");
		CString file_name;
		file_name.Format(_T("script_tmp%d%s"), GetTickCount(), baseFileName);
		res += file_name;
		res += _T(".qml");
	}
	while (PathFileExists(res));
	m_TmpFiles.push_back(res);
	return res;
}

void Project::OnShowAllNetProjs()
{
	sceneView_->SetDrawNetProjs(!sceneView_->GetDrawNetProjs());
	sceneView_->Redraw(RM_REDRAW_CONTENT);
}

void Project::OnUpdateShowAllNetProjs(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(sceneView_->GetDrawNetProjs());
	pCmdUI->Enable(gameDispatcher_.get() != NULL);
}

void Project::OnDrawAllBounds()
{
	sceneView_->SetDrawBounds(!sceneView_->GetDrawBounds());
	sceneView_->Redraw(RM_REDRAW_CONTENT);
}

void Project::OnUpdateDrawAllBounds(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(sceneView_->GetDrawBounds());
	pCmdUI->Enable(gameDispatcher_.get() != NULL);
}


void Project::OnToolbarDropDown(NMHDR* pNMHDR, LRESULT *plr)
{
	NMTOOLBAR* pnmtb = reinterpret_cast<NMTOOLBAR*>(pNMHDR);
	CWnd *pWnd = CWnd::FromHandle(pnmtb->hdr.hwndFrom);

	UINT nID = 0;
	// Switch on button command id's.
	switch (pnmtb->iItem)
	{
	case IDC_EDIT_TRIGGERS:
		nID = IDC_EDIT_TRIGGERS;
		break;
	default:
		return;
	}

	// load and display popup menu
	CMenu menu;
	menu.CreatePopupMenu();
	switch (pnmtb->iItem)
	{
	case IDC_EDIT_TRIGGERS:
		FillMenuForTriggers(&menu);
		break;
	}

	MENUINFO mi;
	memset(&mi, 0, sizeof(MENUINFO));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIM_STYLE|MIM_MENUDATA;
	mi.dwStyle = MNS_NOTIFYBYPOS;
	mi.dwMenuData = static_cast<ULONG_PTR>(nID);

	menu.SetMenuInfo(&mi);

	CRect rc;
	pWnd->SendMessage(TB_GETRECT, pnmtb->iItem, (LPARAM)&rc);
	pWnd->ClientToScreen(&rc);
	menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
		rc.left, rc.bottom, getTreeLogic().getBaseWnd(), &rc);
	//Отцепляем, чтобы не разрушилось при выходе
	// и в OnMenuCommand пришла хорошая ссылка
	menu.Detach();
}

LRESULT Project::OnMenuCommand(WPARAM wParam, LPARAM lParam)
{
	HMENU hmenu = reinterpret_cast<HMENU>(lParam);
	if (!hmenu)
		return TRUE;
	MENUINFO mi;
	memset(&mi, 0, sizeof(MENUINFO));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIM_MENUDATA;

	GetMenuInfo(hmenu, &mi);
	switch(mi.dwMenuData) {
	case IDC_EDIT_TRIGGERS:
		HandleEditTriggersMenu(hmenu, wParam);
		break;
	}
	DestroyMenu(hmenu);
	return TRUE;
}

/*!
Достаем из узла, на который кликнули цепочку и показыаем её в редакторе
триггеров. Показыает редактор триггеров. Устанавливает правильный цвет для 
буквы "Т" рядом с узлами объектов, участвующий в триггере
\param hm	- Описатель меню триггеров
\param pos	- Позиция кликнутого пункта меню 
*/
void Project::HandleEditTriggersMenu(HMENU hm, DWORD pos)
{
	MENUITEMINFO mii;
	memset(&mii, 0, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_DATA;
	GetMenuItemInfo(hm, pos, TRUE, &mii);

	CWaitCursor wait;
	qdTriggerChain* pchain = reinterpret_cast<qdTriggerChain*>(mii.dwItemData);
	if (pchain) 
	{
		if(triggerEditorView_->ShowTriggers(&pchain, 1))
		{
			if (viewActivator_.getSceneViewActive()) 
				viewActivator_ = false;

			setActiveTriggerIcon(pchain, TRUE);
			//если выделенный элемент в дереве есть триггер или "Триггеры",
			//то переносим выделение на показываемый триггер
			ITreeNode* focused = getTreeLogic().getFocusedNode();
			if (!focused
				||TreeNodeTraits::isTrigger(focused)
				||TreeNodeTraits::isTriggers(focused))
			{
				TreeLogic& logic = getTreeLogic();
				logic.showNode(*logic.getObjectNodes(pchain));
			}
			getTreeLogic().getTreeListCtrl().Invalidate();
		}
	}
}


/*!
Пробегаем по всем цепочкам, и добавляем пункты с именем цепочки.Укзатели на 
цепочки кладем в item data
\param pm - меню, которое будем заполнять
*/
void Project::FillMenuForTriggers(CMenu*pm)
{
	typedef qdTriggerChainList::const_iterator ci_t;
	const qdTriggerChainList& tcl = gameDispatcher_->trigger_chain_list();

	if (tcl.empty())
		return;

	MENUITEMINFO mii;
	memset(&mii, 0, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_DATA;

	qdTriggerChain* pc_showed = triggerEditorView_->ShowedTrigger();

	UINT_PTR i = 0;
	ci_t itr = tcl.begin(), end = tcl.end();
	for(;itr != end; ++itr, ++i)
	{
		pm->AppendMenu(MF_STRING, i, (*itr)->name());
		mii.dwItemData = reinterpret_cast<ULONG_PTR>(*itr);
		pm->SetMenuItemInfo(i, &mii, TRUE);
		if (pc_showed == *itr)
			pm->SetDefaultItem(i, TRUE);
	}
}

void Project::OnDelete()
{
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (!focused)
		return;
	CWaitCursor wait;
	deleteNode(focused, true);
}

//! Удаляет узел и объект этого узла 
bool Project::deleteNode(ITreeNode* node, bool ask)
{
	sndDispatcher::SoundDisabler disableSound;
	if (node->deleteObject(getTreeLogic(), ask, true))
	{
		SetModified(true);
		return true;
	}
	return false;
}

//! Копирует анимированный объект в анимированный объект(это могу быть и персонажи)
bool Project::copyAnimatedToAnimated(ITreeNode* dropTarget, 
							qdGameObjectAnimated const* source,
							qdGameScene* scene)
{
	return (getProjectModifier().addAnimated(dropTarget, source, scene) != NULL);
}
//! Копирует анимированный объект в персонаж
bool Project::copyAnimatedToMoving(ITreeNode* dropTarget, 
						  qdGameObjectAnimated const* source,
						  qdGameScene* scene)
{
	return (getProjectModifier().addPersonage(dropTarget, source, scene) != NULL);
}

//! Копирует персонаж в персонаж
bool Project::copyMovingToMoving(ITreeNode* dropTarget, 
						qdGameObjectMoving const* source,
						qdGameScene* scene)
{
	return (getProjectModifier().addPersonage(dropTarget, source, scene) != NULL);
}

qdGameObjectState* Project::moveState(qdGameObjectAnimated* source, 
			   qdGameObjectAnimated* target,
			   qdGameObjectState* movedState, 
			   ITreeNode* targetNode,
			   ITreeNode* insertAfter,
			   int insertBefore)
{
	return getProjectModifier().moveState(source, target, movedState, 
		targetNode, insertAfter, insertBefore);
}

qdGameObjectState* Project::copyState(qdGameObjectAnimated* source, 
			   qdGameObjectAnimated* target,
			   qdGameObjectState* copyState, 
			   ITreeNode* targetNode,
			   ITreeNode* insertAfter,
			   int insertBefore)
{
	return getProjectModifier().copyState(source, target, copyState, 
		targetNode, insertAfter, insertBefore);
}

qdGridZone* Project::moveZone(qdGameScene* scene, 
							  qdGridZone* moved, 
							  qdGridZone const* zoneBefore, 
							  ITreeNode * insertAfter,
							  ITreeNode * parent)
{
	return getProjectModifier().moveZone(scene, moved, zoneBefore, insertAfter, parent);
}

qdVideo* Project::moveVideo(qdVideo* source, 
							qdVideo const* before, 
							ITreeNode* insertAfter,
							ITreeNode* parent)
{
	return getProjectModifier().moveVideo(source, before, insertAfter, parent);
}

qdInventory* Project::copyInventory(qdInventory const* source, ITreeNode* parent)
{
	return getProjectModifier().copyInventory(source, parent);
}

void Project::OnMgParameters()
{
	ITreeNode* node = getTreeLogic().getFocusedNode();
	if (!node)
		return;
	qdMiniGame* miniGame = static_cast<qdMiniGame*>(node->getContent().gameObject);
	if (editMinigameParams(getGameDispatcher(), miniGame))
		SetModified(true);
}

void Project::OnCloseDocument()
{
	saveProjSettings();
	reset();
	__super::OnCloseDocument();
}

void Project::OnTbtnEntryForDialogs() 
{
	if (getTriggerElementFilterScheme() == FS_ENTRY_FOR_DIALOGS)
		triggerElementFilterScheme_ = FS_NONE;
	else
		triggerElementFilterScheme_ = FS_ENTRY_FOR_DIALOGS;

	typedef bool (*f)(const qdNamedObject* po);

	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (getTriggerElementFilterScheme() == FS_ENTRY_FOR_DIALOGS) 
	{

		triggerEditorView_->SelectByFilter(
			tef_dlg_state(getSceneByNode(focused)), 
			ITriggerView::SA_SELECT, 
			ITriggerView::SM_EXCLUSIVE);
	}
	else
		triggerEditorView_->SelectByFilter(tef_by_scene(getSceneByNode(focused)), 
		ITriggerView::SA_SELECT, ITriggerView::SM_EXCLUSIVE);

	if (TreeNodeTraits::isStateNode(focused))
	{
		triggerEditorView_->SelectByFilter(
			tef_by_obj(focused->getContent().gameObject), 
			ITriggerView::SA_SELECT,
			ITriggerView::SM_INTERSECT);
	}
	triggerEditorView_->Redraw();
}

void Project::OnTbtnEntryForDynObj() 
{
	if (getTriggerElementFilterScheme() == FS_ENTRY_FOR_DYN_OBJ)
		triggerElementFilterScheme_ = FS_NONE;
	else
		triggerElementFilterScheme_ = FS_ENTRY_FOR_DYN_OBJ;

	ITreeNode* focused = getTreeLogic().getFocusedNode();
	typedef bool (*f)(const qdNamedObject* po);
	if (getTriggerElementFilterScheme() == FS_ENTRY_FOR_DYN_OBJ) 
	{
		triggerEditorView_->SelectByFilter(
			tef_by_scenes_objs(getSceneByNode(focused),
			ObjectTraits::is_obj_animated), 
			ITriggerView::SA_SELECT, 
			ITriggerView::SM_EXCLUSIVE);
	}
	else
		triggerEditorView_->SelectByFilter(tef_by_scene(getSceneByNode(focused)), 
		ITriggerView::SA_SELECT, ITriggerView::SM_EXCLUSIVE);

	if (TreeNodeTraits::isStateNode(focused))
	{
		triggerEditorView_->SelectByFilter(
			tef_by_obj(focused->getContent().gameObject), 
			ITriggerView::SA_SELECT,
			ITriggerView::SM_INTERSECT);
	}
	triggerEditorView_->Redraw();
}

void Project::OnTbtnEntryForMouse() 
{
	if (getTriggerElementFilterScheme() == FS_ENTRY_FOR_MOUSE)
		triggerElementFilterScheme_ = FS_NONE;
	else
		triggerElementFilterScheme_ = FS_ENTRY_FOR_MOUSE;

	typedef bool (*f)(const qdNamedObject* po);
	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (getTriggerElementFilterScheme() == FS_ENTRY_FOR_MOUSE) 

		triggerEditorView_->SelectByFilter(
			tef_by_scenes_objs(NULL,ObjectTraits::is_obj_mouse), 
		ITriggerView::SA_SELECT, ITriggerView::SM_EXCLUSIVE);
	else
	{
		triggerEditorView_->SelectByFilter(tef_by_scene(getSceneByNode(focused)), 
			ITriggerView::SA_SELECT, ITriggerView::SM_EXCLUSIVE);
	}
	if (TreeNodeTraits::isStateNode(focused))
	{
		triggerEditorView_->SelectByFilter(
			tef_by_obj(focused->getContent().gameObject), 
			ITriggerView::SA_SELECT,
			ITriggerView::SM_INTERSECT);
	}
	triggerEditorView_->Redraw();
}

void Project::OnTbtnEntryForPers() 
{
	if (getTriggerElementFilterScheme() == FS_ENTRY_FOR_PERS)
		triggerElementFilterScheme_ = FS_NONE;
	else
		triggerElementFilterScheme_ = FS_ENTRY_FOR_PERS;

	typedef bool (*f)(const qdNamedObject* po);

	ITreeNode* focused = getTreeLogic().getFocusedNode();
	if (getTriggerElementFilterScheme() == FS_ENTRY_FOR_PERS) 
	{

		triggerEditorView_->SelectByFilter(tef_by_scenes_objs(getSceneByNode(focused),
			ObjectTraits::is_obj_moving), 
			ITriggerView::SA_SELECT, 
			ITriggerView::SM_EXCLUSIVE);
	}
	else
		triggerEditorView_->SelectByFilter(tef_by_scene(getSceneByNode(focused)), 
		ITriggerView::SA_SELECT, ITriggerView::SM_EXCLUSIVE);

	if (TreeNodeTraits::isStateNode(focused))
	{
		triggerEditorView_->SelectByFilter(
			tef_by_obj(focused->getContent().gameObject), 
			ITriggerView::SA_SELECT,
			ITriggerView::SM_INTERSECT);
	}
	triggerEditorView_->Redraw();
}

void Project::OnTbtnShowElement()
{
	showTriggerElement_ = !showTriggerElement_;
}

void Project::OnUpdateTbtnEntryForDialogs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(gameDispatcher_&&!viewActivator_.getSceneViewActive());
	pCmdUI->SetCheck(getTriggerElementFilterScheme() == FS_ENTRY_FOR_DIALOGS);
}

void Project::OnUpdateTbtnEntryForDynObj(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(gameDispatcher_&&!viewActivator_.getSceneViewActive());
	pCmdUI->SetCheck(getTriggerElementFilterScheme() == FS_ENTRY_FOR_DYN_OBJ);
}

void Project::OnUpdateTbtnEntryForMouse(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(gameDispatcher_&&!viewActivator_.getSceneViewActive());
	pCmdUI->SetCheck(getTriggerElementFilterScheme() == FS_ENTRY_FOR_MOUSE);
}

void Project::OnUpdateTbtnEntryForPers(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(gameDispatcher_&&!viewActivator_.getSceneViewActive());
	pCmdUI->SetCheck(getTriggerElementFilterScheme() == FS_ENTRY_FOR_PERS);
}

void Project::OnUpdateTbtnShowElement(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(gameDispatcher_&&!viewActivator_.getSceneViewActive());
	pCmdUI->SetCheck(showTriggerElement_);
}

//! Возвращает сцену,под которой находится узел
qdGameScene* Project::getSceneByNode(ITreeNode const* node) const{
	ITreeNode* curScene = getTreeLogic().getCurrentSceneNode();
	if (!curScene)
		return NULL;
	return static_cast<qdGameScene*>(curScene->getContent().gameObject);
}


void Project::showNewProjWiz()
{
	if (!gameDispatcher_)
		return;

	NewProjMultiWizard wiz;
	wiz.show(getTreeLogic().getBaseWnd(),gameDispatcher_.get(), IDS_NEW_PRJ_WIZ_CAP);
}

boost::shared_ptr<AppSettingsBasePage> Project::getPage()
{
	boost::shared_ptr<GenSettingsPP> ptrPage(new GenSettingsPP);
	ptrPage->AddListener(this);
	return ptrPage;
}

void Project::SettingsChanged()
{
	BOOL bAutoSave = GetMyApp()->GetIntSetting(SETTING_NAME(AUTOSAVE));
	if (bAutoSave)
		startAutoSavePeriod();
	else
		getTreeLogic().getBaseWnd()->KillTimer(TM_AUTO_SAVE_TIMER);
}

void Project::OnAppSettings()
{
	AppStngsDlg dlg;
	dlg.AddUser(this);
	dlg.AddUser(dynamic_cast<IAppSettingsUser*>(sceneView_));
	dlg.AddUser(dynamic_cast<IAppSettingsUser*>(triggerEditorView_));
	dlg.DoModal();
}

void Project::OnCollapseAll()
{
	TreeLogic& logic = getTreeLogic();
	ITreeNode* focused = logic.getFocusedNode();
	logic.expandNodeAndChilds(focused, TreeLogic::EC_COLLAPSE);
}

void Project::OnExpandAll()
{
	TreeLogic& logic = getTreeLogic();
	ITreeNode* focused = logic.getFocusedNode();
	logic.expandNodeAndChilds(focused, TreeLogic::EC_EXPAND);
	
}

void Project::startAutoSavePeriod()
{
	getTreeLogic().getBaseWnd()->KillTimer(TM_AUTO_SAVE_TIMER);
	if(GetMyApp()->GetIntSetting(SETTING_NAME(AUTOSAVE)))
		autoSaveTimer_ = getTreeLogic().getBaseWnd()->SetTimer(TM_AUTO_SAVE_TIMER, 
			GetMyApp()->GetIntSetting(SETTING_NAME(AUTOSAVE_PERIOD)), NULL);
}

void Project::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == TM_AUTO_SAVE_TIMER)
	{
		startAutoSavePeriod();
		autoSave();
	}
}

void Project::autoSave()
{
	if(GetMyApp()->GetIntSetting(SETTING_NAME(AUTOSAVE)))
	{
		CWaitCursor __wait__;
		CString strMsg((LPTSTR)IDS_AUTOSAVE_BEGIN);
		static_cast<CFrameWnd*>(AfxGetMainWnd())->SetMessageText(strMsg);

		safeSaveScript(gameDispatcher_.get(), tempFileName_);
//		gameDispatcher_->save_script(tempFileName_);

		strMsg.LoadString(IDS_AUTOSAVE_END);
		static_cast<CFrameWnd*>(AfxGetMainWnd())->SetMessageText(strMsg);
	}
}

void Project::makeTempFileName()
{
	CString str = GetPathName();
	int iPos = str.Find(_T(".qml"));
	if(iPos != -1)
	{
		do {
			tempFileName_ = str.Left(iPos);
			tempFileName_ += _T("_copy_");
			CTime time ( CTime::GetCurrentTime());
			tempFileName_ += time.Format(_T("%H_%M_%d_%m_%Y"));
		} while(PathFileExists(tempFileName_));
	}
	tempFileName_ += _T(".qml");
}
