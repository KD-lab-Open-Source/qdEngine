// Main\MainFrame.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "QuestEditor.h"
#include ".\mainframe.h"
#include "StructureView.h"
#include "ChildView.h"
#include "TEWnd.h"
#include "SplitterClient.h"
#include "Project.h"
#include "Object List/ObjectListMngr.h"
#include "MiniMapWnd.h"

#include "CurDirSafer.h"

#include "TriggerProfList.h"
#include "ScaleMgr.h"

#include <Mmreg.h>

#include "gdi_gr_dispatcher.h"
#include "ds_snd_dispatcher.h"
#include "qd_game_dispatcher.h"
#include "app_core.h"

#include "AppStngsDlg.h"

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
		ID_INDICATOR_CAPS,
		ID_INDICATOR_NUM,
		ID_INDICATOR_SCRL,
};

const int TE_TOOLBAR_ID = 1;
const int SCENE_TOOLBAR_ID = 2;
const int MAINFRAME_TOOLBAR_ID = 3;
const int SCALE_TOOLBAR_ID = 4;

// MainFrame

IMPLEMENT_DYNCREATE(MainFrame, CFrameWnd)

MainFrame::MainFrame()
: splitterClient_(NULL)
{
}

MainFrame::~MainFrame()
{
}

ISceneViewFacade* MainFrame::getSceneView() const{
	return sceneView_.get();
}
ITriggerView*	MainFrame::getTriggerEditorView() const{
	return teView_.get();
}
StructureView*	MainFrame::getStructureView() const{
	return structureView_;
}

IScaleMgr*		MainFrame::getScaleMngr() const{
	return m_wndScaleMgr.get();
}

IObjectList*	MainFrame::getObjectList() const{
	return objectListMngr_.get();
}

BEGIN_MESSAGE_MAP(MainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(IDC_SCALE_MGR, OnScaleMgr)
	ON_UPDATE_COMMAND_UI(IDC_SCALE_MGR, OnUpdateScaleMgr)
	ON_COMMAND(ID_VIEW_OBJECTLIST, OnViewObjectlist)
	ON_COMMAND(ID_VIEW_SCENE_TOOLBAR, OnViewSceneToolbar)
	ON_COMMAND(ID_VIEW_TE_TOOLBAR, OnViewTeToolbar)
	ON_COMMAND(ID_VIEW_TRIG_DBG_BAR, OnViewTrigDbgBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OBJECTLIST, OnUpdateViewObjectlist)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCENE_TOOLBAR, OnUpdateViewSceneToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TE_TOOLBAR, OnUpdateViewTeToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRIG_DBG_BAR, OnUpdateViewTrigDbgBar)
//	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
//	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateViewStatusBar)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)
//	ON_COMMAND(IDC_APP_SETTINGS, OnAppSettings)
ON_WM_CLOSE()
END_MESSAGE_MAP()


// MainFrame message handlers

BOOL MainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
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
	if (!m_wndSplitterLeft.CreateStatic( 
		&m_wndSplitter, // our parent window is the first splitter 
		2, 1, // the new splitter is 2 rows, 1 column 
		WS_CHILD | WS_VISIBLE | WS_BORDER, // style, WS_BORDER is needed 
		m_wndSplitter.IdFromRowCol(0, 0) 
		// new splitter is in the first row, first column of first splitter 
		))
	{
		TRACE0("Failed to create vertical splitter bar");
		return FALSE;    // failed to create
	}
	
	m_wndSplitterLeft.CreateView(1, 0, RUNTIME_CLASS(StructureView),CSize(0, 0),
		&Context);
	m_wndSplitterLeft.CreateView(0, 0, RUNTIME_CLASS(MiniMapWnd),CSize(0, 0),
		&Context);

	CRect rect;
	GetWindowRect( &rect );

	m_wndSplitterLeft.SetRowInfo(0, rect.Height()/3, 30);
	m_wndSplitter.SetColumnInfo(0, rect.Width()/4, 30);
	//		m_wndSplitter.SetColumnInfo(1, rect.Width()/3, 30);
	return TRUE;
}

int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	splitterClient_ = (CSplitterClient*)m_wndSplitter.GetPane(0, 1);

	teView_.reset(new TEWnd);
	if (!teView_->Create(NULL, NULL, WS_CHILD | WS_BORDER,
		CRect(0, 0, 0, 0), splitterClient_, 0, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	teView_->setSplitterClient(splitterClient_);

	sceneView_.reset(new CChildView);

	if (!sceneView_->Create(NULL, NULL, WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN,
		CRect(0, 0, 0, 0), splitterClient_, 0, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	sceneView_->setSplitterClient(splitterClient_);
	sceneView_->activate();

	structureView_ = reinterpret_cast<StructureView*>(m_wndSplitterLeft.GetPane(1, 0));

	MiniMapWnd* mini_map = reinterpret_cast<MiniMapWnd*>(m_wndSplitterLeft.GetPane(0, 0));
	if (mini_map)
		mini_map->setTEWnd(teView_.get());
	teView_->setMiniMap(mini_map);

	graphDispatcher_.reset(new GDI_grDispatcher);
	init_graphics();
	appSetHandle(sceneView_->m_hWnd);
	soundDisp_.reset(new ds_sndDispatcher);


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

	if (!m_wndTrigProf.get()||!m_wndTrigProf->Create(this, teView_.get(), WS_CHILD |
		CBRS_FLYBY | CBRS_RIGHT | 
		CBRS_SIZE_DYNAMIC))
	{
		TRACE0("Failed to create m_wndTrigDbgDlgBar \n");
		return -1;
	}
	teView_->SetTriggerProfiler(m_wndTrigProf.get());

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

/*
	m_wndSTView->SetObjectList(objectListMngr_.get());
	m_wndSTView->SetScaleMgr(m_wndScaleMgr.get());
	m_wndView->SetScaleMgr(m_wndScaleMgr.get());
	static_cast<IScalable*>(m_wndTE.get())->SetScaleMgr(m_wndScaleMgr.get());
	m_wndScaleMgr->SetScalable(m_wndView.get());
*/
	sceneView_->SetScaleMgr(m_wndScaleMgr.get());
	static_cast<IScalable*>(teView_.get())->SetScaleMgr(m_wndScaleMgr.get());
	m_wndScaleMgr->SetScalable(sceneView_.get());

//	m_wndScaleMgr->Show();
//	objectListMngr_->show();
	ShowControlBar(&m_wndSceneToolBar, TRUE, FALSE);
	ShowControlBar(&m_wndTEToolBar, TRUE, FALSE);
	LoadBarState(CString((LPTSTR)IDS_REG_SEC_NAME_TOOLBARS));

	return 0;
}

void MainFrame::init_graphics(void)
{
	grDispatcher::instance()->Finit();

	grDispatcher::instance()->init(800, 600,
		GR_ARGB1555,static_cast<void*>(sceneView_->m_hWnd));
	grDispatcher::instance()-> SetClip();
	grDispatcher::instance()-> SetClipMode(1);

	grDispatcher::instance()-> Fill(0);
}

BOOL MainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, 
						 AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (GetActiveDocument()
		&&GetActiveDocument()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// let the view have first crack at the command
	if (sceneView_&&sceneView_->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	if (teView_&&teView_->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
		return TRUE;
	}

	if (m_wndScaleMgr.get()
		&&m_wndScaleMgr->GetView()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void MainFrame::OnScaleMgr()
{
	if (m_wndScaleMgr->IsVisible())
		m_wndScaleMgr->Hide();
	else
		m_wndScaleMgr->Show();
}

void MainFrame::OnUpdateScaleMgr(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndScaleMgr->IsVisible());
}

void MainFrame::OnViewObjectlist()
{
	if (objectListMngr_->isVisible())
		objectListMngr_->hide();
	else
		objectListMngr_->show();
}

void MainFrame::OnViewSceneToolbar()
{
	ShowControlBar(&m_wndSceneToolBar,!m_wndSceneToolBar.IsVisible(), FALSE); 
}

void MainFrame::OnViewTeToolbar()
{
	ShowControlBar(&m_wndTEToolBar,!m_wndTEToolBar.IsVisible(), FALSE); 
}

void MainFrame::OnViewTrigDbgBar()
{
	if (m_wndTrigProf->IsVisible())
		m_wndTrigProf->Show(ITriggerProfList::TPL_HIDE);
	else
		m_wndTrigProf->Show(ITriggerProfList::TPL_SHOW);
}

void MainFrame::OnUpdateViewObjectlist(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(objectListMngr_->isVisible());
}

void MainFrame::OnUpdateViewSceneToolbar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndSceneToolBar.IsVisible());
}

void MainFrame::OnUpdateViewTeToolbar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndTEToolBar.IsVisible());
}

void MainFrame::OnUpdateViewTrigDbgBar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndTrigProf->IsVisible());
}

void MainFrame::OnViewStatusBar()
{
	// TODO: Add your command handler code here
}

void MainFrame::OnUpdateViewStatusBar(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

void MainFrame::OnViewToolbar()
{
	ShowControlBar(&m_wndToolBar,!m_wndToolBar.IsVisible(), FALSE); 
}

void MainFrame::OnUpdateViewToolbar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndToolBar.IsVisible());
}

BOOL MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_MAIN_ICON);
	cs.lpszClass = AfxRegisterWndClass(0, 0, 0, hIcon);
	return TRUE;
}

void MainFrame::OnClose()
{
	SaveBarState(CString((LPTSTR)IDS_REG_SEC_NAME_TOOLBARS));

	CFrameWnd::OnClose();
}
