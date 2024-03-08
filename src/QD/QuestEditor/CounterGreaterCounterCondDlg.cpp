// ounterGreaterCounterCondDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "CounterGreaterCounterCondDlg.h"
#include "qd_counter.h"

// CounterGreaterCounterCondDlg dialog

IMPLEMENT_DYNAMIC(CounterGreaterCounterCondDlg, CounterCondBase)
CounterGreaterCounterCondDlg::CounterGreaterCounterCondDlg( DWORD condId,
														   CWnd* pParent /*=NULL*/)
: CounterCondBase(CounterGreaterCounterCondDlg::IDD, condId, IDC_COMBO_FIRST_COUNTER)
{

}

CounterGreaterCounterCondDlg::~CounterGreaterCounterCondDlg()
{
}

void CounterGreaterCounterCondDlg::DoDataExchange(CDataExchange* pDX)
{
	CounterCondBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SECOND_COUNTER, m_wndCounters2);
}


BEGIN_MESSAGE_MAP(CounterGreaterCounterCondDlg, CounterCondBase)
	ON_CBN_SELCHANGE(IDC_COMBO_SECOND_COUNTER, OnCbnSelchangeComboSecondCounter)
END_MESSAGE_MAP()

BOOL CounterGreaterCounterCondDlg::OnInitDialog()
{
	BOOL res = CounterCondBase::OnInitDialog();
	res = loadCounters(m_wndCounters2);
	m_wndCounters2.SetCurSel(1);
	OnCbnSelchangeComboSecondCounter();
	return res;
}
// CounterGreaterCounterCondDlg message handlers

void CounterGreaterCounterCondDlg::OnCbnSelchangeComboSecondCounter()
{
	m_qdCondition.put_object(1, getCounterFromCombo(m_wndCounters2));
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

bool CounterGreaterCounterCondDlg::IsCondDataComplete()
{
	qdNamedObject const* const pc = m_qdCondition.get_object(0);
	qdNamedObject const* const pc1 = m_qdCondition.get_object(1);
	return (pc&&pc1&&pc != pc1);
}
