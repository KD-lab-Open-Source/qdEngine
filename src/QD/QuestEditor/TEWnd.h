#if !defined(AFX_TEWND_H__EB730C5F_B2BE_4843_BD88_74E30FA472A1__INCLUDED_)
#define AFX_TEWND_H__EB730C5F_B2BE_4843_BD88_74E30FA472A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TEWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TEWnd window
#include <afxole.h>
#include "auto_object.h"
#include "itriggerview.h"
#include "ScaleInterfaces.h"
#include <list>
#include <utility>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include "IAppSettingsChangeListener.h"
#include "IAppSettingsUser.h"
#include <Additional/gdi_resource_guard.h>

#include "SelectionManager.h"

class qdNamedObject;
class qdTriggerLink;


class TEBaseWorkMode;
class TEBaseWorkModeDrawer;
class TEAllChainWorkMode;
class TEAllChainWorkModeDrawer;
class TEProfilerWorkMode;
class TEStatusColoredEleDrawer;

class qdGameDispatcher;
class MiniMapWnd;

interface ITEWorkMode;

interface ITriggerProfList;

class qdTriggerElement;
typedef boost::intrusive_ptr<qdTriggerElement> qdTriggerElementPtr;
typedef boost::intrusive_ptr<qdTriggerElement const> qdTriggerElementConstPtr;

typedef std::vector<qdTriggerElementPtr> qdTriggerElementList;
typedef std::vector<qdTriggerLink> qdTriggerLinkList;

class CSplitterClient;
/************************************************************************/
/* Constants                                                            */
/************************************************************************/
#include "te_consts.h"

class TEWnd : public CWnd, public ITriggerView
			, public IScalable
			, public IAppSettingsUser
			, public IAppSettingsChangeListener
{
	//Private typedefs

	class TEDropTarget;
	friend class TEDropTarget;

	typedef std::auto_ptr<qdTriggerElement> qdTriggerElementAPtr_t;
	typedef std::vector<qdTriggerElementPtr> TgrEleVect_t;
	typedef std::vector<qdTriggerChain*> TgrChainVect_t;
	typedef std::map<std::string, CRect> ChainToRect;

	typedef struct tagBRUSH_INDX_ENTRY : public std::pair<int, HBRUSH> {
		typedef std::pair<int, HBRUSH> _Base;
		tagBRUSH_INDX_ENTRY():_Base(0, NULL){}
		tagBRUSH_INDX_ENTRY(int key, COLORREF clr):_Base(key, CreateSolidBrush(clr)){}
		bool operator == (int type) const {
			return first == type;
		}
	} BRUSH_INDEX_ENTRY;

	typedef std::vector<BRUSH_INDEX_ENTRY> BrushIdx_t;

	enum SCALE_ACTION{SA_MINUS, SA_PLUS, SA_RESET};

	// Construction
public:
	TEWnd();
	//Scroll Area Place
	enum {
		SAP_LEFT	= 1,
		SAP_RIGHT	= 2,
		SAP_TOP		= 4,
		SAP_BOTTOM	= 16,
	};

	void setGameDisp(qdGameDispatcher* ptrDisp);
	void setSplitterClient(CSplitterClient* client);
	CSplitterClient* getSplitterClient() const;
	void setMiniMap(MiniMapWnd* mm) { miniMap_ = mm; }
	const MiniMapWnd* miniMap() const { return miniMap_; }
// Attributes
public:

	void reset();
	void SetTriggerProfiler(ITriggerProfList* ptrITPList);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TEWnd)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~TEWnd();

	void ScrollView(int dx, int dy);
	bool ExtendWorkArea();
	void EnsureVisibleRect(const CRect& r);
	void UpdateScrollers();
	bool TestForBetweenPlots(CPoint p);
	//проверка делается, когда пытаемся зацепить один елемент за другой
	bool CanPlugLink(qdTriggerElementPtr ptrTestEle);
	//интерфейс ITriggerView
	const qdTriggerChain* ConstShowedTrigger() const;
	qdTriggerChain* ShowedTrigger();
	CString const GetLastEditedTrigger();
	BOOL ShowLTOfSelected(const te_filter_base& f);
	BOOL MergeTriggers(qdTriggerChain const* from,qdTriggerChain* to);
	BOOL IsVisible();
	void EnsureVisible(qdTriggerElementPtr const*eles, int count);

	void RemakeCellIdx(qdTriggerElementPtr pe, const CPoint& p);
	//обновляет прямоугольник цепочки после изменения положения элемента
	void RemakeChainRect(qdTriggerChain* ptrChain);
	//обновляем рабочую область цепочки.
	//т.е. область по которой выравниваются рабочая область окна,
	//когда показывается именно эта цепочка
	void UpdateChainWorkArea(qdTriggerChain* ptrChain);
	//определяем находится ли точка у краев окна
	DWORD PtInScrollArea(const CPoint& p);

	int GetNetCellWidth() const{
		return m_iNetCellWidth;
	}
	int GetNetCellHeight() const{
		return m_iNetCellHeight;
	}
	//расстояние по горизонтали между ячейками
	int GetNetAmongCellW() const{
		return m_iNetAmongCellW;
	}
	//расстояние по вертикали между ячейками
	int GetNetAmongCellH() const{
		return m_iNetAmongCellH;
	}

	CSize const GetNetCellDims() const{
		return CSize(m_iNetCellWidth, m_iNetCellHeight);
	}
	CSize const GetNetAmongCellDims() const{
		return CSize(m_iNetAmongCellW, m_iNetAmongCellH);
	}
	HDC getDC() const{
		return m_hDC;
	}

	//сдвигает только gen_layout
	void OffsetChain(qdTriggerChain* ptrChain, const CPoint& new_lt);
	//определяет рабочую область редактора. 
	//По р.о. выравниваются скроллеры
	void UpdateWndWorkArea();

	BOOL Redraw();
	// Generated message map functions
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, 
		AFX_CMDHANDLERINFO* pHandlerInfo);
protected:
	//{{AFX_MSG(TEWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnUpdateTbtnAllStates2Editor(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	BOOL OnTTNGetDispInfo(UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	afx_msg void OnRunTriggerTest();
	afx_msg void OnUpdateTbtnRunTriggerTest(CCmdUI* pCmdUI);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnProfilerOnOff();
	afx_msg void OnUpdateProfilerOnOff(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTbtnClearStarts(CCmdUI *pCmdUI);
	afx_msg void OnDeactivateAllTriggers();
	afx_msg void OnUpdateDeactivateAllTriggers(CCmdUI *pCmdUI);
	afx_msg void OnActiveToBeginForAllTrgrs();
	afx_msg void OnUpdateActiveToBeginForAllTrgrs(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCommnadUI(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP();
	afx_msg void OnDestroy();

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
protected:
	/*
	 *	ITriggerView
	 */
	BOOL AddState(CPoint& offset, qdGameObjectState*const* ptrState, int count);
	BOOL SetViewState(TVIEW_STATE estate);
	BOOL ShowTriggers(class qdTriggerChain** ptrTCs, int count);
	BOOL SelectByFilter(const te_filter_base& f, 
								eSelectAction sa, eSelectMode mode);
	void ShowTrigger(qdNamedObject const* object);
	void clear_active(qdTriggerChain* ptrChain);

	void SetScaleMgr(IScaleMgr* psmgr);
	void IncreaseScale();
	void DecreaseScale();
	float CurrentScale();
	void SetScale(float fscale);
	void UpdateScaleInfo();

	virtual void SaveIniSettings(IProjectSettings* pps);
	virtual void ReadIniSettings(IProjectSettings* pps);
	virtual void OnEndIniReading();
	//! Вызывается до изменения
	virtual bool OnVisibilityChanging(eVCAction action);
	//! Вызывается после изменения
	virtual void OnVisibilityChange(eVCAction action);

	boost::shared_ptr<AppSettingsBasePage> getPage();
	void SettingsChanged();
	void SaveVisibleRect();
	void activate();
	void deactivate();
	void clearUndobuffer();
private://Methods

	BOOL OnDropData(void* pData, const CPoint& point);
	//возвращает истину, если в текущий момент можно добавить елемент в цепочку
	bool CanDrop(const CPoint& p, void* pData);
	
	/*
	 *	прорисовка
	 */
	void PaintRect(const RECT* lpRc);
		
	/*
	 *	не пришей к пизде рукав или пятая нога
	 */
	void TrackPopupMenu(DWORD id);
	void ModifyMenu(CMenu* pmenu, DWORD id);

	//сдвигаем общее положение у всех цепочек
	void OffsetChainsGL(const CSize& sz);
	//сдвигает всю цепочку вместе с прямоугольниками элементов,
	//но не трогает общее положение(gen_layout)
	void OffsetChain(qdTriggerChain* ptrChain, const CSize &sz);
	bool RemakeEleRect(const POINT&v, CRect& rcE);
	void RemakeElesAndChainRect(qdTriggerChain* ptrChain);

	//определяет номера ячеек для цепочек триггеров
	void CheckLayout();

	int get_wa_x_add(){
		return ((m_iNetCellWidth<<1) + (m_iNetAmongCellW<<1));
	}
	int get_wa_y_add(){
		return ((m_iNetCellHeight<<1) + (m_iNetAmongCellH<<1));
	}

	CSize GetLinkOffset(const CPoint& pFrom, 
							const CPoint& ptTo, const RECT& bound);

	void CalcLinkOffsets(qdTriggerElementPtr parent, qdTriggerElementPtr child,
						CPoint ptFrom, CPoint ptTo);
/************************************************************************/
/* Вспомогательные для создания элемента                                */
/************************************************************************/		


	//! вычисляем какую сцену запустить для данного элемента
	const CString get_scene_name(qdGameDispatcher* gameDisp,
								qdTriggerElementConstPtr ptrEle, 
									qdTriggerChain* ptrChain);

public:
	CRect GetRootNodeRect(qdTriggerElementPtr ptrRootEle, const RECT& rcFirstEle);

	std::string make_te_title(const qdNamedObject* ptrObj);
	const RECT make_te_rect(const CPoint& p, const std::string& title);

	qdTriggerElementPtr find_te(const CPoint& p);
	qdTriggerLink* find_link(const CPoint &p, qdTriggerElementPtr* ppe);
	qdTriggerChain* find_chain(const CPoint& p);

	qdTriggerChain* WillIntersect(const CRect&r, qdTriggerChain* exclude);
	//перед сдвигом элемента проверяет нет ли на этом месте кого-нибудь
	void OffsetIfEmpty(qdTriggerElementPtr pele, int offsetx, int offsety);
	void setToolTipText(CString const& str);
	CString getToolTipText() const{
		return m_strToolTipText;
	}
	void trackToolTip(CPoint const& pos);

	SelectionManager& getSelectionManager();

	CPoint viewportOrg() const { return m_ptViewportOrg; }
	void setViewportOrg(CPoint pnt) { m_ptViewportOrg = pnt; }
private:
/*
 *	всевозможный поиск
 */
	qdTriggerElementPtr find_zone_ele(const qdNamedObject* obj, int status);
	qdTriggerLink* find_link_in_eles(const CPoint& p, 
							const qdTriggerElementList& lst, 
							qdTriggerElementPtr* ppe);

	qdTriggerLink* find_link_in_ele(const CPoint& p, qdTriggerElementPtr pele);

	bool InTrigger(qdNamedObject* pobj, qdTriggerChain* pc);

	qdTriggerChain* WillIntrsBefMe(const CRect&r, qdTriggerChain* me);

		
	//создаёт truetype фонт для отрисовки заголовков
	void CrtFont();
	HWND CreateTT();
		
	/*
	 *	скроллинг и масштабирование
	 */
	//возвращаем самый левый и верхний угол прямоугольника, охватывающего
	//все цепочки 
	CSize GetGLLeftTop();
	void SetupViewportOrg();
	void SetScrollRanges();
	void SetScaleRanges();
	void OffsetViewportOrg(int dx, int dy);		
	void SetScale(const SCALE_ACTION action);
	void SetScale(float fOldScale, float fNewScale);
	void InitViewportOrg();
	void InitScale();

	void ExtendWAToLeft(int ex);
	void ExtendWAToRight(int x);
	void ExtendWAToTop(int ex);
	void ExtendWAToBottom(int ex);

	//выставляет масштаб таким,чтобы охватывающий прямоугльник
	//цепочки полностью попадал на экран
	void EnsureVisibleBR(qdTriggerChain* ptrChain);

	bool SetHorzScroller();
	bool SetVertScroller();
	BOOL OnScroll(UINT nScrollCode);
	
public:
	void log2scr(CSize* psz) const{
		POINT p0 = {0, 0};
		POINT p1 = {psz->cx, psz->cy};
		log2scr(&p0, 1);
		log2scr(&p1, 1);
		psz->cx = p1.x - p0.x;
		psz->cy = p1.y - p0.y;
	}
	
	void log2scr(POINT* p) const{
		scale_point(p);
		p->x += m_ptViewportOrg.x;
		p->y += m_ptViewportOrg.y;
	}
	//перевод координат
	void log2scr(POINT* p, int cnt) const{
		for(int i = cnt; --i>=0;++p)
			log2scr(p);
	}

	void scr2log(POINT* p, int cnt) const{
		for(int i = cnt; --i>=0;++p)
			scr2log(p);
	}


	void log2scr(RECT* pr) const{
		log2scr(reinterpret_cast<POINT*>(pr), 2);
	}
	
	void scr2log(RECT* pr) const{
		scr2log(reinterpret_cast<POINT*>(pr), 2);
	}
	void scr2log(POINT* p) const{
		p->x -= m_ptViewportOrg.x;
		p->y -= m_ptViewportOrg.y;
		descale_point(p);
	}
	//преобразуем точку в верхний левый угол ячейки
	void to_cell_lt(POINT* p);
	int scale_x(const int x) const{
		return round(x*m_fScale);
	}
	int scale_y(const int y) const{
		return round(y*m_fScale);
	}

	int descale_x(const int x) const{
		return round(x/m_fScale);
	}
	int descale_y(const int y) const{
		return round(y/m_fScale);
	}

private:	

	void scale_point(POINT*p) const{
		p->x = scale_x(p->x);
		p->y = scale_y(p->y);
	}
	void scale_point(POINT*p, int cnt) const{
		for(int i = cnt; --i>=0;)
			scale_point(p++);
	}
	
	void descale_point(POINT*p) const{
		p->x = descale_x(p->x);
		p->y = descale_y(p->y);
	}
	void descale_point(POINT*p, int cnt) const{
		for(int i = cnt; --i>=0;)
			descale_point(p++);
	}
	CRect GetVisibleRect();
	void SaveChainVisibleRect(LPCTSTR chainName);
	void ReadChainVisibleRect(LPCTSTR chainName);

	void setCurrendMode(ITEWorkMode* pwm);
private://Members

	std::auto_ptr<TEDropTarget> m_ptrDT;
	
	//ширина ячейки
	int m_iNetCellWidth;
	//высота ячейки
	int m_iNetCellHeight;
	//расстояние по горизонтали между ячейками
	int m_iNetAmongCellW;
	//расстояние по вертикали между ячейками
	int m_iNetAmongCellH;

	HWND m_hToolTipWnd;
	CRect m_rcWorkArea;

	gdi_resource_guard<HFONT> m_hFont;
	HDC m_hDC;

	float m_fScale;
	/*
	 *	пределы скролирования
	 */
	int m_iMaxScrollLimY;
	int m_iMinScrollLimY;
	int m_iMaxScrollLimX;
	int m_iMinScrollLimX;

	float m_fMinScale, m_fMaxScale;

	CPoint m_ptViewportOrg;

	bool m_bShowAll;
	TgrChainVect_t m_vecDrwList;

	enum eAreaExtending{AE_NONE, AE_LEFT, AE_TOP, AE_RIGHT, AE_BOTTOM};
	eAreaExtending m_eAreaExtending;
	IScaleMgr		 *m_ptrIScaleMgr;
	ITEWorkMode								*m_ptrCurrendWorkMode;
	std::auto_ptr<TEBaseWorkMode>			m_ptrBaseWorkMode;
	std::auto_ptr<TEBaseWorkModeDrawer>		m_ptrBaseWorkModeDrawer;
	std::auto_ptr<TEAllChainWorkMode>		m_ptrAllChainWorkMode;
	std::auto_ptr<TEAllChainWorkModeDrawer> m_ptrAllChainWorkModeDrawer;
	std::auto_ptr<TEProfilerWorkMode>		m_ptrProfilerWorkMode;
	std::auto_ptr<TEStatusColoredEleDrawer> m_ptrProfilerWorkModeDrawer;

	CString m_strToolTipText;
	qdGameDispatcher* m_ptrGameDisp;
	//! Сохранен ли порямоугольник
	bool m_bVisibleRect4ChainSaved;
	//! по имени цепочки сохраняем видимый прямоугольник
	ChainToRect chainToRect_;
	//! Имя цепочки, с которой работаем в текущий момент
	//! Страхует нас от случаев, когда цепочка удалена, а мы используем имя этой цеопчки
	//! Если отображаются все цепочки, то не задано
	CString activeChainName_;
	CString lastEditedChainName_;
	//! Управление выбранными элементами
	SelectionManager selectionManager_;

	CSplitterClient* splitterClient_;
	//! Для возможности перерисовки миникарты при изменении редактора триггеров
	MiniMapWnd* miniMap_;
private://classes
class TEDropTarget : public COleDropTarget
{
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pData, 
											DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, 
											DWORD dwKeyState, CPoint point );
	virtual DROPEFFECT OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, 
											DWORD dwKeyState, CPoint point );
	virtual DROPEFFECT OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);
};
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_TEWND_H__EB730C5F_B2BE_4843_BD88_74E30FA472A1__INCLUDED_)
