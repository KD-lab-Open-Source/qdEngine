// StateCoordsStacker.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "StateCoordsStacker.h"

#include "ChildView.h"
#include "Mainfrm.h"

#include "qd_animation_set.h"
#include "qd_game_object_moving.h"
#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"
#include "ptree_hlprs.hpp"
#include "ptree_api.h"
#include "sv_api.h"

#include "moveocntr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void PlayError();

void SaveScrLayoutOnBndEdt(qdGameObjectState* pstate, qdCamera* pcam,
						   Vect3f const& old_bnd);

/////////////////////////////////////////////////////////////////////////////
// StateCoordsStacker dialog
namespace
{

/// функтор для сортировки списка объектов по алфавиту
class ObjectListSortOp
{
public:
	bool operator () (const qdGameObject* obj1, const qdGameObject* obj2) const {
		std::string name1 = obj1->name() ? obj1->name() : "";
		std::string name2 = obj2->name() ? obj2->name() : "";

		return name1 < name2;
	}
};

/// функтор для сортировки списка состояний по алфавиту
class StateListSortOp
{
public:
	bool operator () (const qdGameObjectState* st1, const qdGameObjectState* st2) const {
		std::string name1 = st1->name() ? st1->name() : "";
		std::string name2 = st2->name() ? st2->name() : "";

		return name1 < name2;
	}
};

/// Составляет строку "Имя объекта::имя состояния"
inline CString form_str(LPCTSTR strObjName, LPCTSTR strStateName)
{
	CString str(strObjName);
	str += _T("::");
	str += strStateName;
	return str;
}

/// Составляет строку "номер кадра: имя анимации"
inline CString form_str(int i, LPCTSTR strAnimName)
{
	CString str;
	str.Format(_T("%d"), i);
	str += _T(":");
	str += strAnimName;
	return str;
}

/// Перезагружает ресурсы анимации
void reload_animation(qdAnimationInfo* pai){
	ASSERT(pai);
	qdAnimation* pa = pai->animation();
	if (!pa) return;
	pa->load_resources();
}

/// Перезагружает всю используемую в состоянии анимацию. 
void reload_anim_res(qdGameObjectState* pstate){

	if (pstate->state_type() == qdGameObjectState::STATE_STATIC) {
		qdGameObjectStateStatic* ps = 
			static_cast<qdGameObjectStateStatic*>(pstate);
		reload_animation(ps->animation_info());
	}
	else if (pstate->state_type() == qdGameObjectState::STATE_WALK){
		qdGameObjectStateWalk* ps = 
			static_cast<qdGameObjectStateWalk*>(pstate);
		qdAnimationSet* pas = ps->animation_set();
		int nCount = pas->size();
		for(int i = 0; i < nCount; ++i){
			reload_animation(pas->get_static_animation_info(i));
			reload_animation(pas->get_animation_info(i));
		}
	}
}

}
// эта используется за пределами этого файла
/// Выравнивает ширину выпадающего из комбо списка, так чтобы текст полнстью убирался
void AdjustListWidth(CCJFlatComboBox* pmyComboBox)
{
	CString      str;
	CSize      sz;
	int      dx = 0;
	TEXTMETRIC   tm;
	CDC*      pDC = pmyComboBox->GetDC();
	CFont*      pFont = pmyComboBox->GetFont();

	// Select the listbox font, save the old font
	CFont* pOldFont = pDC->SelectObject(pFont);
	// Get the text metrics for avg char width
	pDC->GetTextMetrics(&tm);

	for (int i = 0; i < pmyComboBox->GetCount(); i++)
	{
		pmyComboBox->GetLBText(i, str);
		sz = pDC->GetTextExtent(str);

		// Add the avg width to prevent clipping
		sz.cx += tm.tmAveCharWidth;

		if (sz.cx > dx)
			dx = sz.cx;
	}
	// Select the old font back into the DC
	pDC->SelectObject(pOldFont);
	pmyComboBox->ReleaseDC(pDC);

	// Adjust the width for the vertical scroll bar and the left and right border.
	dx += ::GetSystemMetrics(SM_CXVSCROLL) + 2*::GetSystemMetrics(SM_CXEDGE);

	// Set the width of the list box so that every item is completely visible.
	pmyComboBox->SetDroppedWidth(dx);
}

/// Является ли pai остановкой
qdGameObjectStateWalk::OffsetType ai_offs_type(qdAnimationInfo const* pai, qdGameObjectStateWalk const* pws)
{
	qdAnimationSet *const ptrASet = pws->animation_set();
	size_t const n = ptrASet->size();
	for(int i = 0; i < n; ++i){
		if(ptrASet->get_static_animation_info(i) == pai) return qdGameObjectStateWalk::OFFSET_STATIC;
		else if(ptrASet->get_start_animation_info(i) == pai) return qdGameObjectStateWalk::OFFSET_START;
		else if(ptrASet->get_stop_animation_info(i) == pai) return qdGameObjectStateWalk::OFFSET_END;
		else if(ptrASet->get_animation_info(i) == pai) return qdGameObjectStateWalk::OFFSET_WALK;
	}

	return qdGameObjectStateWalk::OFFSET_WALK;
}

/// Номер pai в списке анимации в походке
int state_ai_num(qdAnimationSet* const pas, qdAnimationInfo* pai){
	size_t const n = pas->size();
	for(int i = 0; i < n; ++i)
	{
		if (pai == pas->get_animation_info(i)
			|| pai==pas->get_static_animation_info(i)
			|| pai==pas->get_start_animation_info(i)
			|| pai==pas->get_stop_animation_info(i))
			return i;
	}
	return -1;
}

/// Возвращает смещение направелния походки, указанного его(направления) анимацией
Vect2i const get_walk_dir_offset(qdGameObjectState* pstate, qdAnimationInfo* ai)
{
	ASSERT(ptree_helpers::IsMovementState(pstate));

	qdGameObjectStateWalk const* const pws = static_cast<qdGameObjectStateWalk*>(pstate);
	return pws->center_offset(state_ai_num(pws->animation_set(), ai), ai_offs_type(ai, pws));
}

/// Назначает смещение направелния походки, указанного его(направления) анимацией
void set_walk_dir_offset(qdGameObjectState* pstate, qdAnimationInfo* ai, int x, int y){
	ASSERT(ptree_helpers::IsMovementState(pstate));
	qdGameObjectStateWalk * const pws = static_cast<qdGameObjectStateWalk*>(pstate);

	return pws->set_center_offset(state_ai_num(pws->animation_set(), ai), 
										Vect2i(x, y), ai_offs_type(ai, pws));
}
StateCoordsStacker::activation_delay_guard::activation_delay_guard():
	pState_(NULL), delay_(0.f)
{

}

StateCoordsStacker::activation_delay_guard::activation_delay_guard(
																													qdGameObjectState* pState)
{
	set(pState);
}

inline void StateCoordsStacker::activation_delay_guard::release(){
	if (pState_ && qdGameDispatcher::get_dispatcher()){
		pState_->set_activation_delay(delay_);
		pState_ = NULL;
	}
}

inline void StateCoordsStacker::activation_delay_guard::set(qdGameObjectState* pState){
	if (pState) 
	{
		pState_ = pState;
		delay_ = pState_->activation_delay();
		pState_->set_activation_delay(0.f);
	}
}

StateCoordsStacker::activation_delay_guard::~activation_delay_guard(){
	release();
}

void StateCoordsStacker::activation_delay_guard::reset(qdGameObjectState* pState)
{
	release();
	set(pState);
}

StateCoordsStacker::StateCoordsStacker(CWnd* pParent /*=NULL*/)
	: CDialog(StateCoordsStacker::IDD, pParent)
	, m_bSyncScale(FALSE)
	, m_iWhatShift(WS_CENTER)
	, m_bMastabChanged(false)
	, m_iXSelState(0)
	, m_iYSelState(0)
	, m_iZSelState(0)
	, NO_EXCLUDE(NULL)
{
	//{{AFX_DATA_INIT(StateCoordsStacker)
	m_iX = 0;
	m_iY = 0;
	m_iZ = 0;
	m_strStaticObjCurFrame = _T("");
	m_strStaticEditedStateCurFrame = _T("");
	m_iObjStateAnimFrame = 0;
	m_iEditedStateAnimFrame = 0;
	m_iStackMode = SM_STATE_OFFSET;
	//}}AFX_DATA_INIT
	m_bInitDialog = true;
	m_ptrSelectedState = NULL;
	m_ptrState = NULL;
	m_ptrBackAnimationInfo = NULL;
	m_ptrCurStateAI = NULL;
	m_iSelectedStateCurFrameNum = 0;
	m_iEditStateCurFrameNum = 0;
	m_ptrCurStateAI = NULL;
	backObjStateAIType	= qdGameObjectStateWalk::OFFSET_WALK;
	editedStateAIType	= qdGameObjectStateWalk::OFFSET_WALK;
}

void StateCoordsStacker::set_state(qdGameObjectState* ptrState)
{
	m_ptrState = ptrState;
	m_adg.reset(ptrState);
}

qdGameObjectState* StateCoordsStacker::getStackedState()const{
	return m_ptrState;
}

void StateCoordsStacker::SetView(ISceneViewFacade* ptrView){
	m_ptrView = ptrView;
	m_ptrView->SetStackedState(m_ptrState);
	m_ptrView->SetStateStackerDlg(this);
}

void StateCoordsStacker::EndEdit(eEndEditMode mode){
	if (mode == EEM_DISCARD) 
	{
		CWaitCursor _wait_;
		bak2real();
		reload_anim_res(m_ptrState);
	}
	else if (m_ptrState->state_type() == qdGameObjectState::STATE_STATIC
		&&m_bMastabChanged)
	{
		qdAnimation* pa = static_cast<qdGameObjectStateStatic*>(m_ptrState)->animation();
		pa->qda_save(pa->qda_file());
	}

	m_ptrState->coords_animation()->reset_scroll();
	m_ptrState->start();

	m_ptrView->SetStackedState(NULL);

	if (m_ptrSelectedState) {
		m_ptrSelectedState->start();
		m_ptrSelectedState->coords_animation()->reset_scroll();
		m_ptrSelectedState->owner()->drop_flag(QD_OBJ_HIDDEN_FLAG);
		static_cast<qdGameObjectAnimated*>(m_ptrSelectedState->owner())->
			set_state(m_ptrSelectedState);
	}
	qdGameObjectAnimated* ptrObj = 
		static_cast<qdGameObjectAnimated*>(m_ptrState->owner());
	ptrObj->set_state(m_ptrState);
	
	ptrObj->drop_flag(QD_OBJ_HIDDEN_FLAG);
	
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void StateCoordsStacker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StateCoordsStacker)
	DDX_Control(pDX, IDC_COMBO_CUR_STATE_ANIM, m_wndCurStateAnim);
	DDX_Control(pDX, IDC_STATIC_CUR_STATE, m_wndCurStateCap);
	DDX_Control(pDX, IDC_SLIDER_CUR_CADR2, m_wndEditedStateAnimFrames);
	DDX_Control(pDX, IDC_SLIDER_CUR_CADR, m_wndObjStateAnimFrames);
	DDX_Control(pDX, IDC_COMBO_ANIMATION, m_wndAnim);
	DDX_Control(pDX, IDC_COMBO_STATES, m_wndStates);
	DDX_Text(pDX, IDC_EDIT_X, m_iX);
	DDX_Text(pDX, IDC_EDIT_Y, m_iY);
	DDX_Text(pDX, IDC_Z_EDIT, m_iZ);
	DDX_Text(pDX, IDC_STATIC_CUR_FRAME, m_strStaticObjCurFrame);
	DDX_Text(pDX, IDC_STATIC_CUR_FRAME2, m_strStaticEditedStateCurFrame);
	DDX_Slider(pDX, IDC_SLIDER_CUR_CADR, m_iObjStateAnimFrame);
	DDX_Slider(pDX, IDC_SLIDER_CUR_CADR2, m_iEditedStateAnimFrame);
	DDX_Radio(pDX, IDC_RADIO1, m_iStackMode);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_TOGETHER, m_bSyncScale);
	DDX_Control(pDX, IDC_SLIDER_CA_ANIM_SCROLLER, m_wndEditedStateCAScroller);
	DDX_Control(pDX, IDC_STATIC_SCROLL, m_wndScrollStatic);
	DDX_Control(pDX, IDC_STATIC_SCROLL2, m_wndScrollStatic2);
	DDX_Control(pDX, IDC_SLIDER_CA_ANIM_SCROLLER2, m_wndSelStateCAScroller);
	DDX_Control(pDX, IDC_SPIN_X, m_wndSpinX);
	DDX_Control(pDX, IDC_SPIN_Y, m_wndSpinY);
	DDX_Radio(pDX, IDC_RADIO_TO_CENTER, m_iWhatShift);
	DDX_Text(pDX, IDC_EDIT_X3, m_iXSelState);
	DDX_Text(pDX, IDC_EDIT_Y2, m_iYSelState);
	DDX_Text(pDX, IDC_Z_EDIT2, m_iZSelState);
}

BEGIN_MESSAGE_MAP(StateCoordsStacker, CDialog)
	//{{AFX_MSG_MAP(StateCoordsStacker)
	ON_CBN_SELCHANGE(IDC_COMBO_STATES, OnSelchangeComboStates)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_X, OnDeltaposSpinX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Y, OnDeltaposSpinY)
	ON_CBN_SELCHANGE(IDC_COMBO_ANIMATION, OnSelchangeComboAnimation)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO_CUR_STATE_ANIM, OnSelchangeComboCurStateAnim)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_CHECK_TOGETHER, OnBnClickedCheckTogether)
	//}}AFX_MSG_MAP
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_RADIO_TO_CENTER, OnBnClickedRadioToCenter)
	ON_BN_CLICKED(IDC_RADIO_TO_IMAGE, OnBnClickedRadioToImage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StateCoordsStacker message handlers

//выбрали состояние, с которым будем стыковать
void StateCoordsStacker::OnSelchangeComboStates() 
{
	int iSel = m_wndStates.GetCurSel();
	
	qdGameObjectState* ptrOld = m_ptrSelectedState;


	m_ptrSelectedState = 
		static_cast<qdGameObjectState*>(m_wndStates.GetItemDataPtr(iSel));

	m_adg2.reset(m_ptrSelectedState);
	
	if (m_ptrSelectedState) {
		qdGameObjectAnimated* ptrObj = 
			static_cast<qdGameObjectAnimated*>(m_ptrSelectedState->owner());

		ptrObj->set_state(m_ptrSelectedState);
		//скрыавем объект, чтобы он не мешался на экране
		//и мы могли показыавть вместо него то, что хотим
		ptrObj->set_flag(QD_OBJ_HIDDEN_FLAG);
	}

	if (!m_ptrSelectedState
		||m_ptrSelectedState->owner() != m_ptrState->owner()) {
		//выбрали разные объекты, значит владельцу 
		//редактируемого состояния надо назначить
		//редактируемое состояние
		qdGameObjectAnimated* ptrObj = 
			static_cast<qdGameObjectAnimated*>(m_ptrState->owner());
		if(ptrObj->get_state(ptrObj->cur_state()) != m_ptrState)
			ptrObj->set_state(m_ptrState);
		//скрываем по той же причине, что и раньше
		ptrObj->set_flag(QD_OBJ_HIDDEN_FLAG);
	}

	if (ptrOld) {//надо восстановить состояние по умолчанию для объекта

		//скроллировали или нет все равно сбросим, чтобы подстарховаться.
		ptrOld->coords_animation()->reset_scroll();

		if ((!m_ptrSelectedState
			|| m_ptrSelectedState->owner() != ptrOld->owner())
			&&(ptrOld->owner() != m_ptrState->owner())) 
		{
			qdGameObjectAnimated* paobj = 
				static_cast<qdGameObjectAnimated*>(ptrOld->owner());
			paobj->set_default_state();
			paobj->drop_flag(QD_OBJ_HIDDEN_FLAG);
		}
	}

	LoadStateAnimation(m_wndAnim, m_ptrSelectedState);
	UpdateAnimation();
	UpdateObjAnimControls();
	UpdateModeCtrls();
	UpdateCAAnimScroller(m_wndSelStateCAScroller, m_wndScrollStatic2,
		m_ptrSelectedState, &m_iSelectedStateCurFrameNum);

	UpdateSelectedStateCtrls();
	state2wnd();

	//вновь выбранную анимацию рассматриваем сначала
	m_iObjStateAnimFrame = 0;

	UpdateData(FALSE);
	
	m_ptrState->load_resources();
	if(m_ptrSelectedState != ptrOld)
		m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
}

void StateCoordsStacker::state2begin(qdGameObjectState* ptrState)
{
	qdGameObjectAnimated* ptrObj = static_cast<qdGameObjectAnimated*>(ptrState->owner());

	if (!ptrState->coords_animation()->is_empty()) 
		ptrState->coords_animation()->set_time_rel(0.f);
	else
	{
		if (ptrObj->get_state(ptrObj->cur_state()) == ptrState) 
			ptrObj->get_animation()->set_time_rel(0.f);
	}
}

bool StateCoordsStacker::IsMouseCursor()
{
	return (m_ptrState->owner()->named_object_type() == QD_NAMED_OBJECT_MOUSE_OBJ);
}

BOOL StateCoordsStacker::OnInitDialog() 
{
	CDialog::OnInitDialog();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_X))->SetRange(-20000, 20000);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Y))->SetRange(-20000, 20000);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Z))->SetRange(-20000, 20000);
	m_wndObjStateAnimFrames.SetRange(0, MAX_ANIM_FRAMES);
	m_wndEditedStateAnimFrames.SetRange(0, MAX_ANIM_FRAMES);
	m_wndObjStateAnimFrames.SetLineSize(1);
	m_wndEditedStateAnimFrames.SetLineSize(1);

	state2begin(m_ptrState);
	//написали координатки в окошках
	state2wnd();
	real2bak();

	m_ptrState->owner()->set_flag(QD_OBJ_HIDDEN_FLAG);
	m_iStackMode = SM_STATE_OFFSET;
	if (IsMouseCursor())
	{
		LoadObjectStates(static_cast<qdGameObjectAnimated*>(m_ptrState->owner()));
		GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO3)->EnableWindow(FALSE);
		LoadStateAnimation(m_wndCurStateAnim, m_ptrState);
		int iPos = m_wndStates.InsertString(0, CString((LPTSTR)IDS_NOTHING));
		if(iPos != -1)
			m_wndStates.SetCurSel(iPos);
		m_iWhatShift = WS_IMAGE;
	}
	else{
		//загрузили состояния всех объектов из сцены.
		LoadObjectsStates();
		
		qdGameObjectAnimated* ptrObj = 
			static_cast<qdGameObjectAnimated*>(m_ptrState->owner());

		if (m_ptrState->state_type() == qdGameObjectState::STATE_WALK) {
			GetDlgItem(IDC_SPIN_Z)->EnableWindow(FALSE);
			GetDlgItem(IDC_Z_EDIT)->EnableWindow(FALSE);
		}
		//так как не выбрано ни каких состояний
		//скрываем объект, чтобы он не мешался нам на экране
		
		CString strCap;
		strCap.Format(IDS_CAP_STATE_STACKER, 
			m_ptrState->owner()->name(), m_ptrState->name());
		SetWindowText(strCap);

		m_wndCurStateCap.SetWindowText(
			form_str(m_ptrState->owner()->name(), m_ptrState->name()));

		//загружаем имена анимаций для состояния
		LoadStateAnimation(m_wndCurStateAnim, m_ptrState);

		if (m_ptrState->state_type() == qdGameObjectState::STATE_STATIC) {
			m_strStaticEditedStateCurFrame.Format(IDS_CUR_FRAME_TMPL, 
				static_cast<qdGameObjectStateStatic*>
					(m_ptrState)->animation_info()->animation_name());
		}
		else if (m_ptrState->state_type() == qdGameObjectState::STATE_WALK)
		{
			if(m_ptrCurStateAI)
				m_strStaticEditedStateCurFrame.Format(IDS_CUR_FRAME_TMPL, 
					m_ptrCurStateAI->animation_name());
		}
	}	
	UpdateObjAnimControls();
	UpdateSceneViewState();
	UpdateEditedStateAnimation();
	UpdateEditedStateAnimControls();

//	WhatShiftEnable(m_iStackMode == SM_STATE_OFFSET&&PermitAccess2WhatShift());
	WhatShiftEnable(true);

	UpdatePosControls();
	UpdateCAAnimScroller(m_wndEditedStateCAScroller, m_wndScrollStatic,
						m_ptrState, &m_iEditStateCurFrameNum);
	UpdateCAAnimScroller(m_wndSelStateCAScroller, m_wndScrollStatic2,
						m_ptrSelectedState, &m_iSelectedStateCurFrameNum);

	UpdateData(FALSE);

	m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
	
	m_bInitDialog = false;
	return TRUE;
}

void StateCoordsStacker::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
}

void StateCoordsStacker::OnOK() 
{
	if (!UpdateData())
		return;
	CDialog::OnOK();
	wnd2state();

	ptree_api::EndStateStack(ptree_api::EESTM_OK);
}

void StateCoordsStacker::OnCancel() 
{
	CDialog::OnCancel();
	ptree_api::EndStateStack(ptree_api::EESTM_CANCEL);
}

//загружает состояния всех объектов из сцены
void StateCoordsStacker::LoadObjectsStates()
{
	qdGameScene* ptrScene = 
		static_cast<qdGameScene*>(m_ptrState->owner()->owner());
	const qdGameObjectList& ol = ptrScene->object_list();

	typedef std::vector<qdGameObject*> ObjectVect;
	ObjectVect lst;
	lst.insert(lst.end(), ol.begin(), ol.end());
	std::sort(lst.begin(), lst.end(), ObjectListSortOp());

	ObjectVect::const_iterator _itr = lst.begin(), _end = lst.end();
	
	for(;_itr != _end; ++_itr)
	{
		qdGameObject* ptrObj = *_itr;
		if(ptrObj->named_object_type() != QD_NAMED_OBJECT_STATIC_OBJ)
			LoadObjectStates(static_cast<qdGameObjectAnimated*>(ptrObj));
	}

	int iPos = m_wndStates.InsertString(0, CString((LPTSTR)IDS_NOTHING));
	if(iPos != -1)
		m_wndStates.SetCurSel(iPos);
	AdjustListWidth(&m_wndStates);
}

/// загружает состояния объекта, 
/// исключая стыкуемое состояние, если это непоходка
void StateCoordsStacker::LoadObjectStates(qdGameObjectAnimated* ptrObj)
{
	const qdGameObjectStateVector& gosv = ptrObj->state_vector();

	qdGameObjectStateVector lst;
	lst.insert(lst.end(), gosv.begin(), gosv.end());
	std::sort(lst.begin(), lst.end(), StateListSortOp());

	qdGameObjectStateVector::const_iterator _itr = lst.begin(), 
											_end = lst.end();
	for(;_itr != _end; ++_itr)
	{
		qdGameObjectState* ptrState = *_itr;
		//исключаем редактируемое,если это непоходка, и маски
		if ((m_ptrState != ptrState
			||ptree_helpers::IsMovementState(m_ptrState))
			&&ptrState->state_type() != qdGameObjectState::STATE_MASK)
		{
			CString str = form_str(ptrObj->name(), ptrState->name());

			int iPos = m_wndStates.AddString(str);
			if(iPos != -1)
				m_wndStates.SetItemDataPtr(iPos, ptrState);
		}
	}
}

void StateCoordsStacker::LoadStaticStateAnimation(CCJFlatComboBox& wnd, 
											qdGameObjectStateStatic const* ptrState)
{
	ASSERT(ptrState->animation_info()->animation_name());
	CString str(ptrState->animation_info()->animation_name());
	int iItem = wnd.AddString(ptrState->animation_info()->animation_name());
	if (iItem != CB_ERR) 
		wnd.SetItemDataPtr(iItem, 
					const_cast<qdAnimationInfo*>(ptrState->animation_info()));
}

void StateCoordsStacker::LoadWalkStateAnimation(CCJFlatComboBox& wnd, 
											qdGameObjectStateWalk const* ptrState,
											qdAnimationInfo const* exclude)
{
	//если состояние содержит походочную координатную анимацию,
	//то выбор анимации происходит автоматически.
	if (ptrState->coords_animation()->is_empty()) {
		qdAnimationSet* ptrASet = ptrState->animation_set();

		for(int i = 0; i < ptrASet->size(); ++i)
		{
			if (ptrASet->get_animation_info(i) != exclude)
			{
				if(LPCTSTR const lpszAnimName = 
					ptrASet->get_animation_info(i)->animation_name())
				{
					int iItem = wnd.AddString(form_str(i, lpszAnimName));
					if (iItem != CB_ERR) 
						wnd.SetItemDataPtr(iItem, ptrASet->get_animation_info(i));
				}
			}
			if (ptrASet->get_static_animation_info(i) != exclude)
			{
				if(LPCTSTR const lpszAnimName = 
					ptrASet->get_static_animation_info(i)->animation_name())
				{
					int iItem = wnd.AddString(form_str(i, lpszAnimName));
					if(iItem != CB_ERR)
						wnd.SetItemDataPtr(iItem, ptrASet->get_static_animation_info(i));
				}
			}
			if (ptrASet->get_start_animation_info(i) != exclude)
			{
				if(LPCTSTR const lpszAnimName = 
					ptrASet->get_start_animation_info(i)->animation_name())
				{
					int iItem = wnd.AddString(form_str(i, lpszAnimName));
					if(iItem != CB_ERR)
						wnd.SetItemDataPtr(iItem, ptrASet->get_start_animation_info(i));
				}
			}
			if (ptrASet->get_stop_animation_info(i) != exclude)
			{
				if(LPCTSTR const lpszAnimName = 
					ptrASet->get_stop_animation_info(i)->animation_name())
				{
					int iItem = wnd.AddString(form_str(i, lpszAnimName));
					if(iItem != CB_ERR)
						wnd.SetItemDataPtr(iItem, ptrASet->get_stop_animation_info(i));
				}
			}
		}
	}
	else
		wnd.AddString(CString((LPTSTR)IDS_AUTOMATIC_SELECTED_ANIM));
}

void StateCoordsStacker::LoadStateAnimation(CCJFlatComboBox& wnd,
											qdGameObjectState const* ptrState)
{
	wnd.ResetContent();
	wnd.EnableWindow(m_ptrState != NULL);

	if (!ptrState) {
		return;
	}
	if (ptrState->state_type() == qdGameObjectState::STATE_STATIC) {
		LoadStaticStateAnimation(wnd, 
			static_cast<qdGameObjectStateStatic const*>(ptrState));
		wnd.SetCurSel(0);
	}
	else
	{
		ASSERT(ptree_helpers::IsMovementState(ptrState));
		qdGameObjectStateWalk const* const pws = 
			static_cast<qdGameObjectStateWalk const*>(ptrState);
		qdAnimationInfo const* pexcl = NO_EXCLUDE;

		//если стыкуемое состояние походочное, 
		//то из списка анимаций походки состояния, с которым стыкуем, исключается
		//та анимация, которая выбрана у стыкуемого состояния.
		if (ptree_helpers::IsMovementState(m_ptrState))
			pexcl = m_ptrCurStateAI;

		LoadWalkStateAnimation(wnd, pws, pexcl);

		if (ptrState->coords_animation()->is_empty()) {
			qdGameObjectMoving* pobj = 
				static_cast<qdGameObjectMoving*>(ptrState->owner());

			qdAnimationSet* ptrASet = pws->animation_set();
			int num = ptrASet->get_angle_index(pobj->direction_angle());
			
			wnd.SetCurSel(2*num + 1);
		}
		else
			wnd.SetCurSel(0);
	}
	AdjustListWidth(&wnd);
}

void StateCoordsStacker::state2wnd()
{
	if (m_iStackMode == SM_STATE_OFFSET) {
		Vect2s v;// = m_ptrState->center_offset();
		if (m_ptrSelectedState != m_ptrState)
			v = m_ptrState->center_offset();
		else
			v = get_walk_dir_offset(m_ptrState, get_edited_state_ai());
		m_iX = v.x;
		m_iY = v.y;
		m_iZ = 0;
	}
	else if (m_iStackMode == SM_OBJ_COORDINATE)
	{
		qdGameObjectAnimated* const pobj = 
			static_cast<qdGameObjectAnimated*>(m_ptrState->owner());
		qdGameObjectState* const pstate = pobj->get_cur_state();
		if (pstate != m_ptrState)
			pobj->set_state(m_ptrState);

		const Vect3f *pos = 
			&static_cast<qdGameObjectAnimated*>(m_ptrState->owner())->R();
		if (!m_ptrState->coords_animation()->is_empty()) {
			qdCoordsAnimation* pca = m_ptrState->coords_animation();
			pca->set_time_rel(get_edited_state_anim_phase());
			pos = &pca->get_point(pca->get_cur_point())->dest_pos();
		}

		m_iX = static_cast<int>(pos->x);
		m_iY = static_cast<int>(pos->y);
		m_iZ = static_cast<int>(pos->z);

		if (pstate != m_ptrState)
			pobj->set_state(pstate);
	}
	else if (m_iStackMode == SM_STATE_BOUND)
	{
		if (!m_ptrState->has_bound())
		{
			if (m_ptrState->owner()->named_object_type()== QD_NAMED_OBJECT_MOVING_OBJ)
					m_ptrState->set_bound(static_cast<qdGameObjectMoving*>
									(m_ptrState->owner())->bound());
			m_ptrState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);
		}
		const Vect3f& b = m_ptrState->bound();
		m_iX = static_cast<int>(b.x);
		m_iY = static_cast<int>(b.y);
		m_iZ = static_cast<int>(b.z);
	}
	else if (m_iStackMode == SM_STATE_SCALE){
		if (m_ptrState->state_type() == qdGameObjectState::STATE_WALK){
			qdGameObjectMoving* const po = 
				static_cast<qdGameObjectMoving*>(m_ptrState->owner());
			ASSERT(po);
			Vect2s const size(po->screen_size());
			m_iX = size.x;
			m_iY = size.y;
			m_iZ = 0;
		}
		else if(m_ptrCurStateAI){
			qdAnimation* pa = m_ptrCurStateAI->animation();
			if (pa){
				m_iX = pa->size_x();
				m_iY = pa->size_y();
				m_iZ = 0;
			}
		}
	}
}

void StateCoordsStacker::wnd2state()
{
	if (m_iStackMode == SM_STATE_OFFSET) {
		if (m_iWhatShift == WS_IMAGE )
		{
			if (m_ptrState == m_ptrSelectedState)
			{
				set_walk_dir_offset(m_ptrState, get_edited_state_ai(), m_iX, m_iY);
			}
			else
				m_ptrState->set_center_offset(Vect2s(m_iX, m_iY));
		}
		else if (m_iWhatShift == WS_CENTER)
		{
/*			MoveOCntr mc(m_ptrState->center_offset()- Vect2s(m_iX, m_iY) , m_ptrState, false);
			m_ptrState->set_center_offset(
					m_ptrState->center_offset() - mc(qdCamera::current_camera()));
			m_iX = m_ptrState->center_offset().x;
			m_iY = m_ptrState->center_offset().y;*/
			if (m_ptrState == m_ptrSelectedState)
			{
				set_walk_dir_offset(m_ptrState, get_edited_state_ai(), m_iX, m_iY);
			}
			else
				m_ptrState->set_center_offset(Vect2s(m_iX, m_iY));
			UpdateData(FALSE);
		}
	}
	else if (m_iStackMode == SM_OBJ_COORDINATE){
		Vect3f pos;
		pos.x = static_cast<float>(m_iX);
		pos.y = static_cast<float>(m_iY);
		pos.z = static_cast<float>(m_iZ);
		qdCoordsAnimation* ptrCA = m_ptrState->coords_animation();
		ASSERT(ptrCA);
		if (!ptrCA->is_empty()) {
			ptrCA->get_point(ptrCA->get_cur_point())->set_dest_pos(pos);
			ptrCA->reset_cur_point();
		}
		else
		{
			qdGameObjectAnimated* pa = 
				static_cast<qdGameObjectAnimated*>(m_ptrState->owner());
			pa->set_pos(pos);
			pa->set_default_pos(pos);
		}
	}
	else if (m_iStackMode == SM_STATE_BOUND)
	{
		Vect3f const old_bnd(m_ptrState->bound());

		Vect3f const b(static_cast<float>(m_iX),
				static_cast<float>(m_iY),
				static_cast<float>(m_iZ));
		m_ptrState->set_bound(b);
		if (ptree_helpers::is_obj_moving(m_ptrState->owner()))
		{
//			if (!m_ptrState->coords_animation()->is_empty())
				SaveScrLayoutOnBndEdt(m_ptrState, 
					static_cast<qdGameScene*>(m_ptrState->owner()->owner())->get_camera(),
					old_bnd);
			static_cast<qdGameObjectMoving*>(m_ptrState->owner())->adjust_z();
		}
	}
	else if (m_iStackMode == SM_STATE_SCALE){

		if (m_ptrState->state_type() == qdGameObjectState::STATE_WALK){
			qdGameObjectMoving* po = 
				static_cast<qdGameObjectMoving*>(m_ptrState->owner());
			ASSERT(po);
			ASSERT(po->get_cur_state() == m_ptrState);
			Vect2s size(po->screen_size());
			if (m_iX != size.x|| m_iY != size.y){
				CWaitCursor __wait__;
				float xs = m_iX/static_cast<float>(size.x);
				float ys = m_iY/static_cast<float>(size.y);
				scale_gait(m_ptrState, xs, ys);
				m_bMastabChanged = true;
			}
		}
		else if (m_ptrCurStateAI) {
			qdAnimation* pa = m_ptrCurStateAI->animation();
			if (pa)
				if (m_iX != pa->size_x()||m_iY != pa->size_y()){
					CWaitCursor __wait__;
					float xs = m_iX/static_cast<float>(pa->size_x());
					float ys = m_iY/static_cast<float>(pa->size_y());
					pa->scale(xs, ys);
					m_bMastabChanged = true;
				}
		}
	}
}

/// масштабировать qdAnimationInfo
inline void scale_ai(qdAnimationInfo* pai, float xs, float ys){
	if (!pai) return;
	qdAnimation* pa = pai->animation();
	if (!pa) return;
	
	pa->scale(xs, ys);
}

void StateCoordsStacker::scale_gait(qdGameObjectState* ptrState, float xs, float ys)
{
	ASSERT(ptrState->state_type()==qdGameObjectState::STATE_WALK);
	qdGameObjectStateWalk* ps = static_cast<qdGameObjectStateWalk*>(m_ptrState);
	qdAnimationSet* pas = ps->animation_set();
	ASSERT(pas);
	int nCount = pas->size();
	for(int i = 0; i < nCount; ++i){
		qdAnimationInfo* pai = pas->get_static_animation_info(i);
		scale_ai(pai,xs, ys);

		pai = pas->get_animation_info(i);
		scale_ai(pai,xs, ys);
	}
}

void StateCoordsStacker::real2bak()
{
	qdGameObjectAnimated* const pa = 
		static_cast<qdGameObjectAnimated*>(m_ptrState->owner());

	m_vObjPosBak = pa->R();
	if (m_ptrStateBak.get()) 
		*m_ptrStateBak.get() = *m_ptrState;
	else
		m_ptrStateBak.reset(m_ptrState->clone());
}

void StateCoordsStacker::bak2real()
{
	//предполагается, что вызов будет происходить один раз
	//при вызове OnCancel

	//эти присваиванием возвращаем назад все изменения внутри состояния
	//как то: коодринатная анимация, отступ
	*m_ptrState = *m_ptrStateBak.get();
	
	if(m_ptrState->coords_animation()->is_empty())
	{
		//возвращаем объект на место
		qdGameObjectAnimated* pa = 
			static_cast<qdGameObjectAnimated*>(m_ptrState->owner());
		pa->set_pos(m_vObjPosBak);
		pa->set_default_pos(m_vObjPosBak);
	}
}

void StateCoordsStacker::OnDeltaposSpinX(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;

	if (m_iStackMode == SM_STATE_SCALE) 
	{
		if (m_bSyncScale)
		{
			if ((pNMUpDown->iPos + pNMUpDown->iDelta<=0)
				||(m_wndSpinY.GetPos() + pNMUpDown->iDelta<=0)) 
			{
				PlayError();
				*pResult = 1;
			}
			else
				m_wndSpinY.SetPos(m_wndSpinY.GetPos() + pNMUpDown->iDelta);
		}
		else 
		{
			if (pNMUpDown->iPos + pNMUpDown->iDelta<=0)
			{
				PlayError();
				*pResult = 1;
			}
		}
	}
}


void StateCoordsStacker::OnDeltaposSpinY(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;
	if (m_iStackMode == SM_STATE_SCALE) 
	{
		if (m_bSyncScale)
		{
			if ((pNMUpDown->iPos + pNMUpDown->iDelta<=0)
				||(m_wndSpinX.GetPos() + pNMUpDown->iDelta<=0)) 
			{
				*pResult = 1;
				PlayError();
			}
			else
				m_wndSpinX.SetPos(m_wndSpinX.GetPos() + pNMUpDown->iDelta);
		}
		else
		{
			if (pNMUpDown->iPos + pNMUpDown->iDelta<=0)
			{
				PlayError();
				*pResult = 1;
			}
		}
	}
}

BOOL StateCoordsStacker::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_EDIT_X)->m_hWnd
		||pMsg->hwnd == GetDlgItem(IDC_EDIT_Y)->m_hWnd
		||pMsg->hwnd == GetDlgItem(IDC_Z_EDIT)->m_hWnd) {
			if (UpdateData()) {
				wnd2state();
				sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
			}
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void StateCoordsStacker::UpdateStateInfo()
{
	state2wnd();
	UpdateData(FALSE);
}

void StateCoordsStacker::UpdateEditedStateAnimControls()
{
	m_wndCurStateAnim.EnableWindow(m_ptrCurStateAI != NULL);
//	m_wndEditedStateAnimFrames.EnableWindow(m_ptrCurStateAI != NULL);
//	GetDlgItem(IDC_STATIC_CUR_FRAME2)->EnableWindow(m_ptrCurStateAI != NULL);
	
	if (m_ptrCurStateAI) {
		CString strAnimName;
		m_wndCurStateAnim.GetLBText(m_wndCurStateAnim.GetCurSel(), strAnimName);
		m_strStaticEditedStateCurFrame.Format(IDS_CUR_FRAME_TMPL, (LPCTSTR)strAnimName);
	}
	else
		m_strStaticEditedStateCurFrame.LoadString(IDS_CAP_ANIMATION_PHASE);
}

void StateCoordsStacker::UpdateObjAnimControls()
{
	const bool benable = m_ptrSelectedState != NULL&&m_ptrBackAnimationInfo != NULL;
	m_wndAnim.EnableWindow(benable);
	GetDlgItem(IDC_STATIC_CUR_FRAME)->EnableWindow(m_ptrSelectedState != NULL);
	m_wndObjStateAnimFrames.EnableWindow(m_ptrSelectedState != NULL);
	
	if (benable) {
		CString strAnimName;
		m_wndAnim.GetLBText(m_wndAnim.GetCurSel(), strAnimName);
		m_strStaticObjCurFrame.Format(IDS_CUR_FRAME_TMPL, (LPCTSTR)strAnimName);
	}
	else
		m_strStaticObjCurFrame.LoadString(IDS_CAP_ANIMATION_PHASE);
}

void StateCoordsStacker::UpdateSceneViewState()
{
	if (m_iStackMode == SM_STATE_OFFSET)
		m_ptrView->SetSubTools(SSTE_EDIT_STATE_CENTER_OFFSET);
	else if (m_iStackMode == SM_OBJ_COORDINATE)
		m_ptrView->SetSubTools(SSTE_EDIT_STATE_POS);
	else if (m_iStackMode == SM_STATE_BOUND)
		m_ptrView->SetSubTools(SSTE_EDIT_DIMS);
	else if (m_iStackMode == SM_STATE_SCALE){
		m_ptrView->SetSubTools(SSTE_NONE);
	}
}

void StateCoordsStacker::UpdatePosControls()
{
	if (m_iStackMode == SM_STATE_OFFSET) 
	{//будем редактировать смещение
		if (m_iWhatShift == WS_IMAGE)
		{
			GetDlgItem(IDC_Z_EDIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_SPIN_Z)->EnableWindow(FALSE);
			
			GetDlgItem(IDC_EDIT_Y)->EnableWindow(TRUE);
			GetDlgItem(IDC_SPIN_Y)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_X)->EnableWindow(TRUE);
			GetDlgItem(IDC_SPIN_X)->EnableWindow(TRUE);
		}
		else if (m_iWhatShift == WS_CENTER){
			GetDlgItem(IDC_Z_EDIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_SPIN_Z)->EnableWindow(FALSE);

//			GetDlgItem(IDC_EDIT_Y)->EnableWindow(FALSE);
//			GetDlgItem(IDC_SPIN_Y)->EnableWindow(FALSE);
//			GetDlgItem(IDC_EDIT_X)->EnableWindow(FALSE);
//			GetDlgItem(IDC_SPIN_X)->EnableWindow(FALSE);
		}
	}
	else if (m_iStackMode == SM_OBJ_COORDINATE) {
		if (ptree_helpers::is_obj_moving(m_ptrState->owner())) {
			GetDlgItem(IDC_Z_EDIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_SPIN_Z)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_Z_EDIT)->EnableWindow(TRUE);
			GetDlgItem(IDC_SPIN_Z)->EnableWindow(TRUE);
		}
		GetDlgItem(IDC_EDIT_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_X)->EnableWindow(TRUE);
	}
	else if (m_iStackMode == SM_STATE_BOUND){
		GetDlgItem(IDC_Z_EDIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_Z)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_X)->EnableWindow(TRUE);
	}
	else if (m_iStackMode == SM_STATE_SCALE){
		if ((!m_ptrCurStateAI||!m_ptrCurStateAI->animation())
			&&m_ptrState->state_type() == qdGameObjectState::STATE_STATIC){
			GetDlgItem(IDC_EDIT_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_SPIN_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_SPIN_X)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_EDIT_Y)->EnableWindow(TRUE);
			GetDlgItem(IDC_SPIN_Y)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_X)->EnableWindow(TRUE);
			GetDlgItem(IDC_SPIN_X)->EnableWindow(TRUE);
		}
		GetDlgItem(IDC_Z_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_Z)->EnableWindow(FALSE);
	}
}

void StateCoordsStacker::OnSelchangeComboAnimation() 
{
	m_iObjStateAnimFrame = 0;
	UpdateObjAnimControls();
	UpdateAnimation();
	if (m_ptrSelectedState == m_ptrState)
		UpdateSelectedStateCtrls();
	UpdateData(FALSE);
	m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
}

void StateCoordsStacker::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar->GetDlgCtrlID() == m_wndObjStateAnimFrames.GetDlgCtrlID()) {
		if (nSBCode == TB_ENDTRACK) {
			m_iObjStateAnimFrame = m_wndObjStateAnimFrames.GetPos();
			m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
			UpdateSelectedStateCtrls();
			UpdateData(FALSE);
		}
	}
	else if (pScrollBar->GetDlgCtrlID() == m_wndEditedStateAnimFrames.GetDlgCtrlID()) {
		if (nSBCode == TB_ENDTRACK) {
			m_iEditedStateAnimFrame = m_wndEditedStateAnimFrames.GetPos();
			m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
			state2wnd();
			UpdateData(FALSE);
		}
	}
	else if (pScrollBar->GetDlgCtrlID() == m_wndEditedStateCAScroller.GetDlgCtrlID()){
		if (nSBCode == TB_ENDTRACK){
			int pos = m_wndEditedStateCAScroller.GetPos();
			if (pos != m_iEditStateCurFrameNum){
				bool bDir = pos > m_iEditStateCurFrameNum;
				int nCount = (std::max)(pos, m_iEditStateCurFrameNum)
					- (std::min)(pos, m_iEditStateCurFrameNum);
				for(int i = 0; i < nCount; ++i)
					m_ptrState->coords_animation()->change_animation_frame(bDir);

				m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
				m_iEditStateCurFrameNum = pos;
			}
		}
	}
	else if (pScrollBar->GetDlgCtrlID() == m_wndSelStateCAScroller.GetDlgCtrlID()){
		if (nSBCode == TB_ENDTRACK){
			int pos = m_wndSelStateCAScroller.GetPos();
			if (pos != m_iSelectedStateCurFrameNum){
				bool bDir = pos > m_iSelectedStateCurFrameNum;
				int nCount = (std::max)(pos, m_iSelectedStateCurFrameNum)
					- (std::min)(pos, m_iSelectedStateCurFrameNum);
				for(int i = 0; i < nCount; ++i)
					m_ptrSelectedState->coords_animation()->change_animation_frame(bDir);

				m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
				m_iSelectedStateCurFrameNum = pos;
			}
		}
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void StateCoordsStacker::UpdateAnimation()
{
	backObjStateAIType = qdGameObjectStateWalk::OFFSET_WALK;
	int const cur_sel = m_wndAnim.GetCurSel();
	if(m_ptrSelectedState&&cur_sel!=CB_ERR)
	{
		m_ptrBackAnimationInfo = 
			static_cast<qdAnimationInfo*>(m_wndAnim.GetItemDataPtr(cur_sel));
		if (ptree_helpers::IsMovementState(m_ptrSelectedState))
			backObjStateAIType = ai_offs_type(m_ptrBackAnimationInfo,
				static_cast<qdGameObjectStateWalk*>(m_ptrSelectedState));
	}
	else
		m_ptrBackAnimationInfo = NULL;
}

void StateCoordsStacker::UpdateEditedStateAnimation()
{
	editedStateAIType = qdGameObjectStateWalk::OFFSET_WALK;
	int const cur_sel = m_wndCurStateAnim.GetCurSel();
	if (cur_sel != CB_ERR) 
	{
		m_ptrCurStateAI = 
			static_cast<qdAnimationInfo*>(m_wndCurStateAnim.GetItemDataPtr(cur_sel));
		if (ptree_helpers::IsMovementState(m_ptrState))
			editedStateAIType = ai_offs_type(m_ptrCurStateAI, 
						static_cast<qdGameObjectStateWalk*>(m_ptrState));
	}
	else
		m_ptrCurStateAI = NULL;
}

qdGameObjectAnimated* StateCoordsStacker::get_back_obj() const
{
	if (m_ptrSelectedState) 
		return static_cast<qdGameObjectAnimated*>(m_ptrSelectedState->owner());
	return NULL;
}

void StateCoordsStacker::OnSelchangeComboCurStateAnim() 
{
	UpdateEditedStateAnimation();
	UpdateEditedStateAnimControls();
	m_iEditedStateAnimFrame = 0;
	if (ptree_helpers::IsMovementState(m_ptrState)
		&&m_ptrSelectedState
		&&ptree_helpers::IsMovementState(m_ptrSelectedState))
	{
		LoadStateAnimation(m_wndAnim, m_ptrSelectedState);
		OnSelchangeComboAnimation();
	}
	state2wnd();
	UpdateData(FALSE);
	m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
}

void StateCoordsStacker::OnRadio1() 
{
	UpdateData();
	EnablePos(true);
	ShowSyncControls(SW_HIDE);
	state2wnd();
	UpdatePosControls();
	UpdateSceneViewState();
	ShowSyncControls(SW_HIDE);
	WhatShiftEnable(FALSE);
	UpdateSelectedStateCtrls();
	UpdateData(FALSE);
}

void StateCoordsStacker::OnRadio2() 
{
	UpdateData();
//	ShowSyncControls(SW_HIDE);
	state2wnd();
	UpdatePosControls();
	UpdateSceneViewState();
	ShowSyncControls(SW_HIDE);
	const bool b = PermitAccess2WhatShift();
	WhatShiftEnable(b);
//	EnablePos(m_iWhatShift != WS_CENTER);
	UpdateSelectedStateCtrls();
	UpdateData(FALSE);
}

void StateCoordsStacker::OnRadio3() 
{
	UpdateData();
	EnablePos(true);
	ShowSyncControls(SW_HIDE);
	state2wnd();
	UpdatePosControls();
	UpdateSceneViewState();
	WhatShiftEnable(FALSE);
	UpdateSelectedStateCtrls();
	UpdateData(FALSE);
	m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
}

void StateCoordsStacker::OnRadio4() 
{
	UpdateData();
	EnablePos(true);
	ShowSyncControls(SW_SHOW);
	state2wnd();
	UpdatePosControls();
	UpdateSceneViewState();
	WhatShiftEnable(FALSE);
	UpdateSelectedStateCtrls();
	UpdateData(FALSE);
	m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
}
//доступны/недоступны кнопки переключения режимов стыковки
void StateCoordsStacker::UpdateModeCtrls()
{
	//доступна ли стыковка по координате
	BOOL bEnable = !m_ptrSelectedState
					||(m_ptrState->owner()!=m_ptrSelectedState->owner())
					||!m_ptrSelectedState->coords_animation()->is_empty()
					||!m_ptrState->coords_animation()->is_empty();
	GetDlgItem(IDC_RADIO1)->EnableWindow(bEnable);

	//если стыкуются различные направления в походке,
	//то доступна стыковка только смещением
	bEnable = !(ptree_helpers::IsMovementState(m_ptrState)
		&&m_ptrSelectedState&&m_ptrSelectedState==m_ptrState);
	GetDlgItem(IDC_RADIO3)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO4)->EnableWindow(bEnable);
	if (!bEnable) {
		m_iStackMode = SM_STATE_OFFSET;
		UpdateData(FALSE);
		OnRadio2();
	}
}

void StateCoordsStacker::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
}

void StateCoordsStacker::ShowSyncControls(UINT nCmd)
{
	ASSERT(nCmd == SW_HIDE||nCmd == SW_SHOW);

	UINT nOpoziteCmd = nCmd == SW_SHOW ? SW_HIDE: SW_SHOW;

 	GetDlgItem(IDC_STATIC_Z)->ShowWindow(nOpoziteCmd);
	GetDlgItem(IDC_Z_EDIT)->ShowWindow(nOpoziteCmd);
	GetDlgItem(IDC_SPIN_Z)->ShowWindow(nOpoziteCmd);

	GetDlgItem(IDC_CHECK_TOGETHER)->ShowWindow(nCmd);
	UpdateWindow();
}

void StateCoordsStacker::OnBnClickedCheckTogether()
{
	if (!UpdateData())
		return;
	GetDlgItem(IDC_EDIT_X)->SendMessage(ES_READONLY, m_bSyncScale, 0);
	GetDlgItem(IDC_EDIT_Y)->SendMessage(ES_READONLY, m_bSyncScale, 0);
}

void StateCoordsStacker::UpdateCAAnimScroller(CSliderCtrl& wnd, CStatic& wnd_static,
											  qdGameObjectState* pstate, int *iCurFrame)
{
	if (!pstate||pstate->state_type() != qdGameObjectState::STATE_STATIC
		||pstate->coords_animation()->is_empty())
	{
		wnd.EnableWindow(FALSE);
		wnd_static.EnableWindow(FALSE);
		return;
	}
	qdGameObjectStateStatic* ps = static_cast<qdGameObjectStateStatic*>(pstate);
	qdAnimation* pa = ps->animation();
	if (!pa)
	{
		wnd.EnableWindow(FALSE);
		wnd_static.EnableWindow(FALSE);
		return;
	}
	*iCurFrame = pa->get_cur_frame_number();

	wnd.EnableWindow(TRUE);
	wnd_static.EnableWindow(TRUE);

	wnd.SetRange(0, pa->num_frames()-1);
	wnd.SetPos(*iCurFrame);
}

void StateCoordsStacker::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_ENDSCROLL){

		ASSERT(pScrollBar->GetDlgCtrlID()==IDC_SPIN_X
			||pScrollBar->GetDlgCtrlID()==IDC_SPIN_Y
			||pScrollBar->GetDlgCtrlID()==IDC_SPIN_Z);
		UpdateData();
		wnd2state();
		sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

bool StateCoordsStacker::PermitAccess2WhatShift()
{
	if (!m_ptrState->coords_animation()->is_empty()) 
		return true;

	qdGameObjectAnimated* const pa = 
		static_cast<qdGameObjectAnimated*>(m_ptrState->owner());
	if (pa->state_vector().size()==1)
		return true;
	m_iWhatShift = WS_IMAGE;
//	return false;
	return true;
}

void StateCoordsStacker::WhatShiftEnable(BOOL bEnable)
{
	GetDlgItem(IDC_RADIO_TO_CENTER)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_TO_IMAGE)->EnableWindow(bEnable);
}

void StateCoordsStacker::EnablePos(const bool b)
{
	GetDlgItem(IDC_EDIT_X)->EnableWindow(b);
	GetDlgItem(IDC_EDIT_Y)->EnableWindow(b);
	GetDlgItem(IDC_EDIT_Z)->EnableWindow(b);

	GetDlgItem(IDC_SPIN_X)->EnableWindow(b);
	GetDlgItem(IDC_SPIN_Y)->EnableWindow(b);
	GetDlgItem(IDC_SPIN_Z)->EnableWindow(b);
}
void StateCoordsStacker::OnBnClickedRadioToCenter()
{
	if (!UpdateData())
		return;
//	EnablePos(true);
//	m_iX = m_ptrState->center_offset().x;
//	m_iY = m_ptrState->center_offset().y;
//	UpdateData(FALSE);
}

void StateCoordsStacker::OnBnClickedRadioToImage()
{
	if (!UpdateData())
		return;
//	EnablePos(false);
//	m_iX = -m_ptrState->center_offset().x;
//	m_iY = -m_ptrState->center_offset().y;
//	UpdateData(FALSE);
}

void StateCoordsStacker::UpdateSelectedStateCtrls()
{
	if (!m_ptrSelectedState)
	{
		m_iXSelState = 0;
		m_iYSelState = 0;
		m_iZSelState = 0;
		return;
	}

	switch(m_iStackMode) {
	case SM_STATE_OFFSET:
		{
			Vect2s v;
			if (m_ptrSelectedState != m_ptrState)
				v = m_ptrSelectedState->center_offset();
			else
				v = get_walk_dir_offset(m_ptrSelectedState, get_back_obj_state_ai());
			m_iXSelState = v.x;
			m_iYSelState = v.y;
			m_iZSelState = 0;
		}
		break;
	case SM_OBJ_COORDINATE:
		{
			qdGameObjectAnimated* const pobj = 
				static_cast<qdGameObjectAnimated*>(m_ptrSelectedState->owner());
			qdGameObjectState* const pstate = pobj->get_cur_state();
			if (pstate != m_ptrSelectedState)
				pobj->set_state(m_ptrSelectedState);

			const Vect3f *pos = 
				&static_cast<qdGameObjectAnimated*>(m_ptrSelectedState->owner())->R();
			if (!m_ptrSelectedState->coords_animation()->is_empty()) {
				qdCoordsAnimation* pca = m_ptrSelectedState->coords_animation();
				pca->set_time_rel(get_obj_state_anim_phase());
				pos = &pca->get_point(pca->get_cur_point())->dest_pos();
			}

			m_iXSelState = static_cast<int>(pos->x);
			m_iYSelState = static_cast<int>(pos->y);
			m_iZSelState = static_cast<int>(pos->z);

			if (pstate != m_ptrSelectedState)
				pobj->set_state(pstate);
		}
		break;
	case SM_STATE_BOUND:
		{
			if (!m_ptrSelectedState->has_bound())
			{
				if (m_ptrSelectedState->owner()->named_object_type()
					== QD_NAMED_OBJECT_MOVING_OBJ)
					m_ptrSelectedState->set_bound(static_cast<qdGameObjectMoving*>
					(m_ptrSelectedState->owner())->bound());
				m_ptrSelectedState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);
			}
			const Vect3f& b = m_ptrSelectedState->bound();
			m_iXSelState = static_cast<int>(b.x);
			m_iYSelState = static_cast<int>(b.y);
			m_iZSelState = static_cast<int>(b.z);
		}
		break;
	case SM_STATE_SCALE:
		{
			if (m_ptrSelectedState->state_type() == qdGameObjectState::STATE_STATIC)
			{
				qdAnimation* pa = 
					static_cast<qdGameObjectStateStatic*>(m_ptrSelectedState)->
					animation_info()->animation();
				if (pa){
					m_iXSelState = pa->size_x();
					m_iYSelState = pa->size_y();
					m_iZSelState = 0;
				}
			}
			else if (m_ptrSelectedState->state_type() == qdGameObjectState::STATE_WALK){
				qdGameObjectMoving* const po = 
					static_cast<qdGameObjectMoving*>(m_ptrSelectedState->owner());
				ASSERT(po);
				Vect2s const size(po->screen_size());
				m_iXSelState = size.x;
				m_iYSelState = size.y;
				m_iZSelState = 0;
			}
		}
		break;
	}
}

void StateCoordsStacker::UpdateSelectedStateCtrlsAccess()
{

}

int StateCoordsStacker::edited_state_ai_num() const
{
	qdGameObjectMoving* const pobj = 
		dynamic_cast<qdGameObjectMoving*>(m_ptrState->owner());
	if (!pobj)
		return -1;

	if (m_ptrState->state_type() != qdGameObjectState::STATE_WALK)
		return -1;
	qdAnimationSet* const pas = 
		static_cast<qdGameObjectStateWalk*>(m_ptrState)->animation_set();
	return state_ai_num(pas, get_edited_state_ai());
}

int StateCoordsStacker::back_obj_state_ai_num() const
{
	qdGameObjectMoving* const pobj = 
		dynamic_cast<qdGameObjectMoving*>(get_back_obj());
	if (!pobj)
		return -1;

	if (m_ptrSelectedState->state_type() != qdGameObjectState::STATE_WALK)
		return -1;
	qdAnimationSet* const pas = 
		static_cast<qdGameObjectStateWalk*>(m_ptrSelectedState)->animation_set();
	return state_ai_num(pas, get_back_obj_state_ai());
}
