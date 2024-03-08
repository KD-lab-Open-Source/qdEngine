// TimerCond.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "TimerCond.h"
#include "MakeCondDlgBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TimerCond dialog
const int SPIN_MAX = 10000;
const int SPIN_MIN = 0;

TimerCond::TimerCond(DWORD id, CWnd* pParent /*=NULL*/)
	: CondDlgBase(TimerCond::IDD, id, pParent)
{
	//{{AFX_DATA_INIT(TimerCond)
	m_fDuration = 1.f;
	m_iRnd = 100;
	//}}AFX_DATA_INIT
	m_qdCondition.put_value(qdCondition::TIMER_PERIOD, m_fDuration, 0);
	m_qdCondition.put_value(qdCondition::TIMER_RND, m_iRnd, 0);

	m_bInitDialog = true;
}


void TimerCond::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TimerCond)
	DDX_Text(pDX, IDC_EDIT_DURATION, m_fDuration);
	DDX_Text(pDX, IDC_EDIT_RND, m_iRnd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TimerCond, CDialog)
	//{{AFX_MSG_MAP(TimerCond)
	ON_EN_CHANGE(IDC_EDIT_RND, OnChangeEditRnd)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DURATION, OnDeltaposSpinDuration)
	ON_EN_CHANGE(IDC_EDIT_DURATION, OnEnChangeEditDuration)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TimerCond message handlers

BOOL TimerCond::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CondToWnd();	
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_DURATION))->
		SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_RND))->SetRange(0, 100);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void TimerCond::OnEnChangeEditDuration()
{
	if(m_bInitDialog)
		return;
	if (!UpdateData()) {
		return;
	}
	m_qdCondition.put_value(qdCondition::TIMER_PERIOD, m_fDuration, 0);
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void TimerCond::OnChangeEditRnd() 
{
	if(m_bInitDialog)
		return;
	if (!UpdateData()) {
		return;
	}
	m_qdCondition.put_value(qdCondition::TIMER_RND, m_iRnd, 0);
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

bool TimerCond::IsCondDataComplete()
{
	float fPeriod = 0;
	m_qdCondition.get_value(qdCondition::TIMER_PERIOD, fPeriod, 0);
	return (fPeriod != 0.f);
}

void TimerCond::CondToWnd()
{
	m_qdCondition.get_value(qdCondition::TIMER_PERIOD, m_fDuration, 0);

	m_qdCondition.get_value(qdCondition::TIMER_RND, m_iRnd, 0);

	UpdateData(FALSE);
}

void TimerCond::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	m_bInitDialog = false;
	if(bShow)
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void TimerCond::OnDeltaposSpinDuration(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if(!UpdateData()){
		*pResult = 1;
		return;
	}
	m_fDuration += pNMUpDown->iDelta/100.f;
	if (m_fDuration < 0.f)
		m_fDuration = 0.f;
	m_qdCondition.put_value(qdCondition::TIMER_PERIOD, m_fDuration, 0);
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
	UpdateData(FALSE);

	*pResult = 0;
}
