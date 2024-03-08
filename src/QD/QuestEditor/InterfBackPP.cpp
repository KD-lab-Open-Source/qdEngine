// InterfBackPP.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "InterfBackPP.h"
#include "custom_ddx.h"
#include "interf_disk_op.h"
#include "file_tests.h"

#include <Shlwapi.h>

#include "qd_interface_background.h"
#include "qd_interface_screen.h"

namespace {
	//! Устанавливает/удаляет указанный флаг для фона
	void SetFlag(qdInterfaceBackground* pb, BOOL bSet, int iFlag)
	{
		if (bSet)
			pb->set_animation_flag(iFlag);
		else
			pb->drop_animation_flag(iFlag);
	}
}

// InterfBackPP dialog

IMPLEMENT_DYNAMIC(InterfBackPP, CBasePage)
InterfBackPP::InterfBackPP()
	: CBasePage(InterfBackPP::IDD)
	, m_strFileName(_T(""))
	, m_iX(0)
	, m_iY(0)
	, m_iD(0)
	, m_ptrBack(NULL)
	, m_bLoop(FALSE)
	, m_bFlipH(FALSE)
	, m_bFlipV(FALSE)
{
}

InterfBackPP::~InterfBackPP()
{
}

bool InterfBackPP::set_data(qdInterfaceBackground* ptrBack)
{
	m_ptrBack = ptrBack;
	m_ptrCopy.reset(new qdInterfaceBackground(*ptrBack));
	return true;
}

void InterfBackPP::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	custom_ddx::DDX_Text(pDX, IDC_EDIT1, m_strFileName, 
		file_tests::non_strict_file_test());
	DDX_Text(pDX, IDC_EDIT_X, m_iX);
	DDX_Text(pDX, IDC_EDIT_Y, m_iY);
	DDX_Text(pDX, IDC_EDIT_D, m_iD);
	DDX_Check(pDX, IDC_CHECK_LOOP, m_bLoop);
	DDX_Check(pDX, IDC_CHECK_FLIP_HORZ, m_bFlipH);
	DDX_Check(pDX, IDC_CHECK_FLIP_VERT, m_bFlipV);
}


BEGIN_MESSAGE_MAP(InterfBackPP, CBasePage)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_CHECK_LOOP, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_FLIP_HORZ, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_FLIP_VERT, OnBnClickedCheck)
END_MESSAGE_MAP()


BOOL InterfBackPP::OnApply()
{
	if (!UpdateData())
		return FALSE;
	wnd2ele();
	*m_ptrCopy.get() = *m_ptrBack;
	redraw_scene(true);
	return CBasePage::OnApply();
}

void InterfBackPP::OnCancel()
{
	*m_ptrBack = *m_ptrCopy.get();
	m_ptrBack->init();
	redraw_scene(true);
	CBasePage::OnCancel();
}

BOOL InterfBackPP::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if (pMsg->hwnd == ::GetDlgItem(m_hWnd, IDC_EDIT_X)
				||pMsg->hwnd == ::GetDlgItem(m_hWnd, IDC_EDIT_Y)
				||pMsg->hwnd == ::GetDlgItem(m_hWnd, IDC_EDIT_D)) 
			{
				OnChangePos();
				return TRUE;
			}
			if (pMsg->hwnd == ::GetDlgItem(m_hWnd, IDC_EDIT1))
			{
				OnEnKillfocusEdit1();
				return TRUE;
			}
		}
	}
	return CBasePage::PreTranslateMessage(pMsg);
}

BOOL InterfBackPP::OnInitDialog()
{
	CBasePage::OnInitDialog();

	const int SPIN_MAX = 10000;
	const int SPIN_MIN = -10000;
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_X))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Y))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_D))->SetRange(SPIN_MIN, SPIN_MAX);

	ele2wnd();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// InterfBackPP message handlers

void InterfBackPP::OnBnClickedButton1()
{
	CString const res = GetIntrfAnimFileName();
	if (res.IsEmpty())
		return;
	m_strFileName = res;

	UpdateData(FALSE);
	reload_file();
	SetModified();
}

void InterfBackPP::ele2wnd()
{
	const Vect2i& vPos = m_ptrBack->r();
	m_iX = vPos.x;
	m_iY = vPos.y;
	m_iD = m_ptrBack->screen_depth();
	m_strFileName = m_ptrBack->animation_file();
	m_bLoop = m_ptrBack->check_animation_flag(QD_ANIMATION_FLAG_LOOP);
	m_bFlipH = m_ptrBack->check_animation_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL);
	m_bFlipV = m_ptrBack->check_animation_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL);
}

void InterfBackPP::wnd2ele()
{
	m_ptrBack->set_screen_depth(m_iD);
	m_ptrBack->set_r(Vect2i(m_iX, m_iY));
	m_ptrBack->set_animation_file(m_strFileName);
	m_ptrBack->init();
	static_cast<qdInterfaceScreen*>(m_ptrBack->owner())->build_visible_elements_list();
}

void InterfBackPP::OnChangePos()
{
	if (!UpdateData())
		return;
	SetModified();
	wnd2ele();
	redraw_scene(true);
}
void InterfBackPP::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int id = pScrollBar->GetDlgCtrlID();
	if (id == IDC_SPIN_X||id == IDC_SPIN_Y||id == IDC_SPIN_D)
	{
		if (nSBCode == SB_ENDSCROLL) 
			OnChangePos();
	}
	CBasePage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void InterfBackPP::OnEnKillfocusEdit1()
{
	if (!UpdateData())
		return;
	SetModified();
	reload_file();
}

void InterfBackPP::reload_file()
{
	m_ptrBack->set_animation_file(m_strFileName);
	m_ptrBack->init();
	redraw_scene(true);
}

void InterfBackPP::OnBnClickedCheck()
{
	if (!UpdateData())
		return;
	SetFlag(m_ptrBack, m_bLoop, QD_ANIMATION_FLAG_LOOP);
	SetFlag(m_ptrBack, m_bFlipH, QD_ANIMATION_FLAG_FLIP_HORIZONTAL);
	SetFlag(m_ptrBack, m_bFlipV, QD_ANIMATION_FLAG_FLIP_VERTICAL);
	m_ptrBack->init();
	redraw_scene(true);
}
