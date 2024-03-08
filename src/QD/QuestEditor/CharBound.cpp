// harBound.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "CharBound.h"
#include "MyPropertySheet.h"

#include "qd_game_scene.h"
#include "qd_game_object_moving.h"
#include "qd_game_dispatcher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//!\brief сохраняем положение картики для всех состоняний объекта, которые
//! не имеют собственной границы
void SaveScrLayoutForStates(qdGameObjectMoving* const pobj, 
							qdCamera* pcam,
							Vect3f const& old_bnd,
							Vect3f const& bnd);
/////////////////////////////////////////////////////////////////////////////
// CharBound property page
const int MIN_X_BOUND = -10000;
const int MIN_Y_BOUND = -10000;
const int MIN_Z_BOUND = -10000;
const int MAX_X_BOUND = 10000;
const int MAX_Y_BOUND = 10000;
const int MAX_Z_BOUND = 10000;

const float DEFAULT_SCALE = 1.f;

IMPLEMENT_DYNCREATE(CharBound, CBasePage)

CharBound::CharBound() : CBasePage(CharBound::IDD)
{
	//{{AFX_DATA_INIT(CharBound)
	m_iXBound = 0;
	m_iYBound = 0;
	m_iZBound = 0;
	m_fScale = DEFAULT_SCALE;
	//}}AFX_DATA_INIT
	m_ptrGameDisp = NULL;
	m_ptrObj = NULL;
	m_bInitDialog = true;

	m_iXBoundBak = 0;
	m_iYBoundBak = 0;
	m_iZBoundBak = 0;
	m_fScaleBak = 0.f;
	m_bStateHasBound = false;
}

bool CharBound::set_data(qdGameDispatcher* ptrDisp, qdGameObjectMoving* ptrObj)
{
	m_ptrGameDisp = ptrDisp;
	m_ptrObj = ptrObj;
//	bnd_safer_.set_obj(ptrObj);
	return true;
}

CharBound::~CharBound()
{
}

void CharBound::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CharBound)
	DDX_Text(pDX, IDC_EDIT_XBOUND, m_iXBound);
	DDX_Text(pDX, IDC_EDIT_YBOUND, m_iYBound);
	DDX_Text(pDX, IDC_EDIT_ZBOUND, m_iZBound);
	DDX_Text(pDX, IDC_EDIT_SCALE, m_fScale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CharBound, CBasePage)
	//{{AFX_MSG_MAP(CharBound)
//	ON_EN_CHANGE(IDC_EDIT_XBOUND, OnChangeBound)
//	ON_EN_CHANGE(IDC_EDIT_YBOUND, OnChangeBound)
//	ON_EN_CHANGE(IDC_EDIT_ZBOUND, OnChangeBound)
	ON_EN_CHANGE(IDC_EDIT_SCALE, OnChangeEditScale)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_AUTO_BND, OnBtnAutoBnd)
	//}}AFX_MSG_MAP
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_X, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Y, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Z, OnDeltaposSpin)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CharBound message handlers

void CharBound::OnChangeBound() 
{
	if(m_bInitDialog)
		return;
	if(!UpdateData())
		return;
	SetModified(TRUE);

	apply_params();
}

void CharBound::OnChangeEditScale() 
{
	if(m_bInitDialog)
		return;
	if(!UpdateData())
		return;
	CWaitCursor __w__;
	SetModified(TRUE);
	apply_params();
}

void CharBound::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CBasePage::OnShowWindow(bShow, nStatus);
	m_bInitDialog = false;	
}

BOOL CharBound::OnInitDialog() 
{
	CBasePage::OnInitDialog();
	AdjustSpinRanges();

	if (IsGlobalObj())
		DisableBndCtrls();
	
	Obj2Dlg();
	real2bak();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//! Данные из объекта копируются в переменные диалога
void CharBound::Obj2Dlg()
{
	Vect3f bnd = m_ptrObj->qdGameObjectAnimated::obj_bound();
	m_iXBound = bnd.x;
	m_iYBound = bnd.y;
	m_iZBound = bnd.z;

	qdGameScene* ptrScene = (qdGameScene*)m_ptrObj->owner();
	_ASSERT(ptrScene);

	m_fScale = DEFAULT_SCALE;
	if(!ptrScene->get_object_scale(m_ptrObj->name(), m_fScale))
		m_fScale = DEFAULT_SCALE;
}

//! Данные из переменных диалога копируются в их *Bak версии
void CharBound::real2bak()
{
	m_iXBoundBak = m_iXBound;
	m_iYBoundBak = m_iYBound;
	m_iZBoundBak = m_iZBound;

	m_fScaleBak	 = m_fScale;
}

//! Данные из Bak версий копируются в переменные диалога
void CharBound::bak2real()
{
	m_iXBound = m_iXBoundBak;
	m_iYBound = m_iYBoundBak;
	m_iZBound = m_iZBoundBak;
	m_fScale  = m_fScaleBak;
}

//!\brief сохраняем положение картики для всех состоняний объекта, которые
//! не имеют собственной границы
void CharBound::SaveStatesScrLayout(Vect3f const& old_bnd, Vect3f const& bnd)
{
	qdCamera* const pcam = m_ptrGameDisp->get_active_scene()->get_camera();
	SaveScrLayoutForStates(static_cast<qdGameObjectMoving*>(m_ptrObj), pcam,
		old_bnd, bnd);
}

//! Из переменных диалога копирует данные в объект
void CharBound::apply_params()
{
	Vect3f bnd(m_iXBound, m_iYBound, m_iZBound);

	Vect3f old_bnd(m_ptrObj->obj_bound());

	m_ptrObj->set_bound(bnd);
	SaveStatesScrLayout(old_bnd, bnd);

	qdGameScene* ptrScene = (qdGameScene*)m_ptrObj->owner();

	_ASSERT(ptrScene);
	ptrScene->set_object_scale(m_ptrObj->name(), m_fScale);
	
	m_ptrObj->set_scale(m_fScale);
	
	m_ptrObj->adjust_z();

	redraw();

	//PPN_BOUND_CHANGED
	QuerySiblings(PPN_BOUND_CHANGED, 0);
}

//! Определяет является ли объект глобальным
bool CharBound::IsGlobalObj(){
	ASSERT(m_ptrObj);
	return (m_ptrObj->owner()->named_object_type() == QD_NAMED_OBJECT_DISPATCHER);
}

//! Перерисовывает окно редактора сцены
void CharBound::redraw()
{
	//если неглобальный или есть активная сцена,
	//то рисуем
	if (IsGlobalObj()||m_ptrGameDisp->get_active_scene()) 
		redraw_scene(true);
}

BOOL CharBound::OnApply() 
{
	real2bak();	
	if (IsGlobalObj())
		SetBoundToCopy();
	return CBasePage::OnApply();
}

void CharBound::OnCancel() 
{
	bak2real();
	apply_params();
//	QuerySiblings(PPN_BOUND_CHANGED, 0);
	CBasePage::OnCancel();
}

//! Устанавливает область изменения для SpinControl'ов
void CharBound::AdjustSpinRanges()
{
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_X))->SetRange(MIN_X_BOUND, MAX_X_BOUND);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_Y))->SetRange(MIN_Y_BOUND, MAX_Y_BOUND);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_Z))->SetRange(MIN_Z_BOUND, MAX_Z_BOUND);
}

void CharBound::OnBtnAutoBnd() 
{
	m_ptrObj->auto_bound();
	Obj2Dlg();
	apply_params();
	SetModified(TRUE);
	UpdateData(FALSE);
}

/*!
  Функтор назначает баунд персонажу
*/

class bound_seter{
	const qdGameObjectMoving* p_;
public:
	bound_seter(const qdGameObjectMoving* p):p_(p){}
	void operator()(qdGameScene* pscene){
		qdGameObjectMoving* p = 
			static_cast<qdGameObjectMoving*>(pscene->get_object(p_->name()));
		if (p){
			p->set_bound(p_->bound());
		}
	}
};
//! Для всех копий персонажа назначает установленные параметры баунда
void CharBound::SetBoundToCopy(){
	const qdGameSceneList& sl = m_ptrGameDisp->scene_list();
	std::for_each(sl.begin(), sl.end(), bound_seter(m_ptrObj));
}

//! Делает недоступными контролы, редактирующие размеры баунда
void CharBound::DisableBndCtrls()
{
	GetDlgItem(IDC_EDIT_XBOUND)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_YBOUND)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ZBOUND)->EnableWindow(FALSE);

	GetDlgItem(IDC_SPIN_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_Y)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_Z)->EnableWindow(FALSE);

	GetDlgItem(IDC_STATIC1)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC2)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC3)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC4)->EnableWindow(FALSE);
}

BOOL CharBound::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
	{
		UINT const id = ::GetDlgCtrlID(pMsg->hwnd);
		if (id == IDC_EDIT_XBOUND
			||id == IDC_EDIT_YBOUND
			||id == IDC_EDIT_ZBOUND)
		{
			OnChangeBound();
			return TRUE;
		}
	}
	return CBasePage::PreTranslateMessage(pMsg);
}

void CharBound::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	OnChangeBound();
	*pResult = 0;
}
