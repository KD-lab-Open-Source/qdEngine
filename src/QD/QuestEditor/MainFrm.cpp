// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "QuestEditor.h"

#include "MainFrm.h"
#include "gdi_gr_dispatcher.h"

#include <Mmreg.h>
#include <Shlwapi.h>

#include "qd_game_object_moving.h"
#include "qd_game_object_animated.h"
#include "ds_snd_dispatcher.h"
#include "qdscr_convert.h"

#include "qd_game_dispatcher.h"
#include "SceneTreeView.h"
#include "ChildView.h"
#include "tewnd.h"

#include "triggerproflist.h"

#include "qd_setup.h"
#include "app_core.h"

#include "CurDirSafer.h"
#include "editstatedlg.h"

#include "statecoordsstacker.h"

#include "disk_op.h"

#include "AppStngsDlg.h"
#include "scenelist.h"
#include "ScaleMgr.h"
#include "GenStngsPP.h"

#include "ProcessLauncher.h"
#include "Object List/ObjectListMngr.h"


#include "app_log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT TIMER_EVENT_COORD_ANIMATION = 1;

namespace{
	bool g_bProjChanged = false;
}
void SetProjChange(){
	g_bProjChanged = true;
}

void ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID = -1);
void ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID = -1);

bool AskUserForName(CString& strDef);


/************************************************************************/
/* Глобальные                                                           */
/************************************************************************/
//используется для перирисовки сцены в других частях программы,
//чтобы не надо было везде подключать mainfrm.h
void RedrawScene(bool bRedrawScene){
	static_cast<CMainFrame*>(AfxGetMainWnd())->InvalidateSceneView(bRedrawScene);
}

CRect GetSceneTBRect()
{
	CQuestEditorApp* papp = GetMyApp();
	CRect res;
	res.left = papp->GetIntSetting(SETTING_NAME(SCENE_TB_LEFT));
	res.top = papp->GetIntSetting(SETTING_NAME(SCENE_TB_TOP));
	res.right = papp->GetIntSetting(SETTING_NAME(SCENE_TB_RIGHT));
	res.bottom = papp->GetIntSetting(SETTING_NAME(SCENE_TB_BOTTOM));
	return res;
}

CRect GetTETBRect()
{
	CQuestEditorApp* papp = GetMyApp();
	CRect res;
	res.left = papp->GetIntSetting(SETTING_NAME(TE_TB_LEFT));
	res.top = papp->GetIntSetting(SETTING_NAME(TE_TB_TOP));
	res.right = papp->GetIntSetting(SETTING_NAME(TE_TB_RIGHT));
	res.bottom = papp->GetIntSetting(SETTING_NAME(TE_TB_BOTTOM));
	return res;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_OPEN_SCRT, OnOpenScrt)
	ON_COMMAND(ID_FILE_SAVE_AS, OnSaveAsScrt)
	ON_COMMAND(IDC_CRT_PROJ, OnCrtProj)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateSaveAsScrt)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateSaveScrt)
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(IDC_SEL_SCENE, OnUpdateSelScene)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_GAITS, OnUpdateEditGaits)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_TRIGGERS, OnUpdateEditTriggers)
	ON_COMMAND(ID__100, On100)
	ON_COMMAND(ID__200, On200)
	ON_COMMAND(ID__25, On25)
	ON_COMMAND(ID__300, On300)
	ON_COMMAND(ID__50, On50)
	ON_COMMAND(ID__SEL_MASTAB, OnSelMastab)
	ON_UPDATE_COMMAND_UI(ID__100, OnUpdate100)
	ON_UPDATE_COMMAND_UI(ID__200, OnUpdate200)
	ON_UPDATE_COMMAND_UI(ID__25, OnUpdate25)
	ON_UPDATE_COMMAND_UI(ID__300, OnUpdate300)
	ON_UPDATE_COMMAND_UI(ID__50, OnUpdate50)
	ON_UPDATE_COMMAND_UI(ID__SEL_MASTAB, OnUpdateSelMastab)
	ON_WM_TIMER()
	ON_COMMAND(IDC_APP_SETTINGS, OnAppSettings)
	ON_COMMAND(ID_FILE_SAVE, OnSaveScrt)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnNeedText)
	
	ON_COMMAND(ID_VIEW_SCENE_TOOLBAR, OnViewSceneToolbar)
	ON_COMMAND(ID_VIEW_TE_TOOLBAR, OnViewTeToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TE_TOOLBAR, OnUpdateViewTeToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCENE_TOOLBAR, OnUpdateViewSceneToolbar)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)
	ON_COMMAND(ID_VIEW_TRIG_DBG_BAR, OnViewTrigDbgBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRIG_DBG_BAR, OnUpdateViewTrigDbgBar)
	ON_COMMAND(IDC_LAST_NAIL, OnLastNail)
	ON_UPDATE_COMMAND_UI(IDC_LAST_NAIL, OnUpdateLastNail)
	ON_COMMAND(IDC_SCALE_MGR, OnScaleMgr)
	ON_UPDATE_COMMAND_UI(IDC_SCALE_MGR, OnUpdateScaleMgr)
	ON_COMMAND(ID_VIEW_OBJECTLIST, OnViewObjectlist)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OBJECTLIST, OnUpdateViewObjectlist)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};



/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_wndSTView = NULL;

	m_uiAutoSaveTimer = 0;

	appLog::default_log().enable();
}

CMainFrame::~CMainFrame()
{
	qdGameDispatcher::free_font(grDispatcher::get_default_font());
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_ptrSplitterClient = (CSplitterClient*)m_wndSplitter.GetPane(0, 1);

	m_wndTE = std::auto_ptr<TEWnd>(new TEWnd);
	if (!m_wndTE->Create(NULL, NULL, WS_CHILD | WS_BORDER,
		CRect(0, 0, 0, 0), m_ptrSplitterClient, 0, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	m_wndView = std::auto_ptr<CChildView>(new CChildView);

	if (!m_wndView->Create(NULL, NULL, WS_CHILD/* | WS_BORDER */| WS_VISIBLE
			|WS_CLIPCHILDREN,
		CRect(0, 0, 0, 0), m_ptrSplitterClient, 0, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	m_ptrSplitterClient->SetResizedChild(m_wndView.get());
	m_wndView->set_main_frame(this);

	m_wndSTView = (SceneTreeView*)m_wndSplitter.GetPane(0, 0);
	m_wndSTView->SetMainFrame(this);
	m_wndSTView->set_views(m_wndView.get(), 
		static_cast<ITriggerView*>(m_wndTE.get()));

	m_ptrGRD = std::auto_ptr<GDI_grDispatcher>(new GDI_grDispatcher);
	init_graphics();
	appSetHandle(m_wndView->m_hWnd);
	m_ptrDSSoundDisp = std::auto_ptr<ds_sndDispatcher>(new ds_sndDispatcher);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, 
		CRect(0, 0, 0, 0), MAINFRAME_TOOLBAR_ID) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	if (!m_wndSceneToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD /*| WS_VISIBLE */| CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, 
		CRect(0, 0, 0, 0), SCENE_TOOLBAR_ID) ||
		!m_wndSceneToolBar.LoadToolBar(IDR_TOOLBAR_SCENE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	if (!m_wndTEToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, 
		CRect(0, 0, 0, 0), TE_TOOLBAR_ID) ||
		!m_wndTEToolBar.LoadToolBar(IDR_TE_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}


	//  be dockable
	EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar, AFX_IDW_DOCKBAR_TOP);

	m_wndSceneToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndSceneToolBar, AFX_IDW_DOCKBAR_TOP);

	//Сделаем кнопку со стрелкой
	DWORD dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
	m_wndSceneToolBar.GetToolBarCtrl().
		SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle);

	DWORD dwStyle = m_wndSceneToolBar.GetButtonStyle(
									m_wndSceneToolBar.CommandToIndex(IDC_EDIT_TRIGGERS));
	dwStyle |= TBSTYLE_DROPDOWN;
	m_wndSceneToolBar.SetButtonStyle(
		m_wndSceneToolBar.CommandToIndex(IDC_EDIT_TRIGGERS), dwStyle);

	m_wndTEToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndTEToolBar, AFX_IDW_DOCKBAR_TOP);

	m_wndTrigProf.reset(new TriggerProfList);

	if (!m_wndTrigProf.get()||!m_wndTrigProf->Create(this, m_wndTE.get(), WS_CHILD |
		CBRS_FLYBY | CBRS_RIGHT | 
		CBRS_SIZE_DYNAMIC))
	{
		TRACE0("Failed to create m_wndTrigDbgDlgBar \n");
		return -1;
	}
	m_wndTE->SetTriggerProfiler(m_wndTrigProf.get());

	m_wndScaleMgr.reset(new ScaleMgr);
	if (!m_wndScaleMgr.get())
		return -1;
	if (!m_wndScaleMgr->Create(this, SCALE_TOOLBAR_ID))
	{
		TRACE0("Failed to create m_wndScaleMgr \n");
		return -1;
	}

	objectListMngr_.reset(new ObjectListMngr);
	if (!objectListMngr_||!objectListMngr_->create(this,  WS_CHILD |
								CBRS_FLYBY | CBRS_RIGHT | CBRS_SIZE_DYNAMIC))
	{
		TRACE0("Failed to create Object List Manager \n");
		return -1;
	}

	m_wndSTView->SetObjectList(objectListMngr_.get());
	m_wndSTView->SetScaleMgr(m_wndScaleMgr.get());
	m_wndView->SetScaleMgr(m_wndScaleMgr.get());
	static_cast<IScalable*>(m_wndTE.get())->SetScaleMgr(m_wndScaleMgr.get());
	m_wndScaleMgr->SetScalable(m_wndView.get());

	m_wndScaleMgr->Show();
//	objectListMngr_->show();
	ShowControlBar(&m_wndSceneToolBar, TRUE, FALSE);
	ShowControlBar(&m_wndTEToolBar, TRUE, FALSE);
	LoadBarState(CString((LPTSTR)IDS_REG_SEC_NAME_TOOLBARS));
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_MAIN_ICON);
	cs.lpszClass = AfxRegisterWndClass(0, 0, 0, hIcon);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
//	if(m_wndView.get())
//		m_wndView->set_focus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.get()&&m_wndView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;

	if (m_wndSTView&&m_wndSTView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
		return TRUE;
	}

	if (m_wndTE.get()&&m_wndTE->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
		return TRUE;
	}
	if (m_wndScaleMgr.get()
		&&m_wndScaleMgr->GetView()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// CG: The following block was added by the Splitter Bar component.
	{
		CCreateContext Context;
		memset(&Context, 0, sizeof(CCreateContext));
		Context.m_pCurrentFrame = this;
		if(!m_wndSplitter.CreateStatic(this,1, 2))
		{
			TRACE0("Failed to create splitter bar ");
			return FALSE;    // failed to create
		}
		
		m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CSplitterClient),CSize(0, 0),
			&Context);
		m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(SceneTreeView),CSize(0, 0),
			&Context);
		
		
		CRect rect;
		GetWindowRect( &rect );
		
		m_wndSplitter.SetColumnInfo(0, rect.Width()/4, 30);
		//		m_wndSplitter.SetColumnInfo(1, rect.Width()/3, 30);
		return TRUE;
	}
}

bool CMainFrame::SaveAndClose(bool bAsk)
{
	if(g_bProjChanged&&bAsk)
	{
		int res = 0;
		res = AfxMessageBox(IDS_SAVE_PROJ_MSG, MB_YESNOCANCEL);
		if(res == IDYES)
		{
			SaveProj();
		}
		else if(res == IDCANCEL)
			return false;
	}
	else if (g_bProjChanged) {
		SaveProj();
	}
	SaveProjIni();
	return true;
}

void CMainFrame::OnOpenScrt() 
{
	SaveAndClose(true);
	
	CString str;
	str.LoadString(IDS_OPEN_SCRIPT_EXT);
	CString str2;
	str2.LoadString(IDS_OPEN_SCRIPT_FILTER);

	//здесь мы ничего не делаем для сохранения тек. дир., 
	//потому что всё что делается в других местах, направленно
	//на то, чтобы сохранить выставленную здесь тек. дир.
	CFileDialog dlg(true, str, NULL, OFN_ENABLESIZING , str2);
	CString curDir = CurDirSafer::GetCurrentDirectory();
	TRACE(_T("Current Directory - "));
	TRACE(curDir);
	TRACE(_T("\n"));

	dlg.m_ofn.lpstrInitialDir = curDir;
	if(dlg.DoModal() == IDOK)
		OpenScript(dlg.GetFileName());
	TRACE(_T("Current Directory after open- "));
	TRACE(CurDirSafer::GetCurrentDirectory());
	TRACE(_T("\n"));
}

void CMainFrame::init_graphics(void)
{
	grDispatcher::instance()->Finit();
	
	grDispatcher::instance()->init(800, 600,
		GR_ARGB1555,static_cast<void*>(m_wndView->m_hWnd));
	grDispatcher::instance()-> SetClip();
	grDispatcher::instance()-> SetClipMode(1);
	
	grDispatcher::instance()-> Fill(0);
}

void CMainFrame::OnSaveScrt() 
{
	SaveProj();
}

void CMainFrame::OnSaveAsScrt() 
{
	CString str;
	str.LoadString(IDS_OPEN_SCRIPT_EXT);
	CString str2;
	str2.LoadString(IDS_OPEN_SCRIPT_FILTER);
	CString strTitle((LPTSTR)IDS_OSD_CAP_SAVE_AS);
	
	CFileDialog dlg(FALSE, str, NULL, OFN_OVERWRITEPROMPT, str2);
	CString curDir = CurDirSafer::GetCurrentDirectory();
	dlg.m_ofn.lpstrInitialDir = curDir;
	dlg.m_ofn.lpstrTitle = strTitle;

	if(dlg.DoModal() == IDOK)
	{
		CWaitCursor __wait__;
		m_strFileName = dlg.GetFileName();
		SaveProj();
		//имя проекта в загловок
		SetCaption(m_strFileName);
		AfxGetApp()->AddToRecentFileList(dlg.GetPathName());
		GetMyApp()->ResetProjectSettingsMgr(GetProjIniFileName());
		SaveProjIni();
	}
}

void CMainFrame::ShowObjectInTree(qdNamedObject* ptrObj)
{
	m_wndSTView->ShowObjectInTree(ptrObj);
}

void CMainFrame::ShowProperties(qdNamedObject* ptrObj)
{
	m_wndSTView->ShowProperties(ptrObj, 0);
}

void CMainFrame::DeleteObject(qdNamedObject* ptrObj)
{
	m_wndSTView->DeleteObject(ptrObj, true);
}

//перерисовывает окно, в котором отабражена сцена
void CMainFrame::InvalidateSceneView(bool bRedrawScene)
{
	CSingleLock __sl(m_wndView->GetDrawGuard());
	if(bRedrawScene)
	{
		qdGameScene* pscene = m_ptrGameDisp->get_active_scene();
		if(pscene)
			m_wndView->RedrawScene(pscene);
	}

	CRect r;
	m_wndView->GetClientRect(r);
	m_wndView->PaintRect(r);
}

//void CMainFrame::SelectObjectInSceneView(qdGameObject* ptrObj)
//{
//	m_wndView->SelectObj(ptrObj);
//}

void CMainFrame::DeleteTmpCopy()
{
	if (m_strTempCopyFileName.IsEmpty())
		return;
	CString str = m_strProjDir;
	str += _T('\\');
	str += m_strTempCopyFileName;
	DeleteFile(str);
}

void CMainFrame::reset()
{
	g_bProjChanged = false;

	if(m_wndSTView)
		m_wndSTView->reset();
	ResetSceneView();
	m_wndTE->reset();

	if(m_uiAutoSaveTimer)
		KillTimer(m_uiAutoSaveTimer);

	DeleteTmpCopy();
	Invalidate();
}

bool CMainFrame::AdjustBitmapSize()
{
	m_wndView->AdjustBitmapSize();
	return true;
}

#include "relativepath.h"
#include "WizardSheet.h"
#include ".\mainfrm.h"
bool CreateDirStruct(CString file_name)
{
	CStringArray ar;
	r_p::DecompositePath(file_name,ar);
	int size = ar.GetSize();
	CString dir;
	for(int i = 0; i < size; ++i)
	{
		dir += ar.GetAt(i);
		if(!CreateDirectory(dir, NULL))
		{
			if(ERROR_FILE_EXISTS != GetLastError()&&
				ERROR_ALREADY_EXISTS != GetLastError())
				return false;
		}
		dir += "\\";
	}
	return true;
}
bool SaveResource(CString strFileName, DWORD rid, LPCTSTR strType)
{
	HRSRC hrsc0	= FindResource(NULL,MAKEINTRESOURCE(rid),strType);
	if (!hrsc0) 
		return false;
	DWORD buf_sz = SizeofResource(NULL,(HRSRC)hrsc0);
	if(!buf_sz)
		return false;
	HGLOBAL hrsc1 = LoadResource(NULL,(HRSRC)hrsc0);
	if(!hrsc1)
		return false;
	
	char* buf = (char*)LockResource(hrsc1);	
	if(!buf)
		return false;

	if(!CreateDirStruct(strFileName))
		return false;

	HANDLE hFile = CreateFile(strFileName, GENERIC_WRITE, 0, NULL, 
							CREATE_NEW, FILE_FLAG_WRITE_THROUGH, NULL); 
	if(hFile == INVALID_HANDLE_VALUE)
	{
		//если файл с таким именем уже есть, считаем что он правильный
		if(ERROR_FILE_EXISTS == GetLastError()||
			ERROR_ALREADY_EXISTS == GetLastError())
			return true;

		return false;
	}
	DWORD nWritten = 0;
	BOOL res = WriteFile(hFile, buf, buf_sz, &nWritten, NULL);
	res = (buf_sz == nWritten);

	CloseHandle(hFile);
	return static_cast<bool>(res);
}

bool CMainFrame::CreateDefResource()
{
	CString file_name;
	file_name.LoadString(IDS_DEF_CURSOR_FILE_NAME);
	if(!SaveResource(file_name, IDR_DEFAULT_CURSOR, "TGA"))
	{
		if(ERROR_ALREADY_EXISTS != GetLastError())
			return false;
	}
	file_name.LoadString(IDS_DEF_FONT_FILE_NAME);
	if(!SaveResource(file_name, IDR_DEFAULT_FONT, "TGA"))
	{
	}
	file_name.LoadString(IDS_DEF_FONT_IDX_FILE_NAME);
	if(!SaveResource(file_name, IDR_DEFAULT_FONT_IDX, "IDX"))
	{
		if(ERROR_ALREADY_EXISTS != GetLastError())
			return false;
	}
	return true;
}

void CMainFrame::OnCrtProj() 
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
		m_strFileName = dlg.GetFileName();

		if(CreateDefResource())
		{
			reset();

			m_ptrGameDisp.reset(new qdGameDispatcher);
			qdGameDispatcher::set_dispatcher(m_ptrGameDisp.get());
			grDispatcher::set_default_font(qdGameDispatcher::create_font(0));

			m_ptrGameDisp->save_script(m_strFileName);

			m_wndSTView->LoadStructTree(m_ptrGameDisp.get());

			//имя проекта в загловок
			SetCaption(m_strFileName);
			m_wndSplitter.SetActivePane(0, 0);

			m_wndSTView->ShowNewProjWiz();

			AfxGetApp()->AddToRecentFileList(dlg.GetPathName());
		}
		else
		{
			ShowError(*this, IDS_CANNT_CRT_DEFAULT_RESOUCES);
		}
	}
}

void CMainFrame::SetCaption(const CString str)
{
	CString cur = AfxGetApp()->m_pszAppName;

	cur += " - ";
	cur += str;
	SetWindowText(cur);
}

CString CMainFrame::GetCurrScriptFileName()
{
	return m_strFileName;
}

void CMainFrame::ResetSceneView()
{
	if(m_wndView.get())
		m_wndView->reset();
}

void CMainFrame::OnUpdateSaveAsScrt(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp.get()!=NULL);	
}

void CMainFrame::OnUpdateSaveScrt(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp.get()!=NULL/*&&g_bProjChanged*/);
}

void CMainFrame::OnClose() 
{
	CString strMsg(m_wndSTView->CanClose());
	if (!strMsg.IsEmpty()) {
		ShowError(*this, strMsg, IDS_ERR_MSG_CAPTION);
		return;
	}

	if (!SaveAndClose(true)) 
		return;
	
	SaveBarState(CString((LPTSTR)IDS_REG_SEC_NAME_TOOLBARS));

	DeleteTmpCopy();
	CFrameWnd::OnClose();
}

CString CheckExtension(const CString& strFileName){
	CString res(strFileName.Right(3));
	if (!res.CompareNoCase(_T("qds"))) 
	{
		res = strFileName.Left(strFileName.GetLength()-3);
		res += _T("qml");
		return res;
	}
	return strFileName;
}


CString make_ini_file_name(const CString& strFileName)
{
	int g = strFileName.Find(_T(".qml"));
	if (g == -1) 
		g = strFileName.Find(_T(".qds"));

	ASSERT(g != -1);
	return (strFileName.Left(g) + _T(".ini"));
}

const CString CMainFrame::GetProjIniFileName() const{
	CString strIniFileName = m_strProjDir;
	strIniFileName += _T("\\");
	strIniFileName += make_ini_file_name(m_strFileName);
	return strIniFileName;
}

void CMainFrame::SaveProjIni()
{
	if (!m_ptrGameDisp.get())
		return;
	m_wndSTView->SaveIni();
	GetMyApp()->GetProjectSettings()->Save();
}

void CMainFrame::ReadProjIni()
{
	GetMyApp()->ResetProjectSettingsMgr(GetProjIniFileName());
//	m_wndSTView->ReadIni(GetProjIniFileName());
	m_wndSTView->ReadIni();
}

void CMainFrame::SaveProj()
{
	CWaitCursor __wait__;
//	m_ptrGameDisp->save_script(m_strFileName);
	if (!SafeSaveScript(m_ptrGameDisp.get(), m_strFileName))
	{
		ShowError(m_hWnd, IDS_ERR_SAVE_FAILED);
	}
	g_bProjChanged = false;
}

bool SelectName(CString& strName){
	do {
		if(!AskUserForName(strName))
			return false;
		LPCTSTR lpszExt = PathFindExtension(strName);
		if (!lpszExt||_tcsicmp(lpszExt, _T(".qml"))){
			strName += _T(".qml");
		}

		if(!PathFileExists(strName))
			return true;
		CString msg;
		msg.Format(IDS_FILE_CONVERTER_FILE_EXIST_SELECT_NEW, 
			static_cast<LPCTSTR>(strName));
		AfxMessageBox(msg);
	} while(true);
	return true;
}

const CString CheckFileFormat(const CString &strFileName){
	if (!qdscr_is_XML(strFileName))
	{
		CString strNewFileName(qdscr_get_XML_file_name(strFileName));
		if (PathFileExists(strNewFileName)) 
		{
			CString strMsg;
			strMsg.Format(IDS_FILE_CONVERTER_XML_FILE_EXIST_OVERWRITE,
				static_cast<LPCTSTR>(strFileName));
			int res = MessageBox(AfxGetMainWnd()->m_hWnd,strMsg, NULL, MB_YESNOCANCEL);
			switch(res) {
			case IDYES:
				if(!SelectName(strNewFileName))
					return CString();
				break;
			case IDNO:
				//ничего не делаем
				break;
			case IDCANCEL:
				return CString();
			}
		}
		if(qdscr_convert_to_XML(strFileName, strNewFileName))
			return strNewFileName;
		else
		{
			CString strMsg;
			strMsg.Format(IDS_ERR_CONVERT_TO_XML_FAILED, 
				static_cast<LPCTSTR>(strFileName));

			ShowError(AfxGetMainWnd()->m_hWnd,strMsg);
			return CString();
		}
	}
	return strFileName;
}

bool CMainFrame::OpenScript(const CString & strFileName)
{
	CWaitCursor __wait__;
	reset();

	m_strFileName = CheckFileFormat(strFileName);
	if (m_strFileName.IsEmpty())
		return false;

	m_ptrGameDisp.reset(new qdGameDispatcher);
	qdGameDispatcher::set_dispatcher(m_ptrGameDisp.get());

	m_ptrGameDisp->load_script(m_strFileName);
	m_wndSTView->LoadStructTree(m_ptrGameDisp.get());
	m_wndTE->setGameDisp(m_ptrGameDisp.get());
	
	qdGameDispatcher::free_font(grDispatcher::get_default_font());
	grDispatcher::set_default_font(qdGameDispatcher::create_font(0));
	
/*
	CString def_font;
	def_font.LoadString(IDS_DEF_FONT_FILE_NAME);
	CString def_font_idx;
	def_font_idx.LoadString(IDS_DEF_FONT_IDX_FILE_NAME);
*/
	
	
	m_strProjDir = CurDirSafer::GetCurrentDirectory();

	MakeTempCopyFileName();
	
	//имя проекта в загловок
	SetCaption(m_strFileName);
	
	m_wndSplitter.SetActivePane(0, 0);

	ReadProjIni();

	StartAutoSavePeriod();
	
	AfxGetApp()->AddToRecentFileList(m_strProjDir + _T("\\") + m_strFileName);
	m_wndToolBar.RedrawWindow();
	m_wndSceneToolBar.RedrawWindow();
	m_wndTEToolBar.RedrawWindow();
	return true;
}

void CMainFrame::ShowSceneView()
{
	ShowControlBar(&m_wndTEToolBar, FALSE, FALSE);

	m_wndView->ShowWindow(SW_SHOW);
	m_wndTE->ShowWindow(SW_HIDE);
	m_ptrSplitterClient->SetResizedChild(m_wndView.get());
}

void CMainFrame::ShowTEView()
{
	ShowControlBar(&m_wndTEToolBar, TRUE, FALSE);

	m_wndView->ShowWindow(SW_HIDE);
	m_wndTE->ShowWindow(SW_SHOW);
	m_ptrSplitterClient->SetResizedChild(m_wndTE.get());
}

bool CMainFrame::OnCreateMask(qdGameObject* ptrGO)
{
	return m_wndView->OnCreateStateMask(ptrGO);
}

bool CMainFrame::OnEndStateMaskCreate(qdGameObjectStateMask* ptrStateMask)
{
	return m_wndSTView->OnEndStateMaskCreate(ptrStateMask);
}

BOOL CMainFrame::OnNeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
    UINT nID =pTTTStruct->idFrom;
    if (!(pTTT->uFlags & TTF_IDISHWND))
    {
        if(nID)
        {
            pTTT->lpszText = MAKEINTRESOURCE(nID);
            pTTT->hinst = AfxGetResourceHandle();
            return(TRUE);
        }
    }
	return FALSE;
}


void CMainFrame::OnUpdateSelScene(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp.get() != NULL);	
}

void CMainFrame::OnUpdateEditGaits(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp.get() != NULL);	
}

void CMainFrame::OnUpdateEditTriggers(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp.get() != NULL);	
}

void CMainFrame::On100() 
{
	m_wndView->SetScale(1.f);	
}

void CMainFrame::On200() 
{
	m_wndView->SetScale(2.f);	
}

void CMainFrame::On25() 
{
	m_wndView->SetScale(.25f);	
}

void CMainFrame::On300() 
{
	m_wndView->SetScale(3.f);	
}

void CMainFrame::On50() 
{
	m_wndView->SetScale(.5f);	
}

void CMainFrame::OnSelMastab() 
{
}

void CMainFrame::OnUpdate100(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp.get()&&m_ptrGameDisp->get_active_scene());	
}

void CMainFrame::OnUpdate200(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp.get()&&m_ptrGameDisp->get_active_scene());	
}

void CMainFrame::OnUpdate25(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp.get()&&m_ptrGameDisp->get_active_scene());	
}

void CMainFrame::OnUpdate300(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp.get()&&m_ptrGameDisp->get_active_scene());	
}

void CMainFrame::OnUpdate50(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp.get()&&m_ptrGameDisp->get_active_scene());	
}

void CMainFrame::OnUpdateSelMastab(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ptrGameDisp.get()&&m_ptrGameDisp->get_active_scene());	
}

void CMainFrame::WriteStatus(LPCTSTR lpszString)
{
	m_wndStatusBar.SetPaneText (0, lpszString, TRUE);
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == TM_AUTO_SAVE_TIMER)
	{
		StartAutoSavePeriod();
		AutoSave();
	}
	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::StartAutoSavePeriod()
{
	KillTimer(TM_AUTO_SAVE_TIMER);
	if(GetMyApp()->GetIntSetting(SETTING_NAME(AUTOSAVE)))
		m_uiAutoSaveTimer = SetTimer(TM_AUTO_SAVE_TIMER, 
				GetMyApp()->GetIntSetting(SETTING_NAME(AUTOSAVE_PERIOD)), NULL);
}

void CMainFrame::AutoSave()
{
	if(GetMyApp()->GetIntSetting(SETTING_NAME(AUTOSAVE)))
	{
		CWaitCursor __wait__;
		CString strMsg((LPTSTR)IDS_AUTOSAVE_BEGIN);
		WriteStatus(strMsg);

		CString str = m_strProjDir;
		str += _T('\\');
		str += m_strTempCopyFileName;

		m_ptrGameDisp->save_script(str);

		strMsg.LoadString(IDS_AUTOSAVE_END);
		WriteStatus(strMsg);
	}
}

void CMainFrame::MakeTempCopyFileName()
{
	int iPos = m_strFileName.Find(_T(".qml"));
	if(iPos != -1)
	{
		do {
			m_strTempCopyFileName = m_strFileName.Left(iPos);
			m_strTempCopyFileName += _T("_copy_");
			CTime time ( CTime::GetCurrentTime());
			m_strTempCopyFileName += time.Format(_T("%H_%M_%d_%m_%Y"));
		} while(PathFileExists(m_strTempCopyFileName));
	}
	m_strTempCopyFileName += _T(".qml");
}

bool CMainFrame::OnCompleteCreateZone(qdGridZone * ptrZone)
{
	return m_wndSTView->AddGridZone2Tree(ptrZone);
}

bool CMainFrame::OnSaveGZChanges(bool bSave)
{
	return m_wndSTView->OnSaveGZChanges(bSave);
}

void CMainFrame::OnAppSettings()
{
	AppStngsDlg dlg;
	dlg.AddUser(this);
	dlg.AddUser(m_wndView.get());
	dlg.AddUser(m_wndTE.get());
	dlg.DoModal();
}


void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
}

void CMainFrame::OnViewSceneToolbar()
{
	ShowControlBar(&m_wndSceneToolBar,!m_wndSceneToolBar.IsVisible(), FALSE); 
}

void CMainFrame::OnViewTeToolbar()
{
	ShowControlBar(&m_wndTEToolBar,!m_wndTEToolBar.IsVisible(), FALSE); 
}

void CMainFrame::OnUpdateViewTeToolbar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndTEToolBar.IsVisible());
}

void CMainFrame::OnUpdateViewSceneToolbar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndSceneToolBar.IsVisible());
}

void CMainFrame::OnViewToolbar()
{
	ShowControlBar(&m_wndToolBar,!m_wndToolBar.IsVisible(), FALSE); 
}

void CMainFrame::OnUpdateViewToolbar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndToolBar.IsVisible());
}

void CMainFrame::OnViewTrigDbgBar()
{
	if (m_wndTrigProf->IsVisible())
		m_wndTrigProf->Show(ITriggerProfList::TPL_HIDE);
	else
		m_wndTrigProf->Show(ITriggerProfList::TPL_SHOW);
}

void CMainFrame::OnUpdateViewTrigDbgBar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndTrigProf->IsVisible());
}

void CMainFrame::OnScaleMgr()
{
	if (m_wndScaleMgr->IsVisible())
		m_wndScaleMgr->Hide();
	else
		m_wndScaleMgr->Show();
}

void CMainFrame::OnUpdateScaleMgr(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndScaleMgr->IsVisible());
}

bool CMainFrame::SafeSaveScript(qdGameDispatcher* pdisp, LPCTSTR lpszFileName)
{
	CString str(lpszFileName);
	str += _T(".temp_save");
#ifndef _DEBUG
	try{
#endif // _DEBUG
		if (pdisp->save_script(str))
		{
			DeleteFile(lpszFileName);
			MoveFile(str, lpszFileName);
			return true;
		}
#ifndef _DEBUG
	}
	catch (...) {
	}
#endif   //_DEBUG
	return false;
}

void CMainFrame::OnLastNail()
{
	CString tmp;
	if (g_bProjChanged)
	{
		tmp.LoadString(IDS_LAST_NAIL_PROJ_NOT_SAVED);
		if (MessageBox(tmp, NULL, MB_YESNO) == IDYES)
			if (!SafeSaveScript(m_ptrGameDisp.get(), m_strFileName))
				return;
	}

	tmp.LoadString(IDC_LAST_NAIL_EXE);
	CString app_path = disk_op::GetPathFromFullFileName(disk_op::GetModulePath());
	app_path += tmp;

	CString comand_line = _T("\"");
	comand_line += app_path;
	comand_line += _T("\"");

	comand_line += _T(" \"");
	comand_line += CurDirSafer::GetCurrentDirectory();
	comand_line += _T("\\");
	comand_line += m_strFileName;
	comand_line += _T("\"");

	if (!m_ptrLastNail) 
		m_ptrLastNail.reset(new ProcessLauncher);

	if (!m_ptrLastNail->createProcess(app_path, comand_line.GetBuffer()))
	{
		tmp.Format(IDS_CNT_LOAD_LAST_NAIL_EXE, static_cast<LPCTSTR>(app_path));
		app_path.LoadString(IDS_ERR_MSG_CAPTION);
		MessageBox(tmp, app_path, MB_OK|MB_ICONSTOP);
	}
}

void CMainFrame::OnUpdateLastNail(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_ptrGameDisp.get() != NULL
		&&(!m_ptrLastNail||!m_ptrLastNail->isStillAlive()));
}

boost::shared_ptr<AppSettingsBasePage> CMainFrame::getPage()
{
	boost::shared_ptr<GenSettingsPP> ptrPage(new GenSettingsPP);
	ptrPage->AddListener(this);
	return ptrPage;
}
void CMainFrame::SettingsChanged()
{
	BOOL bAutoSave = GetMyApp()->GetIntSetting(SETTING_NAME(AUTOSAVE));
	if (bAutoSave)
		StartAutoSavePeriod();
}

void CMainFrame::OnViewObjectlist()
{
	if (objectListMngr_->isVisible())
		objectListMngr_->hide();
	else
		objectListMngr_->show();
}

void CMainFrame::OnUpdateViewObjectlist(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(objectListMngr_->isVisible());
}
