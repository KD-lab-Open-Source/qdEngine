// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__DA0B862A_5909_4B6D_A6EA_A386F280D6D6__INCLUDED_)
#define AFX_MAINFRM_H__DA0B862A_5909_4B6D_A6EA_A386F280D6D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SplitterClient.h"

#include <memory>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "coordanimdlg.h"

#include "IAppSettingsChangeListener.h"
#include "IAppSettingsUser.h"

const int TE_TOOLBAR_ID = 1;
const int SCENE_TOOLBAR_ID = 2;
const int MAINFRAME_TOOLBAR_ID = 3;
const int SCALE_TOOLBAR_ID = 4;

class StateCoordsStacker;
class SceneTreeView;
class EditStateDlg;
class CChildView;
class TEWnd;
class TriggerDbgDlg;

class bFont;
class GDI_grDispatcher;

class ProcessLauncher;

class qdGameObjectStateMask;
class qdGameObjectState;
class qdGameDispatcher;
class qdNamedObject;
class qdGameObject;
class qdGameScene;
class qdGridZone;
class TriggerProfList;
class ScaleMgr;
class ObjectListMngr;


class ds_sndDispatcher;

const UINT TM_AUTO_SAVE_TIMER		= 1;
const DWORD AUTO_SAVE_TIMER_PERIOD	= 120000;

/// Класс управляет главным окном приложения в старой версии. Сейчас не используется
class CMainFrame : public CFrameWnd
	, public IAppSettingsUser
	, public IAppSettingsChangeListener
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)
		
		// Attributes
public:
	
	// Operations
public:

	afx_msg void OnSaveScrt();
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL
	
	static bool SafeSaveScript(qdGameDispatcher* pdisp, LPCTSTR lpszFileName);
	// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual boost::shared_ptr<AppSettingsBasePage> getPage();
	virtual void SettingsChanged();

	qdGameDispatcher* get_game_disp(){
		return m_ptrGameDisp.get();
	}

	void ShowObjectInTree(qdNamedObject* ptrObj);
	void ShowProperties(qdNamedObject* ptrObj);
	void DeleteObject(qdNamedObject* ptrObj);

	//перерисовывает окно, в котором отабражена сцена
	void InvalidateSceneView(bool bRedrawScene);
	
	//назначает выбранный объект в окне, в котором отабражена сцена
//	void SelectObjectInSceneView(qdGameObject* ptrObj);

	//выравниваем размер битмапа по границам сцены
	bool AdjustBitmapSize();
		
	CString GetCurrScriptFileName();
	CString GetCurrProjDir() const{
		return m_strProjDir;
	}

	//очищаем экран и все вспомогательные переменные.
	//это необходимо при перезагрузке сцен скирптов
	void ResetSceneView();

	//сохраняет скрипт, елси он был изменен
	//возвращает фэлс если при запросе выбрали отмену
	bool SaveAndClose(bool bAsk);

	//открывает скрипт-файл 
	bool OpenScript(const CString &strFileName);

	bool OnCreateMask(qdGameObject* ptrGO);
	bool OnEndStateMaskCreate(qdGameObjectStateMask* ptrStateMask);

	bool OnSceneSelect(qdGameScene* ptrScene, bool bSel);
	//когда выбираем узел "Сетка" в дереве
	bool OnNetSelect(bool bSelect);
	bool OnGridZoneSelect(qdGridZone* ptrZone, bool bSelect);
	bool OnMaskStateSelect(qdGameObjectStateMask* ptrState, bool bSelect);

	bool OnCreateNewZone();
	bool OnCompleteCreateZone(qdGridZone* ptrZone);
	bool OnSaveGZChanges(bool bSave);

	void WriteStatus(LPCTSTR lpszString);

	//зупускает, если можно, таймер для автосейва
	void StartAutoSavePeriod();

	void ShowSceneView();
	void ShowTEView();

	const CString GetProjIniFileName() const;
	// Generated message map functions
protected:  // control bar embedded members
	CStatusBar		m_wndStatusBar;
	CToolBar		m_wndToolBar;
	CToolBar		m_wndSceneToolBar;
	CToolBar		m_wndTEToolBar;
	CToolBar		m_wndViewStateToolBar;
	CDialogBar		m_wndDBar;

	std::auto_ptr<CChildView>		m_wndView;
	std::auto_ptr<TEWnd>			m_wndTE;
	std::auto_ptr<TriggerProfList> m_wndTrigProf;
	std::auto_ptr<ScaleMgr>			m_wndScaleMgr;
	std::auto_ptr<ds_sndDispatcher> m_ptrDSSoundDisp;
	
	SceneTreeView	*m_wndSTView;
	CSplitterClient	*m_ptrSplitterClient;
	
	std::auto_ptr<qdGameDispatcher> m_ptrGameDisp;
	std::auto_ptr<GDI_grDispatcher> m_ptrGRD;

	//имя открытого файла  
	CString m_strFileName;
	//имя файла временной копии
	CString m_strTempCopyFileName;
	CString m_strProjDir;
	
	CSplitterWnd m_wndSplitter;

	UINT m_uiAutoSaveTimer;

	boost::scoped_ptr<ProcessLauncher>	m_ptrLastNail;
	boost::scoped_ptr<ObjectListMngr>	objectListMngr_;
private:
	void init_graphics(void);
	bool CreateDefResource();
	void reset();
	
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnOpenScrt();
	afx_msg void OnSaveAsScrt();
	afx_msg void OnCrtProj();
	afx_msg void OnUpdateSaveAsScrt(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveScrt(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnUpdateSelScene(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditGaits(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditTriggers(CCmdUI* pCmdUI);
	afx_msg void On100();
	afx_msg void On200();
	afx_msg void On25();
	afx_msg void On300();
	afx_msg void On50();
	afx_msg void OnSelMastab();
	afx_msg void OnUpdate100(CCmdUI* pCmdUI);
	afx_msg void OnUpdate200(CCmdUI* pCmdUI);
	afx_msg void OnUpdate25(CCmdUI* pCmdUI);
	afx_msg void OnUpdate300(CCmdUI* pCmdUI);
	afx_msg void OnUpdate50(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelMastab(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAppSettings();
	afx_msg void OnDestroy();
	afx_msg void OnViewSceneToolbar();
	afx_msg void OnViewTeToolbar();
	afx_msg void OnUpdateViewTeToolbar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewSceneToolbar(CCmdUI *pCmdUI);
	afx_msg void OnViewToolbar();
	afx_msg void OnUpdateViewToolbar(CCmdUI *pCmdUI);
	afx_msg void OnViewTrigDbgBar();
	afx_msg void OnUpdateViewTrigDbgBar(CCmdUI *pCmdUI);
	afx_msg void OnLastNail();
	afx_msg void OnUpdateLastNail(CCmdUI* pCmdUI);
	afx_msg void OnScaleMgr();
	afx_msg void OnUpdateScaleMgr(CCmdUI *pCmdUI);
	afx_msg void OnViewObjectlist();
	afx_msg void OnUpdateViewObjectlist(CCmdUI *pCmdUI);
	//}}AFX_MSG
	afx_msg BOOL OnNeedText(UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	DECLARE_MESSAGE_MAP();

	void SetCaption(const CString str);
	void SaveProj();
	void AutoSave();
	void SaveProjIni();
	void ReadProjIni();

	void MakeTempCopyFileName();

	void DeleteTmpCopy();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MAINFRM_H__DA0B862A_5909_4B6D_A6EA_A386F280D6D6__INCLUDED_)
