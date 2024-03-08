// EditStateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include <additional/addition.h>
#include "EditStateDlg.h"
#include "curdirsafer.h"

#include "TextIDSelector.h"

#include "qd_sound.h"
#include "qd_animation_set.h"
#include "qd_game_object_state.h"
#include "qd_game_object_animated.h"
#include "qd_game_object_mouse.h"
#include "qd_game_dispatcher.h"
#include "qd_textdb.h"

#include <boost/function_output_iterator.hpp>
#include <boost/rtl.hpp>

#include "ptree_hlprs.hpp"
#include ".\editstatedlg.h"

#include "Main/Utils/SoundPlayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool EditAnimation(HWND hParent, LPCTSTR lpszFileName);

class EditStateDlg::insert_string
{
public:
	insert_string(CCJFlatComboBox& box):combo_(box){}

	void operator()(std::string const& str) const{
		int pos = combo_.AddString(str.c_str());
	}
private:
	CCJFlatComboBox& combo_;///< —юда вставл€ем строку
};

/////////////////////////////////////////////////////////////////////////////
// EditStateDlg dialog
/// ’ранитель текущей директории дл€ выбора текстового файла
CurDirSafer g_curTextFileDirSafe;

CStringSet_t * MakeAnimationList(qdGameDispatcher* ptrDisp, 
								 qdGameDispatcherBase* ptrDispBase);

void AdjustListWidth(CCJFlatComboBox* pCombo);
/// функци€ из файла statebounddlg.cpp
bool EditStateBound(qdGameObjectState* pstate);
bool SetAccelerationAndMaxSpeed(qdGameObjectStateWalk* ps);

int const MAX_SOUND_VOLUME = 255;///< ћаксимальна€ громкость звука

EditStateDlg::EditStateDlg(HWND hOwnedWnd /*=NULL*/)
	: CDialog(EditStateDlg::IDD, NULL)
	, m_bSoundLoop(FALSE)
	, m_iSoundVolume(MAX_SOUND_VOLUME)
	, m_bLoadToMemory(FALSE)
	, m_strShortText(_T(""))
	, m_strFullText(_T(""))
	, textArrangement_(FALSE)
	, shortPhraseID_(_T(""))
	, longPhraseID_(_T(""))
	, isGlobal_(FALSE)
	, fadeMode_(0)
{
	//{{AFX_DATA_INIT(EditStateDlg)
	m_bLoop = FALSE;
	m_bFlipVert = FALSE;
	m_bFlipHorz = FALSE;
	m_iAnimation = -1;
	m_iSound = -1;
	m_bSetPoints = FALSE;
	m_iSpeed = 0;
	m_bGo2First = FALSE;
	m_iStateType = -1;
	m_IsDlgPhrase = FALSE;
	angle_ = angle_speed_ = 0.f;
	scale_x_ = scale_y_ = 1.f;
	scale_x_speed_ = scale_y_speed_ = 1.f;
	//}}AFX_DATA_INIT

	m_ptrState = NULL;

	m_bInitDialog = true;
	m_bUseCoordAnim = true;
	m_bUseStacking = true;
	m_bUseBound = true;
	m_hOwnedWnd = hOwnedWnd;
}

bool EditStateDlg::set_data(qdGameDispatcher* ptrDisp, 
												qdGameObjectAnimated* ptrObj)
{
	m_ptrDisp  = ptrDisp;
	m_ptrObj = ptrObj;
	return true;
}

bool EditStateDlg::set_state(qdGameObjectState* ptrState)
{
	m_ptrState = ptrState;
	m_ptrStateBak.reset(ptrState->clone());
	return true;
}

void EditStateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EditStateDlg)
	DDX_Control(pDX, IDC_COMBO_ANIMATION, m_wndAnim);
	DDX_Check(pDX, IDC_FLAG_LOOP, m_bLoop);
	DDX_Check(pDX, IDC_FLAG_FLIP_VERT, m_bFlipVert);
	DDX_Check(pDX, IDC_FLAG_FLIP_HORZ, m_bFlipHorz);
	DDX_CBIndex(pDX, IDC_COMBO_ANIMATION, m_iAnimation);
	DDX_CBIndex(pDX, IDC_COMBO_SOUND, m_iSound);
	DDX_Check(pDX, IDC_CHECK_SET_POINTS, m_bSetPoints);
	DDX_Text(pDX, IDC_EDIT_SPEED, m_iSpeed);
	DDX_Radio(pDX, IDC_RADIO_STATE_TYPE, m_iStateType);
	DDX_Check(pDX, IDC_CHECK_DLG_PHRASE, m_IsDlgPhrase);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_CURSOR, m_wndCursors);
	DDX_Check(pDX, IDC_CHECK_SOUND_LOOP, m_bSoundLoop);
	DDX_Text(pDX, IDC_EDIT_SOUND, m_iSoundVolume);
	DDX_Check(pDX, IDC_CHECK_LOAD_TO_MEMORY, m_bLoadToMemory);
	DDX_Control(pDX, IDC_COMBO_SHORT_PHRASE_ID, m_wndShortPhraseIds);
	DDX_Control(pDX, IDC_COMBO_FULL_PHRASE_ID, m_wndLongPhraseIds);
	DDX_Text(pDX, IDC_EDIT_SHORT_PHRASE_TEXT, m_strShortText);
	DDX_Text(pDX, IDC_EDIT_LONG_PHRASE_TEXT2, m_strFullText);
	DDX_Radio(pDX, IDC_RADIO_OUTPUT_ARRANGEMENT, textArrangement_);
	DDX_Text(pDX, IDC_SHORT_PHRASE_ID, shortPhraseID_);
	DDX_Text(pDX, IDC_LONG_PHRASE_ID, longPhraseID_);
	DDX_Control(pDX, IDC_COMBO_SOUND, sounds);
	DDX_Control(pDX, IDC_COMBO_SAVE_MODE, autosaveMode_);
	DDX_Control(pDX, IDC_BUTTON_COLOR, textColorWnd_);
	DDX_Control(pDX, IDC_BUTTON_HILITE_COLOR, hiliteColorWnd_);
	DDX_Control(pDX, IDC_COMBO_FONTS, fontsWnd_);
	DDX_Text(pDX, IDC_EDIT_SAVE_ID, autosaveSlot_);
	DDV_MinMaxInt(pDX, autosaveSlot_, 0, INT_MAX);
	DDX_Check(pDX, IDC_CHECK_IS_GLOBAL, isGlobal_);
	DDX_Radio(pDX, IDC_RADIO_FADE_OFF, fadeMode_);
	DDX_Text(pDX, IDC_EDIT_FADE_TIME, fadeTime_);
	DDX_Text(pDX, IDC_EDIT_ANGLE, angle_);
	DDX_Text(pDX, IDC_EDIT_ANGLE_SP, angle_speed_);
	DDX_Text(pDX, IDC_EDIT_SCALE_X, scale_x_);
	DDX_Text(pDX, IDC_EDIT_SCALE_X_SP, scale_x_speed_);
	DDX_Text(pDX, IDC_EDIT_SCALE_Y, scale_y_);
	DDX_Text(pDX, IDC_EDIT_SCALE_Y_SP, scale_y_speed_);
	DDX_Text(pDX, IDC_EDIT_TRANSPARENCY, shadowAlpha_);
	DDX_Control(pDX, IDC_BUTTON_SHADOW_COLOR, shadowColorWnd_);
	DDX_Control(pDX, IDC_COMBO_ALIGN, alignWnd_);
}


BEGIN_MESSAGE_MAP(EditStateDlg, CDialog)
//{{AFX_MSG_MAP(EditStateDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_ANIMATION, OnSelchangeComboAnimation)
	ON_BN_CLICKED(IDC_CHECK_SET_POINTS, OnCheckSetPoints)
	ON_EN_CHANGE(IDC_EDIT_SPEED, OnChangeEditSpeed)
	ON_BN_CLICKED(IDC_BTN_STACK_STATE, OnBtnStackState)
	ON_BN_CLICKED(IDC_RADIO_STATE_TYPE, OnRadioStateType)
	ON_BN_CLICKED(IDC_RADIO_STATE_TYPE2, OnRadioStateType2)
	ON_BN_CLICKED(IDC_BUTTON_DIMS, OnButtonDims)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_SOUND, OnCbnSelchangeComboSound)
	ON_BN_CLICKED(IDC_BUTTON_ACCELERATION, OnBnClickedButtonAcceleration)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_SHORT_PHRASE_ID, OnCbnSelchangeComboShortPhraseId)
	ON_CBN_SELCHANGE(IDC_COMBO_FULL_PHRASE_ID, OnCbnSelchangeComboFullPhraseId)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_SEL_SHORT_PHRASE_ID, OnBnClickedBtnSelShortPhraseId)
	ON_BN_CLICKED(IDC_BTN_SEL_LONG_PHRASE_ID, OnBnClickedBtnSelLongPhraseId)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_SOUND, OnBnClickedButtonPlaySound)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_ANIMATION, OnBnClickedButtonEditAnimation)
	ON_BN_CLICKED(IDC_CHECK_IS_GLOBAL, OnBnClickedCheckIsGlobal)
	ON_CBN_SELCHANGE(IDC_COMBO_SAVE_MODE, OnCbnSelchangeComboSaveMode)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_FADE, OnDeltaposSpinFade)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ANGLE, OnDeltaposSpinAngle)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ANGLE_SP, OnDeltaposSpinAngleSpeed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SCALE_X, OnDeltaposSpinScaleX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SCALE_X_SP, OnDeltaposSpinScaleXSpeed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SCALE_Y, OnDeltaposSpinScaleY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SCALE_Y_SP, OnDeltaposSpinScaleYSpeed)
	ON_EN_CHANGE(IDC_EDIT_TRANSPARENCY, OnEnChangeEditTransparency)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditStateDlg message handlers

void EditStateDlg::animation2wnd()
{
	m_iAnimation = 0;
	qdAnimationInfo* ai = 
		static_cast<qdGameObjectStateStatic*>(m_ptrState)->animation_info();
	CString str(ai->animation_name());
	if(str.IsEmpty())
		return;
	
	m_bFlipVert = ai->check_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL);
	m_bFlipHorz = ai->check_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL);
	m_bLoop		= ai->check_flag(QD_ANIMATION_FLAG_LOOP);
	
	CStringSet_t::const_iterator _itr = m_ptrAnimSet->find(str);

	if(_itr != m_ptrAnimSet->end())
	{
#ifdef _DEBUG
		TRACE(*_itr);
		TRACE0("\n");
#endif
		m_iAnimation = 1 +
			std::distance<CStringSet_t::const_iterator>(m_ptrAnimSet->begin(), _itr);
	}
	if(!m_ptrState->coords_animation()->is_empty())
	{
		//будет выбран пункт координатной анимации
		m_iStateType = ST_CA_STATE;
		m_iSpeed = static_cast<int>(m_ptrState->coords_animation()->speed());

		m_bGo2First = m_ptrState->coords_animation()->
							check_flag(QD_COORDS_ANM_OBJECT_START_FLAG);
	}		
}

void EditStateDlg::gaint2wnd()
{
	m_iAnimation = 0;
	qdNamedObject* asi = 
		static_cast<qdGameObjectStateWalk*>(m_ptrState)->animation_set_info();
	CString str(asi->name());
	if(str.IsEmpty())
		return;
	CStringSet_t::const_iterator _itr = m_ptrPersGaintsSet->find(str);
	if(_itr != m_ptrPersGaintsSet->end())
	{
		m_iAnimation = std::distance<CStringSet_t::const_iterator>(
							m_ptrPersGaintsSet->begin(), _itr) + 1;
	}
	if(!m_ptrState->coords_animation()->is_empty())
	{
		m_iStateType = ST_CA_STATE;

		m_iSpeed = static_cast<int>(m_ptrState->coords_animation()->speed());
		m_bGo2First = m_ptrState->coords_animation()->
							check_flag(QD_COORDS_ANM_OBJECT_START_FLAG);
	}
}

void EditStateDlg::wnd2gaint()
{
	CString strText;
	if(m_iAnimation != 0)
	{
		m_wndAnim.GetLBText(m_iAnimation, strText);
		_ASSERT(!strText.IsEmpty());
		static_cast<qdGameObjectStateWalk*>(m_ptrState)->
			animation_set_info()->set_name(strText);
	}
	else
		static_cast<qdGameObjectStateWalk*>(m_ptrState)->
			animation_set_info()->set_name(NULL);

	if (m_iStateType == ST_NORMAL_STATE) {
		//очищаем координатную анимацию
		m_ptrState->coords_animation()->clear();
	}
	else
	{
		if (m_bGo2First) {
			m_ptrState->coords_animation()->
				set_flag(QD_COORDS_ANM_OBJECT_START_FLAG);
		}
		else
			m_ptrState->coords_animation()->
				drop_flag(QD_COORDS_ANM_OBJECT_START_FLAG);
	}
}
void EditStateDlg::wnd2animation()
{
	CString strText;
	if(m_iAnimation != 0)
	{
		m_wndAnim.GetLBText(m_iAnimation, strText);

		qdAnimationInfo* ai = 
			static_cast<qdGameObjectStateStatic*>(m_ptrState)->animation_info();

		ai->set_animation_name(strText);

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

	}
	else
		static_cast<qdGameObjectStateStatic*>(m_ptrState)->
						animation_info()->set_animation_name(NULL);

	if(m_iStateType == ST_NORMAL_STATE)
		m_ptrState->coords_animation()->clear();
	else
	{
		if (m_bGo2First) 
			m_ptrState->coords_animation()->set_flag(QD_COORDS_ANM_OBJECT_START_FLAG);
		else
			m_ptrState->coords_animation()->drop_flag(QD_COORDS_ANM_OBJECT_START_FLAG);
		
	}
}

BOOL EditStateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!m_hOwnedWnd) 
		m_hOwnedWnd = GetParent()->m_hWnd;

	CString str;
	str.LoadString(IDS_TEXT_ALIGN_LEFT);
	alignWnd_.AddString(str);

	str.LoadString(IDS_TEXT_ALIGN_CENTER);
	alignWnd_.AddString(str);

	str.LoadString(IDS_TEXT_ALIGN_RIGHT);
	alignWnd_.AddString(str);

	autosaveMode_.AddString(CString((LPTSTR)IDS_OFF));
	autosaveMode_.AddString(CString((LPTSTR)IDS_LOAD));
	autosaveMode_.AddString(CString((LPTSTR)IDS_SAVE));

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_FADE))->SetRange(1, 100);

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_ANGLE))->SetRange(1, 100);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_ANGLE_SP))->SetRange(1, 100);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SCALE_X))->SetRange(1, 10);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SCALE_X_SP))->SetRange(1, 10);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SCALE_Y))->SetRange(1, 10);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SCALE_Y_SP))->SetRange(1, 10);

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_TRANSPARENCY))->
		SetRange(QD_NO_SHADOW_ALPHA, QD_SHADOW_ALPHA_MAX);

	LoadStateResources();
	LoadTextIds(m_wndShortPhraseIds);
	LoadTextIds(m_wndLongPhraseIds);
	//создаЄт уникальные списки походок, и аниамаций
	MakeSets();

	SetData2SoundCombo(m_ptrSoundSet.get());
	SetData2CursorCombo();
	loadFonts();

	state2wnd();

	//если редактируем состо€ние из неактивной сцены, то 
	//запрещаем стыковку, коориднатную анимацию и редактирование размеров

	if (!m_bUseCoordAnim) {
		GetDlgItem(IDC_RADIO_STATE_TYPE2)->EnableWindow(m_bUseCoordAnim);
		m_iStateType = ST_NORMAL_STATE;
	}
	SetupCtrls();
	UpdateData(FALSE);
	
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SPEED))->SetRange(0, 32000);

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SOUND))->
							SetRange(0, MAX_SOUND_VOLUME);

	CheckAccess2StackStateBtn();
	m_bInitDialog = false;	

	shadowColorWnd_.EnableWindow(shadowAlpha_ != QD_NO_SHADOW_ALPHA);
	
	CString cap;
	cap.Format(IDS_DLG_CAP_EDIT_STATE, 
		m_ptrState->owner()->name(),m_ptrState->name());
	SetWindowText(cap);

	toggleFormatSettings();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void EditStateDlg::text2wnd(){
	qdTextDB & db = qdTextDB::instance();
	db.load(qd_get_game_dispatcher()->texts_database());

	shortPhraseID_ = m_ptrState->short_text_ID();
	if (shortPhraseID_.IsEmpty())
		shortPhraseID_.LoadString(IDS_NOTHING);
	else
		m_strShortText = db.getText(shortPhraseID_);

	longPhraseID_ = m_ptrState->full_text_ID();
	if (longPhraseID_.IsEmpty())
		longPhraseID_.LoadString(IDS_NOTHING);
	else
		m_strFullText = db.getText(longPhraseID_);

	m_IsDlgPhrase = static_cast<int>(m_ptrState->
				check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_DIALOG_PHRASE));
}
void EditStateDlg::wnd2text(){
	CString nothing;
	nothing.LoadString(IDS_NOTHING);
	if (shortPhraseID_ == nothing)
		m_ptrState->set_short_text_ID(_T(""));
	else
		m_ptrState->set_short_text_ID(shortPhraseID_);

	if (longPhraseID_ == nothing)
		m_ptrState->set_full_text_ID(_T(""));
	else
		m_ptrState->set_full_text_ID(longPhraseID_);

	if (m_IsDlgPhrase)//значит выбрали "ƒа"
	{
		m_ptrState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_DIALOG_PHRASE);
		if (!hasCondition(m_ptrState)) 
		{
			std::auto_ptr<qdCondition> ptr
				(new qdCondition(qdCondition::CONDITION_MOUSE_DIALOG_CLICK));
			if (ptr.get()) 
			{
				if (m_ptrState->add_condition(ptr.get())) 
					ptr.release();
			}
		}
	}
	else
	{
		m_ptrState->drop_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_DIALOG_PHRASE);
		for(int i = 0; i < m_ptrState->conditions_count();){
			const qdCondition* pc = m_ptrState->get_condition(i);
			ASSERT(pc);
			if (pc->type() == qdCondition::CONDITION_MOUSE_DIALOG_CLICK)
				m_ptrState->remove_conditon(i);
			else
				++i;
		}
	}
}

void EditStateDlg::cursor2wnd()
{
	m_wndCursors.SetCurSel(m_ptrState->mouse_cursor_ID()+1);
}

void EditStateDlg::wnd2cursor()
{
	ASSERT(m_wndCursors.GetCurSel() != CB_ERR);
	m_ptrState->set_mouse_cursor_ID(m_wndCursors.GetCurSel()-1);
	int const cursorID = m_ptrState->mouse_cursor_ID();
	if (cursorID != qdGameObjectState::CURSOR_UNASSIGNED)
	{
		qdGameObjectMouse const& mouse = *m_ptrDisp->mouse_object();
		m_ptrState->set_cursor_name(mouse.state_vector()[cursorID]->name());
	}
}

void EditStateDlg::textOutputParams2wnd()
{
	if (!m_ptrState)
	{
		ASSERT(0);
		return;
	}
	qdScreenTextFormat const& format = m_ptrState->text_format();
	textArrangement_ = static_cast<int>(format.arrangement());
	textColorWnd_.SetColor(static_cast<COLORREF>(format.color()));
	hiliteColorWnd_.SetColor(static_cast<COLORREF>(format.hover_color()));
	alignWnd_.SetCurSel(format.alignment());
	if (format.font_type() == QD_FONT_TYPE_NONE)
		fontsWnd_.SetCurSel(0);
	else
	{
		qdFontInfo const* fi = m_ptrDisp->get_font_info(format.font_type());
		if (!fi||fontsWnd_.SelectString(-1, fi->name()) == CB_ERR)
			fontsWnd_.SetCurSel(0);
	}
	isGlobal_ = format.is_global_depend();
}

void EditStateDlg::wnd2textOutputParams()
{
	qdScreenTextFormat format;
	format.set_arrangement(
		static_cast<qdScreenTextFormat::arrangement_t>(textArrangement_));
	format.set_color(getTextColor());
	format.set_hover_color(getHiliteColor());
	format.set_font_type(getFontType());
	format.set_alignment(qdScreenTextFormat::alignment_t(alignWnd_.GetCurSel()));
	format.toggle_global_depend(isGlobal_);
	m_ptrState->set_text_format(format);
}

void EditStateDlg::state2wnd()
{
	if(!m_ptrState)
		return;

	state2sound();

	m_iStateType = ST_NORMAL_STATE;
	if(m_ptrState->state_type() == qdGameObjectState::STATE_WALK)
		gaint2wnd();
	else if(m_ptrState->state_type() == qdGameObjectState::STATE_STATIC)
		animation2wnd();

	m_bLoadToMemory = 
		m_ptrState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_FORCED_LOAD);
	text2wnd();
	cursor2wnd();
	textOutputParams2wnd();

	autosaveMode_.SetCurSel(autosaveMode());
	autosaveSlot_ = m_ptrState->autosave_slot();

	if(m_ptrState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_FADE_IN))
		fadeMode_ = 1;
	else if(m_ptrState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_FADE_OUT))
		fadeMode_ = 2;
	else 
		fadeMode_ = 0;

	fadeTime_ = m_ptrState->fade_time();

	shadowAlpha_ = m_ptrState->shadow_alpha();
	shadowColorWnd_.SetColor(static_cast<COLORREF>(m_ptrState->shadow_color()));

	angle_ = R2G(m_ptrState->transform().angle());
	angle_speed_ = R2G(m_ptrState->transform_speed().angle());

	scale_x_ = m_ptrState->transform().scale().x;
	scale_x_speed_ = m_ptrState->transform_speed().scale().x;
	scale_y_ = m_ptrState->transform().scale().y;
	scale_y_speed_ = m_ptrState->transform_speed().scale().y;
}

void EditStateDlg::state2sound()
{
	if (!m_ptrState->has_sound()) 
		m_iSound = 0;
	else
	{
		CCJFlatComboBox* ptrSoundCombo = 
				static_cast<CCJFlatComboBox*>(GetDlgItem(IDC_COMBO_SOUND));
		m_iSound = ptrSoundCombo->FindString(0, m_ptrState->sound_name());
		if (m_iSound == CB_ERR)
			m_iSound = 0;
		else
			m_iSoundVolume = m_ptrState->sound()->volume();
	}
	if (!m_iSound) 
	{
		GetDlgItem(IDC_CHECK_SOUND_LOOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SOUND)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_SOUND)->EnableWindow(FALSE);
	}
	else
		m_bSoundLoop = m_ptrState->check_sound_flag(qdSoundInfo::LOOP_SOUND_FLAG);
}

void EditStateDlg::sound2state()
{
	if (m_iSound) {
		CCJFlatComboBox* ptrSoundCombo = 
			static_cast<CCJFlatComboBox*>(GetDlgItem(IDC_COMBO_SOUND));
		CString str;
		ptrSoundCombo->GetLBText(m_iSound, str);
		m_ptrState->set_sound_name(str);
		if (m_bSoundLoop)
			m_ptrState->set_sound_flag(qdSoundInfo::LOOP_SOUND_FLAG);
		else
			m_ptrState->drop_sound_flag(qdSoundInfo::LOOP_SOUND_FLAG);
		m_ptrState->sound()->set_volume(m_iSoundVolume);
	}
	else
	{
		m_ptrState->set_sound_name(NULL);
		m_ptrState->drop_sound_flag(qdSoundInfo::LOOP_SOUND_FLAG);
	}
}

void EditStateDlg::wnd2state()
{
	if(!m_ptrState)
		return;

	sound2state();
	//убиваем этот флаг, чтобы потом,если надо восстановить его
	if(m_ptrState->state_type() == qdGameObjectState::STATE_WALK)
		wnd2gaint();
	else if(m_ptrState->state_type() == qdGameObjectState::STATE_STATIC)
		wnd2animation();

	if (m_bLoadToMemory)
		m_ptrState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_FORCED_LOAD);
	else
		m_ptrState->drop_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_FORCED_LOAD);
	wnd2text();
	wnd2cursor();
	wnd2textOutputParams();

	m_ptrState->drop_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_AUTO_LOAD | qdGameObjectState::QD_OBJ_STATE_FLAG_AUTO_SAVE);
	switch(autosaveMode_.GetCurSel()){
	case 1:
		m_ptrState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_AUTO_LOAD);
		m_ptrState->set_autosave_slot(autosaveSlot_);
		break;
	case 2:
		m_ptrState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_AUTO_SAVE);
		m_ptrState->set_autosave_slot(autosaveSlot_);
		break;
	}

	m_ptrState->drop_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_FADE_IN | qdGameObjectState::QD_OBJ_STATE_FLAG_FADE_OUT);

	switch(fadeMode_){
	case 1:
		m_ptrState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_FADE_IN);
		break;
	case 2:
		m_ptrState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_FADE_OUT);
		break;
	}

	m_ptrState->set_fade_time(fadeTime_);

	m_ptrState->set_shadow(shadowColorWnd_.GetColor(), shadowAlpha_);

	qdScreenTransform transform = m_ptrState->transform();
	transform.set_angle(G2R(angle_));
	transform.set_scale(Vect2f(scale_x_, scale_y_));
	m_ptrState->set_transform(transform);

	transform = m_ptrState->transform_speed();
	transform.set_angle(G2R(angle_speed_));
	transform.set_scale(Vect2f(scale_x_speed_, scale_y_speed_));
	m_ptrState->set_transform_speed(transform);
}

bool EditStateDlg::MakeSets()
{
	m_ptrSoundSet = merge_lists(m_ptrDisp, 
					static_cast<qdGameDispatcherBase*>(m_ptrObj->owner()),
					qdGameDispatcherBase::sound_list);
	if(m_ptrState->state_type() == qdGameObjectState::STATE_STATIC)
	{
		m_ptrAnimSet = merge_lists(m_ptrDisp,
						static_cast<qdGameDispatcherBase*>(m_ptrObj->owner()),
						qdGameDispatcherBase::animation_list);
		if(!m_ptrAnimSet.get())
			return false;
	}
	else if(m_ptrState->state_type() == qdGameObjectState::STATE_WALK)
	{
		m_ptrPersGaintsSet = merge_lists(m_ptrDisp,
						static_cast<qdGameDispatcherBase*>(m_ptrObj->owner()),
						qdGameDispatcherBase::animation_set_list);
		if(!m_ptrPersGaintsSet.get())
			return false;
	}
	return true;
}

void EditStateDlg::OnOK() 
{
	if(!UpdateData())
		return;

	wnd2state();
	CDialog::OnOK();
}


void EditStateDlg::SetupCtrls()
{
	GetDlgItem(IDC_BUTTON_EDIT_ANIMATION)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PLAY_SOUND)->EnableWindow(FALSE);

	if (m_ptrState->state_type() == qdGameObjectState::STATE_MASK) {
		SetupCtrls4StateMask();
	}
	else if(m_ptrState->state_type() == qdGameObjectState::STATE_WALK)
	{
		if(m_iStateType == ST_CA_STATE)
			SetupCtrls4CoordAnim();
		else
			SetupCtrls4PersGaints();
		GetDlgItem(IDC_BUTTON_PLAY_SOUND)->EnableWindow(m_iSound);
	}
	else if(m_ptrState->state_type() == qdGameObjectState::STATE_STATIC)
	{
		switch(m_iStateType) {
		case ST_NORMAL_STATE:
			SetupCtrls4Anim();
			break;
		case ST_CA_STATE:
			SetupCtrls4CoordAnim();
			break;
		}
		GetDlgItem(IDC_BUTTON_EDIT_ANIMATION)->EnableWindow(m_iAnimation);
		GetDlgItem(IDC_BUTTON_PLAY_SOUND)->EnableWindow(m_iSound);
	}

	if(!autosaveMode())
		GetDlgItem(IDC_EDIT_SAVE_ID)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_EDIT_SAVE_ID)->EnableWindow(TRUE);
}

void EditStateDlg::SetupCtrls4StateMask()
{
	GetDlgItem(IDC_COMBO_ANIMATION)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_FLAG_LOOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_FLAG_FLIP_HORZ)->EnableWindow(FALSE);
	GetDlgItem(IDC_FLAG_FLIP_VERT)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_CHECK_SET_POINTS)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_SPIN_SPEED)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SPEED)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_BUTTON_DIMS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ACCELERATION)->EnableWindow(FALSE);
}

void EditStateDlg::SetupCtrls4Anim()
{
	GetDlgItem(IDC_COMBO_ANIMATION)->EnableWindow(TRUE);
	
	GetDlgItem(IDC_FLAG_LOOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_FLAG_FLIP_HORZ)->EnableWindow(TRUE);
	GetDlgItem(IDC_FLAG_FLIP_VERT)->EnableWindow(TRUE);

	GetDlgItem(IDC_CHECK_SET_POINTS)->EnableWindow(FALSE);

	GetDlgItem(IDC_SPIN_SPEED)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SPEED)->EnableWindow(FALSE);

	GetDlgItem(IDC_COMBO_SOUND)->EnableWindow(TRUE);
	
	CString str((LPTSTR)IDC_STATIC_ANIM_CAPTION1);
	GetDlgItem(IDC_STATIC_ANIM)->SetWindowText(str);

	GetDlgItem(IDC_BUTTON_DIMS)->EnableWindow(m_bUseBound);
	GetDlgItem(IDC_BUTTON_ACCELERATION)->EnableWindow(FALSE);
	
	SetData2AnimCombo(m_ptrAnimSet.get());
	m_iSpeed = 0;
}

void EditStateDlg::SetupCtrls4CoordAnim()
{
	if (m_ptrState->state_type() == qdGameObjectState::STATE_WALK) {
		GetDlgItem(IDC_COMBO_ANIMATION)->EnableWindow(TRUE);
		
		GetDlgItem(IDC_FLAG_LOOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_FLAG_FLIP_HORZ)->EnableWindow(FALSE);
		GetDlgItem(IDC_FLAG_FLIP_VERT)->EnableWindow(FALSE);
		
		
		GetDlgItem(IDC_COMBO_SOUND)->EnableWindow(TRUE);
		
		CString str((LPTSTR)IDC_STATIC_ANIM_CAPTION2);
		GetDlgItem(IDC_STATIC_ANIM)->SetWindowText(str);
		
		SetData2AnimCombo(m_ptrPersGaintsSet.get());

		GetDlgItem(IDC_BUTTON_ACCELERATION)->EnableWindow(TRUE);
	}else if(m_ptrState->state_type() == qdGameObjectState::STATE_STATIC){
		GetDlgItem(IDC_COMBO_ANIMATION)->EnableWindow(TRUE);

		GetDlgItem(IDC_FLAG_LOOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_FLAG_FLIP_HORZ)->EnableWindow(TRUE);
		GetDlgItem(IDC_FLAG_FLIP_VERT)->EnableWindow(TRUE);
		
		
		GetDlgItem(IDC_COMBO_SOUND)->EnableWindow(TRUE);
		
		CString str((LPTSTR)IDC_STATIC_ANIM_CAPTION1);
		GetDlgItem(IDC_STATIC_ANIM)->SetWindowText(str);

		SetData2AnimCombo(m_ptrAnimSet.get());

		GetDlgItem(IDC_BUTTON_ACCELERATION)->EnableWindow(FALSE);
	}
	GetDlgItem(IDC_BUTTON_DIMS)->EnableWindow(m_bUseBound);
	GetDlgItem(IDC_SPIN_SPEED)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SPEED)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_SET_POINTS)->EnableWindow(m_bEditCoordAnim);
	m_iSpeed = static_cast<int>(m_ptrState->coords_animation()->speed());
}

void EditStateDlg::SetupCtrls4PersGaints()
{
	GetDlgItem(IDC_COMBO_ANIMATION)->EnableWindow(TRUE);

	GetDlgItem(IDC_FLAG_LOOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_FLAG_FLIP_HORZ)->EnableWindow(FALSE);
	GetDlgItem(IDC_FLAG_FLIP_VERT)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_CHECK_SET_POINTS)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_SPIN_SPEED)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SPEED)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_COMBO_SOUND)->EnableWindow(TRUE);
	
	CString str((LPTSTR)IDC_STATIC_ANIM_CAPTION2);
	GetDlgItem(IDC_STATIC_ANIM)->SetWindowText(str);

	GetDlgItem(IDC_BUTTON_DIMS)->EnableWindow(m_bUseBound);
	SetData2AnimCombo(m_ptrPersGaintsSet.get());
	GetDlgItem(IDC_BUTTON_ACCELERATION)->EnableWindow(TRUE);
	m_iSpeed = 0;
}

void EditStateDlg::SetData2AnimCombo(CStringSet_t* ptrSet)
{
	CCJFlatComboBox* ptrWnd = ((CCJFlatComboBox*)GetDlgItem(IDC_COMBO_ANIMATION));
	ptrWnd->ResetContent();
	if(!ptrSet)
		return;
	
	CStringSet_t::iterator _itr = ptrSet->begin(), _end = ptrSet->end();
	for(; _itr != _end; ++_itr)
		ptrWnd->AddString(*_itr);
	CString strNoThing((LPTSTR)IDS_NOTHING);
	ptrWnd->InsertString(0, strNoThing);
	AdjustListWidth(ptrWnd);
}

void EditStateDlg::SetData2SoundCombo(CStringSet_t* ptrSet)
{
	CCJFlatComboBox* ptrWnd = ((CCJFlatComboBox*)GetDlgItem(IDC_COMBO_SOUND));
	ptrWnd->ResetContent();
	if(!ptrSet)
		return;
	
	CStringSet_t::iterator _itr = ptrSet->begin(), _end = ptrSet->end();
	for(; _itr != _end; ++_itr)
		ptrWnd->AddString(*_itr);
	CString strNoThing((LPTSTR)IDS_NOTHING);
	ptrWnd->InsertString(0, strNoThing);

	AdjustListWidth(ptrWnd);
}

void EditStateDlg::SetData2CursorCombo()
{
	qdGameObjectMouse* pmouse = m_ptrDisp->mouse_object();
	const qdGameObjectStateVector& v = pmouse->state_vector();
	qdGameObjectStateVector::const_iterator i = v.begin(), e = v.end();
	for(;i != e; ++i)
		m_wndCursors.AddString((*i)->name());

	m_wndCursors.InsertString(0, CString((LPTSTR)IDS_DEFAULT_CURSOR_NAME));
	AdjustListWidth(&m_wndCursors);
}

void EditStateDlg::OnSelchangeComboAnimation() 
{
	if(!UpdateData())
		return ;
	if (m_ptrState->state_type() == qdGameObjectState::STATE_STATIC)
		GetDlgItem(IDC_BUTTON_EDIT_ANIMATION)->EnableWindow(m_iAnimation);
	else
		GetDlgItem(IDC_BUTTON_EDIT_ANIMATION)->EnableWindow(FALSE);

}
											
void EditStateDlg::OnCheckSetPoints() 
{
	if(!UpdateData())
		return;
	if(::SendMessage(m_hOwnedWnd, WM_SET_COORD_ANIMATION_MODE, 
			m_bSetPoints, (LPARAM)static_cast<IRequestingStateEdit*>(this)))
	{
		m_ptrState->coords_animation()->set_cur_point(0);
		GetParent()->EnableWindow(TRUE);
		m_bSetPoints = FALSE;
		UpdateData(FALSE);
		ShowWindow(SW_HIDE);
	}
}

void EditStateDlg::EndEditCoordAnimation()
{
	m_bSetPoints = FALSE;
	if(::SendMessage(m_hOwnedWnd, WM_SET_COORD_ANIMATION_MODE, 
		m_bSetPoints, (LPARAM)static_cast<IRequestingStateEdit*>(this)))
	{
		GetParent()->EnableWindow(FALSE);
		EnableWindow(TRUE);
		UpdateData(FALSE);
		ShowWindow(SW_SHOW);
	}
	CheckAccess2StackStateBtn();
}

qdCoordsAnimation& EditStateDlg::GetCoordsAnimation()
{
	return *m_ptrState->coords_animation();
}

void EditStateDlg::OnChangeEditSpeed() 
{
	if(m_bInitDialog)
		return;
	if(!UpdateData())
		return;
	m_ptrState->coords_animation()->set_speed(static_cast<float>(m_iSpeed));
}

void EditStateDlg::OnButtonDims() 
{
	if (m_ptrState->state_type() == qdGameObjectState::STATE_STATIC)
	{
		if(!static_cast<qdGameObjectStateStatic*>(m_ptrState)->
			animation_info()->animation_name()) {
				AfxMessageBox(IDS_ERR_NO_ANIMATION_IN_STATE);
				return;
			}
	}
	if (m_ptrState->state_type() == qdGameObjectState::STATE_WALK) {
		if (!static_cast<qdGameObjectStateWalk*>(m_ptrState)->animation_set()
			||!static_cast<qdGameObjectStateWalk*>(m_ptrState)->
			animation_set()->size()) {
				AfxMessageBox(IDS_ERR_NO_GAIT_IN_STATE);
				return;
			}
	}
	wnd2state();

	if(::SendMessage(m_hOwnedWnd, WM_STATE_DIMS, TRUE, 
										(LPARAM)static_cast<IRequestingStateEdit*>(this)))
	{
		GetParent()->EnableWindow(TRUE);
		ShowWindow(SW_HIDE);
	}
}

void EditStateDlg::EndEditStateDims(){
	if(::SendMessage(m_hOwnedWnd, WM_STATE_DIMS, FALSE, 
									(LPARAM)static_cast<IRequestingStateEdit*>(this)))
	{
		GetParent()->EnableWindow(FALSE);
		EnableWindow(TRUE);
		ShowWindow(SW_SHOW);
	}
}

void EditStateDlg::OnBtnStackState() 
{
	if (m_ptrState->state_type() == qdGameObjectState::STATE_STATIC)
	{
		if(!static_cast<qdGameObjectStateStatic*>(m_ptrState)->
										animation_info()->animation_name()) {
			AfxMessageBox(IDS_ERR_NO_ANIMATION_IN_STATE);
			return;
		}
	}
	if (m_ptrState->state_type() == qdGameObjectState::STATE_WALK) {
		if (!static_cast<qdGameObjectStateWalk*>(m_ptrState)->animation_set()
			||!static_cast<qdGameObjectStateWalk*>(m_ptrState)->
													animation_set()->size()) {
			AfxMessageBox(IDS_ERR_NO_GAIT_IN_STATE);
			return;
		}
	}

	wnd2state();

	if(::SendMessage(m_hOwnedWnd, WM_STACK_STATE, TRUE, 
				(LPARAM)static_cast<IRequestingStateEdit*>(this)))
	{
		GetParent()->EnableWindow(TRUE);
		ShowWindow(SW_HIDE);
	}
}

qdGameObjectState* EditStateDlg::GetState()
{
	return m_ptrState;
}

void EditStateDlg::EndStackState()
{
	if(::SendMessage(m_hOwnedWnd, WM_STACK_STATE, FALSE, 
						(LPARAM)static_cast<IRequestingStateEdit*>(this)))
	{
		GetParent()->EnableWindow(FALSE);
		EnableWindow(TRUE);
		ShowWindow(SW_SHOW);
	}
}

void EditStateDlg::CheckAccess2StackStateBtn()
{
	if(m_ptrState->state_type() == qdGameObjectState::STATE_MASK
		||!m_bUseStacking)
	{
		GetDlgItem(IDC_BTN_STACK_STATE)->EnableWindow(FALSE);
		return;
	}
}

void EditStateDlg::OnRadioStateType() 
{
	if(!UpdateData())
		return;

	SetupCtrls();
	CheckAccess2StackStateBtn();
	UpdateData(FALSE);
}

void EditStateDlg::OnRadioStateType2() 
{
	if(!UpdateData())
		return;

	SetupCtrls();
	CheckAccess2StackStateBtn();
	UpdateData(FALSE);
}

void EditStateDlg::OnCancel() 
{
	*m_ptrState = *m_ptrStateBak.get();	

	CDialog::OnCancel();
}

void EditStateDlg::OnCbnSelchangeComboSound()
{
	UpdateData();
	GetDlgItem(IDC_CHECK_SOUND_LOOP)->EnableWindow(m_iSound);
	GetDlgItem(IDC_EDIT_SOUND)->EnableWindow(m_iSound);
	GetDlgItem(IDC_SPIN_SOUND)->EnableWindow(m_iSound);
	GetDlgItem(IDC_BUTTON_PLAY_SOUND)->EnableWindow(m_iSound);
	if (!m_iSound) 
		m_bSoundLoop = FALSE;
	else
	{
		CCJFlatComboBox* ptrSoundCombo = 
			static_cast<CCJFlatComboBox*>(GetDlgItem(IDC_COMBO_SOUND));
		CString str;
		ptrSoundCombo->GetLBText(m_iSound, str);
		m_ptrState->set_sound_name(str);

		if (qdSound const* ps = m_ptrState->sound())
		{
			m_iSoundVolume = ps->volume();
		}
	}

	UpdateData(FALSE);
}


void EditStateDlg::OnBnClickedButtonAcceleration()
{
	SetAccelerationAndMaxSpeed(static_cast<qdGameObjectStateWalk*>(m_ptrState));
}

void EditStateDlg::OnDestroy()
{
	__super::OnDestroy();
	FreeStateResources();
}

void EditStateDlg::LoadStateResources()
{
	if (!m_ptrState) 
		return;
	m_ptrState->load_resources();
	if (m_ptrState->has_sound()&&m_ptrState->sound())
		m_ptrState->sound()->load_resource();
}

void EditStateDlg::FreeStateResources()
{
	if (!m_ptrState) 
		return;
	if (m_ptrState->has_sound()&&m_ptrState->sound())
		m_ptrState->sound()->free_resource();
	m_ptrState->free_resources();
}

void EditStateDlg::loadFonts()
{
	boost::rtl::crange<qdFontInfoList const> r(m_ptrDisp->fonts_list());
	for(; r; ++r)
	{
		int item = fontsWnd_.AddString((*r)->name());
		if (item != CB_ERR)
			fontsWnd_.SetItemDataPtr(item, *r);
	}
	CString str;
	str.LoadString(IDS_DEFAULT_FONT);
	fontsWnd_.InsertString(0, str);
}

void EditStateDlg::LoadTextIds(CCJFlatComboBox& box)
{
	qdTextDB & db = qdTextDB::instance();
	db.load(qd_get_game_dispatcher()->texts_database());
	qdTextDB::IdList lst;
	if (!db.getIdList(lst))
		return;
	lst.sort();
	std::copy(lst.begin(), lst.end(),
		boost::make_function_output_iterator(insert_string(box)));
	CString str;
	str.LoadString(IDS_NOTHING);
	box.InsertString(0, str);
	box.SetCurSel(0);
}

void EditStateDlg::setTextId2wnd(CCJFlatComboBox& box, LPCTSTR id)
{
	int item = box.FindString(1, id);
	if (item == CB_ERR)
		item = 0;
	box.SetCurSel(item);
	SendMessage(WM_COMMAND, MAKELONG(box.GetDlgCtrlID(), CBN_SELCHANGE),
		reinterpret_cast<LPARAM>(box.m_hWnd));
}

void EditStateDlg::OnCbnSelchangeComboShortPhraseId()
{
	int const curSel = m_wndShortPhraseIds.GetCurSel();
	switch(curSel) {
	case CB_ERR:
		m_wndShortPhraseIds.SetCurSel(0);
	case 0:
		m_strShortText.Empty();
		break;
	default:
		{
			CString str;
			m_wndShortPhraseIds.GetWindowText(str);
			m_strShortText = qdTextDB::instance().getText(str);
		}
	}
	UpdateData(FALSE);
}

void EditStateDlg::OnCbnSelchangeComboFullPhraseId()
{
	int const curSel = m_wndLongPhraseIds.GetCurSel();
	switch(curSel) {
	case CB_ERR:
		m_wndLongPhraseIds.SetCurSel(0);
	case 0:
		m_strFullText.Empty();
		break;
	default:
		{
			CString str;
			m_wndLongPhraseIds.GetWindowText(str);
			m_strFullText = qdTextDB::instance().getText(str);
		}
	}
	UpdateData(FALSE);
}

CString EditStateDlg::getIdText(CCJFlatComboBox& box)
{
	int const curSel = box.GetCurSel();
	CString res;
	if(curSel) 
		box.GetWindowText(res);
	return res;
}

COLORREF EditStateDlg::getHiliteColor() const
{
	return hiliteColorWnd_.GetColor();
}
COLORREF EditStateDlg::getTextColor() const
{
	return textColorWnd_.GetColor();
}

int EditStateDlg::getFontType() const{
	int curSel = fontsWnd_.GetCurSel();
	if (curSel == CB_ERR||curSel == 0)
		return QD_FONT_TYPE_NONE;
	qdFontInfo const* fi = 
		reinterpret_cast<qdFontInfo*>(fontsWnd_.GetItemDataPtr(curSel));
	return fi->type();
}

bool EditStateDlg::hasCondition(qdGameObjectState const* state) const
{
	int const nCount = state->conditions_count();
	for(int i = 0; i < nCount; ++i){
		qdCondition const* const condition = state->get_condition(i);
		if (condition->type() == qdCondition::CONDITION_MOUSE_DIALOG_CLICK) 
			return true;
	}
	return false;
}

HBRUSH EditStateDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void EditStateDlg::OnBnClickedBtnSelShortPhraseId()
{
	CString nothing;
	nothing.LoadString(IDS_NOTHING);

	qdTextDB & db = qdTextDB::instance();
	CString textID(shortPhraseID_);
	if (textID.IsEmpty())
		textID = nothing;

	if (TextIdSelector::run(*this, &db, textID))
	{
		shortPhraseID_ = textID;
		if (shortPhraseID_ != nothing)
			m_strShortText = db.getText(shortPhraseID_);
		else
			m_strShortText.Empty();
		UpdateData(FALSE);
	}
}

void EditStateDlg::OnBnClickedBtnSelLongPhraseId()
{
	CWaitCursor wait;
	CString nothing;
	nothing.LoadString(IDS_NOTHING);

	qdTextDB & db = qdTextDB::instance();
	CString textID(longPhraseID_);
	if (textID.IsEmpty())
		textID = nothing;

	if (TextIdSelector::run(*this, &db, textID))
	{
		longPhraseID_ = textID;
		if (longPhraseID_ != nothing)
			m_strFullText = db.getText(longPhraseID_);
		else
			m_strFullText.Empty();
		UpdateData(FALSE);
	}
}


void EditStateDlg::OnBnClickedButtonPlaySound()
{
	UpdateData();
	qdSound* sound = getSound();
	if (!sound)
		return;

	SoundPlayer::play(sound, m_iSoundVolume);
}

void EditStateDlg::OnBnClickedButtonEditAnimation()
{
	//дл€ состо€ний отличных от статических 
	//этот пункт должен быть недоступен
	qdAnimation* animation = getAnimation();
	if (!animation)
		return;
	LPCTSTR lpszFileName = animation->qda_file();
	if (!lpszFileName) 
	{
		AfxMessageBox(IDS_ERR_NO_ANIMATION_FOUND);
		return;
	}

	CWaitCursor __wait;
	::EditAnimation(*this, lpszFileName);
}

void EditStateDlg::OnEnChangeEditTransparency()
{
	if (m_bInitDialog) 
		return;
	UpdateData();
	shadowColorWnd_.EnableWindow(shadowAlpha_ != QD_NO_SHADOW_ALPHA);
}

qdSound* EditStateDlg::getSound()
{
	int curSel = sounds.GetCurSel();
	if (curSel == CB_ERR||curSel == 0)
		return NULL;
	CString soundName;
	sounds.GetLBText(curSel, soundName);

	assert(m_ptrObj);
	qdGameDispatcherBase* disp = static_cast<qdGameDispatcherBase*>(m_ptrObj->owner());
	if (disp)
	{
		qdSound* snd = disp->get_sound(soundName);
		if (snd)
			return snd;
	}

	return m_ptrDisp->get_sound(soundName);
}

qdAnimation* EditStateDlg::getAnimation()
{
	int curSel = m_wndAnim.GetCurSel();
	if (curSel == CB_ERR||curSel == 0)
		return NULL;
	CString name;
	m_wndAnim.GetLBText(curSel, name);

	assert(m_ptrObj);
	qdGameDispatcherBase* disp = static_cast<qdGameDispatcherBase*>(m_ptrObj->owner());
	if (disp)
	{
		qdAnimation* anim = disp->get_animation(name);
		if (anim)
			return anim;
	}

	return m_ptrDisp->get_animation(name);
}

void EditStateDlg::toggleFormatSettings()
{
	textColorWnd_.EnableWindow(!isGlobal_);
	hiliteColorWnd_.EnableWindow(!isGlobal_);
	fontsWnd_.EnableWindow(!isGlobal_);

	CWnd* dlg_elem = GetDlgItem(IDC_RADIO_OUTPUT_ARRANGEMENT);
	if (dlg_elem) dlg_elem->EnableWindow(!isGlobal_);
	else ASSERT(0);

	dlg_elem = GetDlgItem(IDC_RADIO_OUTPUT_ARRANGEMENT2);
	if (dlg_elem) dlg_elem->EnableWindow(!isGlobal_);
	else ASSERT(0);
}

int EditStateDlg::autosaveMode()
{
	if(!m_ptrState)
		return 0;

	if(m_ptrState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_AUTO_LOAD))
		return 1;

	if(m_ptrState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_AUTO_SAVE))
		return 2;

	return 0;
}

void EditStateDlg::OnBnClickedCheckIsGlobal()
{
	UpdateData();
	toggleFormatSettings();
}

void EditStateDlg::OnCbnSelchangeComboSaveMode()
{
	if(m_bInitDialog)
		return;

	UpdateData();

	if(!autosaveMode_.GetCurSel())
		GetDlgItem(IDC_EDIT_SAVE_ID)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_EDIT_SAVE_ID)->EnableWindow(TRUE);
}

void EditStateDlg::OnDeltaposSpinFade(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}
	fadeTime_ += pNMUpDown->iDelta / 100.f;
	if(fadeTime_ < 0.f)
		fadeTime_ = 0.f;
	UpdateData(FALSE);

	*pResult = 0;
}

void EditStateDlg::OnDeltaposSpinAngle(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}
	angle_ += pNMUpDown->iDelta;
	UpdateData(FALSE);

	*pResult = 0;
}

void EditStateDlg::OnDeltaposSpinAngleSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}
	angle_speed_ += pNMUpDown->iDelta;
	UpdateData(FALSE);

	*pResult = 0;
}

void EditStateDlg::OnDeltaposSpinScaleX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}
	scale_x_ += pNMUpDown->iDelta / 10.f;
	UpdateData(FALSE);

	*pResult = 0;
}

void EditStateDlg::OnDeltaposSpinScaleXSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}
	scale_x_speed_ += pNMUpDown->iDelta / 10.f;
	UpdateData(FALSE);

	*pResult = 0;
}

void EditStateDlg::OnDeltaposSpinScaleY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}
	scale_y_ += pNMUpDown->iDelta / 10.f;
	UpdateData(FALSE);

	*pResult = 0;
}

void EditStateDlg::OnDeltaposSpinScaleYSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}
	scale_y_speed_ += pNMUpDown->iDelta / 10.f;
	UpdateData(FALSE);

	*pResult = 0;
}
