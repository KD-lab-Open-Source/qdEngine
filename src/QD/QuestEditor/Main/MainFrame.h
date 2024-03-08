#pragma once

#include <boost/scoped_ptr.hpp>

// MainFrame frame
class CSplitterClient;
interface ISceneViewFacade;
interface ITriggerView;
interface IObjectList;

class ObjectListMngr;
class ScaleMgr;
class TriggerProfList;

interface IScaleMgr;

class MainFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(MainFrame)
protected:
	void init_graphics(void);
public:
	MainFrame();           // protected constructor used by dynamic creation
	virtual ~MainFrame();

	ISceneViewFacade*		getSceneView() const;
	ITriggerView*			getTriggerEditorView() const;
	class StructureView*	getStructureView() const;
	IScaleMgr*				getScaleMngr() const;
	IObjectList*			getObjectList() const;
protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, 
						AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnScaleMgr();
	afx_msg void OnUpdateScaleMgr(CCmdUI *pCmdUI);
	afx_msg void OnViewObjectlist();
	afx_msg void OnViewSceneToolbar();
	afx_msg void OnViewTeToolbar();
	afx_msg void OnViewTrigDbgBar();
	afx_msg void OnUpdateViewObjectlist(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewSceneToolbar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewTeToolbar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewTrigDbgBar(CCmdUI *pCmdUI);
	afx_msg void OnViewStatusBar();
	afx_msg void OnUpdateViewStatusBar(CCmdUI *pCmdUI);
	afx_msg void OnViewToolbar();
	afx_msg void OnUpdateViewToolbar(CCmdUI *pCmdUI);
	afx_msg void OnCrtProj();
	afx_msg void OnMakeDirStruct();
	afx_msg void OnClose();
private:
	CSplitterWnd	m_wndSplitter;
	CSplitterWnd	m_wndSplitterLeft;
	CSplitterClient *splitterClient_;

	boost::scoped_ptr<class CChildView>			sceneView_;
	boost::scoped_ptr<class TEWnd>				teView_;
	boost::scoped_ptr<class TriggerProfList>	triggerProfilerList_;
	boost::scoped_ptr<class ScaleMgr>			scaleMgr_;
	boost::scoped_ptr<class ds_sndDispatcher>	soundDisp_;
	boost::scoped_ptr<class GDI_grDispatcher>	graphDispatcher_;

	class StructureView	*structureView_;

	CStatusBar		m_wndStatusBar;
	CToolBar		m_wndToolBar;
	CToolBar		m_wndSceneToolBar;
	CToolBar		m_wndTEToolBar;
	CToolBar		m_wndViewStateToolBar;
	CDialogBar		m_wndDBar;

	boost::scoped_ptr<TriggerProfList>		m_wndTrigProf;
	boost::scoped_ptr<ScaleMgr>				m_wndScaleMgr;
	boost::scoped_ptr<ObjectListMngr>	objectListMngr_;
};


