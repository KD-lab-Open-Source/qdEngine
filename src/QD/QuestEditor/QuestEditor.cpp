// QuestEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FileExtAssociator.h"
#include "QuestEditor.h"

#include "reg_setting.h"

#include "MainFrm.h"

#include "XUtil.h"
#include "gr_dispatcher.h"

#include "qd_setup.h"
#include "curdirsafer.h"
#include "Shlwapi.h"

#include "SettingDefs.h"
#include "winver.h"
#include "disk_op.h"
#include <memory>

#include "Main/Utils/DefaultResourceCreator.h"
#include "DirMakeWizardShower.h"
#include "Main/IIdleProcessor.h"
#ifdef USE_NEW_PROJ

#include "Main/QEDocTemplate.h"
#include "Main/MainFrame.h"
#include "Main/Project.h"
#include "Main/StructureView.h"
#include ".\questeditor.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "condition.h"

/*!
	Это workaround для того, чтобы SEH пойманный в catch(...) отправлялся дальше-
	куда следует
*/
//extern "C" void straight_to_debugger(unsigned int, EXCEPTION_POINTERS*){
//	throw;
//}
//extern "C" void (*old_translator)(unsigned, EXCEPTION_POINTERS*)
//	= _set_se_translator(straight_to_debugger);


#define ADD_INT_SETTING(stg_name, strSection)					\
	add_int_setting(SETTING_NAME(stg_name), strSection,			\
	REG_VAL_NAME(stg_name), DEFAULT_VALUE(stg_name))

#define ADD_STR_SETTING(stg_name, strSection)					\
	add_string_setting(SETTING_NAME(stg_name), strSection,			\
	REG_VAL_NAME(stg_name), DEFAULT_VALUE(stg_name))

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DWORD dwHandle = 0;
	CString str = disk_op::GetModulePath();
	DWORD dwSize = GetFileVersionInfoSize(str.GetBuffer(0), &dwHandle);
	str.ReleaseBuffer();
	
	void* buff = malloc(dwSize);
	GetFileVersionInfo(str.GetBuffer(0), 0, dwSize, buff);
	str.ReleaseBuffer();

	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;
	
	// Read the list of languages and code pages.
	
	UINT cbTranslate = 0;
	VerQueryValue(buff, 
		TEXT("\\VarFileInfo\\Translation"),
		(LPVOID*)&lpTranslate,
		&cbTranslate);
	
	// Read the file description for each language and code page.
	
	void *data = NULL;
	UINT len = 0;
	for( int i=0; i < (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
	{
		CString SubBlock;
		SubBlock.Format(TEXT("\\StringFileInfo\\%04x%04x\\ProductVersion"),
            lpTranslate[i].wLanguage,
            lpTranslate[i].wCodePage);
		
		// Retrieve file description for language and code page "i". 
		
		VerQueryValue(buff, 
			SubBlock.GetBuffer(0), 
			&data, 
			&len); 
		break;
	}	
	if (len){
		CString strText;
		GetDlgItem(IDC_STATIC_VER)->GetWindowText(strText);
		strText += (TCHAR*)data;
		GetDlgItem(IDC_STATIC_VER)->SetWindowText(strText);
	}
	free(buff);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CQuestEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CQuestEditorApp
BEGIN_MESSAGE_MAP(CQuestEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CQuestEditorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	ON_COMMAND(ID_OPEN_SCRT, CWinApp::OnFileOpen)
	//}}AFX_MSG_MAP
#ifdef USE_NEW_PROJ
	ON_COMMAND(IDC_CRT_PROJ, OnCrtProj)
	ON_COMMAND(IDC_MAKE_DIR_STRUCT, OnMakeDirStruct)
#endif // USE_NEW_PROJ
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuestEditorApp construction

CQuestEditorApp::CQuestEditorApp():
  m_pDocTemplate(NULL)
, idleProcessor_(NULL)
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CQuestEditorApp object

CQuestEditorApp theApp;

CQuestEditorApp* GetMyApp()
{
	return &theApp;
}

/////////////////////////////////////////////////////////////////////////////
// CQuestEditorApp initialization

BOOL CQuestEditorApp::InitInstance()
{
	AfxEnableControlContainer();
	AfxOleInit();

	FileExtAssociator __fa__;
//	__fa__.DO(IDS_FILE_EXT_REG_KEY);
	__fa__.DO(IDS_FILE_EXT_REG_KEY2);
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("K-D Lab"));

	CString res = GetProfileString(CString((LPTSTR)IDS_REG_SEC_NAME_GEN_OPT),
					CString((LPTSTR)IDS_REG_VAL_NAME_LAST_CUR_DIR), 
					disk_op::GetPathFromFullFileName(disk_op::GetModulePath()));

	make_settings_list();
	load_settings_list();
	
	SetCurrentDirectory(res);

	LoadStdProfileSettings();
	
	qdGameConfig::get_config().load();
	
	grDispatcher::sys_init();

	// Грузим перестановку, определяющую порядок следования названий условий в списке
	CString permut_file;
	permut_file.LoadString(IDS_CONDITION_LIST_SETUP_FILE);
	permut_file = disk_op::GetPathFromFullFileName(disk_op::GetModulePath()) + permut_file;
	g_permutation.load(permut_file);
	
#ifdef USE_NEW_PROJ
	QEDocTemplate* pDocTemplate = new QEDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(Project),
		RUNTIME_CLASS(MainFrame),       // main SDI frame window
		RUNTIME_CLASS(StructureView));
	if (!pDocTemplate)
		return FALSE;
	m_pDocTemplate = pDocTemplate;
	AddDocTemplate(pDocTemplate);
	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	m_nCmdShow = SW_MAXIMIZE;
	CCommandLineInfo cmdInfo; 
	ParseCommandLine(cmdInfo);
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

#else
	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;
#endif

	// create and load the frame with its resources

#if !defined USE_NEW_PROJ
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
		NULL,
		NULL);
#endif
	
	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_MAXIMIZE);
	m_pMainWnd->UpdateWindow();
	
#if !defined USE_NEW_PROJ
	if (__argc > 1)
	{
		CString file_dir = disk_op::GetPathFromFullFileName(__argv[1]);
		if(!file_dir.IsEmpty())
			VERIFY(SetCurrentDirectory(file_dir));
		else
			SetCurrentDirectory(
			disk_op::GetPathFromFullFileName(disk_op::GetModulePath()));

		pFrame->OpenScript(PathFindFileName(__argv[1]));
	}
#endif
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CQuestEditorApp message handlers

void CQuestEditorApp::setIdleProcessor(IIdleProcessor* processor){
	idleProcessor_ = processor;
}

IIdleProcessor* CQuestEditorApp::getIdleProcessor(){
	return idleProcessor_;
}

int CQuestEditorApp::ExitInstance() 
{
	grDispatcher::sys_finit();
	
	WriteProfileString(CString((LPTSTR)IDS_REG_SEC_NAME_GEN_OPT),
		CString((LPTSTR)IDS_REG_VAL_NAME_LAST_CUR_DIR), 
		CurDirSafer::GetCurrentDirectory());
	
	save_settings_list();
	return CWinApp::ExitInstance();
}

CDocument* CQuestEditorApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
#if !defined USE_NEW_PROJ
	if(PathFileExists(lpszFileName))
	{
		if(static_cast<CMainFrame*>(m_pMainWnd)->SaveAndClose(true))
		{
			CString file_dir = disk_op::GetPathFromFullFileName(lpszFileName);

			ASSERT(!file_dir.IsEmpty());
			SetCurrentDirectory(file_dir);

			static_cast<CMainFrame*>(m_pMainWnd)->
				OpenScript(PathFindFileName(lpszFileName));
			//может в будещем это будет баг!!!
			return reinterpret_cast<CDocument*>(TRUE);
		}
	}
	MessageBox(m_pMainWnd->m_hWnd, CString((LPTSTR)IDS_FILE_NOT_FOUND), 
								CString((LPTSTR)IDS_ERR_MSG_CAPTION), MB_OK);
	return NULL;
#else
	return CWinApp::OpenDocumentFile(lpszFileName);
#endif
}

void CQuestEditorApp::make_settings_list()
{
	CString strGenOptSecName((LPTSTR)IDS_REG_SEC_NAME_GEN_OPT);
	ADD_INT_SETTING(AUTOSAVE, strGenOptSecName);
	ADD_INT_SETTING(AUTOSAVE_PERIOD, strGenOptSecName);
	/*
	 *	настройки сетки
	 */
	CString strSectionName((LPTSTR)IDS_REG_SEC_NAME_NET_DEFAULTS);
	ADD_INT_SETTING(NET_ANGLE_X, strSectionName);
	ADD_INT_SETTING(NET_ANGLE_Y, strSectionName);
	ADD_INT_SETTING(NET_ANGLE_Z, strSectionName);

	ADD_INT_SETTING(NET_FOCUS, strSectionName);
	ADD_INT_SETTING(NET_VERT_OFFSET, strSectionName);

	ADD_INT_SETTING(NET_SX,			strSectionName);
	ADD_INT_SETTING(NET_SY,			strSectionName);
	ADD_INT_SETTING(NET_CELL_SZ,	strSectionName);

	ADD_STR_SETTING(NET_SCALE_Z_OFFSET, strSectionName);
	ADD_STR_SETTING(NET_SCALE_POW, strSectionName);

	/*
	 *	настройки редактора триггеров
	 */
	strSectionName.LoadString(IDS_REG_SEC_NAME_TE);
	ADD_INT_SETTING(TE_NET_CELL_SX,			strSectionName);
	ADD_INT_SETTING(TE_NET_CELL_SY,			strSectionName);
	ADD_INT_SETTING(TE_NET_BIND_TO_NODE,	strSectionName);
	ADD_INT_SETTING(TE_NET_AMONG_CELL_SX,	strSectionName);
	ADD_INT_SETTING(TE_NET_AMONG_CELL_SY,	strSectionName);
	ADD_INT_SETTING(TE_SHOW_MENU_ON_LINK_CREATE, strSectionName);

	/*
	 *	настройки дерева проекта
	 */
	ADD_INT_SETTING(PT_COLLAPSE_SCENES, strGenOptSecName);
	/*
	 *	цвета для условий
	 */
	//Не кладем в реестр
/*
	ADD_INT_SETTING(COND_MOUSE_CLICK_COLOR,						strSectionName);
	ADD_INT_SETTING(COND_MOUSE_OBJECT_CLICK_COLOR,				strSectionName);
	ADD_INT_SETTING(COND_OBJECT_IN_ZONE_COLOR,					strSectionName);
	ADD_INT_SETTING(COND_PERSONAGE_WALK_DIRECTION_COLOR,		strSectionName);
	ADD_INT_SETTING(COND_PERSONAGE_STATIC_DIRECTION_COLOR,		strSectionName);
	ADD_INT_SETTING(COND_TIMER_COLOR,							strSectionName);
	ADD_INT_SETTING(COND_MOUSE_DIALOG_CLICK_COLOR,				strSectionName);
	ADD_INT_SETTING(COND_MINIGAME_IN_STATE_COLOR,				strSectionName);
	ADD_INT_SETTING(COND_OBJECT_STATE_COLOR,					strSectionName);
	ADD_INT_SETTING(COND_MOUSE_ZONE_CLICK_COLOR,				strSectionName);
	ADD_INT_SETTING(COND_MOUSE_OBJECT_ZONE_CLICK_COLOR,			strSectionName);
	ADD_INT_SETTING(COND_OBJECT_STATE_WAS_ACTIVATED_COLOR,		strSectionName);
	ADD_INT_SETTING(COND_OBJECTS_DISTANCE_COLOR,				strSectionName);

	ADD_INT_SETTING(COND_OBJECT_STATE_ANIMATION_PHASE_COLOR,		strSectionName);
	ADD_INT_SETTING(COND_OBJECT_PREV_STATE_COLOR,					strSectionName);
	ADD_INT_SETTING(COND_STATE_TIME_GREATER_THAN_VALUE_COLOR,		strSectionName);
	ADD_INT_SETTING(COND_STATE_TIME_GREATER_THAN_STATE_TIME_COLOR,	strSectionName);
	ADD_INT_SETTING(COND_STATE_TIME_IN_INTERVAL_COLOR,				strSectionName);
	ADD_INT_SETTING(COND_COUNTER_GREATER_THAN_VALUE_COLOR,			strSectionName);
	ADD_INT_SETTING(COND_COUNTER_LESS_THAN_VALUE_COLOR,				strSectionName);
	ADD_INT_SETTING(COND_COUNTER_GREATER_THAN_COUNTER_COLOR,		strSectionName);
	ADD_INT_SETTING(COND_COUNTER_IN_INTERVAL_COLOR,					strSectionName);
	ADD_INT_SETTING(COND_OBJECT_ON_PERSONAGE_WAY_COLOR,				strSectionName);
	ADD_INT_SETTING(COND_KEYPRESS_COLOR,							strSectionName);
	ADD_INT_SETTING(COND_ANY_PERSONAGE_IN_ZONE_COLOR,				strSectionName);
*/
	/*
	 *	настройки окна просмотра сцены
	 */
	
	strSectionName.LoadString(IDS_REG_SEC_NAME_SV);
	ADD_INT_SETTING(SV_LEFT_RIGHT_OFFSET,	strSectionName);
	ADD_INT_SETTING(SV_TOP_BOTTOM_OFFSET,	strSectionName);
}

bool CQuestEditorApp::add_string_setting(UINT uiSettingName, 
			const CString& strSecName, UINT uiValueName, CString strDefVal)
{
	CString strSettingName((LPTSTR)uiSettingName);
	CString strValueName((LPTSTR)uiValueName);

	m_Settings.insert(std::make_pair(strSettingName, 
			reg_setting(strSecName, strValueName, strDefVal)));
	return true;
}

bool CQuestEditorApp::add_int_setting(UINT uiSettingName, 
			const CString& strSecName, UINT uiValueName, int iDefVal)
{
	CString strSettingName((LPTSTR)uiSettingName);
	CString strValueName((LPTSTR)uiValueName);
	m_Settings.insert(std::make_pair(strSettingName, 
			reg_setting(strSecName, strValueName, iDefVal)));
	return true;
}

Setting& CQuestEditorApp::GetSetting(const CString& strSettingName)
{
	ASSERT(m_Settings.find(strSettingName) != m_Settings.end());	

	return m_Settings.find(strSettingName)->second;
}

Setting& CQuestEditorApp::GetSetting(UINT uiSettingName)
{
	return GetSetting(CString((LPTSTR)uiSettingName));
}

int CQuestEditorApp::GetIntSetting(const CString& strSettingName)
{
	return GetSetting(strSettingName);
}

int CQuestEditorApp::GetIntSetting(UINT uiSettingName)
{
	return GetIntSetting(CString((LPTSTR)uiSettingName));
}

const CString CQuestEditorApp::GetStrSetting(const CString& strSettingName)
{
	return GetSetting(strSettingName);
}

const CString CQuestEditorApp::GetStrSetting(UINT uiSettingName)
{
	return GetSetting(CString((LPTSTR)uiSettingName));
}

bool CQuestEditorApp::WriteIntSetting(const CString& strSettingName, int Val)
{
	return GetSetting(strSettingName).set_value(Val);
}

bool CQuestEditorApp::WriteIntSetting(UINT uiSettingName, int Val)
{
	return WriteIntSetting(CString((LPTSTR)uiSettingName), Val);
}

bool CQuestEditorApp::WriteStrSetting(const CString& strSettingName, CString Val)
{
	ASSERT(m_Settings.find(strSettingName) != m_Settings.end());	
	return m_Settings.find(strSettingName)->second.set_value(Val);
}

bool CQuestEditorApp::WriteStrSetting(UINT uiSettingName, CString Val)
{
	return WriteStrSetting(CString((LPTSTR)uiSettingName), Val);
}


void CQuestEditorApp::load_settings_list()
{
	SettingsMap_t::iterator _itr = m_Settings.begin(), _end = m_Settings.end();
	for(; _itr != _end; ++_itr)
	{
		reg_setting& s = _itr->second;
		switch (s.type()) {
		case Setting::TYPE_INT:
			{
				s.set_value(static_cast<int>(
							GetProfileInt(s.GetSecName(), s.GetValName(), s)));
			}
			break;
		case Setting::TYPE_STRING:
			{
				s.set_value(
						GetProfileString(s.GetSecName(), s.GetValName(), s));
			}
			break;
		default:
			ASSERT(("load_settings_list()", 0));
		}
	}
}

void CQuestEditorApp::save_settings_list()
{
	SettingsMap_t::iterator _itr = m_Settings.begin(), _end = m_Settings.end();
	for(; _itr != _end; ++_itr)
	{
		reg_setting& s = _itr->second;
		switch (s.type()) {
		case Setting::TYPE_INT:
			{
				WriteProfileInt(s.GetSecName(), s.GetValName(), s);
			}
			break;
		case Setting::TYPE_STRING:
			{
				WriteProfileString(s.GetSecName(), s.GetValName(), s);
			}
			break;
		default:
			ASSERT(("write_settings_list()", 0));
		}
	}
}

const CString CQuestEditorApp::GetProjIniFileName()			
{
#ifdef USE_NEW_PROJ
	return m_IniMngr.getFileName();
#else
	CMainFrame* pmf = static_cast<CMainFrame*>(GetMainWnd());
	return pmf->GetProjIniFileName();
#endif // USE_NEW_PROJ
}

bool CQuestEditorApp::ResetProjectSettingsMgr(const CString& strIniName)
{
	m_IniMngr.SetFileName(strIniName);
	return true;
}

IProjectSettings* CQuestEditorApp::GetProjectSettings() {
	return &m_IniMngr;
}
BOOL CQuestEditorApp::OnCmdMsg(UINT nID, int nCode, 
							   void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
//	if (AfxGetMainWnd()&&AfxGetMainWnd()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
//		return TRUE;
	return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CQuestEditorApp::OnIdle(LONG lCount)
{
	BOOL bMore = CWinApp::OnIdle(lCount);
	if (getIdleProcessor())
		bMore = getIdleProcessor()->OnIdle(lCount);
	else
		bMore = FALSE;
	return bMore;
}

#ifdef USE_NEW_PROJ
void CQuestEditorApp::OnCrtProj()
{
	CString str;
	str.LoadString(IDS_OPEN_SCRIPT_EXT);
	CString str2;
	str2.LoadString(IDS_NEW_SCRIPT_FILTER);
	CString strTitle((LPTSTR)IDS_OSD_CAP_CREATE);

	CString strFN;
	strFN.LoadString(IDS_NEW_PROJ_FILE_NAME);
	CFileDialog dlg(false, str, strFN, OFN_OVERWRITEPROMPT, str2);
	CString curDir = CurDirSafer::GetCurrentDirectory();
	dlg.m_ofn.lpstrInitialDir = curDir;
	dlg.m_ofn.lpstrTitle = strTitle;

	if(dlg.DoModal() == IDOK)
	{
		if(DefaultResourceCreator::create())
		{
			{
				qdGameDispatcher disp;
				disp.save_script(dlg.GetPathName());
			}
			Project* proj = 
				static_cast<Project*>(OpenDocumentFile(dlg.GetPathName()));
			if (proj)
				proj->showNewProjWiz();
		}
		else
		{
			AfxMessageBox(IDS_CANNT_CRT_DEFAULT_RESOUCES);
		}
	}
}

void CQuestEditorApp::OnMakeDirStruct()
{
	DirMakeWizardShower wiz;
	wiz.show(AfxGetMainWnd());
}
#endif // USE_NEW_PROJ

