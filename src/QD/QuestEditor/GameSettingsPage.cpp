// WPgGlobSet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "gamesettingspage.h"
#include "qd_setup.h"
#include "qd_camera_mode.h"
#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"
#include "qd_textdb.h"
#include "qd_game_dispatcher.h"

#include "custom_ddx.h"
#include "file_tests.h"
#include "disk_op.h"
#include "CurDirSafer.h"

#include <algorithm>

#include "iscreen2combo.h"

#include <boost/function_output_iterator.hpp>
#include <boost/rtl.hpp>

#include ".\gamesettingspage.h"

void SetProjChange();

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(GameSettingsPage, CBasePage)
/////////////////////////////////////////////////////////////////////////////
// WPgGlobSet property page

namespace{

CurDirSafer& GetTextDBCurDirSafer()
{
	static CurDirSafer safer;
	return safer;
}

}
VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);

GameSettingsPage::GameSettingsPage() : CBasePage(GameSettingsPage::IDD)
, m_iScrollSpeed(1)
, m_iScroolDistance(1)
, compression_(0)
, hallOfFameSize_(0)
, m_strGameName(_T(""))
, m_strDBFileName(_T(""))
, m_strSerialKey(_T(""))
, needSceneRedraw_(FALSE)
, textHorizontal_(TRUE)
, topicTextHorizontal_(TRUE)
, m_boolNeedSaveScreenshot(FALSE)
{
	//{{AFX_DATA_INIT(WPgGlobSet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_ptrGameDisp = NULL;
}

bool GameSettingsPage::set_data(qdGameDispatcher* ptrDisp)
{
	m_ptrGameDisp = ptrDisp;
	return true;
}
GameSettingsPage::~GameSettingsPage()
{
}

void GameSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(WPgGlobSet)
	DDX_Control(pDX, IDC_RESOLUTION, m_combo);
	DDX_Control(pDX, IDC_COMBO_SCREEN, m_wndScreens);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_SCENES, m_wndScenes);
	//	DDX_Text(pDX, IDC_EDIT_SCROLL_SPEED, m_iScrollSpeed);
	//	DDX_Text(pDX, IDC_EDIT_SCROLL_DIST, m_iScroolDistance);
	DDX_Control(pDX, IDC_COMBO_SCR_FOR_INV, m_wndScrn4Inv);
	DDX_Control(pDX, IDC_COMBO_SCR_WITHOUT_INV, m_wndScrnWithoutInv);
	DDX_Control(pDX, IDC_COMBO_SAVE_PROMPT_SCR, m_wndSavePromptScrn);
	DDX_Control(pDX, IDC_COMBO_SAVE_NAME_SCR, saveTitleScreen);
	DDX_Text(pDX, IDC_EDIT_GAME_NAME, m_strGameName);
	custom_ddx::DDX_Text(pDX, IDC_EDIT_DB_FILE, m_strDBFileName, 
		file_tests::non_strict_file_test());
	DDX_Text(pDX, IDC_EDIT_SERIAL_KEY, m_strSerialKey);
	if (!m_strSerialKey.IsEmpty())
	{
		if (m_strSerialKey.GetLength() != 32)
		{
			CString str;
			str.Format(IDS_CD_KEY_WRONG_LEN, m_strSerialKey.GetLength());
			AfxMessageBox(str);
			pDX->Fail();
		}
	}
	DDX_Check(pDX, IDC_CHECK_NEED_SCENE_REDRAW, needSceneRedraw_);
	// Настройки дефолтового формата текста
	DDX_Radio(pDX, IDC_RADIO_ALIGN_HORIZONTAL, textHorizontal_);
	DDX_Control(pDX, IDC_BUTTON_COLOR, textColorWnd_);
	DDX_Control(pDX, IDC_BUTTON_HILITE_COLOR, hiliteColorWnd_);
	DDX_Control(pDX, IDC_COMBO_FONT, fontsWnd_);
	DDX_Control(pDX, IDC_COMBO_ALIGN, alignWnd_);
	//	DDX_Control(pDX, IDC_COMBO_DEFAULT_FONT, defaultFont_);
	// Настройки дефолтового формата текста тем
	DDX_Radio(pDX, IDC_RADIO_ALIGN_HORIZONTAL2, topicTextHorizontal_);
	DDX_Control(pDX, IDC_BUTTON_COLOR2, topicTextColorWnd_);
	DDX_Control(pDX, IDC_BUTTON_HILITE_COLOR2, topicHiliteColorWnd_);
	DDX_Control(pDX, IDC_COMBO_FONT2, topicFontsWnd_);
	DDX_Control(pDX, IDC_COMBO_ALIGN2, topicAlignWnd_);
	DDX_Control(pDX, IDC_COMBO_SAVE_FONT, m_wndSaveFont);
	DDX_Control(pDX, IDC_CHECK_NEED_SAVE_TIME_SHOW, m_wndNeedSaveTimeShow);
	DDX_Control(pDX, IDC_CHECK_NEED_SAVE_NAME_SHOW, m_wndNeedSaveTitleShow);
	DDX_Check(pDX, IDC_CHECK_NEED_SAVE_SCREENSHOT, m_boolNeedSaveScreenshot);
	DDX_Control(pDX, IDC_BUTTON_SAVE_FONT_COLOR, saveFontColorWnd_);
	DDX_Slider(pDX, IDC_SLIDER_COMPRESSION, compression_);
	DDX_Text(pDX, IDC_EDIT_HALL_OF_FAME_SIZE, hallOfFameSize_);
}

BEGIN_MESSAGE_MAP(GameSettingsPage, CBasePage)
	//{{AFX_MSG_MAP(WPgGlobSet)
	ON_CBN_SELCHANGE(IDC_RESOLUTION, OnSelchangeResolution)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERA_MODE, OnCbnSelchangeComboCameraMode)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
	ON_EN_CHANGE(IDC_EDIT_SCROLL_SPEED, OnEnChangeEditScroll)
	ON_EN_CHANGE(IDC_EDIT_SCROLL_DIST, OnEnChangeEditScroll)
	ON_CBN_SELCHANGE(IDC_COMBO_SCREEN, OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_SCR_FOR_INV, OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_SCR_WITHOUT_INV, OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_SAVE_PROMPT_SCR, OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_SAVE_NAME_SCR, OnCbnSelchangeCombo)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_TDB, OnBnClickedButtonSelectTdb)
	ON_EN_CHANGE(IDC_EDIT_SERIAL_KEY, OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_DB_FILE, OnEnChangeEdit)
	ON_BN_CLICKED(IDC_CHECK_NEED_SCENE_REDRAW, OnBnClickedCheckNeedSceneRedraw)
//	ON_CBN_SELCHANGE(IDC_COMBO_DEFAULT_FONT, OnCbnSelchangeComboDefaultFont)
	ON_BN_CLICKED(IDC_CHECK_NEED_SAVE_TIME_SHOW, OnBnClickedCheckNeedSaveTimeShow)
	ON_BN_CLICKED(IDC_CHECK_NEED_SAVE_NAME_SHOW, OnBnClickedCheckNeedSaveTitleShow)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HALL_OF_FAME_SIZE, OnDeltaposSpinHallOfFame)
END_MESSAGE_MAP()

BOOL GameSettingsPage::OnInitDialog()
{
	CBasePage::OnInitDialog();

	static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_COMPRESSION))->
		SetRange(0, 9);

//	LoadFonts();
	LoadResolutions();
	LoadCameraModes();
	LoadInterfaceScreens(m_wndScreens);

	CString str;
	str.LoadString(IDS_TEXT_ALIGN_LEFT);
	alignWnd_.AddString(str);
	topicAlignWnd_.AddString(str);

	str.LoadString(IDS_TEXT_ALIGN_CENTER);
	alignWnd_.AddString(str);
	topicAlignWnd_.AddString(str);

	str.LoadString(IDS_TEXT_ALIGN_RIGHT);
	alignWnd_.AddString(str);
	topicAlignWnd_.AddString(str);

	qdInterfaceDispatcher* const pdisp = qdInterfaceDispatcher::get_dispatcher();
	if (pdisp->has_main_menu())
		m_wndScreens.SelectString(-1, pdisp->main_menu_screen_name());
	else
		m_wndScreens.SetCurSel(0);

	needSceneRedraw_ = pdisp->need_scene_redraw();
	m_strSerialKey = m_ptrGameDisp->cd_key();

	compression_ = m_ptrGameDisp->resource_compression();
	hallOfFameSize_ = m_ptrGameDisp->hall_of_fame_size();

	LoadInterfaceScreens(m_wndScrn4Inv);
	if (pdisp->has_ingame_screen(true))
		m_wndScrn4Inv.SelectString(-1, pdisp->ingame_screen_name(true));
	else
		m_wndScrn4Inv.SetCurSel(0);
	LoadInterfaceScreens(m_wndScrnWithoutInv);
	if (pdisp->has_ingame_screen(false))
		m_wndScrnWithoutInv.SelectString(-1, pdisp->ingame_screen_name(false));
	else
		m_wndScrnWithoutInv.SetCurSel(0);

	LoadInterfaceScreens(m_wndSavePromptScrn);
	if(pdisp->has_save_prompt_screen())
		m_wndSavePromptScrn.SelectString(-1, pdisp->save_prompt_screen_name());
	else
		m_wndSavePromptScrn.SetCurSel(0);

	LoadInterfaceScreens(saveTitleScreen);
	if(pdisp->has_save_title_screen())
		saveTitleScreen.SelectString(-1, pdisp->save_title_screen_name());
	else
		saveTitleScreen.SetCurSel(0);

	m_boolNeedSaveScreenshot = pdisp->need_save_screenshot();
	toggleSaveTimeComponents(pdisp->need_show_save_time());	
	toggleSaveTitleComponents(pdisp->need_show_save_title());	

	m_strGameName = m_ptrGameDisp->game_title();
	if (LoadScenes())
	{
		int iCurSel = 0;
		if (m_ptrGameDisp->has_startup_scene())
		{
			iCurSel = m_wndScenes.FindString(-1, m_ptrGameDisp->startup_scene());
			if (iCurSel == CB_ERR)
				iCurSel = 0;
		}

		m_wndScenes.SetCurSel(iCurSel);
		AdjustListWidth(&m_wndScenes);
	}
	else
		m_wndScenes.EnableWindow(FALSE);

	AdjustListWidth(&m_combo);

	if (qdGameDispatcher* pdisp = qd_get_game_dispatcher())
		m_strDBFileName = pdisp->texts_database();

	LoadFonts();

	textOutputDefault2wnd();
	if (QD_FONT_TYPE_NONE == pdisp->save_font_type())
		m_wndSaveFont.SetCurSel(0);
	else
	{
		if (!m_ptrGameDisp)
			ASSERT(0);
		else
		{
			qdFontInfo const* fi = m_ptrGameDisp->get_font_info(pdisp->save_font_type());
			if (!fi||m_wndSaveFont.SelectString(-1, fi->name()) == CB_ERR)
				m_wndSaveFont.SetCurSel(0);
		}
	}
	saveFontColorWnd_.SetColor(pdisp->save_font_color());

//	SetDefaultFont();
	UpdateData(FALSE);

	return TRUE;
}

bool GameSettingsPage::LoadCameraModes()
{
	return true;
}

//! Загружает список доступных разрешений экрана
bool GameSettingsPage::LoadResolutions()
{
	CString strResolutions((LPTSTR)IDS_STR_RESOLUTIONS);

	m_combo.AddString("640x480");
	m_combo.AddString("800x600");
	m_combo.AddString("1024x768");
	int scrsize_x = qdGameConfig::get_config().screen_sx();
	if(scrsize_x == 640)
		m_combo.SetCurSel(0);
	else if (scrsize_x == 800) {
		m_combo.SetCurSel(1);
	}else if (scrsize_x == 1024) {
		m_combo.SetCurSel(2);
	}
	else
		m_combo.SetCurSel(0);
	return true;
}

BOOL GameSettingsPage::OnWazardFinish()
{
	return CBasePage::OnWizardFinish();
}

BOOL GameSettingsPage::OnApply()
{

	SetupStartupScene();
	SetupMainMenu();
	SetupIngameInvScr();
	SetupIngameScr();

	// Связанное с сэйвом
	SetupSavePromptScr();
	int font_type = QD_FONT_TYPE_NONE;
	int curSel = m_wndSaveFont.GetCurSel();
	if(curSel != CB_ERR && curSel != 0){
		qdFontInfo const* fi = 
			reinterpret_cast<qdFontInfo*>(m_wndSaveFont.GetItemDataPtr(curSel));
		font_type =  fi->type();
	}
	if (qdInterfaceDispatcher* ip = qdInterfaceDispatcher::get_dispatcher())
	{
		ip->toggle_save_screenshot(m_boolNeedSaveScreenshot);
		ip->toggle_show_save_time(m_wndNeedSaveTimeShow.GetCheck());
		ip->toggle_show_save_title(m_wndNeedSaveTitleShow.GetCheck());
		ip->set_save_font_type(font_type);
		ip->set_save_font_color(saveFontColorWnd_.GetColor());
	}
	else
		ASSERT(0);

	CWaitCursor wait;
	if (m_strDBFileName.IsEmpty())
		qd_get_game_dispatcher()->set_texts_database(_T(""));
	else if (qdTextDB::instance().load(m_strDBFileName))
	{
		if (qdGameDispatcher* pdisp = qd_get_game_dispatcher())
			pdisp->set_texts_database(m_strDBFileName);
	}

	qdInterfaceDispatcher::get_dispatcher()->set_scene_redraw(needSceneRedraw_);

	m_ptrGameDisp->set_game_title(m_strGameName);
	m_ptrGameDisp->set_cd_key(m_strSerialKey);

	m_ptrGameDisp->set_resource_compression(compression_);
	m_ptrGameDisp->set_hall_of_fame_size(hallOfFameSize_);

	wnd2textOutputDefault();

//	int font = defaultFont_.GetCurSel();
//	if (font)
//		font = reinterpret_cast<qdFontInfo*>(defaultFont_.GetItemDataPtr(font))->type();
//	else
//		font = -1;
//	m_ptrGameDisp->set_default_font(font);
	SetProjChange();
	return CBasePage::OnApply();
}

void GameSettingsPage::OnSelchangeResolution() 
{
	SetModified(TRUE);	

	CString str;
	m_combo.GetLBText(m_combo.GetCurSel(), str);
	if (strcmp("640x480", str) == 0) {
		qdGameConfig::get_config().set_screen_size(640, 480);
	}
	else if (strcmp("800x600", str) == 0) {
		qdGameConfig::get_config().set_screen_size(800, 600);
	}
	else if (strcmp("1024x768", str) == 0) {
		qdGameConfig::get_config().set_screen_size(1024, 768);
	}
}

//! Управляет состоянием (доступностны/недосутупны) кнопок "Назад" "Дальше"
bool GameSettingsPage::UpdateWizardBtnsState()
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	switch(m_iPagePlace) {
	case PAGE_PLACE_FIRST:
		psheet->SetWizardButtons( PSWIZB_NEXT);
		break;
	case PAGE_PLACE_MIDLE:
		psheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT);
		break;
	case PAGE_PLACE_LAST:
		psheet->SetWizardButtons(PSWIZB_BACK );
		break;
		//	default:
		//		ASSERT(("UpdateWizardBtnsState Defalut", 0));
	}
	
	return true;
}

BOOL GameSettingsPage::OnSetActive() 
{
	UpdateWizardBtnsState();
	return CBasePage::OnSetActive();
}


void GameSettingsPage::OnCbnSelchangeComboCameraMode()
{
	SetModified(TRUE);
}

//! Загружает список сцен
bool GameSettingsPage::LoadScenes()
{
	if (m_ptrGameDisp->scene_list().empty())
		return false;

	std::copy(m_ptrGameDisp->scene_list().begin(), m_ptrGameDisp->scene_list().end(),
		boost::make_function_output_iterator(insert_scenes(m_wndScenes)));

	return m_wndScenes.GetCount() != 0;
}
//! Указывает в диспетчере стартовую сцену
void GameSettingsPage::SetupStartupScene()
{
	if (m_wndScenes.GetCount() == 0)
		return;
	qdGameScene* pscene = 
		reinterpret_cast<qdGameScene*>(m_wndScenes.GetItemDataPtr(m_wndScenes.GetCurSel()));
	m_ptrGameDisp->set_startup_scene(pscene->name());
}

//! Указывает интерфейсный экран, на котором расположено основное меню
void GameSettingsPage::SetupMainMenu()
{
	if (m_wndScreens.GetCount() == 0)
		return;

	int const cur_sel = m_wndScreens.GetCurSel();
	qdInterfaceDispatcher* const pdisp = qdInterfaceDispatcher::get_dispatcher();
	if (cur_sel == CB_ERR||!cur_sel)
		pdisp->set_main_menu_screen(NULL);
	else
	{
		CString str;
		m_wndScreens.GetLBText(cur_sel, str);
		pdisp->set_main_menu_screen(str);
	}
}

//! Назначает внутриигровой интерфейсный экран с инвентори
void GameSettingsPage::SetupIngameInvScr()//! с инвентори
{
	if (m_wndScrn4Inv.GetCount() == 0)
		return;

	int const cur_sel = m_wndScrn4Inv.GetCurSel();
	qdInterfaceDispatcher* const pdisp = qdInterfaceDispatcher::get_dispatcher();
	if (cur_sel == CB_ERR||!cur_sel)
		pdisp->set_ingame_screen(NULL, true);
	else
	{
		CString str;
		m_wndScrn4Inv.GetLBText(cur_sel, str);
		pdisp->set_ingame_screen(str, true);
	}
}

//! Назначает внутриигровой интерфейсный экран без инвентори
void GameSettingsPage::SetupIngameScr()//! без инвентори
{
	if (m_wndScrnWithoutInv.GetCount() == 0)
		return;

	int const cur_sel = m_wndScrnWithoutInv.GetCurSel();
	qdInterfaceDispatcher* const pdisp = qdInterfaceDispatcher::get_dispatcher();
	if (cur_sel == CB_ERR||!cur_sel)
		pdisp->set_ingame_screen(NULL, false);
	else
	{
		CString str;
		m_wndScrnWithoutInv.GetLBText(cur_sel, str);
		pdisp->set_ingame_screen(str, false);
	}
}

//! Назначает экран, вызываемый при запросе на перезапись сохраненной игры
void GameSettingsPage::SetupSavePromptScr()
{
	if(m_wndSavePromptScrn.GetCount()){
		int cur_sel = m_wndSavePromptScrn.GetCurSel();
		qdInterfaceDispatcher* const pdisp = qdInterfaceDispatcher::get_dispatcher();
		if(cur_sel == CB_ERR||!cur_sel){
			pdisp->set_save_prompt_screen_name("");
		}
		else {
			CString str;
			m_wndSavePromptScrn.GetLBText(cur_sel, str);
			pdisp->set_save_prompt_screen_name(str);
		}
	}

	if(saveTitleScreen.GetCount()){
		int cur_sel = saveTitleScreen.GetCurSel();
		qdInterfaceDispatcher* const pdisp = qdInterfaceDispatcher::get_dispatcher();
		if(cur_sel == CB_ERR || !cur_sel){
			pdisp->set_save_title_screen_name("");
		}
		else {
			CString str;
			saveTitleScreen.GetLBText(cur_sel, str);
			pdisp->set_save_title_screen_name(str);
		}
	}
}

void GameSettingsPage::OnCbnSelchangeComboScenes()
{
	SetModified();
}

void GameSettingsPage::OnEnChangeEditScroll()
{
	SetModified();
}

//! Загружает список доступных интерфейсных экранов
bool GameSettingsPage::LoadInterfaceScreens(CCJFlatComboBox& box)
{
	qdInterfaceDispatcher* const pdisp = qdInterfaceDispatcher::get_dispatcher();
	const qdInterfaceDispatcher::screen_list_t::const_iterator itr = 
		pdisp->screen_list().begin(),  end = pdisp->screen_list().end();
	typedef boost::function_output_iterator<iscreens2combo> out_itr_t;

	box.InsertString(0, CString((LPTSTR)IDS_NOTHING));
	std::copy(itr, end, out_itr_t(iscreens2combo(box)));

	return true;
}

void GameSettingsPage::OnCbnSelchangeCombo()
{
	SetModified();
}

BOOL GameSettingsPage::PreTranslateMessage(MSG* pmsg)
{
	if (pmsg->message == WM_KEYDOWN && pmsg->wParam == VK_RETURN)
	{
		if (pmsg->hwnd == ::GetDlgItem(m_hWnd, IDC_EDIT_GAME_NAME))
		{
			UpdateData();
			SetModified();
			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pmsg);
}

void GameSettingsPage::OnBnClickedButtonSelectTdb()
{
	CString strFilter;
	strFilter.LoadString(IDS_TEXTDB_FILTER);
	CString strExt;
	strExt.LoadString(IDS_TEXTDB_FILE_EXT);
	CString strCaption;
	strCaption.LoadString(IDS_OSD_CAP_SELECT_TEXTDB);
	m_strDBFileName = 
		disk_op::Get1DiskFileName(GetTextDBCurDirSafer, strFilter, strExt, strCaption);
	SetModified();
	UpdateData(FALSE);
}

void GameSettingsPage::OnEnChangeEdit()
{
	SetModified();
}

void GameSettingsPage::OnBnClickedCheckNeedSceneRedraw()
{
	SetModified();
}

void GameSettingsPage::OnDeltaposSpinHallOfFame(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	hallOfFameSize_ -= pNMUpDown->iDelta;

	if(hallOfFameSize_ < 0)
		hallOfFameSize_ = 0;

	UpdateData(FALSE);
	SetModified();
	*pResult = 0;
}

void GameSettingsPage::InitFontBox(CCJFlatComboBox& box)
{
	box.Clear();
	if (!m_ptrGameDisp)
	{
		ASSERT(0);
		return;
	}
	boost::rtl::crange<qdFontInfoList const> r(m_ptrGameDisp->fonts_list());
	for(; r; ++r)
	{
		int item = box.AddString((*r)->name());
		if (item != CB_ERR)
			box.SetItemDataPtr(item, *r);
	}
	CString str;
	str.LoadString(IDS_DEFAULT_FONT);
	box.InsertString(0, str);
}

void GameSettingsPage::LoadFonts()
{
	if (!m_ptrGameDisp)
	{
		ASSERT(0);
		return;
	}
	InitFontBox(fontsWnd_);
	InitFontBox(topicFontsWnd_);
	InitFontBox(m_wndSaveFont);
}

void GameSettingsPage::textOutputDefault2wnd()
{
	qdScreenTextFormat format = qdScreenTextFormat::global_text_format();
	textHorizontal_ = static_cast<int>(format.arrangement());

	textColorWnd_.SetColor(static_cast<COLORREF>(format.color()));
	hiliteColorWnd_.SetColor(static_cast<COLORREF>(format.hover_color()));
	if (format.font_type() == QD_FONT_TYPE_NONE)
		fontsWnd_.SetCurSel(0);
	else
	{
		if (!m_ptrGameDisp)
		{
			ASSERT(0);
			return;
		}
		qdFontInfo const* fi = m_ptrGameDisp->get_font_info(format.font_type());
		if (!fi||fontsWnd_.SelectString(-1, fi->name()) == CB_ERR)
			fontsWnd_.SetCurSel(0);
	}
	alignWnd_.SetCurSel(format.alignment());

	format = qdScreenTextFormat::global_topic_format();
	topicTextHorizontal_ = static_cast<int>(format.arrangement());

	topicTextColorWnd_.SetColor(static_cast<COLORREF>(format.color()));
	topicHiliteColorWnd_.SetColor(static_cast<COLORREF>(format.hover_color()));
	if (format.font_type() == QD_FONT_TYPE_NONE)
		topicFontsWnd_.SetCurSel(0);
	else
	{
		if (!m_ptrGameDisp)
		{
			ASSERT(0);
			return;
		}
		qdFontInfo const* fi = m_ptrGameDisp->get_font_info(format.font_type());
		if (!fi||topicFontsWnd_.SelectString(-1, fi->name()) == CB_ERR)
			topicFontsWnd_.SetCurSel(0);
	}
	topicAlignWnd_.SetCurSel(format.alignment());
}

void GameSettingsPage::wnd2textOutputDefault()
{
	qdScreenTextFormat format;
	format.set_arrangement(
		static_cast<qdScreenTextFormat::arrangement_t>(textHorizontal_));
	format.set_color(textColorWnd_.GetColor());
	format.set_hover_color(hiliteColorWnd_.GetColor());

	int font_type;
	int curSel = fontsWnd_.GetCurSel();
	if (curSel == CB_ERR||curSel == 0)
		font_type =  QD_FONT_TYPE_NONE;
	else
	{
		qdFontInfo const* fi = 
			reinterpret_cast<qdFontInfo*>(fontsWnd_.GetItemDataPtr(curSel));
		font_type =  fi->type();
	}
	format.set_font_type(font_type);	
	format.set_alignment(qdScreenTextFormat::alignment_t(alignWnd_.GetCurSel()));

	qdScreenTextFormat::set_global_text_format(format);

	format.set_arrangement(
		static_cast<qdScreenTextFormat::arrangement_t>(topicTextHorizontal_));
	format.set_color(topicTextColorWnd_.GetColor());
	format.set_hover_color(topicHiliteColorWnd_.GetColor());

	curSel = topicFontsWnd_.GetCurSel();
	if (curSel == CB_ERR||curSel == 0)
		font_type =  QD_FONT_TYPE_NONE;
	else
	{
		qdFontInfo const* fi = 
			reinterpret_cast<qdFontInfo*>(topicFontsWnd_.GetItemDataPtr(curSel));
		font_type =  fi->type();
	}
	format.set_font_type(font_type);	
	format.set_alignment(qdScreenTextFormat::alignment_t(topicAlignWnd_.GetCurSel()));
	qdScreenTextFormat::set_global_topic_format(format);
}

//! Управляет галочкой, разрешающей вывод времени сэйва, и связанными с ней компонентами
void GameSettingsPage::toggleSaveTimeComponents(bool activate)
{
	if (activate)
	{
		m_wndNeedSaveTimeShow.SetCheck(BST_CHECKED);
		m_wndSaveFont.EnableWindow(true);
		saveFontColorWnd_.EnableWindow(true);
	}
	else
	{
		m_wndNeedSaveTimeShow.SetCheck(BST_UNCHECKED);
		if(!m_wndNeedSaveTitleShow.GetCheck() & BST_CHECKED){
			m_wndSaveFont.EnableWindow(false);
			saveFontColorWnd_.EnableWindow(false);
		}
	}
}

void GameSettingsPage::toggleSaveTitleComponents(bool activate)
{
	if (activate)
	{
		m_wndNeedSaveTitleShow.SetCheck(BST_CHECKED);
		m_wndSaveFont.EnableWindow(true);
		saveFontColorWnd_.EnableWindow(true);
	}
	else
	{
		m_wndNeedSaveTitleShow.SetCheck(BST_UNCHECKED);
		if(!m_wndNeedSaveTimeShow.GetCheck() & BST_CHECKED){
			m_wndSaveFont.EnableWindow(false);
			saveFontColorWnd_.EnableWindow(false);
		}
	}
}

void GameSettingsPage::OnBnClickedCheckNeedSaveTimeShow()
{
	toggleSaveTimeComponents(m_wndNeedSaveTimeShow.GetCheck());
}

void GameSettingsPage::OnBnClickedCheckNeedSaveTitleShow()
{
	toggleSaveTitleComponents(m_wndNeedSaveTitleShow.GetCheck());
}

/*
void GameSettingsPage::LoadFonts()
{
	boost::rtl::crange<qdFontInfoList const> r(m_ptrGameDisp->fonts_list());
	for(; r; ++r)
	{
		int const item = defaultFont_.AddString((*r)->name());
		if (item != CB_ERR)
			defaultFont_.SetItemDataPtr(item, *r);
	}

	CString str;
	str.LoadString(IDS_NOTHING);
	defaultFont_.InsertString(0, str);
}

void GameSettingsPage::OnCbnSelchangeComboDefaultFont()
{
	SetModified();
}

void GameSettingsPage::SetDefaultFont()
{
	for (int i = 1; i < defaultFont_.GetCount(); ++i)
	{
		qdFontInfo* fi = reinterpret_cast<qdFontInfo*>(defaultFont_.GetItemDataPtr(i));
		if (fi->type() == m_ptrGameDisp->default_font())
		{
			defaultFont_.SetCurSel(i);
			return;
		}
	}
	defaultFont_.SetCurSel(0);
}
*/
