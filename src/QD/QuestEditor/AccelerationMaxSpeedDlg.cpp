// AccelerationMaxSpeedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "AccelerationMaxSpeedDlg.h"
#include "qd_game_object_state.h"

bool SetAccelerationAndMaxSpeed(qdGameObjectStateWalk* ps)
{
	AccelerationMaxSpeedDlg dlg;
	dlg.set_data(ps);
	
	return (dlg.DoModal() == IDOK);
}

// AccelerationMaxSpeedDlg dialog

IMPLEMENT_DYNAMIC(AccelerationMaxSpeedDlg, CDialog)
AccelerationMaxSpeedDlg::AccelerationMaxSpeedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AccelerationMaxSpeedDlg::IDD, pParent)
	, m_fAcceleration(0.f)
	, m_fMaxSpeed(0.f)
	, m_ptrState(NULL)
{
}

AccelerationMaxSpeedDlg::~AccelerationMaxSpeedDlg()
{
}

void AccelerationMaxSpeedDlg::set_data(qdGameObjectStateWalk* pstate){
	m_ptrState = pstate;
}
void AccelerationMaxSpeedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_fAcceleration);
	DDX_Text(pDX, IDC_EDIT2, m_fMaxSpeed);
}


BEGIN_MESSAGE_MAP(AccelerationMaxSpeedDlg, CDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ACCELERATION, OnDeltaposSpinAcceleration)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAX_SPEED, OnDeltaposSpinMaxSpeed)
END_MESSAGE_MAP()


// AccelerationMaxSpeedDlg message handlers

void AccelerationMaxSpeedDlg::OnDeltaposSpinAcceleration(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}
	m_fAcceleration += pNMUpDown->iDelta / 100.f;
	if (fabs(m_fAcceleration) < 1e-6) 
		m_fAcceleration = 0.f;
	UpdateData(FALSE);
	*pResult = 0;
}

void AccelerationMaxSpeedDlg::OnDeltaposSpinMaxSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}
	m_fMaxSpeed += pNMUpDown->iDelta / 100.f;
	if (m_fMaxSpeed < 0.f)
		m_fMaxSpeed = 0.f;
	UpdateData(FALSE);
	*pResult = 0;
}

BOOL AccelerationMaxSpeedDlg::PreTranslateMessage(MSG* pmsg)
{
	if (pmsg->message == WM_KEYDOWN && pmsg->wParam == VK_RETURN)
	{
		CWnd* const pwnd = GetFocus();
		if (::GetDlgItem(m_hWnd, IDC_EDIT1) == pwnd->m_hWnd
			||::GetDlgItem(m_hWnd, IDC_EDIT2) == pwnd->m_hWnd)
		{
			UpdateData();
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pmsg);
}

BOOL AccelerationMaxSpeedDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_ACCELERATION))->SetRange(1, 100);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_MAX_SPEED))->SetRange(1, 100);
	m_fMaxSpeed = m_ptrState->max_speed();
	m_fAcceleration = m_ptrState->acceleration();
	UpdateData(FALSE);
	return TRUE;
}

void AccelerationMaxSpeedDlg::OnOK()
{
	if (!UpdateData())
		return;
	m_ptrState->set_acceleration(m_fAcceleration);
	m_ptrState->set_max_speed(m_fMaxSpeed);
	CDialog::OnOK();
}