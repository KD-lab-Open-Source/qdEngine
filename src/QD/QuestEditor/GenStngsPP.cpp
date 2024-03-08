// GenStngsPP.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "GenStngsPP.h"

#include "QuestEditor.h"

#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GenSettingsPP property page

GenSettingsPP::GenSettingsPP() : AppSettingsBasePage(GenSettingsPP::IDD)
{
	//{{AFX_DATA_INIT(GenSettingsPP)
	m_bAutoSave = FALSE;
	m_iASPeriod = 0;
	m_bCollapseScenes = FALSE;
	//}}AFX_DATA_INIT
}

GenSettingsPP::~GenSettingsPP()
{
}

void GenSettingsPP::DoDataExchange(CDataExchange* pDX)
{
	AppSettingsBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GenSettingsPP)
	DDX_Check(pDX, IDC_CHECK_USE_AUTOSAVE, m_bAutoSave);
	DDX_Text(pDX, IDC_EDIT_AS_PERIOD, m_iASPeriod);
	DDX_Check(pDX, IDC_CHECK_COLLAPSE_SCENES, m_bCollapseScenes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GenSettingsPP, AppSettingsBasePage)
	//{{AFX_MSG_MAP(GenSettingsPP)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_COLLAPSE_SCENES, OnBnClickedCheckCollapseScenes)
	ON_BN_CLICKED(IDC_CHECK_USE_AUTOSAVE, OnBnClickedCheckUseAutosave)
	ON_EN_CHANGE(IDC_EDIT_AS_PERIOD, OnEnChangeEditAsPeriod)
END_MESSAGE_MAP()



BOOL GenSettingsPP::OnApply() 
{
	if (!UpdateData())
		return FALSE;

	GetMyApp()->WriteIntSetting(SETTING_NAME(AUTOSAVE), m_bAutoSave);
	GetMyApp()->
		WriteIntSetting(SETTING_NAME(AUTOSAVE_PERIOD), m_iASPeriod*60000);
	GetMyApp()->
		WriteIntSetting(SETTING_NAME(PT_COLLAPSE_SCENES), m_bCollapseScenes);
	
	return AppSettingsBasePage::OnApply();
}

BOOL GenSettingsPP::OnInitDialog() 
{
	AppSettingsBasePage::OnInitDialog();
	
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_AUTOSAVE))->
													SetRange(1, 100);

	m_bAutoSave = GetMyApp()->GetIntSetting(SETTING_NAME(AUTOSAVE));
	m_iASPeriod = 
				GetMyApp()->GetIntSetting(SETTING_NAME(AUTOSAVE_PERIOD))/60000;
	m_bCollapseScenes = 
		GetMyApp()->GetIntSetting(SETTING_NAME(PT_COLLAPSE_SCENES));

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void GenSettingsPP::OnBnClickedCheckCollapseScenes()
{
	setModified(true);
}

void GenSettingsPP::OnBnClickedCheckUseAutosave()
{
	setModified(true);
}

void GenSettingsPP::OnEnChangeEditAsPeriod()
{
	setModified(true);
}
