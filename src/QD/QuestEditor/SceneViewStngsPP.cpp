// SceneViewStngsPP.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "SceneViewStngsPP.h"
#include ".\sceneviewstngspp.h"


// SceneViewStngsPP dialog

//IMPLEMENT_DYNAMIC(SceneViewStngsPP, AppSettingsBasePage)
SceneViewStngsPP::SceneViewStngsPP()
	: AppSettingsBasePage(SceneViewStngsPP::IDD)
	, m_iTopBottom(0)
	, m_iLeftRight(0)
{
}

SceneViewStngsPP::~SceneViewStngsPP()
{
}

void SceneViewStngsPP::DoDataExchange(CDataExchange* pDX)
{
	AppSettingsBasePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_X, m_iTopBottom);
	DDX_Text(pDX, IDC_EDIT_Y, m_iLeftRight);
}


BEGIN_MESSAGE_MAP(SceneViewStngsPP, AppSettingsBasePage)
	ON_EN_CHANGE(IDC_EDIT_Y, OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_X, OnEnChangeEdit)
END_MESSAGE_MAP()


// SceneViewStngsPP message handlers

BOOL SceneViewStngsPP::OnInitDialog()
{
	AppSettingsBasePage::OnInitDialog();

	m_iTopBottom = GetMyApp()->GetIntSetting(SETTING_NAME(SV_TOP_BOTTOM_OFFSET));
	m_iLeftRight = GetMyApp()->GetIntSetting(SETTING_NAME(SV_LEFT_RIGHT_OFFSET));
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL SceneViewStngsPP::OnApply()
{
	if (!UpdateData())
		return FALSE;

	GetMyApp()->WriteIntSetting(SETTING_NAME(SV_TOP_BOTTOM_OFFSET), m_iTopBottom);
	GetMyApp()->WriteIntSetting(SETTING_NAME(SV_LEFT_RIGHT_OFFSET), m_iLeftRight);
	
	return AppSettingsBasePage::OnApply();
}

void SceneViewStngsPP::OnEnChangeEdit()
{
	setModified(true);
}

