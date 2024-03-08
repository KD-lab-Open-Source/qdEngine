// MWGlobalSettingPage.cpp: implementation of the MWGlobalSettingPage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "questeditor.h"
#include "MWGlobalSettingPage.h"

#include "qd_setup.h"
#include "qd_interface_screen.h"
#include "qd_interface_dispatcher.h"
#include <boost/function_output_iterator.hpp>

/// ‘унктор, добавл€ющий экран в комбобокс
class iscreens2combo{
	CCJFlatComboBox& box_;
public:
	iscreens2combo(CCJFlatComboBox& box):box_(box){}
	void operator()(qdInterfaceScreen* pscr) const{
		int const i = box_.AddString(pscr->name());
		if (i != CB_ERR)
			box_.SetItemDataPtr(i, pscr);
	}
};

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(MWGlobalSettingPage, WizardBasePage)


/////////////////////////////////////////////////////////////////////////////
// MWGlobalSettingPage property page

MWGlobalSettingPage::MWGlobalSettingPage() : WizardBasePage(MWGlobalSettingPage::IDD)
, m_iScrollSpeed(1)
, m_iScroolDistance(1)
{
	//{{AFX_DATA_INIT(MWGlobalSettingPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_ptrGameDisp = NULL;
}

bool MWGlobalSettingPage::set_dispatcher(qdGameDispatcher* ptrDisp)
{
	m_ptrGameDisp = ptrDisp;
	return true;
}

MWGlobalSettingPage::~MWGlobalSettingPage()
{
}

void MWGlobalSettingPage::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MWGlobalSettingPage)
	DDX_Control(pDX, IDC_RESOLUTION, m_combo);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_SCROLL_SPEED, m_iScrollSpeed);
	DDX_Text(pDX, IDC_EDIT_SCROLL_DIST, m_iScroolDistance);
}

BEGIN_MESSAGE_MAP(MWGlobalSettingPage, WizardBasePage)
	//{{AFX_MSG_MAP(MWGlobalSettingPage)
	ON_CBN_SELCHANGE(IDC_RESOLUTION, OnSelchangeResolution)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL MWGlobalSettingPage::OnInitDialog()
{
	WizardBasePage::OnInitDialog();
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SPEED))->SetRange(1, 10000);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_DIST))->SetRange(1, 10000);

	LoadResolutions();
	LoadCameraModes();
	return TRUE;
}

bool MWGlobalSettingPage::LoadResolutions()
{
	CString strResolutions((LPTSTR)IDS_STR_RESOLUTIONS);

	m_combo.AddString(_T("640x480"));
	m_combo.AddString(_T("800x600"));
	m_combo.AddString(_T("1024x768"));
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

BOOL MWGlobalSettingPage::OnWazardFinish()
{

	return WizardBasePage::OnWizardFinish();
}

BOOL MWGlobalSettingPage::OnApply()
{
	return WizardBasePage::OnApply();
}

/**
	прописывает выбранное разрешение в конфиг игры
 */
void MWGlobalSettingPage::OnSelchangeResolution() 
{
	SetModified(TRUE);	

	CString str;
	m_combo.GetLBText(m_combo.GetCurSel(), str);
	if (_tcscmp(_T("640x480"), str) == 0) {
		qdGameConfig::get_config().set_screen_size(640, 480);
	}
	else if (_tcscmp(_T("800x600"), str) == 0) {
		qdGameConfig::get_config().set_screen_size(800, 600);
	}
	else if (_tcscmp(_T("1024x768"), str) == 0) {
		qdGameConfig::get_config().set_screen_size(1024, 768);
	}
}

bool MWGlobalSettingPage::UpdateWizardBtnsState()
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_FINISH);
	return true;
}

BOOL MWGlobalSettingPage::OnSetActive() 
{
	UpdateWizardBtnsState();
	return WizardBasePage::OnSetActive();
}

bool MWGlobalSettingPage::LoadCameraModes()
{
	return true;
}

void MWGlobalSettingPage::OnCbnSelchangeComboCameraMode()
{
}

bool MWGlobalSettingPage::LoadInterfaceScreens(CCJFlatComboBox& box)
{
	qdInterfaceDispatcher* const pdisp = qdInterfaceDispatcher::get_dispatcher();
	const qdInterfaceDispatcher::screen_list_t::const_iterator itr = 
		pdisp->screen_list().begin(),  end = pdisp->screen_list().end();
	typedef boost::function_output_iterator<iscreens2combo> out_itr_t;

	box.InsertString(0, CString((LPTSTR)IDS_NOTHING));
	std::copy(itr, end, out_itr_t(iscreens2combo(box)));
	return true;
}
void MWGlobalSettingPage::OnCbnSelchangeComboScreen()
{
//	int const cur_sel = m_wndScreens.GetCurSel();
//	qdInterfaceDispatcher* const pdisp = qdInterfaceDispatcher::get_dispatcher();
//	if (cur_sel == CB_ERR||!cur_sel)
//		pdisp->set_main_menu_screen(NULL);
//	else
//	{
//		CString str;
//		m_wndScreens.GetLBText(cur_sel, str);
//		pdisp->set_main_menu_screen(str);
//	}
}
