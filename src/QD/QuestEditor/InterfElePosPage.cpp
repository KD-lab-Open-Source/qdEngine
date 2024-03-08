// InterfElePosPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "InterfElePosPage.h"

#include "qd_interface_element.h"
#include "qd_interface_screen.h"

// InterfElePosPage dialog

IMPLEMENT_DYNAMIC(InterfElePosPage, CBasePage)
InterfElePosPage::InterfElePosPage(UINT dlgId)
: CBasePage(dlgId)
, m_bLeftTop(FALSE)
, m_iX(0)
, m_iY(0)
, m_iD(0)
, m_iXBak(0)
, m_iYBak(0)
, m_iDBak(0)
, m_ptrIDisp(NULL)
, m_ptrEle(NULL)
{

}
InterfElePosPage::InterfElePosPage()
	: CBasePage(InterfElePosPage::IDD)
	, m_bLeftTop(FALSE)
	, m_iX(0)
	, m_iY(0)
	, m_iD(0)
	, m_iXBak(0)
	, m_iYBak(0)
	, m_iDBak(0)
	, m_ptrIDisp(NULL)
	, m_ptrEle(NULL)
{
}

void InterfElePosPage::set_data(qdInterfaceDispatcher* pd, qdInterfaceElement* ptrEle)
{
	m_ptrIDisp = pd;
	m_ptrEle = ptrEle;
}

InterfElePosPage::~InterfElePosPage()
{
}

void InterfElePosPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_LEFT_TOP, m_bLeftTop);
	DDX_Text(pDX, IDC_EDIT_X, m_iX);
	DDX_Text(pDX, IDC_EDIT_X2, m_iY);
	DDX_Text(pDX, IDC_EDIT_D, m_iD);
}


BEGIN_MESSAGE_MAP(InterfElePosPage, CBasePage)
	ON_BN_CLICKED(IDC_CHECK_LEFT_TOP, OnBnClickedCheckLeftTop)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// InterfElePosPage message handlers

void InterfElePosPage::OnBnClickedCheckLeftTop()
{
	
}

BOOL InterfElePosPage::OnApply()
{
	if (!UpdateData())
		return FALSE;

	real2bak();
	wnd2ele();
	redraw_scene(true);
	return CBasePage::OnApply();
}

void InterfElePosPage::OnCancel()
{
	bak2real();
	wnd2ele();
	redraw_scene(true);
	CBasePage::OnCancel();
}

BOOL InterfElePosPage::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if (pMsg->hwnd == GetDlgItem(IDC_EDIT_X)->m_hWnd
				||pMsg->hwnd == GetDlgItem(IDC_EDIT_Y)->m_hWnd
				||pMsg->hwnd == GetDlgItem(IDC_EDIT_D)->m_hWnd) 
			{
				OnChangePos();
				return TRUE;
			}
		}
	}
	return CBasePage::PreTranslateMessage(pMsg);
}

BOOL InterfElePosPage::OnInitDialog()
{
	CBasePage::OnInitDialog();

	const int SPIN_MAX = 10000;
	const int SPIN_MIN = -10000;
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_X))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Y))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_D))->SetRange(SPIN_MIN, SPIN_MAX);

	ele2wnd();
	real2bak();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void InterfElePosPage::ele2wnd()
{
	const Vect2i& vPos = m_ptrEle->r();
	m_iX = vPos.x;
	m_iY = vPos.y;
	m_iD = m_ptrEle->screen_depth();
}

void InterfElePosPage::wnd2ele()
{
	m_ptrEle->set_screen_depth(m_iD);
	m_ptrEle->set_r(Vect2i(m_iX, m_iY));
	static_cast<qdInterfaceScreen*>(m_ptrEle->owner())->build_visible_elements_list();
}

void InterfElePosPage::real2bak()
{
	m_iXBak = m_iX;
	m_iYBak = m_iY;
	m_iDBak = m_iD;
}

void InterfElePosPage::bak2real()
{
	m_iX = m_iXBak;
	m_iY = m_iYBak;
	m_iD = m_iDBak;
}

void InterfElePosPage::OnChangePos()
{
	if (!UpdateData())
		return;
	SetModified();
	wnd2ele();
	redraw_scene(true);
}
void InterfElePosPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int id = pScrollBar->GetDlgCtrlID();
	if (id == IDC_SPIN_X||id == IDC_SPIN_Y||id == IDC_SPIN_D)
	{
		if (nSBCode == SB_ENDSCROLL) 
			OnChangePos();
	}
	CBasePage::OnVScroll(nSBCode, nPos, pScrollBar);
}
