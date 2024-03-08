// ChooseName.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "ChoseName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ChooseName dialog
const CString AskUserForName(const CString& strDef)
{
	ChooseName dlg;
	dlg.set_str(strDef);
	dlg.DoModal();
	return dlg.get_str();
}

bool AskUserForName(CString& strName)
{
	ChooseName dlg;
	dlg.set_str(strName);
	if(dlg.DoModal()==IDOK)
	{
		strName = dlg.get_str();
		return true;
	}
	return false;
}


ChooseName::ChooseName(CWnd* pParent /*=NULL*/)
	: CDialog(ChooseName::IDD, pParent)
{
	//{{AFX_DATA_INIT(ChooseName)
	m_strName = _T("");
	//}}AFX_DATA_INIT
}


void ChooseName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ChooseName)
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ChooseName, CDialog)
	//{{AFX_MSG_MAP(ChooseName)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ChooseName message handlers

void ChooseName::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	CEdit* ptrE = (CEdit*)GetDlgItem(IDC_EDIT_NAME);
	ptrE->SetFocus();
	ptrE->SetSel(0, -1);
}

void ChooseName::OnOK() 
{
	CString strOld = m_strName;
	UpdateData();
	if (m_strName.IsEmpty()) {
		MessageBox(CString((LPTSTR)IDS_ERR_NAME_EMPTY), CString((LPTSTR)IDS_ERR_MSG_CAPTION), MB_OK);
		m_strName = strOld;
		UpdateData(FALSE);
		return;
	}
	CDialog::OnOK();
}
