// PersonageControlPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "PersonageControlPropPage.h"

#include "MovementTypeManager.h"

#include "qd_game_object_moving.h"
#include ".\personagecontrolproppage.h"

#include <boost/rtl.hpp>

#include "obj_type_detectors.h"

// PersonageControlPropPage dialog

void AdjustListWidth(CCJFlatComboBox* box);
IMPLEMENT_DYNAMIC(PersonageControlPropPage, CBasePage)
PersonageControlPropPage::PersonageControlPropPage()
	: CBasePage(PersonageControlPropPage::IDD)
	, m_bMouseControl(FALSE)
	, m_bKeyboardControl(FALSE)
	, m_bMouseControlBak(FALSE)
	, m_bKeyboardControlBak(FALSE)
	, m_fCollisionRadius(0.f)
	, m_fCollisionDelay(0.f)
	, m_fCollisionPath(0.f)
	, m_fCollisionRadiusBak(0.f)
	, m_fCollisionDelayBak(0.f)
	, m_fCollisionPathBak(0.f)
	, m_bCollisionControl(FALSE)
	, m_bCollisionControlBak(FALSE)
	, m_bAvoidCollision(FALSE)
	, m_bAutoMove(FALSE)
	, m_bAvoidCollisionBak(FALSE)
	, m_bAutoMoveBak(FALSE)
	, m_ptrMovementTypeManager(new MovementTypeManager)
	, clearPath_(FALSE)
	, followActivePers_(FALSE)
	, repeatActivePersMovement_(FALSE)
	, followRadiusMin_(0.f)
	, followRadiusMax_(0)
	, attacherOffsetX_(0)
	, attacherOffsetY_(0)
	, controlAttach_(FALSE)
	, controlAttachBak_(FALSE)
	, attacherBak_(NULL)
	, attacherOffsetXBak_(0)
	, attacherOffsetYBak_(0)
	, controlAttachWithoutDirRel_(FALSE)
	, controlAttachToActiveWithMoving_(FALSE)
	, controlAttachWithoutDirRelBak_(FALSE)
	, controlAttachToActiveWithMovingBak_(FALSE)
	, m_bActiveClickReacting(FALSE)
	, m_bActiveClickReactingBak(FALSE)
	, m_fRotationSpeed(1.0f)
	, m_fRotationSpeedBak(1.0f)
	, m_bAnimatedRotation(FALSE)
	, m_bAnimatedRotationBak(FALSE)
{
}

PersonageControlPropPage::~PersonageControlPropPage()
{
}

void PersonageControlPropPage::set_data(qdGameObjectMoving* ptrObj)
{
	m_ptrObj = ptrObj;
}

void PersonageControlPropPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_MOUSE_CONTROL, m_bMouseControl);
	DDX_Check(pDX, IDC_CHECK_KEYBOARD_CONTROL, m_bKeyboardControl);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_fCollisionRadius);
	DDX_Text(pDX, IDC_EDIT_TIME, m_fCollisionDelay);
	DDX_Check(pDX, IDC_CHECK_COLLISION_CONTROL, m_bCollisionControl);
	DDX_Text(pDX, IDC_EDIT_COLLISION_PATH, m_fCollisionPath);
	DDX_Check(pDX, IDC_CHECK_AVOID_COLLISION, m_bAvoidCollision);
	DDX_Check(pDX, IDC_CHECK_AUTO_MOVE, m_bAutoMove);
	DDX_Control(pDX, IDC_COMBO_MOVEMENT_MODE, m_wndMovementModes);
	DDX_Check(pDX, IDC_CHECK_CLEAR_PATH, clearPath_);
	DDX_Check(pDX, IDC_CHECK_FOLLOW_ACTIVE_PERSONAGE, followActivePers_);
	DDX_Check(pDX, IDC_CHECK_REPEAT_ACTIVE_PERSONAGE_MOVEMENT, repeatActivePersMovement_);
	DDX_Text(pDX, IDC_EDIT_FOLLOW_RADIUS_MIN, followRadiusMin_);
	DDX_Text(pDX, IDC_EDIT_FOLLOW_RADIUS_MAX, followRadiusMax_);
	DDX_Text(pDX, IDC_EDIT_ATTACHER_OFFSET_X, attacherOffsetX_);
	DDX_Text(pDX, IDC_EDIT_ATTACHER_OFFSET_Y, attacherOffsetY_);
	DDX_Control(pDX, IDC_COMBO_ATTACHER, attachers_);
	DDX_Check(pDX, IDC_CHECK_CONTROL_ATTACHMENT, controlAttach_);
	DDX_Check(pDX, IDC_CHECK_CONTROL_ATTACHMENT_WITHOUT_DIR_REL, controlAttachWithoutDirRel_);
	DDX_Check(pDX, IDC_CHECK_CONTROL_ATTACHMENT_TO_ACTIVE_WITH_MOVING, controlAttachToActiveWithMoving_);
	DDX_Check(pDX, IDC_CHECK_ACTIVE_CLICK_REACTING, m_bActiveClickReacting);
	DDX_Check(pDX, IDC_CHECK_ANIMATE_ROTATION, m_bAnimatedRotation);
	DDX_Text(pDX, IDC_EDIT_ROTATION_SPEED, m_fRotationSpeed);
	if (pDX->m_bSaveAndValidate && (m_fRotationSpeed < 0.1f)) m_fRotationSpeed = 0.1f;
}


BEGIN_MESSAGE_MAP(PersonageControlPropPage, CBasePage)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RADIUS, OnDeltaposSpinRadius)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TIME, OnDeltaposSpinTime)
	ON_BN_CLICKED(IDC_CHECK_MOUSE_CONTROL, OnBnClickedCheckMouseControl)
	ON_BN_CLICKED(IDC_CHECK_KEYBOARD_CONTROL, OnBnClickedCheckKeyboardControl)
	ON_BN_CLICKED(IDC_CHECK_COLLISION_CONTROL, OnBnClickedCheckCollisionControl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COLLISION_PATH, OnDeltaposSpinCollisionPath)
	ON_BN_CLICKED(IDC_CHECK_AVOID_COLLISION, OnBnClickedAnyCheck)
	ON_BN_CLICKED(IDC_CHECK_AUTO_MOVE, OnBnClickedAnyCheck)
	ON_BN_CLICKED(IDC_CHECK_CLEAR_PATH, OnBnClickedAnyCheck)
	ON_BN_CLICKED(IDC_CHECK_REPEAT_ACTIVE_PERSONAGE_MOVEMENT, OnBnClickedAnyCheck)
	ON_CBN_SELCHANGE(IDC_COMBO_MOVEMENT_MODE, OnCbnSelchangeComboMovementMode)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_FOLLOW_RADIUS_MIN, OnDeltaposSpinFollowRadiusMin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_FOLLOW_RADIUS_MAX, OnDeltaposSpinFollowRadiusMax)
	ON_BN_CLICKED(IDC_CHECK_FOLLOW_ACTIVE_PERSONAGE, OnBnClickedCheckFollowActivePersonage)
	ON_BN_CLICKED(IDC_CHECK_CONTROL_ATTACHMENT, OnBnClickedCheckControlAttachment)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ATTACHER_OFFSET_X, OnDeltaposSpinAttacherOffset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ATTACHER_OFFSET_Y, OnDeltaposSpinAttacherOffset)
	ON_CBN_SELCHANGE(IDC_COMBO_ATTACHER, OnCbnSelchangeComboAttacher)
	ON_BN_CLICKED(IDC_CHECK_CONTROL_ATTACHMENT_TO_ACTIVE_WITH_MOVING, OnBnClickedCheckControlAttachmentToActiveWithMoving)
	ON_BN_CLICKED(IDC_CHECK_CONTROL_ATTACHMENT_WITHOUT_DIR_REL, OnBnClickedCheckControlAttachmentWithoutDirRel)
	ON_BN_CLICKED(IDC_CHECK_ACTIVE_CLICK_REACTING, OnBnClickedCheckActiveClickReacting)
	ON_BN_CLICKED(IDC_CHECK_ANIMATE_ROTATION, OnBnClickedCheckAnimatedRotation)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ROTATION_SPEED, OnDeltaposSpinRotationSpeed)
END_MESSAGE_MAP()

void PersonageControlPropPage::real2bak()
{
	m_bMouseControlBak		= m_bMouseControl;
	m_bKeyboardControlBak	= m_bKeyboardControl;
	m_bCollisionControlBak	= m_bCollisionControl;
	m_bAvoidCollisionBak	= m_bAvoidCollision;
	m_bAutoMoveBak			= m_bAutoMove;
	m_fCollisionRadiusBak	= m_fCollisionRadius;
	m_fCollisionDelayBak	= m_fCollisionDelay;
	m_fCollisionPathBak		= m_fCollisionPath;
	followActivePersBak_	= followActivePers_;
	repeatActivePersMovementBak_ = repeatActivePersMovement_;
	followRadiusMinBak_		= followRadiusMin_;
	followRadiusMaxBak_		= followRadiusMax_;
	clearPathBak_			= clearPath_;

	controlAttachBak_ = controlAttach_;
	attacherBak_ = m_ptrObj->attacher();
	attacherOffsetXBak_ = attacherOffsetX_;
	attacherOffsetYBak_ = attacherOffsetY_;

	controlAttachWithoutDirRelBak_		= controlAttachWithoutDirRel_;
	controlAttachToActiveWithMovingBak_ = controlAttachToActiveWithMoving_;

	m_bActiveClickReactingBak = m_bActiveClickReacting;

	m_bAnimatedRotationBak = m_bAnimatedRotation;
	m_fRotationSpeedBak = m_fRotationSpeed;
}

void PersonageControlPropPage::bak2real()
{
	m_bMouseControl		= m_bMouseControlBak;
	m_bKeyboardControl	= m_bKeyboardControlBak;
	m_bCollisionControl = m_bCollisionControlBak;
	m_bAvoidCollision	= m_bAvoidCollisionBak;
	m_bAutoMove			= m_bAutoMoveBak;
	m_fCollisionRadius	= m_fCollisionRadiusBak;
	m_fCollisionDelay	= m_fCollisionDelayBak;
	m_fCollisionPath	= m_fCollisionPathBak;

	followActivePers_	= followActivePersBak_;
	repeatActivePersMovement_ = repeatActivePersMovementBak_;
	followRadiusMin_		= followRadiusMinBak_;
	followRadiusMax_		= followRadiusMaxBak_;

	clearPath_			= clearPathBak_;

	controlAttach_		= controlAttachBak_;
	m_ptrObj->set_attacher(attacherBak_);
	attacherOffsetX_	= attacherOffsetXBak_;
	attacherOffsetY_	= attacherOffsetYBak_;

	controlAttachWithoutDirRel_		= controlAttachWithoutDirRelBak_;
	controlAttachToActiveWithMoving_ = controlAttachToActiveWithMovingBak_;

	m_bActiveClickReacting = m_bActiveClickReactingBak;
	m_bAnimatedRotation = m_bAnimatedRotationBak;

	m_fRotationSpeed = m_fRotationSpeedBak;
}

// PersonageControlPropPage message handlers

BOOL PersonageControlPropPage::OnInitDialog()
{
	CBasePage::OnInitDialog();

	loadPersonages();
	if (!attachers_.IsWindowEnabled())
	{
		GetDlgItem(IDC_CHECK_CONTROL_ATTACHMENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CONTROL_ATTACHMENT_WITHOUT_DIR_REL)->EnableWindow(FALSE);
	}
	obj2wnd();
	real2bak();
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_RADIUS))->SetRange(1, 100);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_TIME))->SetRange(1, 100);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_COLLISION_PATH))->SetRange(1, 100);
	static_cast<CSpinButtonCtrl*>(
		GetDlgItem(IDC_SPIN_FOLLOW_RADIUS_MIN))->SetRange(1, 100);
	static_cast<CSpinButtonCtrl*>(
		GetDlgItem(IDC_SPIN_FOLLOW_RADIUS_MAX))->SetRange(1, 100);

	static_cast<CSpinButtonCtrl*>(
		GetDlgItem(IDC_SPIN_ATTACHER_OFFSET_X))->SetRange(-10000, 10000);
	static_cast<CSpinButtonCtrl*>(
		GetDlgItem(IDC_SPIN_ATTACHER_OFFSET_Y))->SetRange(-10000, 10000);

	static_cast<CSpinButtonCtrl*>(
		GetDlgItem(IDC_SPIN_ROTATION_SPEED))->SetRange(1, 100);

	load_movement_types();
	AdjustListWidth(&m_wndMovementModes);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL PersonageControlPropPage::OnApply()
{
	if (!UpdateData())
		return FALSE;

	wnd2obj();
	real2bak();

	return CBasePage::OnApply();
}

void PersonageControlPropPage::OnCancel()
{
	bak2real();
	CBasePage::OnCancel();
}

void PersonageControlPropPage::wnd2obj()
{
	reset_control(m_bMouseControl, qdGameObjectMoving::CONTROL_MOUSE);
	reset_control(m_bKeyboardControl, qdGameObjectMoving::CONTROL_KEYBOARD);
	reset_control(m_bCollisionControl, qdGameObjectMoving::CONTROL_COLLISION);
	reset_control(m_bAvoidCollision, qdGameObjectMoving::CONTROL_AVOID_COLLISION);
	reset_control(m_bAutoMove, qdGameObjectMoving::CONTROL_AUTO_MOVE);
	reset_control(clearPath_, qdGameObjectMoving::CONTROL_CLEAR_PATH);
	reset_control(followActivePers_, 
		qdGameObjectMoving::CONTROL_FOLLOW_ACTIVE_PERSONAGE);
	reset_control(repeatActivePersMovement_, 
		qdGameObjectMoving::CONTROL_REPEAT_ACTIVE_PERSONAGE_MOVEMENT);
	reset_control(controlAttach_, qdGameObjectMoving::CONTROL_ATTACHMENT_WITH_DIR_REL);

	reset_control(controlAttachToActiveWithMoving_, 
		qdGameObjectMoving::CONTROL_ATTACHMENT_TO_ACTIVE_WITH_MOVING);
	reset_control(controlAttachWithoutDirRel_, 
		qdGameObjectMoving::CONTROL_ATTACHMENT_WITHOUT_DIR_REL);
	
	reset_control(m_bActiveClickReacting, 
		qdGameObjectMoving::CONTROL_ACTIVE_CLICK_REACTING);
	reset_control(m_bAnimatedRotation, 
		qdGameObjectMoving::CONTROL_ANIMATED_ROTATION);

	m_ptrObj->set_collision_delay(m_fCollisionDelay);
	m_ptrObj->set_collision_radius(m_fCollisionRadius);
	m_ptrObj->set_collision_path(m_fCollisionPath);
	m_ptrObj->set_follow_min_radius(followRadiusMin_);
	m_ptrObj->set_follow_max_radius(followRadiusMax_);

	m_ptrObj->set_attacher(NULL);
	m_ptrObj->set_attach_shift(Vect2s(0, 0));

	if (controlAttach_||controlAttachWithoutDirRel_)
	{
		int const item = attachers_.GetCurSel();
		if (item != CB_ERR)
		{
			m_ptrObj->set_attacher(
				reinterpret_cast<qdGameObjectMoving*>(attachers_.GetItemDataPtr(item)));
		}	
	}
	if (controlAttach_
		||controlAttachWithoutDirRel_
		||controlAttachToActiveWithMoving_)
		m_ptrObj->set_attach_shift(Vect2s(attacherOffsetX_, attacherOffsetY_));

	m_ptrObj->set_rotation_angle_per_quant(m_fRotationSpeed);
//	m_ptrObj->set_movement_type(get_movement_type());
}

void PersonageControlPropPage::reset_control(BOOL bSetFlag, int iSetValue)
{
	if (bSetFlag) 
		m_ptrObj->add_control_type(
			static_cast<qdGameObjectMoving::control_type_t>(iSetValue));
	else 
		m_ptrObj->remove_control_type(
			static_cast<qdGameObjectMoving::control_type_t>(iSetValue));
}
void PersonageControlPropPage::obj2wnd()
{
	m_bMouseControl = m_ptrObj->has_control_type(qdGameObjectMoving::CONTROL_MOUSE);
	m_bKeyboardControl = 
		m_ptrObj->has_control_type(qdGameObjectMoving::CONTROL_KEYBOARD);
	m_bCollisionControl = 
		m_ptrObj->has_control_type(qdGameObjectMoving::CONTROL_COLLISION);
	m_bAvoidCollision = 
		m_ptrObj->has_control_type(qdGameObjectMoving::CONTROL_AVOID_COLLISION);
	m_bAutoMove = 
		m_ptrObj->has_control_type(qdGameObjectMoving::CONTROL_AUTO_MOVE);
	clearPath_ = m_ptrObj->has_control_type(qdGameObjectMoving::CONTROL_CLEAR_PATH);
	followActivePers_ = 
		m_ptrObj->has_control_type(qdGameObjectMoving::CONTROL_FOLLOW_ACTIVE_PERSONAGE);
	repeatActivePersMovement_ = 
		m_ptrObj->has_control_type(
			qdGameObjectMoving::CONTROL_REPEAT_ACTIVE_PERSONAGE_MOVEMENT);

	controlAttachToActiveWithMoving_ = 
		m_ptrObj->has_control_type(
			qdGameObjectMoving::CONTROL_ATTACHMENT_TO_ACTIVE_WITH_MOVING);

	m_bActiveClickReacting = 
		m_ptrObj->has_control_type(qdGameObjectMoving::CONTROL_ACTIVE_CLICK_REACTING);
	m_bAnimatedRotation =
		m_ptrObj->has_control_type(qdGameObjectMoving::CONTROL_ANIMATED_ROTATION);


	GetDlgItem(IDC_EDIT_FOLLOW_RADIUS_MIN)->EnableWindow(followActivePers_);
	GetDlgItem(IDC_EDIT_FOLLOW_RADIUS_MAX)->EnableWindow(followActivePers_);
	GetDlgItem(IDC_SPIN_FOLLOW_RADIUS_MIN)->EnableWindow(followActivePers_);
	GetDlgItem(IDC_SPIN_FOLLOW_RADIUS_MAX)->EnableWindow(followActivePers_);


	followRadiusMin_	= m_ptrObj->follow_min_radius();
	followRadiusMax_	= m_ptrObj->follow_max_radius();


	m_fCollisionRadius	= m_ptrObj->collision_radius();
	m_fCollisionDelay	= m_ptrObj->collision_delay();
	m_fCollisionPath	= m_ptrObj->collision_path();

	attacherOffsetX_ = m_ptrObj->attach_shift().x;
	attacherOffsetY_ = m_ptrObj->attach_shift().y;

	if (GetDlgItem(IDC_CHECK_CONTROL_ATTACHMENT)->IsWindowEnabled())
	{
		controlAttach_ = 
			m_ptrObj->has_control_type(qdGameObjectMoving::CONTROL_ATTACHMENT_WITH_DIR_REL);
		controlAttachWithoutDirRel_ =
			m_ptrObj->has_control_type(
								qdGameObjectMoving::CONTROL_ATTACHMENT_WITHOUT_DIR_REL);
		if (controlAttach_||controlAttachWithoutDirRel_)
		{
			if (m_ptrObj->attacher())
				attachers_.SelectString(-1, m_ptrObj->attacher()->name());
		}
		else
			EnableAttacherControls(FALSE);
	}
	if (controlAttachToActiveWithMoving_)
		EnableOffsetCtrls(controlAttachToActiveWithMoving_);

	if (m_bAnimatedRotation)
		m_fRotationSpeed = m_ptrObj->rotation_angle_per_quant();
	GetDlgItem(IDC_EDIT_ROTATION_SPEED)->EnableWindow(m_bAnimatedRotation);
	GetDlgItem(IDC_SPIN_ROTATION_SPEED)->EnableWindow(m_bAnimatedRotation);
}

void PersonageControlPropPage::OnDeltaposSpinRadius(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
		return;
	m_fCollisionRadius += pNMUpDown->iDelta/100.f;
	if (m_fCollisionRadius < 0)
		m_fCollisionRadius = 0;
	UpdateData(FALSE);
	SetModified();
	*pResult = 0;
}

void PersonageControlPropPage::OnDeltaposSpinTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
		return;
	m_fCollisionDelay += pNMUpDown->iDelta/100.f;
	if (m_fCollisionDelay < 0)
		m_fCollisionDelay = 0;
	UpdateData(FALSE);
	SetModified();
	*pResult = 0;
}

void PersonageControlPropPage::OnBnClickedCheckMouseControl()
{
	SetModified();
}

void PersonageControlPropPage::OnBnClickedCheckKeyboardControl()
{
	SetModified();
}

void PersonageControlPropPage::OnBnClickedCheckCollisionControl()
{
	SetModified();
}

BOOL PersonageControlPropPage::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN ) 
	{
		CWnd* pfocus = GetFocus();
		if (pfocus->m_hWnd == ::GetDlgItem(*this, IDC_EDIT_RADIUS)
			||pfocus->m_hWnd == ::GetDlgItem(*this, IDC_EDIT_TIME)
			||pfocus->m_hWnd == ::GetDlgItem(*this, IDC_EDIT_FOLLOW_RADIUS_MIN)
			||pfocus->m_hWnd == ::GetDlgItem(*this, IDC_EDIT_FOLLOW_RADIUS_MAX)
			||pfocus->m_hWnd == ::GetDlgItem(*this, IDC_EDIT_ATTACHER_OFFSET_X)
			||pfocus->m_hWnd == ::GetDlgItem(*this, IDC_EDIT_ATTACHER_OFFSET_Y)
			||pfocus->m_hWnd == ::GetDlgItem(*this, IDC_EDIT_ROTATION_SPEED)) 
		{
			if (UpdateData())
			{
				SetModified();
			}
			return TRUE;
		}
	}
	return CBasePage::PreTranslateMessage(pMsg);
}

void PersonageControlPropPage::OnDeltaposSpinCollisionPath(NMHDR *pNMHDR, 
														   LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
		return;
	m_fCollisionPath += pNMUpDown->iDelta/2.f;
	if (m_fCollisionPath < 0)
		m_fCollisionPath = 0;
	UpdateData(FALSE);
	SetModified();
	*pResult = 0;
}

void PersonageControlPropPage::OnBnClickedAnyCheck()
{
	SetModified();
}

void PersonageControlPropPage::load_movement_types()
{
	m_wndMovementModes.ResetContent();
	size_t const count = m_ptrMovementTypeManager->size();
	for(size_t i = 0; i < count; ++i)
	{
		int const iItem = 
			m_wndMovementModes.AddString((*m_ptrMovementTypeManager)[i]->description());
		if (iItem != CB_ERR)
			m_wndMovementModes.SetItemDataPtr(iItem,(*m_ptrMovementTypeManager)[i]);
	}
	m_wndMovementModes.SelectString(-1, 
		(*m_ptrMovementTypeManager)[m_ptrObj->movement_type()]->description());
}

void PersonageControlPropPage::OnCbnSelchangeComboMovementMode()
{
	SetModified();
}

qdGameObjectStateWalk::movement_type_t PersonageControlPropPage::get_movement_type()const
{
	int const iCurSel = m_wndMovementModes.GetCurSel();
	ASSERT(iCurSel != CB_ERR);
	return reinterpret_cast<MovementType*>(
			m_wndMovementModes.GetItemDataPtr(iCurSel))->movement_type();
}

void PersonageControlPropPage::OnDeltaposSpinFollowRadiusMin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
		return;

	followRadiusMin_ += pNMUpDown->iDelta/10.f;
	if (followRadiusMin_ < 0.f)
		followRadiusMin_ = 0.f;
	if (followRadiusMin_ > followRadiusMax_)
		followRadiusMax_ = followRadiusMin_;

	UpdateData(FALSE);
	SetModified();
	*pResult = 0;
}

void PersonageControlPropPage::OnDeltaposSpinFollowRadiusMax(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
		return;

	followRadiusMax_ += pNMUpDown->iDelta/10.f;
	if (followRadiusMax_ < 0.f)
		followRadiusMax_ = 0.f;
	if (followRadiusMin_ > followRadiusMax_)
		followRadiusMin_ = followRadiusMax_;

	UpdateData(FALSE);
	SetModified();
	*pResult = 0;
}

void PersonageControlPropPage::OnBnClickedCheckFollowActivePersonage()
{
	UpdateData();

	GetDlgItem(IDC_EDIT_FOLLOW_RADIUS_MIN)->EnableWindow(followActivePers_);
	GetDlgItem(IDC_EDIT_FOLLOW_RADIUS_MAX)->EnableWindow(followActivePers_);
	GetDlgItem(IDC_SPIN_FOLLOW_RADIUS_MIN)->EnableWindow(followActivePers_);
	GetDlgItem(IDC_SPIN_FOLLOW_RADIUS_MAX)->EnableWindow(followActivePers_);
	SetModified();
}

//! Загружает список пероснажей, исключая тот который редактируем
void PersonageControlPropPage::loadPersonages()
{
	qdGameScene* scene = static_cast<qdGameScene*>(m_ptrObj->owner());
	ASSERT(scene);
	boost::rtl::filtered_range<
		qdGameObjectList const,
		bool (*)(qdNamedObject const*)
	> r(scene->object_list(), ptree_helpers::is_obj_moving);
	for(; r; ++r)
	{
		qdGameObjectMoving* const moving = 
			static_cast<qdGameObjectMoving *>(*r);
		if (moving != m_ptrObj)
		{
			int const item = attachers_.AddString(moving->name());
			if (item != CB_ERR)
				attachers_.SetItemDataPtr(item, moving);
		}
	}
	if (!attachers_.GetCount())
	{
		CString str;
		str.LoadString(IDS_NOTHING);
		attachers_.AddString(str);
		EnableAttacherControls(FALSE);
		if (controlAttachToActiveWithMoving_)
			EnableOffsetCtrls(controlAttachToActiveWithMoving_);
	}
	attachers_.SetCurSel(0);
}

void PersonageControlPropPage::EnableAttacherControls(BOOL bEnable)
{
	attachers_.EnableWindow(bEnable);
	EnableOffsetCtrls(bEnable);
}

void PersonageControlPropPage::OnBnClickedCheckControlAttachment()
{
	if (!UpdateData())
		return;

	SetModified();
	EnableAttacherControls(controlAttach_||controlAttachWithoutDirRel_);
	if (controlAttachToActiveWithMoving_)
		EnableOffsetCtrls(controlAttachToActiveWithMoving_);
}

void PersonageControlPropPage::OnDeltaposSpinAttacherOffset(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	SetModified();
	*pResult = 0;
}

void PersonageControlPropPage::OnCbnSelchangeComboAttacher()
{
	if (!UpdateData())
		return;
	SetModified();
}

void PersonageControlPropPage::OnBnClickedCheckControlAttachmentToActiveWithMoving()
{
	if (!UpdateData())
		return;
	SetModified();
	EnableOffsetCtrls(controlAttachToActiveWithMoving_);
}

void PersonageControlPropPage::OnBnClickedCheckControlAttachmentWithoutDirRel()
{
	if (!UpdateData())
		return;

	SetModified();
	EnableAttacherControls(controlAttach_||controlAttachWithoutDirRel_);
	if (controlAttachToActiveWithMoving_)
		EnableOffsetCtrls(controlAttachToActiveWithMoving_);
}

void PersonageControlPropPage::EnableOffsetCtrls(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_ATTACHER_OFFSET_X)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_ATTACHER_OFFSET_Y)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPIN_ATTACHER_OFFSET_X)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPIN_ATTACHER_OFFSET_Y)->EnableWindow(bEnable);
}

void PersonageControlPropPage::OnBnClickedCheckActiveClickReacting()
{
	SetModified();
}

void PersonageControlPropPage::OnBnClickedCheckAnimatedRotation()
{
	UpdateData();

	GetDlgItem(IDC_EDIT_ROTATION_SPEED)->EnableWindow(m_bAnimatedRotation);
	GetDlgItem(IDC_SPIN_ROTATION_SPEED)->EnableWindow(m_bAnimatedRotation);
	SetModified();
}

void PersonageControlPropPage::OnDeltaposSpinRotationSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
		return;
	m_fRotationSpeed += pNMUpDown->iDelta/10.f;
	if (m_fRotationSpeed < 0.1f)
		m_fRotationSpeed = 0.1f;
	UpdateData(FALSE);
	SetModified();
	*pResult = 0;
}

