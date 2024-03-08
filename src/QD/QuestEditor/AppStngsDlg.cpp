// AppStngsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "AppStngsDlg.h"

#include "GenStngsPP.h"
#include "TESettings.h"
#include "SceneViewStngsPP.h"

#include <algorithm>
#include "object_eraser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AppStngsDlg dialog

AppStngsDlg::AppStngsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AppStngsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AppStngsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AppStngsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AppStngsDlg)
	DDX_Control(pDX, IDC_TAB, m_wndTab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AppStngsDlg, CDialog)
	//{{AFX_MSG_MAP(AppStngsDlg)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB, OnSelchangingTab)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AppStngsDlg message handlers

void AppStngsDlg::OnOK() 
{
	PageList_t::iterator _itr = m_pages.begin(), _end = m_pages.end();
	for(; _itr != _end; ++_itr)
	{
		PPPtr_t p = *_itr;
		if (!p->OnApply()) {
			int n = std::distance(m_pages.begin(), _itr);

			SelectCurPage(FALSE);
			m_wndTab.SetCurSel(n);
			SelectCurPage(TRUE);
			return;
		}
	}
	CDialog::OnOK();
}

BOOL AppStngsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InsertPages();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void AppStngsDlg::OnCancel() 
{
	
	CDialog::OnCancel();
}

bool AppStngsDlg::InsertPages()
{
/*	InsertPage(new GenSettingsPP, 0);

	m_wndTab.SetCurSel(0);
	SelectCurPage(TRUE);

	InsertPage(new TESettings, 1);

	InsertPage(new SceneViewStngsPP, 2);
*/
	PageList_t::const_iterator i = m_pages.begin(), e = m_pages.end();
	for(int num = 0; i != e; ++i, ++num)
		InsertPage(*i, num);

	m_wndTab.SetCurSel(0);
	SelectCurPage(TRUE);
	return true;
}


void AppStngsDlg::OnSelchangingTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SelectCurPage(FALSE);
	*pResult = 0;
}

void AppStngsDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SelectCurPage(TRUE);
	*pResult = 0;
}

void AppStngsDlg::SelectCurPage(BOOL bSelect)
{
	int iCurSel = m_wndTab.GetCurSel();
	PPPtr_t ptrPP = m_pages[iCurSel];
	if (bSelect) {
		ptrPP->ShowWindow(SW_SHOW);
		ptrPP->EnableWindow(TRUE);
	}
	else
	{
		ptrPP->ShowWindow(SW_HIDE);
		ptrPP->EnableWindow(FALSE);
	}
}

void AppStngsDlg::AddUser(IAppSettingsUser* puser)
{
	PPPtr_t pp(puser->getPage());
	m_pages.push_back(pp);
}

bool AppStngsDlg::InsertPage(PPPtr_t ptrPage, int iItem)
{
	CString strCap;
	PPPtr_t pGSPP ( ptrPage );
	pGSPP->Create(&m_wndTab);

	pGSPP->GetWindowText(strCap);
	pGSPP->ModifyStyle(WS_CAPTION, 0, 0);

	m_wndTab.InsertItem(iItem, strCap, 0);

	//вычисляем прямоугольник для странички
	CRect r;
	m_wndTab.GetClientRect(&r);
	m_wndTab.AdjustRect(FALSE, &r);

	pGSPP->MoveWindow(r, FALSE);

	return true;	
}
