// WTGreaterValue.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "WTGreaterValue.h"


// WTGreaterValue dialog

const UINT MAX_VALUE = 100;
IMPLEMENT_DYNAMIC(WTGreaterValue, WTCondDlgBase);

WTGreaterValue::WTGreaterValue(DWORD condId, CWnd* pParent /*=NULL*/)
	: WTCondDlgBase(
				WTGreaterValue::IDD, 
				condId, 
				IDC_COMBO_STATES, 
				IDC_COMBO_STATES_SCENES, IDC_STATIC_SCENE)
	, m_fValue(0.f)
{
}

WTGreaterValue::~WTGreaterValue()
{
}

void WTGreaterValue::DoDataExchange(CDataExchange* pDX)
{
	WTCondDlgBase::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_fValue);
}


BEGIN_MESSAGE_MAP(WTGreaterValue, WTCondDlgBase)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VALUE, OnDeltaposSpinValue)
END_MESSAGE_MAP()


// WTGreaterValue message handlers

BOOL WTGreaterValue::OnInitDialog()
{
	WTCondDlgBase::OnInitDialog();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_VALUE))->SetRange(1, MAX_VALUE);
	return TRUE;  
}

bool WTGreaterValue::IsCondDataComplete()
{
	return (m_qdCondition.get_object(0) != NULL);
}

void WTGreaterValue::OnDeltaposSpinValue(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	if(!UpdateData()){
		*pResult = 1;
		return;
	}
	m_fValue += pNMUpDown->iDelta/static_cast<float>(MAX_VALUE);
	if (m_fValue < 0.f)
		m_fValue = 0.f;
	m_qdCondition.put_value(0, m_fValue);
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
	UpdateData(FALSE);
	
	*pResult = 0;
}
