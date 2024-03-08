// ZonePropPage.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "ZonePropPage.h"

#include "qd_grid_zone.h"
#include "qd_game_object_mouse.h"
#include ".\zoneproppage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ZonePropPage dialog

const int HS_LOW = -10000;
const int HS_HIGH = 10000;

IMPLEMENT_DYNCREATE(ZonePropPage, CBasePage)

ZonePropPage::ZonePropPage(CWnd* pParent /*=NULL*/)
	: CBasePage(ZonePropPage::IDD)
	, m_bNeedCursor(FALSE)
	, m_bNeedCursorBak(FALSE)
	, transparency_(0)
	, transparencyBack_(0)
	, shadowColorBack_(0)
{
	//{{AFX_DATA_INIT(ZonePropPage)
	m_iH = 0;
	m_iDefState = -1;
	//}}AFX_DATA_INIT

	m_bInitDialog	= true;
	m_ptrGameDisp	= NULL;
	m_ptrZone		= NULL;
}

bool ZonePropPage::set_data(qdGameDispatcher* ptrGD, qdGridZone* ptrZone)
{
	m_ptrGameDisp	= ptrGD;
	m_ptrZone		= ptrZone;
	return true;
}

void ZonePropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ZonePropPage)
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_iH);
	DDX_Radio(pDX, IDC_RADIO_PASS, m_iDefState);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_NEED_CURSOR, m_bNeedCursor);
	DDX_Text(pDX, IDC_EDIT_TRANSPARENCY, transparency_);
	DDX_Control(pDX, IDC_BUTTON_COLOR, shadowColor_);
}


BEGIN_MESSAGE_MAP(ZonePropPage, CBasePage)
	//{{AFX_MSG_MAP(ZonePropPage)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnChangeEditHeight)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_RADIO_PASS, OnRadioPass)
	ON_BN_CLICKED(IDC_RADIO_PASS2, OnRadioPass2)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_NEED_CURSOR, OnBnClickedCheckNeedCursor)
	ON_EN_CHANGE(IDC_EDIT_TRANSPARENCY, OnEnChangeEditTransparency)
	ON_MESSAGE(CPN_SELCHANGE, OnColorPicSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ZonePropPage message handlers

BOOL ZonePropPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	zone2wnd();
	real2bak();
	m_ptrZone->set_state(true);
	
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_HEIGHT))->
				SetRange(HS_LOW, HS_HIGH);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_TRANSPARENCY))->
		SetRange(QD_NO_SHADOW_ALPHA, QD_SHADOW_ALPHA_MAX);

	shadowColor_.EnableWindow(transparency_ != QD_NO_SHADOW_ALPHA);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ZonePropPage::OnChangeEditHeight() 
{
	if (m_bInitDialog) 
		return;

	if (!UpdateData()) {
		return;
	}
	m_ptrZone->set_height(m_iH);
	redraw();
	SetModified(TRUE);
}

void ZonePropPage::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	
	m_bInitDialog = false;	
}

BOOL ZonePropPage::OnApply() 
{
	if(!UpdateData())
		return FALSE;

	wnd2zone();
	real2bak();
//	m_ptrZone->set_height(m_iH);
	redraw();

	return CBasePage::OnApply();
}

void ZonePropPage::OnCancel() 
{
	bak2real();
	wnd2zone();
	redraw();
	CBasePage::OnCancel();
}

void ZonePropPage::zone2wnd()
{
	m_iH = m_ptrZone->height();
	m_iDefState = !m_ptrZone->state();
	m_bNeedCursor = m_ptrZone->check_flag(qdGridZone::ZONE_EXIT_FLAG);
	transparency_ = m_ptrZone->shadow_alpha();
	shadowColor_.SetColor(m_ptrZone->shadow_color());
}

void ZonePropPage::wnd2zone()
{
	m_ptrZone->set_height(m_iH);
	m_ptrZone->set_state(m_iDefState == 0);
	if (m_bNeedCursor) 
		m_ptrZone->set_flag(qdGridZone::ZONE_EXIT_FLAG);
	else
		m_ptrZone->drop_flag(qdGridZone::ZONE_EXIT_FLAG);

	m_ptrZone->set_shadow_alpha(transparency_);
	m_ptrZone->set_shadow_color(shadowColor_.GetColor());
}

void ZonePropPage::real2bak()
{
	m_iHBak = m_iH;
	m_iDefStateBak = m_iDefState;
	m_bNeedCursorBak = m_bNeedCursor;

	transparencyBack_ = transparency_;
	shadowColorBack_ = shadowColor_.GetColor();
}

void ZonePropPage::bak2real()
{
	m_iH = m_iHBak;
	m_iDefState = m_iDefStateBak;
	m_bNeedCursor = m_bNeedCursorBak;
	transparency_ = transparencyBack_;
	shadowColor_.SetColor(shadowColorBack_);
}

void ZonePropPage::redraw()
{
	redraw_scene(true);
}

void ZonePropPage::OnRadioPass() 
{
	SetModified(TRUE);
}

void ZonePropPage::OnRadioPass2() 
{
	SetModified(TRUE);
}

void ZonePropPage::ReapplyZones(qdGameScene* ptrScene)
{
	const qdGridZoneList& gzl = ptrScene->grid_zone_list();
	qdGridZoneList::const_iterator _itr = gzl.begin(), _end = gzl.end();
	for(; _itr != _end; ++_itr)
	{
		qdGridZone* ptrZone = *_itr;
		if(ptrZone->state())
			ptrZone->set_state(true);
	}
}

void ZonePropPage::OnBnClickedCheckNeedCursor()
{
	SetModified();
}

void ZonePropPage::OnEnChangeEditTransparency()
{
	if (m_bInitDialog) 
		return;
	UpdateData();
	shadowColor_.EnableWindow(transparency_ != QD_NO_SHADOW_ALPHA);
	SetModified();
}

LRESULT ZonePropPage::OnColorPicSelChange(WPARAM, LPARAM)
{
	SetModified();
	return 0;
}
