// StateCondDlg.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "StateCondDlg.h"


#include "qd_game_object_state.h"
#include "qd_game_object_animated.h"
#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"
#include "condition.h"
#include "ptree_hlprs.hpp"
#include "qd_camera_mode.h"

#include <boost/function_output_iterator.hpp>
#include <boost/rtl.hpp>

#include <limits>
#include ".\stateconddlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AdjustListWidth(CCJFlatComboBox* pmyComboBox);

/////////////////////////////////////////////////////////////////////////////
// StateCondDlg dialog
bool ShowCondDlg(qdGameObjectState* ptrState, qdGameDispatcher* ptrDisp)
{
	StateCondDlg dlg;
	dlg.set_data(ptrDisp, ptrState);
	dlg.DoModal();
	return dlg.changed();
}

const int MIN_STATE_START_DELAY = 0;
const int MAX_STATE_START_DELAY = 10000;
const int MIN_STATE_DURATION = 0;
const int MAX_STATE_DURATION = 10000;
const int MAX_SCROLL_DISTANCE = 10000;
const int MIN_SCROLL_DISTANCE = 0;

float GetAnimWorkTime(qdAnimationFrameList const& lst){
	qdAnimationFrameList::const_iterator i = lst.begin(), e = lst.end();
	float res = 0.f;
	for(; i != e; ++i)
		res += (*i)->length();
	return res;
}

StateCondDlg::StateCondDlg(CWnd* pParent /*=NULL*/)
: CDialog(StateCondDlg::IDD, pParent)
, m_bSyncWithSound(FALSE)
, m_bInventory(FALSE)
, m_bMoveToInventory(FALSE)
, m_bEnableInterrupt(FALSE)
, m_fCamModeWorkTime(0)
, m_CondsMgr(member)
, m_fCamScrollSpeed(0)
, m_iCamScrollDistance(0)
, m_bCamSmoothSwitch(FALSE)
, m_bMouseState(FALSE)
, m_bMouseHoverState(FALSE)
, m_bStayInInventory(FALSE)
, m_vCamModeCameraOffset(0, 0)
, m_fSoundDelay(0.f)
, m_bAddConditionOnClose(false)
, m_bInterruptStateByClick(FALSE)
, textDelay_(0.f)
, moveToZone_(false)
, moveOnObject_(false)
, relativeCA_(FALSE)
, activatePersonage_(FALSE)
{
	//{{AFX_DATA_INIT(StateCondDlg)
	m_iCondLog = -1;
	m_bRestorePrev = FALSE;
	m_bHideObj = FALSE;
	m_bCAGo2First = FALSE;
	m_fStartDelay = 0.f;
	m_fStateDuration = 0.f;
	//}}AFX_DATA_INIT
	m_ptrSourceState = NULL;
	m_bChanged = false;
}

void StateCondDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StateCondDlg)
	DDX_Control(pDX, IDC_COMBO_CONDITION, m_wndConds);
	DDX_Radio(pDX, IDC_RADIO_LOG_CONN, m_iCondLog);
	DDX_Check(pDX, IDC_CHECK_RESTORE_PREF, m_bRestorePrev);
	DDX_Check(pDX, IDC_CHECK_HIDE_OBJ, m_bHideObj);
	DDX_Check(pDX, IDC_CHECK_CA_TO_FIRST, m_bCAGo2First);
	DDX_Text(pDX, IDC_EDIT_START_DELAY, m_fStartDelay);
	DDX_Text(pDX, IDC_EDIT_STATE_DURATION, m_fStateDuration);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_SYNC_WITH_SOUND, m_bSyncWithSound);
	DDX_Check(pDX, IDC_CHECK_INVENTORY, m_bInventory);
	DDX_Check(pDX, IDC_CHECK_MOVE2INVENTORY, m_bMoveToInventory);
	DDX_Check(pDX, IDC_CHECK_ENABLE_INTERRUPT, m_bEnableInterrupt);
	DDX_Check(pDX, IDC_CHECK_CANT_INTERRUPT_MOVING, m_bDisableInterruptWalk);
	DDX_Control(pDX, IDC_COMBO_CAMERA_MODE, m_wndCamModes);
	DDX_Text(pDX, IDC_EDIT_CAM_MODE_WORK_TIME, m_fCamModeWorkTime);
	DDX_Control(pDX, IDC_TREE, member);
	DDX_Text(pDX, IDC_EDIT_CAM_MODE_SCROLL_SPEED, m_fCamScrollSpeed);
	DDX_Text(pDX, IDC_EDIT_CAM_MODE_SCROLL_DISTANCE, m_iCamScrollDistance);
	DDV_MinMaxInt(pDX, m_iCamScrollDistance, MIN_SCROLL_DISTANCE, MAX_SCROLL_DISTANCE);
	DDX_Check(pDX, IDC_CHECK_SMOOTH_SWITCH, m_bCamSmoothSwitch);
	DDX_Check(pDX, IDC_CHECK_MOUSE_STATE, m_bMouseState);
	DDX_Check(pDX, IDC_CHECK_MOUSE_HOVER_STATE, m_bMouseHoverState);
	DDX_Check(pDX, IDC_CHECK_STAY_IN_INVENTORY, m_bStayInInventory);
	DDX_Text(pDX, IDC_EDIT_OFFSET_X, m_vCamModeCameraOffset.x);
	DDX_Text(pDX, IDC_EDIT_OFFS_Y, m_vCamModeCameraOffset.y);
	DDX_Text(pDX, IDC_EDIT_SOUND_DELAY, m_fSoundDelay);
	DDV_MinMaxFloat(pDX, m_fSoundDelay, 0.f, std::numeric_limits<float>::max());
	DDX_Check(pDX, IDC_CHECK_INTERRUPT_STATE_BY_CLICK, m_bInterruptStateByClick);
	DDX_Text(pDX, IDC_EDIT_TEXT_DELAY, textDelay_);
	DDX_Check(pDX, IDC_CHECK_MOVE_TO_ZONE, moveToZone_);
	DDX_Check(pDX, IDC_CHECK_MOVE_ON_OBJECT, moveOnObject_);
	DDX_Check(pDX, IDC_CHECK_RELATIVE_CA, relativeCA_);
	DDX_Control(pDX, IDC_COMBO_OBJECTS, caRelativeObjects_);
	DDX_Check(pDX, IDC_CHECK_ACTIVATE_PERSONAGE, activatePersonage_);
}


BEGIN_MESSAGE_MAP(StateCondDlg, CDialog)
	//{{AFX_MSG_MAP(StateCondDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_CONDITION, OnSelchangeComboCondition)
	ON_BN_CLICKED(IDC_BUTTON_ADD_COND, OnButtonAddCond)
	//}}AFX_MSG_MAP
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_START_DELAY, OnDeltaposSpinStartDelay)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_STATE_DURATION, OnDeltaposSpinStateDuration)
	ON_BN_CLICKED(IDC_CHECK_INVENTORY, OnBnClickedCheckInventory)
	ON_BN_CLICKED(IDC_CHECK_MOVE2INVENTORY, OnBnClickedCheckMove2inventory)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CAM_MODE_WORK_TIME, OnDeltaposSpinCamModeWorkTime)
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERA_MODE, OnCbnSelchangeComboCameraMode)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, OnNMRclickTree)
	ON_COMMAND(IDC_CREATE_OR, OnCreateOr)
	ON_COMMAND(IDC_CREATE_AND, OnCreateAnd)
	ON_COMMAND(IDC_CHANGE_TYPE, OnChangeType)
	ON_COMMAND(IDC_DELETE, OnDelete)
	ON_COMMAND(IDC_DELETE_ALL, OnDeleteAll)
	ON_NOTIFY(NM_MT_CAN_DROP_REQ, IDC_TREE, OnCanDropReq)
	ON_NOTIFY(NM_MT_DROP_REQ, IDC_TREE, OnDropReq)
	ON_NOTIFY(NM_MT_CAN_DRAG_REQ, IDC_TREE, OnCanDragReq)
	ON_COMMAND(IDC_REMOVE_FROM_GROUP, OnRemoveFromGroup)
	ON_BN_CLICKED(IDC_CHECK_SYNC_WITH_SOUND, OnBnClickedCheckSyncWithSound)
	ON_COMMAND(IDC_CREATE_AND_FOR_ALL, OnCreateAndForAll)
	ON_COMMAND(IDC_CREATE_OR_FOR_ALL, OnCreateOrForAll)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CAM_MODE_SCROLL_SPEED, OnDeltaposSpinCamModeScrollSpeed)
	ON_BN_CLICKED(IDC_CHECK_MOUSE_STATE, OnBnClickedCheckInventory1)
	ON_BN_CLICKED(IDC_CHECK_MOUSE_HOVER_STATE, OnBnClickedCheckInventory2)
	ON_BN_CLICKED(IDC_CHECK_STAY_IN_INVENTORY, OnBnClickedCheckInventory3)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SOUND_DELAY, OnDeltaposSpinSoundDelay)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TEXT_DELAY, OnDeltaposSpinTextDelay)
	ON_BN_CLICKED(IDC_CHECK_RELATIVE_CA, OnBnClickedCheckRelativeCa)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECTS, OnCbnSelchangeComboObjects)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StateCondDlg message handlers

bool StateCondDlg::set_data(qdGameDispatcher* ptrDisp, qdGameObjectState* ptrState){
	m_ptrGameDisp = ptrDisp;
	m_ptrSourceState = ptrState;
	m_qdState.reset(ptrState->clone());
	m_CondsMgr.set_obj(m_qdState.get());
	return true;
}

void StateCondDlg::OnOK() 
{
	CDialog::OnOK();

	if(getAddConditionOnClose())
	{
		if (AfxMessageBox(IDS_ASK_ADD_CONDITION, MB_OKCANCEL) == IDOK) {
			OnButtonAddCond();
		}
	}

	wnd2state();
	TreeToCO();

	if (!changed()) 
		return;

	*m_ptrSourceState = *m_qdState.get();
}

void StateCondDlg::testAndApplyFlag(qdNamedObject* pobj, BOOL bTestVal, int iFlag)
{
	if (!changed()) 
		m_bChanged = (bTestVal == static_cast<BOOL>(pobj->check_flag(iFlag)));

	if (bTestVal) 
		pobj->set_flag(iFlag);
	else
		pobj->drop_flag(iFlag);
}
//! загружает в состо€ние данные, которые не относ€тс€ к услови€м
void StateCondDlg::wnd2state()
{
	if (!changed()) {
		if (m_iCondLog == 0) {
			m_bChanged = qdConditionalObject::CONDITIONS_AND
						!=m_qdState->conditions_mode();
		}else if (m_iCondLog == 1) {
			m_bChanged = qdConditionalObject::CONDITIONS_OR
						!=m_qdState->conditions_mode();
		}
	}
	if (m_iCondLog == 0) 
		m_qdState->set_conditions_mode(qdConditionalObject::CONDITIONS_AND);
	else if (m_iCondLog == 1) 
		m_qdState->set_conditions_mode(qdConditionalObject::CONDITIONS_OR);

	testAndApplyFlag(m_qdState.get(), m_bRestorePrev, 
		qdGameObjectState::QD_OBJ_STATE_FLAG_RESTORE_PREV_STATE);

	testAndApplyFlag(m_qdState.get(), m_bHideObj, 
		qdGameObjectState::QD_OBJ_STATE_FLAG_HIDE_OBJECT);

	testAndApplyFlag(m_qdState.get(), m_bSyncWithSound,
		qdGameObjectState::QD_OBJ_STATE_FLAG_SOUND_SYNC);

	testAndApplyFlag(m_qdState->coords_animation(), m_bCAGo2First,
		QD_COORDS_ANM_OBJECT_START_FLAG);

	testAndApplyFlag(m_qdState.get(), m_bInventory, 
		qdGameObjectState::QD_OBJ_STATE_FLAG_INVENTORY);

	testAndApplyFlag(m_qdState.get(), m_bMoveToInventory,
		qdGameObjectState::QD_OBJ_STATE_FLAG_MOVE_TO_INVENTORY);

	testAndApplyFlag(m_qdState.get(), m_bEnableInterrupt,
		qdGameObjectState::QD_OBJ_STATE_FLAG_ENABLE_INTERRUPT);

	testAndApplyFlag(m_qdState.get(), m_bDisableInterruptWalk,
		qdGameObjectState::QD_OBJ_STATE_FLAG_DISABLE_WALK_INTERRUPT);

	testAndApplyFlag(m_qdState.get(), FALSE,
		qdGameObjectState::QD_OBJ_STATE_FLAG_MOUSE_HOVER_STATE
		|qdGameObjectState::QD_OBJ_STATE_FLAG_STAY_IN_INVENTORY
		|qdGameObjectState::QD_OBJ_STATE_FLAG_MOUSE_STATE);

	testAndApplyFlag(m_qdState.get(), m_bInterruptStateByClick, 
		qdGameObjectState::QD_OBJ_STATE_FLAG_ENABLE_SKIP);

	testAndApplyFlag(m_qdState.get(), moveToZone_,
		qdGameObjectState::QD_OBJ_STATE_FLAG_MOVE_TO_ZONE);

	testAndApplyFlag(m_qdState.get(), moveOnObject_,
		qdGameObjectState::QD_OBJ_STATE_FLAG_MOVE_ON_OBJECT);

	testAndApplyFlag(m_qdState->coords_animation(), relativeCA_,
		QD_COORDS_ANM_RELATIVE_FLAG);
	testAndApplyFlag(m_qdState.get(), activatePersonage_,
		qdGameObjectState::QD_OBJ_STATE_FLAG_ACTIVATE_PERSONAGE);
	if (relativeCA_&&m_bChanged)
	{
		int const curSel = caRelativeObjects_.GetCurSel();
		if (curSel != CB_ERR)
			m_qdState->coords_animation()->
				set_start_object(reinterpret_cast<qdGameObject*>(
											caRelativeObjects_.GetItemDataPtr(curSel)));
	}

	if (m_bInventory)
	{
		if (m_bStayInInventory)
			testAndApplyFlag(m_qdState.get(), m_bStayInInventory,
				qdGameObjectState::QD_OBJ_STATE_FLAG_STAY_IN_INVENTORY
				|qdGameObjectState::QD_OBJ_STATE_FLAG_MOUSE_STATE);
		if (m_bMouseState)
			testAndApplyFlag(m_qdState.get(), m_bMouseState,
				qdGameObjectState::QD_OBJ_STATE_FLAG_MOUSE_STATE);
		if (m_bMouseHoverState)
			testAndApplyFlag(m_qdState.get(), m_bMouseHoverState,
				qdGameObjectState::QD_OBJ_STATE_FLAG_MOUSE_HOVER_STATE);
	}
	
	if (m_fStartDelay != m_qdState->activation_delay()) {
		m_bChanged = true;
		m_qdState->set_activation_delay(m_fStartDelay);
	}
	if (m_fStateDuration != m_qdState->work_time()){
		m_bChanged = true;
		m_qdState->set_work_time(m_fStateDuration);
	}

	if (m_fSoundDelay != m_qdState->sound_delay()){
		m_bChanged = true;
		m_qdState->set_sound_delay(m_fSoundDelay);
	}

	if (textDelay_ != m_qdState->text_delay()){
		m_bChanged = true;
		m_qdState->set_text_delay(textDelay_);
	}

	qdCameraMode mode(m_cam_mode_rep.get_mode(m_wndCamModes.GetCurSel()));
	if (!changed()){
		m_bChanged = (mode.work_time() != m_fCamModeWorkTime);
		m_bChanged = m_bChanged ||(mode.scrolling_speed() != m_fCamScrollSpeed);
		m_bChanged = m_bChanged ||(mode.scrolling_distance() != m_iCamScrollDistance);
		m_bChanged = m_bChanged ||(mode.smooth_switch() != 
			static_cast<bool>(m_bCamSmoothSwitch));
		if (!changed()) 
			m_bChanged = (mode.camera_mode() != m_qdState->camera_mode().camera_mode());
	}

	if (mode.camera_mode() != qdCameraMode::MODE_UNASSIGNED) 
	{
		mode.set_work_time(m_fCamModeWorkTime);
		mode.set_scrolling_speed(m_fCamScrollSpeed);
		mode.set_scrolling_distance(m_iCamScrollDistance);
		mode.set_smooth_switch(m_bCamSmoothSwitch);
		if (m_cam_mode_rep.useCenterOffset(mode))
			mode.set_center_offset(m_vCamModeCameraOffset);
	}
	else
	{
		mode.set_work_time(0.f);
		mode.set_scrolling_speed(0.f);
		mode.set_scrolling_distance(0);
		mode.set_smooth_switch(false);
	}

	m_qdState->set_camera_mode(mode);
}
//! загружает из состо€ний данные, которые не относ€тс€ к услови€м
void StateCondDlg::state2wnd()
{
	LoadCondMode();

	m_bRestorePrev  = 
		m_qdState->check_flag(
		qdGameObjectState::QD_OBJ_STATE_FLAG_RESTORE_PREV_STATE);
	m_bHideObj		= 
		m_qdState->check_flag(
		qdGameObjectState::QD_OBJ_STATE_FLAG_HIDE_OBJECT);
	
	if (!m_qdState->coords_animation()->is_empty()) {
		GetDlgItem(IDC_CHECK_CA_TO_FIRST)->EnableWindow();
		m_bCAGo2First = m_qdState->coords_animation()->
			check_flag(QD_COORDS_ANM_OBJECT_START_FLAG);
	}

	if(m_qdState->sound_name())
	{
		m_bSyncWithSound = 
			m_qdState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_SOUND_SYNC);
	}
	else
		GetDlgItem(IDC_CHECK_SYNC_WITH_SOUND)->EnableWindow(FALSE);

	//это нужно, чтобы правильно определилось врем€ работы состо€ни€,
	//если установлен флаг QD_OBJ_STATE_FLAG_SOUND_SYNC
	if(m_qdState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_SOUND_SYNC))
	{
		if(qdSound* p = m_qdState->sound())
			p->load_resource();
	}

	m_fStartDelay = m_qdState->activation_delay();
	m_fStateDuration = m_qdState->work_time();
	m_fSoundDelay = m_qdState->sound_delay();
	textDelay_ = m_qdState->text_delay();

	m_bInventory = 
		m_qdState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_INVENTORY);
	EnableInventoryOptions(m_bInventory);

	m_bMoveToInventory = 
		m_qdState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_MOVE_TO_INVENTORY);
	m_bEnableInterrupt =
		m_qdState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_ENABLE_INTERRUPT);
	m_bDisableInterruptWalk = m_qdState->check_flag(
				qdGameObjectState::QD_OBJ_STATE_FLAG_DISABLE_WALK_INTERRUPT);

	moveOnObject_ = m_qdState->check_flag(
		qdGameObjectState::QD_OBJ_STATE_FLAG_MOVE_ON_OBJECT);
	moveToZone_	  = m_qdState->check_flag(
		qdGameObjectState::QD_OBJ_STATE_FLAG_MOVE_TO_ZONE);

	if (ptree_helpers::is_obj_moving(m_qdState->owner()))
		activatePersonage_ = m_qdState->check_flag(
		qdGameObjectState::QD_OBJ_STATE_FLAG_ACTIVATE_PERSONAGE);
	else
		GetDlgItem(IDC_CHECK_ACTIVATE_PERSONAGE)->EnableWindow(FALSE);

	if (!m_qdState->coords_animation()->is_empty())
	{
		relativeCA_ = 
			m_qdState->coords_animation()->check_flag(QD_COORDS_ANM_RELATIVE_FLAG);
		if (relativeCA_)
		{
			caRelativeObjects_.EnableWindow(TRUE);
			if (m_qdState->coords_animation()->start_object())
			{
				caRelativeObjects_.SelectString(-1, 
					m_qdState->coords_animation()->start_object()->name());
			}
		}
	}
	else
		GetDlgItem(IDC_CHECK_RELATIVE_CA)->EnableWindow(FALSE);

	if (m_qdState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_MOUSE_STATE))
	{
		if (m_qdState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_STAY_IN_INVENTORY))
			m_bStayInInventory = TRUE;
		else
			m_bMouseState = TRUE;
	}
	else if (m_qdState->
			check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_MOUSE_HOVER_STATE))
		m_bMouseHoverState = TRUE;

	m_bInterruptStateByClick = 
		m_qdState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_ENABLE_SKIP);

	m_wndCamModes.SelectString(-1, 
				m_cam_mode_rep.get_mode_string(m_qdState->camera_mode()));
	m_fCamModeWorkTime = m_qdState->camera_mode().work_time();
	m_fCamScrollSpeed = m_qdState->camera_mode().scrolling_speed();
	m_iCamScrollDistance = m_qdState->camera_mode().scrolling_distance();
	m_bCamSmoothSwitch	= m_qdState->camera_mode().smooth_switch();
	if (m_cam_mode_rep.useCenterOffset(m_qdState->camera_mode()))
		m_vCamModeCameraOffset = m_qdState->camera_mode().center_offset();

	CString strAnimWorkTime;
	if (m_qdState->state_type() == qdGameObjectState::STATE_STATIC)
	{
		qdGameObjectStateStatic* pst = 
			static_cast<qdGameObjectStateStatic*>(m_qdState.get());
		qdAnimation* pan = pst->animation();
		if (pan)
		{
			pan->load_resource();
			strAnimWorkTime.Format(IDS_ANIM_WORK_TIME, 
				GetAnimWorkTime(pan->frames_list()));
		}
		else
			strAnimWorkTime.LoadString(IDS_NO_ANIM_WORK_TIME);
	}
	else
		strAnimWorkTime.LoadString(IDS_NO_ANIM_WORK_TIME);
	GetDlgItem(IDC_STATIC_ANIM_TIME)->SetWindowText(strAnimWorkTime);
	SetAccessToStateDuration();

}

bool StateCondDlg::CanEnableInventory()
{
	return(m_qdState->state_type()!=qdGameObjectState::STATE_MASK
		&&m_qdState->state_type()!=qdGameObjectState::STATE_WALK
		&&m_qdState->coords_animation()->is_empty()
		&&!m_bMoveToInventory);
}

bool StateCondDlg::CanEnableMoveToInv(){
	return (m_qdState->state_type()!=qdGameObjectState::STATE_MASK
		&&!ptree_helpers::is_obj_moving(m_qdState->owner())
		&&!m_bInventory);
}

bool StateCondDlg::IsMouseCursor(){
	return (m_qdState->owner()->named_object_type() == QD_NAMED_OBJECT_MOUSE_OBJ);
}

//! —тартова€ функци€ диалога. ѕроизводит настройку всех контролов
BOOL StateCondDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_START_DELAY))->
		SetRange(MIN_STATE_START_DELAY, MAX_STATE_START_DELAY);
	static_cast<CSpinButtonCtrl*>(GetDlgItem((IDC_SPIN_STATE_DURATION)))->
		SetRange(MIN_STATE_DURATION, MAX_STATE_DURATION);
	static_cast<CSpinButtonCtrl*>(GetDlgItem((IDC_SPIN_CAM_MODE_SCROLL_DISTANCE)))->
		SetRange(MIN_SCROLL_DISTANCE, MAX_SCROLL_DISTANCE);
	static_cast<CSpinButtonCtrl*>(GetDlgItem((IDC_SPIN_CAM_MODE_SCROLL_SPEED)))->
		SetRange(0, 100);
	static_cast<CSpinButtonCtrl*>(GetDlgItem((IDC_SPIN_SOUND_DELAY)))->
		SetRange(0, 100);
	static_cast<CSpinButtonCtrl*>(GetDlgItem((IDC_SPIN_TEXT_DELAY)))->
		SetRange(0, 100);

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_OFFS_X))->SetRange(-10000, 1000);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_OFFS_Y))->SetRange(-10000, 1000);

	CWnd* pWnd = GetDlgItem(IDC_STATIC_PLACE_HOLDER);

	CRect rcPlaceHolder;
	pWnd->GetWindowRect(rcPlaceHolder);
	ScreenToClient(rcPlaceHolder);
	m_CDMgr.init(this, this, rcPlaceHolder);

	SetupAccessToCtrls();

	LoadConditions();
	LoadCameraModes();
	loadCARelativeObjects(caRelativeObjects_, m_ptrSourceState->owner());

	caRelativeObjects_.EnableWindow(FALSE);

	GetDlgItem(IDC_BUTTON_ADD_COND)->EnableWindow(FALSE);
	SetButtonsState();

	LoadGroups();
	ObjConditionsToTree();

	state2wnd();
	
	UpdateData(FALSE);

	AdjustListWidth(&m_wndConds);

	GetDlgItem(IDC_CHECK_MOVE2INVENTORY)->EnableWindow(CanEnableMoveToInv());
	GetDlgItem(IDC_CHECK_INVENTORY)->EnableWindow(CanEnableInventory());

	GetDlgItem(IDC_SPIN_SOUND_DELAY)->EnableWindow(m_qdState->has_sound());
	GetDlgItem(IDC_EDIT_SOUND_DELAY)->EnableWindow(m_qdState->has_sound());

	bool const enableTextDelay = (m_qdState->has_short_text()
		&&!m_qdState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_DIALOG_PHRASE))
		||m_qdState->has_full_text();
	GetDlgItem(IDC_SPIN_TEXT_DELAY)->EnableWindow(enableTextDelay);
	GetDlgItem(IDC_EDIT_TEXT_DELAY)->EnableWindow(enableTextDelay);


	AdjustCamModeWorkTimeControlAccs();
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_CAM_MODE_WORK_TIME))
												->SetRange(0, 100);
	CString cap;
	cap.Format(IDS_DLG_CAP_STATE_COND, 
		m_ptrSourceState->owner()->name(),m_ptrSourceState->name());
	SetWindowText(cap);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void StateCondDlg::LoadConditions()
{
	/*
	for(int i = 0; i < g_iCondsSize; ++i)
	{
		int iPos = m_wndConds.AddString(g_arrConds[i].strName);
		if (iPos != -1) {
			m_wndConds.SetItemData(iPos, static_cast<DWORD>(g_arrConds[i].id));
		}
	}*/
	fillConditionList(m_wndConds);
}

void StateCondDlg::OnSelchangeComboCondition() 
{
	int iCurSel = m_wndConds.GetCurSel();
	_ASSERT(iCurSel != -1);

	DWORD id = m_wndConds.GetItemData(iCurSel);
	ShowCondDlg(id);
}

void StateCondDlg::ShowCondDlg(DWORD id)
{
	if(m_CDMgr.get_current())
		m_CDMgr.get_current()->ShowWindow(SW_HIDE);

	m_CDMgr.set_current(id);
	if(m_CDMgr.get_current())
		m_CDMgr.get_current()->ShowWindow(SW_SHOW);
}


void StateCondDlg::LoadCondMode()
{
	if(m_qdState->conditions_mode() == qdConditionalObject::CONDITIONS_AND)
		m_iCondLog = 0;
	else if (m_qdState->conditions_mode() == qdConditionalObject::CONDITIONS_OR) {
		m_iCondLog = 1;
	}
}

void StateCondDlg::OnButtonAddCond() 
{
	// ќбновл€ем данные текущего выбранного диалога редактировани€ услови€
	if (FALSE == m_CDMgr.get_current()->UpdateData(true))
		return;

	if (m_CondsMgr.AddCondition(&m_CDMgr.get_current()->get_condtion()))
	{
		GetDlgItem(IDC_BUTTON_ADD_COND)->EnableWindow(CanAdd());
	}
	setAddConditionOnClose(false);
}

bool StateCondDlg::CanAdd()
{
	if (m_CDMgr.get_current()) 
		return m_CDMgr.get_current()->CanAdd(m_qdState.get());
	return false;
}

void StateCondDlg::OnCondDataComplete(bool bComplete)
{
	bool const b = bComplete&&CanAdd()&&m_CDMgr.get_current();
	setAddConditionOnClose(b);
	GetDlgItem(IDC_BUTTON_ADD_COND)->EnableWindow(b);
}

void StateCondDlg::SetButtonsState()
{
	if (m_CDMgr.get_current())
		OnCondDataComplete(m_CDMgr.get_current()->IsCondDataComplete());
}	

BOOL StateCondDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_DELETE)	
	{
		if(pMsg->hwnd == member.m_hWnd)
		{
			OnDelete();
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

class qdGameScene* StateCondDlg::get_scene() const{
	qdNamedObject* pno = m_ptrSourceState->ref_owner();
	if (ptree_helpers::IsGlobalObj(pno)) 
		return NULL;
	return static_cast<qdGameScene*>(pno->owner());
}

void StateCondDlg::OnDeltaposSpinStartDelay(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if(!UpdateData()){
		*pResult = 1;
		return;
	}
	m_fStartDelay += pNMUpDown->iDelta/100.f;
	if (m_fStartDelay< 0.f)
		m_fStartDelay = 0.f;
	UpdateData(FALSE);
	*pResult = 0;
}

void StateCondDlg::OnDeltaposSpinStateDuration(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if(!UpdateData()){
		*pResult = 1;
		return;
	}
	m_fStateDuration += pNMUpDown->iDelta/100.f;
	if (m_fStateDuration < 0.f)
		m_fStateDuration = 0.f;
	UpdateData(FALSE);
	*pResult = 0;
}

void StateCondDlg::OnBnClickedCheckInventory()
{
	UpdateData();
	EnableInventoryOptions(m_bInventory);
	GetDlgItem(IDC_CHECK_MOVE2INVENTORY)->EnableWindow(CanEnableMoveToInv());	
}

void StateCondDlg::OnBnClickedCheckMove2inventory()
{
	UpdateData();

	GetDlgItem(IDC_CHECK_INVENTORY)->EnableWindow(CanEnableInventory());	
}

void StateCondDlg::OnDeltaposSpinCamModeWorkTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	if(!UpdateData()){
		*pResult = 1;
		return;
	}
	m_fCamModeWorkTime += pNMUpDown->iDelta/10.f;
	if (m_fCamModeWorkTime < 0.f)
		m_fCamModeWorkTime = 0.f;

	UpdateData(FALSE);

	*pResult = 0;
}

void StateCondDlg::OnDeltaposSpinCamModeScrollSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if(!UpdateData()){
		*pResult = 1;
		return;
	}
	m_fCamScrollSpeed += pNMUpDown->iDelta/10.f;
	if (m_fCamScrollSpeed < 0.f)
		m_fCamScrollSpeed = 0.f;

	UpdateData(FALSE);

	*pResult = 0;
}

void StateCondDlg::LoadCameraModes()
{
	for(int i = 0; i < m_cam_mode_rep.modes_count(); ++i)
		m_wndCamModes.AddString(m_cam_mode_rep.get_mode_string(i));

	CString str(m_cam_mode_rep.get_mode_string(m_qdState->camera_mode()));
	m_wndCamModes.SelectString(-1, str);

	AdjustListWidth(&m_wndCamModes);
}

void StateCondDlg::AdjustCamModeWorkTimeControlAccs()
{
	qdCameraMode mode = m_cam_mode_rep.get_mode(m_wndCamModes.GetCurSel());
	const bool bE = (mode.camera_mode() != qdCameraMode::MODE_UNASSIGNED);

	GetDlgItem(IDC_SPIN_CAM_MODE_WORK_TIME)->EnableWindow(bE);
	GetDlgItem(IDC_EDIT_CAM_MODE_WORK_TIME)->EnableWindow(bE);
	GetDlgItem(IDC_EDIT_CAM_MODE_SCROLL_SPEED)->EnableWindow(bE);
	GetDlgItem(IDC_EDIT_CAM_MODE_SCROLL_DISTANCE)->EnableWindow(bE);
	GetDlgItem(IDC_SPIN_CAM_MODE_SCROLL_SPEED)->EnableWindow(bE);
	GetDlgItem(IDC_SPIN_CAM_MODE_SCROLL_DISTANCE)->EnableWindow(bE);
	GetDlgItem(IDC_CHECK_SMOOTH_SWITCH)->EnableWindow(bE);
	const bool bUseCenter = m_cam_mode_rep.useCenterOffset(mode);
	GetDlgItem(IDC_EDIT_OFFSET_X)->EnableWindow(bUseCenter);
	GetDlgItem(IDC_EDIT_OFFS_Y)->EnableWindow(bUseCenter);
	GetDlgItem(IDC_SPIN_OFFS_X)->EnableWindow(bUseCenter);
	GetDlgItem(IDC_SPIN_OFFS_Y)->EnableWindow(bUseCenter);
}

void StateCondDlg::OnCbnSelchangeComboCameraMode()
{
	AdjustCamModeWorkTimeControlAccs();
}

void StateCondDlg::SetupAccessToCtrls()
{
	if (IsMouseCursor()) 
	{
		GetDlgItem(IDC_EDIT_START_DELAY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SPIN_START_DELAY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_STATE_DURATION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SPIN_STATE_DURATION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_CAMERA_MODE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_CAM_MODE_WORK_TIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SPIN_CAM_MODE_WORK_TIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_CA_TO_FIRST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_INVENTORY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_MOVE2INVENTORY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_ENABLE_INTERRUPT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_DURATION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_6)->ShowWindow(SW_HIDE);

		CRect r;
		GetDlgItem(IDC_CHECK_SYNC_WITH_SOUND)->GetWindowRect(r);
		ScreenToClient(r);
		
		int new_bts_pos = r.bottom + 10;

		CWnd* pb = GetDlgItem(IDOK);
		pb->GetWindowRect(&r);
		ScreenToClient(r);
		pb->MoveWindow(r.left, new_bts_pos,r.Width(), r.Height(), FALSE);


		pb = GetDlgItem(IDCANCEL);
		pb->GetWindowRect(&r);
		ScreenToClient(r);
		pb->MoveWindow(r.left, new_bts_pos,r.Width(), r.Height(), FALSE);

		pb->GetWindowRect(r);

		CRect dlgRect;
		GetWindowRect(dlgRect);
		dlgRect.bottom = r.bottom + 10;
		MoveWindow(dlgRect);

	}
}

bool StateCondDlg::LoadGroups()
{
	return m_CondsMgr.LoadGroups();
}

void StateCondDlg::OnNMRclickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = m_CondsMgr.OnNMRclickTree(this);
}

void StateCondDlg::OnCreateOr()
{
	m_CondsMgr.CreateGroup(qdConditionGroup::CONDITIONS_OR);
}

void StateCondDlg::OnCreateAnd()
{
	m_CondsMgr.CreateGroup(qdConditionGroup::CONDITIONS_AND);
}

void StateCondDlg::OnChangeType()
{
	m_CondsMgr.InvertType();
}

void StateCondDlg::ObjConditionsToTree()
{
	m_CondsMgr.CondsToTree();
}

void StateCondDlg::OnDelete()
{
	m_CondsMgr.DeleteSelected();
}

void StateCondDlg::OnCanDropReq(NMHDR* phdr, LRESULT* pResult)
{
	*pResult = m_CondsMgr.OnCanDropReq(reinterpret_cast<NM_MT_CAN_DROP*>(phdr));
}

void StateCondDlg::OnCanDragReq(NMHDR* phdr, LRESULT* pResult)
{
	*pResult = m_CondsMgr.OnCanDragReq(reinterpret_cast<NM_MT_CAN_DRAG*>(phdr));
}

void StateCondDlg::OnDropReq(NMHDR* phdr, LRESULT* pResult)
{
	*pResult = m_CondsMgr.OnDropReq(reinterpret_cast<NM_MT_DROP*>(phdr));
}

void StateCondDlg::TreeToCO()
{
	if (m_CondsMgr.was_changed())
		m_CondsMgr.TreeToCO();
}

void StateCondDlg::OnRemoveFromGroup()
{
	m_CondsMgr.RemoveFromGroup();
}

void StateCondDlg::OnBnClickedCheckSyncWithSound()
{
	if (!UpdateData())
		return;
	if (!m_bSyncWithSound)
	{
		m_fStateDuration = 0.f;
	}
	else
		m_fStateDuration = m_qdState->work_time();
	UpdateData(FALSE);

	SetAccessToStateDuration();
}

void StateCondDlg::SetAccessToStateDuration()
{
	if (m_qdState->state_type() == qdGameObjectState::STATE_STATIC)
	{
		qdGameObjectStateStatic* pst = 
		static_cast<qdGameObjectStateStatic*>(m_qdState.get());
		qdAnimation* pan = pst->animation();
		if (!pan)
		{
			GetDlgItem(IDC_EDIT_STATE_DURATION)->EnableWindow(FALSE);
			GetDlgItem(IDC_SPIN_STATE_DURATION)->EnableWindow(FALSE);
			return;
		}
	}
	GetDlgItem(IDC_EDIT_STATE_DURATION)->EnableWindow(!m_bSyncWithSound);
	GetDlgItem(IDC_SPIN_STATE_DURATION)->EnableWindow(!m_bSyncWithSound);
}

void StateCondDlg::OnDeleteAll(){
	m_CondsMgr.DeleteAll();
}

void StateCondDlg::OnCreateAndForAll()
{
	m_CondsMgr.CreateGroups(qdConditionGroup::CONDITIONS_AND);
}

void StateCondDlg::OnCreateOrForAll()
{
	m_CondsMgr.CreateGroups(qdConditionGroup::CONDITIONS_OR);
}

void StateCondDlg::EnableInventoryOptions(BOOL bEnable)
{
	GetDlgItem(IDC_CHECK_MOUSE_STATE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_MOUSE_HOVER_STATE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_STAY_IN_INVENTORY)->EnableWindow(bEnable);
}
void StateCondDlg::OnBnClickedCheckInventory1()
{
	UpdateData();
	m_bMouseHoverState = FALSE;
	m_bStayInInventory = FALSE;
	UpdateData(FALSE);
}

void StateCondDlg::OnBnClickedCheckInventory2()
{
	UpdateData();
	m_bMouseState = FALSE;
	m_bStayInInventory = FALSE;
	UpdateData(FALSE);
}

void StateCondDlg::OnBnClickedCheckInventory3()
{
	UpdateData();
	m_bMouseHoverState = FALSE;
	m_bMouseState = FALSE;
	UpdateData(FALSE);
}

void StateCondDlg::OnDeltaposSpinSoundDelay(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}
	m_fSoundDelay += pNMUpDown->iDelta / 100.f;
	if (m_fSoundDelay < 0.f)
		m_fSoundDelay = 0.f;
	UpdateData(FALSE);
	*pResult = 0;
}

void StateCondDlg::OnDeltaposSpinTextDelay(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}
	textDelay_ += pNMUpDown->iDelta / 100.f;
	if (textDelay_ < 0.f)
		textDelay_ = 0.f;
	UpdateData(FALSE);
	*pResult = 0;
}

void StateCondDlg::loadCARelativeObjects(CCJFlatComboBox& box, 
										 qdNamedObject const* exclude)
{
	qdGameScene const* scene = get_scene();
	if (!scene)
		return;

	boost::rtl::crange<qdGameObjectList const> r(scene->object_list());
	for(; r; ++r)
	{
		qdGameObject const* obj = *r;
		if (obj != exclude)
		{
			int const item = box.AddString(obj->name());
			if (item != CB_ERR)
				box.SetItemDataPtr(item, *r);
		}
	}
	CString str;
	str.LoadString(IDS_SELF_RELATIVE);
	box.InsertString(0, str);
	box.SetCurSel(0);
}

void StateCondDlg::OnBnClickedCheckRelativeCa()
{
	if (!UpdateData())
		return;
	caRelativeObjects_.EnableWindow(relativeCA_);
}

void StateCondDlg::OnCbnSelchangeComboObjects()
{

}
