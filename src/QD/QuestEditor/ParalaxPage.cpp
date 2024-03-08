// ParalaxPage.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "qd_game_object.h"
#include "ParalaxPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int OFFSET_X_MIN = -1000;
const int OFFSET_X_MAX = 1000;
const int OFFSET_Y_MIN = -1000;
const int OFFSET_Y_MAX = 1000;

/////////////////////////////////////////////////////////////////////////////
// ParalaxPage property page

IMPLEMENT_DYNCREATE(ParalaxPage, CBasePage)

ParalaxPage::ParalaxPage() : CBasePage(ParalaxPage::IDD)
{
	//{{AFX_DATA_INIT(ParalaxPage)
	m_iX = 0;
	m_iY = 0;
	//}}AFX_DATA_INIT
	m_iXBak = 0;
	m_iYBak = 0;
	m_bInitDialog = true;
	m_ptrObj = NULL;
}

bool ParalaxPage::set_data(qdGameObject* ptrObj)
{
	m_ptrObj = ptrObj;
	return true;
}

ParalaxPage::~ParalaxPage()
{
}

void ParalaxPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ParalaxPage)
	DDX_Text(pDX, IDC_EDIT_X, m_iX);
	DDX_Text(pDX, IDC_EDIT_Y, m_iY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ParalaxPage, CBasePage)
	//{{AFX_MSG_MAP(ParalaxPage)
	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_EDIT_X, OnChangeParalax)
	ON_EN_CHANGE(IDC_EDIT_Y, OnChangeParalax)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ParalaxPage message handlers

void ParalaxPage::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CBasePage::OnShowWindow(bShow, nStatus);
	
	m_bInitDialog = false;
}


void ParalaxPage::OnChangeParalax() 
{
	if(m_bInitDialog)
		return;
	if(!UpdateData())
		return;
	apply_params();
	SetModified(TRUE);
}

void ParalaxPage::Obj2Dlg()
{
	Vect2s offset = m_ptrObj->get_parallax_offset();

	m_iX = offset.x;
	m_iY = offset.y;
	real2bak();
}

void ParalaxPage::real2bak()
{
	m_iXBak = m_iX;
	m_iYBak = m_iY;
}

void ParalaxPage::bak2real()
{
	m_iX = m_iXBak;
	m_iY = m_iYBak;
}


BOOL ParalaxPage::OnApply() 
{
	real2bak();	
	apply_params();
	return CBasePage::OnApply();
}

void ParalaxPage::OnCancel() 
{
	bak2real();
	apply_params();

	CBasePage::OnCancel();
}

void ParalaxPage::apply_params()
{
	m_ptrObj->set_parallax_offset(m_iX, m_iY);
}

BOOL ParalaxPage::OnInitDialog() 
{
	CBasePage::OnInitDialog();
	
	AdjustSpinRanges();
	Obj2Dlg();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ParalaxPage::AdjustSpinRanges()
{
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WX))->SetRange(OFFSET_X_MIN, OFFSET_X_MAX);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WY))->SetRange(OFFSET_Y_MIN, OFFSET_Y_MAX);
}
