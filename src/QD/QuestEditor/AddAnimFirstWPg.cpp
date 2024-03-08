// AddAnimFirstWPg.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "AddAnimFirstWPg.h"
#include "AnimAddMgr.h"
#include "qd_game_dispatcher.h"
#include "curdirsafer.h"
#include "SelectDirs.h"
#include "disk_op.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//forward declaration
bool GetAnimationFileName(HWND hWnd, bool bNewFile, CStringVect_t* ptrFiles);

/////////////////////////////////////////////////////////////////////////////
// AddAnimFirstWPg property page

IMPLEMENT_DYNCREATE(AddAnimFirstWPg, CBasePage)

AddAnimFirstWPg::AddAnimFirstWPg() : CBasePage(AddAnimFirstWPg::IDD)
{
	//{{AFX_DATA_INIT(AddAnimFirstWPg)
	m_bCrtObj = FALSE;
	m_iAction = -1;
	//}}AFX_DATA_INIT
}

AddAnimFirstWPg::~AddAnimFirstWPg()
{
}

bool AddAnimFirstWPg::set_data(qdGameDispatcher* ptrGameDisp, qdGameDispatcherBase* ptrDispBase)
{
	m_ptrGameDisp = ptrGameDisp;
	m_ptrDispBase = ptrDispBase;
	return true;
}

void AddAnimFirstWPg::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddAnimFirstWPg)
	DDX_Check(pDX, IDC_CHECK_CRT_OBJ, m_bCrtObj);
	DDX_Radio(pDX, IDC_RADIO_CONVERT_FOR_SCENE, m_iAction);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddAnimFirstWPg, CBasePage)
//{{AFX_MSG_MAP(AddAnimFirstWPg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_RADIO_EXIST, OnRadioExist)
	ON_BN_CLICKED(IDC_RADIO_CONV_DISP, OnRadioConvDisp)
	ON_BN_CLICKED(IDC_RADIO_CONVERT_FOR_SCENE, OnRadioConvertForScene)
	ON_BN_CLICKED(IDC_BTN_RUN, OnBtnRun)
	ON_BN_CLICKED(IDC_RADIO_CONV_DISP2, OnRadioConvDisp)
	ON_BN_CLICKED(IDC_RADIO_CONV_DISP3, OnRadioConvDisp)
	//}}AFX_MSG_MAP
//ON_MESSAGE(PSM_QUERYSIBLINGS, OnQuerySiblings)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AddAnimFirstWPg message handlers

LRESULT AddAnimFirstWPg::OnWizardBack() 
{
	return CPropertyPage::OnWizardBack();
}

LRESULT AddAnimFirstWPg::OnWizardNext() 
{
	UpdateData();
	return CPropertyPage::OnWizardNext();
}

BOOL AddAnimFirstWPg::OnWizardFinish() 
{
	CWaitCursor __wait__;
	AnimAddMgr aam;
	aam.DO(m_ptrGameDisp, m_ptrDispBase, m_vecStrs, m_iAction, m_bCrtObj);
	return CBasePage::OnWizardFinish();
}

BOOL AddAnimFirstWPg::OnSetActive() 
{
	UpdateWizardBtnsState();
	UpdateRunBtn();
	return CPropertyPage::OnSetActive();
}

void AddAnimFirstWPg::OnButton1() 
{
	SelectDiskObj();
}

void AddAnimFirstWPg::OnRadioExist() 
{
	int iOld = m_iAction;
	UpdateData();
	if(m_iAction != iOld)
		m_vecStrs.clear();

	SelectBrowseBtnCap();
	UpdatePlaceState();
	UpdateWizardBtnsState();
	UpdateRunBtn();
}

void AddAnimFirstWPg::OnRadioConvDisp() 
{
	int iOld = m_iAction;
	UpdateData();
	if(m_iAction != iOld)
		if((iOld != AAA_CONV_FOR_SCENES)
		&&(iOld != AAA_CONV_FOR_SCENES_AND_CRT_USE_STRICT_FLDR_STRUCT))
		m_vecStrs.clear();

	SelectBrowseBtnCap();
	UpdatePlaceState();
	UpdateWizardBtnsState();
	UpdateRunBtn();
}

void AddAnimFirstWPg::OnRadioConvertForScene() 
{
	int iOld = m_iAction;
	UpdateData();
	if(m_iAction != iOld)
		m_vecStrs.clear();

	SelectBrowseBtnCap();
	UpdatePlaceState();
	UpdateWizardBtnsState();
	UpdateRunBtn();
}

void AddAnimFirstWPg::SelectBrowseBtnCap()
{
	CString strText;
	switch(m_iAction) {
	case AAA_EXISTING:
		strText.LoadString(IDS_CAP_BROWSE_BUTTON1);
		break;
	case AAA_CONV_CATALOG:
		strText.LoadString(IDS_CAP_BROWSE_BUTTON2);
		break;
	case AAA_CONV_FOR_SCENES:
	case AAA_CONV_FOR_SCENES_AND_CRT_USE_STRICT_FLDR_STRUCT:
		strText.LoadString(IDS_CAP_BROWSE_BUTTON3);
		break;
	}
	GetDlgItem(IDC_BUTTON1)->SetWindowText(strText);
}

void AddAnimFirstWPg::UpdatePlaceState()
{
	BOOL bEnable = FALSE;
	switch(m_iAction)
	{
	case AAA_EXISTING:
		bEnable = TRUE;
		break;
	case AAA_CONV_CATALOG:
		bEnable = TRUE;
		break;
	case AAA_CONV_FOR_SCENES:
		bEnable = FALSE;
		break;
	}
}


bool AddAnimFirstWPg::SelectDiskObj()
{
	bool res = false;
	switch(m_iAction) {
	case AAA_EXISTING:
		res = SelectFiles();
		break;
	case AAA_CONV_CATALOG:
		res = SelectCatalogs();
		break;
	case AAA_CONV_FOR_SCENES:
	case AAA_CONV_FOR_SCENES_AND_CRT_USE_STRICT_FLDR_STRUCT:
		res = SelectCatalog();
		break;
	}
	if(res)
	{
		UpdateWizardBtnsState();
		UpdateRunBtn();
	}
	return false;
}

bool AddAnimFirstWPg::SelectFiles()
{
	CStringVect_t vecStrs;
	if(GetAnimationFileName(*this, false, &vecStrs))
	{
		m_vecStrs = vecStrs;
		return true;
	}
	return false;
}

bool AddAnimFirstWPg::SelectCatalog()
{
	CString title;
	title.LoadString(IDS_TITLE_FOR_SEL_FOLDER_DLG);
	CString res = disk_op::GetDiskFolder(*this, title, false, NULL);
	
	if(res.IsEmpty())
		return false;
	
	m_vecStrs.clear();
	m_vecStrs.push_back(res);
	return true;
}

bool  AddAnimFirstWPg::SelectCatalogs()
{
	CurDirSafer __safe;
	CurDirGuard __cdg(&__safe);

	CStringVect_t vecStrs;
	SelectDirs sd(&vecStrs);
	if(sd.DoModal()==IDOK)
	{
		m_vecStrs.swap(vecStrs);
		return true;
	}
	return false;
}

bool AddAnimFirstWPg::UpdateWizardBtnsState()
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	bool bEnable = (m_vecStrs.size() != 0);
	
	switch(m_iPagePlace) {
	case PAGE_PLACE_FIRST:
		if(bEnable)
			psheet->SetWizardButtons( PSWIZB_NEXT);
		else
			psheet->SetWizardButtons( PSWIZB_NEXT);
		break;
	case PAGE_PLACE_MIDLE:
		if(bEnable)
			psheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT);
		else
			psheet->SetWizardButtons( PSWIZB_BACK);
		break;
	case PAGE_PLACE_LAST:
		if(bEnable)
			psheet->SetWizardButtons(PSWIZB_BACK );
		else
			psheet->SetWizardButtons(PSWIZB_BACK);
		break;
	}
	
	return true;
}

void AddAnimFirstWPg::OnBtnRun() 
{
	UpdateData();
	CWaitCursor __wait__;
	AnimAddMgr aam;
	if(aam.DO(m_ptrGameDisp, m_ptrDispBase, m_vecStrs, m_iAction, m_bCrtObj))
		SetModified();
}

bool AddAnimFirstWPg::UpdateRunBtn()
{
	GetDlgItem(IDC_BTN_RUN)->EnableWindow(!m_vecStrs.empty());
	return true;
}

BOOL AddAnimFirstWPg::OnInitDialog()
{
	BOOL bRes = CPropertyPage::OnInitDialog();

	if(m_ptrGameDisp != m_ptrDispBase)
	{
		GetDlgItem(IDC_RADIO_CONV_DISP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_CONV_DISP2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_CONV_DISP3)->ShowWindow(SW_HIDE);
	}
	else
		GetDlgItem(IDC_CHECK_CRT_OBJ)->ShowWindow(SW_HIDE);
	return bRes;
}

