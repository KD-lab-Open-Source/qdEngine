// AnimObjBound.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "AnimObjBound.h"

#include "qd_game_object_animated.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const int SPIN_MIN = -10000;
const int SPIN_MAX = 10000;

/////////////////////////////////////////////////////////////////////////////
// AnimObjBound property page

IMPLEMENT_DYNCREATE(AnimObjBound, CBasePage)

AnimObjBound::AnimObjBound() : CBasePage(AnimObjBound::IDD)
{
	//{{AFX_DATA_INIT(AnimObjBound)
	m_iEditX = 0;
	m_iEditY = 0;
	m_iEditZ = 0;
	//}}AFX_DATA_INIT
	m_bInitDialog = true;
}

AnimObjBound::~AnimObjBound()
{
}

//! Данные из изменяемых полей в объект
bool AnimObjBound::set_data(qdGameObjectAnimated* ptrAnim)
{
	m_ptrObj = ptrAnim;
	m_bHasBound = m_ptrObj->check_flag(QD_OBJ_HAS_BOUND_FLAG);
	return (m_ptrObj != NULL);
}

void AnimObjBound::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AnimObjBound)
	DDX_Text(pDX, IDC_EDIT_X, m_iEditX);
	DDX_Text(pDX, IDC_EDIT_Y, m_iEditY);
	DDX_Text(pDX, IDC_EDIT_Z, m_iEditZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AnimObjBound, CBasePage)
	//{{AFX_MSG_MAP(AnimObjBound)
	ON_EN_CHANGE(IDC_EDIT_Z, OnChangeEditX)
	ON_EN_CHANGE(IDC_EDIT_Y, OnChangeEditX)
	ON_EN_CHANGE(IDC_EDIT_X, OnChangeEditX)
	//}}AFX_MSG_MAP
//	ON_WM_HSCROLL()
ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_X1, OnDeltaposSpinX1)
ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Y1, OnDeltaposSpinY1)
ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Z1, OnDeltaposSpinZ1)
ON_WM_VSCROLL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AnimObjBound message handlers

BOOL AnimObjBound::OnApply() 
{
	real2bak();	
	return CBasePage::OnApply();
}

void AnimObjBound::OnCancel() 
{
	if(!m_bHasBound)
		m_ptrObj->drop_flag(QD_OBJ_HAS_BOUND_FLAG);

	bak2real();
	Wnd2Obj();
	redraw_scene(true);

	CBasePage::OnCancel();
}

BOOL AnimObjBound::OnInitDialog() 
{
	CBasePage::OnInitDialog();
	
	Obj2Wnd();
	real2bak();


	static_cast<CSpinButtonCtrl*>
		(GetDlgItem(IDC_SPIN_X1))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>
		(GetDlgItem(IDC_SPIN_Y1))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>
		(GetDlgItem(IDC_SPIN_Z1))->SetRange(SPIN_MIN, SPIN_MAX);
	UpdateData(FALSE);

	m_bInitDialog = false;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AnimObjBound::OnChangeEditX() 
{
	if(m_bInitDialog)
		return;
	if (!UpdateData()) 
		return;
	SetModified(TRUE);
	
	Obj2Wnd();
	m_ptrObj->set_flag(QD_OBJ_HAS_BOUND_FLAG);
}

//! Копирует данные из изменяемых полей в резервные копии
VOID AnimObjBound::real2bak()
{
	m_iEditXBak = m_iEditX;
	m_iEditYBak = m_iEditY;
	m_iEditZBak = m_iEditZ;
}

//! Копирует данные из резервных копий в изменняемые поля
VOID AnimObjBound::bak2real()
{
	m_iEditX = m_iEditXBak;
	m_iEditY = m_iEditYBak;
	m_iEditZ = m_iEditZBak;
}

//! Из объекта данные в изменяемые поля
VOID AnimObjBound::Obj2Wnd()
{
	Vect3f v = m_ptrObj->bound();
	m_iEditX = static_cast<int>(v.x);
	m_iEditY = static_cast<int>(v.y);
	m_iEditZ = static_cast<int>(v.z);
}

//! Данные из изменяемых полей в объект
VOID AnimObjBound::Wnd2Obj()
{
	Vect3f v(m_iEditX, m_iEditY, m_iEditZ);
	m_ptrObj->set_bound(v);
}

void AnimObjBound::OnDeltaposSpinX1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void AnimObjBound::OnDeltaposSpinY1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void AnimObjBound::OnDeltaposSpinZ1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void AnimObjBound::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CBasePage::OnVScroll(nSBCode, nPos, pScrollBar);
}
