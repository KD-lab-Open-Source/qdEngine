// CursorPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "CursorPropDlg.h"
#include "qd_game_object_state.h"
#include "qd_game_object_mouse.h"
#include <algorithm>
#include ".\cursorpropdlg.h"


// CursorPropDlg dialog

IMPLEMENT_DYNAMIC(CursorPropDlg, CDialog)
CursorPropDlg::CursorPropDlg(HWND hOwnedWnd /*=NULL*/)
	: CDialog(CursorPropDlg::IDD, NULL)
	, m_ptrSourceCursor(NULL)
	, m_ptrGameDisp(NULL)
	, m_bLoop(FALSE)
	, m_bFlipHorz(FALSE)
	, m_bFlipVert(FALSE)
	, m_hOwnedWnd(hOwnedWnd)
	, m_bForDynObj(FALSE)
	, m_bForZones(FALSE)
	, m_bForMainMenu(FALSE)
	, m_bForIngameInterf(FALSE)
	, moveRadius_(0.f)
	, moveSpeed_(0.f)
	, forObjectsInInventory_(FALSE)
{
}

bool CursorPropDlg::set_data(qdGameDispatcher* ptrDisp, 
										qdGameObjectStateStatic* ptrCursor)
{
	ASSERT(ptrCursor);
	m_ptrGameDisp = ptrDisp;
	m_ptrSourceCursor = ptrCursor;
	m_ptrCopyCursor.reset(
		static_cast<qdGameObjectStateStatic*>(m_ptrSourceCursor->clone()));
	return (m_ptrCopyCursor.get());
}

CursorPropDlg::~CursorPropDlg()
{
}

void CursorPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ANIMATION, m_wndAnim);
	DDX_Check(pDX, IDC_FLAG_LOOP, m_bLoop);
	DDX_Check(pDX, IDC_FLAG_FLIP_HORZ, m_bFlipHorz);
	DDX_Check(pDX, IDC_FLAG_FLIP_VERT, m_bFlipVert);
	DDX_Check(pDX, IDC_CHECK_FOR_DYN_OBJ, m_bForDynObj);
	DDX_Check(pDX, IDC_CHECK_FOR_ZONES, m_bForZones);
	DDX_Check(pDX, IDC_CHECK_FOR_MAIN_MENU, m_bForMainMenu);
	DDX_Check(pDX, IDC_CHECK_FOR_INGAME_INTERF, m_bForIngameInterf);
	DDX_Text(pDX, IDC_EDIT_MOVE_RADIUS, moveRadius_);
	DDX_Text(pDX, IDC_EDIT_MOVE_SPEED, moveSpeed_);
	DDX_Check(pDX, IDC_CHECK_FOR_OBJ_IN_INVENTORY, forObjectsInInventory_);
}


BEGIN_MESSAGE_MAP(CursorPropDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_STACK_STATE, OnBnClickedBtnStackState)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MOVE_SPEED, OnDeltaposSpinMoveSpeed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MOVE_RADIUS, OnDeltaposSpinMoveRadius)
END_MESSAGE_MAP()


/// возвращает идентифиатор курсора(суть порядковый номер)
int get_cursor_id(qdGameObjectState* pcursor)
{
	const qdGameObjectMouse* pmouse = 
		static_cast<qdGameObjectMouse*>(pcursor->owner());
	const qdGameObjectStateVector& v = pmouse->state_vector();
	qdGameObjectStateVector::const_iterator itr = std::find(v.begin(), v.end(), pcursor);
	if (itr == v.end())
		return -1;
	return std::distance(v.begin(), itr);
}
// CursorPropDlg message handlers
void CursorPropDlg::cursor2wnd()
{
	qdAnimationInfo* ai = m_ptrSourceCursor->animation_info();
	if(ai->animation_name())
		m_wndAnim.SelectString(-1, ai->animation_name());

	m_bFlipVert = ai->check_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL);
	m_bFlipHorz = ai->check_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL);
	m_bLoop			= ai->check_flag(QD_ANIMATION_FLAG_LOOP);

	qdGameObjectMouse* pmouse = 
		static_cast<qdGameObjectMouse*>(m_ptrSourceCursor->owner());

	moveRadius_ = m_ptrSourceCursor->rnd_move_radius();
	moveSpeed_	= m_ptrSourceCursor->rnd_move_speed();

	int id = get_cursor_id(m_ptrSourceCursor);
	if (id) 
	{
		m_bForDynObj = (pmouse->default_cursor(qdGameObjectMouse::OBJECT_CURSOR) == id);
		if (!m_bForDynObj&&pmouse->default_cursor(qdGameObjectMouse::OBJECT_CURSOR)) 
			GetDlgItem(IDC_CHECK_FOR_DYN_OBJ)->EnableWindow(FALSE);

		m_bForZones	= (pmouse->default_cursor(qdGameObjectMouse::ZONE_CURSOR) == id);
		if (!m_bForZones&&pmouse->default_cursor(qdGameObjectMouse::ZONE_CURSOR)) 
			GetDlgItem(IDC_CHECK_FOR_ZONES)->EnableWindow(FALSE);

		m_bForMainMenu = 
			(pmouse->default_cursor(qdGameObjectMouse::MAIN_MENU_CURSOR) == id);
		if (!m_bForMainMenu
			//! если курсор уже установлен, то делаем недоступным
			&&pmouse->default_cursor(qdGameObjectMouse::MAIN_MENU_CURSOR))
			GetDlgItem(IDC_CHECK_FOR_MAIN_MENU)->EnableWindow(FALSE);

		m_bForIngameInterf = 
			(pmouse->default_cursor(qdGameObjectMouse::INGAME_INTERFACE_CURSOR) == id);
		if (!m_bForIngameInterf
			&&pmouse->default_cursor(qdGameObjectMouse::INGAME_INTERFACE_CURSOR))
			GetDlgItem(IDC_CHECK_FOR_INGAME_INTERF)->EnableWindow(FALSE);

		forObjectsInInventory_ = 
			(pmouse->default_cursor(qdGameObjectMouse::INVENTORY_OBJECT_CURSOR) == id);
		if (!forObjectsInInventory_
			&&pmouse->default_cursor(qdGameObjectMouse::INVENTORY_OBJECT_CURSOR))
			GetDlgItem(IDC_CHECK_FOR_OBJ_IN_INVENTORY)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_FOR_DYN_OBJ)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FOR_ZONES)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FOR_MAIN_MENU)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FOR_INGAME_INTERF)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FOR_OBJ_IN_INVENTORY)->EnableWindow(FALSE);
	}
}

void CursorPropDlg::wnd2cursor()
{
	qdAnimationInfo* ai = m_ptrSourceCursor->animation_info();
	if(m_bFlipVert)
		ai->set_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL);
	else
		ai->drop_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL);

	if(m_bFlipHorz)
		ai->set_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL);
	else
		ai->drop_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL);

	if(m_bLoop)
		ai->set_flag(QD_ANIMATION_FLAG_LOOP);
	else
		ai->drop_flag(QD_ANIMATION_FLAG_LOOP);

	int iCurSel = m_wndAnim.GetCurSel();
	if (iCurSel != CB_ERR) 
	{
		CString strText;
		m_wndAnim.GetLBText(iCurSel, strText);
		ai->set_animation_name(strText);
	}
	qdGameObjectMouse* pmouse = 
		static_cast<qdGameObjectMouse*>(m_ptrSourceCursor->owner());
	int id = get_cursor_id(m_ptrSourceCursor);

	if (m_bForDynObj) 
		pmouse->set_default_cursor(qdGameObjectMouse::OBJECT_CURSOR, id);
	else if (GetDlgItem(IDC_CHECK_FOR_DYN_OBJ)->IsWindowEnabled()) 
		pmouse->set_default_cursor(qdGameObjectMouse::OBJECT_CURSOR, 0);

	if (m_bForZones) 
		pmouse->set_default_cursor(qdGameObjectMouse::ZONE_CURSOR, id);
	else if (GetDlgItem(IDC_CHECK_FOR_ZONES)->IsWindowEnabled()) 
		pmouse->set_default_cursor(qdGameObjectMouse::ZONE_CURSOR, 0);
	
	if (m_bForMainMenu)
		pmouse->set_default_cursor(qdGameObjectMouse::MAIN_MENU_CURSOR, id);
	else if (GetDlgItem(IDC_CHECK_FOR_MAIN_MENU)->IsWindowEnabled()) 
		pmouse->set_default_cursor(qdGameObjectMouse::MAIN_MENU_CURSOR, 0);


	if (m_bForIngameInterf)
		pmouse->set_default_cursor(qdGameObjectMouse::INGAME_INTERFACE_CURSOR, id);
	else if (GetDlgItem(IDC_CHECK_FOR_INGAME_INTERF)->IsWindowEnabled()) 
		pmouse->set_default_cursor(qdGameObjectMouse::INGAME_INTERFACE_CURSOR, 0);

	if (forObjectsInInventory_)
		pmouse->set_default_cursor(qdGameObjectMouse::INVENTORY_OBJECT_CURSOR, id);
	else if (GetDlgItem(IDC_CHECK_FOR_OBJ_IN_INVENTORY)->IsWindowEnabled()) 
		pmouse->set_default_cursor(qdGameObjectMouse::INVENTORY_OBJECT_CURSOR, 0);

	m_ptrSourceCursor->set_rnd_move_radius(moveRadius_);
	m_ptrSourceCursor->set_rnd_move_speed(moveSpeed_);


}

BOOL CursorPropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	LoadAnimations();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_MOVE_SPEED))->SetRange(0, 100);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_MOVE_RADIUS))->SetRange(0, 100);
	cursor2wnd();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CursorPropDlg::OnBnClickedBtnStackState()
{
	if(!m_ptrSourceCursor->animation_info()->animation_name()) {
		AfxMessageBox(IDS_ERR_NO_ANIMATION_IN_STATE);
		return;
	}

	wnd2cursor();

	if(::SendMessage(m_hOwnedWnd, WM_STACK_STATE, TRUE, 
									(LPARAM)static_cast<IRequestingStateEdit*>(this)))
	{
		GetParent()->EnableWindow(TRUE);
		ShowWindow(SW_HIDE);
	}
}
qdGameObjectState* CursorPropDlg::GetState()
{
	return m_ptrSourceCursor;
}

void CursorPropDlg::EndEditStateDims()
{
	ASSERT(0);
}

void CursorPropDlg::EndEditCoordAnimation()
{
	ASSERT(0);
}
void CursorPropDlg::EndStackState()
{
	if(::SendMessage(m_hOwnedWnd, WM_STACK_STATE, FALSE, 
						(LPARAM)static_cast<IRequestingStateEdit*>(this)))
	{
		GetParent()->EnableWindow(FALSE);
		EnableWindow(TRUE);
		ShowWindow(SW_SHOW);
	}
}

///	Функтор, который запихивает в комбобокс переданную ему анимацию
class anim_to_combo
{
	CCJFlatComboBox& combo_;
public:
	anim_to_combo(CCJFlatComboBox& comb):combo_(comb){}
	void operator()(qdAnimation* pa) const{
		int i = combo_.AddString(pa->name());
		if (i != CB_ERR)
			combo_.SetItemDataPtr(i, pa);
	}
};

void CursorPropDlg::LoadAnimations()
{
	const qdAnimationList& lst = m_ptrGameDisp->animation_list();
	std::for_each(lst.begin(), lst.end(), anim_to_combo(m_wndAnim));
}


void CursorPropDlg::OnCancel()
{
	*m_ptrSourceCursor = *m_ptrCopyCursor.get();
	__super::OnCancel();
}

void CursorPropDlg::OnOK()
{
	__super::OnOK();
	wnd2cursor();
}
void CursorPropDlg::OnDeltaposSpinMoveSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
		return;
	moveSpeed_ += pNMUpDown->iDelta/100.f;
	if (moveSpeed_ < 0.f)
		moveSpeed_ = 0.f;
	UpdateData(FALSE);
	*pResult = 0;
}

void CursorPropDlg::OnDeltaposSpinMoveRadius(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
		return;
	moveRadius_ += pNMUpDown->iDelta/100.f;
	if (moveRadius_< 0.f)
		moveRadius_ = 0.f;
	UpdateData(FALSE);
	*pResult = 0;
}
