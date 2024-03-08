// ChildView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__EB7776B2_F342_45C9_AE83_47B2E7D6A18C__INCLUDED_)
#define AFX_CHILDVIEW_H__EB7776B2_F342_45C9_AE83_47B2E7D6A18C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CChildView window
class CoordAnimDlg;
class StateCoordsStacker;
class UndoBufferMgr;
class ubentry;
class ubeObjPos;
class CSplitterClient;

class qdInterfaceElement;


#include <memory>

#include <vector>

#include <afxmt.h>

#include "qd_game_object_animated.h"

#include "helpers.h"
#include "ScaleInterfaces.h"
#include <boost/shared_ptr.hpp>

#include "IAppSettingsUser.h"
#include "IAppSettingsChangeListener.h"

#include "ISceneViewFacade.h"
#include "Main/IIdleProcessor.h"

#include "state_guard.h"

#include "sv_api.h"
#include "qdengine_frwrds.h"

typedef std::vector<qdNamedObject*> NamedObjectVec_t;
typedef std::vector<POINT> POINTVect_t;

class CMainFrame;
//! ��������� �������� ������ ��������� �����
interface ISceneViewMode;
interface IProjectFacade;
//! ������ ����� ��� ����� ��
const DWORD		CA_POINT_WIDTH				= 12;
//! ������ ����� ��� ����� ��
const DWORD		CA_POINT_HEIGHT				= CA_POINT_WIDTH;

const DWORD		CA_POINT_OUT_WIDTH				= 14;
const DWORD		CA_POINT_OUT_HEIGHT				= CA_POINT_OUT_WIDTH;

//! ���� ����� ��� ��
const COLORREF	CA_POINT_COLOR				= RGB(0, 0, 0);
//! ���� ����� ����� ��� ��
const COLORREF	CA_POINT_FRAME_COLOR		= RGB(255, 255, 255);
//! ���� �������� ����� ��
const COLORREF	CA_CUR_POINT_COLOR			= RGB(0, 0, 200);
//! ���� �����, ���� � ��� ����� ����� ������
const COLORREF	CA_IMPASSABLE_POINT_COLOR	= RGB(255, 0, 0);

//! ���� �����, ����������� ����� ��
const COLORREF	CA_LINE_COLOR		= RGB(0, 200, 0);

//! ������ ����� ��� ������� ����� �����
DWORD		const SM_POINT_WIDTH		= CA_POINT_WIDTH;
//! ������ ����� ��� ������� ����� �����
DWORD		const SM_POINT_HEIGHT		= CA_POINT_HEIGHT;
//! ���� ����� ��� ������� ����� �����
COLORREF	const SM_POINT_COLOR		= CA_POINT_COLOR;
//! ���� ����� ����� ��� ������� ����� �����
const COLORREF	SM_POINT_FRAME_COLOR		= CA_POINT_FRAME_COLOR;
//! ���� ����� ��� ������� ������� ������ �����
COLORREF	const SM_CUR_POINT_COLOR	= CA_CUR_POINT_COLOR;
//! ���� �����, ����������� ������� ����� �����
COLORREF	const SM_LINE_COLOR		= CA_LINE_COLOR;

//! ������ ����� ��� ������� ����� ����
const DWORD		GZ_POINT_WIDTH		= CA_POINT_WIDTH;
//! ������ ����� ��� ������� ����� ����
const DWORD		GZ_POINT_HEIGHT		= CA_POINT_HEIGHT;
//! ���� ����� ��� ������� ����� ����
const COLORREF	GZ_POINT_COLOR		= CA_POINT_COLOR;
//! ���� ����� ��� ������� ������� ������ ����
const COLORREF	GZ_CUR_POINT_COLOR	= CA_CUR_POINT_COLOR;
//! ���� �����, ����������� ������� ����� ����
const COLORREF	GZ_LINE_COLOR		= CA_LINE_COLOR;
//! ���� �����, ����������� ������� ����� ������������� ����
const COLORREF	GZ_EDITED_ZONE_LINE_COLOR = RGB(0, 0, 0);

//! ������� �����, ������������ ������� �������
const DWORD WORK_AREA_BOUND_THICKNESS = 1;

class CChildView : public CWnd
					, public IScalable
					, public IAppSettingsUser
					, public IAppSettingsChangeListener
					, public ISceneViewFacade
					, public IIdleProcessor
{
	//! ��������� �� ����� ����������. ��� �������� �����
	CMainFrame* m_ptrMainFrame;
	//! ����, � ������� ���� ��� �������� ������ ���
	CSplitterClient* splitterClient_;

	//! ��������� ������� ������
	qdGameObject		*m_ptrSelObj;
	//! \brief ���������, ������� ������� � ������ �������.
	//! ������������� ��� ��������� VS_EDIT_SCENE
	qdGameObjectState*	m_ptrActiveState;

	//! �������������, ����������� ��������� ������. ��� ����� �������
	CRect				m_SelObjRect;
	//! ���� �� ����������� CChildView::m_SelObjRect
	bool				m_bSelRectChanged;
	//! ����� �������
	CString				m_strToolTip;

	//! ����� ���������������� �����
	int					m_iDraggedAnchor;
	//! �������������, ������� �������������� ��� ������ � ������� ��������
	CRect				m_rDraggedGameArea;
	//! DC ����
	HDC					m_ptrViewDC;
	
	//!	������� ������� � ����
	CRect				m_rcVisibleArea;
	//! ����� �� ������������� ����� ��� ������ �������-����
	bool				m_bCanDragScene;

	//! ������� ������
	HCURSOR				m_hCurrCursor;
	//! ������ ����
	HCURSOR				m_hHandCur;
	//! ���������� ����
	HCURSOR				m_hDragHandCur;

	//! ��� �������������� ��������
	HCURSOR				m_hPenCursor;
	//! ��� �������������� ���������� ������������ ��������
	HCURSOR				m_hFootCursor;
	//! ��� �������������� �����
	HCURSOR				m_hCrossCursor;
	//! ����������, ������������� ������
	HCURSOR				m_hPrevCursor;
	//! �����, �� ������� ������������� ��� �������� ��������	
	CPoint				m_base_point;
	//! ����� ���������� �������
	CPoint				m_tt_point;
	//! ������������, � ������� ��������� ����� ���-����
	CRect				m_tt_rect;
	
	//! ���������(����� ������) ��������� �����
	VIEW_STATE			m_eViewState;
	//! ��� ������ � ������� ������ ������
	STATE_SUBTOOLS_ENUM	m_eSubTools;

	//! ������������� ����
	qdGridZone			*m_ptrGZEdited;

	//! ���������� ����� ������ 
	/*!
		������������, ����� �������� ����� �������������� �����,
		����� ������������ ���������� ���������
	*/
	VIEW_STATE			m_eOldViewState;

	//! ��������� �� �������� ��������
	IScaleMgr*			m_ptrIScaleMgr;

	//! ������� �������
	float				m_fScale;
	//! ������ ������� ���. �.�. � ����� ����� ���� ������ ������� ����� ���� ���
	CPoint				m_pointBmpViewOrg;
	//! ������� ������������� �� �
	int					m_iMaxScrollLimX, m_iMinScrollLimX;
	//! ������� �������������� �� �
	int					m_iMaxScrollLimY, m_iMinScrollLimY;
	
	//! ��������� �� ������ �������������� ��
	CoordAnimDlg		*m_ptrCADlg;
	//! ��������� �� ������ ��������
	StateCoordsStacker	*m_ptrStateStacker;
	//! ��������� �� ������ �������������� ��������
	class StateBoundDlg	*m_ptrStateBoundDlg;
	//! �� ��� ��������������
	/*!
	 	� �� ���������� �������� ��. � ����� �������������� ��� ����������
		������ ��������.
	*/
	qdCoordsAnimation	m_refCAnim;
	//! ����� �� �� ������ ����� � ��.
	bool				m_bCACanDrag;

	//! �������������� ������ � ����������
	CCriticalSection	m_csDrawGuard;

	//! ��������� ������ ��� �������� �����
	/*!
	 	� �� ���������� �������� �����. � ����� �������������� ��� ����������
		������ ��������.
	*/
	qdGameObjectStateMask	m_qdStateMask;
	//! �������� �����
	qdGameObjectStateMask	*m_ptrSourceMask;
	//! ������������ ������ ��� �����.(������, � �������� ��������� �����)
	qdGameObject			*m_ptrSMParent;

	//! �������� ���������
	class qdInventory		*m_ptrActiveInventory;
	//! ������������� ��������� ������ �����
	CRect					m_ActiveCellSetRect;
	//! ��������������� ����, ����������� � ��������
	/*!
	 	����� ��� �������� ������������ ���������������
	*/
	CRect					m_rcHelper;
	//! ������� ������� �����
	/*!
	 	������������ ��� �������������� ��, �����, ��� �� �����
	*/
	int						m_iActivePoint;
	//! �������	�� ������������� ������
	bool					m_bConturClosed;
	//! ������� �� ��������� � ������� ����������.
	/*!
	 	������������ ��� �������� ����� � ���.
	*/
	bool					m_bWorkChanged;
	//! �������� �� �������� �� ����� ��� ���� ��������
	bool					m_bDrawNetProjs;
	//! �������� �� ������� ��� ���� ��������
	bool					m_bDrawBounds;	

	//! ������, � �������� ������������� ������� � �����������
	qdGameObjectAnimated* m_ptrEditedAnimated;
	//! ������� ������� CChildView::m_ptrEditedAnimated
	Vect3f m_pers_bounds;

	//! ��������� ���������
	/*!
		��� ���������, ������� ��������� � ������. ��� �� ����� �� ������
	*/
	qdGameObjectState* m_ptrStackedState;

	//! ������������� ���������.
	/*!
	 	������������ ��� ����������� ������� ���������
	*/
	qdGameObjectState* m_ptrEditedState;

	//! ������������ ������������. ���������� ��� ��������.
	CRect m_rectHilited;


	//! ��������� �� undo ������
	std::auto_ptr<UndoBufferMgr> m_ptrUndo;
	//! ������ undo ������� ��� ������� �������
	std::auto_ptr<ubeObjPos> m_ptrUbeObjPos;

	//! ������������� �������� ������ ����������
	DWORD m_dwMainThreadID;

	//! ������ ������, ������������� ����� �������������� ����������
	std::auto_ptr<ISceneViewMode> m_ptrIntefEditMode;
	//! ������ ������,  ������������� ����� �������������� ����� ��������
	std::auto_ptr<ISceneViewMode> m_ptrEditDialogsFrameMode;

	//! ������� ����� 
	ISceneViewMode* m_ptrCurrentSceneViewMode;

	//! ������� ������
	IProjectFacade* project_;

	//! ������, ��� �������� �����������
	state_guard				playedObject_;
	//! ���������, ��� �������� �������������
	qdGameObjectState*		playedState_;
	//! ���� �� ���������� ����������� ��������
	BOOL					stopAnimation_;

	UINT showVideoTimer_;

	//! ���� ����� ���������� � ���, ��� ��������� � ���� ����� ���������
	IWorkOwner* workOwner_;
	// Construction
public:
	CChildView();

	void setProject(IProjectFacade* project);
	IProjectFacade* getProject() const;
	// Attributes
public:

// Operations
public:
	DECLARE_DYNCREATE(CChildView);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildView();


	//! ������������� �������� ��������
	void SetScaleMgr(IScaleMgr* psmgr);
	//! ��������� �������
	void IncreaseScale();
	//! ��������� �������
	void DecreaseScale();
	//! ������� �������
	float CurrentScale();
	//! ��������� ������� �������
	void SetScale(float fscale);

	//! ������������� �������� CChildView::m_bDrawNetProjs
	void SetDrawNetProjs(bool bval);
	//! ���������� �������� CChildView::m_bDrawNetProjs
	bool GetDrawNetProjs() const;
	//! ������������� �������� CChildView::m_bDrawBounds
	void SetDrawBounds(bool bval);
	//! ���������� �������� CChildView::m_bDrawBounds
	bool GetDrawBounds() const;

	//! ���������� �������� �������� ��� ��������� ����� 
	virtual boost::shared_ptr<AppSettingsBasePage> getPage();
	//! ���� ������� �������� � ����������
	virtual void SettingsChanged();

//#ifndef USE_NEW_PROJ
	//! ��������� �����
	void set_main_frame(CMainFrame* pmf){m_ptrMainFrame = pmf;}
//#endif // USE_NEW_PROJ
	void setSplitterClient(CSplitterClient* client);
	CSplitterClient* getSplitterClient() const;

	qdGameDispatcher* getGameDispatcher() const;
	//������ ������ �� �������� �����
	qdCamera*	 get_camera();

	//! �������� ��������� ����
	void OnCreateGridZone();
	//! �������� ������������� ����
	void OnEditGridZone(qdGridZone* ptrZone, IWorkOwner* workOwner);
	//! �� ����������� �������������� �������� ����
	void OnAbortGridZoneEdit(bool bSave);

	//! �� ������ �������������� �������� ���������
	void OnEditStateDims(class StateBoundDlg* pdlg, qdGameObjectState* pstate);
	//! �� ����������� ������������� ��������
	void OnEndEditStateDims(bool bSave);

	//! ������������� ���������(������� �����) ��������� �����
	void SetViewState(VIEW_STATE eVal);
	//! ���������� ������� �����
	VIEW_STATE GetViewState(){return m_eViewState;}
	//! ������������� ��������� ��� �������� ���������(������ ������)
	void SetSubTools(STATE_SUBTOOLS_ENUM ssteVal);
	//! ���������� ��������� ��� ������ ������
	STATE_SUBTOOLS_ENUM GetSubTools(){return m_eSubTools;}

	//! ������������ �������� ��� CChildView::m_hCursor
	void SetCursor(HCURSOR hCur);

	//! �� ������ �������� �����
	bool OnCreateStateMask(qdGameObject* ptrGO);
	//! �� ������ �������������� �����
	bool OnEditStateMask(qdGameObjectStateMask* ptrMask);
	//! �� ��������� �������������� �����
	void OnAbortStateMaskEdit(bool bSave);

	//! �� ������ �������������� ����������� � ��������
	bool OnEditAnimated(qdGameObjectAnimated* ptrPers);
	//! �� ��������� �������������� ����������� � ��������
	void OnAbortAnimatedEdit(bool bSave);
	
	//! ���������� ���������� ������� � ��������� ���������
	void reset();
	//! �������������� �����
	void RedrawScene(qdGameScene* pscene);

	//! ����������� ����������� ����
//#ifdef USE_NEW_PROJ
	void Redraw(eRedrawMode emode);
//#endif 
//	void Redraw(sv_api::eRedrawMode emode);

	//! �������� ������ � ��������� �����
	void SelectObj(qdGameObject* ptrObj);
	//! ������������� �������� ���������
	void SetActiveState(qdGameObjectState* pstate);
	//! ������������� �������� ���������
	void SelectInventory(qdInventory* ptrInv);

	//! ��������� ��������� ����������
	void UpdateScrollers();
	//! ����������� ������ ��� ���, ����� � ���� ������ ������� ����� � ��������
	bool AdjustBitmapSize();

	//! ������������� �������� ��� CChildView::m_ptrCADlg
	void SetCoordsAnimationDlg(CoordAnimDlg* ptrDlg){
		m_ptrCADlg = ptrDlg;
	}

	//! ������������� �������� ��� CChildView::m_ptrStateStacker
	void SetStateStackerDlg(StateCoordsStacker* ptrDlg){
		m_ptrStateStacker = ptrDlg;
	}
	//! ���������� ������ �� ������������� ��
	qdCoordsAnimation& GetCoordsAnimation(){
		return m_refCAnim;
	}

	//! ��������� ���������� ��� �� ��������������, ���������������� r, � ����
	void PaintRect(const RECT &r);

	//! ���������� ���������� ����������
	CCriticalSection* GetDrawGuard(){
		return &m_csDrawGuard;
	}

	//! \brief ��������� ���� �� ����� � ��������� ������� ���������
	//! �������, ���� ������ ����� ������
	void CACheckPointsCount();
	//! ��������� ������������� �������� � ��������� �����
	void UpdateCoordsAnimation();

	//! �� ������ ������������� ��
	void BeginEditCoordAnim(qdCoordsAnimation* ptrCA);
	//! �� ��������� �������������� ��
	void EndEditCoordAnim();

	//! ������������� �������� ��� CChildView::m_ptrStackedState
	void SetStackedState(qdGameObjectState* ptrState){
		m_ptrStackedState = ptrState;
	}

	//! ��������� �������� undo
	void Undo(ubentry* pube);

	//! ���������� ���������� ������� �������
	const Vect2s GetLogCurPos();
	//! ���������� ���������� ������� �������
	const CPoint& GetLogCurPos(CPoint& point);
	//! ���������� ������� ������� � ������� �����������
	const CPoint GetCurPos();

	virtual BOOL OnIdle(LONG lCount);

	virtual void activate();
	virtual void deactivate();
	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBtnHand();
	afx_msg void OnUpdateBtnHand(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEditUndo(UINT nID);
	afx_msg BOOL OnEditRedo(UINT nID);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void OnDraw(CDC* pDC);
private:
	BOOL getStopAnimation() const;
	void setStopAnimation(BOOL flag);

	virtual void PlayStopVideo(qdVideo* video);
	virtual void StopVideo(qdVideo* video);
	virtual bool isVideoPlaying() const;

	virtual void StartAnimation(qdGameObjectState* state);
	virtual void StopAnimation(qdGameObjectState* state);
	virtual void PauseAnimation();

	void ShowAnimation();

	//! ��������� ���������� � ������� �������� � ��������� ��������
	void UpdateScaleInfo();

	//! ���������� ������
	bool DrawToolTip();
	//! ������ ������ ����� ������ �����
	void DrawWorkAreaBound();
	//! ������������ ������� ������� 
	void DrawVisibleArea();
	//! �������� ��������� � ����� ������� ���� �����
	void DrawInventory();
	//! ������������ ������� ������� ��� ���������
	void DrawVisibleAreaForInventory();
	//! ������� ������ ��������� ������ �����
	void DrawCellsetBounds(qdInventory* ptrInv);
	//! ������������ ����� ��������� ������ ����� 
	void DrawActiveCellsetBound(qdInventoryCellSet const& set, 
		CPoint const& SceneLeftTop);
	//! ������������ ����� ������ ����� 
	void DrawCellsetBound(const qdInventoryCellSet& set, const CPoint& SceneLeftTop);

	//! �������� ���������
	void DrawInterface();
	//! �������� ������������� ���������
	void DrawIngameInterface();

	//! ������������ ��������� ������
	bool DrawSelObject();

	//! ���������������� ��������� ����� ������������ ��������
	void DrawCoordsAnimPts(const qdCoordsAnimation& ca, bool bClosed, 
										qdCamera* ptrCam, COLORREF line_clr);
	//! ��������� ����� ������������ ��������
	void DrawCoordAnimationPoints();
	//! ������������ �����
	void DrawMask();

	//! ������������ ������ ���� �� �����
	void DrawGridZoneContour();

	//! ������ ������ ����, ������ ������ �������� � ����
	void DrawGridZoneContour2(const qdGridZone* ptrZone, COLORREF line_clr);

	//! ������ ������ ���� �� ������ �������
	void DrawGridZoneContour(const grid_contour_t& gc, bool bClosed, 
									int iActivePoint, bool bDrwPoints, 
									COLORREF line_clr = GZ_LINE_COLOR);
	//! ������������ ������� ���� ���
	void DrawGridZoneContours();

	//! ���������� �������� �� ����� ���������� �������
	bool DrawNetProj(qdGameObject const* ptrObj, int inner_color = 0x000000FF);
	//! ������ �������� ��� �����, ��� ����� �������� �� �����
	void DrawNetProjs(const qdGameScene* pscene);
	//! ������ ������� ��� �����, ��� ����� �������
	void DrawBounds(const qdGameScene* pscene);

	//! ��������������� ��� CChildView::DrawStackedState()
	void drw_obj(qdGameObjectAnimated* ptrAnimObj, 
				 qdGameObjectState* ptrState,
				 qdCoordsAnimation* pca,
				 qdAnimationInfo*ptrAnimInfo, 
				 float fAnimPhase, 
				 int dir_num,
				 int offs_type,
				 COLORREF cntr_clr);
	//! ������ ��������� ��������� ��������
	void DrawStackedState();

	//! ������ ������������ �������������
	void HiliteRect(const CRect& rect);

	//! ������������� ������� ������� 
	bool MoveObjectTo(qdGameObject* ptrObj, Vect3f pos);

	//������������ ������� �������(Visible Area (VA))
	//��� ������� �������� ���������� ����� �����, ������� 
	//� ���� ��� �������� ����������

	//! ��������� ���� �� ������������� �������� ������� �������
	CSize VATest(qdGameObjectMoving* ppers);
	//! �������� ������� �������, �������� ������� ������� �������(�������)
	bool VAMove(CSize sz);

	//! ������� ������ �� �����
	qdGameObject* GetObjectFromPoint(const CPoint& p);

	//���������� �������� �����
	inline qdGameScene* get_active_scene();

	//! ������������ �����
	bool drag_point(qdGameObject const* pobj, Vect3f &pos, 
					CPoint const& point, UINT nFlags, qdCamera const* ptrCam);
	//������ ������ �� �������� �����
//	inline qdCamera*	 get_camera();
	inline qdCamera*	 get_camera(qdGameScene* pscene);

	void clear_sel(qdCamera* ptrCam);

	void NTOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void NTOnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void NTOnLButtonDown(UINT nFlags, CPoint point);
	void NTOnLButtonUp(UINT nFlags, CPoint point);
	void NTOnLButtonDblClk(UINT nFlags, CPoint point);
	void NTOnRButtonUp(UINT nFlags, const CPoint& point);
	void NTOnMouseMove(UINT nFlags, const CPoint& point);
	BOOL NTOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	
	//������� ���������� ��� ������ ��������/�������������� �����
	void SMOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void SMOnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void SMOnLButtonDown(UINT nFlags, const CPoint& point);
	void SMOnLButtonUp(UINT nFlags, const CPoint& point);
	void SMOnLButtonDblClk(UINT nFlags, const CPoint& point);
	void SMOnRButtonUp(UINT nFlags, const CPoint& point);
	void SMOnMouseMove(UINT nFlags, const CPoint &point);
	BOOL SMOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	
	void SMSaveWork();
	void SMKillWork();
	void SMAbortEdit(bool bSave);
	std::auto_ptr<std::vector<Vect2s> > SMPts2Scr(const std::vector<Vect2s>& mc);
		
	//������� �� ����� ���� ��� ��������
	void GAFindAnchor(const CPoint& point);
	void GADrawDragRect();
	
	bool SetHorzScroller();
	bool SetVertScroller();

	qdCoordsAnimationPoint CAV2s2CAPnt(const Vect2s& vPos);
	CRect CAGetFlushRect(const qdCoordsAnimation& ca, qdCamera* ptrCam,bool bClosed);

	int  CAFindPoint(const Vect2s& vPos);
	void CAAddPoint(const Vect2s& vPos);
	bool CAInsertPoint(const Vect2s& vPos);
	bool CABeginEditPoint(const Vect2s& vPos);
	void CADelPoint(const Vect2s& vPos);
	void CADelPoint(int iPoint);


	void CAOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void CAOnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL CAOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	void CAOnLButtonDown(UINT nFlags, const CPoint &point);
	void CAOnLButtonUp(UINT nFlags, const CPoint &point);
	void CAOnLButtonDblClk(UINT nFlags, const CPoint &point);
	void CAOnRButtonUp(UINT nFlags, const CPoint& point);
	void CAOnMouseMove(UINT nFlags, const CPoint& point);
	
	void scr2log(POINT *point, int count);
	void log2scr(POINT *point, int count);

	void scr2log(CRect* r);
	void log2scr(CRect* r);

	void scale_pts(POINT *point, int count);
	void descale_pts(POINT *point, int count);

	void scale_pts(CRect* r);
	void descale_pts(CRect* r);
	//�������� ������ ������� �������� � ������������,
	//����� ��� �� �������� �� ������������ �������
	void moveBmpOrg(int dx, int dy);

	//�������� ����� moveBmpOrg � ��������������
	void scrollbmp(int dx, int dy);

	void SetScrollRanges();

	//game araa
	void GAOnMouseMove(UINT nFlags, CPoint point);
	BOOL GAOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	void GAOnLButtonDown(UINT nFlags, const CPoint& point);
	void GAOnLButtonUp(UINT nFlags, const CPoint& point);

	//edit scene
	void ESOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void ESOnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void ESOnLButtonDown(UINT nFlags, const CPoint &point);
	void ESOnLButtonUp(UINT nFlags, const CPoint &point);
	void ESOnLButtonDblClk(UINT nFlags, const CPoint &point);
	void ESOnRButtonUp(UINT nFlags, const CPoint& point);
	void ESOnMouseMove(UINT nFlags, const CPoint& point);
	BOOL ESOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);


	qdGridZone* GZCreate();
	void GZSaveWork();
	void GZKillWork();
	bool GZSelectAll(bool bSelect);
	//��� ������������� ������. �������� ������� ���� � ������
	void GZAbortEdit(bool bSave);

	//���� ���� �������� �� ������,
	//��������� ��������� � ����� �������
	qdGridZone* GZFindZoneByCntrPnt(const Vect2s&vPoint, 
									const qdGridZone* ptrExcludeZone);

	//���� ���� �������� �� ������
	qdGridZone* GZFindZone(const Vect3f& vPoint, const qdGridZone* ptrExcludeZone);
	//���� ������� ����� ����
	int GZFindPoint(const Vect2s& vPoint, qdGridZone*ptrZone);
	//��������� ������� �����.
	//���������� � ���������� �����
	int GZAddPoint(const Vect2s& vPoint);
	//��������� �����, ���� �������� �� �����.
	//true - ���� ��������
	bool GZInsertPoint(const Vect3f& planePoint);

	//��������� �����, ���� �������� �� �����.
	//true - ���� ��������
	bool SMInsertPoint(const Vect2s& mask_pos);
	bool SMAddPoint(const Vect2s& mask_pos);
	
	//����� ����� �� ������� �����
	int SMFindPoint(const Vect2s& point);

	//��������� ���������� �� �������� � �������� � �������
	inline const Vect2s SMScr2Mask(const Vect2s& point);
	inline const Vect2s SMMask2Scr(const Vect2s& point);

	inline void SMScr2Mask(Vect2s& point);
	inline void SMMask2Scr(Vect2s& point);

	bool SMCreateByteMask();

	void CalcSelRect();
	inline bool IsMouseOverSelected(const CPoint& scr_pt);
/************************************************************************/
/* UNDO                                                                 */
/************************************************************************/
	void undoObjPos(ubeObjPos* pe);
/************************************************************************/
/* �������������� ������� � ����������� �������������� �������          */
/************************************************************************/
	void EAOnAbortEdit(bool bSave);
	void EASaveWork();
	void EAKillWork();

	void EAOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void EAOnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void EAOnLButtonDown(UINT nFlags, const CPoint &point);
	void EAOnLButtonUp(UINT nFlags, const CPoint &point);
	void EAOnLButtonDblClk(UINT nFlags, const CPoint &point);
	void EAOnRButtonUp(UINT nFlags, const CPoint& point);
	void EAOnMouseMove(UINT nFlags, const CPoint& point);
	BOOL EAOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* �������������� ������� ��� ���������                                 */
/************************************************************************/

	void ESTOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void ESTOnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void ESTOnLButtonDown(UINT nFlags, const CPoint &point);
	void ESTOnLButtonUp(UINT nFlags, const CPoint &point);
	void ESTOnLButtonDblClk(UINT nFlags, const CPoint &point);
	void ESTOnRButtonUp(UINT nFlags, const CPoint& point);
	void ESTOnMouseMove(UINT nFlags, const CPoint& point);
	BOOL ESTOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* �������� ���������							                        */
/************************************************************************/
	void SSOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void SSOnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void SSOnLButtonDown(UINT nFlags, const CPoint &point);
	void SSOnLButtonUp(UINT nFlags, const CPoint &point);
	void SSOnLButtonDblClk(UINT nFlags, const CPoint &point);
	void SSOnRButtonUp(UINT nFlags, const CPoint& point);
	void SSOnMouseMove(UINT nFlags, const CPoint& point);
	BOOL SSOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	void SSOnMouseMoveEditPos(UINT nFlags, const CPoint& point);
	void SSOnMouseMoveEditOffset(UINT nFlags, const CPoint& point);
	void SSOnMouseMoveEditBound(UINT nFlags,const CPoint& point);
		
	//���������������� ��������� ��������� ������� ��� ��������������
	void SSRedrawStackedState(qdGameObjectState* ptrState, int dx, int dy, const Vect3f& vecOldPos);
	
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*  �������������� ��������� ((E)dit (I)nevtory)                        */
/************************************************************************/
	void EIOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void EIOnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void EIOnLButtonDown(UINT nFlags, const CPoint &point);
	void EIOnLButtonUp(UINT nFlags, const CPoint &point);
	void EIOnLButtonDblClk(UINT nFlags, const CPoint &point);
	void EIOnRButtonUp(UINT nFlags, const CPoint& point);
	void EIOnMouseMove(UINT nFlags, const CPoint& point);
	BOOL EIOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);


	CPoint GetCellsetLT(const class qdInventoryCellSet& set);
	void log2scr_inv(POINT* p, int count);
	void scr2log_inv(POINT* p, int count);

	int EIFindAnchor(const CPoint& scr_point, const CRect &set_rc);
	void EIApplyAnchorChangeToRect(const CPoint &dt, RECT* lprc);
	const CPoint EICalcOffset(const CPoint& dt, const CRect& rcDragged, const CRect &bound);
	//�� �������� �������������� ��������, �� ��� ����� ���������� �� ������,
	//��������� � ������ �����
	void EIFromHelperToVisible(const CRect &bound);
	enum eStickBound {STB_LEFT, STB_RIGHT, STB_TOP, STB_BOTTOM};
	bool EIFindStickBound(const CPoint& scene_lt, const CRect& rcSrc, 
		eStickBound bound, int* pstick_coord);
//////////////////////////////////////////////////////////////////////////
	
	//GameObject Hit Test
	class go_hit_test{
		std::auto_ptr<NamedObjectVec_t> m_hit_res;
		CPoint m_tst_pnt;
	public:
		go_hit_test(const CPoint& p):m_tst_pnt(p){
			m_hit_res = std::auto_ptr<NamedObjectVec_t>(new NamedObjectVec_t);
			m_hit_res->reserve(10);
		}
		go_hit_test(go_hit_test& ght):m_tst_pnt(ght.m_tst_pnt){
			m_hit_res = ght.m_hit_res;}
		go_hit_test& operator= (go_hit_test& ght){
			m_hit_res = ght.m_hit_res;
			m_tst_pnt = ght.m_tst_pnt;
			return *this;
		}
		
		template<class _Obj>
			void operator()(_Obj* pno){
			if(pno->hit(m_tst_pnt.x, m_tst_pnt.y))
				m_hit_res->push_back(pno);
		}
		
		const NamedObjectVec_t* get_hit_res(){
			return m_hit_res.get();
		}
		bool empty(){return m_hit_res->empty();}
	};
	
	class Vect2s_point_ht{
	public:
		const Vect2s& m_vPoint;
		unsigned m_width;
		Vect2s_point_ht(const Vect2s& vPnt, int _w):m_vPoint(vPnt), m_width(_w){}
		bool operator()(const Vect2s& vVertex) const{
			Vect2s __t  = vVertex;
			__t -= m_vPoint;
			if(__t.norm() <= m_width)
				return true;
			return false;
		}
	};

	class pers_dir_safer{
		float m_dir;
		qdGameObjectMoving* m_ptrMoving;
	public:
		inline pers_dir_safer(qdGameObjectAnimated* ptrAnim);
		inline ~pers_dir_safer();
	};
	
	//���������� min � max �� � � � ��� ������� Vect3f
	template<class _MinType, class _VectType>
		class min_max{
		_MinType x_min, x_max;
		_MinType y_min, y_max;
	public:
		min_max(_MinType _xmin, _MinType _xmax, _MinType _ymin, _MinType _ymax)
			:x_min(_xmin), x_max(_xmax), y_min(_ymin), y_max(_ymax){}
		
		min_max(_MinType min_ini, _MinType max_ini)
			:x_min(min_ini), x_max(max_ini), y_min(min_ini), y_max(max_ini){}
		
		void operator()(const _VectType& v){
			if(x_min > v.x) x_min = v.x;
			if(x_max < v.x) x_max = v.x;
			if(y_min > v.y) y_min = v.y;
			if(y_max < v.y) y_max = v.y;
		}
		_MinType xmin(){return x_min;};
		_MinType ymin(){return y_min;};
		_MinType xmax(){return x_max;};
		_MinType ymax(){return y_max;};
	};
	
	class draw_contours{
		CChildView* m_ptrView;
		qdGridZone* m_ptrSel;
	public:
		draw_contours(CChildView* ptrView, qdGridZone*ptrSel):m_ptrView(ptrView), m_ptrSel(ptrSel){}
		inline void operator()(const qdGridZone* ptrZone)const;
	};
	friend draw_contours;

	friend bool GZFillCells(qdGridZone* ptrZone, qdCamera* ptrCam);
};

extern CChildView* g_ptrSceneView;
inline CChildView* GetSceneView(){
	return g_ptrSceneView;
}
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_CHILDVIEW_H__EB7776B2_F342_45C9_AE83_47B2E7D6A18C__INCLUDED_)
