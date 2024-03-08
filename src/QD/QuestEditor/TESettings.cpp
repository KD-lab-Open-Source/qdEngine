// TESettings.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "TESettings.h"
#include ".\tesettings.h"
#include "CondListEditor.h"
#include "Condition.h"
#include "disk_op.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TESettings dialog
//IMPLEMENT_DYNCREATE(TESettings, AppSettingsBasePage)


TESettings::TESettings(CWnd* pParent /*=NULL*/)
	: AppSettingsBasePage(TESettings::IDD)
	, m_bShowMenuOnLinkCreate(false)
{
	//{{AFX_DATA_ILUHA_INIT(TESettings)
	m_iX = 0;
	m_iY = 0;
	m_iBindToNetNode = FALSE;
	//}}AFX_DATA_INIT
}


void TESettings::DoDataExchange(CDataExchange* pDX)
{
	AppSettingsBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TESettings)
	DDX_Text(pDX, IDC_EDIT_X, m_iX);
	DDX_Text(pDX, IDC_EDIT_Y, m_iY);
	DDX_Check(pDX, IDC_CHECK_BIND_TO_NET_NODE, m_iBindToNetNode);
	DDX_Check(pDX, IDC_CHECK_SHOW_MENU_ON_LINK_CREATE, m_bShowMenuOnLinkCreate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TESettings, AppSettingsBasePage)
	//{{AFX_MSG_MAP(TESettings)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_COND_LIST_EDITOR, OnBnClickedButtonCondListEditor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TESettings message handlers

BOOL TESettings::OnInitDialog() 
{
	AppSettingsBasePage::OnInitDialog();
	
	m_iX = GetMyApp()->GetIntSetting(SETTING_NAME(TE_NET_CELL_SX));
	m_iY = GetMyApp()->GetIntSetting(SETTING_NAME(TE_NET_CELL_SY));
	m_iBindToNetNode = GetMyApp()->
						GetIntSetting(SETTING_NAME(TE_NET_BIND_TO_NODE));
	m_bShowMenuOnLinkCreate = GetMyApp()->
						GetIntSetting(SETTING_NAME(TE_SHOW_MENU_ON_LINK_CREATE));
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL TESettings::OnApply()
{
	if (!UpdateData())
		return FALSE;

	GetMyApp()->WriteIntSetting(SETTING_NAME(TE_NET_CELL_SX), m_iX);
	GetMyApp()->WriteIntSetting(SETTING_NAME(TE_NET_CELL_SY), m_iY);
	GetMyApp()->
		WriteIntSetting(SETTING_NAME(TE_NET_BIND_TO_NODE), m_iBindToNetNode);
	GetMyApp()->
		WriteIntSetting(SETTING_NAME(TE_SHOW_MENU_ON_LINK_CREATE), 
						m_bShowMenuOnLinkCreate);
	return AppSettingsBasePage::OnApply();
}
void TESettings::OnBnClickedButtonCondListEditor()
{
	CCondListEditor list_editor;
	if (IDOK == list_editor.DoModal())
	{
		CString permut_file;
		permut_file.LoadString(IDS_CONDITION_LIST_SETUP_FILE);
		permut_file = disk_op::GetPathFromFullFileName(disk_op::GetModulePath()) + permut_file;
		g_permutation.save(permut_file);
	}
}
