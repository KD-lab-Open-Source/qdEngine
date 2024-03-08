// KeyPressedCondDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "KeyPressedCondDlg.h"


// KeyPressedCondDlg dialog

IMPLEMENT_DYNAMIC(KeyPressedCondDlg, CondDlgBase)
KeyPressedCondDlg::KeyPressedCondDlg(DWORD condId, CWnd* pParent /*=NULL*/)
	: CondDlgBase(KeyPressedCondDlg::IDD, condId, pParent)
{
}

KeyPressedCondDlg::~KeyPressedCondDlg()
{
}

void KeyPressedCondDlg::DoDataExchange(CDataExchange* pDX)
{
	CondDlgBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KeyPressedCondDlg, CondDlgBase)
END_MESSAGE_MAP()


// KeyPressedCondDlg message handlers

BOOL KeyPressedCondDlg::OnInitDialog()
{
	BOOL res = CondDlgBase::OnInitDialog();

	return TRUE;
}

bool KeyPressedCondDlg::IsCondDataComplete()
{
	int vKey = 0;
	m_qdCondition.get_value(0, vKey, 0);
	return (vKey != 0);
}

BOOL KeyPressedCondDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&GetFocus()==GetDlgItem(IDC_EDIT_KEY))
	{
		int const BUFF_SIZE = 32;
		TCHAR buff[BUFF_SIZE];
		LONG skey = static_cast<LONG>(MapVirtualKey(pMsg->wParam, 0))<<16;
		if (GetKeyNameText(skey, buff, BUFF_SIZE))
			m_qdCondition.put_value(0, static_cast<int>(pMsg->wParam));
		else
			m_qdCondition.put_value(0, 0);

		GetDlgItem(IDC_EDIT_KEY)->SetWindowText(buff);
		
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
		return TRUE;
	}

	return CondDlgBase::PreTranslateMessage(pMsg);
}
