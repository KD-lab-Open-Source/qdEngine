// StateBoundDlg.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "StateBoundDlg.h"

#include "qd_game_object_state.h"
#include "qd_game_object_moving.h"

#include "sv_api.h"
#include "ptree_api.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// StateBoundDlg dialog
const int BOUND_SPIN_MIN = 1;
const int BOUND_SPIN_MAX = 100000;

//реализация находиться в CChildView
void SaveScrLayoutOnBndEdt(qdGameObjectState* pstate, qdCamera* pcam, 
						   Vect3f const& old_bnd);

bool EditStateBound(qdGameObjectState* pstate)
{
	StateBoundDlg dlg;
	dlg.set_data(pstate);
	return (dlg.DoModal() == IDOK);
}

StateBoundDlg::StateBoundDlg(CWnd* pParent /*=NULL*/)
	: CDialog(StateBoundDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(StateBoundDlg)
	m_iX = 0;
	m_iY = 0;
	m_iZ = 0;
	//}}AFX_DATA_INIT
	m_bInitDialog = true;
}

qdGameObjectState* StateBoundDlg::getEditedState() const{
	return m_pSrcState;
}

void StateBoundDlg::set_data(qdGameObjectState* pstate)
{
	ASSERT(pstate);
	m_pSrcState = pstate;
	m_ptrStateCopy.reset(m_pSrcState->clone());
	if (!m_pSrcState->has_bound()) 
	{
		m_pSrcState->set_bound(static_cast<qdGameObjectAnimated*>
			(m_pSrcState->owner())->bound());
		m_pSrcState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);
	}
}

void StateBoundDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StateBoundDlg)
	DDX_Text(pDX, IDC_EDIT_X, m_iX);
	DDX_Text(pDX, IDC_EDIT_Y, m_iY);
	DDX_Text(pDX, IDC_EDIT_Z_, m_iZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(StateBoundDlg, CDialog)
	//{{AFX_MSG_MAP(StateBoundDlg)
/*
	ON_EN_CHANGE(IDC_EDIT_X, OnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_Y, OnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_Z_, OnChangeEdit)
*/
	//}}AFX_MSG_MAP
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StateBoundDlg message handlers

BOOL StateBoundDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	state2wnd();
	
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_X))
						->SetRange32(BOUND_SPIN_MIN, BOUND_SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Y))
						->SetRange32(BOUND_SPIN_MIN, BOUND_SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Z))
						->SetRange32(BOUND_SPIN_MIN, BOUND_SPIN_MAX);
	UpdateData(FALSE);

	m_bInitDialog = false;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void StateBoundDlg::state2wnd()
{
	qdGameObjectAnimated* const pobj = 
		static_cast<qdGameObjectAnimated*>(m_pSrcState->owner());

	const Vect3f& bound = pobj->bound();
	m_iX = static_cast<int>(bound.x);
	m_iY = static_cast<int>(bound.y);
	m_iZ = static_cast<int>(bound.z);
}

void StateBoundDlg::wnd2state()
{
	Vect3f const old_bnd(m_pSrcState->bound());
	m_pSrcState->set_bound(Vect3f(static_cast<int>(m_iX),
									static_cast<int>(m_iY),
									static_cast<int>(m_iZ)));
	if (m_pSrcState->owner()->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ)
	{
//		if (!m_pSrcState->coords_animation()->is_empty())
			SaveScrLayoutOnBndEdt(m_pSrcState,
				static_cast<qdGameScene*>(m_pSrcState->owner()->owner())->get_camera(),
				old_bnd);
		static_cast<qdGameObjectMoving*>(m_pSrcState->owner())->adjust_z();
	}
}

void StateBoundDlg::OnOK() 
{
	if (!UpdateData())
		return;
	CDialog::OnOK();
	wnd2state();
	ptree_api::EndResizeState(ptree_api::EESTM_OK);
}

void StateBoundDlg::OnCancel() 
{
	CDialog::OnCancel();
	ptree_api::EndResizeState(ptree_api::EESTM_CANCEL);
}

void StateBoundDlg::EndEdit(eEndEditMode mode)
{
	if (EEM_DISCARD == mode)
		*m_pSrcState = *m_ptrStateCopy.get();
}

void StateBoundDlg::UpdateStateData()
{
	state2wnd();
	UpdateData(FALSE);
}

void StateBoundDlg::OnChangeEdit() 
{
	if(!UpdateData())
		return;
	wnd2state();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

BOOL StateBoundDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			CWnd* pfoc = GetFocus();
			if (pfoc){
				switch(pfoc->GetDlgCtrlID()) {
				case IDC_EDIT_X:
				case IDC_EDIT_Y:
				case IDC_EDIT_Z_:
					OnChangeEdit();
					return TRUE;
				}
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void StateBoundDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_ENDSCROLL){
		ASSERT(pScrollBar->GetDlgCtrlID() == IDC_SPIN_X
			||pScrollBar->GetDlgCtrlID() == IDC_SPIN_Y
			||pScrollBar->GetDlgCtrlID() == IDC_SPIN_Z);

		OnChangeEdit();
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
