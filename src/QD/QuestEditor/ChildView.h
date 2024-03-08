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
//! Интерфейс рабочего режима редактора сцены
interface ISceneViewMode;
interface IProjectFacade;
//! Ширина якоря для узлов КА
const DWORD		CA_POINT_WIDTH				= 12;
//! Ширина якоря для узлов КА
const DWORD		CA_POINT_HEIGHT				= CA_POINT_WIDTH;

const DWORD		CA_POINT_OUT_WIDTH				= 14;
const DWORD		CA_POINT_OUT_HEIGHT				= CA_POINT_OUT_WIDTH;

//! Цвет якоря для КА
const COLORREF	CA_POINT_COLOR				= RGB(0, 0, 0);
//! Цвет рамки якоря для КА
const COLORREF	CA_POINT_FRAME_COLOR		= RGB(255, 255, 255);
//! Цвет текущего якора КА
const COLORREF	CA_CUR_POINT_COLOR			= RGB(0, 0, 200);
//! Цвет якоря, если в эту точку дойти нельзя
const COLORREF	CA_IMPASSABLE_POINT_COLOR	= RGB(255, 0, 0);

//! Цвет линни, соединяющей точки КА
const COLORREF	CA_LINE_COLOR		= RGB(0, 200, 0);

//! Ширина якоря для опорных точек маски
DWORD		const SM_POINT_WIDTH		= CA_POINT_WIDTH;
//! Высота якоря для опорных точек маски
DWORD		const SM_POINT_HEIGHT		= CA_POINT_HEIGHT;
//! Цвет якоря для опорных точек маски
COLORREF	const SM_POINT_COLOR		= CA_POINT_COLOR;
//! Цвет рамки якоря для опорных точек маски
const COLORREF	SM_POINT_FRAME_COLOR		= CA_POINT_FRAME_COLOR;
//! Цвет якоря для текущей опорной точеки маски
COLORREF	const SM_CUR_POINT_COLOR	= CA_CUR_POINT_COLOR;
//! Цвет линии, соединяющей опорные точки маски
COLORREF	const SM_LINE_COLOR		= CA_LINE_COLOR;

//! Ширина якоря для опорных точек зоны
const DWORD		GZ_POINT_WIDTH		= CA_POINT_WIDTH;
//! Высота якоря для опорных точек зоны
const DWORD		GZ_POINT_HEIGHT		= CA_POINT_HEIGHT;
//! Цвет якоря для опорных точек зоны
const COLORREF	GZ_POINT_COLOR		= CA_POINT_COLOR;
//! Цвет якоря для текущей опорной точеки зоны
const COLORREF	GZ_CUR_POINT_COLOR	= CA_CUR_POINT_COLOR;
//! Цвет линии, соединяющей опорные точки зоны
const COLORREF	GZ_LINE_COLOR		= CA_LINE_COLOR;
//! Цвет линии, соединяющей опорные точки редактируемой зоны
const COLORREF	GZ_EDITED_ZONE_LINE_COLOR = RGB(0, 0, 0);

//! Толщина рамки, обозначающей рабочую область
const DWORD WORK_AREA_BOUND_THICKNESS = 1;

class CChildView : public CWnd
					, public IScalable
					, public IAppSettingsUser
					, public IAppSettingsChangeListener
					, public ISceneViewFacade
					, public IIdleProcessor
{
	//! Указатель на фрайм приложения. Для обратной связи
	CMainFrame* m_ptrMainFrame;
	//! Окно, в котором один вью заменяет другой вью
	CSplitterClient* splitterClient_;

	//! Выбранный игровой объект
	qdGameObject		*m_ptrSelObj;
	//! \brief Состояние, которое выбрали в дереве проекта.
	//! Действительно для состояния VS_EDIT_SCENE
	qdGameObjectState*	m_ptrActiveState;

	//! Прямоугольник, накрывающий выбранный объект. Для смены курсора
	CRect				m_SelObjRect;
	//! Надо ли пересчитать CChildView::m_SelObjRect
	bool				m_bSelRectChanged;
	//! Текст тултипа
	CString				m_strToolTip;

	//! Номер перетаскиваемого якоря
	int					m_iDraggedAnchor;
	//! Прямоугольник, который отрисовываестя при работе с игровой областью
	CRect				m_rDraggedGameArea;
	//! DC окна
	HDC					m_ptrViewDC;
	
	//!	Область видимая в игре
	CRect				m_rcVisibleArea;
	//! Можно ли скроллировать сцену при помощи курсора-руки
	bool				m_bCanDragScene;

	//! Текущий курсор
	HCURSOR				m_hCurrCursor;
	//! Курсор рука
	HCURSOR				m_hHandCur;
	//! Схватившая рука
	HCURSOR				m_hDragHandCur;

	//! При редактировании контуров
	HCURSOR				m_hPenCursor;
	//! При редактировании траектории координатной анимации
	HCURSOR				m_hFootCursor;
	//! При редактировании сцены
	HCURSOR				m_hCrossCursor;
	//! Предыдущий, установленный курсор
	HCURSOR				m_hPrevCursor;
	//! Точка, от которой отталкиваемся при подсчете смещения	
	CPoint				m_base_point;
	//! Точка прорисовки тултипа
	CPoint				m_tt_point;
	//! Прямогольник, в который вмещается текст тул-типа
	CRect				m_tt_rect;
	
	//! Состояние(режим работы) редактора сцены
	VIEW_STATE			m_eViewState;
	//! Что делаем в текущем режиме работы
	STATE_SUBTOOLS_ENUM	m_eSubTools;

	//! Редактируемая зона
	qdGridZone			*m_ptrGZEdited;

	//! Предыдущий режим работы 
	/*!
		Используется, когда выбираем режим скроллирования рукой,
		чтобы восстановить предыдущее состояние
	*/
	VIEW_STATE			m_eOldViewState;

	//! Указатель на менеджер масштаба
	IScaleMgr*			m_ptrIScaleMgr;

	//! Текущий масштаб
	float				m_fScale;
	//! Начало отсчета бмп. Т.е. в какую точку окна кладем верхний левый угол бмп
	CPoint				m_pointBmpViewOrg;
	//! Пределы скроллроавния по Х
	int					m_iMaxScrollLimX, m_iMinScrollLimX;
	//! Пределы скроллирования по У
	int					m_iMaxScrollLimY, m_iMinScrollLimY;
	
	//! Указатель на диалог редактирования КА
	CoordAnimDlg		*m_ptrCADlg;
	//! Указатель на диалог стыковки
	StateCoordsStacker	*m_ptrStateStacker;
	//! Указатель на диалог редактирования размеров
	class StateBoundDlg	*m_ptrStateBoundDlg;
	//! КА для редактирования
	/*!
	 	В неё копируется исходная КА. А после редактирования она копируется
		поверх исходной.
	*/
	qdCoordsAnimation	m_refCAnim;
	//! Можем ли мы тащить якорь в ка.
	bool				m_bCACanDrag;

	//! Разграничивает доступ к прорисовке
	CCriticalSection	m_csDrawGuard;

	//! Временный объект для создания масок
	/*!
	 	В неё копируется исходная маска. А после редактирования она копируется
		поверх исходной.
	*/
	qdGameObjectStateMask	m_qdStateMask;
	//! Исходная маска
	qdGameObjectStateMask	*m_ptrSourceMask;
	//! Родительский объект для маски.(Объект, к которому привязана маска)
	qdGameObject			*m_ptrSMParent;

	//! Активный инвентори
	class qdInventory		*m_ptrActiveInventory;
	//! Прямоугольник активного набора ячеек
	CRect					m_ActiveCellSetRect;
	//! Вспомогательный рект, участвующий в расчетах
	/*!
	 	Нужен при расчетах прилипающего прямоугольничка
	*/
	CRect					m_rcHelper;
	//! Текущая опорная точка
	/*!
	 	Используется при редактирования КА, масок, зон на сетке
	*/
	int						m_iActivePoint;
	//! Замкнут	ли редактируемый контур
	bool					m_bConturClosed;
	//! Сделаны ли изменения с момента сохранения.
	/*!
	 	Используется при создании масок и зон.
	*/
	bool					m_bWorkChanged;
	//! Рисовать ли проекции на сетку для всех объектов
	bool					m_bDrawNetProjs;
	//! Рисовать ли границы для всех объектов
	bool					m_bDrawBounds;	

	//! Объект, у которого редактируются размеры и направление
	qdGameObjectAnimated* m_ptrEditedAnimated;
	//! Размеры объекта CChildView::m_ptrEditedAnimated
	Vect3f m_pers_bounds;

	//! Стыкуемое состояние
	/*!
		Это состояние, которое стыкуется с другим. Его мы возим по экрану
	*/
	qdGameObjectState* m_ptrStackedState;

	//! Редактируемое состояние.
	/*!
	 	Используется при определении размера состояния
	*/
	qdGameObjectState* m_ptrEditedState;

	//! Подсвеченный прямоуголник. Определяет где подсвтка.
	CRect m_rectHilited;


	//! Указатель на undo буффер
	std::auto_ptr<UndoBufferMgr> m_ptrUndo;
	//! Запись undo буффера для позиции объекта
	std::auto_ptr<ubeObjPos> m_ptrUbeObjPos;

	//! Идентификатор главного потока приложения
	DWORD m_dwMainThreadID;

	//! Объект класса, реализующиего режим редактирования интерфейса
	std::auto_ptr<ISceneViewMode> m_ptrIntefEditMode;
	//! Объект класса,  реализующиего режим редактирования рамки диалогов
	std::auto_ptr<ISceneViewMode> m_ptrEditDialogsFrameMode;

	//! Текущий режим 
	ISceneViewMode* m_ptrCurrentSceneViewMode;

	//! Текущий проект
	IProjectFacade* project_;

	//! Объект, чья анимация пригрыается
	state_guard				playedObject_;
	//! Состояние, чья анимация проигрывается
	qdGameObjectState*		playedState_;
	//! Надо ли продолжать проигрывать анимацию
	BOOL					stopAnimation_;

	UINT showVideoTimer_;

	//! Кого будем уведомлять о том, что изменения в зоне будем сохранять
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


	//! Устанавливает менеджер масштаба
	void SetScaleMgr(IScaleMgr* psmgr);
	//! Нарастить масштаб
	void IncreaseScale();
	//! Уменьшить масштаб
	void DecreaseScale();
	//! Текущий масштаб
	float CurrentScale();
	//! Назначить текущий масштаб
	void SetScale(float fscale);

	//! Устанавливает значение CChildView::m_bDrawNetProjs
	void SetDrawNetProjs(bool bval);
	//! Возвращает значение CChildView::m_bDrawNetProjs
	bool GetDrawNetProjs() const;
	//! Устанавливает значение CChildView::m_bDrawBounds
	void SetDrawBounds(bool bval);
	//! Возвращает значение CChildView::m_bDrawBounds
	bool GetDrawBounds() const;

	//! Возвращает страницу настроек для редактора сцены 
	virtual boost::shared_ptr<AppSettingsBasePage> getPage();
	//! Были сделаны измениея в настройках
	virtual void SettingsChanged();

//#ifndef USE_NEW_PROJ
	//! Назначает фрайм
	void set_main_frame(CMainFrame* pmf){m_ptrMainFrame = pmf;}
//#endif // USE_NEW_PROJ
	void setSplitterClient(CSplitterClient* client);
	CSplitterClient* getSplitterClient() const;

	qdGameDispatcher* getGameDispatcher() const;
	//достаёт камеру из активной сцены
	qdCamera*	 get_camera();

	//! Начинает создавать зону
	void OnCreateGridZone();
	//! Начинает редактировать зону
	void OnEditGridZone(qdGridZone* ptrZone, IWorkOwner* workOwner);
	//! На прекращение редактирования сеточной зоны
	void OnAbortGridZoneEdit(bool bSave);

	//! На начало редактирования размеров состояния
	void OnEditStateDims(class StateBoundDlg* pdlg, qdGameObjectState* pstate);
	//! На прекращение редктирования размреов
	void OnEndEditStateDims(bool bSave);

	//! Устанавливает состояние(рабочий режим) редактора сцены
	void SetViewState(VIEW_STATE eVal);
	//! Возвращает рабочий режим
	VIEW_STATE GetViewState(){return m_eViewState;}
	//! Устанавливает уточнение для текущего состояния(режима работы)
	void SetSubTools(STATE_SUBTOOLS_ENUM ssteVal);
	//! Возвращает уточнение для режима работы
	STATE_SUBTOOLS_ENUM GetSubTools(){return m_eSubTools;}

	//! Устанавлиает значение для CChildView::m_hCursor
	void SetCursor(HCURSOR hCur);

	//! На начало создание маски
	bool OnCreateStateMask(qdGameObject* ptrGO);
	//! На начало редактирования маски
	bool OnEditStateMask(qdGameObjectStateMask* ptrMask);
	//! На окончание редактирования маски
	void OnAbortStateMaskEdit(bool bSave);

	//! На начало редактирования направления и размеров
	bool OnEditAnimated(qdGameObjectAnimated* ptrPers);
	//! На окончание редактирования направления и размеров
	void OnAbortAnimatedEdit(bool bSave);
	
	//! Производит приведение объекта в начальное состояние
	void reset();
	//! Перерисовывает сцену
	void RedrawScene(qdGameScene* pscene);

	//! Перерисовка содержимого окна
//#ifdef USE_NEW_PROJ
	void Redraw(eRedrawMode emode);
//#endif 
//	void Redraw(sv_api::eRedrawMode emode);

	//! Выделяет объект в редакторе сцены
	void SelectObj(qdGameObject* ptrObj);
	//! Устанавливает активное состояние
	void SetActiveState(qdGameObjectState* pstate);
	//! Устанавливает активный инвентори
	void SelectInventory(qdInventory* ptrInv);

	//! Обновляет параметры скроллеров
	void UpdateScrollers();
	//! Выравнивает размер бмп так, чтобы в него влазил контент сцены с напуском
	bool AdjustBitmapSize();

	//! Устанавливает значение для CChildView::m_ptrCADlg
	void SetCoordsAnimationDlg(CoordAnimDlg* ptrDlg){
		m_ptrCADlg = ptrDlg;
	}

	//! Устанавливает значение для CChildView::m_ptrStateStacker
	void SetStateStackerDlg(StateCoordsStacker* ptrDlg){
		m_ptrStateStacker = ptrDlg;
	}
	//! Возвращает ссылку на редактируемую КА
	qdCoordsAnimation& GetCoordsAnimation(){
		return m_refCAnim;
	}

	//! Флеширует содержание бмп из прямоугольника, соответствующего r, в окно
	void PaintRect(const RECT &r);

	//! Возвращает засчитника прорисовки
	CCriticalSection* GetDrawGuard(){
		return &m_csDrawGuard;
	}

	//! \brief Проверяем есть ли точки и добавляем текущее полоежние
	//! объекта, если список точке пустой
	void CACheckPointsCount();
	//! Обновляет кооридинатную анимацию в редакторе сцены
	void UpdateCoordsAnimation();

	//! На начало редктирования КА
	void BeginEditCoordAnim(qdCoordsAnimation* ptrCA);
	//! На окончание редактирования КА
	void EndEditCoordAnim();

	//! Устанавливает значение для CChildView::m_ptrStackedState
	void SetStackedState(qdGameObjectState* ptrState){
		m_ptrStackedState = ptrState;
	}

	//! Выполняет операцию undo
	void Undo(ubentry* pube);

	//! Возвращает логическую позицию курсора
	const Vect2s GetLogCurPos();
	//! Возвращает логическую позицию курсора
	const CPoint& GetLogCurPos(CPoint& point);
	//! Возвращает позицию курсора в оконных координатах
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

	//! Обновляет информацию о текущем масштабе в менеджере масштаба
	void UpdateScaleInfo();

	//! Отрисовать тултип
	bool DrawToolTip();
	//! Рисует желтую рамку вокрур сцены
	void DrawWorkAreaBound();
	//! Отрисовывает видимую область 
	void DrawVisibleArea();
	//! Рендерит инвентори в левом верхнем углу сцены
	void DrawInventory();
	//! Отрисовывает видимую область для инвентори
	void DrawVisibleAreaForInventory();
	//! Обводим рамкой отдельные наборы ячеек
	void DrawCellsetBounds(qdInventory* ptrInv);
	//! Отрисовывает рамку активного набора ячеек 
	void DrawActiveCellsetBound(qdInventoryCellSet const& set, 
		CPoint const& SceneLeftTop);
	//! Отрисовывает рамку набора ячеек 
	void DrawCellsetBound(const qdInventoryCellSet& set, const CPoint& SceneLeftTop);

	//! Рендерит интерфейс
	void DrawInterface();
	//! Рендерит внутриигровой интерфейс
	void DrawIngameInterface();

	//! Отрисовывает выбранный объект
	bool DrawSelObject();

	//! Непосредственная отрисовка точек координатной анимации
	void DrawCoordsAnimPts(const qdCoordsAnimation& ca, bool bClosed, 
										qdCamera* ptrCam, COLORREF line_clr);
	//! Отрисовка точек координатной анимации
	void DrawCoordAnimationPoints();
	//! Отрисовывает маску
	void DrawMask();

	//! Отрисовывает контур зоны на сетке
	void DrawGridZoneContour();

	//! Рисует контур зоны, обводя ячейки входящие в зону
	void DrawGridZoneContour2(const qdGridZone* ptrZone, COLORREF line_clr);

	//! Рисует контур зоны по точкам контура
	void DrawGridZoneContour(const grid_contour_t& gc, bool bClosed, 
									int iActivePoint, bool bDrwPoints, 
									COLORREF line_clr = GZ_LINE_COLOR);
	//! Отрисовывает контуры всех зон
	void DrawGridZoneContours();

	//! Отображает проекцию на сетку выбранного объекта
	bool DrawNetProj(qdGameObject const* ptrObj, int inner_color = 0x000000FF);
	//! Рисует проекции для всего, что имеет проекцию на сетку
	void DrawNetProjs(const qdGameScene* pscene);
	//! Рисует размеры для всего, что имеет размеры
	void DrawBounds(const qdGameScene* pscene);

	//! Вспомогательная для CChildView::DrawStackedState()
	void drw_obj(qdGameObjectAnimated* ptrAnimObj, 
				 qdGameObjectState* ptrState,
				 qdCoordsAnimation* pca,
				 qdAnimationInfo*ptrAnimInfo, 
				 float fAnimPhase, 
				 int dir_num,
				 int offs_type,
				 COLORREF cntr_clr);
	//! Делает отрисовку стыкуемых объектов
	void DrawStackedState();

	//! Рисует подсвеченный прямоугольник
	void HiliteRect(const CRect& rect);

	//! Устанавливает позицию объекта 
	bool MoveObjectTo(qdGameObject* ptrObj, Vect3f pos);

	//визуализация видимой области(Visible Area (VA))
	//под видимой областью понимается часть сцены, видимая 
	//в игре при заданном разрешении

	//! Проверяет есть ли необходимость сдвинуть видимую область
	CSize VATest(qdGameObjectMoving* ppers);
	//! Сдвигает видимую область, учитывая размеры игровой области(рабочей)
	bool VAMove(CSize sz);

	//! Находит объект по точке
	qdGameObject* GetObjectFromPoint(const CPoint& p);

	//возвращает активную сцену
	inline qdGameScene* get_active_scene();

	//! передвижение точки
	bool drag_point(qdGameObject const* pobj, Vect3f &pos, 
					CPoint const& point, UINT nFlags, qdCamera const* ptrCam);
	//достаёт камеру из активной сцены
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
	
	//функции обработчки для режима создания/редактирования маски
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
		
	//находит по точке узел для таскания
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
	//сдвигает начало отсчета картинки и контролирует,
	//чтобы оно не вылетало за устанвленные пределы
	void moveBmpOrg(int dx, int dy);

	//сдвигает через moveBmpOrg и перерисовывает
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
	//при принудительно обрыве. Например сменили узел в дереве
	void GZAbortEdit(bool bSave);

	//ищет зону отличную от данной,
	//определяя попадание в точку контура
	qdGridZone* GZFindZoneByCntrPnt(const Vect2s&vPoint, 
									const qdGridZone* ptrExcludeZone);

	//ищет зону отличную от данной
	qdGridZone* GZFindZone(const Vect3f& vPoint, const qdGridZone* ptrExcludeZone);
	//ицет опорную точку зоны
	int GZFindPoint(const Vect2s& vPoint, qdGridZone*ptrZone);
	//добавляет опорную точку.
	//возвращает её порядковый номер
	int GZAddPoint(const Vect2s& vPoint);
	//вставляет точку, если кликнули по линии.
	//true - если вставили
	bool GZInsertPoint(const Vect3f& planePoint);

	//вставляет точку, если кликнули по линии.
	//true - если вставили
	bool SMInsertPoint(const Vect2s& mask_pos);
	bool SMAddPoint(const Vect2s& mask_pos);
	
	//поиск точки по контуру маски
	int SMFindPoint(const Vect2s& point);

	//переводят координаты из экранных в масочные и обратно
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
/* редактирование размера и направления анимированного объекта          */
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
/* редактирование размера для состояния                                 */
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
/* Стыковка состояний							                        */
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
		
	//перерисовавываем стыкуемое состояние объекта при перетаскивании
	void SSRedrawStackedState(qdGameObjectState* ptrState, int dx, int dy, const Vect3f& vecOldPos);
	
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*  Редактирование инвентори ((E)dit (I)nevtory)                        */
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
	//из рабочего прямоугольника получаем, то что будем показывать на экране,
	//применять к набору ячеек
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
	
	//определяет min и max по х и у для массива Vect3f
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
