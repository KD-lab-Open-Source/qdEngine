// IELikeComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "IELikeComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// IELikeComboBox

IELikeComboBox::IELikeComboBox()
{
	m_bDelText = false;
}

IELikeComboBox::~IELikeComboBox()
{
}


BEGIN_MESSAGE_MAP(IELikeComboBox, CComboBox)
	//{{AFX_MSG_MAP(IELikeComboBox)
	ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditupdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// IELikeComboBox message handlers

BOOL IELikeComboBox::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_DELETE || pMsg->wParam == VK_BACK) {
			m_bDelText = true;
		}
		else 
			m_bDelText = false;
	}
	return CComboBox::PreTranslateMessage(pMsg);
}


void IELikeComboBox::OnEditupdate() 
{
	CString strText;
	GetWindowText(strText);
	
	int iStr = FindString(-1, strText);
	if(iStr != -1)
	{
		int l = strText.GetLength();
		if(!m_bDelText)
		{
			CString str;
			GetLBText(iStr, str);
			strText += str.Mid(l);

			if(!GetDroppedState())
			{
				ShowDropDown(TRUE);
			}
			SetWindowText(strText);
			SetEditSel(l, -1);
		}
	}
	else
	{
		if(GetDroppedState())
		{
			ShowDropDown(FALSE);
		}
	}
}
