// CoordAnimDlg.cpp : implementation file
//

#include "stdafx.h"
#include <additional/addition.h>

#include "questeditor.h"
#include "CoordAnimDlg.h"
#include "childview.h"
#include "sv_api.h"
#include "ptree_api.h"
#include "qd_game_object_animated.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CoordAnimDlg dialog

/// Константы задающие границы Spin контролов
const int WND_WIDTH				= 250;
const int WND_COLLAPSED_HEIGHT	= 158;
const int WND_FULL_HEIGHT		= 200;

const DWORD PHASA_SPIN_MAX	= 100;

const int Z_SPIN_MIN		= -30000;
const int Z_SPIN_MAX		= 30000;

const int DIR_SPIN_MIN		= 0;
const int DIR_SPIN_MAX		= 359;

/// Возвращает владельца КА
inline qdGameObjectAnimated* get_owner_obj(qdCoordsAnimation* ptrAnim){
	return static_cast<qdGameObjectAnimated*>(ptrAnim->owner()->owner());
}

/// Является ли точка последней по порядку
inline bool is_last_point(const qdCoordsAnimation* p, int iNum){
	return (!p->is_empty()&&(iNum == p->size()-1));
}

CoordAnimDlg::CoordAnimDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CoordAnimDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CoordAnimDlg)
	m_iZ = 0;
	m_iCurPoint = 0;
	m_iPhase = 0;
	m_bUseDir = FALSE;
	m_iDir = 0;
	//}}AFX_DATA_INIT
	m_ptrView = NULL;
	m_ptrEditedCA = NULL;
	m_bInitDialog = true;
}

void CoordAnimDlg::SetView(ISceneViewFacade* ptrView)
{
	ASSERT(m_ptrSourceCA);//должна быть уже установлна
	m_ptrView = ptrView;
	m_ptrView->BeginEditCoordAnim(m_ptrSourceCA);
	m_ptrEditedCA = &m_ptrView->GetCoordsAnimation();
}

void CoordAnimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CoordAnimDlg)
	DDX_Text(pDX, IDC_EDIT_Z, m_iZ);
	DDX_Text(pDX, IDC_EDT_CUR_POINT, m_iCurPoint);
	DDX_Text(pDX, IDC_EDT_PHASE, m_iPhase);
	DDX_Check(pDX, IDC_CHECK_USE_ORIENTATION, m_bUseDir);
	DDX_Text(pDX, IDC_EDIT_DIRECTION, m_iDir);
	DDV_MinMaxInt(pDX, m_iDir, 0, 359);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CoordAnimDlg, CDialog)
	//{{AFX_MSG_MAP(CoordAnimDlg)
	ON_EN_CHANGE(IDC_EDT_CUR_POINT, OnChangeEdtCurPoint)
	ON_EN_CHANGE(IDC_EDT_PHASE, OnChangeEdtPhase)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_EDIT_Z, OnChangeEditZ)
	ON_BN_CLICKED(IDC_CHECK_USE_ORIENTATION, OnCheckUseOrientation)
	ON_EN_CHANGE(IDC_EDIT_DIRECTION, OnChangeEditDirection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CoordAnimDlg message handlers

void CoordAnimDlg::OnOK() 
{
	CDialog::OnOK();
	ptree_api::EndEditCoordAnimationMode(ptree_api::EESTM_OK);
}

void CoordAnimDlg::OnCancel() 
{
	CDialog::OnCancel();
	ptree_api::EndEditCoordAnimationMode(ptree_api::EESTM_CANCEL);
}

void CoordAnimDlg::OnChangeCurrPoint()
{
	CurPointToWnd();
}

BOOL CoordAnimDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UDACCEL ua[3];
	ua[0].nInc = 1;
	ua[0].nSec = 0;
	
	ua[1].nInc = 4;
	ua[1].nSec = 2;
	
	ua[2].nInc = 10;
	ua[2].nSec = 4;
	
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_PHASA))->SetAccel(3, ua);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_PHASA))
											->SetRange(0, PHASA_SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Z))
											->SetRange(Z_SPIN_MIN, Z_SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_DIR))
							->SetRange(DIR_SPIN_MIN, DIR_SPIN_MAX);
	CheckZAccess();

	CurPointToWnd();

	m_iPhase = GetCurrAnimFrame();

	m_bInitDialog = false;	

	return TRUE;  
}

void CoordAnimDlg::OnChangeEditZ() 
{
	if(m_bInitDialog||!UpdateData())
		return;
	Vect3f pos = m_ptrEditedCA->get_point(m_ptrEditedCA->get_cur_point())->dest_pos();
	pos.z = static_cast<float>(m_iZ);
	m_ptrEditedCA->get_point(m_ptrEditedCA->get_cur_point())->set_dest_pos(pos);
	m_ptrView->UpdateCoordsAnimation();
}

void CoordAnimDlg::OnChangeEdtCurPoint() 
{
	if(m_bInitDialog||!UpdateData())
		return;
	m_ptrEditedCA->set_cur_point(m_iCurPoint);
	CurPointToWnd();
	m_ptrView->UpdateCoordsAnimation();
}

void CoordAnimDlg::OnChangeEdtPhase() 
{
	int iOld = m_iPhase;
	if(m_bInitDialog||!UpdateData())
		return;
	
	m_ptrEditedCA->change_animation_frame((m_iPhase - iOld) > 0);
	m_ptrEditedCA->set_cur_point(m_iCurPoint);
	CurPointToWnd();
	m_ptrView->UpdateCoordsAnimation();
}

void CoordAnimDlg::OnChangeEditDirection() 
{
	if(m_bInitDialog||!UpdateData())
		return;
	m_ptrEditedCA->get_point(m_iCurPoint)->set_direction_angle(m_iDir*M_PI/180.f);
	m_ptrView->UpdateCoordsAnimation();
}

void CoordAnimDlg::CurPointToWnd()
{
	if (m_ptrEditedCA->is_empty()) {
		GetDlgItem(IDC_EDT_CUR_POINT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_CUR_POINT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_PHASA)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_PHASE)->EnableWindow(FALSE);
		
		GetDlgItem(IDC_EDIT_DIRECTION)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_DIR)->EnableWindow(FALSE);

		m_iCurPoint = -1;
		static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_CUR_POINT))->SetRange(0, 0);
	}
	else
	{
		m_iCurPoint = m_ptrEditedCA->get_cur_point();

		qdCoordsAnimationPoint *p = m_ptrEditedCA->get_point(m_iCurPoint);
		ASSERT(p);

		m_iPhase = GetCurrAnimFrame();

		if (m_ptrSourceCA->type() != qdCoordsAnimation::CA_WALK) 
		{
			m_iZ = static_cast<int>(p->dest_pos().z);
			GetDlgItem(IDC_SPIN_Z)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_Z)->EnableWindow(TRUE);
		}
		GetDlgItem(IDC_EDT_CUR_POINT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_CUR_POINT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_PHASA)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_PHASE)->EnableWindow(TRUE);
		
		static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_CUR_POINT))->
			SetRange(0, m_ptrEditedCA->size()-1);

		m_bUseDir = (p->direction_angle() != qdCoordsAnimationPoint::NO_DIRECTION);
		if (m_bUseDir){
			//можно использовать направление только если это последня точка
			m_bUseDir = is_last_point(m_ptrEditedCA, m_iCurPoint);
		}
		SetDirAccess();
		if (m_bUseDir) 
			m_iDir = static_cast<int>(p->direction_angle()/M_PI*180);
		else{
			m_iDir = 0;
			p->set_direction_angle(qdCoordsAnimationPoint::NO_DIRECTION);
		}
	}
	UpdateData(FALSE);
}


void CoordAnimDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(pScrollBar->GetDlgCtrlID() == IDC_SPIN_Z
	||pScrollBar->GetDlgCtrlID() == IDC_SPIN_PHASA
	||pScrollBar->GetDlgCtrlID() == IDC_SPIN_CUR_POINT)
	{
		if(nSBCode == SB_ENDSCROLL)
		{
			m_ptrEditedCA->set_cur_point(m_iCurPoint);
			m_ptrView->UpdateCoordsAnimation();
		}
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CoordAnimDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN){
		CWnd* pFoc = CWnd::GetFocus();
		if(pFoc == GetDlgItem(IDC_EDT_CUR_POINT)
		||pFoc == GetDlgItem(IDC_EDIT_Z)
		||pFoc == GetDlgItem(IDC_EDT_PHASE))
		{
			if(pMsg->wParam == VK_RETURN)
			{
				m_ptrView->UpdateCoordsAnimation();
				return TRUE;
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

int CoordAnimDlg::GetCurrAnimFrame()
{
	qdGameObjectState* ptrState = static_cast<qdGameObjectState*>
		(m_ptrEditedCA->owner());
	qdGameObjectAnimated* ptrObj = static_cast<qdGameObjectAnimated*>
		(ptrState->owner());
	return ptrObj->get_animation()->get_cur_frame_number();
}

VOID CoordAnimDlg::EndEdit(eEndEditMode mode)
{
	if (mode == EEM_SAVE) 
	{
		CheckPointsDirection();
		if (m_ptrSourceCA->is_empty())
		{
			if (m_ptrEditedCA->owner()->owner()->named_object_type()==
				QD_NAMED_OBJECT_MOVING_OBJ)
				//Выставляем флаг, для по просьбе сборщиков
				m_ptrEditedCA->set_flag(QD_COORDS_ANM_OBJECT_START_FLAG);
		}
		*m_ptrSourceCA = *m_ptrEditedCA;
	}
	m_ptrView->EndEditCoordAnim();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

VOID CoordAnimDlg::CheckZAccess(){
	BOOL bEnable = m_ptrSourceCA->type() != qdCoordsAnimation::CA_WALK;
	GetDlgItem(IDC_EDIT_Z)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPIN_Z)->EnableWindow(bEnable);
}

VOID CoordAnimDlg::SetDirAccess(){
	GetDlgItem(IDC_EDIT_DIRECTION)->EnableWindow(m_bUseDir);
	GetDlgItem(IDC_SPIN_DIR)->EnableWindow(m_bUseDir);
	GetDlgItem(IDC_CHECK_USE_ORIENTATION)->
		EnableWindow(is_last_point(m_ptrEditedCA, m_iCurPoint));
}

VOID CoordAnimDlg::OnCheckUseOrientation() 
{
	if(!UpdateData())
		return;
	if (m_bUseDir)//не использовалось до этого
	{
		float fSrcDir = 0.f;
		//обращаемся к исходным данным, чтобы получить значение угла.
		if (m_iCurPoint < m_ptrSourceCA->size()) 
			fSrcDir = m_ptrSourceCA->get_point(m_iCurPoint)->direction_angle();

		if (fSrcDir == qdCoordsAnimationPoint::NO_DIRECTION)
			fSrcDir = 0.f;

		m_ptrEditedCA->get_point(m_iCurPoint)->set_direction_angle(fSrcDir);
		m_iDir = static_cast<int>(fSrcDir/M_PI*180);
	}
	else
	{
		m_ptrEditedCA->get_point(m_iCurPoint)->
			set_direction_angle(qdCoordsAnimationPoint::NO_DIRECTION);
		m_iDir = 0;
	}
	
	SetDirAccess();
	UpdateData(FALSE);
}

VOID CoordAnimDlg::CheckPointsDirection()
{
	for(int i = 0; i < m_ptrEditedCA->size()-1; ++i){
		qdCoordsAnimationPoint* p = m_ptrEditedCA->get_point(i);
		p->set_direction_angle(qdCoordsAnimationPoint::NO_DIRECTION);
	}
}
