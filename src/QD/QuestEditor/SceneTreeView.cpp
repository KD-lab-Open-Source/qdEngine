// SceneTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"

#include <xutil.h>
#include "GR_dispatcher.h"
#include <algorithm>
#include <iterator>
#include <Shlwapi.h>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/functional.hpp>

#include "qd_music_track.h"
#include "qd_game_object_static.h"
#include "qd_game_object_animated.h"
#include "qd_game_object_moving.h"
#include "qd_game_object_mouse.h"

#include "qd_interface_object_base.h"
#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"
#include "qd_interface_button.h"
#include "qd_interface_slider.h"
#include "qd_interface_background.h"
#include "qd_interface_element_state.h"
#include "qd_interface_text_window.h"

#include "qd_trigger_profiler.h"
#include "qd_minigame.h"
#include "qd_grid_zone.h"
#include "qd_trigger_chain.h"
#include "qd_video.h"
#include "qd_interface_save.h"
#include "qd_game_end.h"
#include "qd_counter.h"
#include "snd_dispatcher.h"
#include "qd_screen_text_dispatcher.h"

#include "SceneTreeView.h"
#include "ptree_hlprs.hpp"
#include "ptree_loading.h"
#include "ptree_api.h"

#include "childview.h"
#include "MainFrm.h"

#include "progdlg.h"
#include "movestopdlg.h"
#include "editstatedlg.h"
#include "cursorpropdlg.h"

#include "name_op.h"
#include "disk_op.h"

//PropertyPages
#include "gamesettingspage.h"
#include "AddAnimStateWPg.h"
#include "AddAnimFirstWPg.h"
#include "wizardsheet.h"
#include "relativepath.h"
#include "soundcatalogdlg.h"
#include "AnimCatalogDlg.h"

#include "ITriggerView.h"

#include "ptree_te_filters.h"

#include "statecoordsstacker.h"
#include "coordanimdlg.h"


#include "statebounddlg.h"
#include "RqstngStateEdit.h"

#include "dirmakewizardshower.h"


#include "baseplanepage.h"
#include "screenprop.h"
#include "staticobjpage.h"
#include "objposproppage.h"
#include "animobjstates.h"
#include "characterproppage.h"
#include "charbound.h"
#include "paralaxpage.h"
#include "zoneproppage.h"
#include "animobjbound.h"
#include "inventoryppforpers.h"
#include "inventorypp.h"
#include "videopage.h"
#include "InvetoryPropPage.h"
#include "CounterPropPage.h"
#include "PersonageControlPropPage.h"
#include "MusicTrackPropPage.h"
#include "ScreenTextPropPage.h"
#include "InterfaceTextWindowPP.h"
#include "fontproppage.h"
#include "FontsPropPage.h"

#include "InterfSldrStateProp.h"
#include "InterfSldrProp.h"
#include "InterfSaveProp.h"
#include "InterfElePosPage.h"
#include "InterfBackPP.h"
#include "InterfScreenProp.h"
#include "InterfBtnProp.h"

#include "EdgeRemover.h"

#include "MyPropertySheet.h"

#include "newprojmultiwizard.h"

#include "scenelist.h"

#include "rename_helpers.h"

#include "settingdefs.h"

#include "addintrfbackgrndwizardshow.h"
#include "addinterfsldrwizardshow.h"
#include "addinterfbtnwizshow.h"
#include "addinterfbtnstatewizshow.h"
#include "addinterfsavewizshow.h"
#include "addgameendwizardshower.h"

#include "TriggerJoinerCondsChecker.h"
#include "ConvertTGAtoQDAInInterface.h"
#include "ProjectJoiner.h"
#include "animationfilenameextractor.h"
#include ".\scenetreeview.h"

#include "WaveLocalization/WaveToLoc.h"
#include "sv_api.h"
#include <boost/rtl.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SceneTreeView * g_ptrProjTreeView = NULL;

namespace ptree_api{
SceneTreeView* GetProjTree()
	{return g_ptrProjTreeView;}
}
/************************************************************************/
/* Внешние определения                                                  */
/************************************************************************/

using namespace ptree_helpers;

bool SelectMovementType(qdGameObjectStateWalk* pstate);

void ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID = -1);
void ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID = -1);
DWORD WINAPI ShowAnimationThread(LPVOID pData);
bool EditStateBound(qdGameObjectState* pstate);

std::pair<bool, qdGameObject*> AskForObject(qdGameScene* ptrScene, 
											int type, bool bCanCrtNew);

const CString AskUserForName(const CString& strDef);
bool AskUserForName(CString& strDef);
bool ShowCondDlg(qdGameObjectState* ptrState, qdGameDispatcher* ptrDisp);
void SaveScrLayoutOnBndEdt(qdGameObjectState* pstate, qdCamera* pcam, 
						   Vect3f const& old_bnd);
void SaveScrLayoutOnBndEdt(qdGameObjectState* pstate, qdCamera* pcam, 
						   Vect3f const& old_bnd, Vect3f const&bnd);
//запускает редактор анимации
bool EditAnimation(HWND hParent, LPCTSTR lpszFileName);

void PlayError();
bool showCDDiskManager(qdGameDispatcher* pdispatcher);

//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// SceneTreeView
IMPLEMENT_DYNCREATE(SceneTreeView, CTreeView)

/************************************************************************/
/* Предварительные объявления                                           */
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* Вспомогалово:)                                                       */
/************************************************************************/

void SceneLoadingProgressCallback(int percents, void * data)
{
	CProgressDlg* dlg = (CProgressDlg*)data;
	dlg->SetPos(percents);
}
//////////////////////////////////////////////////////////////////////////

//c this здесь все в порядке. потому что я его тут исопльзую
//только для инициализации.
#pragma warning(disable : 4355)

//! Функтор для поиска элемента цепочки триггеров, содержащего объект
class SceneTreeView::is_object_in_te
{
	qdNamedObject* pno_;
public:
	is_object_in_te(qdNamedObject* pno):pno_(pno){}
	inline bool operator()(qdTriggerElementConstPtr const& pe)
	{
		return (pe->object() == pno_);
	}
};


SceneTreeView::SceneTreeView():m_IsSceneViewActive(this)
{
	m_ptrGameDisp				= NULL;
	m_ptrMainFrame				= NULL;
	m_uiShowVideoTimer			= (UINT)-1;
	m_hCurrScene				= NULL;
	g_ptrProjTreeView			= this;
	m_ptrPlayedObject			= NULL;
	m_ptrPlayedState			= NULL;
	m_dwAnimThreadId			= 0;
	m_hDraggedItem				= NULL;
	m_hDraggedItemParent		= NULL;
	m_ptrSceneView				= NULL;
	m_ptrIReqStateEdit			= NULL;
	m_eFilterScheme				= FS_NONE;
	m_ptrIScaleMgr				= NULL;
	m_bActivateTgrView			= true;
	objectList_					= NULL;
	showTriggerElement_			= true;
}

SceneTreeView::~SceneTreeView()
{
	ASSERT(!m_ptrStateStacker.get());
}


BEGIN_MESSAGE_MAP(SceneTreeView, CTreeView)
	//{{AFX_MSG_MAP(SceneTreeView)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_COMMAND(IDC_PROPERIES, OnProperies)
	ON_COMMAND(IDC_ANIMATION_LIST, OnAnimationList)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_COMMAND(IDC_CRT_SCENE, OnCrtScene)
	ON_COMMAND(IDC_CRT_MOVING_OBJ, OnCrtMovingObj)
	ON_COMMAND(IDC_CRT_STATIC_OBJ, OnCrtStaticObj)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_RUN_SCENE, OnUpdateButtonRunScene)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_RUN_SCRIPT, OnUpdateButtonRunScript)
	ON_COMMAND(IDC_REMOVE, OnRemove)
	ON_COMMAND(IDC_CRT_ANIMATION, OnCrtAnimation)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_COMMAND(IDC_MENU_MOVINGS_STOPS, OnMenuMovingsStops)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(IDC_CREATE_STATICS, OnCreateStatics)
	ON_UPDATE_COMMAND_UI(IDC_CREATE_STATICS, OnUpdateCreateStatics)
	ON_COMMAND(IDC_CREATE_BACK, OnCreateBack)
	ON_UPDATE_COMMAND_UI(IDC_CREATE_BACK, OnUpdateCreateBack)
	ON_WM_KEYDOWN()
	ON_COMMAND(IDC_NET_PROPERTIES, OnNetProperties)
	ON_COMMAND(IDC_EDIT_ANIMATION, OnEditAnimation)
	ON_COMMAND(IDC_EDIT_GAITS, OnEditGaits)
	ON_COMMAND(IDC_CRT_ANIM_OBJS, OnCrtAnimObjs)
	ON_COMMAND(IDC_CRT_ANIMATED_OBJ, OnCrtAnimObjs)
	ON_UPDATE_COMMAND_UI(IDC_CRT_MOVING_OBJ, OnUpdateCrtMovingObj)
	ON_UPDATE_COMMAND_UI(IDC_NET_PROPERTIES, OnUpdateNetProperties)
	ON_UPDATE_COMMAND_UI(IDC_CRT_ANIM_OBJS, OnUpdateCrtAnimObjs)
	ON_COMMAND(IDC_GAME_SETTINGS, OnGameSettings)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_ANIMATION, OnUpdateEditAnimation)
	ON_COMMAND(IDC_CREATE_STATE, OnCreateState)
	ON_WM_DESTROY()
//	ON_COMMAND(IDC_DEL_VIDEO, OnDelVideo)
	ON_COMMAND(IDC_PLAY_STOP_VIDEO, OnPlayStopVideo)
	ON_COMMAND(IDC_STOP_VIDEO, OnStopVideo)
	ON_COMMAND(IDC_STATE_CONDITION, OnStateCondition)
	ON_COMMAND(ID_BUTTON_RUN_SCENE, OnButtonRunScene)
	ON_COMMAND(IDC_BUTTON_RUN_SCRIPT, OnButtonRunScript)
	ON_COMMAND(IDC_CRT_GRID_ZONE, OnCrtGridZone)
	ON_COMMAND(IDC_CREATE_MASK, OnCreateMask)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(IDC_SOUND_LIST, OnSoundList)
	ON_COMMAND(IDC_EDIT_SOUND, OnEditSound)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_SOUND, OnUpdateEditSound)
	ON_COMMAND(IDC_SEL_SCENE, OnSelScene)
	ON_COMMAND(IDC_PERS_DIR, OnPersDir)
	ON_COMMAND(ID_TBTN_ALL_STATES_2_EDITOR, OnTbtnAllStates2Editor)
	ON_COMMAND(IDC_CRT_TRIGGER, OnCrtTrigger)
	ON_COMMAND(IDC_EDIT_TRIGGERS, OnEditTriggers)
	ON_UPDATE_COMMAND_UI(ID_TBTN_ALL_STATES_2_EDITOR, OnUpdateTbtnAllStates2Editor)
	ON_COMMAND(IDC_NOT_CONTROLED_PERS, OnNotControledPers)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_COMMAND(IDC_ADJUST_GAME_AREA, OnAdjustGameArea)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_GAITS, OnUpdateEditGaits)
	ON_UPDATE_COMMAND_UI(IDC_SEL_SCENE, OnUpdateSelScene)
	ON_COMMAND(IDC_STATE_DIMS, OnStateDims)
	ON_COMMAND(IDC_STATE_HAS_BOUND, OnStateHasBound)
	ON_COMMAND(IDC_BTN_AUTO_BND, OnBtnAutoBnd)
	ON_COMMAND(IDC_STACK_STATE, OnStackState)
	ON_COMMAND(IDC_COORDS_ANIMATION, OnCoordsAnimation)
	ON_COMMAND(IDC_CRT_NEW_INVENTORY, OnCrtNewInventory)
	ON_COMMAND(IDC_ADD_VIDEO, OnAddVideo)
	ON_COMMAND(IDC_REMOVE_CELL_SET, OnRemoveCellSet)
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
	ON_MESSAGE(WM_SET_COORD_ANIMATION_MODE, OnSetCoordAnimationMode)
	ON_MESSAGE(WM_STACK_STATE, OnStackState)
	ON_MESSAGE(WM_STATE_DIMS, OnEditStateDims)
	ON_COMMAND(IDC_CREATE_MINIGAME, OnCreateMinigame)
	ON_COMMAND(IDC_COND_OBJ_CONDITION, OnCOCondition)
	ON_COMMAND(IDC_ADD_CELL_SET, OnAddCellSet)
	ON_COMMAND(IDC_DISABLE_MOUSE, OnDisableMouse)
	//}}AFX_MSG_MAP
	ON_COMMAND(IDC_ADD_MUSIC_TRACK, OnAddMusicTrack)
	ON_COMMAND(IDC_MUSIC_TRACK_CYCLED, OnMusicTrackCycled)
	ON_COMMAND(IDC_MUSIC_TRACK_PLAY, OnMusicTrackPlay)
	ON_COMMAND(IDC_MUSIC_TRACK_STOP, OnMusicTrackStop)
	ON_UPDATE_COMMAND_UI(IDC_CRT_STATIC_OBJ, OnUpdateCrtStaticObj)
	ON_UPDATE_COMMAND_UI(IDC_CRT_GRID_ZONE, OnUpdateCrtGridZone)
	ON_UPDATE_COMMAND_UI(IDC_PLAY_STOP_VIDEO, OnUpdatePlayStopVideo)
	ON_UPDATE_COMMAND_UI(IDC_STOP_VIDEO, OnUpdateStopVideo)
	ON_UPDATE_COMMAND_UI(IDC_CRT_NEW_INVENTORY, OnUpdateCrtNewInventory)
	ON_COMMAND(IDC_PLAY_MINIGAME, OnPlayMinigame)
	ON_COMMAND(IDC_MAKE_DIR_STRUCT, OnMakeDirStruct)
	ON_COMMAND(IDC_EDIT_STATE_ANIMATION, OnEditStateAnimation)
	ON_COMMAND(IDC_REMOVE_EDGES, OnRemoveEdges)
	ON_NOTIFY(TBN_DROPDOWN, SCENE_TOOLBAR_ID, OnToolbarDropDown)
	ON_COMMAND(IDC_REMOVE_EDGES_IN_STATES, OnRemoveEdgesInStates)
	ON_COMMAND(IDC_REMOVE_EDGES_FOR_ALL_OBJS, OnRemoveEdgesForAllObjs)
	ON_COMMAND(IDC_BIND_OBJ_CENTER, OnBindObjCenter)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_COMMAND(IDC_SHOW_ALL_NET_PROJS, OnShowAllNetProjs)
	ON_UPDATE_COMMAND_UI(IDC_SHOW_ALL_NET_PROJS, OnUpdateShowAllNetProjs)
	ON_COMMAND(IDC_DRAW_ALL_BOUNDS, OnDrawAllBounds)
	ON_UPDATE_COMMAND_UI(IDC_DRAW_ALL_BOUNDS, OnUpdateDrawAllBounds)
	ON_WM_CTLCOLOR()
	ON_COMMAND(IDC_CREATE_ISCREEN, OnCreateIscreen)
	ON_COMMAND(IDC_ADD_INTERF_BACKGROUND, OnAddInterfBackground)
	ON_COMMAND(IDC_ADD_INTERF_SLIDER, OnAddInterfSlider)
	ON_COMMAND(IDC_ADD_INTERF_TEXT_WINDOW, OnAddInterfTextWindow)
	ON_COMMAND(IDC_ADD_INTRF_BUTTON, OnAddIntrfButton)
	ON_UPDATE_COMMAND_UI(IDC_ADD_VIDEO, OnUpdateAddVideo)
	ON_COMMAND(IDC_PERS_DIM, OnPersDim)
	ON_COMMAND(IDC_ADD_INTERF_BTN_STATE, OnAddInterfBtnState)
	ON_COMMAND(IDC_ADD_INTERF_SAVE, OnAddInterfSave)
	ON_COMMAND(IDC_RUN_ANIM_EDITOR, OnRunAnimEditor)
	ON_COMMAND(IDC_CRT_ANIM_OBJ_BY_MASK, OnCrtAnimObjByMask)
	ON_COMMAND(IDC_CONVERT_TO_QDA, OnConvertToQda)
	ON_COMMAND(ID_BUTTON_JOIN_PROJECT, OnJoinProject)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_JOIN_PROJECT, OnUpdateJoinProject)
	ON_COMMAND(IDC_ADD_GAME_END, OnAddGameEnd)
	ON_COMMAND(IDC_CREATE_COUNTER, OnCreateCounter)
	ON_COMMAND(IDC_SELECT_MOVEMENT_TYPE, OnSelectMovemtnType)
	ON_COMMAND(ID_MANAGE_CDS, OnManageCds)
	ON_COMMAND(ID_BTN_EDIT_DIALOGS, OnEditDialogs)
	ON_UPDATE_COMMAND_UI(ID_BTN_EDIT_DIALOGS, OnUpdateEditDialogs)
	ON_COMMAND(IDC_MAKE_BOUND_AS_FIRST, OnMakeBoundAsFirst)
	ON_COMMAND(IDC_CANCEL_REMOVING_EDGES, OnCancelRemovingEdges)
	ON_COMMAND(IDC_ADD_FONT, OnAddFont)
	ON_COMMAND(IDC_CREATE_FONT, OnCreateFont)
	ON_COMMAND(ID_WAVE_TO_LOCALIZATION, OnWaveToLocal)
	ON_COMMAND(ID_LOCALIZATION_TO_WAVE, OnLocalToWave)
	ON_UPDATE_COMMAND_UI(ID_WAVE_TO_LOCALIZATION, OnUpdateLocalToWave)
	ON_UPDATE_COMMAND_UI(ID_LOCALIZATION_TO_WAVE, OnUpdateWaveToLocal)
	ON_COMMAND(IDC_STAT_DEPENDENCIES, OnStatDependencies)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SceneTreeView drawing

/////////////////////////////////////////////////////////////////////////////
// SceneTreeView diagnostics

#ifdef _DEBUG
void SceneTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void SceneTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// SceneTreeView message handlers

void SceneTreeView::SetScaleMgr(IScaleMgr* psm){
	m_ptrIScaleMgr = psm;
}

void SceneTreeView::SetObjectList(IObjectList* objectList){
	objectList_ = objectList;
}

void SceneTreeView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	CTreeCtrl& tree	= GetTreeCtrl();
	CPoint pnt;
	GetCursorPos(&pnt);
	tree.ScreenToClient(&pnt);

	UINT uFlags;
	HTREEITEM hItem = tree.HitTest(pnt,&uFlags);
	if(!hItem || !(uFlags&TVHT_ONITEM))
		return;
	tree.SelectItem(hItem);
	ShowCorrespondingMenu(hItem);
}

int SceneTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CTreeCtrl& tc = GetTreeCtrl();
	LONG styles = GetWindowLong(tc.m_hWnd, GWL_STYLE);
	
	styles |= TVS_HASLINES | TVS_LINESATROOT | 
		TVS_HASBUTTONS | TVS_SHOWSELALWAYS|TVS_EDITLABELS;
	styles &=  ~TVS_DISABLEDRAGDROP;
	
	SetWindowLong(tc.m_hWnd, GWL_STYLE, styles);
	
	m_ptrTreeImageList = std::auto_ptr<CImageList>(new CImageList);
	LoadBitmaps(m_ptrTreeImageList.get());
	tc.SetImageList(m_ptrTreeImageList.get(), TVSIL_NORMAL);

	/*
	 *	добавляем в имиджи для состояний в триггере
	 */

	CImageList* pil = new CImageList;
	LoadStateBitmaps(pil);
	tc.SetImageList(pil, TVSIL_STATE);

	m_hAnimThread = axl::auto_object<HANDLE>(CreateThread
		(NULL, 0, ShowAnimationThread, this, 
										CREATE_SUSPENDED, &m_dwAnimThreadId));
	VERIFY(SetThreadPriority(m_hAnimThread, THREAD_PRIORITY_IDLE));

	m_ptrDT = std::auto_ptr<SceneTreeDropTarget>(new SceneTreeDropTarget);
	m_ptrDT->Register(this);	

	return 0;
}

/*!
 	Вызывает CChildView::SelectObj() для того, чтобы выбрать объект в редакторе сцены
	\param pno	- объект, который надо заселектить
	\remarks В отладочной версии приложения делается проверка, что объект принадлежит
			активной сцене
*/
void SceneTreeView::SelectObjectInSceneView(qdGameObject* pno)
{
//	ASSERT(!pno||pno->owner()== m_ptrGameDisp->get_active_scene());
	m_ptrSceneView->SelectObj(pno);
}

/*!
 	Проверяет является ли указанный именем экран активным.Если нет, то выбирает его
	(qdInterfaceDispatcher::select_screen()). Выравнивает размер битмапа в редакторе
	сцены(SceneTreeView::AdjustBitmapSize())
	\param name - имя экрана
	\return true - если выбранный экран сменился.
*/
bool SceneTreeView::SelectInterfaceScreen(LPCTSTR name)
{
	qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
	if (!pd->selected_screen()
		||_tcscmp(pd->selected_screen()->name(), name))
	{
		if(pd->select_screen(name))
		{
			AdjustBitmapSize();
			return true;
		}
	}
	return false;
}

/*!
 	Достает из узла сцену(CTreeCtrl::GetItemData()). Проверяет не является ли она(сцена)
	уже активной. Если является, то ничего не делаем. Иначе. Рессетит редактор 
	сцены(CChildView::reset()).Устанавливает callback 
	(SceneLoadingProgressCallback)на загрузку 
	(qdGameDispatcher::set_scene_loading_progress_callback())
	Создает дилог отражающий процесс загрузки.
	Выбирает сцену в диспетчере игры(qdGameDispatcher::select_scene()).
	Сбрасывае кэлбэк.
	Выравнивает размер битмапа в редакторе сцены(SceneTreeView::AdjustBitmapSize())
	Оживляет сцену( вызывает 2! раза qdGameDispatcher::quant())
	\param tree - дерево проекта
	\param hItem - узел сцены
	\return \a true - если сцена не была раньше активной
*/
bool SceneTreeView::SelectScene(CTreeCtrl& tree, HTREEITEM hItem)
{
	qdGameScene* ptrScene = 
		reinterpret_cast<qdGameScene*>(tree.GetItemData(hItem));
	if(ptrScene != m_ptrGameDisp->get_active_scene())
	{
		CWaitCursor __wait__;
		//m_ptrMainFrame->ResetSceneView();
		m_ptrSceneView->reset();
		CProgressDlg progress;
		//Устанавливаем каллбэк, чтобы получать статус загрузки
		m_ptrGameDisp->
			set_scene_loading_progress_callback(SceneLoadingProgressCallback,
														(void*)&progress);
		progress.Create();
		CString strStatus((LPTSTR)CG_IDS_PROGRESS_CAPTION);
		progress.SetStatus(strStatus);

		m_ptrGameDisp->select_scene(ptrScene);

		//сбрасываем каллбэк
		m_ptrGameDisp->set_loading_progress_callback(NULL,NULL);
		
		AdjustBitmapSize();
		m_ptrGameDisp->quant();
		m_ptrGameDisp->quant();
		return true;
	}
	return false;
}

/*!
 	Достает из узла состояние. Получает его владельца и
	вызывает SceneTreeView::SelectObjectInSceneView()
	\param hItem - Узел состояния
	\retval true - всегда
*/
bool SceneTreeView::SelectOwner(HTREEITEM hItem)
{
	qdGameObjectState* ptrState = 
		reinterpret_cast<qdGameObjectState*>(GetTreeCtrl().GetItemData(hItem));
	ASSERT(ptrState);
//	if(ptrState->owner()->named_object_type() != QD_NAMED_OBJECT_MOUSE_OBJ)
	SelectObjectInSceneView(static_cast<qdGameObject*>(ptrState->owner()));
	return true;
}

/*!
	На получение:
		выбирает объект в редакторе сцены. 
		редактор переводится в состояние редактирования сцены(VS_EDIT_SCENE)
	На потерю:
	 	убирает выделение,
		если шло редактирование маски, то прекращаем его.
	\param ptrObj	- Объект, узел которого получил/потерял выделение
	\param bIsObj	- Является ли \a ptrObj узателем на объект
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectStaticObjNode(qdNamedObject* ptrObj, 
										  bool bIsObj, bool bSelect)
{
	if (bSelect) {
		if (bIsObj) {
			SelectObjectInSceneView(static_cast<qdGameObject*>(ptrObj));
		}
		else
			SelectObjectInSceneView(NULL);
		m_ptrSceneView->SetViewState(VS_EDIT_SCENE);
		return true;
	}
	else 
	{	
		if (m_ptrSceneView->GetViewState() == VS_EDIT_MASK) {
			m_ptrSceneView->OnAbortStateMaskEdit(false);
		}
		else if (bIsObj) {
			SelectObjectInSceneView(NULL);
		}
	}
	return false;
}

/*!
	На получение:
		выбирает объект в редакторе сцены. 
		редактор переводится в состояние редактирования сцены(VS_EDIT_SCENE)
	На потерю:
		убирает выделение.
	\param ptrObj	- Объект, узел которого получил/потерял выделение
	\param bIsObj	- Является ли \a ptrObj узателем на объект
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectAnimatedObjNode2(qdNamedObject* ptrObj, 
											 bool bIsObj, 
											 bool bSelect)
{
	if (bSelect&&bIsObj)
	{
		switch(m_eFilterScheme) {
		case FS_NONE:
			m_ptrITView->SelectByFilter(tef_by_owner(ptrObj), 
				ITriggerView::SA_SELECT,
				ITriggerView::SM_EXCLUSIVE);
			break;
		}
		return true;
	}
	return false;
}

bool SceneTreeView::OnSelectAnimatedObjNode(qdNamedObject* ptrObj, 
											bool bIsObj, bool bSelect)
{
	if (bSelect) {
		if (bIsObj) {
			SelectObjectInSceneView(static_cast<qdGameObject*>(ptrObj));
		}
		else
			SelectObjectInSceneView(NULL);
		m_ptrSceneView->SetViewState(VS_EDIT_SCENE);
		return true;
	}
	else if (bIsObj) {
		SelectObjectInSceneView(NULL);
	}
	return false;
}

/*!
	На получение:
		выбирает объект в редакторе сцены. 
		редактор переводится в состояние редактирования сцены(VS_EDIT_SCENE)
		если персонаж глобальный, то на убираем выделение и переводим в состояние
		VS_NONE
	На потерю:
		убирает выделение
	
	\param ptrObj	- Объект, узел которого получил/потерял выделение
	\param bIsObj	- Является ли \a ptrObj узателем на объект
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectMovingObjNode(qdNamedObject* ptrObj, 
											bool bIsObj, bool bSelect)
{
	if (bSelect) {
		if (bIsObj) {
			if (IsGlobalObj(ptrObj)) {
				SelectObjectInSceneView(NULL);
				m_ptrSceneView->SetViewState(VS_NONE);
			}
			else
			{
				SelectObjectInSceneView(static_cast<qdGameObject*>(ptrObj));
				m_ptrSceneView->SetViewState(VS_EDIT_SCENE);
			}
		}
		else
		{
			if(IsObjUnderScene(GetTreeCtrl(),GetTreeCtrl().GetSelectedItem())){
				m_ptrSceneView->SetViewState(VS_EDIT_SCENE);
			}
			else
				m_ptrSceneView->SetViewState(VS_NONE);
			SelectObjectInSceneView(NULL);
		}
		return true;
	}
	else if (bIsObj) {
		SelectObjectInSceneView(NULL);
	}

	return false;
}

/*!
	На получение:
		Выделяет объекты в редакторе триггеров в зависимости от выбранного фильтра
		Выставляет левый верхний угол окна по прямоугольнику, охватывающему все 
		выделенные элменты
	\param ptrScene	- Объект, узел которого получил/потерял выделение
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectSceneNode2(qdGameScene* ptrScene, bool bSelect)
{
	//это такая штука, чтобы темплайт инстанциировать правильно
	typedef bool (*f)(const qdNamedObject* po);

	if (bSelect) 
	{
		switch(m_eFilterScheme) {
		case FS_NONE:
			m_ptrITView->SelectByFilter(tef_by_scene(ptrScene), 
				ITriggerView::SA_SELECT,
				ITriggerView::SM_EXCLUSIVE);
			break;
		case FS_ENTRY_FOR_DIALOGS:
			m_ptrITView->SelectByFilter(tef_dlg_state(ptrScene), 
				ITriggerView::SA_SELECT, 
				ITriggerView::SM_EXCLUSIVE);
			break;
		case FS_ENTRY_FOR_DYN_OBJ:
			m_ptrITView->SelectByFilter(tef_by_scenes_objs(ptrScene,
				ptree_helpers::is_obj_animated), 
				ITriggerView::SA_SELECT, 
				ITriggerView::SM_EXCLUSIVE);
			break;
		case FS_ENTRY_FOR_MOUSE:
			break;
		case FS_ENTRY_FOR_PERS:
			m_ptrITView->SelectByFilter(tef_by_scenes_objs(ptrScene,
				ptree_helpers::is_obj_moving), 
				ITriggerView::SA_SELECT, 
				ITriggerView::SM_EXCLUSIVE);
			break;
		}
		if (showTriggerElement_&&ptrScene->is_in_triggers())
			m_ptrITView->ShowTrigger(ptrScene);
		else
			m_ptrITView->ShowLTOfSelected(tef_by_scene(ptrScene));

	}
	return true;
}

/*!
	На получение:
 		Переводит редактор сцены в состояния редактирования игровой области
		(VS_EDIT_GAME_AREA)
		Выравнивает размеры битмапа в редакторе сцены
	\param ptrScene	- Объект, узел которого получил/потерял выделение
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectSceneNode(qdGameScene* ptrScene, bool bSelect)
{
	if(bSelect)
	{
		m_ptrSceneView->SetViewState(VS_EDIT_GAME_AREA);
		AdjustBitmapSize();
		return true;
	}
	return false;
}

/*!
	На получение:
		выбирает объект-владелец состояния в редакторе сцены. 
		Если состояние маска, то редактор переводится в состояние редактирования маски
		и вызывается метод OnEditStateMask()
		Если состояние другого типа редактор переводится в состояние редактирования 
			сцены(VS_EDIT_SCENE). Если владелец состояния мышь, то устанавливает её в
			центр экрана. Проигрывает анимацию


	На потерю:
		Если состояние маска, то останавливаем редактирование маски.
		Если обычное, то прикращаем прокручивать анимацию или редактировать размеры, 
		если они редактировались
	\param pno		- Объект, узел которого получил/потерял выделение
	\param bIsObj	- Является ли \a ptrObj узателем на объект
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectStateNode(qdNamedObject* pno, 
									  bool bIsObj, bool bSelect)
{
	if (bSelect) {
		if (bIsObj) {
			qdGameObjectState* ptrState = static_cast<qdGameObjectState*>(pno);
			SelectObjectInSceneView(static_cast<qdGameObject*>(ptrState->owner()));
			m_ptrSceneView->SetActiveState(ptrState);

			if (ptrState->state_type()==qdGameObjectState::STATE_MASK) {
				
				static_cast<qdGameObjectAnimated*>(
					ptrState->owner())->set_state(ptrState);
				m_ptrSceneView->SetViewState(VS_EDIT_MASK);
				m_ptrSceneView->SetSubTools(SSTE_EDIT_MASK);
				//состояния выставляются внутри
				m_ptrSceneView->OnEditStateMask(
							static_cast<qdGameObjectStateMask*>(ptrState));
				return true;
			}
			else
			{
				m_ptrSceneView->SetViewState(VS_EDIT_SCENE);
				if (m_ptrGameDisp->get_active_scene()) {
					if (ptree_helpers::is_obj_mouse(ptrState->owner()))
					{
						qdGameObjectMouse* const p = 
							static_cast<qdGameObjectMouse*>(ptrState->owner());
						qdCamera* const pcam = 
							m_ptrGameDisp->get_active_scene()->get_camera();
						p->set_pos(Vect3f(pcam->get_scr_center_x(),
							pcam->get_scr_center_y(), 0.f));
						p->set_default_pos(Vect3f(pcam->get_scr_center_x(),
							pcam->get_scr_center_y(), 0.f));

					}
					ShowStateAnimation(ptrState);
				}

				return false;
			}
		}
		m_ptrSceneView->SetViewState(VS_EDIT_SCENE);
		return true;
	}
	else
	{
		if (bIsObj) {
			m_ptrSceneView->SetActiveState(NULL);
			qdGameObjectState *ptrState = static_cast<qdGameObjectState*>(pno);
			if (ptrState->state_type()==qdGameObjectState::STATE_MASK) 
				m_ptrSceneView->OnAbortStateMaskEdit(true);
			else
			{
				if (m_ptrSceneView->GetViewState() == VS_EDIT_STATE) 
					EndEditStateDims(true);

				if (m_ptrPlayedState == ptrState)	
					StopAnimation(ptrState);
			}
		}
	}
	
	return false;
}

/*!
	Всегда, если есть активная сцена, выделяет объект в соответствии с установленным 
	типом фильтра
	На получение:
		Выделяет узел, соответствующий состоянию
	\param pno		- Объект, узел которого получил/потерял выделение
	\param bIsObj	- Является ли \a ptrObj узателем на объект
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
 	
*/
bool SceneTreeView::OnSelectStateNode2(qdNamedObject* pno, bool bIsObj, bool bSelect)
{
	//это такая штука, чтобы темплэйт инстанциировать правильно
	typedef bool (*f)(const qdNamedObject* po);

	qdGameScene * const ptrScene = m_ptrGameDisp->get_active_scene();
	if (bSelect&&ptrScene)
	{
		switch(m_eFilterScheme) {
		case FS_ENTRY_FOR_DIALOGS:
			m_ptrITView->SelectByFilter(
				tef_dlg_state(ptrScene), 
				ITriggerView::SA_SELECT, 
				ITriggerView::SM_EXCLUSIVE);
			break;
		case FS_ENTRY_FOR_DYN_OBJ:
			m_ptrITView->SelectByFilter(
				tef_by_scenes_objs(ptrScene,
				ptree_helpers::is_obj_animated), 
				ITriggerView::SA_SELECT, 
				ITriggerView::SM_EXCLUSIVE);
			break;
		case FS_ENTRY_FOR_PERS:
			m_ptrITView->SelectByFilter(
				tef_by_scenes_objs(ptrScene,
				ptree_helpers::is_obj_moving), 
				ITriggerView::SA_SELECT, 
				ITriggerView::SM_EXCLUSIVE);
			break;
		}
	}
	if (bSelect) 
	{
		m_ptrITView->SelectByFilter(
			tef_by_obj(pno), 
			ITriggerView::SA_SELECT,
			(FS_NONE == m_eFilterScheme)?
				ITriggerView::SM_EXCLUSIVE:
				ITriggerView::SM_INTERSECT);
	}
	if (bSelect&&showTriggerElement_&&pno->is_in_triggers())
		m_ptrITView->ShowTrigger(pno);
	return true;
}

/*!
	На получение:
		Если выделили узел зоны, то 
		- переводим редактор сцены в состояние редактирования сетки с уточнением - 
		редактирование зоны
		- копируем зону и запускаем для копии процедуру редакирования зоны. 
		Если выделили узел сетки, то переводим редактор сцены в состояние 
		редактирования сетки
	На потерю:
		Удаляем копию зоны, и вызываем прекращение редактирование зоны без созранения 
		данных
	\param ptrObj	- Объект, узел которого получил/потерял выделение
	\param bIsObj	- Является ли \a ptrObj узателем на объект
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectGridZoneNode(qdNamedObject* ptrObj,bool bIsObj,bool bSelect)
{
	if (bSelect) {
		if(bIsObj)
		{
			m_ptrSceneView->SetViewState(VS_EDIT_NET);
			m_ptrSceneView->SetSubTools(SSTE_EDIT_ZONE);
			qdGridZone* ptrZone = static_cast<qdGridZone*>(ptrObj);
			if (!m_ptrEditedGZ.get()) {
				m_ptrEditedGZ.reset(new qdGridZone);
			}
			*m_ptrEditedGZ.get() = *ptrZone;
			m_ptrSceneView->OnEditGridZone(m_ptrEditedGZ.get(), NULL);
		}
		else
		{
			ASSERT(m_ptrGameDisp->get_active_scene()
				&&(m_ptrGameDisp->get_active_scene()==
				reinterpret_cast<qdGameScene*>(GetTreeCtrl().
												GetItemData(m_hCurrScene))));
			
			m_ptrSceneView->SetViewState(VS_EDIT_NET);
			m_ptrSceneView->SetSubTools(SSTE_NONE);
		}
		return true;
	}
	else
	{
		m_ptrSceneView->OnAbortGridZoneEdit(false);
		//
		m_ptrEditedGZ.reset();
	}
	
	return false;
}

/*!
	На получение:
		редактор сцены переводится в состояние редактирования сцены(VS_EDIT_SCENE)
	На потерю:
	\param ptrObj	- Объект, узел которого получил/потерял выделение
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectMouseNode(qdNamedObject* ptrObj, bool bSelect)
{
	if (bSelect) {
		m_ptrSceneView->SetViewState(VS_EDIT_SCENE);
		return true;
	}
	return false;
}

/*!
	На получение:
		редактор переводится в состояние ничего неделанья(VS_NONE)
		Если это объект, то запускаем проигрывание видео
	На потерю:
		Если это объект, то останавливаем проигрывание видео

	\param ptrObj	- Объект, узел которого получил/потерял выделение
	\param bIsObj	- Является ли \a ptrObj узателем на объект
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectVideoNode(qdNamedObject* ptrObj, 
													bool bIsObj, bool bSelect)
{
	if (bSelect) {
		m_ptrSceneView->SetViewState(VS_NONE);
		if (bIsObj)
			PlayStopVideo(static_cast<qdVideo*>(ptrObj));
		return true;
	}
	else if (bIsObj)
		StopVideo(static_cast<qdVideo*>(ptrObj));

	return false;
}

/*!
	На получение:
		редактор сцены переводится в состояние ничего неделанья(VS_NONE)

	\param ptrObj	- Объект, узел которого получил/потерял выделение
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectGlobalNode(qdNamedObject* ptrObj, bool bSelect)
{
	if (bSelect) {
		m_ptrSceneView->SetViewState(VS_NONE);
		return true;
	}
	return false;
}

/*!
 	Копирует все цепочки в отдельный массив. Перевым элементом массива 
	кладет TE_SHOW_ALL. Передает для редактирования редактору триггеров этот 
	массив(ITriggerView::ShowTriggers()).
	
*/
void SceneTreeView::ShowAllTriggers(){
	const qdTriggerChainList& tcl = m_ptrGameDisp->trigger_chain_list();

	std::vector<qdTriggerChain*> vec;
	vec.reserve(tcl.size()+1);//это может быть не самая эффективная строчка

	std::copy(tcl.begin(), tcl.end(), std::back_inserter(vec));

	vec.insert(vec.begin(),const_cast<qdTriggerChain* const>(TE_SHOW_ALL));
	m_ptrITView->ShowTriggers(&*vec.begin(), vec.size());
	m_ptrITView->Redraw();
}

/*!
	На получение:
		Если это цепочка, то передаем её в редактор триггеров для редактирования
		Иначе отображаем все цепочки сразу
		Устанавливаем букву "Т" для элементов входщих в триггера

	\param ptrObj	- Объект, узел которого получил/потерял выделение
	\param bIsObj	- Является ли \a ptrObj узателем на объект
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectTriggerNode2(qdNamedObject* ptrObj, 
										 bool bIsObj, bool bSelect)
{
	if (bSelect) {
		if (bIsObj) {
			qdTriggerChain* ptrChain = static_cast<qdTriggerChain*>(ptrObj);
			m_ptrITView->ShowTriggers(&ptrChain, 1);
		}
		else
			ShowAllTriggers();
		SetTgrIcon4All(TRUE);
		return true;
	}
	return false;
}

/*!
	На получение:
		редактор сцены переводится в состояние ничего неделанья(VS_NONE)
		синхронизируются иконки рядом с узлами объектов, входящими в триггер(
		SceneTreeView::SetTgrIcon4All())
		если можно активировать редактор триггеров - он активируется.

	\param ptrObj	- Объект, узел которого получил/потерял выделение
	\param bIsObj	- Является ли \a ptrObj узателем на объект
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectTriggerNode(qdNamedObject* ptrObj, 
										bool bIsObj, bool bSelect)
{
	CWaitCursor __wait__;
	
	if (bSelect) {
		m_ptrSceneView->SetViewState(VS_NONE);
		SetTgrIcon4All(TRUE);

		if (m_bActivateTgrView&&m_IsSceneViewActive) 
			m_IsSceneViewActive = false;
		return true;
	}
	return false;
}

/*!
	На получение:
		Если объект:
		редактор сцены переводится в состояние редактирования инвентори(VS_EDIT_INVENTORY)
		выбирает объект-инвентори в редакторе сцены. 

		Если нет:
		редактор сцены переводится в состояние ничего неделанья(VS_NONE)
		
	На потерю:
		Устанавилвает NULL вместо выбранного объекта-инвентори в редакторе сцены. 
	\param ptrObj	- Объект, узел которого получил/потерял выделение
	\param bIsObj	- Является ли \a ptrObj узателем на объект
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectInventoryNode(qdNamedObject* ptrObj, 
										  bool bIsObj, bool bSelect)
{
	if (bSelect) {
		if (bIsObj)
		{
			m_ptrSceneView->SetViewState(VS_EDIT_INVENTORY);
			m_ptrSceneView->SelectInventory(static_cast<qdInventory*>(ptrObj));
		}
		else
			m_ptrSceneView->SetViewState(VS_NONE);
		return true;
	} else {
		if (bIsObj)
			m_ptrSceneView->SelectInventory(NULL);
	}
	return false;
}

/*!
	На получение:
		редактор сцены переводится в состояние ничего неделанья(VS_NONE)

	\param ptrObj	- Объект, узел которого получил/потерял выделение
	\param bIsObj	- Является ли \a ptrObj узателем на объект
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectMinigameNode(qdNamedObject* po, bool bIsObj, bool bSelect){
	if (bSelect) {
		m_ptrSceneView->SetViewState(VS_NONE);
		return true;
	} 
	return false;
}

/*!
	Определяет зону, которой принадлежит состояние и передает управление
	SceneTreeView::OnSelectGridZoneNode()
	\param hItem	- Узел состояния, получивший/потерявший выделение
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectGZStateNode(HTREEITEM hItem, bool bSelect){
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hPar = tree.GetParentItem(hItem);
	qdNamedObject* pno = reinterpret_cast<qdNamedObject*>(tree.GetItemData(hPar));
	bool res = OnSelectGridZoneNode(pno, true, bSelect);
	if (!m_IsSceneViewActive) 
	{
		qdNamedObject* state = reinterpret_cast<qdNamedObject*>(tree.GetItemData(hItem));
		if (bSelect&&showTriggerElement_&&state->is_in_triggers())
		{
			m_ptrITView->ShowTrigger(state);
			return true;
		}
	}
	return res;
}

/*!
	На получение:
		редактор переводится в состояние редактирования сцены(VS_EDIT_SCENE)
		Если объект, то запускаем музыкальный трек
	На потерю:
		Если объект, то останавливаем музыкальный трек
	\param ptrObj	- Объект, узел которого получил/потерял выделение
	\param bIsObj	- Является ли \a ptrObj узателем на объект
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectMusicNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect)
{
	if (bSelect)
	{
		m_ptrSceneView->SetViewState(VS_EDIT_SCENE);
		m_ptrSceneView->SetSubTools(SSTE_NONE);
		if (bIsObj)
			m_ptrGameDisp->play_music_track(static_cast<qdMusicTrack*>(ptrObj));
	}
	else if (bIsObj)
		m_ptrGameDisp->stop_music();

	return false;
}

/*!
	На получение:
		редактор переводится в состояние редактирования интерфейса(VS_EDIT_INTERFACE)
		выбирает привязанный к конечному экрану интерфейсный экран

	\param ptrObj	- Объект, узел которого получил/потерял выделение
	\param bIsObj	- Является ли \a ptrObj узателем на объект
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectGameEndNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect)
{
	if (bSelect)
	{
		if (bIsObj)
		{
			qdGameEnd* pge = static_cast<qdGameEnd*>(ptrObj);
			m_ptrSceneView->SetViewState(VS_EDIT_INTERFACE);
			m_ptrSceneView->SetSubTools(SSTE_NONE);
			return !SelectInterfaceScreen(pge->interface_screen());
		}
	}
	return false;
}

/*!
	На получение:
		редактор переводится в состояние редактирования интерфейса(VS_EDIT_INTERFACE)
 	
	\param pobj		- Объект, узел которого получил/потерял выделение
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectInterfaceNode(qdInterfaceObjectBase* pobj, bool bSelect)
{
	if (bSelect)
	{
		m_ptrSceneView->SetViewState(VS_EDIT_INTERFACE);
		m_ptrSceneView->SetSubTools(SSTE_NONE);

		return true;
	}
	return false;
}

/*!
	На получение:
		редактор переводится в состояние редактирования интерфейса(VS_EDIT_INTERFACE)
 		делает указанный экран активным(SceneTreeView::SelectInterfaceScreen())
	\param pobj		- Объект, узел которого получил/потерял выделение
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectInterfaceScreenNode(qdInterfaceObjectBase* pobj, 
													bool bSelect)
{
	if (bSelect)
	{
		m_ptrSceneView->SetViewState(VS_EDIT_INTERFACE);
		m_ptrSceneView->SetSubTools(SSTE_NONE);
		return !SelectInterfaceScreen(pobj->name());
	}
	return false;
}

/*!
	На получение:
		элемент помечается как выбранный
		редактор переводится в состояние редактирования интерфейса(VS_EDIT_INTERFACE)
 		делает экран-владелец активным(SceneTreeView::SelectInterfaceScreen())
	На потерю
		элемент помечается как невыбранный
	\param pobj		- Объект, узел которого получил/потерял выделение
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectInterfaceElementNode(qdInterfaceObjectBase* pobj, 
									bool bSelect)
{
	qdInterfaceScreen* const pscr = 
		static_cast<qdInterfaceScreen*>(pobj->owner());
	pscr->select_element(static_cast<qdInterfaceElement*>(pobj), bSelect);
	if (bSelect)
	{
		m_ptrSceneView->SetViewState(VS_EDIT_INTERFACE);
		m_ptrSceneView->SetSubTools(SSTE_NONE);
		return !SelectInterfaceScreen(pobj->owner()->name());
	}
	return false;
}

/*!
 	На получение:
 		редактор переводится в состояние редактирования интерфейса(VS_EDIT_INTERFACE)
 	 	делает экран-владелец активным(SceneTreeView::SelectInterfaceScreen())
		активизирует указанное состояние у элемента
 	\param pobj		- Объект, узел которого получил/потерял выделение
 	\param bSelect	- \a true - получил выделение.\a false - потерял
 	\retval true	- если надо перерисовать активный редактор
 	\retval false	- если не надо перерисовать активный редактор
*/
bool SceneTreeView::OnSelectInterfaceElementStateNode(qdInterfaceObjectBase* pobj, 
													bool bSelect)
{
	if (bSelect)
	{
		m_ptrSceneView->SetViewState(VS_EDIT_INTERFACE);
		m_ptrSceneView->SetSubTools(SSTE_NONE);
		SelectInterfaceScreen(pobj->owner()->owner()->name());

		if (qdInterfaceButton* const pbtn = 
			dynamic_cast<qdInterfaceButton*>(pobj->owner()))
		{
			int const num_states = pbtn->num_states();
			for(int i = 0; i < num_states; ++i)
				if (pbtn->get_state(i) == static_cast<qdInterfaceElementState*>(pobj))
				{
					pbtn->activate_state(i);
					break;
				}
		}
		return true;
	}
	return false;
}

/*!
	Получаем даныные хранящиеся в узел(CTreeCtrl::GetItemData()).
	Определяем объект это или каталог-объектов.
	В зависимости от типа объекта вызывается соответствующая OnSelect...() 
	со значение \a true для параметра \a bSelect.
	Если активен редактор триггеров, то вместо OnSelect...() вызываются 
	OnSelect...2(). Так же вставляется подменю в меню главного окна
	(SceneTreeView::InsertCorrespondingMenu()). Если требуется производится
	перерисовка окна редактора сцены или редактора триггеров
 	\param hItem - узел, который стал выделенным
*/
void SceneTreeView::OnSelchanged4New(HTREEITEM hItem)
{
	CTreeCtrl& tree = GetTreeCtrl();
	DWORD_PTR dwData = tree.GetItemData(hItem);
	
	//если это плохой указатель, значит ткнули в узел,
	//который есть классификационный узел:). Пр.: "Сцены", "Локации" и т.д.
	const bool bIsObj = is_obj(dwData);

	bool bNeedRedraw = false;
	if (bIsObj&&ptree_helpers::is_any_interf_obj(dwData).second) 
	{
		qdInterfaceObjectBase* pobj = 
			reinterpret_cast<qdInterfaceObjectBase*>(dwData);

		const type_info& td = typeid(*pobj);
		if (typeid(qdInterfaceDispatcher) == td)
			bNeedRedraw = OnSelectInterfaceNode(pobj, true);
		else if (typeid(qdInterfaceScreen) == td) 
			bNeedRedraw = OnSelectInterfaceScreenNode(pobj, true);
		else if (typeid(qdInterfaceButton) == td
				||typeid(qdInterfaceSlider) == td
				||typeid(qdInterfaceBackground) == td
				||typeid(qdInterfaceSave) == td
				||typeid(qdInterfaceTextWindow) == td) 
			bNeedRedraw = OnSelectInterfaceElementNode(pobj, true);
		else if (typeid(qdInterfaceElementState) == td)
			bNeedRedraw = OnSelectInterfaceElementStateNode(pobj, true);

		const DWORD dwMenuResID = GetCorrspondingMenuResID(pobj);

		if (bNeedRedraw) 
			sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	}
	else{
		qdNamedObject* const pno = reinterpret_cast<qdNamedObject*>(dwData);

		const qdNamedObjectType type = bIsObj 
							? static_cast<qdNamedObjectType>(pno->named_object_type())
							: static_cast<qdNamedObjectType>(dwData);
		
		const DWORD dwMenuResID = 
			GetCorrspondingMenuResID(!bIsObj, type, bIsObj && IsGlobalObj(pno));

		HTREEITEM hOldScene = m_hCurrScene;
		m_hCurrScene = IsObjUnderScene(tree, hItem);
		if (m_IsSceneViewActive) {
			if(m_hCurrScene)
			{
				bNeedRedraw = SelectScene(tree, m_hCurrScene);
				if (m_hCurrScene!=hOldScene) //значит сцену поменяли
					CollapseAllScenes(m_ptrGameDisp->get_active_scene());
			}
			
			switch(type) {
			case QD_NAMED_OBJECT_STATIC_OBJ:
				bNeedRedraw = OnSelectStaticObjNode(pno, bIsObj, true);
				break;
			case QD_NAMED_OBJECT_ANIMATED_OBJ:
				bNeedRedraw = OnSelectAnimatedObjNode(pno, bIsObj, true);
				break;
			case QD_NAMED_OBJECT_MOVING_OBJ:
				bNeedRedraw = OnSelectMovingObjNode(pno, bIsObj, true);
				break;
			case QD_NAMED_OBJECT_SCENE:
				//сцена к этому времени уже выбрана в диспетчере
				bNeedRedraw = OnSelectSceneNode(m_ptrGameDisp->get_active_scene(), true);
				break;
			case QD_NAMED_OBJECT_OBJ_STATE:
				bNeedRedraw = OnSelectStateNode(pno, true, true);
				break;
			case QE_STATE_CATALOG:
			case QE_MOVEMENT_STATE_CATALOG:
				bNeedRedraw = OnSelectStateNode(pno, false, true);
				break;
			case QD_NAMED_OBJECT_GRID_ZONE:
				bNeedRedraw = OnSelectGridZoneNode(pno, bIsObj, true);
				break;
			case QD_NAMED_OBJECT_MOUSE_OBJ:
				bNeedRedraw = OnSelectMouseNode(pno, true);
				break;
			case QD_NAMED_OBJECT_VIDEO:
				bNeedRedraw = OnSelectVideoNode(pno, bIsObj, true);
				break;			
			case QD_NAMED_OBJECT_DISPATCHER:
				bNeedRedraw = OnSelectGlobalNode(pno, true);
				break;
			case QD_NAMED_OBJECT_TRIGGER_CHAIN:
				bNeedRedraw = OnSelectTriggerNode(pno, bIsObj, true);
				break;
			case QD_NAMED_OBJECT_INVENTORY:
				bNeedRedraw = OnSelectInventoryNode(pno, bIsObj, true);
				break;
			case QD_NAMED_OBJECT_MINIGAME:
				bNeedRedraw = OnSelectMinigameNode(pno, bIsObj, true);
				break;
			case QD_NAMED_OBJECT_GRID_ZONE_STATE:
				bNeedRedraw = OnSelectGZStateNode(hItem, true);
				break;
			case QD_NAMED_OBJECT_MUSIC_TRACK:
				bNeedRedraw = OnSelectMusicNode(pno, bIsObj, true);
				break;
			case QD_NAMED_OBJECT_GAME_END:
				bNeedRedraw = OnSelectGameEndNode(pno, bIsObj, true);
				break;
			}
			if (bNeedRedraw) 
				sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
		}
		else
		{
			if (m_hCurrScene&&m_hCurrScene!=hOldScene) {//значит сцену поменяли
				qdGameScene* pscene = 
					reinterpret_cast<qdGameScene*>(tree.GetItemData(m_hCurrScene));
				CollapseAllScenes(pscene);
			}
			switch(type) {
			case QD_NAMED_OBJECT_TRIGGER_CHAIN:
				bNeedRedraw = OnSelectTriggerNode2(pno, bIsObj, true);
				break;
			case QD_NAMED_OBJECT_OBJ_STATE:
				bNeedRedraw = OnSelectStateNode2(pno, bIsObj, true);
				break;
			case QD_NAMED_OBJECT_SCENE:
				bNeedRedraw = OnSelectSceneNode2(static_cast<qdGameScene*>(pno), true);
				break;
			case QD_NAMED_OBJECT_ANIMATED_OBJ:
			case QD_NAMED_OBJECT_MOVING_OBJ:
				bNeedRedraw = OnSelectAnimatedObjNode2(pno, bIsObj, true);
				break;
			case QD_NAMED_OBJECT_GRID_ZONE_STATE:
				bNeedRedraw = OnSelectGZStateNode(hItem, true);
				break;
			}
			if (bNeedRedraw) 
				m_ptrITView->Redraw();
		}
	}
}

/*!
	Получаем даныные хранящиеся в узел(CTreeCtrl::GetItemData()).
	Определяем объект это или каталог-объектов.
	В зависимости от типа объекта вызывается соответствующая OnSelect...() 
	со значение \a false для параметра \a bSelect.
	Если активен редактор триггеров, то вместо OnSelect...() вызываются 
	OnSelect...2(). Если требуется производится
	перерисовка окна редактора сцены или редактора триггеров.
	\param hItem - узел, который перестал быть выделенным
*/
void SceneTreeView::OnSelchanged4Old(HTREEITEM hItem)
{
	CTreeCtrl& tree = GetTreeCtrl();
	DWORD_PTR dwData = tree.GetItemData(hItem);
	
	//если это плохой указатель, значит ткнули в узел,
	//который есть классификационный узел:). Пр.: "Сцены", "Локации" и т.д.
	const bool bIsObj = is_obj(dwData);
	
	bool bNeedRedraw = false;
	if (bIsObj&&ptree_helpers::is_any_interf_obj(dwData).second) 
	{
		qdInterfaceObjectBase* pobj = 
			reinterpret_cast<qdInterfaceObjectBase*>(dwData);

		const type_info& td = typeid(*pobj);
		if (typeid(qdInterfaceDispatcher) == td)
			bNeedRedraw = OnSelectInterfaceNode(pobj, false);
		else if (typeid(qdInterfaceScreen) == td) 
			bNeedRedraw = OnSelectInterfaceScreenNode(pobj, false);
		else if (typeid(qdInterfaceButton) == td
			||typeid(qdInterfaceSlider) == td
			||typeid(qdInterfaceBackground) == td
			||typeid(qdInterfaceSave) == td
			||typeid(qdInterfaceTextWindow) == td) 
			bNeedRedraw = OnSelectInterfaceElementNode(pobj, false);
		else if (typeid(qdInterfaceElementState) == td)
			bNeedRedraw = OnSelectInterfaceElementStateNode(pobj, false);

		if (bNeedRedraw) 
			sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	}
	else{
		qdNamedObject * const pno = reinterpret_cast<qdNamedObject*>(dwData);
		qdNamedObjectType const type = bIsObj 
			?static_cast<qdNamedObjectType>(pno->named_object_type()) 
			: static_cast<qdNamedObjectType>(dwData);
		
		if (m_IsSceneViewActive) {
			switch(type) {
			case QD_NAMED_OBJECT_STATIC_OBJ:
				bNeedRedraw = OnSelectStaticObjNode(pno, bIsObj, false);
				break;
			case QD_NAMED_OBJECT_ANIMATED_OBJ:
				bNeedRedraw = OnSelectAnimatedObjNode(pno, bIsObj, false);
				break;
			case QD_NAMED_OBJECT_MOVING_OBJ:
				bNeedRedraw = OnSelectMovingObjNode(pno, bIsObj, false);
				break;
			case QD_NAMED_OBJECT_SCENE:
				bNeedRedraw = OnSelectSceneNode(m_ptrGameDisp->get_active_scene(), false);
				break;
			case QD_NAMED_OBJECT_OBJ_STATE:
				bNeedRedraw = OnSelectStateNode(pno, true, false);
				break;
			case QE_STATE_CATALOG:
			case QE_MOVEMENT_STATE_CATALOG:
				bNeedRedraw = OnSelectStateNode(pno, false, false);
				break;
			case QD_NAMED_OBJECT_GRID_ZONE:
				bNeedRedraw = OnSelectGridZoneNode(pno, bIsObj, false);
				break;
			case QD_NAMED_OBJECT_MOUSE_OBJ:
				bNeedRedraw = OnSelectMouseNode(pno, false);
				break;
			case QD_NAMED_OBJECT_VIDEO:
				bNeedRedraw = OnSelectVideoNode(pno, bIsObj, false);
				break;			
			case QD_NAMED_OBJECT_DISPATCHER:
				bNeedRedraw = OnSelectGlobalNode(pno, false);
				break;
			case QD_NAMED_OBJECT_TRIGGER_CHAIN:
				bNeedRedraw = OnSelectTriggerNode(pno, bIsObj, false);
				break;
			case QD_NAMED_OBJECT_INVENTORY:
				bNeedRedraw = OnSelectInventoryNode(pno, bIsObj, false);
				break;
			case QD_NAMED_OBJECT_MINIGAME:
				bNeedRedraw = OnSelectMinigameNode(pno, bIsObj, false);
				break;
			case QD_NAMED_OBJECT_GRID_ZONE_STATE:
				bNeedRedraw = OnSelectGZStateNode(hItem, false);
				break;
			case QD_NAMED_OBJECT_MUSIC_TRACK:
				bNeedRedraw = OnSelectMusicNode(pno, bIsObj, false);
				break;
			case QD_NAMED_OBJECT_GAME_END:
				bNeedRedraw = OnSelectGameEndNode(pno, bIsObj, true);
				break;
			}
			if (bNeedRedraw) 
				sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
		}
		else
		{
			switch(type) {
			case QD_NAMED_OBJECT_TRIGGER_CHAIN:
				bNeedRedraw = OnSelectTriggerNode2(pno, bIsObj, false);
				break;
			case QD_NAMED_OBJECT_OBJ_STATE:
				bNeedRedraw = OnSelectStateNode2(pno, bIsObj, false);
				break;
			case QD_NAMED_OBJECT_SCENE:
				bNeedRedraw = OnSelectSceneNode2(static_cast<qdGameScene*>(pno), false);
				break;
			case QD_NAMED_OBJECT_ANIMATED_OBJ:
			case QD_NAMED_OBJECT_MOVING_OBJ:
				bNeedRedraw = OnSelectAnimatedObjNode2(pno, bIsObj, true);
				break;
			}
			if (bNeedRedraw) 
				m_ptrITView->Redraw();
		}
	}
}

void SceneTreeView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* ptv = (NM_TREEVIEW*)pNMHDR;
	
	if(ptv->itemOld.hItem)
		OnSelchanged4Old(ptv->itemOld.hItem);
	if(ptv->itemNew.hItem)
		OnSelchanged4New(ptv->itemNew.hItem);

	*pResult = 0;
}

/*!
 	Для выравнивания вызывает CChildView::AdjustBitmapSize()
*/
void SceneTreeView::AdjustBitmapSize()
{
	m_ptrSceneView->AdjustBitmapSize();
}

void SceneTreeView::OnProperies() 
{
	thread_suspender __ts(this);

	CTreeCtrl & tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	ASSERT(hItem);

	bool bNeedRedraw = false;
	qdNamedObjectBase* pnob = 
		reinterpret_cast<qdNamedObjectBase*>(tree.GetItemData(hItem));
	if (ptree_helpers::is_node_type(tree, hItem, QD_NAMED_OBJECT_GRID_ZONE))
	{
			if (m_ptrEditedGZ.get()) 
				pnob = m_ptrEditedGZ.get();
	}
	ShowProperties(pnob, 0);

	if(bNeedRedraw)
		sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

bool EditInterfBtnState(qdInterfaceElementState* pstate, 
						qdInterfaceElementState::state_mode_t state_mode);

/*!
 	Передает управление SceneTreeView::ShowCorrespondingPropDlg(). Если она вернула
	\a true, то производит обновления изображений у узлов в дереве
	\param ptrObj		- объект, свойства которого будем редактировать.
	\param iActivePage	- активная страница свойств
	\return \a true - если SceneTreeView::ShowCorrespondingPropDlg() вернула \a true
*/
bool SceneTreeView::ShowProperties(qdNamedObjectBase* ptrObj, int iActivePage)
{
	if (!ptrObj) {
		ASSERT(0);
		return false;
	}

	bool bIsObj = is_obj(reinterpret_cast<DWORD_PTR>(ptrObj));
	if(ShowCorrespondingPropDlg(ptrObj, iActivePage, bIsObj))
	{
		if (bIsObj){
			if (qdNamedObject* pno = dynamic_cast<qdNamedObject*>(ptrObj)) 
			{
				if(ptree_helpers::is_obj_state(pno)) 
				{
					qdGameObjectState* pstate = static_cast<qdGameObjectState*>(pno);

					UpdateStateImage(pstate, NULL);
//					if (HasGlobalFlag(pstate)) 
//						UpdateStateImage4AllScenes(pstate);
					SuspendAndClear();
					SetAndResume(pstate);
				}
				else if (ptree_helpers::is_game_obj(pno))
					UpdateObjImage(static_cast<qdGameObject*>(pno));
				else if (ptree_helpers::is_obj_zone(pno))
				{
					int iImage = 
						ptree_helpers::SelGZonePic(static_cast<qdGridZone*>(pno));
					GetTreeCtrl().SetItemImage(FindObjectItem(pno), iImage, iImage);
				}
			}
			else
			{

			}
		}
		SetProjChange();
		sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
		return true;
	}
	return false;
}

bool ShowPropDlg(CPropertyPage** ptrPages, int iActive, UINT uiCaption)
{
	CMyPropertySheet propSheet(ptrPages, iActive, uiCaption);
	int res = propSheet.DoModal();
	return (res == IDOK);
}

/*!
 	Определяет тип родителя объекта(qdNamedObject или qdInterfaceObjectBase) и в 
	зависимости от него вызывает 
	SceneTreeView::ShowCorrespondingPropDlg(qdNamedObject* ptrObj, int iActive, bool bIsObj)
	или
	SceneTreeView::ShowCorrespondingPropDlg(qdInterfaceObjectBase* ptrObj, int iActive)
	\param ptrObj	- объект, свойства которого будут редактироваться
	\param iActive	- номер странички, которая будет вынесена на передний план.
	\param bIsObj	- флаг, указывающий содержит ли ptrObj указатель на объект или 
						идентификатор типа.
	\return \a true - если в диалоге свойств нажали ОК
*/
bool SceneTreeView::ShowCorrespondingPropDlg(qdNamedObjectBase* ptrObj, 
											int iActive, bool bIsObj)
{
	if (!bIsObj||dynamic_cast<qdNamedObject*>(ptrObj))
	{
		return ShowCorrespondingPropDlg(static_cast<qdNamedObject*>(ptrObj),
							iActive, bIsObj);
	}
	else
		return ShowCorrespondingPropDlg(static_cast<qdInterfaceObjectBase*>(ptrObj), 
								iActive);

}

/*!
 	Определяет динамический тип объекта через type_info Формирует набор страничек 
	в соответствии с типом и вызывает ShowPropDlg для отображения диалога.
	\param ptrObj - объект, свойства которого будут редактироваться
	\param iActive	- номер странички, которая будет вынесена на передний план.
	\return \a true - если в диалоге свойств нажали ОК
*/

bool SceneTreeView::ShowCorrespondingPropDlg(qdInterfaceObjectBase* ptrObj, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	const type_info& ti = typeid(*ptrObj);
	if (typeid(qdInterfaceButton) == ti)
	{
		std::auto_ptr<InterfBtnProp> ptrPosPage(new InterfBtnProp);
		ptrPosPage->set_data(qdInterfaceDispatcher::get_dispatcher(), 
			static_cast<qdInterfaceButton*>( ptrObj));
		ptrPages[0] = ptrPosPage.get();
		return ShowPropDlg(ptrPages, iActive, IDS_INTERF_BTN_PROPERTIES);
	}
	else if (typeid(qdInterfaceSlider) == ti)
	{
		std::auto_ptr<InterfSldrProp> ptrPosPage(new InterfSldrProp);
		ptrPosPage->set_data(static_cast<qdInterfaceSlider*>( ptrObj));
		ptrPages[0] = ptrPosPage.get();

		std::auto_ptr<InterfSldrStateProp> 
			ptrSSp1(new InterfSldrStateProp(SC_SLIDER, IDS_CAP_SLIDER_FORGRND_PROP));
		ptrSSp1->set_data(static_cast<qdInterfaceSlider*>(ptrObj));
		ptrPages[1] = ptrSSp1.get();

		std::auto_ptr<InterfSldrStateProp> 
			ptrSSp2(new InterfSldrStateProp(SC_BACK, IDS_CAP_SLIDER_BACKGRND_PROP));
		ptrSSp2->set_data(static_cast<qdInterfaceSlider*>(ptrObj));
		ptrPages[2] = ptrSSp2.get();

		return ShowPropDlg(ptrPages, iActive, IDS_INTERF_SLIDER_PROPERTIES);
	}
	else if (typeid(qdInterfaceBackground) == ti)
	{
		std::auto_ptr<InterfBackPP> ptrPosPage(new InterfBackPP);
		ptrPosPage->set_data(static_cast<qdInterfaceBackground*>( ptrObj));
		ptrPages[0] = ptrPosPage.get();
		return ShowPropDlg(ptrPages, iActive, IDS_INTERF_BACK_PROPERTIES);
	}
	else if (typeid(qdInterfaceSave) == ti)
	{
		std::auto_ptr<InterfSaveProp> ptrISPage(new InterfSaveProp);
		ptrISPage->set_data(static_cast<qdInterfaceSave*>(ptrObj));
		ptrPages[0] = ptrISPage.get();
		return ShowPropDlg(ptrPages, iActive, IDS_INTERF_SAVE_PROPERTIES);
	}
	else if (typeid(qdInterfaceElementState) == ti)
	{
		return EditInterfBtnState(static_cast<qdInterfaceElementState*>(ptrObj),
								qdInterfaceElementState::DEFAULT_MODE);
	}
	else if (typeid(qdInterfaceScreen) == ti)
	{
		std::auto_ptr<InterfScreenProp> ptrISPage(new InterfScreenProp);
		ptrISPage->set_data(static_cast<qdInterfaceScreen*>(ptrObj));
		ptrPages[0] = ptrISPage.get();
		return ShowPropDlg(ptrPages, iActive, IDS_INTERF_SCREEN_PROPERTIES);
	}
	else if (typeid(qdInterfaceTextWindow) == ti)
	{
		std::auto_ptr<InterfaceTextWindowPP> ptrTextWindow(new InterfaceTextWindowPP);
		ptrTextWindow->set_data(static_cast<qdInterfaceTextWindow*>(ptrObj));
		ptrPages[0] = ptrTextWindow.get();
		return ShowPropDlg(ptrPages, iActive, IDS_INTERF_TEXT_WINDOW_PROPERTIES);
	}
	return false;
}

/*!
 	Определяет тип объекта через qdNamedObject::named_object_type(), если bIsObj = true
	или как (long)ptrObj, если bIsObj = false. Формирует набор страничек 
	в соответствии с типом и вызывает ShowPropDlg для отображения диалога.
	\param ptrObj - объект, свойства которого будут редактироваться
	\param iActive	- номер странички, которая будет вынесена на передний план.
	\param bIsObj	- флаг, указывающий содержит ли ptrObj указатель на объект или 
					идентификатор типа.
	\return \a true - если в диалоге свойств нажали ОК
*/
bool SceneTreeView::ShowCorrespondingPropDlg(qdNamedObject* ptrObj, 
												int iActive, bool bIsObj)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	long _type = bIsObj ?ptrObj->named_object_type(): 
					reinterpret_cast<DWORD_PTR>(ptrObj);
	
	qdGameScene * ptrSelctedScene = NULL;
	if (m_hCurrScene)
		ptrSelctedScene = 
			reinterpret_cast<qdGameScene*>(GetTreeCtrl().GetItemData(m_hCurrScene));
	switch(_type) {
	case QD_NAMED_OBJECT_SCENE:
		{
			std::auto_ptr<ScreenProp> ptrSP (new ScreenProp);
			ptrPages[0] = ptrSP.get();
			ptrSP->set_data(m_ptrGameDisp, ptrSelctedScene);
			
			return ShowPropDlg(ptrPages, 0, IDS_PPCAP_SCENE_PROPERTIES);
		}
		break;
	case QD_NAMED_OBJECT_GRID_ZONE:
		{
			if (bIsObj) {
				std::auto_ptr<ZonePropPage> ptrZPP ( new ZonePropPage);
				ptrPages[0] = ptrZPP.get();

				ptrZPP->set_data(m_ptrGameDisp, static_cast<qdGridZone*>(ptrObj));
				if(ShowPropDlg(ptrPages, 1, IDS_GRID_ZONE_PROPERTIES))
				{
					OnSaveGZChanges(true);
					ZonePropPage::ReapplyZones
							(static_cast<qdGameScene*>(ptrObj->owner()));
					return true;
				}
				ZonePropPage::ReapplyZones
							(static_cast<qdGameScene*>(ptrObj->owner()));
				return false;
			}
			else
			{
				std::auto_ptr<BasePlanePage> ptrBPP(new BasePlanePage);
				ptrPages[0] = ptrBPP.get();
				ptrBPP->set_data(m_ptrGameDisp, ptrSelctedScene);
				return ShowPropDlg(ptrPages, 0, IDS_PPCAP_NET_PROPERTIES);
			}
		}
	case QD_NAMED_OBJECT_STATIC_OBJ:
		{
			ASSERT(ptrObj->owner()->named_object_type() == QD_NAMED_OBJECT_SCENE);

			std::auto_ptr<StaticObjPage> pPage ( new StaticObjPage);
			pPage->set_data(m_ptrGameDisp,static_cast<qdGameObjectStatic*>(ptrObj));
			ptrPages[0] = pPage.get();

			std::auto_ptr<ParalaxPage> pParalax ( new ParalaxPage);
			pParalax->set_data(static_cast<qdGameObject*>(ptrObj));
			ptrPages[1] = pParalax.get();

			return ShowPropDlg(ptrPages, iActive, IDS_PPCAP_STATIC_OBJ_PROPERTIES);
		}
		break;
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
		{
			qdGameObjectAnimated* pao = 
				static_cast<qdGameObjectAnimated*>(ptrObj);
			std::auto_ptr<ObjPosPropPage> pAPage ( new ObjPosPropPage);
			pAPage->set_data(m_ptrGameDisp, pao);
			ptrPages[0] = pAPage.get();

			std::auto_ptr<AnimObjStates> ptrAOS (new AnimObjStates);
			ptrAOS->set_data(m_ptrGameDisp, pao);
			ptrPages[1] = ptrAOS.get();

			std::auto_ptr<ParalaxPage> pParalax ( new ParalaxPage);
			pParalax->set_data((qdGameObject*)ptrObj);
			ptrPages[2] = pParalax.get();

			std::auto_ptr<InventoryPP> pIPP(new InventoryPP);
			pIPP->set_data(m_ptrGameDisp, pao);
			ptrPages[3] = pIPP.get();
			return ShowPropDlg(ptrPages, iActive, IDS_PPCAP_ANIMATED_OBJ_PROPERTIES);
		}
	case QD_NAMED_OBJECT_MOVING_OBJ:
		{
			if (IsGlobalObj(ptrObj)) {
				std::auto_ptr<CharacterGaitsPage> ptrCPP ( new CharacterGaitsPage);
				ptrCPP->set_data(m_ptrGameDisp, (qdGameObjectMoving*)ptrObj);
				ptrPages[0] = ptrCPP.get();
				
				std::auto_ptr<AnimObjStates> ptrAOS 
					( new AnimObjStates(AnimObjStates::IDD, IDS_PROPPAGE_CAPTION_STOPS));
				ptrAOS->set_data(m_ptrGameDisp, (qdGameObjectAnimated*)ptrObj);
				ptrPages[1] = ptrAOS.get();
/*
				std::auto_ptr<InventoryPPForPers> ptrIPP( new InventoryPPForPers);
				ptrIPP->set_data(m_ptrGameDisp, (qdGameObjectMoving*)ptrObj);
				ptrIPP->set_global_obj();
				ptrPages[2] = ptrIPP.get();
*/				
//				std::auto_ptr<CharBound> ptrCB ( new CharBound);
//				ptrCB->set_data(m_ptrGameDisp, (qdGameObjectMoving*)ptrObj);
//				ptrPages[3] = ptrCB.get();

				return ShowPropDlg(ptrPages, iActive, IDS_PPCAP_MOVING_OBJ_PROPERTIES);
			}
			else
			{
				std::auto_ptr<ObjPosPropPage> pAPage (new ObjPosPropPage);
				pAPage->set_data(m_ptrGameDisp,
					static_cast<qdGameObjectMoving*>(ptrObj));
				ptrPages[0] = pAPage.get();
				
				std::auto_ptr<CharacterGaitsPage> ptrCPP( new CharacterGaitsPage);
				ptrCPP->set_data(m_ptrGameDisp, 
					static_cast<qdGameObjectMoving*>(ptrObj));
				ptrPages[1] = ptrCPP.get();

				std::auto_ptr<AnimObjStates> ptrAOS 
					( new AnimObjStates(AnimObjStates::IDD, IDS_PROPPAGE_CAPTION_STOPS));

				ptrAOS->set_data(m_ptrGameDisp, 
					static_cast<qdGameObjectMoving*>(ptrObj));
				ptrPages[2] = ptrAOS.get();

				std::auto_ptr<CharBound> ptrCB ( new CharBound);
				ptrCB->set_data(m_ptrGameDisp, static_cast<qdGameObjectMoving*>(ptrObj));
				ptrPages[3] = ptrCB.get();
				
				std::auto_ptr<InventoryPPForPers> ptrIPP( new InventoryPPForPers);
				ptrIPP->set_data(m_ptrGameDisp, 
					static_cast<qdGameObjectMoving*>(ptrObj));
				ptrPages[4] = ptrIPP.get();

				std::auto_ptr<PersonageControlPropPage> 
					ptrPCP( new PersonageControlPropPage);
				ptrPCP->set_data(static_cast<qdGameObjectMoving*>(ptrObj));
				ptrPages[5] = ptrPCP.get();

//				if (ptree_helpers::IsCopyOfGlobal(m_ptrGameDisp, ptrObj->name()))
//					ptrIPP->set_disable_all();

				return ShowPropDlg(ptrPages, iActive, IDS_PPCAP_MOVING_OBJ_PROPERTIES);
			}
		}
	case QD_NAMED_OBJECT_MOUSE_OBJ:
		{
			std::auto_ptr<AnimObjStates> ptrAOS ( new AnimObjStates);
			ptrAOS->set_data(m_ptrGameDisp, static_cast<qdGameObjectAnimated*>(ptrObj));
			ptrPages[0] = ptrAOS.get();
			return ShowPropDlg(ptrPages, iActive, IDS_PPCAP_MOUSE_OBJ_PROPERTIES);
		}
	case QD_NAMED_OBJECT_OBJ_STATE:
			return EditState(static_cast<qdGameObjectState*>(ptrObj));
	case QD_NAMED_OBJECT_VIDEO:
		{
			std::auto_ptr<VideoPage> ptrPage(new VideoPage);
			ptrPage->set_data(m_ptrGameDisp, static_cast<qdVideo*>(ptrObj));
			ptrPages[0] = ptrPage.get();
			return ShowPropDlg(ptrPages, iActive,IDS_PPCAP_VIDEO_PROPERTIES);
		}
	case QD_NAMED_OBJECT_INVENTORY:
		{
			std::auto_ptr<InventoryPropPage> ptrPage(new InventoryPropPage);
			if (bIsObj)
				ptrPage->set_data(m_ptrGameDisp, static_cast<qdInventory*>(ptrObj));
			else
				ptrPage->set_data(m_ptrGameDisp, NULL);
			ptrPages[0] = ptrPage.get();
			return ShowPropDlg(ptrPages, iActive, IDS_PPCAP_INVENTORY_PROPERTIES);
		}
	case QD_NAMED_OBJECT_COUNTER:
		{
			std::auto_ptr<CounterPropPage> ptrPage(new CounterPropPage);
			ptrPage->set_data(m_ptrGameDisp, static_cast<qdCounter*>(ptrObj));
			ptrPages[0] = ptrPage.get();
			return ShowPropDlg(ptrPages, iActive, IDS_PPCAP_COUNTER_PROPERTIES);
		}
	case QD_NAMED_OBJECT_MUSIC_TRACK:
		{
			std::auto_ptr<MusicTrackPropPage> ptrPage(new MusicTrackPropPage);
			ptrPage->set_data(static_cast<qdMusicTrack*>(ptrObj));
			ptrPages[0] = ptrPage.get();
			return ShowPropDlg(ptrPages, iActive, IDS_PPCAP_MUSIC_TRACK_PROPERTIES);
		}
	case QD_NAMED_OBJECT_FONT_INFO:
		{
			if (bIsObj)
			{
				std::auto_ptr<FontPropPage> ptrPage(new FontPropPage);
				ptrPage->set_data(m_ptrGameDisp, static_cast<qdFontInfo*>(ptrObj));
				ptrPages[0] = ptrPage.get();
				return ShowPropDlg(ptrPages, iActive, IDS_PPCAP_FONT_PROPERTIES);
			}
			else
			{
				std::auto_ptr<FontsPropPage> ptrPage(new FontsPropPage);
				ptrPage->set_data(m_ptrGameDisp);
				ptrPages[0] = ptrPage.get();
				return ShowPropDlg(ptrPages, iActive, IDS_PPCAP_FONTS_PROPERTIES);
			}
		}
	default:
		return false;
	}
}

/*!
	Определяет тип родителя объекта(qdNamedObject или qdInterfaceObjectBase) и в 
	зависимости от него вызывает ptree_helpers::CreateTreePath(const qdNamedObject* pobj)
	или  ptree_helpers::CreateTreePath(const qdInterfaceObjectBase* pobj)
*/
std::auto_ptr<CStringArray> SceneTreeView::CreateTreePath(qdNamedObjectBase const* pobj)
{
	if (const qdNamedObject* pno = dynamic_cast<const qdNamedObject*>(pobj)) 
		return ptree_helpers::CreateTreePath(pno);
	else
		return ptree_helpers::CreateTreePath(static_cast<const qdInterfaceObjectBase*>(pobj));
}

/*!
 	Создает путь от объект к корню(SceneTreeView::CreateTreePath(
	qdNamedObjectBase const* pobj)). Затем спускается вниз от корня к объекту
	\param ptrObj - объект, для которого ищем соответствие
	\return описатель узла, если он найден. NULL в противном случае
*/
HTREEITEM SceneTreeView::FindObjectItem(const qdNamedObjectBase* ptrObj)
{
	ASSERT(ptrObj);
	std::auto_ptr<CStringArray> pArr = CreateTreePath(ptrObj);
	
	HTREEITEM hPar = NULL;
	for(int j = pArr->GetSize(); --j>=0;)
	{
		hPar = FindTreeItemByText(GetTreeCtrl(), hPar, pArr->GetAt(j));
	}
	return hPar;
}

/*!
 	Находит узел соответствующий объекту(SceneTreeView::FindObjectItem()).
	Показывает узел (SceneTreeView::ShowItem()). Если узел не найден, то выделение в 
	дереве не меняется. Если \a ptrObj - NULL, то удаляем выделение(
	CTreeCtrl::SelectItem(NULL))
	\param ptrObj - объект, который надо показать в дереве
	\return \a true - если операция прошла успешно
*/
bool SceneTreeView::ShowObjectInTree(qdNamedObjectBase const* ptrObj)
{
	if(ptrObj)
	{
		HTREEITEM hItem = FindObjectItem(ptrObj);
		if(hItem)
		{
			ShowItem(hItem);
			return true;
		}
		return false;
	}
	else
		GetTreeCtrl().SelectItem(NULL);
	return true;
}


void SceneTreeView::OnAnimationList() 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	ASSERT(hItem);

	hItem = GetDispatcherNode(GetTreeCtrl(), hItem);
	ShowAnimationCatalogDlg(hItem, GOPA_SHOW, true);
}

/*!
 	Выбирает узел(CTreeCtrl::SelectItem()).
	Делает узел видимым(CTreeCtrl::EnsureVisible())
	\param hItem	- узел
	\return \a true - если удалось
*/
bool SceneTreeView::ShowItem(HTREEITEM hItem)
{
	GetTreeCtrl().SelectItem(hItem);
	return (GetTreeCtrl().EnsureVisible(hItem) == TRUE);
}

void SceneTreeView::OnCrtScene() 
{
	if(AddScene(CString(), true))
	{
		//узел только что созданной сцены будет выбран в дереве
		CTreeCtrl& tree = GetTreeCtrl();
		HTREEITEM hSel = tree.GetSelectedItem();
		ASSERT(hSel);

		qdGameScene* ps = reinterpret_cast<qdGameScene*>(tree.GetItemData(hSel));
		CreateBackground(hSel, ps);
	}
}

void SceneTreeView::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	
	*pResult = IsContainerObj(GetTreeCtrl(), pTVDispInfo->item.hItem);
	if(!*pResult)
	{
		DWORD_PTR const dwData = GetTreeCtrl().GetItemData(pTVDispInfo->item.hItem);
		std::pair<qdNamedObjectBase*, bool> r = is_any_interf_obj(dwData);
		if (r.second)
		{
		}
		else{
			qdNamedObject* pno = reinterpret_cast<qdGameObject*>(dwData);
//			if (qdConditionalObject const* pco 
//				= dynamic_cast<qdConditionalObject const*>(pno))
//			{
//				if (pco->is_in_triggers())
//				{
//					*pResult = 1;
//					return;
//				}
//			}

			if (ptree_helpers::IsGameObject(pno)) {
				if (!IsGlobalObj(static_cast<qdGameObject*>(pno))) {
					*pResult = IsCopyOfGlobal(m_ptrGameDisp, pno->name())? 1: 0;
				}
			}
			else if (pno->named_object_type()==QD_NAMED_OBJECT_OBJ_STATE) {
				*pResult = IsGlobalState(pno,pTVDispInfo->item.hItem);
			}
			else if (pno->named_object_type() == QD_NAMED_OBJECT_GRID_ZONE_STATE){
				*pResult = 1;
			}
		}
	}
}

/*!
 	Тест прозиводится функцией bool uniq_name(LPCTSTR strText, const _Cont& _c).
	\param strName	- имя для теста
	\param pobj		- объект, которому это имя предназначается
	\return \a true - если имя уникальное
*/
bool SceneTreeView::CorrespondingTestName(LPCTSTR strName, qdInterfaceObjectBase* pobj)
{
	const type_info& ti = typeid(*pobj);
	if (ti == typeid(qdInterfaceScreen))
	{
		qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
		return uniq_name(strName, pd->screen_list());
	}
	else if (ti == typeid(qdInterfaceButton)
		||ti == typeid(qdInterfaceBackground)
		||ti == typeid(qdInterfaceSlider)
		||ti == typeid(qdInterfaceSave))
	{
		qdInterfaceScreen* const pscr = static_cast<qdInterfaceScreen*>(pobj->owner());
		return uniq_name(strName, pscr->element_list());
	}
	else if (ti == typeid(qdInterfaceElementState))
	{
		const qdInterfaceButton* const pbtn = 
			static_cast<qdInterfaceButton*>(pobj->owner());
		int const cnt = pbtn->num_states();
		for(int i = 0; i < cnt; ++i)
		{
			const qdInterfaceElementState* const pstate = pbtn->get_state(i);
			if (pobj != static_cast<const qdInterfaceObjectBase*>(pstate))
			{
				LPCTSTR name = pstate->name();
				if (name&&!_tcsicmp(strName, name))
					return false;
			}
		}
		return true;
	}

	return false;
}

/*!
 	Тест прозиводится функцией bool uniq_name(LPCTSTR strText, const _Cont& _c).
	\param strName	- имя для теста
	\param ptrObj	- объект, которому это имя предназначается
	\return \a true - если имя уникальное
*/
bool SceneTreeView::CorrespondingTestName(LPCTSTR strName, qdNamedObject* ptrObj)
{
	bool res = false;
	ASSERT(!IsBadReadPtr(ptrObj, sizeof(qdNamedObject)));

	
	if (IsGameObject(ptrObj)&&IsGlobalObj(ptrObj)) 
		res = uniq_name(strName, m_ptrGameDisp->global_object_list());
	else
	{
		qdNamedObject *owner = ptrObj->owner();
		switch(ptrObj->named_object_type())
		{

		case QD_NAMED_OBJECT_STATIC_OBJ:
		case QD_NAMED_OBJECT_ANIMATED_OBJ:
		case QD_NAMED_OBJECT_MOVING_OBJ:
			res = uniq_name(strName, 
				reinterpret_cast<qdGameScene*>(owner)->object_list());
			break;
		case QD_NAMED_OBJECT_SCENE:
			res = uniq_name(strName, 
				reinterpret_cast<qdGameDispatcher*>(owner)->scene_list());
			break;
		case QD_NAMED_OBJECT_ANIMATION:
			res = uniq_name(strName, 
				reinterpret_cast<qdGameDispatcherBase*>(owner)
													->animation_list());
			break;
		case QD_NAMED_OBJECT_OBJ_STATE:
			res = uniq_name(strName,
				reinterpret_cast<qdGameObjectAnimated*>(owner)->state_vector());
			break;
		case QD_NAMED_OBJECT_TRIGGER_CHAIN:
			res = uniq_name(strName,
				reinterpret_cast<qdGameDispatcher*>(owner)->trigger_chain_list());
			break;
		case QD_NAMED_OBJECT_VIDEO:
			res = uniq_name(strName,
				reinterpret_cast<qdGameDispatcher*>(owner)->video_list());
			break;
		case QD_NAMED_OBJECT_GRID_ZONE:
			res = uniq_name(strName,
				reinterpret_cast<qdGameScene*>(owner)->grid_zone_list());
			break;
		case QD_NAMED_OBJECT_MINIGAME:
			res = uniq_name(strName,
				reinterpret_cast<qdGameDispatcher*>(owner)->minigame_list());
			break;
		case QD_NAMED_OBJECT_INVENTORY:
			res = uniq_name(strName,
				reinterpret_cast<qdGameDispatcher*>(owner)->inventory_list());
			break;
		case QD_NAMED_OBJECT_GAME_END:
			res = uniq_name(strName, 
				reinterpret_cast<qdGameDispatcher*>(owner)->game_end_list());
			break;
		case QD_NAMED_OBJECT_COUNTER:
			res = uniq_name(strName, 
				reinterpret_cast<qdGameDispatcher*>(owner)->counter_list());
			break;
		};
	}
	return res;
}

void SceneTreeView::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pDispInfo = (TV_DISPINFO*)pNMHDR;
	if(!pDispInfo->item.pszText||
		!*pDispInfo->item.pszText)
	{
		*pResult = 0;
		return;
	}
	CWaitCursor __wait__;
	thread_suspender suspend(this);

	DWORD_PTR dwData = GetTreeCtrl().GetItemData(pDispInfo->item.hItem);
	
	std::pair<qdNamedObjectBase*, bool> r = is_any_interf_obj(dwData);
	LPCTSTR strNewName = pDispInfo->item.pszText;

	if (r.second)
	{
		qdInterfaceObjectBase* const piob = 
			static_cast<qdInterfaceObjectBase*>(r.first);
		if (CorrespondingTestName(strNewName, piob))
		{
			if (RenameObject(piob, strNewName))
			{
				*pResult = TRUE;
				SetProjChange();
			}
		}
	}
	else
	{
		qdNamedObject* po = static_cast<qdNamedObject*>(r.first);
		if(CorrespondingTestName(strNewName, po))
		{
			if (ptree_helpers::is_obj_moving(po)) {
				//убираем и добавляем заново масштаб
				qdGameScene* pscene = static_cast<qdGameScene*>(po->owner());
				pscene->remove_object_scale(po->name());
				pscene->set_object_scale(strNewName,
					static_cast<qdGameObjectMoving*>(po)->scale());
			}

			if (IsGameObject(po)&&IsGlobalObj(po)) 
				RenameCopyOfGlobal(po->name(), strNewName);

			CString oldName(po->name());
			if (RenameObject(po,strNewName))
			{
				switch(po->named_object_type()) {
				case QD_NAMED_OBJECT_OBJ_STATE:
					if (HasGlobalFlag(po)) {
						SyncGlobalStatesCatalog(
							static_cast<qdGameObjectAnimated*>(po->ref_owner()));
					}
					if (po->is_in_triggers()){
						UpdateObjTrgEleTitles(po);
						m_ptrITView->Redraw();
					}
					break;
				case QD_NAMED_OBJECT_GRID_ZONE:
					{
						qdGridZone* zone= static_cast<qdGridZone*>(po);
						qdGridZoneState* state = 
							zone->get_state(qdGridZoneState::ZONE_STATE_ON_NAME);
						if (state->is_in_triggers())
						{
							UpdateObjTrgEleTitles(state);
							m_ptrITView->Redraw();
						}
						state = zone->get_state(qdGridZoneState::ZONE_STATE_OFF_NAME);
						if (state->is_in_triggers())
						{
							UpdateObjTrgEleTitles(state);
							m_ptrITView->Redraw();
						}
						break;
					}
				case QD_NAMED_OBJECT_SCENE:
					UpdateObjTrgEleTitles(po);
					UpdateTrgEleTitle4SceneObjs(static_cast<qdGameScene*>(po));
					m_ptrITView->Redraw();
					break;
				case QD_NAMED_OBJECT_ANIMATED_OBJ:
				case QD_NAMED_OBJECT_MOVING_OBJ:
					UpdateTrgEleTitle4ObjStates(static_cast<qdGameObjectAnimated*>(po));
					m_ptrITView->Redraw();
					break;
				case QD_NAMED_OBJECT_TRIGGER_CHAIN:
					m_ptrITView->Redraw();
					break;
				}
				*pResult = TRUE;
				SetProjChange();
			}
		}
		else
		{
			CString str;
			str.Format(IDS_NAME_REPEATED, strNewName);
			CString cap((LPTSTR)IDS_ERR_MSG_CAPTION);
			MessageBox(str, cap, MB_OK|MB_ICONHAND);
			*pResult = 0;
		}
	}
}

void SceneTreeView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTreeCtrl &tree = GetTreeCtrl();
	CPoint p;
	GetCursorPos(&p);
	tree.ScreenToClient(&p);

	HTREEITEM hItem = tree.GetSelectedItem();

	UINT uiFlags = 0;
	HTREEITEM hHitItem = tree.HitTest(p, &uiFlags);
	if(!(uiFlags&TVHT_ONITEM)||(hHitItem != hItem))
	{
		*pResult = 1;
		return;
	}

	if(!IsContainerObj(tree, hItem)||is_net(tree, hItem))
	{
#ifdef _DEBUG
		if (is_state_node(tree, hItem)) {
			HTREEITEM hObjItem = IsUnderAnimated(hItem, NULL);
			if (hObjItem) {
				qdGameObjectAnimated* pobj = 
					reinterpret_cast<qdGameObjectAnimated*>(tree.GetItemData(hObjItem));
				qdGameObjectState* pstate = 
					reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hItem));
				if (IsGlobalState4Obj(pstate, pobj)) 
				{
					ASSERT(0);
					return;
				}
			}
		}
		else 
#endif // _DEBUG
		if (is_minigame_item(tree, hItem)) {
			qdMiniGame* pmg = reinterpret_cast<qdMiniGame*>(tree.GetItemData(hItem));
			RunMiniGame(pmg);
			return;
		}
		OnProperies();
		*pResult = 1;
		sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	}
}

void SceneTreeView::OnCrtStaticObj() 
{
	CString str;
	str.LoadString(IDS_NEW_STATIC_OBJ_NAME);
	ASSERT(m_hCurrScene);

	qdGameScene* ptrScene = 
	   reinterpret_cast<qdGameScene*>(GetTreeCtrl().GetItemData(m_hCurrScene));

	qdGameObjectStatic* ptrObj = AddStatic(ptrScene, str, true);

	if(ptrObj)
	{
		qdCamera* pcam = ptrScene->get_camera();
		Vect2s scr_cntr;
		scr_cntr.x = pcam->get_scr_center_x();
		scr_cntr.y = pcam->get_scr_center_y();
		float fD = DEF_BACKGROUND_DEPTH;
		
		qdGameObject* farthest = 
			ptree_helpers::GetFarthestGO(ptrScene, 
			ptree_helpers::is_obj_static);
		if (farthest) {
			Vect3f v = pcam->global2camera_coord(farthest->R());
			fD = v.z - 1.f;
		}
		scr_cntr = pcam->scr2rscr(scr_cntr);
		ptrObj->set_pos(pcam->rscr2global(scr_cntr, fD));
		
		ShowProperties(ptrObj, 0);
		if(!ptrObj->get_sprite()->file()
			||!strlen(ptrObj->get_sprite()->file()))
			DeleteObject(ptrObj,false);
	}

	Invalidate();
}

/*!
 	Из ини файла считыает имя игры(обычно qd_game.exe). Получает имя временно файла
	для скрипта, сохраняет в этот файл проект. Формирует командную строку в зависимости
	от режима и запускает процесс с этой командной строкой
	\param strSceneName - сцена, которую надо запустить.
	\param emode		- режим, в котором должна быть запущена игра
*/
void SceneTreeView::RunScene(const CString& strSceneName, ptree_api::eRunMode emode){
	CString strGameExeName;
	strGameExeName.LoadString(IDS_GAME_EXE_FILE_NAME);

	TCHAR buff[MAX_PATH];
	CString strIniFileName = disk_op::GetPathFromFullFileName(disk_op::GetModulePath());
	strIniFileName += _T("QuestEditor.ini");
	GetPrivateProfileString(_T("game_exe"), _T("name"),strGameExeName, 
		buff,MAX_PATH,strIniFileName);
	strGameExeName = buff;	

	CString app_path = disk_op::GetPathFromFullFileName(disk_op::GetModulePath());
	CString strTmpFileName = GetTemporaryFileName();
	if (!m_ptrGameDisp->save_script(strTmpFileName))
	{
		ShowError(m_hWnd, IDS_ERR_SAVE_FAILED);
		return;
	}

	app_path += strGameExeName;

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	CString comand_line = _T("\"");
	comand_line += app_path;
	comand_line += _T("\"");

	comand_line += _T(" \"");
	comand_line += strTmpFileName;
	comand_line += _T("\"");

	if (emode != ptree_api::RM_RUN_SCRIPT)
	{
		comand_line += _T(" -S ");
		comand_line += _T("\"");
		comand_line += strSceneName;
		comand_line += _T("\"");
	}
	if (emode == ptree_api::RM_TRIGGER_TEST) 
	{
		comand_line += _T(" -");
		comand_line += qdTriggerChain::debug_comline();
		comand_line += _T(" ");
		comand_line += _T(" -");
		comand_line += qdTriggerProfiler::activation_comline();
	}

	if(!CreateProcess(app_path,
		const_cast<LPTSTR>((LPCTSTR)(comand_line)), NULL, NULL, FALSE,
		0, NULL, NULL, &si, &pi))
	{
		strGameExeName.LoadString(IDS_CNT_LOAD_GAME_EXE);
		app_path.LoadString(IDS_ERR_MSG_CAPTION);
		MessageBox(strGameExeName, app_path, MB_OK|MB_ICONSTOP);
	}
}

//! Запускает редактор диалогов
void SceneTreeView::RunDialogEditor()
{
	CString strExeName;
	strExeName.LoadString(IDS_DIALOG_EDITOR);

	CString app_path;
	GetModuleFileName(AfxGetApp()->m_hInstance, app_path.GetBufferSetLength(1024), 1024);
	app_path.ReleaseBuffer();

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath( app_path, drive, dir, fname, ext );

	app_path = drive;
	app_path += dir;
	app_path += strExeName;

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	CString comand_line = _T("\"");
	comand_line += app_path;
	comand_line += _T("\"");

	CString fileName = CurDirSafer::GetCurrentDirectory();
	fileName += _T("\\");
	fileName += m_ptrMainFrame->GetCurrScriptFileName();
	PathRenameExtension(fileName.GetBuffer(fileName.GetLength() + 1), _T(".tdb"));
	fileName.ReleaseBuffer();
	
	comand_line += _T(" \"");
	comand_line += fileName;
	comand_line += _T("\"");

	comand_line += _T(" -GM");

	comand_line += _T(" -E\"");
	fileName = CurDirSafer::GetCurrentDirectory();
	fileName += _T("\\Resource\\");
	fileName += m_ptrMainFrame->GetCurrScriptFileName();
	PathRenameExtension(fileName.GetBuffer(fileName.GetLength() + 2), _T(".btdb"));
	fileName.ReleaseBuffer();
	comand_line += fileName;
	comand_line += _T("\"");

	if(!CreateProcess(app_path,
		const_cast<LPTSTR>((LPCTSTR)(comand_line)), NULL, NULL, FALSE,
		0, NULL, NULL, &si, &pi))
	{
		strExeName.LoadString(IDS_CNT_LOAD_DIALOG_EDITOR_EXE);
		app_path.LoadString(IDS_ERR_MSG_CAPTION);
		MessageBox(strExeName, app_path, MB_OK|MB_ICONSTOP);
	}
}

/*!
	Запускает внешнее приложение для проигрывания.
	\param pmg - Миниигра, которую будем запускать.
*/
void SceneTreeView::RunMiniGame(qdMiniGame* pmg){
	CString strGameExeName;
	strGameExeName.LoadString(IDS_MINI_GAME_EXE_FILE_NAME);

	CString app_path;
	GetModuleFileName(AfxGetApp()->m_hInstance, app_path.GetBufferSetLength(1024), 1024);
	app_path.ReleaseBuffer();

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath( app_path, drive, dir, fname, ext );

	app_path = drive;
	app_path += dir;

	app_path += strGameExeName;

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	CString comand_line = _T("\"");
	comand_line += app_path;
	comand_line += _T("\"");

	comand_line += _T(" \"");
	comand_line += pmg->dll_name();
	comand_line += _T("\"");
//	comand_line += _T(" \"");
//	if (*pmg->config_file() == _T('\0')) 
//		comand_line += pmg->name();
//	else
//		comand_line += pmg->config_file();
//	comand_line += _T("\"");

	if(!CreateProcess(app_path,
		const_cast<LPTSTR>((LPCTSTR)(comand_line)), NULL, NULL, FALSE,
		0, NULL, NULL, &si, &pi))
	{
		strGameExeName.LoadString(IDS_CNT_LOAD_MINI_GAME_EXE);
		app_path.LoadString(IDS_ERR_MSG_CAPTION);
		MessageBox(strGameExeName, app_path, MB_OK|MB_ICONSTOP);
	}
}

void SceneTreeView::OnButtonRunScript()
{
	RunScene(CString(), ptree_api::RM_RUN_SCRIPT);
}

void SceneTreeView::OnButtonRunScene() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelect = tree.GetSelectedItem();
	if (!hSelect) 
		return;

	HTREEITEM hScene = IsObjUnderScene(tree, hSelect);
	ASSERT(hScene);
	if(!hScene)
		return;
	CString strSceneName = tree.GetItemText(hScene);
	RunScene(strSceneName, ptree_api::RM_NORMAL);
}

void SceneTreeView::OnUpdateButtonRunScene(CCmdUI* pCmdUI) 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelect = tree.GetSelectedItem();
	if (!hSelect) 
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	HTREEITEM hScene = IsObjUnderScene(GetTreeCtrl(), hSelect);
	pCmdUI->Enable(hScene != NULL);
}

void SceneTreeView::OnUpdateButtonRunScript(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp != NULL);
}

void SceneTreeView::OnEditDialogs()
{
	RunDialogEditor();
}

void SceneTreeView::OnUpdateEditDialogs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_ptrGameDisp != NULL);
}

/*!
 	Достаёт из узла диспетчер(CTreeCtrl::GetItemData()). Его анимация будет загружена.
	Если bUniqueList установлен в true, то список будет дополнен содержимым глобального
	каталога анимации. Анимации из глобального каталога с именами, которые встречаются 
	в каталоге сцены, отображаться не будут.
	Настравивает объект класс каталога анимации(AnimCatalogDlg).
	Отображает каталог (AnimCatalogDlg::DoModal()).
	Если изменения были сделаны, то устанавливается флаг, сигнализирующий, что
	проект был изменен(SetProjChange())
	\param hItem		- узел сцены или глобального диспетчера
	\param eAction		- какое действие выполнить после появления диалога с 
							содержимым каталога. См.: AnimCatalogDlgAction
	\param bUniqueList	- будет список уникальным содержимым каталога сцены и глобального
							каталога. Или же будут отображено только содержимое катлога
							сцены
*/
void SceneTreeView::ShowAnimationCatalogDlg(HTREEITEM hItem, int eAction, 
															bool bUniqueList)
{
	CTreeCtrl& tree = GetTreeCtrl();
	qdGameDispatcherBase* ptrGB = (qdGameDispatcherBase*)tree.GetItemData(hItem);

	AnimCatalogDlg pg;
	pg.set_data(m_ptrGameDisp, ptrGB);
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
		SetProjChange();
}


void SceneTreeView::OnRemove() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	ASSERT(hSelected);
	m_ptrSceneView->SelectObj(NULL);
	RemoveItem(hSelected, true);
}

/*!
 	Вызывается перед загрузкой проекта
	\return Всегда возвращает true
*/
bool SceneTreeView::reset()
{
	if (m_ptrPlayedObject) {
		SuspendThread();
		m_ptrPlayedObject.set(NULL);
		m_ptrPlayedState = NULL;
	}

	CTreeCtrl& tree = GetTreeCtrl();
	tree.SelectItem(NULL);
	tree.DeleteAllItems();

	m_mapSceneIndex.clear();
	m_menu.DestroyMenu();
	m_hCurrScene = NULL;
	m_ptrGameDisp = NULL;
	RemoveTmps();
	m_IsSceneViewActive = true;
	
	return true;
}

/*!
 	Проверяет кому принадлежит принадлежит состояние. Если мыши, то запускаем
	диалог свойств для курсора(CursorPropDlg::DoModal). Если состояние объекта, то 
	показывает диалог свойств состояния(EditStateDlg::DoModal()). В зависимости
	от типа состояния диалогу делаются необходимые настройки.
	\param pstate - состояние, с которым работаем
	\return true - если в диалоге была нажата кнопка "ОК"
*/
bool SceneTreeView::EditState(qdGameObjectState* pstate)
{
	if (ptree_helpers::is_obj_mouse(pstate->owner()))
	{
		CursorPropDlg dlg(m_hWnd);
		dlg.set_data(m_ptrGameDisp, static_cast<qdGameObjectStateStatic*>(pstate));
		return (dlg.DoModal() == IDOK);
	}
	else{
		EditStateDlg dlg(m_hWnd);
		dlg.set_data(m_ptrGameDisp, 
			static_cast<qdGameObjectAnimated*>(pstate->owner()));
		if (IsGlobalObj(pstate->ref_owner())) {
			dlg.suppress_coord_anim();
			if (!m_ptrGameDisp->get_active_scene()){
				dlg.suppress_bound();
				dlg.suppress_stacking();
			}
		}
		else if (static_cast<qdGameScene*>(pstate->owner()->owner()) 
			!= m_ptrGameDisp->get_active_scene()) {
			dlg.suppress_bound();
			dlg.suppress_edit_coord_anim();
			dlg.suppress_stacking();
		}
		else if (pstate->state_type()==qdGameObjectState::STATE_MASK)
		{
			dlg.suppress_coord_anim();
			dlg.suppress_stacking();
		}
		dlg.set_state(pstate);
		
		return (dlg.DoModal() == IDOK);
	}
}

/*!
 	Достает из узла состояние(CTreeCtrl::GetItemData()).Передает управление
	SceneTreeView::EditState(qdGameObjectState* pstate).
	\param	hItem - узел состояния
	\return	Возвращает результат работы SceneTreeView::EditState(qdGameObjectState* 
	pstate).
*/
bool SceneTreeView::EditState(HTREEITEM hItem)

{
	qdGameObjectState* ptrObj = 
		reinterpret_cast<qdGameObjectState*>(GetTreeCtrl().GetItemData(hItem));
	return EditState(ptrObj);
}

void SceneTreeView::OnCrtAnimation() 
{
	//первым делом находим диспетчер, в который и засунем новую анимацию
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hPar = tree.GetSelectedItem();
	ASSERT(hPar);
	ShowAnimationCatalogDlg(hPar, GOPA_CREATE, false);
}

/*!
	Для каждой строки, хранящейся в \a m_TmpFiles вызываем DeleteFile
*/
void SceneTreeView::RemoveTmps()
{
	std::for_each(m_TmpFiles.begin(), m_TmpFiles.end(), DeleteFile);
}

/*!
 	Создает временный файл для скрипта в каталоге проекта. Кладет полученное имя
	в m_TmpFiles.
	\return Возвращает полное имя временного файла
*/
CString SceneTreeView::GetTemporaryFileName()
{
	if (!m_TmpFiles.empty()) 
	{
		if (DeleteFile(m_TmpFiles.back()))
			m_TmpFiles.pop_back();
	}
	CString res;
	do{
		res = m_ptrMainFrame->GetCurrProjDir();
		res += _T("\\");
		ASSERT(res == (CurDirSafer::GetCurrentDirectory()+_T("\\")));
		CString file_name;
		file_name.Format(_T("script_tmp%d%s"),
			GetTickCount(), m_ptrMainFrame->GetCurrScriptFileName());
		res += file_name;
	}
	while (PathFileExists(res));
	m_TmpFiles.push_back(res);
	return res;
}

/*!
 	Функция потока прокручивающего анимацию
*/
DWORD WINAPI ShowAnimationThread(LPVOID pData)
{
	SceneTreeView* ptrTree = static_cast<SceneTreeView*>(pData);
	return ptrTree->ShowAnimation();
}

/*!
	Работает в другом потоке.Поток создаётся и живет до конца программы,
	если нечего прокручивать он ставиться на паузу
*/
int SceneTreeView::ShowAnimation()
{
	MSG msg;
	PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE);

	CSingleLock __sl(&m_csAnimationGurad, FALSE);
	CSingleLock __dg_lock(m_ptrSceneView->GetDrawGuard(), FALSE);
	while (true) {
		__sl.Lock();

		qdGameScene* ptrScene = m_ptrGameDisp->get_active_scene();
		ASSERT(!IsBadReadPtr(ptrScene, sizeof(qdGameScene)));

		ASSERT(!IsBadReadPtr(m_ptrGameDisp, sizeof(qdGameDispatcher)));

		ASSERT(!IsBadReadPtr(m_ptrSceneView, sizeof(CChildView)));
		ASSERT(static_cast<bool>(m_ptrPlayedObject));
		ASSERT(m_ptrPlayedState);

		//активный объект используется при прорисовке
		//поэтому его переназначение должно принадлежать одному потоку
		__dg_lock.Lock();
		ptrScene->activate_object(m_ptrPlayedObject);
		m_ptrGameDisp->quant();

		sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);

		ptrScene->activate_object(NULL);

		if(m_ptrPlayedObject->state_status(m_ptrPlayedState)
			== qdGameObjectAnimated::STATE_DONE)
			m_ptrPlayedObject->set_state(m_ptrPlayedState);

		__dg_lock.Unlock();
		__sl.Unlock();
		
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				return 0;
		}
	}
	return 0;
}

/*!
 	Останавливает после того как удалось захватить SceneTreeView::m_csAnimationGurad
*/
bool SceneTreeView::SuspendThread()
{
	CSingleLock __sl(&m_csAnimationGurad, TRUE);
	DWORD res = ::SuspendThread(m_hAnimThread.get());
	return (res != static_cast<DWORD>(-1));
}

bool SceneTreeView::ResumeThread()
{
	DWORD res = ::ResumeThread(m_hAnimThread.get());
	return (res != static_cast<DWORD>(-1));
}

/*!
	\retval true - всегда
*/
bool SceneTreeView::SuspendAndClear(){
	VERIFY(SuspendThread());
	if(sndDispatcher* p = sndDispatcher::get_dispatcher())
		p -> disable();
	if (m_ptrPlayedState) {
		m_ptrPlayedObject = NULL;
		m_ptrPlayedState = NULL;

		m_ptrGameDisp->update_time();
		m_ptrGameDisp->quant();
	}
	return true;
}

/*!
 	Устанавливает m_ptrPlayedObject и m_ptrPlayedState. И запускает поток
*/
bool SceneTreeView::SetAndResume(qdGameObjectState* ptrState)
{
	if(sndDispatcher* p = sndDispatcher::get_dispatcher())
		p -> enable();

	m_ptrPlayedState = ptrState;
	m_ptrPlayedObject = static_cast<qdGameObjectAnimated*>(m_ptrPlayedState->owner());
	m_ptrPlayedObject->set_state(ptrState);
	//! чтобы анимация не делала скачка
	m_ptrGameDisp->update_time();

	return ResumeThread();
}

/*!
 	Передает управление SceneTreeView::SetAndResume(qdGameObjectState* ptrState)
*/
void SceneTreeView::ShowStateAnimation(qdGameObjectState* pno)
{
	SetAndResume(pno);
}

/*!
 	Передает управление SceneTreeView::SuspendAndClear()
*/
void SceneTreeView::StopAnimation(qdGameObjectState* ptrState)
{
	SuspendAndClear();
}

void SceneTreeView::OnMenuMovingsStops() 
{
	MoveStopDlg dlg;
	
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	qdGameDispatcherBase * ptr = 
		reinterpret_cast<qdGameDispatcherBase*>(tree.GetItemData(hItem));
	ASSERT(ptr->named_object_type() == QD_NAMED_OBJECT_SCENE
		||ptr->named_object_type() == QD_NAMED_OBJECT_DISPATCHER);
	
	dlg.set_data(m_ptrGameDisp, ptr);
	dlg.DoModal();
	if(dlg.has_change())
		SetProjChange();
}

void SceneTreeView::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == TIMER_EVENT_PLAY_VIDEO) {
		if(m_ptrGameDisp->is_video_playing())
			if(m_ptrGameDisp->is_video_finished())
			{
				m_ptrGameDisp->close_video();
				KillTimer(m_uiShowVideoTimer);
				m_uiShowVideoTimer = (UINT)-1;
				sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
			}
	}
	CTreeView::OnTimer(nIDEvent);
}

/*!
 	Вызывает соответствующую типу объекта функцию переименования.
	\param piob - Объект для переименования
	\param lpszNewName - новое имя
	\return \a true - если перименование удалось
*/
bool SceneTreeView::RenameObject(qdInterfaceObjectBase* piob, LPCTSTR lpszNewName)
{
	const std::type_info& ti = typeid(*piob);
	if (ti == typeid(qdInterfaceScreen))
	{
		qdInterfaceDispatcher *const pd = qdInterfaceDispatcher::get_dispatcher();
		return pd->rename_screen(static_cast<qdInterfaceScreen*>(piob), lpszNewName);
	}
	else if (ti == typeid(qdInterfaceButton)
		||ti == typeid(qdInterfaceBackground)
		||ti == typeid(qdInterfaceSlider)
		||ti == typeid(qdInterfaceSave))
	{
		qdInterfaceScreen* const pscr = 
			static_cast<qdInterfaceScreen*>(piob->owner());
		return pscr->rename_element(static_cast<qdInterfaceElement*>(piob), lpszNewName);
	}
	else if (ti == typeid(qdInterfaceElementState))
	{
		piob->set_name(lpszNewName);
		return true;
	}
	ASSERT(0);
	return false;
}

/*!
 	Переименовывает объекты унаследованные от qdNamedObject. Вызывает соответствующую
	типу объекта функцию переименования
	\param pno			- Объект для переименования
	\param lpszNewName	- Новое имя объекта
	\return \a true	- есил переименовать удалось
*/
bool SceneTreeView::RenameObject(qdNamedObject* pno, LPCTSTR lpszNewName)
{
	if (IsGameObject(pno)&&IsGlobalObj(pno)) {
		m_ptrGameDisp->rename_global_object(
			static_cast<qdGameObject*>(pno), lpszNewName);
		return true;
	}

	qdNamedObject* ptrOwner = pno->ref_owner();
	switch(pno->named_object_type()) {
	case QD_NAMED_OBJECT_SCENE:
		static_cast<qdGameDispatcher*>(ptrOwner)->
			rename_scene(static_cast<qdGameScene*>(pno), lpszNewName);
		break;
	case QD_NAMED_OBJECT_STATIC_OBJ:
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
	case QD_NAMED_OBJECT_MOVING_OBJ:
			static_cast<qdGameScene*>(ptrOwner)->
				rename_object(static_cast<qdGameObject*>(pno), lpszNewName);
		break;
	case QD_NAMED_OBJECT_OBJ_STATE:
		pno->set_name(lpszNewName);
		break;
	case QD_NAMED_OBJECT_GRID_ZONE:
		if (static_cast<qdGameScene*>(ptrOwner)->
				rename_grid_zone(static_cast<qdGridZone*>(pno), lpszNewName)
			&&m_ptrEditedGZ.get())
				m_ptrEditedGZ->set_name(lpszNewName);
		break;
	case QD_NAMED_OBJECT_MINIGAME:
		static_cast<qdGameDispatcher*>(ptrOwner)->
				rename_minigame(static_cast<qdMiniGame*>(pno), lpszNewName);
		break;
	case QD_NAMED_OBJECT_TRIGGER_CHAIN:
		static_cast<qdGameDispatcher*>(ptrOwner)->
			rename_trigger_chain(static_cast<qdTriggerChain*>(pno), lpszNewName);
		break;
	case QD_NAMED_OBJECT_VIDEO:
		static_cast<qdGameDispatcher*>(ptrOwner)->
			rename_video(static_cast<qdVideo*>(pno), lpszNewName);
		break;
	case QD_NAMED_OBJECT_INVENTORY:
		{
			return RenameInventory(static_cast<qdGameDispatcher*>(ptrOwner),
				static_cast<qdInventory*>(pno), lpszNewName);
//			static_cast<qdGameDispatcher*>(ptrOwner)->
//				rename_inventory(static_cast<qdInventory*>(pno), lpszNewName);
		}
		break;
	case QD_NAMED_OBJECT_GAME_END:
		static_cast<qdGameDispatcher*>(ptrOwner)->
			rename_game_end(static_cast<qdGameEnd*>(pno), lpszNewName);
		break;
	case QD_NAMED_OBJECT_COUNTER:
		static_cast<qdGameDispatcher*>(ptrOwner)->
			rename_counter(static_cast<qdCounter*>(pno), lpszNewName);
		break;
	default:
		return false;
	}
	return true;
}

void SceneTreeView::OnCreateStatics() 
{
	AddStatics(m_hCurrScene);
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void SceneTreeView::OnUpdateCreateStatics(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_hCurrScene != NULL&&m_IsSceneViewActive);
}

void SceneTreeView::OnCreateBack() 
{
	ASSERT(m_hCurrScene);
	qdGameScene* ptrScene = 
		(qdGameScene*)GetTreeCtrl().GetItemData(m_hCurrScene);
    CreateBackground(m_hCurrScene, ptrScene);
}

void SceneTreeView::OnUpdateCreateBack(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_hCurrScene != NULL&&m_IsSceneViewActive);
}

void SceneTreeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_DELETE) {
		HTREEITEM hSelected = GetTreeCtrl().GetSelectedItem();
		if(CanRemoveItem(hSelected))
		{
			RemoveItem(hSelected, true);
		}
		else 
			MessageBeep(MB_ICONASTERISK);
	}
	CTreeView::OnKeyDown(nChar, nRepCnt, nFlags);
}

//получает первую попавшуюся сцену
HTREEITEM GetFirstScene(CTreeCtrl& tree)
{
	//получаем глобальный узел
	HTREEITEM hRes = tree.GetChildItem(TVI_ROOT);
	if(!hRes)
		return NULL;
	//получаем узел локации
	//ничего не делаем, потому как локации ушли(может быть временно)
	//получаем узел сцены.
	return tree.GetChildItem(hRes);
}
void SceneTreeView::OnNetProperties() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	if(!m_hCurrScene)
	{
		//получаем первую попавшуюся сцену
		m_hCurrScene = GetFirstScene(tree);
	}
	HTREEITEM hItem = FindTreeItemByText(GetTreeCtrl(), m_hCurrScene, 
							CString((LPTSTR)IDS_NET_NODE_NAME));
	if (!hItem) {
		ASSERT(0);
		return;

	}

	ShowItem(hItem);

	qdNamedObject* pno = (qdNamedObject*)tree.GetItemData(hItem);
	ShowCorrespondingPropDlg(pno, 0, false);
}

void SceneTreeView::OnEditAnimation() 
{
	HTREEITEM hItem = m_hCurrScene;
	ASSERT(hItem);
	if(hItem)
		ShowAnimationCatalogDlg(hItem, GOPA_SHOW, false);
}

void SceneTreeView::OnEditGaits() 
{

	MoveStopDlg dlg;
	dlg.set_data(m_ptrGameDisp, m_ptrGameDisp);
	dlg.DoModal();
	if(dlg.has_change())
	{
		SetProjChange();
		UpdateCatalogsInScenes(QD_NAMED_OBJECT_MOVING_OBJ);
	}
}

/*
 *	вызов этой функции происходит не только из меню, поэтому 
 *	нельзя рассчитывать что выбранный в данный момент элемент
 *  подходит нам на все сто
 */

void SceneTreeView::OnCrtAnimObjs() 
{
	ASSERT(m_hCurrScene);
	qdGameScene * ptrScene = 
		reinterpret_cast<qdGameScene*>(GetTreeCtrl().GetItemData(m_hCurrScene));
	ASSERT(ptrScene);
	
	HTREEITEM hCatalog = FindTreeItemByText(GetTreeCtrl(), m_hCurrScene, 
					CString((LPTSTR)IDS_ANIMATED_OBJS_NODE));
	
	ASSERT(hCatalog);
	
	AddAnimStateWPg pg;
	pg.set_data(m_ptrGameDisp, ptrScene);
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
		DeleteChildern(hCatalog);
		
		UpdateSceneObjsCatalog(QD_NAMED_OBJECT_ANIMATED_OBJ, 
														hCatalog, ptrScene);
		Invalidate();
		SetProjChange();
		sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	}
}

/*
 *	вызов этой функции происходит не только из меню, поэтому 
 *	нельзя рассчитывать что выбранный в данный момент элемент
 *  подходит нам на все сто
 */

/*
 *	если выбрана сцена, то добавляем в сцену. иначе добавляем
 *  в глобальный каталог
 */
void SceneTreeView::OnCrtMovingObj() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	ASSERT(hSelected);

	HTREEITEM hScene = IsObjUnderScene(tree, hSelected);

	qdGameDispatcherBase* ptrObjDisp = NULL;
	if (hScene) {
		ptrObjDisp = reinterpret_cast<qdGameScene*>(tree.GetItemData(hScene));
	}
	else
	{
		hScene = tree.GetRootItem();
		ptrObjDisp = m_ptrGameDisp;
	}

	HTREEITEM hCatalog = FindTreeItemByText(GetTreeCtrl(), hScene, 
										CString((LPTSTR)IDS_MOVING_OBJS_NODE));
	
	AddAnimStateWPg pg;
	pg.set_data(m_ptrGameDisp, ptrObjDisp);
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
		
		DeleteChildern(hCatalog);
		if (ptrObjDisp->named_object_type() == QD_NAMED_OBJECT_SCENE) {
			UpdateObjsCatalog(QD_NAMED_OBJECT_MOVING_OBJ, hCatalog, 
				static_cast<qdGameScene*>(ptrObjDisp)->object_list());
		}
		else if (ptrObjDisp->named_object_type() == QD_NAMED_OBJECT_DISPATCHER) 
		{
			UpdateObjsCatalog(QD_NAMED_OBJECT_MOVING_OBJ, hCatalog, 
				m_ptrGameDisp->global_object_list());
			
			//пользуемся тем, что добавление происходит в конец
			qdGameObject* padded = m_ptrGameDisp->global_object_list().back();
			SyncGlobalStatesCatalog(static_cast<qdGameObjectAnimated*>(padded));
		}
		sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	}
	Invalidate();
}

/*!
 	Проверяет есть ли у состояния флаг глобального состояния. Если есть, то проверяем
	находится ли узел состояния под сценой. Если да, то это глобальное состояние
	\param pst		- состояние
	\param hItem	- узел состояния
	\return \a true - если состояние глобальное.
	\remarks Глобальными состояниями являются состояния, унаследованные объектами-копиями
			от глобального объекта
*/
bool SceneTreeView::IsGlobalState(const qdNamedObject* pst, HTREEITEM hItem)
{
	if (HasGlobalFlag(pst)) {
		return ptree_helpers::IsObjUnderScene(GetTreeCtrl(), hItem);
	}
	return false;
}

/*!
 	Пробегает по списку сцен и проверяет: есть ли объект с таким именем в сцене.
	Если нет, то создает объект-копию(SceneTreeView::AddGlobalObj2Scene(
		qdGameScene* pscene, const qdGameObject* pobj)).
	\param pobj - глобальный объект, копии которого будем создавать
*/
void SceneTreeView::GlobalObjToScenes(qdGameObject const* pobj)
{
	const qdGameSceneList& gsl = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator _itr = gsl.begin(), _end = gsl.end();
	for(; _itr != _end; ++_itr)
	{
		qdGameScene* pscene = *_itr;
		if (!pscene->get_object(pobj->name())) {
			AddGlobalObj2Scene(pscene, pobj);
		}
	}
}

/*!
 	Для каждого глобального объекта вызывает SceneTreeViewGlobalObjToScenes(
		qdGameObject const* pobj)
*/
void SceneTreeView::GlobalObjsToScenes()
{
	const qdGameObjectList& gl = m_ptrGameDisp->global_object_list();
	qdGameObjectList::const_iterator _i = gl.begin(), _E = gl.end();
	for(; _i != _E; ++_i)
	{
		qdGameObject *pgo = *_i;
		GlobalObjToScenes(pgo);
	}
}

/*!
 	Пробегает по всем сценам. Ищет копию глобального объекта, указанного именем.
	Если для найденной копии функция-условие возвращает true, то она(копия) добавляется
	в список
	\param name	- имя глобального объекта
	\param f	- указатель на функцию-фильтр для объектов-копий
	\param pres	- указатель на список, куда будут сложены все копии прошедшие фильтрацию
*/
void SceneTreeView::GetCopyOfGlobalList(LPCTSTR name, CHECK_FUN f, 
										qdGameObjectList* pres)
{
	const qdGameSceneList& gsl = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator _itr = gsl.begin(), _end = gsl.end();
	for(; _itr != _end; ++_itr)
	{
		qdGameScene* pscene = *_itr;
		if (qdGameObject* p = pscene->get_object(name)) {
			if (f(p)) {
				pres->push_back(p);
			}
		}
	}
}

/*!
 	Ищет объекты-копии во всех сценах(SceneTreeView::GetCopyOfGlobalList(LPCTSTR name, 
	CHECK_FUN f, qdGameObjectList* pres)). Для каждого найденного объекта-копии:
		находит узел в дереве и меняет его текст; если объект персонаж, то удаляет
		масштаб со старым именем(qdGameDispatcherBase::remove_object_scale())
		и добавляет тот же с новым(qdGameDispatcherBase::set_object_scale());
		переименовывает объект(SceneTreeView::RenameObject(qdInterfaceObjectBase* piob, 
		LPCTSTR lpszNewName));обновляет заголовки элементов триггеров
		(SceneTreeView::UpdateTrgEleTitle4ObjStates(qdGameObjectAnimated* pobj))
	\param lpszOld - старое имя
	\param lpszNew - новое имя
*/
void SceneTreeView::RenameCopyOfGlobal(LPCTSTR lpszOld, LPCTSTR lpszNew)
{
	qdGameObjectList list;
	GetCopyOfGlobalList(lpszOld, ptree_helpers::cfALL, &list);
	if (!list.empty()) {
		qdGameObjectList::iterator i = list.begin(), e = list.end();
		for(; i != e; ++i)
		{
			qdGameObject* pobj = *i;

			HTREEITEM hItem = FindObjectItem(pobj);
			ASSERT(hItem);
			if(hItem)
				GetTreeCtrl().SetItemText(hItem, lpszNew);
			if (ptree_helpers::is_obj_moving(pobj)) {
				qdGameScene* pscene = 
					static_cast<qdGameScene*>(pobj->owner());

				pscene->remove_object_scale(pobj->name());
				pscene->set_object_scale(lpszNew,
					static_cast<qdGameObjectMoving*>(pobj)->scale());
			}
			RenameObject(pobj, lpszNew);

			ASSERT(pobj->named_object_type() != QD_NAMED_OBJECT_STATIC_OBJ);

			UpdateTrgEleTitle4ObjStates(
				static_cast<qdGameObjectAnimated*>(pobj));
		}
	}
}

/*!
 	Находит узел сцены, выбирает его и разворачивает
	\param ptrScene - сцена
	\return \a TRUE - если узел найден
*/
BOOL SceneTreeView::ActivateScene(qdGameScene const* ptrScene)
{
	HTREEITEM hParent = FindObjectItem(ptrScene);
	if(hParent)
	{
		if(m_hCurrScene)
			GetTreeCtrl().Expand(m_hCurrScene, TVE_COLLAPSE);
		
		ShowItem(hParent);
		GetTreeCtrl().Expand(hParent, TVE_EXPAND);
		return TRUE;
	}
	return FALSE;
}

void SceneTreeView::OnUpdateCrtMovingObj(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_IsSceneViewActive&&m_ptrGameDisp);	
}

void SceneTreeView::OnUpdateNetProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_hCurrScene != NULL&&m_IsSceneViewActive);	
}

void SceneTreeView::OnUpdateCrtAnimObjs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_hCurrScene != NULL&&m_IsSceneViewActive);
}

void SceneTreeView::OnUpdateEditAnimation(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_hCurrScene != NULL&&m_IsSceneViewActive);
}


void SceneTreeView::OnGameSettings() 
{
	GameSettingsPage pg;
	CPropertyPage* pp[] = {&pg, 0};
	pg.set_data(m_ptrGameDisp);
	
	CMyPropertySheet propSheet(pp, 0, IDS_CAP_GAME_SETTINGS);
	int res = propSheet.DoModal();
}


void SceneTreeView::OnCreateState() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	if(!hSelected)
		return;

	bool bUseMovement = false;

	HTREEITEM hObjItem = hSelected;

	if(IsStateCatalog(tree, hSelected))
	{	//т.е. ткнули на состояния для персонажа
		bUseMovement = IsWalkStateCatalog(tree, hSelected);
		hObjItem = tree.GetParentItem(hSelected);
		ASSERT(hObjItem);
	}

	qdGameObjectAnimated* ptrObj = 
		reinterpret_cast<qdGameObjectAnimated*>(tree.GetItemData(hObjItem));

	AddAnimStateWPg aas;
	aas.set_data(m_ptrGameDisp, static_cast<qdGameDispatcherBase*>(ptrObj->owner()));

	aas.set_obj(ptrObj);
	aas.set_place(PAGE_PLACE_FIRST_LAST);
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
		ReloadStates(tree, hObjItem, ptrObj);

		if (IsGlobalObj(ptrObj))
			SyncGlobalStatesCatalog(ptrObj);

		SetProjChange();
		Invalidate();
	}
}

/*!
 	Для верности отрезает, а потом объединяет, глобальный объект с его копиями в сценах.
	Затем бежит по всем сценам. Если в сцене есть копия, то прегружает список состояний
	копии в дереве(ptree_api::ReloadStates()).
	\param ptrObj - Глобальный объект, состояния которого синхронизируются.
*/
void SceneTreeView::SyncGlobalStatesCatalog(qdGameObjectAnimated* ptrObj)
{
	CWaitCursor __wait__;

	m_ptrGameDisp->split_global_objects(ptrObj);
	m_ptrGameDisp->merge_global_objects(ptrObj);

	const qdGameSceneList& gsl = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator _i = gsl.begin(), _E = gsl.end();
	for(; _i != _E; ++_i)
	{
		qdGameScene* ptrScene = *_i;
		//ищем, есть ли в сцене объект с таким же именем как у глобального.
		qdGameObject* ptrDouble = ptrScene->get_object(ptrObj->name());
		if (ptrDouble) {
			HTREEITEM hObjItem = FindObjectItem(ptrDouble);
			ReloadStates(GetTreeCtrl(), hObjItem,
				static_cast<qdGameObjectAnimated*>(ptrDouble));
		}
	}
}

/*!
 	Предполагает, что все подузлы каталога уже удалены.
	Выбирает из сцены объекты нужного типа и вставляет их в дерево(
	ptree_loading::InsertNamedObject()). Если надо грузит
	состояния(ptree_loading::LoadStates())
	\param type		- тип объектов
	\param hCatalog - Узел каталога объектов, соответствующего типу 
	\param gol		- Список объектов сцены
*/
void SceneTreeView::UpdateObjsCatalog(qdNamedObjectType type, 
											HTREEITEM hCatalog, 
											const qdGameObjectList& gol)
{
	UINT uiImage = -1;
	switch(type) {
	case QD_NAMED_OBJECT_STATIC_OBJ:
		uiImage = II_STATIC_OBJ_NODE;
		break;
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
		uiImage = II_ANIMATED_OBJ_NODE;
		break;
	case QD_NAMED_OBJECT_MOVING_OBJ:
		uiImage = II_MOVING_OBJ_NODE;
		break;
	}
	
	CTreeCtrl &wndTree = GetTreeCtrl();

	qdGameObjectList::const_iterator _itr = gol.begin(), _end = gol.end();
	for(; _itr != _end; ++_itr)
	{
		qdGameObject* pgo = *_itr;
		if(pgo->named_object_type() == type)
		{
			HTREEITEM hObj = 
				InsertNamedObject(wndTree, hCatalog, pgo, uiImage);
			if(!hObj)
				return ;
			
			LoadStates(wndTree, hObj, pgo);
			
			wndTree.Expand(hObj, TVE_EXPAND);
		}
	}
}

/*!
 	Передает управление(SceneTreeView::UpdateObjsCatalog(qdNamedObjectType type, 
	HTREEITEM hCatalog, const qdGameObjectList& gol))
	\param type		- Тип объектов, каталог которых будет обновлен
	\param hCatalog - Описатель узла каталога
	\param ptrScene	- Сцена, чей каталог в дереве будет обновлен
*/
void SceneTreeView::UpdateSceneObjsCatalog(qdNamedObjectType type, 
										   HTREEITEM hCatalog, 
										   qdGameScene const* ptrScene)
{
	UpdateObjsCatalog(type, hCatalog, ptrScene->object_list());
}

/*!
 	Находит узел сцены, находит в ней каталог объектов указанного типа. Удаляет все 
	объекты оттуда. Затем вызывает(SceneTreeView::UpdateSceneObjsCatalog())
	\param type		- тип объектов
	\param pscene	- сцена, где будет обновлен список объектов
*/
void SceneTreeView::UpdateObjCatalogInScene(qdNamedObjectType type, 
											qdGameScene const* pscene)
{
	CString strCat = ptree_helpers::GetFolderNodeName(type);
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hScene = 
		FindTreeItemByText(tree, tree.GetChildItem(TVI_ROOT), pscene->name());

	if (hScene) {
		qdGameScene* ptrScene = 
			reinterpret_cast<qdGameScene*>(tree.GetItemData(hScene));
		HTREEITEM hCat = FindTreeItemByText(tree, hScene, strCat);
		if(hCat)
		{
			DeleteChildern(hCat);
			UpdateSceneObjsCatalog(type, hCat, ptrScene);
		}
	}
}

/*!
	Пробегает по сценам и вызывает для каждой из них(
	SceneTreeView::UpdateObjCatalogInScene()) 
	\param type - Тип объектов, список которых надо обновить
*/
void SceneTreeView::UpdateCatalogsInScenes(qdNamedObjectType type)
{
	CTreeCtrl & tree = GetTreeCtrl();
	const qdGameSceneList& gsl = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator _i = gsl.begin(), _E = gsl.end();
	for(; _i != _E; ++_i)
		UpdateObjCatalogInScene(type, *_i);
}

void SceneTreeView::OnDestroy() 
{
	CTreeView::OnDestroy();
	
	RemoveTmps();
	CImageList* temp = GetTreeCtrl().GetImageList(TVSIL_STATE);
	delete temp;

	PostThreadMessage(m_dwAnimThreadId, WM_QUIT, 0, 0);
	if(WaitForSingleObject(m_hAnimThread, 1000) == WAIT_TIMEOUT)
		TerminateThread(m_hAnimThread, 0);
}

void SceneTreeView::OnAddVideo() 
{
	HTREEITEM hItem = GetVideoCatNode();
	if(!hItem)
		return;
	
	if(AddVideo())
	{
		DeleteChildern(hItem);
		LoadVideo(hItem, m_ptrGameDisp->video_list());
		SetProjChange();
		Invalidate();
	}
}

/*!
 	Загружает из ресурсов название узла-каталога видео и ищет его среди подузлов
	корневого узла(ptree_helpers::FindTreeItemByText())
	\return Описатель узла-каталога видео
*/
HTREEITEM SceneTreeView::GetVideoCatNode()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetRootItem();
	CString str((LPTSTR)IDS_VIDEO_CATALOG);
	return FindTreeItemByText(GetTreeCtrl(), hItem, str);
}

void SceneTreeView::OnDelVideo() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	if(!hSelected)
		return;
	RemoveItem(hSelected, true);
}

/*!
 	Если видео играется, то его останавливает и запускает новое. Запускает видео таймер
	таймер.
	\param	pv - виде для запуска
*/
void SceneTreeView::PlayStopVideo(qdVideo * pv)
{
	if (m_ptrGameDisp->is_video_playing()) 
	{
		m_ptrGameDisp->stop_video();
		m_ptrGameDisp->close_video();
	}
	m_ptrGameDisp->play_video(pv);
	m_uiShowVideoTimer = SetTimer(TIMER_EVENT_PLAY_VIDEO, 1000, NULL);
}

void SceneTreeView::OnPlayStopVideo() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if(!hSel)
		return;
	qdVideo* pVideo = reinterpret_cast<qdVideo*>(tree.GetItemData(hSel));
	
}

/*!
 	Если проигрывается видео, останавливает его. Перерисовывает редактор сцены.
	(CChildView::Redraw()). Убивает видое таймер
	\param pv - видео для остановки
*/
void SceneTreeView::StopVideo(qdVideo* pv)
{
	if (m_ptrGameDisp->is_video_playing(pv)) {
		m_ptrGameDisp->stop_video();
		m_ptrGameDisp->close_video();
		sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	}
	KillTimer(m_uiShowVideoTimer);

}

void SceneTreeView::OnStopVideo() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if(!hSel)
		return;
	qdVideo* pVideo = reinterpret_cast<qdVideo*>(tree.GetItemData(hSel));
}

void SceneTreeView::ShowNewProjWiz()
{
	NewProjMultiWizard wiz;
	wiz.show(this,m_ptrGameDisp, IDS_NEW_PRJ_WIZ_CAP);
}


void SceneTreeView::OnStateCondition() 
{
	thread_suspender __ts(this);
	
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();	
	if(!hSel)
		return;
	qdGameObjectState* ptrState = 
		reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hSel));
	if(ShowCondDlg(ptrState, m_ptrGameDisp))
	{
		UpdateStateImage(ptrState, hSel);
//		if (HasGlobalFlag(ptrState)) 
//			UpdateStateImage4AllScenes(ptrState);

		SetProjChange();
	}
	if (m_ptrGameDisp->get_active_scene())
	{
		SuspendAndClear();
		SetAndResume(ptrState);
	}
}
/************************************************************************/
/* Создание зоны на сетке                                               */
/************************************************************************/
/*!
 	Пробегает по всем зонам и, если зона не имеет флага(скрытый), выделяет её
	клетки на сетке
*/
void SceneTreeView::ReselectZones()
{
	const qdGridZoneList& gzl = 
		m_ptrGameDisp->get_active_scene()->grid_zone_list();
	qdGridZoneList::const_iterator _itr = gzl.begin(), _end = gzl.end();
	for(; _itr != _end; ++_itr)
	{
		qdGridZone * ptrZone = *_itr;
		if(!ptrZone->check_flag(QD_OBJ_HIDDEN_FLAG))
			ptrZone->select(true);
	}
}

void SceneTreeView::OnCrtGridZone() 
{
	m_ptrSceneView->SetViewState(VS_EDIT_NET);
	m_ptrSceneView->SetSubTools(SSTE_CREATE_ZONE);

	qdCamera* ptrCam = m_ptrGameDisp->get_active_scene()->get_camera();

	if (ptrCam->get_redraw_mode() != QDCAM_GRID_NONE) {
		ptrCam->set_redraw_mode(QDCAM_GRID_ABOVE);
	}
	
	m_ptrSceneView->OnCreateGridZone();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

/*!
	Находим исходную зону. Если надо сохранить данные, то копируем измененную копию
	поверх исходной, если надо отклонить данные, то наоборот
	\param bSave - сохранить/отклонить изменения
	\return true - если операция завершилась успешно
*/
bool SceneTreeView::OnSaveGZChanges(bool bSave)
{
	if (!m_ptrEditedGZ.get())
		return false;
	HTREEITEM hZone = FindObjectItem(m_ptrEditedGZ.get());
	if (!hZone) {
		Beep(1000, 100);
		return false;
	}
	qdGridZone * ptrZone = 
		reinterpret_cast<qdGridZone*>(GetTreeCtrl().GetItemData(hZone));
	if (bSave) {
		*ptrZone = *m_ptrEditedGZ;
	}
	else
		*m_ptrEditedGZ = *ptrZone;
	return true;
}
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* Создание масок                                                       */
/************************************************************************/
void SceneTreeView::OnCreateMask() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	ASSERT(hSel);

	qdGameObject* ptrGO = 
		reinterpret_cast<qdGameObject*>(tree.GetItemData(hSel));
	
	m_ptrSceneView->SetViewState(VS_EDIT_MASK);
	m_ptrSceneView->SetSubTools(SSTE_CREATE_MASK);
	if (!m_ptrSceneView->OnCreateStateMask(ptrGO))
	{
		m_ptrSceneView->SetViewState(VS_EDIT_SCENE);
		m_ptrSceneView->SetSubTools(SSTE_NONE);
	}
}

void SceneTreeView::OnCrtAnimObjByMask()
{
	m_ptrSceneView->SetViewState(VS_EDIT_MASK);
	m_ptrSceneView->SetSubTools(SSTE_CREATE_MASK);
	if (!m_ptrSceneView->OnCreateStateMask(reinterpret_cast<qdGameObject*>(-1)))
	{
		m_ptrSceneView->SetViewState(VS_EDIT_SCENE);
		m_ptrSceneView->SetSubTools(SSTE_NONE);
	}
}

/*!
 	Если состояние-маска не имеет владельца(qdNamedObject::owner()), то спрашиваем у 
	юзера, куда он хочет добавить новое состояние. Если владелец есть, то просто 
	добавляем ему состояние.Если владельц == -1, то создаем динамический объект без 
	вопросов. После добавления перезагружаем список состояний объекта, которому было
	добавлено состояние маска(ptree_loading::ReloadStateCatalog()).
	\param ptrStateMask - только что созданное состояние-маска
	\return \a true - есил добавление закончилось успешно
*/
bool SceneTreeView::OnEndStateMaskCreate(qdGameObjectStateMask* ptrStateMask)
{
	qdGameObjectAnimated* pao = NULL;
	if (ptrStateMask->owner())
	{
		//значит маску создавали кликнув в меню
		//анимированного объекта
		pao = static_cast<qdGameObjectAnimated*>(ptrStateMask->owner());
		if (pao == reinterpret_cast<qdGameObjectAnimated*>(-1))
		{
			pao = AddAnimated(m_ptrGameDisp->get_active_scene(), CString(), false);
			if (!pao)
				return false;
		}
	}
	else
	{
		std::pair<bool, qdGameObject*> res =
			AskForObject(m_ptrGameDisp->get_active_scene(), 
						QD_NAMED_OBJECT_ANIMATED_OBJ,  true);
		if (!res.first)
			return false;
		if (!res.second) {
			res.second = 
				AddAnimated(m_ptrGameDisp->get_active_scene(), CString(), false);
			if (!res.second) 
				return false;
		}
		pao = static_cast<qdGameObjectAnimated*>(res.second);
	}	
	ASSERT(pao);
	CString str;
	str.Format(IDS_MASK_NAME, ptrStateMask->parent_name());

	str = AskUserForName((const CString&)str);
	str = get_unique_name(str, pao->state_vector());
	ptrStateMask->set_name(str);
	if(pao->add_state(ptrStateMask))
	{
		ReloadStateCatalog(GetTreeCtrl(),
			FindObjectItem(pao), pao, false);

		ShowItem(FindObjectItem(ptrStateMask));
		pao->set_default_state();
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////

void SceneTreeView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (GetFocus() != this&&!GetTreeCtrl().GetEditControl()
		&&GetForegroundWindow()==m_ptrMainFrame) 
		SetFocus();	
	CTreeView::OnMouseMove(nFlags, point);
}
/************************************************************************/
/* Управление каталогом звуков                                          */
/************************************************************************/
//  [26/7/2002]
void SceneTreeView::OnSoundList() 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	ASSERT(hItem);
	
	hItem = GetDispatcherNode(GetTreeCtrl(), hItem);
	ShowSoundCatalogDlg(hItem, true);
}

void SceneTreeView::OnEditSound() 
{
	HTREEITEM hItem = m_hCurrScene;
	ASSERT(hItem);
	if(hItem)
		ShowSoundCatalogDlg(hItem, false);
}

void SceneTreeView::OnUpdateEditSound(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_hCurrScene != NULL&&m_IsSceneViewActive);
}

/*!
 	Достаёт из узла диспетчер(CTreeCtrl::GetItemData()). Его звуки будут загружены.
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
void SceneTreeView::ShowSoundCatalogDlg(HTREEITEM hItem, bool bUniqueList)
{
	CTreeCtrl& tree = GetTreeCtrl();
	qdNamedObject* pno = 
		reinterpret_cast<qdNamedObject*>(tree.GetItemData(hItem));
	
	qdGameDispatcherBase* ptrGB = static_cast<qdGameDispatcherBase*>( pno);
	
	SoundCatalogDlg dlg;
	dlg.set_data(m_ptrGameDisp, ptrGB);
	
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
		SetProjChange();
	
}
//////////////////////////////////////////////////////////////////////////


void SceneTreeView::OnSelScene() 
{
	HTREEITEM hSelected = GetTreeCtrl().GetSelectedItem();
	GetTreeCtrl().SelectItem(NULL);
	SceneList sl;
	sl.set_data(m_ptrGameDisp);
	if(sl.DoModal() == IDOK)
	{
		qdGameScene* ptrScene = sl.get_scene();
		if(ptrScene)
			ActivateScene(ptrScene);
		else
			AddScene(CString(), true);
		CollapseAllScenes(m_ptrGameDisp->get_active_scene());
	}
	else
		GetTreeCtrl().SelectItem(hSelected);
}

void SceneTreeView::OnPersDim() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	ASSERT(tree.GetSelectedItem());
	qdGameObjectMoving* const ptrO = 
		reinterpret_cast<qdGameObjectMoving*>(tree.GetItemData(tree.GetSelectedItem()));

	ASSERT (!IsGlobalObj(ptrO));

	OnEditAnimatedDims(ptrO);
}

void SceneTreeView::OnPersDir() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	ASSERT(tree.GetSelectedItem());
	qdGameObjectMoving* ptrO = reinterpret_cast<qdGameObjectMoving*>(
									tree.GetItemData(tree.GetSelectedItem()));
	
	ASSERT (!IsGlobalObj(ptrO));
		
	OnEditAnimatedDir(ptrO);
}

/*!
 	Запускает режим редактирования размеров динамического объекта в редакторе сцены
	\param ptrObj - объект, размеры которого будем редактировать
	\return \a true - всегда
*/
bool SceneTreeView::OnEditAnimatedDims(qdGameObjectAnimated* ptrObj)
{
	m_ptrSceneView->SetViewState(VS_EDIT_ANIMATED);
	m_ptrSceneView->SetSubTools(SSTE_EDIT_DIMS);
	
	m_ptrSceneView->OnEditAnimated(ptrObj);
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	return true;
}

/*!
 	Запускает режим редактирования направления динамического объекта в редакторе сцены
	\param ptrObj - объект, направление которого будем редактировать
	\return \a true - всегда
	\remarks Реально направление можно выставлять только персонажам. Поэтому эта функция
	будет вызываться только для персонажей
*/
bool SceneTreeView::OnEditAnimatedDir(qdGameObjectAnimated* ptrObj)
{
	m_ptrSceneView->SetViewState(VS_EDIT_ANIMATED);
	m_ptrSceneView->SetSubTools(SSTE_EDIT_DIR);
	
	m_ptrSceneView->OnEditAnimated(ptrObj);
	sv_api::Redraw(sv_api::RM_NO_CONTENT_REDRAW);
	return true;
}

/*!
 	Получает указатель на менеджер настроек(CQuestEditorApp::GetProjectSettings())
	Очищает нужную секцию (IProjectSettings::ClearSection())
	Сохраняет текущую сцену(SceneTreeView::SaveCurrentScene())
	Сохраняет состояния узлов(SceneTreeView::SaveSceneItemsState())
	Сохраняет режим прорисовки сетики (SceneTreeView::SaveNetDrawMode())
	Сохраняет активный редактор	(SceneTreeView::SaveActiveView())
*/
void SceneTreeView::SaveIni()
{
	IProjectSettings* pps = GetMyApp()->GetProjectSettings();

	pps->ClearSection(CString((LPTSTR)IDS_INI_SEC_NAME_TREE));	

	SaveCurrentScene(pps);
	SaveSceneItemsState(pps);
	SaveNetDrawMode(pps);
	SaveShowTriggerElement(pps);
	m_ptrITView->SaveIniSettings(pps);
	SaveActiveView(pps);
}

/*!
 	Читает состояния узлов, режим прорисовки сетки, текущую сцену, активный редактор,
	настройки для редактора триггеров
*/
void SceneTreeView::ReadIni()
{
	CWaitCursor wait;
	IProjectSettings* pps = GetMyApp()->GetProjectSettings();

	ReadSceneItemState(pps);
	ReadNetDrawMode(pps);

	//здесь происходит загрузка и перерисовка
	ReadCurrentScene(pps);
	m_ptrITView->ReadIniSettings(pps);
	ReadActiveView(pps);
	ReadShowTriggerElement(pps);
	m_ptrITView->OnEndIniReading();
}

void SceneTreeView::SaveSelectedItem(IProjectSettings* pps)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();

	CString strData;
	while (hSelected) {
		strData += tree.GetItemText(hSelected);
		strData += _T("\\");
		hSelected = tree.GetParentItem(hSelected);
	}
	pps->WriteStrSetting(CString((LPTSTR)IDS_INI_SEC_NAME_TREE), 
				CString((LPTSTR)IDS_INI_KEY_NAME_SELECTED), strData);
}

void SceneTreeView::SaveCurrentScene(IProjectSettings* pps)
{
	CString strData;
	if (m_hCurrScene)
		strData = GetTreeCtrl().GetItemText(m_hCurrScene);

	pps->WriteStrSetting(CString((LPTSTR)IDS_INI_SEC_NAME_TREE), 
		CString((LPTSTR)IDS_INI_KEY_NAME_CUR_SCENE), strData);
}

void SceneTreeView::SaveSceneItemsState(IProjectSettings* pps)
{
	CTreeCtrl & tree = GetTreeCtrl();
	HTREEITEM hRoot = tree.GetChildItem(TVI_ROOT);
	if (!hRoot)
		return;
	
	if (HTREEITEM hChild = tree.GetChildItem(hRoot)) {
		CString strSecName((LPTSTR)IDS_INI_SEC_NAME_TREE);
		
		do {
			CString strText = tree.GetItemText(hChild);
			
			UINT uiState = tree.GetItemState(hChild, TVIS_EXPANDED);
			pps->WriteIntSetting(strSecName, strText, 
				static_cast<int>((uiState&TVIS_EXPANDED) == TVIS_EXPANDED));
		} while((hChild = tree.GetNextSiblingItem(hChild)));
	}
}
void SceneTreeView::SaveNetDrawMode(IProjectSettings* pps)
{
	if (!m_ptrGameDisp)
		return;
	const qdGameSceneList& gsl = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator _itr = gsl.begin(), _end = gsl.end();
	CString strKeyName((LPTSTR)IDS_INI_KEY_NAME_NET_DRAW_MODE);
	for(;_itr != _end; ++_itr)
	{
		pps->WriteIntSetting((*_itr)->name(), strKeyName,
			(*_itr)->get_camera()->get_redraw_mode());
	}
}

void SceneTreeView::SaveActiveView(IProjectSettings* pps)
{
	pps->WriteIntSetting(CString((LPTSTR)IDS_INI_SEC_NAME_APP),
		CString((LPTSTR)IDS_INI_KEY_NAME_ACTIVE_VIEW),
		static_cast<int>(m_IsSceneViewActive));
}

//! Сохраняет в настройках проекта флаг showTirgger_
void SceneTreeView::SaveShowTriggerElement(interface IProjectSettings* pps)
{
	pps->WriteIntSetting(CString((LPTSTR)IDS_INI_SEC_NAME_APP),
		CString((LPTSTR)IDS_INI_KEY_NAME_SHOW_TRIGGER),
		static_cast<int>(showTriggerElement_));
}

//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* Чтение                                                               */
/************************************************************************/

/*!
	\param pps - Указатель на менеджер настроек проекта
*/
void SceneTreeView::ReadCurrentScene(IProjectSettings* pps)
{
	CString res = pps->GetStrSetting(CString((LPTSTR)IDS_INI_SEC_NAME_TREE), 
		CString((LPTSTR)IDS_INI_KEY_NAME_CUR_SCENE), CString());
	if (res.IsEmpty()) 
		return;

	CTreeCtrl & tree = GetTreeCtrl();
	HTREEITEM hRoot = tree.GetChildItem(TVI_ROOT);
	ASSERT(hRoot);

	HTREEITEM hItem = FindTreeItemByText(tree, hRoot, res);
	if(hItem)
		tree.SelectItem(hItem);
}

/*!
	\param strSecName	- Имя секции в файле настройки
	\param lpszItemName	- Имя узла, для которого устанавливаем состояние
	\param pps			- Указатель на менеджер настроек проекта
	\param hPar			- Родительский узел в дереве
*/
void SceneTreeView::AdjustItemState(const CString& strSecName, 
									LPCTSTR lpszItemName, 
									IProjectSettings* pps, 
									HTREEITEM hPar)
{
	int iState = pps->GetIntSetting(strSecName, lpszItemName, 0);
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = FindTreeItemByText(tree, hPar, lpszItemName);
	if(hItem&&iState)
		tree.Expand(hItem, TVE_EXPAND);
}

void SceneTreeView::ReadSceneItemState(IProjectSettings* pps)
{
	CTreeCtrl & tree = GetTreeCtrl();
	HTREEITEM hRoot = tree.GetChildItem(TVI_ROOT);
	ASSERT(hRoot);
		
	CString strMouseNodeName((LPTSTR)IDS_MOUSE_OBJ_NODE);
	CString strVideoNodeName((LPTSTR)IDS_VIDEO_CATALOG);
	CString strSecName((LPTSTR)IDS_INI_SEC_NAME_TREE);

	AdjustItemState(strSecName, strMouseNodeName, pps, hRoot);
	AdjustItemState(strSecName, strVideoNodeName, pps, hRoot);

	//определяемся со сценами
	const qdGameSceneList& gsl = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator _itr = gsl.begin(), _end = gsl.end();
	for(; _itr != _end; ++_itr)
	{
		qdGameScene* ptrScene = *_itr;
		AdjustItemState(strSecName, ptrScene->name(), pps, hRoot);
	}
}

/*!
	\param ptrScene		- Сцена, для сетки которой устанавливается режим
	\param lpszItemName	- Имя настройки в ини файле
	\param pps			- Указатель на менеджер настроек проекта
*/
void SceneTreeView::AdjustNetDrawMode(qdGameScene* ptrScene, 
									  LPCTSTR lpszItemName, 
									  IProjectSettings* pps)
{
	int iMode = pps->GetIntSetting(ptrScene->name(), lpszItemName, 1);
	ptrScene->get_camera()->set_redraw_mode(iMode);

	HTREEITEM hItem = FindObjectItem(ptrScene);
	ASSERT(hItem);
	hItem = FindTreeItemByText(GetTreeCtrl(), hItem, 
								CString((LPCTSTR)IDS_NET_NODE_NAME));
	ASSERT(hItem);

	if(iMode == static_cast<int>(QDCAM_GRID_NONE))
		SetCheck(hItem, FALSE);
}

/*!
	Пробегает по всем сценам и вызывает SceneTreeView::AdjustNetDrawMode() для каждой
	из них
	\param pps - Указатель на менеджер настроек проекта
*/
void SceneTreeView::ReadNetDrawMode(IProjectSettings* pps)
{
	CString strKeyName((LPTSTR)IDS_INI_KEY_NAME_NET_DRAW_MODE);

	const qdGameSceneList& gsl = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator _itr = gsl.begin(), _end = gsl.end();
	for(;_itr != _end; ++_itr)
	{
		qdGameScene* ptrScene = *_itr;
		AdjustNetDrawMode(ptrScene, strKeyName, pps);
	}
}

/*!
	\param pps - Указатель на менеджер настроек проекта
*/
void SceneTreeView::ReadActiveView(IProjectSettings* pps)
{
	bool const val = static_cast<bool>(
		pps->GetIntSetting(CString((LPTSTR)IDS_INI_SEC_NAME_APP),
			CString((LPTSTR)IDS_INI_KEY_NAME_ACTIVE_VIEW), 1));
	if (!val) {
		if (!m_ptrGameDisp->trigger_chain_list().empty()) 
		{
			//! Кого показывали последним
			CString const str = m_ptrITView->GetLastEditedTrigger();
			if (str == CString((LPTSTR)IDS_ALL_TRIGGER_CHAINS_NAME))
			{
				//! Значит показываем все
				m_IsSceneViewActive = false;
				ShowAllTriggers();
			}
			else
			{
				qdTriggerChainList const& lst = m_ptrGameDisp->trigger_chain_list();
				//! Находим по имени, если не нашли, то берем первую
				qdTriggerChain* chainToShow = 
					ptree_helpers::findTriggerChainByName(lst, str);
				if (!chainToShow)
					chainToShow = lst.front();
				m_ptrITView->ShowTriggers(&chainToShow, 1);
				m_IsSceneViewActive = false;
			}
			return;
		}
		m_IsSceneViewActive = false;
	}
}

//! Считывает из настроек проекта флаг showTirgger_
void SceneTreeView::ReadShowTriggerElement(interface IProjectSettings* pps)
{
	showTriggerElement_ = 
		static_cast<bool>(pps->GetIntSetting(CString((LPTSTR)IDS_INI_SEC_NAME_APP),
			CString((LPTSTR)IDS_INI_KEY_NAME_SHOW_TRIGGER), 1));
}

//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* Работа с редактором триггеров                                        */
/************************************************************************/
void SceneTreeView::OnTbtnAllStates2Editor() 
{
	SceneList dlg;
	dlg.set_data(m_ptrGameDisp);
	dlg.set_multi_select(true);
	dlg.set_show_crt_new(false);
	if (dlg.DoModal() == IDOK) {

		qdGameScene** ptrScenes = dlg.get_scenes();
		int nCount = dlg.count();

		for(int i = 0; i < nCount; ++i)
		{
			qdGameScene* ptrScene = ptrScenes[i];
			
			const qdGameObjectList& gol = ptrScene->object_list();
			qdGameObjectList::const_iterator _itr = gol.begin(), 
										_end = gol.end();

			CPoint offset(-1, -1);
			
			for(; _itr != _end; ++_itr)
			{
				qdGameObject* ptrObj = *_itr;
				if(ptrObj->named_object_type() != QD_NAMED_OBJECT_STATIC_OBJ)
				{
					qdGameObjectAnimated* ptrAnimObj = 
						static_cast<qdGameObjectAnimated*>(ptrObj);
					
					const qdGameObjectStateVector& vec = 
												ptrAnimObj->state_vector();
					ASSERT(!vec.empty());
					if(m_ptrITView->AddState(offset, &*vec.begin(), vec.size()))
						SetProjChange();
				}
			}
		}
	}
}

void SceneTreeView::OnCrtTrigger() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	ASSERT(hSelected);

	CString strName((LPTSTR)IDS_NEW_TRIGGER_CHAIN_NAME);
	if(AskUserForName(strName))
	{
		if (is_trigger_item(tree, hSelected)) 
			hSelected = tree.GetParentItem(hSelected);

		AddTriggerChain(hSelected, strName, true);
	}
}

//////////////////////////////////////////////////////////////////////////
/*!
 	Получает выбранный узел и вызывает SceneTreeView::OnSelchanged4Old(HTREEITEM hItem),
	чтобы сказать, что узел перестал быть выбранным
	\param bIsSVActiveOld - старое состояние редактора сцены(был ли он активен)
	\param bIsSVActiveNew - новое состояние редактора сцены(станет ли он активен)
	\return \retval true - окна можно поменять местами
			\retval false - окна нельзя поменять местами.
*/
bool SceneTreeView::OnChangingViews(bool bIsSVActiveOld, bool bIsSVActiveNew)
{
	if (!m_ptrITView->OnVisibilityChanging(
		bIsSVActiveNew? ITriggerView::VCA_HIDE : ITriggerView::VCA_SHOW))
		return false;
	HTREEITEM hSelected = GetTreeCtrl().GetSelectedItem();
	if (!hSelected)
		return true;
	OnSelchanged4Old(hSelected);
	return true;
}
/*!
	В менеджере масштаба устанавливается масштаб соответствующий текущему редактору
	(сцены или триггеров).
	Получает выбранный узел и вызывает SceneTreeView::OnSelchanged4New(HTREEITEM hItem),
	чтобы сказать, что узел стал выбранным(см. замечания примечания). Выполняет другие
	действия связанные с активацией редакторов.
	\remarks При переключении редакторов эмулируется потеря узлом состояния "Выбранный",
	а затем получаение этого состояния. Это делается для того, чтобы неактивном редакторе
	зачистить вещи связанные с выделением, а в активном с эмулировать выделеление узла
	\param bIsSVActiveOld - старое состояние редактора сцены(был ли он активен)
	\param bIsSVActiveNew - новое состояние редактора сцены(станет ли он активен)
*/
void SceneTreeView::OnChangeViews(bool bIsSVActiveOld, bool bIsSVActiveNew)
{

	if (bIsSVActiveNew)
		m_ptrIScaleMgr->SetScalable(m_ptrSceneView);
	else
		m_ptrIScaleMgr->SetScalable(dynamic_cast<IScalable*>(m_ptrITView));
	m_ptrIScaleMgr->UpdateScaleInfo();

	m_ptrITView->OnVisibilityChange(
						bIsSVActiveNew? 
							ITriggerView::VCA_HIDE 
							: ITriggerView::VCA_SHOW);

	HTREEITEM hSelected = GetTreeCtrl().GetSelectedItem();
	if (!hSelected)
		return ;
	//если сейчас будет виден редактор триггеров,
	//и список триггеров пуст, то создаем триггер
	if (!bIsSVActiveNew&&m_ptrGameDisp->trigger_chain_list().empty()) 
	{
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
	}

	if (bIsSVActiveNew) //т.е. показался редактор сцены(SceneView)
		m_bActivateTgrView = false;

	OnSelchanged4New(hSelected);
	if (!bIsSVActiveNew) 
		SetTgrIcon4All(TRUE);
	if (!m_bActivateTgrView)
		m_bActivateTgrView = true;
}

/*!
	Передает управление CMainFrame::ShowSceneView()	
*/
void SceneTreeView::ShowSceneView()
{
	m_ptrMainFrame->ShowSceneView();
}

/*!
	Передает управление CMainFrame::ShowTEView()	
*/
void SceneTreeView::ShowTEView()
{
	m_ptrMainFrame->ShowTEView();
}

void SceneTreeView::OnEditTriggers() 
{
	if (m_IsSceneViewActive) 
	{
		const qdTriggerChainList& lst = m_ptrGameDisp->trigger_chain_list();
		if (lst.empty()) 
			return;

		qdTriggerChain* pc4show = NULL;
		CTreeCtrl& tree = GetTreeCtrl();
		HTREEITEM hSelected = tree.GetSelectedItem();

		if (ptree_helpers::is_trigger_item(tree, hSelected)) 
			pc4show = reinterpret_cast<qdTriggerChain*>(tree.GetItemData(hSelected));

		if (ptree_helpers::is_container_type(tree, 
										hSelected, QD_NAMED_OBJECT_TRIGGER_CHAIN)) 
			pc4show = ALL_SHOWED;
		else
			pc4show = m_ptrITView->ShowedTrigger();

		if (pc4show == ALL_SHOWED)
		{
			SetTgrIcon4All(TRUE);
			ShowAllTriggers();
		}
		else if (!pc4show)
		{
			CString str = m_ptrITView->GetLastEditedTrigger();
			if (str == CString((LPTSTR)IDS_ALL_TRIGGER_CHAINS_NAME))
			{
				//! Значит показываем все
				//! В этом случае pc4show == NULL
				ShowAllTriggers();
			}
			else
			{
				pc4show = ptree_helpers::findTriggerChainByName(lst, str);
				if (!pc4show)
					pc4show = lst.front();
			}
		}

		if (pc4show)
			m_ptrITView->ShowTriggers(&pc4show, 1);
		m_IsSceneViewActive = false;
	}
	else
		m_IsSceneViewActive = true;	
}

void SceneTreeView::OnUpdateTbtnAllStates2Editor(CCmdUI* pCmdUI) 
{
	BOOL bEnable = !m_IsSceneViewActive;
	if (bEnable&&m_ptrGameDisp) 
		bEnable = !m_ptrGameDisp->trigger_chain_list().empty();

	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	if (bEnable&&hItem)
	{
		DWORD dwData = tree.GetItemData(hItem);
		if (!is_obj(dwData))
		{
			if (dwData == QD_NAMED_OBJECT_TRIGGER_CHAIN)
				bEnable = FALSE;
		}
	}
	pCmdUI->Enable(bEnable);
}


void SceneTreeView::OnNotControledPers() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel) {
		return;
	}
	
	qdNamedObject* pno = 
		reinterpret_cast<qdNamedObject*>(tree.GetItemData(hSel));
	
	if (pno->check_flag(QD_OBJ_NON_PLAYER_PERSONAGE_FLAG)) {
		pno->drop_flag(QD_OBJ_NON_PLAYER_PERSONAGE_FLAG);
	}
	else
		pno->set_flag(QD_OBJ_NON_PLAYER_PERSONAGE_FLAG);
}


void SceneTreeView::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	CTreeCtrl& tree = GetTreeCtrl();
	UINT uFlags = TVHT_ONITEMSTATEICON;
	HTREEITEM hItem = tree.HitTest(point,&uFlags);
	if(!hItem || !(uFlags&TVHT_ONITEMSTATEICON))
		return;
	
	*pResult = 1;
	if(!is_state_node(tree, hItem))
		SetCheck(hItem, !GetCheck(hItem));
}

/*!
 	Скрывает/показывает объекты
*/
void SceneTreeView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTreeView::OnLButtonDown(nFlags, point);

	CTreeCtrl& tree = GetTreeCtrl();
	UINT uFlags;
	HTREEITEM hItem = tree.HitTest(point,&uFlags);
	if(!hItem || !(uFlags&TVHT_ONITEM))
		return;
	if(uFlags&TVHT_ONITEMSTATEICON)
	{
		int iCheck = GetCheck(hItem);
		if(iCheck != -1)
		{
			DWORD_PTR data = tree.GetItemData(hItem);
			if (is_obj(data)) 
			{
				std::pair<qdNamedObjectBase*, bool> pr = 
					ptree_helpers::is_any_interf_obj(data);
				if (pr.second)
				{
					if (qdInterfaceElement* pele = 
						dynamic_cast<qdInterfaceElement*>(pr.first))
					{
						qdInterfaceScreen* const pscr = 
							static_cast<qdInterfaceScreen*>(pele->owner());
						if (iCheck)
							pscr->show_element(pele->name());
						else
							pscr->hide_element(pele->name(), false);
					}
				}
				else
				{
					qdNamedObject * pno = 
						reinterpret_cast<qdNamedObject*>(data);
					if(iCheck)
						pno->drop_flag(QD_OBJ_HIDDEN_FLAG);
					else
						pno->set_flag(QD_OBJ_HIDDEN_FLAG);
					
					//если это зона, то надо убрать выделение 
					//с сетки и контур этой зоны
					if (pno->named_object_type()==QD_NAMED_OBJECT_GRID_ZONE)
						OnCheckChangeGZone(iCheck, static_cast<qdGridZone*>(pno));
				}
			}
			else
			{
				if(data == QD_NAMED_OBJECT_GRID_ZONE)
					OnCheckChangeNet(iCheck, hItem);
			}
			sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
		}
	}
}

/*!
 	Если убрали галочку на чекбоксе, то просто говорим сетке не рисоваться.
	Если галочку поставили, то,  если выделен узел сетки
	(редактор сцены в режиме редактирования сетки), то рисуем сетку поверх всего. Иначе
	рисуем с учетом положения  в пространстве каждой точки сетки.
	\param iCheck	- состояние чекбокса
	\param hItem	- узел сетки
*/
void SceneTreeView::OnCheckChangeNet(int iCheck, HTREEITEM hItem)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSceneItem = IsObjUnderScene(tree, hItem);
	ASSERT(hSceneItem);
	
	qdGameScene* ptrScene = 
		reinterpret_cast<qdGameScene*>(tree.GetItemData(hSceneItem));
	if (iCheck) {
		ASSERT(ptrScene == m_ptrGameDisp->get_active_scene());
		if(hItem == tree.GetSelectedItem())
			ptrScene->get_camera()->set_redraw_mode(QDCAM_GRID_ABOVE);
		else
			ptrScene->get_camera()->set_redraw_mode(QDCAM_GRID_ZBUFFER);
	}
	else
		ptrScene->get_camera()->set_redraw_mode(QDCAM_GRID_NONE);
}

/*!
 	Если редактор сцены в не состоянии редактирования сетки(Не выбран узел "Сетка"),
	то ничего не делаем. Если редактируется зона, и имя pzone совпадает с m_ptrEditedGZ
	(значит редактируем pzone), то устанавливаем/сбрасываем для m_ptrEditedGZ флаг 
	QD_OBJ_HIDDEN_FLAG, а для pzone - ставим/убираем выделение ее ячеек на сетке.
	Если нет, то просто убирает выделение данной зоны и перевибираем все зоны заново
	(SceneTreeView::ReselectZones())
	\param iCheck	- состояние чекбокса
	\param pzone	- Зона, чекбокс которой поменял состояние
*/
void SceneTreeView::OnCheckChangeGZone(int iCheck, qdGridZone* pzone)
{
	//если это зона, то надо убрать выделение с сетки и контур этой зоны
	if (m_ptrSceneView->GetViewState()==VS_EDIT_NET) {
		
		if (m_ptrSceneView->GetSubTools()==SSTE_NONE
			||m_ptrSceneView->GetSubTools()==SSTE_EDIT_ZONE_AND_SHOW_ALL) {
			//если рисуем все зоны
			if(!iCheck)
				pzone->select(false);
			
			ReselectZones();
		}
		else if (m_ptrSceneView->GetSubTools() == SSTE_EDIT_ZONE) 
		{
			if(m_ptrEditedGZ->name()
				&&!strcmp(m_ptrEditedGZ->name(), pzone->name()))
			{
				//если галочку убрали у зоны, 
				//которая в данный момент редактируется,то...
				pzone->select(!pzone->check_flag(QD_OBJ_HIDDEN_FLAG));
				
				if(iCheck)
					m_ptrEditedGZ->drop_flag(QD_OBJ_HIDDEN_FLAG);
				else
					m_ptrEditedGZ->set_flag(QD_OBJ_HIDDEN_FLAG);
			}
		}
	}
}

/*!
	Пробегает по всем цепочкам и вызвает для них SceneTreeView::SetInTgrIcon()
	\param bSet - \a TRUE установить букву, \a FALSE убрать
*/
void SceneTreeView::SetTgrIcon4All(BOOL bSet)
{
	const qdTriggerChainList& tcl = m_ptrGameDisp->trigger_chain_list();
	qdTriggerChainList::const_iterator i = tcl.begin(), e = tcl.end();
	qdTriggerChain* const pactive = m_ptrITView->ShowedTrigger();
	for(; i != e; ++i)
	{
		if (pactive != *i)
			SetInTgrIcon(*i, bSet, II_NOT_ACTIVE_TRIGGER);
	}
	if (pactive&&pactive != ALL_SHOWED)
		SetInTgrIcon(pactive, bSet, II_ACTIVE_TRIGGER);
}

/*!
	Пробегает по всем элементам цепочки. Достает из них объекты. Находит узлы в дереве,
	соответствующие этим объектам. Устанавливает/убирает изображение (
	ptree_loading::SetInTrigger())
	\param pchain	- цепочка, для элементов которой устанавливает/убирается 
						изображение буквы "Т"
	\param bSet		- установить/убрать изображение
	\param image_id	- номер изображения, которое надо установить. Есил bSet = FALSE - 
						игнорируется
	\remarks
 		Для объектов, входящих в неактивную цепочку, устанавливается черная буква "Т",
		для входящих в активную - красная.
*/
void SceneTreeView::SetInTgrIcon(qdTriggerChain* pchain, BOOL bSet, int image_id)
{
	CTreeCtrl& tree = GetTreeCtrl();

	const qdTriggerElementList& lst = pchain->elements_list();
	qdTriggerElementList::const_iterator _i = lst.begin(), _E = lst.end();
	for(; _i != _E; ++_i)
	{
		qdTriggerElementPtr const& pe = *_i;
		qdNamedObject* po = pe->object();
		if (po) 
		{
			HTREEITEM hItem = FindObjectItem(po);
			SetInTrigger(tree, hItem, bSet, image_id);
		}
	}
}

BOOL SceneTreeView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll) 
{
	if (!bDoScroll) {
		int dx = 0;
		int dy = 0;
		switch(HIBYTE(nScrollCode)) {
		case SB_LINEUP:
			dy = -1;
			break;
		case SB_LINEDOWN:
			dy = 1;
			break;
		}
		switch(LOBYTE(nScrollCode)) {
		case SB_LINELEFT:
			dx = -1;
			break;
		case SB_LINERIGHT:
			dx = 1;
			break;
		}
		int sbmin, sbmax, sbpos;
		if (dx) {
			GetScrollRange(SB_HORZ,&sbmin, &sbmax);
			sbpos = GetScrollPos(SB_HORZ);
			if (sbmin < sbpos + dx&&sbpos+dx<sbmax) {
				SetScrollPos(SB_HORZ, sbpos + dx);
				SendMessage(WM_HSCROLL, 
					MAKEWPARAM(LOBYTE(nScrollCode), 0), NULL);
			}
		}
		if (dy) {
			GetScrollRange(SB_VERT,&sbmin, &sbmax);
			sbpos = GetScrollPos(SB_VERT);
			if (sbmin < sbpos + dy&&sbpos+dy<sbmax) {
				SetScrollPos(SB_VERT, sbpos + dy);
				SendMessage(WM_VSCROLL, 
					MAKEWPARAM(HIBYTE(nScrollCode), 0), NULL);
			}
		}
	}
	return TRUE;
}

void SceneTreeView::OnAdjustGameArea() 
{
	CWaitCursor _wait_;
	//Здесь на самом деле выравнивается рабочая область :-)
	HTREEITEM hSelected = GetTreeCtrl().GetSelectedItem();
	qdGameObjectStatic* pobj = reinterpret_cast<qdGameObjectStatic*>
									(GetTreeCtrl().GetItemData(hSelected));

	qdCamera* pcam = m_ptrGameDisp->get_active_scene()->get_camera();
	Vect2i old_scr_size = pcam->get_scr_size();
	Vect2i old_scr_offset = pcam->get_scr_offset();

	Vect2s obj_size = pobj->screen_size();
	
	pcam->set_scr_size(obj_size.x, obj_size.y);
	Vect2i sc = pcam->get_scr_center();
	
	pcam->set_scr_offset(pobj->screen_pos() - Vect2s(sc.x, sc.y));
	m_ptrSceneView->AdjustBitmapSize();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	
	if (old_scr_size != pcam->get_scr_size()
		||old_scr_offset != pcam->get_scr_offset()) {
		SetProjChange();
	}
}

void SceneTreeView::OnUpdateEditGaits(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_IsSceneViewActive&&m_ptrGameDisp);	
}

void SceneTreeView::OnUpdateSelScene(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_IsSceneViewActive&&m_ptrGameDisp);
}

/*!
 	В зависимости от типа объекта выбриает функцию получения номер изображения.
	Получает посредством неё номер изображения и назначает его узлу объекта
	\remarks Для персонажа функция выбра изображения не задана
	\param pobj - объект
*/
void SceneTreeView::UpdateObjImage(const qdGameObject* pobj){
	int (*SelPic)(const qdGameObject* pa) = NULL;
	if (ptree_helpers::is_obj_static(pobj)) 
		SelPic = ptree_helpers::SelStaticObjPic;
	else if (ptree_helpers::is_obj_animated(pobj))
		SelPic = ptree_helpers::SelAnimObjPic;
	if (SelPic)
	{
		int iImage = SelPic(pobj);
		GetTreeCtrl().SetItemImage(FindObjectItem(pobj), iImage, iImage);
	}
}

/*!
 	Если \a "hItem == NULL" находит узел объекта. Пробегает по всем подузлам и обновляет
	изображение для состояний(SceneTreeView::UpdateObjImage(const qdGameObject* pobj))
	\param pobj		- Объект, для состояний которого обновляются изображения
	\param hItem	- Узел объекта. Может быть NULL.
*/
void SceneTreeView::UpdateObjStatesImages(qdGameObjectAnimated const* pobj, 
										  HTREEITEM hItem)
{
	if(!hItem)
		hItem = FindObjectItem(pobj);
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hChild = tree.GetChildItem(hItem);
	if (hChild)
	{
		do {
			qdGameObjectState* pstate = 
				reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hChild));
			UpdateStateImage(pstate, hChild);
		} while(hChild = tree.GetNextSiblingItem(hChild));
	}
}

/*!
 	Если состояние глобальное, то ничего не делаем. Если \a "hItem = NULL" - находит 
	узел состояния. Определяет номер изображения для состояния(
	ptree_helpers::SelStatePic()). И назначает его для узла
	\param pstate	- Состояние, для которого обновляем изображение
	\param hItem	- Узел состояния. Может быть NULL
*/
void SceneTreeView::UpdateStateImage(qdGameObjectState* pstate,	
										HTREEITEM hItem)
{
	if (HasGlobalFlag(pstate)) {
		//ничего не делаем, потому что для глоабльных состояний
		//один раз прибили икнку и она у них пока не меняется
		return;
	}
	if(!hItem)
		hItem = FindObjectItem(pstate);

	const UINT nImage = 
		ptree_helpers::SelStatePic(pstate, 
					static_cast<qdGameObjectAnimated*>(pstate->owner()));

	GetTreeCtrl().SetItemState(hItem, pstate->is_default() ? (INDEXTOOVERLAYMASK(II_DEFAULT_STATE_OVERLAY)) : 0, TVIS_OVERLAYMASK);
	GetTreeCtrl().SetItemImage(hItem, nImage, nImage);
}

/*!
 	Узлы сцен получаеют команду свенуться. Исключаю заданную параметром сцену. Узел 
	исключенной сцены делается видимым.
	Если в настройках приложения отключена галочка сворачивать сцены, то функция ничего
	не делает. 
	\param psceneEx - указатель на сцену, которую сворачивать не надо
*/
void SceneTreeView::CollapseAllScenes(qdGameScene* psceneEx)
{
	CQuestEditorApp* pmyapp = GetMyApp();
	int bSettingVal = pmyapp->GetIntSetting(SETTING_NAME(PT_COLLAPSE_SCENES));
	if (!bSettingVal) {
		return;
	}

	const qdGameSceneList& lst = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator i = lst.begin(), e = lst.end();
	CTreeCtrl & tree = GetTreeCtrl();
	for(; i != e; ++i)
	{
		HTREEITEM hScene = FindObjectItem(*i);
		if (*i != psceneEx) 
			tree.Expand(hScene, TVE_COLLAPSE);
		else
		{
			tree.Expand(hScene, TVE_EXPAND);
			tree.EnsureVisible(hScene);
		}
	}
}

/*!
 	На текущий момент просто обновляем  иконку узла объекта в дереве
	\param pno - удаленный из триггера объект
*/
void SceneTreeView::OnRemoveObjFromTrigger(qdNamedObject* pno)
{
	CTreeCtrl &tree = GetTreeCtrl();
	HTREEITEM hItem = FindObjectItem(pno);
	if (hItem) 
		SetInTrigger(tree, hItem, 
			static_cast<qdConditionalObject*>(pno)->is_in_triggers(), 
			II_NOT_ACTIVE_TRIGGER);
}

/*!
 	На текущий момент просто обновляем  иконку узла объекта в дереве
	\param pno - добавленный в триггер объект
*/
void SceneTreeView::OnAddObjToTrigger(qdNamedObject* pno)
{
	CTreeCtrl &tree = GetTreeCtrl();
	HTREEITEM hItem = FindObjectItem(pno);
	if (hItem) 
		SetInTrigger(tree, hItem, 
		static_cast<qdConditionalObject*>(pno)->is_in_triggers(), 
		II_ACTIVE_TRIGGER);
}

void SceneTreeView::OnStateDims() 
{
	CTreeCtrl &tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	ASSERT(hItem);
	if (!hItem) 
		return;

	qdGameObjectState* pstate = 
		reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hItem));
	BeginEditStateDims(pstate);
}


void SceneTreeView::OnStateHasBound() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	ASSERT(hItem);
	qdGameObjectState* pstate = 
		reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hItem));
	if (pstate->has_bound())
	{
		Vect3f old_bnd;
		if (ptree_helpers::is_obj_moving(pstate->owner()))
			old_bnd = static_cast<qdGameObjectMoving*>(pstate->owner())->bound();

		pstate->drop_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);
		if (ptree_helpers::is_obj_moving(pstate->owner()))
		{
			qdGameObjectMoving* const pobj = 
				static_cast<qdGameObjectMoving*>(pstate->owner());

			SaveScrLayoutOnBndEdt(pstate, 
				m_ptrGameDisp->get_active_scene()->get_camera(), old_bnd, 
				pobj->obj_bound());
			static_cast<qdGameObjectMoving*>(pstate->owner())->adjust_z();
		}
	}
	else
	{
		qdGameObjectAnimated* const pobj = 
			static_cast<qdGameObjectAnimated*>(pstate->owner());
		Vect3f old_bnd(pobj->obj_bound());
		pstate->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);

		if (ptree_helpers::is_obj_moving(pobj)) {
			if (pstate->bound() == qdGameObjectState::DEFAULT_BOUND) 
				pstate->set_bound(pobj->qdGameObjectAnimated::obj_bound());

			SaveScrLayoutOnBndEdt(pstate, 
				m_ptrGameDisp->get_active_scene()->get_camera(), 
				old_bnd);
			static_cast<qdGameObjectMoving*>(pobj)->adjust_z();
		}
		BeginEditStateDims(pstate);
	}
}

void SceneTreeView::OnBtnAutoBnd() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	ASSERT(hItem);

	qdGameObjectState* pstate = 
		reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hItem));

	ASSERT(pstate);

	qdGameObjectAnimated* const pao = 
		static_cast<qdGameObjectAnimated*>(pstate->owner());

	Vect3f old_bnd = pao->obj_bound();
	if (pstate->has_bound())
		old_bnd = pstate->bound();

	pstate->auto_bound();
	pstate->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);
	if (ptree_helpers::is_obj_moving(pstate->owner()))
	{
		SaveScrLayoutOnBndEdt(pstate, 
					m_ptrGameDisp->get_active_scene()->get_camera(), old_bnd);
		static_cast<qdGameObjectMoving*>(pstate->owner())->adjust_z();
	}
	BeginEditStateDims(pstate);
	SetProjChange();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

LRESULT SceneTreeView::OnEditStateDims(WPARAM wParam, LPARAM lParam)
{
	if (lParam) {
		if (wParam)
		{
			m_ptrIReqStateEdit = reinterpret_cast<IRequestingStateEdit*>(lParam);
			return BeginEditStateDims(m_ptrIReqStateEdit->GetState());
		}
		return TRUE;
	}
	return FALSE;
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
LRESULT SceneTreeView::BeginEditStateDims(qdGameObjectState* pstate)
{
	//сначала тормозим поток.
	//при выходе из функции он будет расторможен
	//если не дойдет до вызова SuspendThread
	//иначе будет все в порядке
	thread_suspender __suspend(this);

	m_ptrStateBoundDlg.reset(new StateBoundDlg);

	m_ptrStateBoundDlg->set_data(pstate);
	if (!m_ptrStateBoundDlg->Create(StateBoundDlg::IDD))
		return FALSE;

	SuspendThread();
	m_ptrSceneView->SetViewState(VS_EDIT_STATE);
	m_ptrSceneView->SetSubTools(SSTE_NONE);

	CRect r;
	GetWindowRect(&r);
	m_ptrStateBoundDlg->SetWindowPos(0, r.left, r.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_ptrStateBoundDlg->ShowWindow(SW_SHOW);

	m_ptrSceneView->OnEditStateDims(m_ptrStateBoundDlg.get(), pstate);
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
void SceneTreeView::EndEditStateDims(bool bSave)
{
	m_ptrSceneView->OnEndEditStateDims(bSave);
	m_ptrSceneView->SetViewState(VS_EDIT_SCENE);

	if (m_ptrStateBoundDlg.get()){
		m_ptrStateBoundDlg->EndEdit(bSave?
									StateBoundDlg::EEM_SAVE:
									StateBoundDlg::EEM_DISCARD);
		m_ptrStateBoundDlg->DestroyWindow();
		m_ptrStateBoundDlg.reset(0);
	}

	if (bSave)
		SetProjChange();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);

	if (m_ptrIReqStateEdit) {
		m_ptrIReqStateEdit->EndEditStateDims();
		m_ptrIReqStateEdit = NULL;
	}

	ResumeThread();
}

void SceneTreeView::OnStackState() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	ASSERT(hSelected);

	qdGameObjectState* pstate = 
		reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hSelected));
	ASSERT(pstate);
	BeginStackState(pstate);
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
LRESULT SceneTreeView::BeginStackState(qdGameObjectState* pstate)
{
	//сначала тормозим поток.
	//при выходе из функции он будет расторможен
	//если не дойдет до вызова SuspendThread
	//иначе будет все в порядке
	thread_suspender __suspend(this);

	m_ptrStateStacker.reset(new StateCoordsStacker);
	if (!m_ptrStateStacker.get()) 
		return FALSE;

	m_ptrStateStacker->set_state(pstate);
	m_ptrStateStacker->SetView(m_ptrSceneView);


	m_iOldViewState = m_ptrSceneView->GetViewState();
	m_ptrSceneView->SetViewState(VS_EDIT_STACK_STATE);
	
	if(!m_ptrStateStacker->Create(StateCoordsStacker::IDD, this))
		return FALSE;

	CRect r;
	GetWindowRect(&r);
	m_ptrStateStacker->SetWindowPos(0, r.left, r.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_ptrStateStacker->ShowWindow(SW_SHOW);

	EnableWindow(FALSE);
	SuspendThread();

	qdGameObjectAnimated* po = 
		static_cast<qdGameObjectAnimated*>(pstate->owner());
	po->set_state(pstate);
	ASSERT(po->get_cur_state() == pstate);
	
	return TRUE;				
}

LRESULT SceneTreeView::OnStackState(WPARAM wParam, LPARAM lParam)
{
	if (lParam) {
		if (wParam)
		{
			m_ptrIReqStateEdit = reinterpret_cast<IRequestingStateEdit*>(lParam);
			return BeginStackState(m_ptrIReqStateEdit->GetState());
		}
		return TRUE;
	}
	return FALSE;
}

/*!
 	Восстанавливает состояние редактора сцены в исходное. Информирует диалог стыковки
	о прекращении стыковки(StateBoundDlg::EndEdit()). . Если редактирование было запущено из 
	диалога редактирования свойств, то информирует его об окончании редактирования.
	\param bCancel - отменить/сохранить изменения
*/
void SceneTreeView::EndStateStack(bool bCancel)
{
	m_ptrSceneView->SetViewState(static_cast<VIEW_STATE>(m_iOldViewState));

	EnableWindow(TRUE);

	m_ptrStateStacker->EndEdit(bCancel?StateCoordsStacker::EEM_DISCARD:
									   StateCoordsStacker::EEM_SAVE);
	if (!bCancel)
		SetProjChange();
	m_ptrStateStacker->DestroyWindow();
	m_ptrStateStacker.reset();
	
	if (m_ptrIReqStateEdit) {
		m_ptrIReqStateEdit->EndStackState();
		m_ptrIReqStateEdit = NULL;
	}
	ResumeThread();
}

void SceneTreeView::OnCoordsAnimation() 
{
	CTreeCtrl& tree = GetTreeCtrl();

	HTREEITEM hSelected = tree.GetSelectedItem();
	ASSERT(hSelected);

	qdGameObjectState* pstate = 
		reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hSelected));
	BeginCAEdit(pstate);
}

/*!
 	Останавливает поток проигрывания анимации. Создает диалог редактирования КА и 
	увязывает его с редактором сцены. Переводит редактор сцены в режим редактирования
	КА и инициирует обновление информации о КА в редакторе сцены
	\param pstate - Состояние, чью КА мы будем редактировать
	\retval TRUE - если все в порядке
	\retval FALSE - если не удалось начать стыковку
*/
LRESULT SceneTreeView::BeginCAEdit(qdGameObjectState* pstate)
{
	//сначала тормозим поток.
	//при выходе из функции он будет расторможен
	//если не дойдет до вызова SuspendThread
	//иначе будет все в порядке
	thread_suspender __suspend(this);

	m_ptrCoordAnimDlg.reset(new CoordAnimDlg);
	if(!m_ptrCoordAnimDlg.get())
		return FALSE;

	m_ptrSceneView->SetCoordsAnimationDlg(m_ptrCoordAnimDlg.get());
	m_ptrCoordAnimDlg->set_source_ca(pstate->coords_animation());
	m_ptrCoordAnimDlg->SetView(m_ptrSceneView);
	
	if(!m_ptrCoordAnimDlg->Create(CoordAnimDlg::IDD, this))
		return FALSE;
	
	m_iOldViewState = m_ptrSceneView->GetViewState();
	
	m_ptrSceneView->SetViewState(VS_EDIT_COORD_ANIM);
	m_ptrSceneView->UpdateCoordsAnimation();
	
	CRect r;
	GetWindowRect(&r);
	m_ptrCoordAnimDlg->SetWindowPos(0, r.left, r.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_ptrCoordAnimDlg->ShowWindow(SW_SHOW);

#ifdef _DEBUG
	qdGameObjectAnimated* po = static_cast<qdGameObjectAnimated*>(pstate->owner());
	ASSERT(po->get_cur_state() == pstate);
#endif
	
	EnableWindow(FALSE);
	SuspendThread();
	return TRUE;
}

//вызывается как обработчик запроса на редактирование СА от диалога
LRESULT SceneTreeView::OnSetCoordAnimationMode(WPARAM wParam, LPARAM lParam)
{
	if(lParam)
	{
		if(wParam)
		{
			m_ptrIReqStateEdit = reinterpret_cast<IRequestingStateEdit*>(lParam);
			return BeginCAEdit(m_ptrIReqStateEdit->GetState());
		}
		return TRUE;
	}
	return FALSE;
}

/*!
 	Восстанавливает состояние редактора сцены. Информирует диалог редактирования 
	КА о прекращении редактирования(StateBoundDlg::EndEdit()). Если изменения 
	сохраняются, то обновляет изображение узла состояния
	(SceneTreeView::UpdateStateImage()). Если редактирование было запущено из 
	диалога редактирования свойств, то информирует его об окончании редактирования
	\param bCancel - отменить/сохранить изменения
	\return Всегда true
*/
bool SceneTreeView::EndEditCoordAnimationMode(bool bCancel)
{
	m_ptrSceneView->SetViewState(static_cast<VIEW_STATE>(m_iOldViewState));
	m_ptrCoordAnimDlg->DestroyWindow();
	m_ptrCoordAnimDlg->EndEdit(bCancel?CoordAnimDlg::EEM_DISCARD:
									   CoordAnimDlg::EEM_SAVE);
	if (!bCancel)
	{
		UpdateStateImage(static_cast<qdGameObjectState*>
			(m_ptrCoordAnimDlg->get_source_ca()->owner()), NULL);
		SetProjChange();
	}

	m_ptrCoordAnimDlg.reset();
	
	EnableWindow(TRUE);
	if (m_ptrIReqStateEdit)
	{
		m_ptrIReqStateEdit->EndEditCoordAnimation();
		m_ptrIReqStateEdit = NULL;
	}
	ResumeThread();
	return true;
}

/*!
 	Если идет редактирование КА, редактирование размеров или стыковка, то закрыть нельзя.
	\return Строку причины, если закрыть нельзя
			\retval "Пустая строка" - закрыть можно.
			\retval "Непустая строка" - закрыть нельзя. Строка соодержит причину
*/
CString SceneTreeView::CanClose()
{
	if(m_ptrCoordAnimDlg.get())
		return CString((LPTSTR)IDS_CNNT_CLOSE_EDT_COORD_ANIM);
	if(m_ptrStateStacker.get())
		return CString((LPTSTR)IDS_CNNT_CLOSE_STACK_STATE);
	if (m_ptrStateBoundDlg.get())
		return CString((LPTSTR)IDS_CNNT_CLOSE_EDIT_STATE_DIMS);
	return CString();
}

/*!
 	Пробегает по всем цепочкам и всем элементам в них. Если элемент содержит 
	переданный объект, то вызывает qdTriggerElement::update_object_reference()
	\param pno - Объект, для которого обновляем загловки элементов триггеров
*/
void SceneTreeView::UpdateObjTrgEleTitles(qdNamedObject* pno)
{
	const qdTriggerChainList& lst = m_ptrGameDisp->trigger_chain_list();
	typedef qdTriggerChainList::const_iterator ci_t;
	typedef qdTriggerElementList::const_iterator ctei_t;

	ci_t i = lst.begin(), e = lst.end();
	for(; i != e; ++i)
	{
		qdTriggerChain* pchain = *i;
		const qdTriggerElementList& el = pchain->elements_list();
		ctei_t res = std::find_if(el.begin(), el.end(), is_object_in_te(pno));
		if (res != el.end())
			(*res)->update_object_reference();
	}
}

/*!
 	Пробегает по всем цепочкам и всем элементам в них и вызывает 
	qdTriggerElement::update_object_reference()
*/
void SceneTreeView::UpdateAllTitles()
{
	const qdTriggerChainList& lst = m_ptrGameDisp->trigger_chain_list();
	typedef qdTriggerChainList::const_iterator ci_t;
	typedef qdTriggerElementList::const_iterator ctei_t;
	
	ci_t i = lst.begin(), e = lst.end();
	for(; i != e; ++i)
	{
		qdTriggerChain* pchain = *i;
		const qdTriggerElementList& el = pchain->elements_list();
		ctei_t beg = el.begin(), end = el.end();
		for(;beg != end; ++beg)
			(*beg)->update_object_reference();
	}
}

/*!
 	Для каждого состояния объекта вызывает (SceneTreeView::UpdateObjTrgEleTitles())
	\param pobj - объект, с состояниями которого мы работаем
*/
void SceneTreeView::UpdateTrgEleTitle4ObjStates(qdGameObjectAnimated* pobj)
{
	typedef qdGameObjectStateVector::const_iterator ci_t;
	const qdGameObjectStateVector& v = pobj->state_vector();
	ci_t i = v.begin(), e = v.end();
	for(;i != e; ++i)
	{
		UpdateObjTrgEleTitles(*i);
	}
}

void SceneTreeView::UpdateTrgEleTitle4SceneObjs(qdGameScene* pscene)
{
	const qdGameObjectList& lst = pscene->object_list();
	qdGameObjectList::const_iterator i = lst.begin(), e = lst.end();
	for(; i != e; ++i)
		if ((*i)->named_object_type() != QD_NAMED_OBJECT_STATIC_OBJ)
			UpdateTrgEleTitle4ObjStates(static_cast<qdGameObjectAnimated*>(*i));
}

BOOL AddCellSet(qdGameDispatcher* ptrDisp, qdInventory* ptrInv);

void SceneTreeView::OnCrtNewInventory() 
{
	CString strName((LPTSTR)IDS_INVENTORY_DEFAULT_NAME);
	if(AskUserForName(strName))
	{
		if (m_ptrGameDisp->inventory_cell_types().empty())
			AddInventoryCellTypes();

		if (m_ptrGameDisp->inventory_cell_types().empty())
			return;

		if (m_ptrGameDisp->inventory_list().empty()) 
		{
			std::auto_ptr<qdInventory> pi(CreateInventory(ICM_USE_EXISTING_CELL_TYPES));
			if (pi.get()&&InsertInvToGD(pi.get(), strName)){
				qdInventory* ptr = pi.release();
				InsertInventoryToTree(ptr);
				SetProjChange();
				//если создали первый инв., то назначаем его всем персонажам.
				if (m_ptrGameDisp->inventory_list().size() == 1)
					SetInvToPers(ptr->name());
			}
		}
		else
		{
			std::auto_ptr<qdInventory> pi ( CreateInventory(ICM_EMPTY));
			if (pi.get()&&InsertInvToGD(pi.get(), strName))
			{
				HTREEITEM hItem = InsertInventoryToTree(pi.get());
				if(hItem)
				{
					ShowItem(hItem);
					SetProjChange();
					AddCellSet(m_ptrGameDisp, pi.get());
					if (pi->cell_sets().empty())
						RemoveItem(hItem, false);
					else
						sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
					pi.release();
				}
				else
					m_ptrGameDisp->remove_inventory(pi.get());
			}
		}
	}
}

/*!
 	Пробегаем по всем персонажам во всех сценах и назначаем им инвентори
	(qdGameObjectMoving::set_invetory_name())
	\param name - имя инвентори
 */
void SceneTreeView::SetInvToPers(LPCTSTR name)
{
	const qdGameSceneList& l = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator i = l.begin(), e = l.end();
	for(; i != e; ++i)
	{
		const qdGameObjectList& ol = (*i)->object_list();
		qdGameObjectList::const_iterator b = ol.begin(), end = ol.end();
		while((b = std::find_if(b, end, ptree_helpers::is_obj_moving)) != end){
			static_cast<qdGameObjectMoving*>(*b)->set_inventory_name(name);
			++b;
		}
	}
}


void SceneTreeView::OnRemoveCellSet() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	ASSERT(hSelected);
	
	qdInventory* pinv = 
		reinterpret_cast<qdInventory*>(tree.GetItemData(hSelected));
	if (DeleteInventoryCellset(pinv, pinv->selected_cellset_num()))
	{
		sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	}
}

/*!
 	Пробегает по всем наборам ячеек во всех Inventory и тестирует тип набора
	\param type - тип набора ячеек
	\return \a true если указанный тип совпал с типом какого-либо набора ячеек
 */
bool SceneTreeView::IsInventoryTypeUsed(int type)
{
	typedef qdInventoryList::const_iterator ci_t;
	const qdInventoryList& l = m_ptrGameDisp->inventory_list();
	//проверили в наборах ячеек
	ci_t i = l.begin(), e = l.end();
	for(; i != e; ++i)
	{
		qdInventory* pi = *i;
		const qdInventoryCellSetVector& v = pi->cell_sets();
		typedef qdInventoryCellSetVector::const_iterator ci_t2;
		ci_t2 b = v.begin(), end = v.end();
		for(; b != end; ++b)
			if (b->cells().front().type() == type) 
				return true;
	}

	return false;
}

/*!
 	Пробегаем по все объектам всех сцен проекта и проверяем тип ячейки
	\param type - тип набора ячеек
	\return \a true если указанный тип назначен для объекта
 */
bool SceneTreeView::IsInventoryTypeUsedInObjs(int type)
{
	typedef qdGameSceneList::const_iterator ci_t;

	const qdGameSceneList& lst = m_ptrGameDisp->scene_list();
	ci_t i = lst.begin(), e = lst.end();
	for(; i != e; ++i)
	{
		typedef qdGameObjectList::const_iterator ci_t2;
		const qdGameObjectList& l = (*i)->object_list();
		ci_t2 b = l.begin(), end = l.end();
		while((b = std::find_if(b, end, ptree_helpers::is_obj_animated)) != end)
		{
			qdGameObjectAnimated* pa = 
				static_cast<qdGameObjectAnimated*>(*b);
			if (pa->inventory_type() == type)
				return true;
			++b;
		}
	}
	return false;
}

void SceneTreeView::OnTbtnEntryForDialogs() 
{
	if (m_eFilterScheme == FS_ENTRY_FOR_DIALOGS)
		m_eFilterScheme = FS_NONE;
	else
		m_eFilterScheme = FS_ENTRY_FOR_DIALOGS;

	typedef bool (*f)(const qdNamedObject* po);

	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	if (m_eFilterScheme == FS_ENTRY_FOR_DIALOGS) 
	{

		m_ptrITView->SelectByFilter(tef_dlg_state(GetSceneByItem(tree, hItem)), 
							ITriggerView::SA_SELECT, 
							ITriggerView::SM_EXCLUSIVE);
	}
	else
		m_ptrITView->SelectByFilter(tef_by_scene(GetSceneByItem(tree, hItem)), 
			ITriggerView::SA_SELECT, ITriggerView::SM_EXCLUSIVE);

	if (ptree_helpers::is_state_node(tree, hItem))
	{
		m_ptrITView->SelectByFilter(
			tef_by_obj(reinterpret_cast<qdNamedObject*>(tree.GetItemData(hItem))), 
			ITriggerView::SA_SELECT,
			ITriggerView::SM_INTERSECT);
	}
	m_ptrITView->Redraw();
}

void SceneTreeView::OnTbtnEntryForDynObj() 
{
	if (m_eFilterScheme == FS_ENTRY_FOR_DYN_OBJ)
		m_eFilterScheme = FS_NONE;
	else
		m_eFilterScheme = FS_ENTRY_FOR_DYN_OBJ;
	
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	typedef bool (*f)(const qdNamedObject* po);
	if (m_eFilterScheme == FS_ENTRY_FOR_DYN_OBJ) 
	{
	
		m_ptrITView->SelectByFilter(tef_by_scenes_objs(GetSceneByItem(tree, hItem),
			ptree_helpers::is_obj_animated), 
			ITriggerView::SA_SELECT, 
			ITriggerView::SM_EXCLUSIVE);
	}
	else
		m_ptrITView->SelectByFilter(tef_by_scene(GetSceneByItem(tree, hItem)), 
			ITriggerView::SA_SELECT, ITriggerView::SM_EXCLUSIVE);
	
	if (ptree_helpers::is_state_node(tree, hItem))
	{
		m_ptrITView->SelectByFilter(
			tef_by_obj(reinterpret_cast<qdNamedObject*>(tree.GetItemData(hItem))), 
			ITriggerView::SA_SELECT,
			ITriggerView::SM_INTERSECT);
	}
	m_ptrITView->Redraw();
}

void SceneTreeView::OnTbtnEntryForMouse() 
{
	if (m_eFilterScheme == FS_ENTRY_FOR_MOUSE)
		m_eFilterScheme = FS_NONE;
	else
		m_eFilterScheme = FS_ENTRY_FOR_MOUSE;

	typedef bool (*f)(const qdNamedObject* po);
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	if (m_eFilterScheme == FS_ENTRY_FOR_MOUSE) 

		m_ptrITView->SelectByFilter(
				tef_by_scenes_objs(NULL,ptree_helpers::is_obj_mouse), 
					ITriggerView::SA_SELECT, ITriggerView::SM_EXCLUSIVE);
	else
	{
		m_ptrITView->SelectByFilter(tef_by_scene(GetSceneByItem(tree, hItem)), 
				ITriggerView::SA_SELECT, ITriggerView::SM_EXCLUSIVE);
	}
	if (ptree_helpers::is_state_node(tree, hItem))
	{
		m_ptrITView->SelectByFilter(
			tef_by_obj(reinterpret_cast<qdNamedObject*>(tree.GetItemData(hItem))), 
			ITriggerView::SA_SELECT,
			ITriggerView::SM_INTERSECT);
	}
	m_ptrITView->Redraw();
}

void SceneTreeView::OnTbtnEntryForPers() 
{
	if (m_eFilterScheme == FS_ENTRY_FOR_PERS)
		m_eFilterScheme = FS_NONE;
	else
		m_eFilterScheme = FS_ENTRY_FOR_PERS;

	typedef bool (*f)(const qdNamedObject* po);

	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	if (m_eFilterScheme == FS_ENTRY_FOR_PERS) 
	{
	
		m_ptrITView->SelectByFilter(tef_by_scenes_objs(GetSceneByItem(tree, hItem),
			ptree_helpers::is_obj_moving), 
			ITriggerView::SA_SELECT, 
			ITriggerView::SM_EXCLUSIVE);
	}
	else
		m_ptrITView->SelectByFilter(tef_by_scene(GetSceneByItem(tree, hItem)), 
				ITriggerView::SA_SELECT, ITriggerView::SM_EXCLUSIVE);
	
	if (ptree_helpers::is_state_node(tree, hItem))
	{
		m_ptrITView->SelectByFilter(
			tef_by_obj(reinterpret_cast<qdNamedObject*>(tree.GetItemData(hItem))), 
			ITriggerView::SA_SELECT,
			ITriggerView::SM_INTERSECT);
	}
	m_ptrITView->Redraw();
}

void SceneTreeView::OnTbtnShowElement()
{
	showTriggerElement_ = !showTriggerElement_;
}

void SceneTreeView::OnUpdateTbtnEntryForDialogs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp&&!m_IsSceneViewActive);
	pCmdUI->SetCheck(m_eFilterScheme == FS_ENTRY_FOR_DIALOGS);
}

void SceneTreeView::OnUpdateTbtnEntryForDynObj(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp&&!m_IsSceneViewActive);
	pCmdUI->SetCheck(m_eFilterScheme == FS_ENTRY_FOR_DYN_OBJ);
}

void SceneTreeView::OnUpdateTbtnEntryForMouse(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp&&!m_IsSceneViewActive);
	pCmdUI->SetCheck(m_eFilterScheme == FS_ENTRY_FOR_MOUSE);
}

void SceneTreeView::OnUpdateTbtnEntryForPers(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp&&!m_IsSceneViewActive);
	pCmdUI->SetCheck(m_eFilterScheme == FS_ENTRY_FOR_PERS);
}

void SceneTreeView::OnUpdateTbtnShowElement(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_ptrGameDisp&&!m_IsSceneViewActive);
	pCmdUI->SetCheck(showTriggerElement_);
}

void SceneTreeView::OnCreateMinigame()
{
	AddMiniGame();
}

bool CO_ShowCondDlg(qdGameDispatcher* ptrDisp, qdConditionalObject* ptrGame);
void SceneTreeView::OnCOCondition()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	if (!hSelected)
	{
		PlayError();
		return;
	}
	qdConditionalObject* pco = 
				reinterpret_cast<qdConditionalObject*>(tree.GetItemData(hSelected));
	if (CO_ShowCondDlg(m_ptrGameDisp, pco))
	{
		SetProjChange();
	}
}


void SceneTreeView::OnAddCellSet() 
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	ASSERT(hSelected);

	qdInventory* pinv = 
		reinterpret_cast<qdInventory*>(tree.GetItemData(hSelected));

	AddCellSet(m_ptrGameDisp, pinv);
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void SceneTreeView::OnDisableMouse()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	qdGameObjectAnimated* const pno = 
		reinterpret_cast<qdGameObjectAnimated*>(tree.GetItemData(hItem));
	ASSERT(ptree_helpers::is_obj_animated(pno));
	if (pno->check_flag(QD_OBJ_DISABLE_MOUSE_FLAG))
		pno->drop_flag(QD_OBJ_DISABLE_MOUSE_FLAG);
	else
		pno->set_flag(QD_OBJ_DISABLE_MOUSE_FLAG);
	UpdateObjImage(pno);
}


void SceneTreeView::OnAddMusicTrack()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	if (!hSelected)
		return ;
	HTREEITEM hScene = tree.GetParentItem(hSelected);
	qdGameScene* pscene = reinterpret_cast<qdGameScene*>(tree.GetItemData(hScene));
	ASSERT(pscene);
	AddMusicTrack(pscene, hSelected);
}

void SceneTreeView::OnMusicTrackCycled()
{
	 CTreeCtrl& tree = GetTreeCtrl();
	 HTREEITEM hSelected = tree.GetSelectedItem();
	 if (!hSelected)
		 return;
	 qdMusicTrack* ptrack = 
		 reinterpret_cast<qdMusicTrack*>(tree.GetItemData(hSelected));
	 ptrack->toggle_cycle(!ptrack->is_cycled());
	 SetProjChange();
}

void SceneTreeView::OnMusicTrackPlay()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	if (!hSelected)
		return;
	qdMusicTrack* ptrack = 
		reinterpret_cast<qdMusicTrack*>(tree.GetItemData(hSelected));
	m_ptrGameDisp->play_music_track(ptrack);
}

void SceneTreeView::OnMusicTrackStop()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	if (!hSelected)
		return;
	qdMusicTrack* ptrack = 
		reinterpret_cast<qdMusicTrack*>(tree.GetItemData(hSelected));
	m_ptrGameDisp->stop_music();
}

void SceneTreeView::OnUpdateCrtStaticObj(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_hCurrScene != NULL&&m_IsSceneViewActive);
}

void SceneTreeView::OnUpdateCrtGridZone(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_hCurrScene != NULL&&m_IsSceneViewActive);
}

void SceneTreeView::OnUpdatePlayStopVideo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_IsSceneViewActive);
}

void SceneTreeView::OnUpdateStopVideo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_IsSceneViewActive);
}

void SceneTreeView::OnUpdateCrtNewInventory(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_IsSceneViewActive);
}

void SceneTreeView::OnPlayMinigame()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;
	qdMiniGame* pg = reinterpret_cast<qdMiniGame*>(tree.GetItemData(hSel));
	RunMiniGame(pg);
}

void SceneTreeView::OnMakeDirStruct()
{
	DirMakeWizardShower wiz;
	wiz.show(this);
}

void SceneTreeView::OnEditStateAnimation()
{
	thread_suspender __suspend(this);
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;

	//для состояний отличных от статических 
	//этот пункт должен быть недоступен
	qdNamedObjectBase* pob = 
		reinterpret_cast<qdNamedObjectBase*>(tree.GetItemData(hSel));
	LPCTSTR lpszFileName = AnimationFileNameExtractor::getFileName(pob);
	if (!lpszFileName) 
	{
		ShowError(*this, IDS_ERR_NO_ANIMATION_FOUND);
		return;
	}
	CWaitCursor __wait;
	m_ptrSceneView->EnableWindow(FALSE);
	::EditAnimation(*this, lpszFileName);
	m_ptrSceneView->EnableWindow(TRUE);
}

void SceneTreeView::OnRemoveEdges()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	if (!hSelected)
		return;

	CString msg((LPTSTR)IDS_REMOVE_EDGES_QUESTION);
	if (MessageBox(msg, NULL, MB_OKCANCEL) != IDOK) 
		return;

	CWaitCursor wait;
	DWORD_PTR data = tree.GetItemData(hSelected);
	std::pair<qdNamedObjectBase*, bool> res = ptree_helpers::is_any_interf_obj(data);
	if (res.second)
	{
		RemoveEdgesInInterface(res.first);
		SetProjChange();
	}
	else
	{
		qdNamedObject* pso = static_cast<qdNamedObject*>(res.first);
		switch(pso->named_object_type()) {
		case QD_NAMED_OBJECT_STATIC_OBJ:
			if (static_cast<qdGameObjectStatic*>(pso)->remove_sprite_edges())
			{
				SetProjChange();
				m_ptrGameDisp->save_script(m_ptrMainFrame->GetCurrScriptFileName());
			}
			break;
		case QD_NAMED_OBJECT_OBJ_STATE:
			{
				qdGameObjectState* pstate = static_cast<qdGameObjectState*>(pso);
				bool bShowAnim = (pstate == m_ptrPlayedState);
				if (pstate->state_type() == qdGameObjectState::STATE_STATIC)
				{
					if (bShowAnim)
						StopAnimation(pstate);
					if (RemoveImageEdges(pstate))
					{
						SetProjChange();
						m_ptrMainFrame->SafeSaveScript(m_ptrGameDisp, 
									m_ptrMainFrame->GetCurrScriptFileName());
					}
					if (bShowAnim)
						ShowStateAnimation(pstate);
				}
				else
					ASSERT((_T("Не поддерживаемый тип состояния"), 0));
			}
		break;
		}
	}
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

/*!
 	Получает владельца состояния. Устанавливает его объекту. И вызывает функцию
	обрезки(qdGameObjectAnimated::remove_animation_edges()). Если обрезка произведена
	успешно, то для всех состояний, использующих ту же анимацию, выравниваем смещения(
	SceneTreeView::StateOffsetInOtherAnimUses())
	\param pst - Состояние, для которого делаем обрезку
	\retval true	- Если обрезка состоялась
	\retval false	- Если не состоялась
*/
bool SceneTreeView::RemoveImageEdges(qdGameObjectState* pst)
{
	qdGameObjectAnimated* const pao = 
		static_cast<qdGameObjectAnimated*>(pst->ref_owner());
	pao->set_state(pst);
	Vect2i offset;
	if (pao->remove_animation_edges(Vect2i(), offset))
	{
		if (offset.x||offset.y)
			StateOffsetInOtherAnimUses(
				static_cast<qdGameObjectStateStatic*>(pst), offset);
		return true;
	}
	return false;
}

void SceneTreeView::OnToolbarDropDown(NMHDR* pNMHDR, LRESULT *plr)
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
								rc.left, rc.bottom, this, &rc);
	//Отцепляем, чтобы не разрушилось при выходе
	// и в OnMenuCommand пришла хорошая ссылка
	menu.Detach();
}

void SceneTreeView::OnMenuCommand(WPARAM wParam, LPARAM lParam)
{
	HMENU hmenu = reinterpret_cast<HMENU>(lParam);
	if (!hmenu)
		return;
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
}

/*!
 	Достаем из узла, на который кликнули цепочку и показыаем её в редакторе
	триггеров. Показыает редактор триггеров. Устанавливает правильный цвет для 
	буквы "Т" рядом с узлами объектов, участвующий в триггере
	\param hm	- Описатель меню триггеров
	\param pos	- Позиция кликнутого пункта меню 
*/
void SceneTreeView::HandleEditTriggersMenu(HMENU hm, DWORD pos)
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
		if (m_IsSceneViewActive) 
			m_IsSceneViewActive = false;
		if(m_ptrITView->ShowTriggers(&pchain, 1))
		{
			SetTgrIcon4All(TRUE);

			//если выделенный элемент в дереве есть триггер или "Триггеры",
			//то переносим выделение на показываемый триггер
			CTreeCtrl& tree = GetTreeCtrl();
			HTREEITEM hSelected = tree.GetSelectedItem();
			if (!hSelected||ptree_helpers::is_trigger_item(tree, hSelected)
				||ptree_helpers::is_container_type(tree, 
							hSelected, QD_NAMED_OBJECT_TRIGGER_CHAIN))
			{
				ShowObjectInTree(pchain);
			}
		}
	}
}

BOOL SceneTreeView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (message == WM_MENUCOMMAND) 	
		OnMenuCommand(wParam, lParam);
	return CTreeView::OnWndMsg(message, wParam, lParam, pResult);
}

/*!
	Пробегаем по всем цепочкам, и добавляем пункты с именем цепочки.Укзатели на 
	цепочки кладем в item data
	\param pm - меню, которое будем заполнять
*/
void SceneTreeView::FillMenuForTriggers(CMenu*pm)
{
	typedef qdTriggerChainList::const_iterator ci_t;
	const qdTriggerChainList& tcl = m_ptrGameDisp->trigger_chain_list();

	if (tcl.empty())
		return;

	MENUITEMINFO mii;
	memset(&mii, 0, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_DATA;

	qdTriggerChain* pc_showed = m_ptrITView->ShowedTrigger();

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

/*!
 	Проверяет какой файл используется в анимации. Если имя файла совпадает с
	test_anim_file_name::strFileName_, то кладет имя анимации в список 
	test_anim_file_name::vec_/
*/
class test_anim_file_name
{
	std::vector<LPCTSTR>& vec_;
	CString strFileName_;
public:
	test_anim_file_name(const CString& strFileName,
		std::vector<LPCTSTR>& vec):vec_(vec), strFileName_(strFileName){}
	void operator()(const qdAnimation* p){
		if (!strFileName_.CompareNoCase(p->qda_file()))
			vec_.push_back(p->name());
	}
};

/*!
	Для объектф находит состояния , которые используют анимацию из 
	offset_states::anims_ и испраляет имя смещение
*/
class  offset_states
{
	//! Состояние, которое смещать нельзя
	qdGameObjectStateStatic * const pst_ex_;
	//! Смещение, на которое будут смещены остальные состояния
	Vect2i const&  offs_;
	//! \brief Состояния объектов, которые используют анимацию из этого
	//! списка, подлежат смещению
	std::vector<LPCTSTR> const& anims_;
	//! анимация в pst_ex_ отражена горизонтально
	bool bFlipHorz_;
	//! анимация в pst_ex_ отражена вертикально
	bool bFlipVert_;
public:
	offset_states(qdGameObjectStateStatic* pst_exclude, Vect2i const& offs,
		std::vector<LPCTSTR> const& anims):
		pst_ex_(pst_exclude)
		, offs_(offs)
		, anims_(anims)
		{
			bFlipHorz_ = pst_ex_->animation_info()->
				check_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL);
			bFlipVert_ = pst_ex_->animation_info()->
				check_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL);
		}

	void operator()(qdNamedObject* pobj)
	{
		qdGameObjectAnimated* const panim = 
			static_cast<qdGameObjectAnimated*>(pobj);
		const qdGameObjectStateVector& v = panim->state_vector();

		//для всех статических состояний проверяем список анимаций
		qdGameObjectStateVector::const_iterator ix = v.begin(),ex = v.end();
		while((ix = std::find_if(ix, ex, ptree_helpers::is_state_static)) != ex)
		{
			qdGameObjectStateStatic* const pst =
				static_cast<qdGameObjectStateStatic*>(*ix);
			if (pst != pst_ex_)
			{
				std::vector<LPCTSTR>::const_iterator itr = anims_.begin(),
													 end = anims_.end();
				for(; itr != end; ++itr)
				{
					LPCTSTR const name = pst->animation_info()->name();
					if (name&&!_tcscmp(name, *itr))
					{
						//! Имя анимации находится в списке. Будем смещать.
						Vect2i offs(offs_);
						if (pst->animation_info()->
							check_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL) != bFlipHorz_)
								offs.x = -offs.x;

						if (pst->animation_info()->
							check_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL) != bFlipVert_)
								offs.y = -offs.y;

						qdAnimation* pa = pst->animation_info()->animation();
						if (pa) 
							pa->load_resources();//! Потому что файл анимации был изменен
						pst->set_center_offset(pst->center_offset() + offs);
						break;
					}
				}
			}
			++ix;
		}
	}
};

/*!
	\param strFileName - Имя файла анимации
	\param anim_idx	   - [Выходной парметер] индекс списков анимаций, опирающихся на
						\a strFileName
*/
void SceneTreeView::FindAllAnimation(const CString& strFileName, AnimIndex_t& anim_idx)
{
	std::for_each(m_ptrGameDisp->animation_list().begin(), 
		m_ptrGameDisp->animation_list().end(), 
		test_anim_file_name(strFileName, anim_idx[m_ptrGameDisp]));

	const qdGameSceneList& lst = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator ix = lst.begin(), ex = lst.end();
	for(;ix != ex; ++ix)
	{
		std::for_each((*ix)->animation_list().begin(), 
			(*ix)->animation_list().end(), 
			test_anim_file_name(strFileName, anim_idx[*ix]));
	}
}

/*!
 	Для всех динамических объектов и персонажей в сцене вызывается функтор 
	offset_states.
	\param query_src	- Исходное состояние, которое инициировало смещение всех
	\param offset		- Смещение, на которое надо сместить состояния
	\param pscene		- Сцена, с объектами которой работаем
	\param anim_lst		- Состояния объектов, которые используют анимацию из этого
						  списка, подлежат смещению
*/
void SceneTreeView::StateOffsetInOtherAnimUses4Scene(qdGameObjectStateStatic* query_src, 
									  Vect2i const& offset, qdGameScene* pscene,
									  std::vector<LPCTSTR> const& anim_lst)
{
	const qdGameObjectList& lst = pscene->object_list();

	typedef boost::filter_iterator<
		std::pointer_to_unary_function<const qdNamedObject*, bool>,
		qdGameObjectList::const_iterator> FilterIter;

	FilterIter begin = 
		boost::make_filter_iterator(std::ptr_fun(ptree_helpers::is_animated),
							lst.begin(), lst.end());
	FilterIter end = 
		boost::make_filter_iterator(std::ptr_fun(ptree_helpers::is_animated),
		lst.end(), lst.end());
	//для всех анимированных и персонажей
	std::for_each(begin, end, offset_states(query_src, offset, anim_lst));
}

/*!
	Для каталогов анимации всех сцен и глобального диспетчера формируются индекс списков
	анимаций, которые опираются на тот же файл, что и исходная(
	SceneTreeView::FindAllAnimation()). Затем перобегает по этому индексу и 
	если список сформирован для сцены вызывает 
	SceneTreeView::StateOffsetInOtherAnimUses4Scene().
	Если список был сформирован для глобального каталога(дальше глобальный список), то:
	 - пробегает по списку сцен.
	 - для каждой сцены из глобального списка формируется список анимаций, имя которых
		не повторяется в каталоге анимации сцены.
	 - Для сцены вызывает SceneTreeView::StateOffsetInOtherAnimUses4Scene() и передает
	  туда сформированный для сцены список
	\param query_src	- Исходное состояние. Это состояние уже было смещено и его 
							трогать нельзя
	\param offset		-  Смещение на которое надо сместить другие состояния
*/
void SceneTreeView::StateOffsetInOtherAnimUses(qdGameObjectStateStatic* query_src,
											   Vect2i const& offset)
{
	qdGameScene* query_src_scene = static_cast<qdGameScene*>(query_src->owner());
	CString strAnimFileName(query_src->animation()->qda_file());
	AnimIndex_t anim_indx;
	FindAllAnimation(strAnimFileName, anim_indx);
	
	AnimIndex_t::const_iterator ix = anim_indx.begin(), ex = anim_indx.end();
	for(;ix != ex; ++ix)
	{
		qdNamedObject* pdisp = ix->first;
		if (pdisp->named_object_type() == QD_NAMED_OBJECT_SCENE)
		{
			if (!ix->second.empty())
				StateOffsetInOtherAnimUses4Scene(query_src, offset, 
					static_cast<qdGameScene*>(pdisp), ix->second);
		}
		else //в данном случае это будет глобальный диспетчер
		{
			const qdGameSceneList& lst = m_ptrGameDisp->scene_list();
			qdGameSceneList::const_iterator itr = lst.begin(),
											end = lst.end();
			std::vector<LPCTSTR> vec;
			for(; itr != end; ++itr)
			{
				qdGameScene* const pscene = *itr;
				std::vector<LPCTSTR>::const_iterator j = ix->second.begin(),
													 e = ix->second.end();
				//если в сцене есть анимация с таким именем,
				//то пропускаем эту сцену
				for(; j != e; ++j)
					if (!pscene->get_animation(*j))
						vec.push_back(*j);
				if (!vec.empty())
				{
					StateOffsetInOtherAnimUses4Scene(query_src, offset, pscene, vec);
					vec.clear();
				}
			}
		}
	}
}

/*!
 	Смещает центр объекта и другим состояниям добавляется смещение, чтобы их анимация
	визуально оставалась на том же месте, как и до обрезки.
	Получает экранные координаты, соответствующие текущему положению объекта. Добавляем
	к ним смещение, и переводим экранные координаты обратно в глобальные. Устанавливает
	объекту новые глобальные координаты.
	Состоянию \a pLeadingState устанавливает нулевое смещение. Из смещений всех остальных 
	состояний вычитается \a off

	\param pobj				- Объект, центр которого будем сдвигать
	\param pLeadingState	- Состяние, к центру анимации которого смещается центр 
								объекта.
	\param off				- смещение
	\param pcam				- Камера из сцены, которой принадлежит объект для проведение 
								преобразований
*/
void OffsetObjectCenter(qdGameObjectAnimated* pobj, 
						qdGameObjectState* pLeadingState,
						const Vect2i& off, qdCamera* pcam)
{
	//сдвинули объект для начала
	//в z храниться глубина
	if (off.x||off.y) 
	{
		ASSERT(pLeadingState->coords_animation()->is_empty());
		Vect3f vpos = pcam->global2camera_coord(pobj->R());
		Vect2s scr_pos = pcam->camera_coord2scr(vpos);
		scr_pos += off;
		scr_pos = pcam->scr2rscr(scr_pos);
		pobj->set_pos(pcam->rscr2global(scr_pos, vpos.z));

		pobj->set_default_pos(pobj->R());

		pLeadingState->set_center_offset(Vect2s(0, 0));

		//сдвигаем остальные состояния
		const qdGameObjectStateVector& v = pobj->state_vector();
		qdGameObjectStateVector::const_iterator _itr = v.begin(), end = v.end();
		while ((_itr = std::find_if(_itr, end, ptree_helpers::is_state_static)) != end) {
			qdGameObjectState* pstate = *_itr;
			if (pstate != pLeadingState&&pstate->coords_animation()->is_empty()) 
				pstate->set_center_offset(pstate->center_offset()-off);
			++_itr;
		}
	}
}

/*!
	Пробегает по всем состояниям. Для статических делает обрезку.Если обрезка произведена
	успешно, то для всех состояний, использующих ту же анимацию, выравниваем смещения(
	SceneTreeView::StateOffsetInOtherAnimUses()). Первое обрезанное состояние 
	запоминается. Также запоминается, насколько сместилось изображение при обрезке.
	Если было хоть что-то обрезано и объект не имеет походочных состояний, смещает
	центр объекта в центр анимации первого обрезанного состояния
	\param pobj - объект, над состояниями которого работаем
	\retval true - всегда
*/
bool SceneTreeView::RemoveEdgesForObj(qdGameObjectAnimated* pobj)
{
	const qdGameObjectStateVector& v = pobj->state_vector();
	Vect2i offset(0, 0);//хранит смещение первого годного к обрезке состояния
	Vect2i offset2;//рабочая для цикла
	Vect2i offset3;//рабочая для цикла
	bool bFirstRemove = true;
	qdGameObjectState* pLeadingState = NULL;
	qdGameObjectStateVector::const_iterator _itr = v.begin(), 
											end = v.end();
	while ((_itr = std::find_if(_itr, end, ptree_helpers::is_state_static)) != end) {
		qdGameObjectState* const pstate = *_itr;
		pobj->set_state(pstate);
		if (pobj->remove_animation_edges(offset2, offset3))
		{
			if (offset3.x||offset3.y)
				StateOffsetInOtherAnimUses(
					static_cast<qdGameObjectStateStatic*>(pstate),offset3);
			if (bFirstRemove&&pstate->coords_animation()->is_empty())
			{
				bFirstRemove = false;
				offset = offset2;
				pLeadingState = pstate;
			}
			SetProjChange();
		}
		++_itr;
	}
	pobj->set_default_state();
	if (pLeadingState&&!ptree_helpers::HasWalkState(pobj)) {
		OffsetObjectCenter(pobj, pLeadingState, offset,
			static_cast<qdGameScene*>(pobj->owner())->get_camera());
		pobj->set_default_state();
	}
	return true;
}

void SceneTreeView::OnRemoveEdgesInStates()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;

	CString msg((LPTSTR)IDS_REMOVE_EDGES_QUESTION);
	if (MessageBox(msg, NULL, MB_OKCANCEL) != IDOK) 
		return;

	CWaitCursor __wait__;
	qdGameObjectAnimated* pobj = 
		reinterpret_cast<qdGameObjectAnimated*>(tree.GetItemData(hSel));

	RemoveEdgesForObj(pobj);
	m_ptrGameDisp->save_script(m_ptrMainFrame->GetCurrScriptFileName());

	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void SceneTreeView::OnRemoveEdgesForAllObjs()
{
	CString msg((LPTSTR)IDS_REMOVE_EDGES_QUESTION);
	if (MessageBox(msg, NULL, MB_OKCANCEL) != IDOK) 
		return;

	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;

	CWaitCursor __wait;
	qdGameScene* pscene = reinterpret_cast<qdGameScene*>(tree.GetItemData(hSel));

	boost::rtl::crange<const qdGameObjectList> r(pscene->object_list());
	for(; r; ++r)
	{
		if (dynamic_cast<qdGameObjectAnimated*>(*r))
			RemoveEdgesForObj(static_cast<qdGameObjectAnimated*>(*r));
		else
			static_cast<qdGameObjectStatic*>(*r)->remove_sprite_edges();
	}
	m_ptrGameDisp->save_script(m_ptrMainFrame->GetCurrScriptFileName());
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	SetProjChange();
}

void SceneTreeView::OnBindObjCenter()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;

	CWaitCursor __wait;
	qdGameObjectState* pstate = 
		reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hSel));
	StopAnimation(pstate);
	qdGameObjectAnimated* pobj = static_cast<qdGameObjectAnimated*>(pstate->owner());
	OffsetObjectCenter(pobj, pstate, pstate->center_offset(),
		static_cast<qdGameScene*>(pobj->owner())->get_camera());

	ShowStateAnimation(pstate);
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	SetProjChange();
}

//void SceneTreeView::RenameObjectInConds(qdNamedObject* pno, LPCTSTR lpszOldName)
//{
//	switch(pno->named_object_type()) {
//	case QD_NAMED_OBJECT_OBJ_STATE:
//		RenameStateInConds(static_cast<qdGameObjectState*>(pno), lpszOldName);
//		break;
//	case QD_NAMED_OBJECT_ANIMATED_OBJ:
//	case QD_NAMED_OBJECT_MOVING_OBJ:
//		RenameAnimatedObjInConds(static_cast<qdGameObjectAnimated*>(pno), lpszOldName);
//		break;
//	case QD_NAMED_OBJECT_GRID_ZONE:
//		RenameGridZoneInConds(static_cast<qdGridZone*>(pno), lpszOldName);
//		break;
//	case QD_NAMED_OBJECT_MINIGAME:
//		RenameMiniGameInConds(static_cast<qdMiniGame*>(pno), lpszOldName);
//	default:;
//	}
//}
//
//template <class _CondPred, class _CondAction>
//void SceneTreeView::ChangeAllCondsIF(_CondPred& pred, _CondAction& action)
//{
//	const qdGameObjectList& lstGOs = m_ptrGameDisp->global_object_list();
//	std::for_each(lstGOs.begin(), lstGOs.end(), 
//		rename_h::enum_states<_CondPred, _CondAction>(pred, action));
//	qdGameLocation* pLoc = GetFirstLocation();
//	ASSERT(pLoc);
//	const qdGameSceneList& lstGSs = pLoc->scene_list();
//	std::for_each(lstGSs.begin(), lstGSs.end(), 
//		rename_h::enum_objects<_CondPred, _CondAction>(pred, action));
//
//	const qdMiniGameList& lstMG = m_ptrGameDisp->minigame_list();
//	std::for_each(lstMG.begin(), lstMG.end(), 
//		rename_h::enum_conds<_CondPred, _CondAction>(pred, action));
//}
//
//
//void SceneTreeView::RenameStateInConds(const qdGameObjectState* pState, 
//															 LPCTSTR lpszOldName)
//{
////	ChangeAllCondsIF(rename_h::state_cond_pred(pState, lpszOldName),
////									 rename_h::state_cond_action(pState));
//}
//
//void SceneTreeView::RenameAnimatedObjInConds(const qdGameObjectAnimated* pObj, 
//																	LPCTSTR lpszOldName)
//{
////	ChangeAllCondsIF(anim_obj_cond_pred(pObj, lpszOldName),
//	//	anim_obj_cond_action(pObj));
//}
//void SceneTreeView::RenameMiniGameInConds(const qdMiniGame* pMG, LPCTSTR lpszOldName)
//{
//
//}
//void SceneTreeView::RenameGridZoneInConds(const qdGridZone* pZone, LPCTSTR lpszOldName)
//{
//
//}

void SelectStateItemBack(qdGameObjectState* pstate, LPNMTVCUSTOMDRAW pNMCD)
{
	if (pstate->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HIDE_OBJECT)) 
		pNMCD->clrTextBk = DEFAULT_VALUE(STATE_FLAG_HIDE_OBJECT_COLOR);
	else if (pstate->has_conditions()) 
	{
		const qdCondition* pc = pstate->get_condition();
		if (pc->is_inversed())
		{
			pNMCD->clrTextBk = DEFAULT_VALUE(INVERSE_CONDITION_COLOR);
			return;
		}

		switch(pc->type()) {
		//! клик мышью по объекту
		case qdCondition::CONDITION_MOUSE_CLICK:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_MOUSE_CLICK_COLOR);
			break;
		//! клик объектом на мыши по объекту
		case qdCondition::CONDITION_MOUSE_OBJECT_CLICK:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_MOUSE_OBJECT_CLICK_COLOR);
			break;
		//! персонаж или объект в определенном месте сетки
		case qdCondition::CONDITION_OBJECT_IN_ZONE:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_OBJECT_IN_ZONE_COLOR);
			break;
		//! персонаж движется в определенном направлении
		case qdCondition::CONDITION_PERSONAGE_WALK_DIRECTION:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_PERSONAGE_WALK_DIRECTION_COLOR);
			break;
		//! персонаж стоит и ориентирован в определенном направлении
		case qdCondition::CONDITION_PERSONAGE_STATIC_DIRECTION:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_PERSONAGE_STATIC_DIRECTION_COLOR);
			break;
		//! условие выпоняется с некоторой периодичностью
		case qdCondition::CONDITION_TIMER:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_TIMER_COLOR);
			break;
		//! клик мышью по фразе в диалоге
		case qdCondition::CONDITION_MOUSE_DIALOG_CLICK:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_MOUSE_DIALOG_CLICK_COLOR);
			break;
		//! мини-игра находится в определенном состоянии
		case qdCondition::CONDITION_MINIGAME_STATE:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_MINIGAME_IN_STATE_COLOR);
			break;
		//! объект находится в определенном состоянии
		case qdCondition::CONDITION_OBJECT_STATE:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_OBJECT_STATE_COLOR);
			break;
		//! клик мышью по зоне на сетке сцены
		case qdCondition::CONDITION_MOUSE_ZONE_CLICK:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_MOUSE_ZONE_CLICK_COLOR);
			break;
		case qdCondition::CONDITION_MOUSE_OBJECT_ZONE_CLICK:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_MOUSE_OBJECT_ZONE_CLICK_COLOR);
			break;
		case qdCondition::CONDITION_OBJECT_STATE_WAS_ACTIVATED:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_OBJECT_STATE_WAS_ACTIVATED_COLOR);
			break;
		case qdCondition::CONDITION_OBJECTS_DISTANCE:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_OBJECTS_DISTANCE_COLOR);
			break;
		case qdCondition::CONDITION_PERSONAGE_ACTIVE:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_PERSONAGE_ACTIVE_COLOR);
			break;
		case qdCondition::CONDITION_OBJECT_STATE_WAITING:
			pNMCD->clrTextBk = DEFAULT_VALUE(COND_OBJECT_STATE_WAITING_COLOR);
			break;
		}
	}

}

void SelectItemBack(CTreeCtrl& tree, LPNMTVCUSTOMDRAW pNMCD)
{
	HTREEITEM hItemSpec = reinterpret_cast<HTREEITEM>(pNMCD->nmcd.dwItemSpec);
#ifdef _DEBUG
	CString text = tree.GetItemText(hItemSpec);
#endif
	if (ptree_helpers::is_scene(tree, hItemSpec)) {
//		pNMCD->clrText = RGB(255, 0, 0);
		pNMCD->clrText = RGB(9, 115, 7);
	}
	else if (ptree_helpers::is_interface_root(tree, hItemSpec))
		pNMCD->clrText = RGB(255, 0, 0);
	else if (ptree_helpers::is_container_type(tree, hItemSpec, QD_NAMED_OBJECT_VIDEO)) 
		pNMCD->clrText = RGB(255, 0, 0);
	else if (ptree_helpers::is_container_type(tree, hItemSpec, QD_NAMED_OBJECT_TRIGGER_CHAIN)) 
		pNMCD->clrText = RGB(255, 0, 0);
	else if (ptree_helpers::is_container_type(tree, hItemSpec, QD_NAMED_OBJECT_INVENTORY)) 
		pNMCD->clrText = RGB(255, 0, 0);
	else if (ptree_helpers::is_container_type(tree, hItemSpec, QD_NAMED_OBJECT_MINIGAME)) 
		pNMCD->clrText = RGB(255, 0, 0);
	else if (ptree_helpers::is_node_type(tree, hItemSpec, QD_NAMED_OBJECT_MOUSE_OBJ)) 
		pNMCD->clrText = RGB(255, 0, 0);
	else if (ptree_helpers::is_node_type(tree, hItemSpec, QD_NAMED_OBJECT_DISPATCHER)) 
		pNMCD->clrText = RGB(255, 0, 0);
	else if (ptree_helpers::is_container_type(tree, hItemSpec, QD_NAMED_OBJECT_MOVING_OBJ)
		//только для узла глобальных персонажей
		&&!tree.GetParentItem(tree.GetParentItem(hItemSpec))) 
		pNMCD->clrText = RGB(255, 0, 0);
	else if (ptree_helpers::is_container_type(tree, hItemSpec, QD_NAMED_OBJECT_MUSIC_TRACK)) 
		pNMCD->clrText = RGB(0, 0, 255);
	else if (ptree_helpers::is_net(tree, hItemSpec)) 
		pNMCD->clrText = RGB(0, 0, 255);
	else if (ptree_helpers::is_node_type(tree, hItemSpec, QD_NAMED_OBJECT_MOVING_OBJ)) 
		pNMCD->clrText = RGB(0, 0, 255);
	else if (ptree_helpers::is_state_node(tree, hItemSpec)) 
	{
		qdGameObjectState* pstate = 
			reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hItemSpec));
		SelectStateItemBack(pstate, pNMCD);
	}
	if (hItemSpec == tree.GetSelectedItem()&&GetFocus() == tree.m_hWnd)
		pNMCD->clrText = RGB(255, 255, 255);
}

void SceneTreeView::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
	
	*pResult = 0;
	switch(pNMCD->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
		}
		break;
	case CDDS_ITEMPREPAINT:
		SelectItemBack(GetTreeCtrl(), pNMCD);
		break;
	} 
}

void SceneTreeView::OnShowAllNetProjs()
{
	m_ptrSceneView->SetDrawNetProjs(!m_ptrSceneView->GetDrawNetProjs());
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void SceneTreeView::OnUpdateShowAllNetProjs(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_ptrSceneView->GetDrawNetProjs());
	pCmdUI->Enable(m_ptrGameDisp != NULL);
}

void SceneTreeView::OnDrawAllBounds()
{
	m_ptrSceneView->SetDrawBounds(!m_ptrSceneView->GetDrawBounds());
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void SceneTreeView::OnUpdateDrawAllBounds(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_ptrSceneView->GetDrawBounds());
	pCmdUI->Enable(m_ptrGameDisp != NULL);
}

void SceneTreeView::OnCreateIscreen()
{
	HTREEITEM hSel = GetTreeCtrl().GetSelectedItem();
	if (!hSel)
		return;
	CString str((LPTSTR)IDS_NEW_INTEF_SCREEN_NAME);
	if (AskUserForName(str))
	{
		qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
		str = get_unique_name(str, 
			pd->screen_list());
		if (qdInterfaceScreen* pscr = AddInterfScr(hSel, str))
		{
			pd->select_screen(pscr->name());
			SetProjChange();
			AddIntrfBackgrndWizardShow wiz;
			if (wiz.show(this, pscr))
				sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
		}
	}
}


void SceneTreeView::OnAddInterfBackground()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;
	qdInterfaceScreen* ps = 
		reinterpret_cast<qdInterfaceScreen*>(tree.GetItemData(hSel));
	AddIntrfBackgrndWizardShow wiz;
	if (wiz.show(this, ps))
		SetProjChange();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void SceneTreeView::OnAddInterfSlider()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;
	qdInterfaceScreen* ps = 
		reinterpret_cast<qdInterfaceScreen*>(tree.GetItemData(hSel));

	AddInterfSldrWizardShow wiz;
	if (wiz.show(this, ps))
		SetProjChange();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void SceneTreeView::OnAddInterfTextWindow()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;
	qdInterfaceScreen* ps = 
		reinterpret_cast<qdInterfaceScreen*>(tree.GetItemData(hSel));
	
	CString name = AskUserForName(CString((LPTSTR)IDS_DEFAULT_TEXT_WINDOW_NAME));
	if (name.IsEmpty())
		return;

	if (!AddInterfTextWindow(ps, hSel,name))
		return;
	SetProjChange();

//	AddInterfSldrWizardShow wiz;
//	if (wiz.show(this, ps))
//		SetProjChange();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void SceneTreeView::OnAddIntrfButton()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;
	qdInterfaceScreen* ps = 
		reinterpret_cast<qdInterfaceScreen*>(tree.GetItemData(hSel));

	AddInterfBtnWizShow wiz;
	if (wiz.show(this, ps))
		SetProjChange();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void SceneTreeView::OnAddInterfBtnState()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;
	qdInterfaceButton* ps = 
		reinterpret_cast<qdInterfaceButton*>(tree.GetItemData(hSel));

	AddInterfBtnStateWizShow wiz;
	if (wiz.show(this, ps))
		SetProjChange();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void SceneTreeView::OnAddInterfSave()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;
	qdInterfaceScreen* ps = 
		reinterpret_cast<qdInterfaceScreen*>(tree.GetItemData(hSel));

	AddInterfSaveWizShow wiz;
	if (wiz.show(this, ps))
		SetProjChange();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void SceneTreeView::OnUpdateTbtnScaleMinus(CCmdUI *pCmdUI)
{
	BOOL bEnabl = TRUE;
	if (m_IsSceneViewActive)
		bEnabl = (m_hCurrScene!=NULL);
	else
		bEnabl = (m_ptrITView->ShowedTrigger()!=NULL);

	pCmdUI->Enable(bEnabl);
}

void SceneTreeView::OnUpdateTbtnScalePlus(CCmdUI *pCmdUI)
{
	BOOL bEnabl = TRUE;
	if (m_IsSceneViewActive)
		bEnabl = (m_hCurrScene!=NULL);
	else
		bEnabl = (m_ptrITView->ShowedTrigger()!=NULL);

	pCmdUI->Enable(bEnabl);
}

void SceneTreeView::OnUpdateAddVideo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ptrGameDisp?TRUE:FALSE);
}

void SceneTreeView::OnRunAnimEditor()
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

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;

	CString strCommandLine = _T("\"");
	strCommandLine += app_path;
	strCommandLine += _T("\"");

	if(CreateProcess(app_path,
		const_cast<LPTSTR>((LPCTSTR)strCommandLine), NULL, NULL, FALSE,
		0, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
		ShowError(*this, IDS_CNT_LOAD_ANIMATION_EDITOR);
}

/*!
 	Проверяет можно ли объединить триггера(TriggerJoinerCondsChecker::doCheck())
	Если можно, то объеденяем триггера (ITriggerView::MergeTriggers())
	\retval TRUE - если триггера объеденены
*/
BOOL SceneTreeView::MergeTriggers(qdTriggerChain const* from, qdTriggerChain* to)
{
	TriggerJoinerCondsChecker check(m_ptrITView, static_cast<IViewMgr*>(this));
	if (!check.doCheck(from, to))
		return FALSE;
	return m_ptrITView->MergeTriggers(from, to);
}

/*!
 	\return TRUE - если удалось активизировать
*/
BOOL SceneTreeView::ShowSceneEditor()
{
	m_IsSceneViewActive = true;
	if (m_IsSceneViewActive)
		return TRUE;
	return FALSE;
}

/*!
	\return TRUE - если удалось активизировать
*/
BOOL SceneTreeView::ShowTriggerEditor()
{
	m_IsSceneViewActive = false;
	if (!m_IsSceneViewActive)
		return TRUE;
	return FALSE;
}

/*!
 	Определяет тип объекта и вызывает соответствующую функцию обрезки
	\param pobj - обрезаемый интерфейсный объект
*/
void SceneTreeView::RemoveEdgesInInterface(qdNamedObjectBase* pobj)
{
	type_info const& ti = typeid(*pobj);
	if (ti == typeid(qdInterfaceBackground))
		RemoveEdgesInInterfaceBackground(static_cast<qdInterfaceBackground*>(pobj));
	else if (ti == typeid(qdInterfaceElementState))
		RemoveEdgesInInterfaceElementState(static_cast<qdInterfaceElementState*>(pobj));
	else if (ti == typeid(qdInterfaceSave))
		RemoveEdgesInInterfaceSave(static_cast<qdInterfaceSave*>(pobj));
	else if (ti == typeid(qdInterfaceSlider))
		RemoveEdgesInInterfaceSlider(static_cast<qdInterfaceSlider*>(pobj));
	else if (ti == typeid(qdInterfaceButton))
		RemoveEdgesInInterfaceButton(static_cast<qdInterfaceButton*>(pobj));
	else if (ti == typeid(qdInterfaceScreen))
		RemoveEdgesInInterfaceScreen(static_cast<qdInterfaceScreen*>(pobj));
}

/*!
 	Достает из объекта имя файла анимации и вызывает EdgeRemover::remove_edges для
	обрезки. Смещает положение объекта на расстояние, на которое сместилась анимация
	после обрезки. Реинициализирует объект.
*/
void SceneTreeView::RemoveEdgesInInterfaceBackground(qdInterfaceBackground* pobj)
{
	Vect2i const res = EdgeRemover::remove_edges(pobj->animation_file());
	pobj->set_r(pobj->r() + res);
	pobj->init();
}

/*!
 	Достает из состояния имя файлы анимации для всех режимов и вызывает 
	EdgeRemover::remove_edges дляобрезки. 
*/
void SceneTreeView::RemoveEdgesInInterfaceElementState(qdInterfaceElementState* pobj)
{
	EdgeRemover::remove_edges(pobj->animation_file());
	EdgeRemover::remove_edges(pobj->animation_file(qdInterfaceElementState::MOUSE_HOVER_MODE));
	EdgeRemover::remove_edges(pobj->animation_file(qdInterfaceElementState::EVENT_MODE));
}

/*!
 	Достает из объекта имя файла анимации и вызывает EdgeRemover::remove_edges для
	обрезки. Реинициализирует объект.
*/
void SceneTreeView::RemoveEdgesInInterfaceSave(qdInterfaceSave* pobj)
{
	EdgeRemover::remove_edges(pobj->frame_animation_file());
	pobj->init();
}

/*!
 	Достает из объекта имя файла анимации и вызывает EdgeRemover::remove_edges для
	обрезки. Реинициализирует объект.
*/
void SceneTreeView::RemoveEdgesInInterfaceSlider(qdInterfaceSlider* pobj)
{
	EdgeRemover::remove_edges(pobj->background()->animation_file());
	EdgeRemover::remove_edges(pobj->slider()->animation_file());
	pobj->init();
}

/*!
 	Пробегает по всем элементам и для каждого вызывает 
	SceneTreeView::RemoveEdgesInInterface. Реинициализирует объект.
*/
void SceneTreeView::RemoveEdgesInInterfaceScreen(qdInterfaceScreen* pobj)
{
	qdInterfaceScreen::element_list_t const& lst = pobj->element_list();
	std::for_each(lst.begin(), lst.end(), 
		std::bind1st(std::mem_fun(SceneTreeView::RemoveEdgesInInterface), this));
	pobj->init();
}
/*!
 	Пробегает по всем состояниям кнопки и вызыавет 
	SceneTreeView::RemoveEdgesInInterfaceElementState(). Реинициализирует объект.
*/
void SceneTreeView::RemoveEdgesInInterfaceButton(qdInterfaceButton* pobj)
{
	int const num = pobj->num_states();
	for(int i = 0; i < num; ++i)
		RemoveEdgesInInterfaceElementState(pobj->get_state(i));
	pobj->init();
}

void SceneTreeView::OnConvertToQda()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	if (!hItem)
		return;

	CWaitCursor _wait_;
	DWORD_PTR data = tree.GetItemData(hItem);
	std::pair<qdNamedObjectBase*, bool> res = ptree_helpers::is_any_interf_obj(data);
	if (res.second)
	{
		ConvertTGAtoQDAInInterface(res.first);
		SetProjChange();
	}
}

/*!
 	Передает управление ConvertTGAtoQDAInInterface::convert(), который выполняет 
	всю работу по конвертации
*/
void SceneTreeView::ConvertTGAtoQDAInInterface(qdNamedObjectBase* pobj)
{
	::ConvertTGAtoQDAInInterface convertor;
	convertor.convert(pobj);
}

void SceneTreeView::OnJoinProject()
{
	ProjectJoiner pj;
	pj.join(m_ptrGameDisp);
}

void SceneTreeView::OnUpdateJoinProject(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ptrGameDisp != NULL);
}

void SceneTreeView::OnAddGameEnd()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;
	AddGameEndWizardShower show;
	if (show.show(this, m_ptrGameDisp, hSel))
		SetProjChange();
}

void SceneTreeView::OnCreateCounter()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	ASSERT(hSelected);

	DWORD_PTR dwData = tree.GetItemData(hSelected);
	HTREEITEM hParent = hSelected;
	if (ptree_helpers::is_obj(dwData))
		hParent = tree.GetParentItem(hSelected);
	if (AddCounter(hParent, CString()))
	{
		SetProjChange();
		tree.Expand(hParent, TVE_EXPAND);
	}
}

void SceneTreeView::OnSelectMovemtnType()
{
	thread_suspender suspend(this);
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSelected = tree.GetSelectedItem();
	ASSERT(hSelected);
	if (!hSelected)
		return;
	qdGameObjectStateWalk* pstate = 
		reinterpret_cast<qdGameObjectStateWalk*>(tree.GetItemData(hSelected));
	if (SelectMovementType(pstate))
		SetProjChange();
}

void SceneTreeView::OnManageCds()
{
	if (showCDDiskManager(m_ptrGameDisp))
		SetProjChange();
}

//! Переименование инвентори
bool SceneTreeView::RenameInventory(qdGameDispatcher* disp, 
					 qdInventory* inventory, LPCTSTR lpszNewName)
{
	qdGameObjectList lst;
	CanRemoveInventory(false, inventory, &lst);
	if (!lst.empty())
	{
		CString str;
		str.Format(IDS_ASK_FOR_RENAME_INVENTORY, inventory->name());
		switch (MessageBox(str,NULL, MB_YESNOCANCEL))
		{
		case IDYES:
			{
				if (disp->rename_inventory(inventory, lpszNewName))
				{
					qdGameObjectList::iterator i = lst.begin(), e = lst.end();
					for(; i != e; ++i)
						static_cast<qdGameObjectAnimated*>(*i)->
										set_inventory_name(lpszNewName);
					return true;
				}
				return false;
			}
		case IDCANCEL:
			return false;
		}
	}
	return disp->rename_inventory(inventory, lpszNewName);
}


void SceneTreeView::OnMakeBoundAsFirst()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;
	qdGameObjectState* ptrState = 
		reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hSel));
	qdGameObjectAnimated* ptrObj = static_cast<qdGameObjectAnimated*>(ptrState->owner());
	assert(!ptrObj->state_vector().empty());

	qdGameObjectState* ptrFirstObjectState = ptrObj->state_vector()[0];
	if (ptrFirstObjectState != ptrState&&ptrFirstObjectState->has_bound())
	{
		ptrState->set_bound(ptrFirstObjectState->bound());
		ptrState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);
		SetProjChange();
	}
}

void SceneTreeView::OnCancelRemovingEdges()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;
	qdGameObjectState* ptrState = 
		reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hSel));
	qdGameObjectAnimated* ptrObj = static_cast<qdGameObjectAnimated*>(ptrState->owner());

	qdGameScene* scene = static_cast<qdGameScene*>(ptrObj->owner());
	qdCamera const* camera = scene->get_camera();
	Vect2i center = camera->global2scr(Vect3f(0.f, 0.f, 0.f));
	ptrState->set_center_offset(center - camera->global2scr(ptrObj->R()));
	SetProjChange();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void SceneTreeView::OnAddFont()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;
	AddFont(hSel);
}

void SceneTreeView::OnCreateFont()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;
	CreateFont(hSel);
}

void SceneTreeView::OnWaveToLocal()
{
	WaveToLoc obj;
	obj.run(this, *m_ptrGameDisp, false);
}

void SceneTreeView::OnLocalToWave()
{
	WaveToLoc obj;
	obj.run(this, *m_ptrGameDisp, true);
}

void SceneTreeView::OnUpdateWaveToLocal(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_ptrGameDisp != NULL);
}

void SceneTreeView::OnUpdateLocalToWave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_ptrGameDisp != NULL);
}

void SceneTreeView::OnStatDependencies()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hSel = tree.GetSelectedItem();
	if (!hSel)
		return;
	CWaitCursor wait;
	qdNamedObject* obj = reinterpret_cast<qdNamedObject*>(tree.GetItemData(hSel));
	statObjectDependencies(obj);
}
