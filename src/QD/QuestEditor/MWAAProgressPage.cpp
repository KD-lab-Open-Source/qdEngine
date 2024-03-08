// MWAAProgressPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "MWAAProgressPage.h"
#include "addanimwizdata.h"
#include "addanimation.h"
#include "AddAnimMWWizard.h"


// MWAAProgressPage dialog

IMPLEMENT_DYNAMIC(MWAAProgressPage, WizardBasePage)
MWAAProgressPage::MWAAProgressPage()
	: WizardBasePage(MWAAProgressPage::IDD)
{
	m_bStopPressed = false;
}

MWAAProgressPage::~MWAAProgressPage()
{
}

void MWAAProgressPage::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MWAAProgressPage)
	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
	DDX_Control(pDX, IDC_BTN_STOP, m_wndStop);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MWAAProgressPage, WizardBasePage)
//{{AFX_MSG_MAP(MWAAProgressPage)
ON_WM_SIZE()
//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_BTN_STOP, OnBnClickedBtnStop)
ON_WM_TIMER()
END_MESSAGE_MAP()


// MWAAProgressPage message handlers

BOOL MWAAProgressPage::OnInitDialog()
{
	WizardBasePage::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT MWAAProgressPage::OnWizardBack()
{

	return WizardBasePage::OnWizardBack();
}

LRESULT MWAAProgressPage::OnWizardNext()
{
	return 1;
}

void MWAAProgressPage::OnSize(UINT nType, int cx, int cy)
{
	WizardBasePage::OnSize(nType, cx, cy);

	if (IsWindow(m_wndProgress.m_hWnd)) 
	{
		const int OFFSET = 7;
		CRect r;
		GetClientRect(r);
		r.left += OFFSET;
		r.right -= OFFSET;

		CRect rcProg;
		m_wndProgress.GetWindowRect(rcProg);
		ScreenToClient(rcProg);

		CPoint pCenter = r.CenterPoint();
		r.top = pCenter.y - (rcProg.Height()>>1);
		r.bottom = pCenter.y + (rcProg.Height()>>1);
		m_wndProgress.MoveWindow(r);

		CWnd * pbtn = GetDlgItem(IDC_BTN_STOP);

		pbtn->GetClientRect(rcProg);
		pCenter = r.CenterPoint();

		rcProg.OffsetRect(pCenter-rcProg.TopLeft());

		rcProg.OffsetRect(-(rcProg.Width()>>1), 30);
		pbtn->MoveWindow(rcProg);
	}
}

void MWAAProgressPage::SetPrecents(int prec)
{
	PumpMessages();
	m_wndProgress.SetPos(prec);
}

bool MWAAProgressPage::BreakProcess()
{
	PumpMessages();
	bool bres = m_bStopPressed;
	m_bStopPressed = false;
	return bres;
}

void MWAAProgressPage::SetText(LPCTSTR lpszMsg)
{
	SetWindowText(lpszMsg);
	PumpMessages();
}

void MWAAProgressPage::PumpMessages()
{
    // Must call Create() before using the dialog
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }
}

void MWAAProgressPage::OnBnClickedBtnStop()
{
	m_bStopPressed = true;
}

/**
	Устанавливает время, по истечению которого начнется сборка.
	Отложенный запуск нужен для того, чтобы страничка смогла спокойно появиться 
	на экране
 */
BOOL MWAAProgressPage::OnSetActive()
{
	BOOL bRes = __super::OnSetActive();

	GET_PSHEET()->SetWizardButtons(0);
	CWnd* pw = GET_PSHEET()->GetDlgItem(IDCANCEL);
	if (pw)
		pw->EnableWindow(FALSE);
	SetTimer(1, 500,NULL);

	return bRes;
}

void MWAAProgressPage::set_disps(qdGameDispatcher* ptrGD, qdGameDispatcherBase* ptrDB)
{
	m_ptrDispBase = ptrDB;
	m_ptrGameDisp = ptrGD;
}

/// Запускает процесс сборки
void MWAAProgressPage::OnTimer(UINT nIDEvent)
{
	if (nIDEvent != 1)
		return;
	KillTimer(1);

	CWaitCursor __wait__;

	AddAnimation aa(this);

	AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());
	add_anim_wiz_data* pad = pw->data();

	int iAction = AAA_EXISTING;
	if (pad->add_anim_way_ == AAW_CONVERT) 
	{
		switch (pad->convert_way_)
		{
		case CW_FOLDERS:
			iAction = AAA_CONV_CATALOG;
			break;
		case CW_MOVE_TO_SCENES:
			iAction = AAA_CONV_FOR_SCENES;
			break;
		case CW_SCENE:
			iAction = AAA_CONV_FOR_SCENE;
			break;
		case CW_STRICT_FOLDER_STRUCTURE:
			iAction = AAA_CONV_FOR_SCENES_AND_CRT_USE_STRICT_FLDR_STRUCT;
			break;
		}
	}
	aa.DO(m_ptrGameDisp, m_ptrDispBase, pad->files_, 
		iAction, pad->post_add_way_ == PAW_CREATE_OBJ, pad->cadr_duration_);

	GET_PSHEET()->SetActivePage(GET_PSHEET()->GetActiveIndex()+1);
	CWnd* wnd = GET_PSHEET()->GetDlgItem(IDCANCEL);
	if (wnd)
		wnd->EnableWindow(TRUE);
	__super::OnTimer(nIDEvent);
}
