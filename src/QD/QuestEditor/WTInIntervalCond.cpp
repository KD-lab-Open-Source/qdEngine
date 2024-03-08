// WTInIntervalCond.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "WTInIntervalCond.h"


// WTInIntervalCond dialog

UINT const MAX_VALUE = 100;
IMPLEMENT_DYNAMIC(WTInIntervalCond, WTGreaterValue)
WTInIntervalCond::WTInIntervalCond(DWORD condId, CWnd* pParent /*=NULL*/)
	: WTGreaterValue(condId, pParent)
	, m_fMaxValue(0.f)
{
	m_qdCondition.put_value(0, getValue(), 0);
	m_qdCondition.put_value(0, m_fMaxValue, 1);
}

WTInIntervalCond::~WTInIntervalCond()
{
}

void WTInIntervalCond::DoDataExchange(CDataExchange* pDX)
{
	WTGreaterValue::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VALUE2, m_fMaxValue);
}


BEGIN_MESSAGE_MAP(WTInIntervalCond, WTGreaterValue)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VALUE2, OnDeltaposSpinValue2)
END_MESSAGE_MAP()


// WTInIntervalCond message handlers

BOOL WTInIntervalCond::OnInitDialog()
{
	WTGreaterValue::OnInitDialog();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_VALUE2))->SetRange(1, MAX_VALUE);

	return TRUE;  
}

void WTInIntervalCond::OnDeltaposSpinValue2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	if(!UpdateData()){
		*pResult = 1;
		return;
	}
	*pResult = 0;
	m_fMaxValue += pNMUpDown->iDelta/static_cast<float>(MAX_VALUE);
	if (m_fMaxValue < getValue())
	{
		m_fMaxValue = getValue();
		*pResult = 1;
	}
	m_qdCondition.put_value(0, m_fMaxValue, 1);
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
	UpdateData(FALSE);

}

bool WTInIntervalCond::IsCondDataComplete()
{
	if (!m_qdCondition.get_object(0))
		return false;
	float fMin = 0.f;
	float fMax = 0.f;

	m_qdCondition.get_value(0, fMin, 0);
	m_qdCondition.get_value(0, fMax, 1);

	return (fMin <= fMax);
}
