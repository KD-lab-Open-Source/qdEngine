// CounterLGValue.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "CounterLGValue.h"


// CounterLGValue dialog

IMPLEMENT_DYNAMIC(CounterLGValue, CounterCondBase)
CounterLGValue::CounterLGValue(DWORD condId, CWnd* pParent /*=NULL*/)
	: CounterCondBase(CounterLGValue::IDD, condId, IDC_COMBO_COUNTER)
	, m_iValue(0)
	, m_bInitDialog(true)
{
}

CounterLGValue::~CounterLGValue()
{
}

void CounterLGValue::DoDataExchange(CDataExchange* pDX)
{
	CounterCondBase::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_iValue);
}


BEGIN_MESSAGE_MAP(CounterLGValue, CounterCondBase)
	ON_EN_CHANGE(IDC_EDIT_VALUE, OnEnChangeEditValue)
END_MESSAGE_MAP()


BOOL CounterLGValue::OnInitDialog()
{
	CounterCondBase::OnInitDialog();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_VALUE))->
				SetRange32(INT_MIN,INT_MAX);
	m_bInitDialog = false;
	return TRUE;  
}

void CounterLGValue::OnEnChangeEditValue()
{
	if (m_bInitDialog)
		return;
	if (!UpdateData())
		return;
	m_qdCondition.put_value(0, m_iValue, 0);
}

bool CounterLGValue::IsCondDataComplete()
{
	return (m_qdCondition.get_object(0) != NULL);
}
