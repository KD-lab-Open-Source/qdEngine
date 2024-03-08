// CounterInIntervalCondDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "CounterInIntervalCondDlg.h"


// CounterInIntervalCondDlg dialog

IMPLEMENT_DYNAMIC(CounterInIntervalCondDlg, CounterCondBase)
CounterInIntervalCondDlg::CounterInIntervalCondDlg(DWORD condId, CWnd* pParent /*=NULL*/)
	: CounterCondBase(CounterInIntervalCondDlg::IDD, condId, IDC_COMBO_COUNTER)
	, m_bInitDialog(true)
	, m_iLessValue(0)
	, m_iGrValue(1)
{
	m_qdCondition.put_value(0, m_iLessValue, 0);
	m_qdCondition.put_value(0, m_iGrValue, 1);
}

CounterInIntervalCondDlg::~CounterInIntervalCondDlg()
{
}

void CounterInIntervalCondDlg::DoDataExchange(CDataExchange* pDX)
{
	CounterCondBase::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_iLessValue);
	DDX_Text(pDX, IDC_EDIT_GR_VALUE, m_iGrValue);
	DDV_MinMaxInt(pDX, m_iLessValue, INT_MIN, m_iGrValue);
	DDV_MinMaxInt(pDX, m_iGrValue, m_iLessValue, INT_MAX);
}


BEGIN_MESSAGE_MAP(CounterInIntervalCondDlg, CounterCondBase)
	ON_EN_CHANGE(IDC_EDIT_VALUE, OnEnChangeEditValue)
	ON_EN_CHANGE(IDC_EDIT_GR_VALUE, OnEnChangeEditGrValue)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GR_VALUE, OnDeltaposSpinGrValue)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VALUE, OnDeltaposSpinValue)
END_MESSAGE_MAP()


// CounterInIntervalCondDlg message handlers

BOOL CounterInIntervalCondDlg::OnInitDialog()
{
	CounterCondBase::OnInitDialog();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_VALUE))->
		SetRange32(INT_MIN,INT_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_GR_VALUE))->
		SetRange32(INT_MIN,INT_MAX);
	m_bInitDialog = false;
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CounterInIntervalCondDlg::OnEnChangeEditValue()
{
	if (m_bInitDialog)
		return;
	if (!UpdateData())
		return;

	m_qdCondition.put_value(0, m_iLessValue, 0);
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void CounterInIntervalCondDlg::OnEnChangeEditGrValue()
{
	if (m_bInitDialog)
		return;
	if (!UpdateData())
		return;

	m_qdCondition.put_value(0, m_iGrValue, 1);
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

bool CounterInIntervalCondDlg::IsCondDataComplete()
{
	if (!m_qdCondition.get_object(0))
		return false;
	int iMinValue = 0;
	int iMaxValue = 0;
	m_qdCondition.get_value(0, iMinValue, 0);
	m_qdCondition.get_value(0, iMaxValue, 1);
	return (iMinValue <= iMaxValue);
}
void CounterInIntervalCondDlg::OnDeltaposSpinGrValue(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (m_iLessValue > m_iGrValue + pNMUpDown->iDelta)
		*pResult = 1;
	else
		*pResult = 0;
}

void CounterInIntervalCondDlg::OnDeltaposSpinValue(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (m_iLessValue + pNMUpDown->iDelta > m_iGrValue)
		*pResult = 1;
	else
		*pResult = 0;
}
