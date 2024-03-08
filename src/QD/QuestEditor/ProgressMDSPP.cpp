// ProgressMDSPP.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "ProgressMDSPP.h"
#include "DirMakeWizard.h"
#include "DirMakeWizardData.h"
#include "DirStructMaker.h"


// ProgressMDSPP dialog

IMPLEMENT_DYNAMIC(ProgressMDSPP, WizardBasePage)
ProgressMDSPP::ProgressMDSPP()
	: WizardBasePage(ProgressMDSPP::IDD)
{
	m_bStopPressed = 0;
}

ProgressMDSPP::~ProgressMDSPP()
{
}

void ProgressMDSPP::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
	DDX_Control(pDX, IDC_BTN_STOP, m_wndStop);
}


BEGIN_MESSAGE_MAP(ProgressMDSPP, WizardBasePage)
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// ProgressMDSPP message handlers

BOOL ProgressMDSPP::OnSetActive()
{
	BOOL bRes = __super::OnSetActive();

	GET_PSHEET()->SetWizardButtons(0);
	CWnd* pw = GET_PSHEET()->GetDlgItem(IDCANCEL);
	if (pw)
		pw->EnableWindow(FALSE);
	SetTimer(1, 500,NULL);

	return bRes;
}

void ProgressMDSPP::SetPrecents(int prec)
{
	m_wndProgress.SetPos(prec);
	PumpMessages();
	UpdateWindow();
}

bool ProgressMDSPP::BreakProcess()
{
	PumpMessages();
	bool bres = m_bStopPressed;
	m_bStopPressed = false;
	return bres;
}

void ProgressMDSPP::SetText(LPCTSTR lpszMsg)
{
	SetWindowText(lpszMsg);
	PumpMessages();
}

void ProgressMDSPP::PumpMessages()
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

void ProgressMDSPP::OnSize(UINT nType, int cx, int cy)
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

void ProgressMDSPP::OnTimer(UINT nIDEvent)
{
	if (nIDEvent != 1)
		return;
	KillTimer(1);

	DirStructMaker maker(this);

	DirMakeWizard* pw = static_cast<DirMakeWizard*>(wizard());
	DirMakeWizardData* pd = pw->data();

	maker.make(pd->m_strDir, pd->scenes_count);

	GET_PSHEET()->SetActivePage(GET_PSHEET()->GetActiveIndex()+1);
	CWnd* wnd = GET_PSHEET()->GetDlgItem(IDCANCEL);
	if (wnd)
		wnd->EnableWindow(TRUE);
	__super::OnTimer(nIDEvent);
}

void ProgressMDSPP::OnBnClickedBtnStop()
{
	m_bStopPressed = true;
}
