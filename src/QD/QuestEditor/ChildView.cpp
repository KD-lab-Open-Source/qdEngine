// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include <windowsx.h>

#include "QuestEditor.h"
#include "ChildView.h"
#include "SplitterClient.h"

#include "GR_dispatcher.h"
#include "snd_dispatcher.h"

#include "qd_camera.h"
#include "qd_setup.h"
#include "qd_grid_zone.h"
#include "qd_inventory.h"

#include "qd_game_object_moving.h"
#include "qd_animation_set.h"

#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"

#include "qd_interface_element.h"
#include "qd_interface_screen.h"

#include "mainfrm.h"
#include "chosename.h"
#include "name_op.h"
#include "auto_parray.h"

#include <algorithm>

#include "statecoordsstacker.h"
#include "undobuffermgr.h"
#include "ube_obj_pos.h"

#include "ptree_hlprs.hpp"
#include "ptree_api.h"
#include "statebounddlg.h"

#include "editinterfmodedata.h"

#include "EditInterfMode.h"

#include "moveocntr.h"
#include "SceneViewStngsPP.h"

#include <boost/iterator_adaptors.hpp>
#include <boost/mem_fn.hpp>
#include "sv_helpers.h"
#include "IWorkOwner.h"
#ifdef USE_NEW_PROJ
#include "Main/IProjectFacade.h"
#include ".\childview.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace sv_helpers;

std::pair<bool, qdGameObject*> AskForObject(qdGameScene* ptrScene, 
											int type, bool bCanCrtNew);

/************************************************************************/
/*Константы                                                             */
/************************************************************************/
const float Z_CHANGE_DELTA = 1.f;
const float SM_MAX_DIST_VALUE = 500.f;
const float GZ_MAX_DIST_VALUE = 1500.f;

const int STICK_DISTANCE = 5;

extern HCURSOR GR_MouseCursorCross;

const DWORD HILITE_LINE_LEN			= 5;
const DWORD ANCHOR_WIDTH			= 10;
const DWORD ANCHOR_HEIGHT			= ANCHOR_WIDTH;
const DWORD ANCHOR_COLOR			= RGB(255, 255, 255);

const COLORREF WORK_AREA_BOUND_COLOR = RGB(255, 255, 0);

const DWORD IMPASSIBLE_CELL_CLR		= RGB(255, 0, 0);
const DWORD SELECTED_CELL_CLR		= RGB(0, 0, 255);
const DWORD NORMAL_CELL_CLR			= RGB(255, 255, 255);

const COLORREF GAME_AREA_COLOR		= RGB(0, 0, 255);

const DWORD HILIGHTED_POINT_COLOR	= RGB(0, 0, 0);
const DWORD HILIGHTED_LINE_COLOR	= RGB(0, 0, 0);

const DWORD HILIGHTED_POINT_OFFSET	= 10;

const DWORD SEL_RECT_DELTA		= 8;
const DWORD SEL_RECT_COLOR		= RGB(0, 0, 0);

const float SMALL_VALUE = 0.0001f;

const int NET_PROJ_SIZE	= 10;
const int OBJ_CENTER_SIZE = 6;
const float MAX_INIT = -40000000;
const float MIN_INIT = - MAX_INIT;

const int CELL_SET_COLOR			= RGB(0, 0, 0);
const int CELL_SET_ANCHOR_WIDTH		= 6;
const int CELL_SET_ANCHOR_COLOR		= RGB(255, 255, 255);

//количество пикселей между прорисованными точками
//линии. В результате получаем пунктирную линию, где
//каждая следующая прорисованная точка отстоит от предыдущей на DAHS_LEN
const int DASH_LEN = 2;


inline bool PointInNet(qdCamera const* pcam, Vect3f const& point){
	Vect2s vGridIndexs = pcam->get_cell_index(point);
	return (vGridIndexs.x != -1 && vGridIndexs.y != -1);
}


CChildView::pers_dir_safer::pers_dir_safer(qdGameObjectAnimated* ptrAnim):
m_ptrMoving(NULL)
{
	if (ptree_helpers::is_obj_moving(ptrAnim)) {
		m_ptrMoving = static_cast<qdGameObjectMoving*>(ptrAnim);
		m_dir = m_ptrMoving->direction_angle();
	}
}

CChildView::pers_dir_safer::~pers_dir_safer(){
	if (m_ptrMoving) 
		m_ptrMoving->set_direction(m_dir);
}

inline void CChildView::draw_contours::operator()(const qdGridZone* ptrZone)const{
	if(!ptrZone->check_flag(QD_OBJ_HIDDEN_FLAG))
		if (ptrZone != m_ptrSel) 
			m_ptrView->DrawGridZoneContour2(ptrZone, GZ_LINE_COLOR);
}

/************************************************************************/
/* Глобальные функции                                                   */
/************************************************************************/
void PlayError()
{
	Beep(1000, 100);
	MessageBeep(MB_OK);
}


template<class _Vect>
float GetDist(const _Vect& a, const _Vect& b, const _Vect& p)
{
	return (static_cast<float>(b.y - a.y)*p.x + 
		static_cast<float>(a.x - b.x)*p.y + 
		static_cast<float>(a.y - b.y)*a.x + 
		static_cast<float>(b.x - a.x)*a.y);
}

const Vect3f get_state_pos(qdGameObjectState* ptrState){

	qdCoordsAnimation* ptrCA = ptrState->coords_animation();
	if (!ptrCA->is_empty()) 
		return ptrCA->get_point(ptrCA->get_cur_point())->dest_pos();

	return static_cast<qdGameObjectAnimated*>(ptrState->owner())->default_R();
}

void set_state_pos(qdGameObjectState* ptrState, const Vect3f& pos, 
						   const float grid_center_z)
{
	qdCoordsAnimation* ptrCA = ptrState->coords_animation();
	qdGameObjectAnimated* pobj = static_cast<qdGameObjectAnimated*>(ptrState->owner());
	if (!ptrCA->is_empty()) 
	{
		Vect3f __t(pos);
		//походка всегда усажывается на сетку
		if (ptrCA->type()==qdCoordsAnimation::CA_WALK) 
			__t.z = grid_center_z + pobj->bound().z/2;

		ptrCA->get_point(ptrCA->get_cur_point())->set_dest_pos(__t);
	}
	else 
	{
		pobj->set_pos(pos);
		pobj->set_default_pos(pos);
		if (ptree_helpers::is_obj_moving(pobj))
		{
			static_cast<qdGameObjectMoving*>(pobj)->adjust_z();
			pobj->set_default_pos(pobj->R());
		}
	}
}

void SetDirection(qdGameObjectMoving* pobj, qdGameObjectState* pstate, int dir_num)
{
	qdAnimationSet* const pset = 
		static_cast<qdGameObjectStateWalk*>(pstate)->animation_set();
	pobj->set_direction(pset->get_index_angle(dir_num));
}
/************************************************************************/
/* Вспомогательные для координатной анимации                            */
/************************************************************************/
std::auto_ptr<Vect3fVect_t> CACAPts2Vect3f(const qdCoordsAnimation& refCA)
{
	std::auto_ptr<Vect3fVect_t> res (new Vect3fVect_t);
	if(!res.get())
		return res;

	int isize = refCA.size();
	res->reserve(isize);
	for(int i = 0; i <isize; ++i)
		res->push_back(refCA.get_point(i)->dest_pos());

	return res;
}

bool CAIsEndEqlBeg(const qdCoordsAnimation& ca){
	if (ca.is_empty()||ca.size()==1) return false;
	return (ca.get_point(0)->dest_pos() != ca.get_point(ca.size()-1)->dest_pos());
}

//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/*Вспомогательные для сеточных зон        								*/
/************************************************************************/
std::auto_ptr<Vect3fVect_t> GZIndexes2Plane(const grid_contour_t& gc, qdCamera *ptrCam)
{
	std::auto_ptr<Vect3fVect_t> res (new Vect3fVect_t);
	if(!res.get())
		return res;
	
	grid_contour_t::const_iterator _itr = gc.begin(), _end = gc.end();
	for(; _itr != _end; ++_itr)
		res->push_back(ptrCam->get_cell_coords((*_itr).x, (*_itr).y));
	
	return res;
}

//заполняет ячейками попавшими в контур
bool GZFillCells(qdGridZone* ptrZone, qdCamera* ptrCam)
{
	bool bState = ptrZone->state();
	if (bState) 
		ptrZone->set_state(false);
	ptrZone->select(ptrCam, false);

	bool res = ptrZone->update_contour();
	if (bState) 
		ptrZone->set_state(true);
	ptrZone->select(ptrCam, true);
	return res;
}
//  [9/27/2002]
inline void det_min_max(CPoint& left_top, CPoint& right_bottom, const Vect2s& v)
{
	left_top.x = (std::min)(left_top.x, static_cast<LONG>(v.x));
	left_top.y = (std::min)(left_top.y, static_cast<LONG>(v.y));
	
	right_bottom.x = (std::max)(right_bottom.x, static_cast<LONG>(v.x));
	right_bottom.y = (std::max)(right_bottom.y, static_cast<LONG>(v.y));
}

//возварщает рект в экранных координатах,
//который соответствует боунду персонажа
template<class _ObjType>
CRect GetObjBoundRect(qdCamera* ptrCam, _ObjType *pobj, const Vect3f& r)
{
	Vect3f b = pobj->bound();

	float bx = b.x / 2.0f;
	float by = b.y / 2.0f;
	float bz = b.z / 2.0f;
	
	Vect2s v = ptrCam -> global2scr(Vect3f(r.x - bx,r.y - by,r.z - bz));
	CPoint left_top(v.x, v.y), right_bottom(v.x, v.y);
	
	v = ptrCam -> global2scr(Vect3f(r.x - bx,r.y - by,r.z - bz));
	det_min_max(left_top, right_bottom, v);

	v = ptrCam -> global2scr(Vect3f(r.x - bx,r.y + by,r.z - bz));
	det_min_max(left_top, right_bottom, v);

	v = ptrCam -> global2scr(Vect3f(r.x + bx,r.y + by,r.z - bz));
	det_min_max(left_top, right_bottom, v);

	v = ptrCam -> global2scr(Vect3f(r.x + bx,r.y - by,r.z - bz));
	det_min_max(left_top, right_bottom, v);
	
	v = ptrCam -> global2scr(Vect3f(r.x - bx,r.y - by,r.z + bz));
	det_min_max(left_top, right_bottom, v);
	
	v = ptrCam -> global2scr(Vect3f(r.x - bx,r.y + by,r.z + bz));
	det_min_max(left_top, right_bottom, v);
	
	v = ptrCam -> global2scr(Vect3f(r.x + bx,r.y + by,r.z + bz));
	det_min_max(left_top, right_bottom, v);
	
	v = ptrCam -> global2scr(Vect3f(r.x + bx,r.y - by,r.z + bz));
	det_min_max(left_top, right_bottom, v);
	
	return CRect(left_top, right_bottom);
}

//////////////////////////////////////////////////////////////////////////
/*
Vect3f GetNearestNetPoint(qdCamera* ptrCam, const Vect2f& pos)
{
	int dim = ptrCam->get_grid_sx()*ptrCam->get_cell_sx();

	int coord = round(pos.x) + (dim>>1);
	
	Vect3f res(0, 0, 0);
	res.x = coord/ptrCam->get_cell_sx()*ptrCam->get_cell_sx();
	if((coord - res.x) > (res.x + ptrCam->get_cell_sx() - coord))
		res.x += ptrCam->get_cell_sx();

	res.x -= dim>>1;

	dim = ptrCam->get_grid_sy()*ptrCam->get_cell_sy();
	coord = static_cast<int>(pos.y) + (dim>>1);
	res.y = static_cast<float>(coord/ptrCam->get_cell_sy())*ptrCam->get_cell_sy();

	if((coord - res.y) > (res.y + ptrCam->get_cell_sy()- coord))
		res.y += ptrCam->get_cell_sy();

	res.y -= dim>>1;

	return res;
}

*/
//возвращает ближайшую сторону многоугольника,
//находящюуся на расстоянии меньшем чем lenMax
template<typename __Vect, class __VectCont>
int GetNearestSide(const __Vect& v, const __VectCont& vc, float lenMax)
{
	_ASSERT(!vc.empty());
	int res = 0;
	float len = 1000000.f;
	__VectCont::const_iterator _itr = vc.begin(), _end = vc.end()-1;
	int i = 0;
	__Vect N;//нормаль к отрезку
	for(; _itr != _end; ++i, ++_itr)
	{
		__Vect const& a = *_itr;
		__Vect const& b = *(_itr + 1);


		float x = a.y;
		N.x = a.y - b.y;
		N.y = b.x - a.x;
		
		//определяем положение v отностельно вершинных перпендикляров
		float __al = GetDist(a, a + N, v);
		float __bl = GetDist(b, b + N, v);
		
		if((__al <= 0 && 0 <= __bl)
			||(__bl <= 0 && 0 <= __al))
		{
			//значит v лежит между. можно искать растояние до стороны
			__al = abs(GetDist(a, b, v));
			if (__al < len) {
				len = __al;
				res = i;
			}
		}
	}
	const __Vect& a = *_end;
	const __Vect& b = vc.front();
	
	N.x = a.y - b.y;
	N.y = b.x - a.x;
	
	//определяем положение v отностельно вершинных перпендикляров
	float __al = GetDist(a, a + N, v);
	float __bl = GetDist(b, b + N, v);
	
	if((__al <= 0 && 0 <= __bl)
		||(__bl <= 0 && 0 <= __al))
	{
		//значит v лежит между. можно искать растояние до стороны
		__al = abs(GetDist(a, b, v));
		if (__al < len) {
			len = __al;
			res = vc.size()-1;
		}
	}
	if (len>lenMax)
		return -1;

	return res;
}


void Draw3dBox(const Vect3f& r, const Vect3f& dims, qdCamera const* pcam, int cl)
{
	const float bx = dims.x / 2.0f;
	const float by = dims.y / 2.0f;
	const float bz = dims.z / 2.0f;
	Vect2s v0 = pcam -> global2scr(Vect3f(r.x - bx,r.y - by,r.z - bz));
	Vect2s v1 = pcam -> global2scr(Vect3f(r.x - bx,r.y + by,r.z - bz));
	Vect2s v2 = pcam -> global2scr(Vect3f(r.x + bx,r.y + by,r.z - bz));
	Vect2s v3 = pcam -> global2scr(Vect3f(r.x + bx,r.y - by,r.z - bz));

	Vect2s _v0 = pcam -> global2scr(Vect3f(r.x - bx,r.y - by,r.z + bz));
	Vect2s _v1 = pcam -> global2scr(Vect3f(r.x - bx,r.y + by,r.z + bz));
	Vect2s _v2 = pcam -> global2scr(Vect3f(r.x + bx,r.y + by,r.z + bz));
	Vect2s _v3 = pcam -> global2scr(Vect3f(r.x + bx,r.y - by,r.z + bz));

	grDispatcher* const pgr = grDispatcher::instance();
	pgr->Line(v0.x,v0.y,v1.x,v1.y,cl,4,true);
	pgr->Line(v1.x,v1.y,v2.x,v2.y,cl,4,true);
	pgr->Line(v2.x,v2.y,v3.x,v3.y,cl,4,true);
	pgr->Line(v3.x,v3.y,v0.x,v0.y,cl,4,true);

	pgr->Line(_v0.x,_v0.y,_v1.x,_v1.y,cl,4,true);
	pgr->Line(_v1.x,_v1.y,_v2.x,_v2.y,cl,4,true);
	pgr->Line(_v2.x,_v2.y,_v3.x,_v3.y,cl,4,true);
	pgr->Line(_v3.x,_v3.y,_v0.x,_v0.y,cl,4,true);

	pgr->Line(v0.x,v0.y,_v0.x,_v0.y,cl,4,true);
	pgr->Line(v1.x,v1.y,_v1.x,_v1.y,cl,4,true);
	pgr->Line(v2.x,v2.y,_v2.x,_v2.y,cl,4,true);
	pgr->Line(v3.x,v3.y,_v3.x,_v3.y,cl,4,true);
}

//возвращает прямоугольник в экранных координатах
//Но это не экранные коориданаты сцены.
//чтобы их получить надо добавить левый верхний угол сцены
const CRect GetCellsetRect(const qdInventoryCellSet* pset){
	const Vect2i cell_size(pset->cell_size());
	const Vect2s& screen_pos = pset->screen_pos();
	const Vect2i screen_size(pset->screen_size());
	CRect res(screen_pos.x, screen_pos.y,
		screen_pos.x + screen_size.x, screen_pos.y + screen_size.y);
	res.OffsetRect(-(cell_size.x>>1), -(cell_size.y>>1));
	return res;
}

bool DrawNetProj(qdCamera const* pcam, Vect3f pos, grDispatcher* const pgrd,
				 int inner_color)
{
	Vect2s const scr_pos = pcam->global2scr(pos);
	pos.z = pcam->get_grid_center().z;
	Vect2s proj_pos = pcam->global2scr(pos);
	//прорисовываем проекцию на сетку
	pgrd->Rectangle(proj_pos.x - (NET_PROJ_SIZE>>1), proj_pos.y - (NET_PROJ_SIZE>>1), 
		NET_PROJ_SIZE, NET_PROJ_SIZE, 0x00FF0000, inner_color, GR_FILLED);

	//прорисовываем центр объекта
	pgrd->Rectangle(scr_pos.x - (OBJ_CENTER_SIZE>>1), scr_pos.y - (OBJ_CENTER_SIZE>>1), 
		OBJ_CENTER_SIZE, OBJ_CENTER_SIZE, 0x00FF0000, 0x00FF0000, GR_FILLED);
	pgrd->Line(proj_pos.x, proj_pos.y, scr_pos.x, scr_pos.y, 0x00FF0000);
	return true;
}

void DrawNetProjsForStates(qdGameObjectAnimated const* pobj, 
						   qdCamera const* pcam, int inner_color)
{
	grDispatcher * const pgrd = grDispatcher::instance();
	qdGameObjectStateVector const& v = pobj->state_vector();
	qdGameObjectStateVector::const_iterator i = v.begin(), e = v.end();
	for(; i != e; ++i)
	{
		qdGameObjectState* const pstate = *i;
		if (!pstate->coords_animation()->is_empty()) 
			::DrawNetProj(pcam, pstate->coords_animation()->
							get_point(0)->dest_pos(), pgrd, inner_color);
	}
}

void DrawDraggedRect(HDC dc, const CRect& r, COLORREF color)
{
	int iMode = SetROP2(dc, R2_NOTXORPEN);
	
	HGDIOBJ obr = SelectObject(dc, GetStockObject(NULL_BRUSH));
	HGDIOBJ oldp = SelectObject(dc, CreatePen(PS_SOLID, 1, color));
	
	Rectangle(dc, r.left, r.top, r.right, r.bottom);
	
	SelectObject(dc, obr);
	SelectObject(dc, oldp);
	
	SetROP2(dc, iMode);
}

void SaveScrLayoutOnBndEdt(qdGameObjectState* pstate, qdCamera* pcam, 
						   Vect3f const& old_bnd, Vect3f const&bnd)
{
	Vect3f pos(get_state_pos(pstate));

	Vect2s v1(pcam->global2scr(Vect3f(pos.x, pos.y, old_bnd.z/2.f)));
	Vect2s v2(pcam->global2scr(Vect3f(pos.x, pos.y, bnd.z/2.f)));
	pstate->set_center_offset(pstate->center_offset() + v1-v2);
}

void SaveScrLayoutOnBndEdt(qdGameObjectState* pstate, qdCamera* pcam, 
						   Vect3f const& old_bnd)
{
	SaveScrLayoutOnBndEdt(pstate, pcam, old_bnd, pstate->bound());
}

//сохраняем положение картики для всех состоняний объекта, которые
//не имеют собственной границы
void SaveScrLayoutForStates(qdGameObjectMoving* const pobj, 
							qdCamera* pcam,
							Vect3f const& old_bnd,
							Vect3f const& bnd)
{
	const qdGameObjectStateVector& v = pobj->state_vector();
	qdGameObjectStateVector::const_iterator ix = v.begin(), ex = v.end();

	for(;ix != ex; ++ix)
	{
		if (!(*ix)->has_bound()&&!ptree_helpers::IsGlobalState4Obj(*ix, pobj))
			SaveScrLayoutOnBndEdt(*ix, pcam, old_bnd, bnd);
	}
}

void OffsetNotCAStates(qdGameObjectAnimated* const pobj, Vect2s const& off, 
				  qdGameObjectState* pex)
{
	if (off.x||off.y)
	{
		qdGameObjectStateVector const & v = pobj->state_vector();
		qdGameObjectStateVector::const_iterator i = v.begin(), e = v.end();
		for(; i != e; ++i)
		{
			qdGameObjectState* const pst = *i;
			if (pex != pst&&pst->coords_animation()->is_empty())
				pst->set_center_offset(pst->center_offset() - off);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CChildView
IMPLEMENT_DYNCREATE(CChildView,CWnd);

CChildView* g_ptrSceneView = NULL;

CChildView::CChildView():
m_base_point(0, 0), 
m_tt_point(0, 0),
m_SelObjRect(0, 0, 0, 0)
, m_ptrCurrentSceneViewMode(NULL)
#ifdef USE_NEW_PROJ
, project_(NULL)
#endif // USE_NEW_PROJ
, splitterClient_(NULL)
, workOwner_(NULL)
{
	m_ptrActiveState = NULL;
	m_ptrIntefEditMode.reset(new EditInterfMode);

	m_SelObjRect.SetRectEmpty();
	m_rDraggedGameArea.SetRectEmpty();
	m_rcVisibleArea.SetRectEmpty();
	m_tt_rect.SetRectEmpty();
	m_ActiveCellSetRect.SetRectEmpty();
	m_rcHelper.SetRectEmpty();
	m_rectHilited.SetRectEmpty();

	m_bDrawNetProjs = false;
	m_bDrawBounds		= false;

	g_ptrSceneView = this;
	m_ptrMainFrame = NULL;

	m_ptrUndo = std::auto_ptr<UndoBufferMgr>(new UndoBufferMgr);
	m_ptrUbeObjPos = std::auto_ptr<ubeObjPos>(new ubeObjPos);
	
	m_ptrSelObj = NULL;
	m_eViewState = VS_EDIT_SCENE;
	m_eSubTools = SSTE_NONE;
	
	m_fScale = 1.f;
	m_pointBmpViewOrg = CPoint(0, 0);
	m_iMaxScrollLimX = m_iMinScrollLimX = 0;
	m_iMaxScrollLimY = m_iMinScrollLimY = 0;
	
	m_iDraggedAnchor = -1;

	m_ptrCADlg = NULL;
	m_bCanDragScene = false;

	m_hHandCur		= AfxGetApp()->LoadCursor(IDC_CUR_HAND);
	m_hDragHandCur	= AfxGetApp()->LoadCursor(IDC_CUR_DRAG_HAND);
	m_hCrossCursor	= AfxGetApp()->LoadStandardCursor(IDC_CROSS);
	m_hPenCursor	= AfxGetApp()->LoadCursor(IDC_CUR_PEN);
	m_hFootCursor	= m_hPenCursor; //AfxGetApp()->LoadCursor(IDC_CUR_FOOT);
	
	m_hPrevCursor	= m_hCurrCursor = m_hCrossCursor;
	m_bCACanDrag = false;

	m_eOldViewState = m_eViewState = VS_NONE;

	m_ptrSourceMask = NULL;
	m_iActivePoint = -1;
	m_ptrSMParent = NULL;

	m_bConturClosed = false;
	m_bWorkChanged	= false;
	m_ptrGZEdited = NULL;
	m_bSelRectChanged = true;

	m_tt_rect.SetRectEmpty();
	m_rectHilited.SetRectEmpty();

	m_ptrEditedAnimated = NULL;
	m_ptrStackedState = NULL;
	m_ptrEditedState = NULL;
	m_ptrActiveInventory = NULL;
	m_ptrStateBoundDlg = NULL;
	m_ptrIScaleMgr		= NULL;

	m_dwMainThreadID = GetCurrentThreadId();
}

CChildView::~CChildView()
{
}

/*!
 	Вызывается, когда перезагружается скрипт
*/
void CChildView::reset()
{
	m_ptrSelObj = NULL;
	m_eViewState = VS_EDIT_SCENE;
	m_ptrActiveState = NULL;
	m_eSubTools = SSTE_NONE;
	m_ptrEditedAnimated = NULL;
	m_ptrEditedState = NULL;
	m_ptrGZEdited = NULL;

	if(m_qdStateMask.contour_size())
		m_qdStateMask.clear_contour();

	m_ptrSourceMask = NULL;
	m_iActivePoint = -1;

	m_rectHilited = m_rcHelper = m_ActiveCellSetRect = CRect();
	m_ptrUndo->clear();
	if(grDispatcher::instance())
		grDispatcher::instance()->Fill(0);

//#ifdef USE_NEW_PROJ
//	project_ = NULL;
//#endif // USE_NEW_PROJ

}
void CChildView::setSplitterClient(CSplitterClient* client){
	splitterClient_ = client;
}
CSplitterClient* CChildView::getSplitterClient() const{
	return splitterClient_;
}

qdGameDispatcher* CChildView::getGameDispatcher() const{
#ifdef USE_NEW_PROJ
	return project_->getGameDispatcher();
#else
	return m_ptrMainFrame->get_game_disp();
#endif // _DEBUG
}

inline qdGameScene* CChildView::get_active_scene()
{
	qdGameDispatcher *ptrGD = getGameDispatcher();
	return ptrGD? ptrGD->get_active_scene() : NULL;
}

inline qdCamera*	 CChildView::get_camera()
{
	qdGameScene* ptrScene = get_active_scene();
	return ptrScene? ptrScene->get_camera() : NULL;
}

void CChildView::setProject(IProjectFacade* project){
	project_ = project;
}

IProjectFacade* CChildView::getProject() const{
	return project_;
}

inline qdCamera*	 CChildView::get_camera(qdGameScene* pscene)
{
	_ASSERT(pscene);
	return pscene->get_camera();
}

BOOL CChildView::getStopAnimation() const{
	return stopAnimation_;
}

void CChildView::setStopAnimation(BOOL flag){
	stopAnimation_ = flag;
}

void CChildView::SetDrawNetProjs(bool bval)
{
	m_bDrawNetProjs = bval;
}
bool CChildView::GetDrawNetProjs() const{
	return m_bDrawNetProjs;
}
void CChildView::SetDrawBounds(bool bval)
{
	m_bDrawBounds = bval;
}
bool CChildView::GetDrawBounds() const{
	return m_bDrawBounds;
}

BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_CREATE()
	ON_COMMAND(ID_BTN_HAND, OnBtnHand)
	ON_UPDATE_COMMAND_UI(ID_BTN_HAND, OnUpdateBtnHand)
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_COMMAND_EX(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND_EX(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = grDispatcher::wnd_class_name();
	
	return TRUE;
}

inline bool CChildView::IsMouseOverSelected(const CPoint& scr_pt)
{
	return (m_ptrSelObj&&m_SelObjRect.PtInRect(scr_pt));
}

/*!
 	Отрисовывает тултип и определяет прямоугольник, охватывающий текст тултипа
*/
bool CChildView::DrawToolTip()
{
	if(!m_strToolTip.IsEmpty())
	{
		SetTextColor(m_ptrViewDC, RGB(255, 255, 255));
		SetBkMode(m_ptrViewDC, TRANSPARENT);

		CPoint p(m_tt_point);
		TextOut(m_ptrViewDC, p.x, p.y, m_strToolTip, m_strToolTip.GetLength());

		SIZE size;
		GetTextExtentPoint32(m_ptrViewDC, m_strToolTip, m_strToolTip.GetLength(), &size);

		m_tt_rect = CRect(m_tt_point, size);
	}
	return true;
}

void CChildView::moveBmpOrg(int dx, int dy)
{
	m_pointBmpViewOrg.Offset(dx, dy);
	
	if (m_pointBmpViewOrg.x > m_iMaxScrollLimX) {
		m_pointBmpViewOrg.x = m_iMaxScrollLimX;
	}
	if (m_pointBmpViewOrg.y > m_iMaxScrollLimY) {
		m_pointBmpViewOrg.y = m_iMaxScrollLimY;
	}
	if (m_pointBmpViewOrg.x < m_iMinScrollLimX) {
		m_pointBmpViewOrg.x = m_iMinScrollLimX;
	}
	if (m_pointBmpViewOrg.y < m_iMinScrollLimY) {
		m_pointBmpViewOrg.y = m_iMinScrollLimY;
	}
}

void CChildView::scrollbmp(int dx, int dy)
{
	CPoint oldBmpOrg = m_pointBmpViewOrg;

	moveBmpOrg(dx, dy);

	dx = m_pointBmpViewOrg.x - oldBmpOrg.x;
	dy = m_pointBmpViewOrg.y - oldBmpOrg.y;

	ScrollWindow(dx, dy);

	CRect rectClient;
	GetClientRect(&rectClient);

	if (dx) {
		CRect r(rectClient);

		if (dx>0) 
			r.right = dx;
		else
			r.left = r.right + dx;//dx<0
		IntersectClipRect(m_ptrViewDC, r.left, r.top, r.right, r.bottom);

		PaintRect(r);
		::SelectClipRgn(m_ptrViewDC, NULL);

		ValidateRect(r);
		m_bSelRectChanged = true;
	}

	if (dy) {
		CRect r(rectClient);
		if (dy>0) 
			r.bottom = dy;
		else
			r.top = r.bottom + dy;//dy<0
		
		IntersectClipRect(m_ptrViewDC, r.left, r.top, r.right, r.bottom);
		
		PaintRect(r);
		::SelectClipRgn(m_ptrViewDC, NULL);
		ValidateRect(r);
		m_bSelRectChanged = true;
	}
}

void CChildView::SetScrollRanges()
{
	CRect rectClient;
	GetClientRect(rectClient);
	
	int iScaledBmpSX = 
		round(grDispatcher::instance()->Get_SizeX()*m_fScale);
	int iScaledBmpSY = 
		round(grDispatcher::instance()->Get_SizeY()*m_fScale);
	
	if (rectClient.Width() < iScaledBmpSX) {
		m_iMaxScrollLimX = 0;
		m_iMinScrollLimX = rectClient.Width() - iScaledBmpSX;
	}
	else
		m_iMinScrollLimX = 
		m_iMaxScrollLimX = (rectClient.Width() - iScaledBmpSX)>>1;
	
	if (rectClient.Height() < iScaledBmpSY) {
		m_iMaxScrollLimY = 0;
		m_iMinScrollLimY = rectClient.Height() - iScaledBmpSY;
	}
	else
		m_iMinScrollLimY = 
		m_iMaxScrollLimY = (rectClient.Height() - iScaledBmpSY)>>1;
}

/*!
	Переводит \a \b r логические координаты. Затем флеширует содержание логического 
	прямоугольника на \a \b r
 	Если масштаб таков, что весь битмап влазит в окно, то зарисовывает пустоты 
	черным цветом.
*/
void CChildView::PaintRect(const RECT& r)
{
#ifndef USE_NEW_PROJ
	CSingleLock __sl(&m_csDrawGuard, TRUE);
#endif // USE_NEW_PROJ

	if (m_eViewState != VS_EDIT_INTERFACE)
	{
		if(qdGameScene* pscene = get_active_scene())
		{
			qdCamera* ptrCam = get_camera(pscene);
			CRect src_r(r);
			scr2log(reinterpret_cast<POINT*>(&src_r), 2);
			
			grDispatcher::instance()->
				StretchFlush(r.left, r.top, r.right - r.left, r.bottom - r.top,
							src_r.left, src_r.top, src_r.Width(), src_r.Height());

			if(m_pointBmpViewOrg.x > 0)
			{
				src_r.top = r.top;
				src_r.bottom = r.bottom;
				
				src_r.left = r.left;
				src_r.right = m_pointBmpViewOrg.x;
				
				if(src_r.left < src_r.right)
					FillRect(m_ptrViewDC, src_r, 
						static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
				
				src_r.left = m_pointBmpViewOrg.x + 
						grDispatcher::instance()->Get_SizeX()*m_fScale;
				src_r.right = r.right;
				
				if(src_r.left < src_r.right)
				{
					FillRect(m_ptrViewDC, src_r, 
						static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
				}
			}

			if(m_pointBmpViewOrg.y > 0)
			{
				src_r.top = r.top;
				src_r.bottom = m_pointBmpViewOrg.y;
				
				src_r.left = r.left;
				src_r.right = r.right;
				
				if(src_r.top< src_r.bottom)
					FillRect(m_ptrViewDC, src_r, 
						static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
				
				src_r.top = m_pointBmpViewOrg.y + 
							grDispatcher::instance()->Get_SizeY()*m_fScale;
				src_r.bottom = r.bottom;
				
				if(src_r.top< src_r.bottom)
					FillRect(m_ptrViewDC, src_r, 
						static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
			}
			DrawToolTip();

			return;
		}
		else
		{
			grDispatcher::instance()->Fill(0);
			grDispatcher::instance()->Flush(r.left, r.top, 
				r.right - r.left, r.bottom - r.top);
		}
	}
	else 
	{
		CRect src_r(r);
		scr2log(&src_r);
		grDispatcher::instance()->
			StretchFlush(r.left, r.top, r.right - r.left, r.bottom - r.top,
			src_r.left, src_r.top, src_r.Width(), src_r.Height());

		if(m_pointBmpViewOrg.x > 0)
		{
			src_r.top = r.top;
			src_r.bottom = r.bottom;

			src_r.left = r.left;
			src_r.right = m_pointBmpViewOrg.x;

			if(src_r.left < src_r.right)
				FillRect(m_ptrViewDC, src_r, 
				static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

			src_r.left = m_pointBmpViewOrg.x + 
				grDispatcher::instance()->Get_SizeX()*m_fScale;
			src_r.right = r.right;

			if(src_r.left < src_r.right)
			{
				FillRect(m_ptrViewDC, src_r, 
					static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
			}
		}

		if(m_pointBmpViewOrg.y > 0)
		{
			src_r.top = r.top;
			src_r.bottom = m_pointBmpViewOrg.y;

			src_r.left = r.left;
			src_r.right = r.right;

			if(src_r.top< src_r.bottom)
				FillRect(m_ptrViewDC, src_r, 
				static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

			src_r.top = m_pointBmpViewOrg.y + 
				grDispatcher::instance()->Get_SizeY()*m_fScale;
			src_r.bottom = r.bottom;

			if(src_r.top< src_r.bottom)
				FillRect(m_ptrViewDC, src_r, 
				static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
		}

	}
}

void CChildView::OnDraw(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);

	if(getGameDispatcher()){
		PaintRect(r);
		return;
	}

	grDispatcher::instance()->Fill(0);
	grDispatcher::instance()->Flush(r.left, r.top, 
		r.right - r.left, r.bottom - r.top);
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	RECT& r = dc.m_ps.rcPaint;

	if(getGameDispatcher()){
		PaintRect(r);
		return;
	}
	
	grDispatcher::instance()->Fill(0);
	grDispatcher::instance()->Flush(r.left, r.top, 
										r.right - r.left, r.bottom - r.top);
}

/*!
 	Проверяет, что за координаты содержит объект. Если это экранные, то просто
	рисуем прямоугольник в этой точке. Если - глобальные, то вызывает
	::DrawNetProj(qdCamera const* pcam, 
	Vect3f pos, grDispatcher* const pgrd, int inner_color)
*/
bool CChildView::DrawNetProj(qdGameObject const* ptrObj, int inner_color /*= 0x000000FF*/)
{
	Vect3f const pos = ptrObj->R();
	if(ptrObj->check_flag(QD_OBJ_SCREEN_COORDS_FLAG)){
		grDispatcher::instance()->Rectangle(
			static_cast<short>(pos.x) - (NET_PROJ_SIZE>>1), 
			static_cast<short>(pos.y) - (NET_PROJ_SIZE>>1), 
			NET_PROJ_SIZE, NET_PROJ_SIZE, 
			0x00FF0000, inner_color, GR_FILLED);

		grDispatcher::instance()->Rectangle(
			static_cast<short>(pos.x) - (OBJ_CENTER_SIZE>>1),
			static_cast<short>(pos.y) - (OBJ_CENTER_SIZE>>1), 
			OBJ_CENTER_SIZE, OBJ_CENTER_SIZE, 
			0x00FF0000, 0x00FF0000, GR_FILLED);
	}
	else
	{
		::DrawNetProj(get_camera(), pos, grDispatcher::instance(), inner_color);
	}

	return true;
}

/*!
 	Если идет стыковка состояния, то функция ничего не делает. Иначе рендерит
	контру, отрисовывает границы, и проекцию на сетку
*/
bool CChildView::DrawSelObject()
{
	if(!m_ptrSelObj)
		return true;
	if (m_eViewState != VS_EDIT_STACK_STATE) 
	{
		if (m_ptrSelObj->check_flag(QD_OBJ_DISABLE_MOVEMENT_FLAG))
			m_ptrSelObj->draw_contour(RGB(255, 255, 0));
		else
			m_ptrSelObj->draw_contour(RGB(255,255, 255));
		if(!ptree_helpers::is_obj_static(m_ptrSelObj))
		{
			if (static_cast<qdGameObjectAnimated*>(m_ptrSelObj)->has_bound()) 
				static_cast<qdGameObjectAnimated*>(m_ptrSelObj)->draw_bound();
		}
		DrawNetProj(m_ptrSelObj);
	}

	return true;
}

/*!
	Помогает CChildView::DrawStackedState() отрисовать объекты, участвующие в стыковке.
	\param ptrAnimObj			- Объект, подлежащий отрисовке
	\param pca					- Координатная анимация. Если непустая, то ей 
									выставляют фазу и отрисовывают.Не может быть NULL
	\param ptrAnimInfo			- Анимация, которую будем отрисовывать, если КА пустая
	\param fAnimPhase			- Фаза, в которой будет находится КА или анимация
	\param dir_num				- Номер направления движения
	\param isStaticPartOfGait	- Номер направления движения
	\param cntr_clr				- цвет контура объекта
*/
void CChildView::drw_obj(qdGameObjectAnimated* ptrAnimObj,
						 qdGameObjectState* ptrState,
						 qdCoordsAnimation* pca,
						 qdAnimationInfo*ptrAnimInfo, 
						 float fAnimPhase, 
						 int dir_num,
						 int offs_type,
						 COLORREF cntr_clr)
{
	ASSERT(pca);
	if (ptree_helpers::is_obj_moving(ptrAnimObj)
		&&ptrState&&ptree_helpers::IsMovementState(ptrState))
	{
		//Стыкуем части походки.
		//Устанавливаем правильное направелние, чтобы правильно вычислились
		//экранные координаты
		SetDirection(static_cast<qdGameObjectMoving*>(ptrAnimObj), 
			ptrState, dir_num);

		//! Это остановка или это походка
		switch(offs_type){
		case qdGameObjectStateWalk::OFFSET_STATIC:
			static_cast<qdGameObjectMoving*>(ptrAnimObj)->set_movement_mode(qdGameObjectMoving::MOVEMENT_MODE_STOP);
			ptrAnimObj->drop_flag(QD_OBJ_MOVING_FLAG);
			break;
		case qdGameObjectStateWalk::OFFSET_WALK:
			static_cast<qdGameObjectMoving*>(ptrAnimObj)->set_movement_mode(qdGameObjectMoving::MOVEMENT_MODE_MOVE);
			ptrAnimObj->set_flag(QD_OBJ_MOVING_FLAG);
			break;
		case qdGameObjectStateWalk::OFFSET_START:
			static_cast<qdGameObjectMoving*>(ptrAnimObj)->set_movement_mode(qdGameObjectMoving::MOVEMENT_MODE_START);
			ptrAnimObj->set_flag(QD_OBJ_MOVING_FLAG);
			break;
		case qdGameObjectStateWalk::OFFSET_END:
			static_cast<qdGameObjectMoving*>(ptrAnimObj)->set_movement_mode(qdGameObjectMoving::MOVEMENT_MODE_END);
			ptrAnimObj->drop_flag(QD_OBJ_MOVING_FLAG);
			break;
		}
	}

	if (pca->is_empty()&&ptrAnimInfo) {
		//! Назначаем объекту анимацию, которая будет отрисована
		ptrAnimObj->set_animation_info(ptrAnimInfo);
		//! Устанавливаем фазу
		ptrAnimObj->get_animation()->set_time_rel(fAnimPhase);
	}
	else
		//! Устанавливаем фазу КА
		pca->set_time_rel(fAnimPhase);
	
	if (ptree_helpers::is_obj_mouse(ptrAnimObj))
	{
		CRect r;
		GetClientRect(r);
		scr2log(&r);
		//! если это мышь, то просто ставим в центр окна
		ptrAnimObj->set_pos(Vect3f(r.Width()>>1, r.Height()>>1, 0));
	}

	ptrAnimObj->update_screen_pos();

	ptrAnimObj->redraw();
	ptrAnimObj->draw_contour(cntr_clr);

	if (!pca->is_empty()) {
		DrawCoordsAnimPts(*pca,
			pca->check_flag(QD_COORDS_ANM_LOOP_FLAG),
			get_camera(), cntr_clr);
	}
}

void CChildView::DrawStackedState()
{
	if (m_eViewState != VS_EDIT_STACK_STATE||!m_ptrStackedState) 
		return;
	//! Объект, находящийся снизу. Неподвижный объект. Может быть 0
	qdGameObjectAnimated* ptrBackObj = m_ptrStateStacker->get_back_obj();
	//! Объект, чье состояние стыкуем. Может совпадать с ptrBackObj. НЕ МОЖЕТ БЫТЬ 0
	qdGameObjectAnimated* ptrAnimObj = 
		static_cast<qdGameObjectAnimated*>(m_ptrStackedState->owner());
	COLORREF const BACK_OBJ_BOUND_CLR = RGB(200, 200, 0);

	Vect3f vBackObjDrwPos;//положение, где был отрисован объект-подложка
	if(ptrBackObj)//есть объект, с которым происходит стыковка
	{
		pers_dir_safer __s__(ptrBackObj);//сохраняем направление персонажа
		if (ptrAnimObj == ptrBackObj)
		{
			//если подкладываемое состояние не имеет координатной анимации,
			//а стыкуемое имеет, то переносим подкладываемое в позицию КА
			if (ptrBackObj->get_cur_state()->coords_animation()->is_empty()
				&&!m_ptrStackedState->coords_animation()->is_empty())
			{
				qdCoordsAnimation* const pca = 
					m_ptrStackedState->coords_animation();
				Vect3f v(pca->get_point(pca->get_cur_point())->dest_pos());
				v.z = ptrBackObj->R().z;
				ptrBackObj->set_pos(v);
			}
		}
		//! отрисовываем подложенный объект
		drw_obj(ptrBackObj, 
				m_ptrStateStacker->get_back_obj_state(),
				m_ptrStateStacker->get_back_obj_state()->coords_animation(),
				m_ptrStateStacker->get_back_obj_state_ai(),
				m_ptrStateStacker->get_obj_state_anim_phase(),
				m_ptrStateStacker->back_obj_state_ai_num(),
				m_ptrStateStacker->back_obj_state_type(),
				RGB(255, 255, 0));

		vBackObjDrwPos = ptrBackObj->R();
	}
	

	qdCamera const * const pcam = get_camera();
	int iCurState = ptrAnimObj->cur_state();
	if (ptrAnimObj->get_state(iCurState) != m_ptrStackedState) 
	{
		//стыкуем 2 состояния одного и того же объекта			

		//сохраняем направление
		pers_dir_safer __s__(ptrAnimObj);

		ptrAnimObj->set_state(m_ptrStackedState);

		ASSERT(ptrAnimObj == ptrBackObj);
		//! Если стыкуем 2 состояния одного и того же объекта, то 
		//! необходимо, чтобы объект был скрыт. Для того, чтобы избежать
		//! непрятностей
		ASSERT(ptrAnimObj->check_flag(QD_OBJ_HIDDEN_FLAG));
		

		drw_obj(ptrAnimObj, 
				m_ptrStackedState,
				m_ptrStackedState->coords_animation(),
				m_ptrStateStacker->get_edited_state_ai(),
				m_ptrStateStacker->get_edited_state_anim_phase(), 
				m_ptrStateStacker->edited_state_ai_num(),
				m_ptrStateStacker->edited_state_type(),
				RGB(255, 255, 255));

		DrawNetProj(ptrAnimObj);

		Vect3f const INVALID_BND(-1.f, -1.f, -1.f);
		Vect3f pos = ptrAnimObj->R();
		const bool bDrwBnd = ptrAnimObj->has_bound();
		Vect3f const bnd = bDrwBnd ? ptrAnimObj->bound() : INVALID_BND;
		//! Размеры динамических объектов в независимости от их положения
		//! рисуются на сетке
		if(ptree_helpers::is_obj_animated(ptrAnimObj)&&bDrwBnd)
			pos.z = bnd.z/2.f + pcam->get_grid_center().z;

		//после отрисовки нужного нам состояния вернули предыдущее
		ptrAnimObj->set_state(iCurState);
		//вынес сюда, чтобы рисовать проекцию поверх ptrAnimObj
		if(ptrBackObj)
		{
			::DrawNetProj(pcam, vBackObjDrwPos, 
				grDispatcher::instance(), 0x000000FF);
			if (ptrBackObj->has_bound())
			{
				if(ptrBackObj->named_object_type() == QD_NAMED_OBJECT_ANIMATED_OBJ)
					vBackObjDrwPos.z = 
							ptrBackObj->bound().z/2.f + pcam->get_grid_center().z;
				Draw3dBox(vBackObjDrwPos, ptrBackObj->bound(), 
						pcam, BACK_OBJ_BOUND_CLR);
			}
		}
		if (bDrwBnd)
			Draw3dBox(pos, bnd, pcam, RGB(255, 255, 255));
	}
	else
	{
		pers_dir_safer __s__(ptrAnimObj);
		//в этом случае, мы стыкуем с другим объектом
		drw_obj(ptrAnimObj, 
			m_ptrStackedState,
			m_ptrStackedState->coords_animation(),
			m_ptrStateStacker->get_edited_state_ai(),
			m_ptrStateStacker->get_edited_state_anim_phase(),
			m_ptrStateStacker->edited_state_ai_num(),
			m_ptrStateStacker->edited_state_type(),
			RGB(255, 255, 255));

		//вынес сюда, чтобы рисовать проекцию поверх ptrAnimObj
		if(ptrBackObj)
		{
			if (ptrBackObj->has_bound())
			{
				if(ptrBackObj->named_object_type() == QD_NAMED_OBJECT_ANIMATED_OBJ)
					vBackObjDrwPos.z = 
						ptrBackObj->bound().z/2.f + pcam->get_grid_center().z;
				Draw3dBox(vBackObjDrwPos, 
					ptrBackObj->bound(), pcam, BACK_OBJ_BOUND_CLR);
			}
			DrawNetProj(ptrBackObj);
		}
		//! Отрисовываем границу самого объекта
		if (ptrAnimObj->has_bound()) 
		{
			Vect3f p(ptrAnimObj->R());
			if(ptrAnimObj->named_object_type() == QD_NAMED_OBJECT_ANIMATED_OBJ)
				p.z = ptrAnimObj->bound().z/2.f + pcam->get_grid_center().z;
			Draw3dBox(p,ptrAnimObj->bound(), pcam, RGB(255, 255, 255));
		}
		DrawNetProj(ptrAnimObj);
	}			
}

/*!
 	Рисуется прямо в контекст устройства.
*/
void CChildView::HiliteRect(const CRect& rect)
{
	if (rect.IsRectEmpty()) {
		return;
	}

	int iMode = SetROP2(m_ptrViewDC, R2_NOT);

	HGDIOBJ oBr = SelectObject(m_ptrViewDC, GetStockObject(WHITE_BRUSH));

	::Rectangle(m_ptrViewDC, rect.left, rect.top, rect.right, rect.bottom);
	SelectObject(m_ptrViewDC, oBr);
	SetROP2(m_ptrViewDC, iMode);
}

/*!
 	При передвижении точки мышью вычисляет насколько нужно сдвинуть и сдвигает
	точку
*/
bool CChildView::drag_point(qdGameObject const* pobj, Vect3f &pos, CPoint const& point, 
							UINT nFlags, qdCamera const*ptrCam)
{
	if (pobj->check_flag(QD_OBJ_DISABLE_MOVEMENT_FLAG)) return false;

	int const dx = point.x - m_base_point.x;
	int const dy = point.y - m_base_point.y;

	if(nFlags& MK_CONTROL&&nFlags& MK_SHIFT)
	{
		Vect3f _temp(ptrCam->global2camera_coord(pos));
		Vect2s rscr (ptrCam->camera_coord2rscr(_temp));
		_temp.z -= dy;
		pos = ptrCam->rscr2global(rscr, _temp.z);
		if (ptree_helpers::is_obj_moving(pobj)) {
			const qdGameObjectMoving* const pm = 
				static_cast<const qdGameObjectMoving*>(pobj);
			pos.z = pm->bound().z/2;
		}
		return true;
	}
	else if(nFlags& MK_SHIFT)
	{
		Vect3f posR = pos;
		posR.z = 0;
		
		Vect2s rscr = ptrCam->global2rscr(posR);

		Vect3f t = ptrCam->rscr2plane(rscr);
		
		rscr.x += dx;
		rscr.y -= dy;
		
		t = ptrCam->rscr2plane(rscr) - t;

		pos.x += t.x;
		pos.y += t.y;

		return true;
	}
	return false;
}

BOOL CChildView::GAOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (!get_active_scene())
		return FALSE;
	CPoint point;
	GAFindAnchor(GetLogCurPos(point));
	switch(m_iDraggedAnchor)
	{
	case -1:
		m_hCurrCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		break;
	case 0:
	case 4:
		m_hCurrCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE);
		break;
	case 1:
	case 5:
		m_hCurrCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
		break;
	case 2:
	case 6:
		m_hCurrCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENESW);
		break;
	case 3:
	case 7:
		m_hCurrCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENS);
		break;
	}
	return TRUE;
}

void CChildView::GAOnLButtonUp(UINT nFlags, const CPoint& point)
{
	if(m_iDraggedAnchor != -1)
	{
		GADrawDragRect();

		ReleaseCapture();
		qdGameScene * ptrScene = get_active_scene();
		qdCamera * ptrCam = get_camera(ptrScene);

		scr2log(reinterpret_cast<POINT*>(&m_rDraggedGameArea), 2);
		Vect2i scr_offset;
		scr_offset.x = m_rDraggedGameArea.left 
			+ ((m_rDraggedGameArea.right - m_rDraggedGameArea.left)>>1) - ptrCam->get_scr_center_x();
		scr_offset.y = m_rDraggedGameArea.top
			+ ((m_rDraggedGameArea.bottom - m_rDraggedGameArea.top)>>1) - ptrCam->get_scr_center_y();
		
		if (scr_offset != ptrCam->get_scr_offset()) {

			ptrCam->set_scr_offset(scr_offset);
			ptrCam->set_scr_size(m_rDraggedGameArea.Width(), m_rDraggedGameArea.Height());

			AdjustBitmapSize();

			RedrawScene(ptrScene);
			Invalidate();
			SetProjChange();
		}
	}
	m_iDraggedAnchor = -1;
}

void CChildView::GAOnLButtonDown(UINT nFlags, const CPoint& point)
{
	GAFindAnchor(point);
	if (m_iDraggedAnchor != -1) {

		qdCamera* ptrCam = get_camera();
		_ASSERT(ptrCam);

		int sx = ptrCam->get_scr_sx();
		int sy = ptrCam->get_scr_sy();

		Vect2i scr_offset	= ptrCam->get_scr_offset();
		Vect2i scr_cntr		= ptrCam->get_scr_center();
		//work area center
		Vect2i wa_cntr = scr_cntr + scr_offset;

		m_rDraggedGameArea.left	= wa_cntr.x - (sx>>1);
		m_rDraggedGameArea.right	= wa_cntr.x + (sx>>1);
		
		m_rDraggedGameArea.top		= wa_cntr.y - (sy>>1);
		m_rDraggedGameArea.bottom	= wa_cntr.y + (sy>>1);

		log2scr(reinterpret_cast<POINT*>(&m_rDraggedGameArea), 2);
		
		SetCapture();

		GADrawDragRect();
		m_base_point = point;
	}
	else
	{
		//в этой функции мы будем искть объект
		//если найдем, то все автоматически переключиться
		//в состояние редактирования сцены.Ура!
		ESOnLButtonDown(nFlags, point);
	}
}

void CChildView::GAOnMouseMove(UINT nFlags, CPoint point)
{
	if(nFlags & MK_LBUTTON)
	{
		if (m_iDraggedAnchor == -1) 
			return;
		qdCamera* ptrCam = get_camera();
		
		int dx = (point.x - m_base_point.x);
		int dy = (point.y - m_base_point.y);
		
		//закрашиваем на страом месте.
		GADrawDragRect();
		switch(m_iDraggedAnchor) {
		case 0://левый верхний
			m_rDraggedGameArea.left += dx;
			m_rDraggedGameArea.top += dy;
			break;
		case 1://серединный левый
			m_rDraggedGameArea.left += dx;
			break;
		case 2://нижний левый
			m_rDraggedGameArea.left	+= dx;
			m_rDraggedGameArea.bottom	+= dy;
			break;
		case 3://серединный нижний
			m_rDraggedGameArea.bottom += dy;
			break;
		case 4://правый нижний
			m_rDraggedGameArea.right	+= dx;
			m_rDraggedGameArea.bottom	+= dy;
			break;
		case 5://серединный правый
			m_rDraggedGameArea.right	+= dx;
			break;
		case 6://правый верхний
			m_rDraggedGameArea.right	+= dx;
			m_rDraggedGameArea.top		+= dy;
			break;
		case 7://серединыый верхний
			m_rDraggedGameArea.top		+= dy;
			break;
		}
		//рисуем на новом месте.
		GADrawDragRect();
		
		m_base_point = point;
	}
}

bool GetLastMouseMove(HWND hWnd, UINT nFlags, CPoint& point)
{
	bool bHas = false;
	MSG msg;
	while (PeekMessage(&msg, hWnd,WM_MOUSEMOVE, WM_MOUSEMOVE, PM_NOREMOVE)) {
		if (msg.wParam == nFlags) {
			point.x = GET_X_LPARAM(msg.lParam);
			point.y = GET_Y_LPARAM(msg.lParam);
			PeekMessage(&msg, hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE);
			bHas = true;
		}
		else
			break;
	}
	return bHas;
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (GetFocus() != this&&GetForegroundWindow()==AfxGetMainWnd())
		SetFocus();

	if(m_bCanDragScene||m_eViewState==VS_EDIT_SCREEN_LAYOUT)
	{
		if(nFlags&MK_LBUTTON)
		{
			qdCamera* ptrCam = get_camera();

			int dx = point.x - m_base_point.x;
			int dy = point.y - m_base_point.y;

			scrollbmp(dx, dy);
			UpdateScrollers();
			m_base_point = point;
		}
		CWnd::OnMouseMove(nFlags, point);
		return;
	}

	if (nFlags&MK_SHIFT) 
		GetLastMouseMove(m_hWnd, nFlags, point);

	CPoint scr_point(point);
	scr2log(&point, 1);
	switch (m_eViewState)
	{
		case VS_EDIT_SCENE:
			ESOnMouseMove(nFlags, point);
		break;
		case VS_EDIT_GAME_AREA:
			GAOnMouseMove(nFlags, point);
		break;
		case  VS_EDIT_COORD_ANIM:
			CAOnMouseMove(nFlags, point);
		break;
		case VS_EDIT_MASK:
			SMOnMouseMove(nFlags, point);
		break;
		case VS_EDIT_NET:
			NTOnMouseMove(nFlags, point);
		break;
		case VS_EDIT_ANIMATED:
			EAOnMouseMove(nFlags, point);
		break;
		case VS_EDIT_STACK_STATE:
			SSOnMouseMove(nFlags, point);
		break;
		case VS_EDIT_STATE:
			ESTOnMouseMove(nFlags, point);
		break;
		case VS_EDIT_INVENTORY:
			if (get_active_scene())
				EIOnMouseMove(nFlags, point);
		break;
		case VS_EDIT_INTERFACE:
//			m_ptrIntefEditMode->OnMouseMove(nFlags, point);
			m_ptrCurrentSceneViewMode->OnMouseMove(nFlags, point);
		break;
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CChildView::descale_pts(POINT* pts, int count){
	for(int i = count; --i>= 0; ++pts)
	{
		(*pts).x = static_cast<LONG>((*pts).x/m_fScale);
		(*pts).y = static_cast<LONG>((*pts).y/m_fScale);
	}		
}

void CChildView::scr2log(POINT *pts, int count)
{
	for(int i = count; --i>= 0; ++pts)
	{
		(*pts).x = static_cast<LONG>(((*pts).x - m_pointBmpViewOrg.x)/m_fScale);
		(*pts).y = static_cast<LONG>(((*pts).y - m_pointBmpViewOrg.y)/m_fScale);
	}		
}

void CChildView::scale_pts(POINT* pts, int count){
	for(int i = count; --i>= 0; ++pts)
	{
		(*pts).x = static_cast<LONG>((*pts).x*m_fScale);
		(*pts).y = static_cast<LONG>((*pts).y*m_fScale);
	}		
}

void CChildView::log2scr(POINT *pts, int count)
{
	for(int i = count; --i>= 0; ++pts)
	{
		(*pts).x = static_cast<LONG>((*pts).x*m_fScale) + m_pointBmpViewOrg.x;
		(*pts).y = static_cast<LONG>((*pts).y*m_fScale) + m_pointBmpViewOrg.y;
	}		
}

//////////////////////////////////////////////////////////////////////////
void CChildView::scr2log(CRect* r)
{
	scr2log(reinterpret_cast<POINT*>(r), 2);
}

void CChildView::scale_pts(CRect* r){
	scale_pts(reinterpret_cast<POINT*>(r), 2);
}

void CChildView::descale_pts(CRect* r){
	descale_pts(reinterpret_cast<POINT*>(r), 2);
}

void CChildView::log2scr(CRect* r)
{
	log2scr(reinterpret_cast<POINT*>(r), 2);
}
//////////////////////////////////////////////////////////////////////////

void CChildView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(!getGameDispatcher())
	{
		CWnd ::OnLButtonDown(nFlags, point);
		return;
	}

	static_cast<CSplitterWnd*>(GetParent()->GetParent())->SetActivePane(0, 0, this);

	if((m_bCanDragScene||m_eViewState==VS_EDIT_SCREEN_LAYOUT)&&nFlags&MK_LBUTTON)
	{
		m_base_point = point;

		m_hCurrCursor = m_hDragHandCur;
		SetCursor(m_hCurrCursor);

		SetCapture();
		CWnd ::OnLButtonDown(nFlags, point);
		return;
	}
	CPoint scr_point(point);
	scr2log(&point,1);
	switch(m_eViewState) {
	case VS_EDIT_SCENE:
		if (get_active_scene())
			ESOnLButtonDown(nFlags, point);
		break;
	case VS_EDIT_NET:
		if (get_active_scene())
			NTOnLButtonDown(nFlags, point);
		break;
	case VS_EDIT_GAME_AREA:
		if (get_active_scene())
			GAOnLButtonDown(nFlags, point);
		break;
	case VS_EDIT_COORD_ANIM:
		if (get_active_scene())
			CAOnLButtonDown(nFlags, point);
		break;
	case VS_EDIT_MASK:
		if (get_active_scene())
			SMOnLButtonDown(nFlags, point);
		break;
	case  VS_EDIT_ANIMATED:
		if (get_active_scene())
			EAOnLButtonDown(nFlags, point);
		break;
	case VS_EDIT_STACK_STATE:
		if (get_active_scene())
			SSOnLButtonDown(nFlags, point);
		break;
	case VS_EDIT_STATE:
		if (get_active_scene())
			ESTOnLButtonDown(nFlags, point);
		break;
	case VS_EDIT_INVENTORY:
		if (get_active_scene())
			EIOnLButtonDown(nFlags, point);
		break;
	case VS_EDIT_INTERFACE:
//		m_ptrIntefEditMode->OnLButtonDown(nFlags, point);
		m_ptrCurrentSceneViewMode->OnLButtonDown(nFlags, point);
		break;
	}
	CWnd ::OnLButtonDown(nFlags, point);
}

/*!
 	Выбирается самый ближний по глубине
*/
qdGameObject* CChildView::GetObjectFromPoint(const CPoint& p)
{
#ifndef USE_NEW_PROJ
	CSingleLock __sl(&m_csDrawGuard, TRUE);
#endif // USE_NEW_PROJ
	qdGameObject* __res = NULL;

	qdGameScene* pscene = get_active_scene();
	qdCamera *ptrCam = get_camera(pscene);

	qdGameObjectList::const_iterator _itr = pscene->object_list().begin(),
		_end = pscene->object_list().end();
	
	//ищем среди gameobject
	go_hit_test test_res = std::for_each(_itr, _end, go_hit_test(p));
	
	if(!test_res.empty())
	{
		float min_depth = 9999999;
		NamedObjectVec_t::const_iterator _itr = test_res.get_hit_res()->begin(),
			_end = test_res.get_hit_res()->end();
		for(; _itr != _end; ++_itr)
		{
			qdGameObject * pgo = (qdGameObject*)*_itr;
			Vect3f pos = ptrCam->global2camera_coord(pgo->R());
			if(min_depth > pos.z)
			{
				min_depth = pos.z;
				__res = pgo;
			}
		}
	}
	return __res;
}

void CChildView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if(!getGameDispatcher())
		return;
	if(!get_active_scene())
		return;

	((CSplitterWnd*)GetParent()->GetParent())->SetActivePane(0, 0, this);

	scr2log(&point, 1);
	switch(m_eViewState) {
	case VS_EDIT_SCENE:
		{
			if (get_active_scene())
			{
				qdGameObject* pobj = GetObjectFromPoint(point);

				if(pobj)
				{
//					m_ptrMainFrame->ShowObjectInTree(pobj);
#ifdef USE_NEW_PROJ
					getProject()->ShowObjectInTree(pobj);
					getProject()->ShowCorrespondingMenu(pobj);
#else
					ptree_api::ShowObjectInTree(pobj);
					ptree_api::ShowCorrespondingMenu(pobj);
#endif // USE_NEW_PROJ
				}
			}
		}
		break;
	case VS_EDIT_MASK:
		if (get_active_scene())
			SMOnRButtonUp(nFlags, point);
		break;
	case VS_EDIT_NET:
		if (get_active_scene())
			NTOnRButtonUp(nFlags, point);
		break;
	case VS_EDIT_ANIMATED:
		if (get_active_scene())
			EAOnRButtonUp(nFlags, point);
		break;
	case VS_EDIT_COORD_ANIM:
		if (get_active_scene())
			CAOnRButtonUp(nFlags, point);
		break;
	case VS_EDIT_STATE:
		if (get_active_scene())
			ESTOnRButtonUp(nFlags, point);
		break;
	case VS_EDIT_INVENTORY:
		if (get_active_scene())
			EIOnRButtonUp(nFlags, point);
		break;
	case VS_EDIT_INTERFACE:
//		m_ptrIntefEditMode->OnRButtonUp(nFlags, point);
		m_ptrCurrentSceneViewMode->OnRButtonUp(nFlags, point);
		break;
	}

	CWnd ::OnRButtonUp(nFlags, point);
}

/*!
	\param pscene - Сцена, для которой рисуем проекции
*/
void CChildView::DrawNetProjs(const qdGameScene* pscene){
	const qdGameObjectList& lst = pscene->object_list();
	qdGameObjectList::const_iterator i = lst.begin(), e = lst.end();
	qdCamera* const pcam = get_camera();
	grDispatcher * const pgrd = grDispatcher::instance();
	for(; i != e; ++i)
	{
		qdGameObject const* const pgo = *i;
		if (!pgo->check_flag(QD_OBJ_HIDDEN_FLAG)) 
		{
			if (ptree_helpers::is_animated(pgo))
			{
				qdGameObjectAnimated const * const pobj =
					static_cast<qdGameObjectAnimated const*> (pgo);
				DrawNetProjsForStates(pobj, pcam, 0x000000FF);
				::DrawNetProj(pcam, pobj->default_R(), pgrd, 0x000000FF);
			}
			else
				DrawNetProj(pgo);
		}
	}
}

/*!
 	Для всех динамических и персонажей отрисовывает их размеры и размеры всех состояний,
	которые имеют размеры
*/
void CChildView::DrawBounds(const qdGameScene* pscene){
	const qdGameObjectList& lst = pscene->object_list();
	qdGameObjectList::const_iterator i = lst.begin(), e = lst.end();
	qdCamera* const pcam = get_camera();
	while((i = std::find_if(i, e, ptree_helpers::is_animated)) != e)
	{
		const qdGameObjectAnimated* pa = static_cast<const qdGameObjectAnimated*>(*i++);
		if (pa->check_flag(QD_OBJ_HIDDEN_FLAG)) 
			continue;
		if (pa->has_bound()) 
			pa->draw_bound();
		qdGameObjectStateVector const& v = pa->state_vector();
		qdGameObjectStateVector::const_iterator i = v.begin(), e = v.end();
		for(; i != e; ++i)
		{
			qdGameObjectState* const pstate = *i;
			Vect3f bnd;
			if (pstate->has_bound())
				bnd = pstate->bound();
			else if (pa->check_flag(QD_OBJ_HAS_BOUND_FLAG)) 
				bnd = pa->obj_bound();
			else 
				continue;
			Vect3f pos;
			if (pstate->coords_animation()->is_empty())
				pos = pa->default_R();
			else
				pos = pstate->coords_animation()->get_point(0)->dest_pos();
			if (ptree_helpers::is_obj_moving(pa)) 
				pos.z = bnd.z/2.f + pcam->get_grid_center().z;

			pa->draw_bound(pos, bnd, RGB(255,255, 255));
		}
	}
}

/*!
 	Убеждается, что экран внутреннего интерфейса выбран в диспетчере интерфейса
	и отрисовывает его, опираясь на левый верхний угол сцены
*/
void CChildView::DrawIngameInterface()
{
	qdInterfaceDispatcher* const pdisp = qdInterfaceDispatcher::get_dispatcher();
	ASSERT(pdisp);
	bool bInventory = (m_eViewState == VS_EDIT_INVENTORY);
	if (pdisp->has_ingame_screen(bInventory))
	{
		if (!pdisp->selected_screen()
			||_tcscmp(pdisp->selected_screen()->name(), 
							pdisp->ingame_screen_name(bInventory)))
			pdisp->select_screen(pdisp->ingame_screen_name(bInventory));
		CPoint sceneLeftTop = 
			get_scene_lefttop(get_camera(), WORK_AREA_BOUND_THICKNESS);

		pdisp->redraw(sceneLeftTop.x, sceneLeftTop.y);
	}
}

/*!
	Рендерит сцену заново. И дорисовывает разные феньки.
	\param pscene - Сцена, объекты которой будут отрисованы.(Передается активная сцена)
*/
void CChildView::RedrawScene(qdGameScene* pscene)
{
	ASSERT(pscene);
#ifndef USE_NEW_PROJ
	CSingleLock __sl(&m_csDrawGuard, TRUE);
#endif // USE_NEW_PROJ
	grDispatcher::instance()->Fill(0);
	pscene->redraw();
	getGameDispatcher()->screen_texts_dispatcher().redraw();

	DrawIngameInterface();
	if (m_eViewState != VS_EDIT_INVENTORY)
	{
		DrawWorkAreaBound();
		DrawCoordAnimationPoints();
		DrawGridZoneContours();
		DrawGridZoneContour();
		DrawMask();
		DrawSelObject();
		DrawStackedState();
		DrawVisibleArea();
		if (m_eViewState == VS_EDIT_SCENE) 
		{
			if (m_bDrawNetProjs) 
			{
				DrawNetProjs(pscene);
				if (m_ptrSelObj)
					DrawNetProj(m_ptrSelObj, 0x00FFFFFF);
			}
		}
		if (m_bDrawBounds) 
			DrawBounds(pscene);
		DrawToolTip();
	}
	else{
		DrawInventory();
		DrawVisibleAreaForInventory();
	}

	//рисуем проекцию для активного объекта
	//активный объект - это тот для которого прокручивается анимация
	if (pscene->get_active_object()) 
		DrawNetProj(pscene->get_active_object());

	if (m_eViewState == VS_EDIT_STATE) {
		qdGameObjectAnimated* pobj = 
			static_cast<qdGameObjectAnimated*>(m_ptrEditedState->owner());
		DrawNetProj(pobj);
		if (pobj->has_bound())
			pobj->draw_bound();
	}
}

void CChildView::clear_sel(qdCamera* ptrCam)
{
	if(!ptrCam)
		return;
	ptrCam->reset_all_select();
}

/*!
 	Если состояние VIEW_STATE::VS_EDIT_NET и m_eSubTools == SSTE_NONE
	Выбирает все зоны
*/
void CChildView::SetSubTools(STATE_SUBTOOLS_ENUM ssteVal){
	if(!get_active_scene())
		return;

	m_eSubTools = ssteVal;
	if (m_eViewState == VS_EDIT_NET&&m_eSubTools == SSTE_NONE) {
			GZSelectAll(true);
	}
	
}

/*!
 	Устанавливает режим и делает дополнительные действия в зависимости от 
	рабочего режима
	\param eVal	- Новый рабочий режим
*/
void CChildView::SetViewState(VIEW_STATE eVal)
{
	if (m_eViewState == VS_EDIT_NET) {
		if (m_eSubTools == SSTE_NONE
			||m_eSubTools == SSTE_EDIT_ZONE_AND_SHOW_ALL) {
			GZSelectAll(false);
		}
	}
	m_eViewState = eVal;

	if(m_eViewState == VS_EDIT_NET)
	{
		if(qdCamera* ptrCam = get_camera())
		{
			if(ptrCam->get_redraw_mode() != QDCAM_GRID_NONE)
				ptrCam->set_redraw_mode(QDCAM_GRID_ABOVE);
		}
	}
	else
	{
		if(qdCamera* ptrCam = get_camera())
		{
			if(ptrCam->get_redraw_mode() != QDCAM_GRID_NONE)
				ptrCam->set_redraw_mode(QDCAM_GRID_ZBUFFER);
		}
	}
	if (m_ptrCurrentSceneViewMode)
		m_ptrCurrentSceneViewMode->Deinit();

	if (m_eViewState == VS_EDIT_INTERFACE)
		m_ptrCurrentSceneViewMode = m_ptrIntefEditMode.get();
	else
		m_ptrCurrentSceneViewMode = NULL;

	if (m_ptrCurrentSceneViewMode)
		m_ptrCurrentSceneViewMode->Init(this, NULL);
}

void CChildView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO si;
	if (GetScrollInfo(SB_HORZ, &si, SIF_ALL)) {
		int dx = 0;

		switch(nSBCode) {
		case SB_LINELEFT:
			dx = 1;
			si.nPos -= 1;
			break;
		case SB_LINERIGHT:
			dx = -1;
			si.nPos += 1;
			break;
		case SB_PAGERIGHT:
			dx = si.nPage;
			dx = -dx;
			si.nPos += si.nPage;
			break;
		case SB_PAGELEFT:
			dx = si.nPage;
			si.nPos -= si.nPage;
			break;
		case SB_THUMBTRACK:
			dx = si.nPos - nPos;
			si.nPos = nPos;
			break;
		default:
			return;
		}
		SetScrollInfo(SB_HORZ, &si);
		scrollbmp(dx, 0);
	}
	
	CWnd ::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CChildView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO si;
	if (GetScrollInfo(SB_VERT, &si, SIF_ALL)) {
		int dx = 0;
		
		switch(nSBCode) {
		case SB_LINEUP:
			dx = 1;
			si.nPos -= 1;
			break;
		case SB_LINEDOWN:
			dx = -1;
			si.nPos += 1;
			break;
		case SB_PAGEDOWN:
			dx = si.nPage;
			dx = -dx;
			si.nPos += si.nPage;
			break;
		case SB_PAGEUP:
			dx = si.nPage;
			si.nPos -= si.nPage;
			break;
		case SB_THUMBTRACK:
			dx = si.nPos - nPos;
			si.nPos = nPos;
			break;
		default:
			return;
		}
		SetScrollInfo(SB_VERT, &si);
		scrollbmp(0, dx);
	}
	
	CWnd ::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CChildView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd ::OnSize(nType, cx, cy);
#ifdef USE_NEW_PROJ
	if (project_)
#else
	if(m_ptrMainFrame)
#endif // USE_NEW_PROJ
	{
		AdjustBitmapSize();	
	}
}

void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	qdGameScene* pscene = NULL;
	if(!getGameDispatcher()
		/*||!(pscene = get_active_scene())*/)
	{
		CWnd ::OnLButtonDblClk(nFlags, point);
		return;
	}
	((CSplitterWnd*)GetParent()->GetParent())->SetActivePane(0, 0, this);

	scr2log(&point, 1);
	switch(m_eViewState) {
	case VS_EDIT_SCENE:
		{
			if (get_active_scene())
			{
				qdGameObject* pobj = GetObjectFromPoint(point);
				if (pobj) 
#ifdef USE_NEW_PROJ
					getProject()->ShowProperties(pobj);
#else
					ptree_api::ShowProperties(pobj);
#endif // USE_NEW_PROJ
			}
		}
		break;
	case VS_EDIT_NET:
		if (get_active_scene())
			NTOnLButtonDblClk(nFlags, point);
		break;
	case VS_EDIT_MASK:
		if (get_active_scene())
			SMOnLButtonDblClk(nFlags, point);
		break;
	case VS_EDIT_ANIMATED:
		if (get_active_scene())
			EAOnLButtonDblClk(nFlags, point);
		break;
	case VS_EDIT_INVENTORY:
		if (get_active_scene())
			EIOnLButtonDblClk(nFlags, point);
		break;
	case VS_EDIT_INTERFACE:
		m_ptrCurrentSceneViewMode->OnLButtonDblClk(nFlags, point);
		//		m_ptrIntefEditMode->OnLButtonDblClk(nFlags, point);
		break;
	}
	CWnd ::OnLButtonDblClk(nFlags, point);
}


/************************************************************************/
/* Функции работы с зонами на сетке						                */
/************************************************************************/
inline const Vect2s GZContourPoint2Scr(qdCamera* ptrCam, const Vect2s& cp)
{
	return ptrCam->plane2scr(ptrCam->get_cell_coords(cp));
}

inline const Vect2s GZScr2ContourPoint(qdCamera* ptrCam, const Vect2s& cp, 
									   bool bcrop = true)
{
	return ptrCam->get_cell_index(ptrCam->scr2plane(cp), bcrop);
}

/*
 *	обработчики системынх событий
 */
BOOL CChildView::NTOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (!get_active_scene())
		return FALSE;

	if(get_camera()->get_redraw_mode()==QDCAM_GRID_NONE)
	{
		m_hCurrCursor = m_hCrossCursor;
		return TRUE;
	}
	if (m_eSubTools == SSTE_NONE) {
		m_hCurrCursor = m_hCrossCursor;
	}
	else
	{
		if (m_iActivePoint != -1
			&&IsKeyPressed(VK_SHIFT)
			&&IsKeyPressed(VK_LBUTTON)) 
		{
			m_hCurrCursor = m_hDragHandCur;
		}
		else
		{
			if (m_iActivePoint!=-1
				&&GZFindPoint(GZScr2ContourPoint(get_camera(), 
				GetLogCurPos()), m_ptrGZEdited)==m_iActivePoint) {
				if (IsKeyPressed(VK_SHIFT)) {
					m_hCurrCursor = m_hHandCur;
				}
			}
			else
				m_hCurrCursor = m_hPenCursor;
		}
	}
	return TRUE;
}

void CChildView::NTOnLButtonDown(UINT nFlags, CPoint point)
{
	qdCamera* pcam = get_camera();
		
	bool bInvalidate = false;
	
	Vect3f planePoint = pcam->scr2plane(Vect2s(point.x, point.y));
	
	Vect2s vGridIndexs = pcam->get_cell_index(planePoint, false);
	bool const bPointInNet = PointInNet(pcam, planePoint);
	
	switch(m_eSubTools) {
	case SSTE_NONE:
		{
			qdGridZone* ptrZone = 
				GZFindZoneByCntrPnt(vGridIndexs, m_ptrGZEdited);
			if (!ptrZone) 
				ptrZone = GZFindZone(planePoint, m_ptrGZEdited);
			if(ptrZone)
			{
#ifdef USE_NEW_PROJ
				getProject()->ShowObjectInTree(ptrZone);
#else
				ptree_api::ShowObjectInTree(ptrZone);
#endif // USE_NEW_PROJ
				m_iActivePoint = GZFindPoint(vGridIndexs, ptrZone);
				bInvalidate = true;
			}
		}
		break;
	case SSTE_EDIT_ZONE:
		{
			HiliteRect(m_rectHilited);
			m_rectHilited.SetRectEmpty();

			m_iActivePoint = GZFindPoint(vGridIndexs, m_ptrGZEdited);
			if (m_iActivePoint == -1&&bPointInNet) {
				if (m_bConturClosed) {
					if (!GZInsertPoint(planePoint)) {//не смогли вставить точку
						//пытаемся найти зону отличную от текущей
						qdGridZone* ptrZone = GZFindZone(planePoint, m_ptrGZEdited);
						if(ptrZone)
#ifdef USE_NEW_PROJ
							getProject()->ShowObjectInTree(ptrZone);
#else
							ptree_api::ShowObjectInTree(ptrZone);
#endif // USE_NEW_PROJ
					}
					else
						bInvalidate = true;
				}
				else//if (m_bConturClosed) 
				{
					m_iActivePoint = GZAddPoint(vGridIndexs);
					bInvalidate = true;
				}
			}
			else
				bInvalidate = true;
			m_base_point = point;
		}
		break;
	case SSTE_CREATE_ZONE: 
		{
			HiliteRect(m_rectHilited);
			m_rectHilited.SetRectEmpty();
			
			m_iActivePoint = GZFindPoint(vGridIndexs, m_ptrGZEdited);
			if (m_iActivePoint == -1&&bPointInNet) 
				m_iActivePoint = GZAddPoint(vGridIndexs);
			m_base_point = point;
			bInvalidate = true;
		}
		break;
	case SSTE_EDIT_ZONE_AND_SHOW_ALL:
		{
			HiliteRect(m_rectHilited);
			m_rectHilited.SetRectEmpty();
			
			m_iActivePoint = GZFindPoint(vGridIndexs, m_ptrGZEdited);
			if (m_iActivePoint == -1&&bPointInNet) {
				if (m_bConturClosed) {
					if (!GZInsertPoint(planePoint)) {
						//не смогли вставить точку
						//пытаемся найти зону отличную от текущей
						qdGridZone* ptrZone = 
							GZFindZoneByCntrPnt(vGridIndexs, m_ptrGZEdited);
						if (!ptrZone) {
							ptrZone = GZFindZone(planePoint, m_ptrGZEdited);
						}
						if(ptrZone)
						{
							ptree_api::ShowObjectInTree(ptrZone);
//							OnEditGridZone(ptrZone);
							m_eSubTools = SSTE_EDIT_ZONE_AND_SHOW_ALL;
							m_iActivePoint = GZFindPoint(vGridIndexs, ptrZone);
							bInvalidate = true;
						}
					}
					else
						bInvalidate = true;
				}
				else//if (m_bConturClosed)
				{
					m_iActivePoint = GZAddPoint(vGridIndexs);
					bInvalidate = true;
				}
			}
			else
				bInvalidate = true;
			m_base_point = point;
		}
	}	
	if (bInvalidate) {
		RedrawScene(get_active_scene());
		Invalidate(FALSE);
	}
}

void CChildView::NTOnMouseMove(UINT nFlags, const CPoint& point)
{
	qdCamera* ptrCam = get_camera();
	if(m_iActivePoint != -1&&(nFlags&MK_SHIFT)
		&&(nFlags&MK_LBUTTON))
	{
		int dx = point.x - m_base_point.x;
		int dy = point.y - m_base_point.y;
		
		Vect2s scr_point = 
			GZContourPoint2Scr(ptrCam, m_ptrGZEdited->get_point(m_iActivePoint));

		Vect2s scr_prev = scr_point;
		if (m_iActivePoint) 
			scr_prev = 
				GZContourPoint2Scr(ptrCam, m_ptrGZEdited->get_point(m_iActivePoint-1));
		else if (m_bConturClosed) 
			scr_prev = GZContourPoint2Scr(ptrCam, 
							m_ptrGZEdited->get_point(m_ptrGZEdited->contour_size()-1));
		
		Vect2s scr_next = scr_point;
		if (m_iActivePoint != m_ptrGZEdited->contour_size()-1) 
			scr_next = 
				GZContourPoint2Scr(ptrCam, m_ptrGZEdited->get_point(m_iActivePoint+1));
		else if (m_bConturClosed) 
			scr_next = GZContourPoint2Scr(ptrCam, m_ptrGZEdited->get_point(0));
		
		CRect rectScr(scr_prev.x, scr_prev.y, scr_next.x, scr_next.y);
		rectScr.NormalizeRect();
		rectScr.InflateRect((GZ_POINT_WIDTH>>1), (GZ_POINT_HEIGHT>>1), 
							(GZ_POINT_WIDTH>>1), (GZ_POINT_HEIGHT>>1));
		
		CRect rectFlush(scr_point.x - (GZ_POINT_WIDTH>>1),
			scr_point.y - (GZ_POINT_HEIGHT>>1),
			scr_point.x + (GZ_POINT_WIDTH>>1)	, 
			scr_point.y + (GZ_POINT_HEIGHT>>1));
		
		scr_point.x += dx;
		scr_point.y += dy;
		
		Vect2s newIdx = GZScr2ContourPoint(ptrCam, scr_point);
		if (!(newIdx == Vect2s(-1, -1))) {
			if (!(newIdx == m_ptrGZEdited->get_point(m_iActivePoint))) {
				m_bWorkChanged = true;
				m_ptrGZEdited->update_contour_point(newIdx, m_iActivePoint);
			
				scr_point = GZContourPoint2Scr(ptrCam, newIdx);
				CRect r = CRect(scr_point.x - (GZ_POINT_WIDTH>>1),
					scr_point.y - (GZ_POINT_HEIGHT>>1),
					scr_point.x + (GZ_POINT_WIDTH>>1), 
					scr_point.y + (GZ_POINT_HEIGHT>>1));
				
				
				rectFlush.UnionRect(&rectFlush, &r);
				rectFlush.UnionRect(&rectFlush, &rectScr);
				
				RedrawScene(get_active_scene());
				
				r = rectFlush;
				log2scr(reinterpret_cast<POINT*>(&r), 2);
				grDispatcher::instance()->StretchFlush(
					r.left, r.top, r.Width(), r.Height(),
					rectFlush.left, rectFlush.top, 
					rectFlush.Width(), rectFlush.Height());
			}
		}
		
		scr_point = GZContourPoint2Scr(ptrCam, m_ptrGZEdited->get_point(m_iActivePoint));
		m_base_point.x = scr_point.x;
		m_base_point.y = scr_point.y;
	}
	else
	{
		HiliteRect(m_rectHilited);
		if(m_ptrGZEdited)
		{
			int iPoint = GZFindPoint(GZScr2ContourPoint(ptrCam, 
							Pnt2V2s(point), false), m_ptrGZEdited);
			if (iPoint != -1) {
				CPoint scr_point = V2s2Pnt(GZContourPoint2Scr(ptrCam, 
					m_ptrGZEdited->get_point(iPoint)));

				m_rectHilited = CRect(scr_point.x - (GZ_POINT_WIDTH>>1),
					scr_point.y - (GZ_POINT_HEIGHT>>1),
					scr_point.x + (GZ_POINT_WIDTH>>1), 
					scr_point.y + (GZ_POINT_HEIGHT>>1));

				log2scr(reinterpret_cast<POINT*>(&m_rectHilited), 2);
				HiliteRect(m_rectHilited);
			}
			else
				m_rectHilited.SetRectEmpty();
		}
		else
			m_rectHilited.SetRectEmpty();
	}
}

void CChildView::NTOnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bConturClosed&&m_iActivePoint != -1&&m_bWorkChanged) {
		GZFillCells(m_ptrGZEdited, get_camera());
		GZSaveWork();
		RedrawScene(get_active_scene());
		Invalidate(FALSE);
	}
}

void CChildView::NTOnLButtonDblClk(UINT nFlags, CPoint point)
{
	qdCamera* ptrCam = get_camera();
	if (m_eSubTools == SSTE_EDIT_ZONE) {

		if (m_ptrGZEdited) 
#ifdef USE_NEW_PROJ
			getProject()->ShowProperties(m_ptrGZEdited);
#else
			ptree_api::ShowProperties(m_ptrGZEdited);
#endif // USE_NEW_PROJ
	}
}

void CChildView::NTOnRButtonUp(UINT nFlags, const CPoint& point)
{
	qdCamera* ptrCam = get_camera();
	switch (m_eSubTools)
	{
	case SSTE_CREATE_ZONE:
		{
			if (m_ptrGZEdited->can_be_closed()) {
				m_bConturClosed = !m_bConturClosed;
				if (m_bConturClosed) {
					//создаём/изменяем зону
					GZFillCells(m_ptrGZEdited, ptrCam);
					GZSaveWork();
				}
			}
		}
		break;
	case SSTE_EDIT_ZONE:
	case SSTE_EDIT_ZONE_AND_SHOW_ALL:
		{
			if (m_bConturClosed) {
				m_bConturClosed = false;
			}
			else if (m_ptrGZEdited->can_be_closed()) {
				m_bConturClosed = true;
				if (m_bWorkChanged) {
					GZFillCells(m_ptrGZEdited, ptrCam);
					GZSaveWork();
				}
			}
		}
		break;
	};
	Redraw(RM_REDRAW_CONTENT);
}

void CChildView::NTOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//если сетку не видно, то ничего не делаем
//	if(get_camera()->get_redraw_mode()==QDCAM_GRID_NONE)
//		return;

	bool bInvalidate = false;
	if(nChar == VK_BACK)
	{
		if (m_bConturClosed||m_ptrGZEdited->get_contour().empty())
			//нельзя удалить крайнюю, если замкнут или пуст
			PlayError();
		else 
		{
			m_bWorkChanged = true;
			m_ptrGZEdited->remove_contour_point(m_ptrGZEdited->contour_size()-1);

			if (m_iActivePoint >= m_ptrGZEdited->contour_size()) 
				m_iActivePoint = m_ptrGZEdited->contour_size()-1;

			ASSERT(!m_bConturClosed);
			bInvalidate = true;

			m_rectHilited.SetRectEmpty();
		}
	}

	if (nChar == VK_DELETE) {
		if(m_iActivePoint == -1||
			(m_bConturClosed&&m_ptrGZEdited->contour_size()==3))
			//замкнутый контур не может содержать меньше 3х точек
			PlayError();
		else
		{
			m_bWorkChanged = true;
			m_ptrGZEdited->remove_contour_point(m_iActivePoint);

			if (m_iActivePoint == m_ptrGZEdited->contour_size()) 
				--m_iActivePoint;

			m_rectHilited.SetRectEmpty();

			if (m_bConturClosed) {
				GZFillCells(m_ptrGZEdited, get_camera());
				GZSaveWork();
			}
			bInvalidate = true;
		}
	}
	if(nChar == VK_ESCAPE)
	{
		bInvalidate = true;
		GZKillWork();

		m_rectHilited.SetRectEmpty();
	}
	
	if(nChar == VK_RETURN)
	{
		if (m_bConturClosed) {
			GZFillCells(m_ptrGZEdited, get_camera());
			GZSaveWork();
		}
		else
			PlayError();
		bInvalidate = true;
	}
	if(bInvalidate)
	{
		RedrawScene(get_active_scene());
		Invalidate(FALSE);
	}
}
/*
*	остальные
*/

bool CChildView::GZSelectAll(bool bSelect)
{
	qdGameScene* ptrScene = get_active_scene();
	if (!ptrScene)
		return false;
	const qdGridZoneList& gzl = ptrScene->grid_zone_list();
	typedef bool (qdGridZone::*loc_sel)(bool) const;
	std::for_each(gzl.begin(), gzl.end(), 
		std::bind2nd(
			std::mem_fun(static_cast<loc_sel>(qdGridZone::select)), bSelect));
	return true;
}

void CChildView::DrawGridZoneContours()
{
	qdGameScene* ptrScene = get_active_scene();
	ASSERT(ptrScene);

	std::for_each(ptrScene->grid_zone_list().begin(), 
		ptrScene->grid_zone_list().end(), draw_contours(this, m_ptrGZEdited));
}

/*!
 	Пробегает по всем точкам контура и отрисовывает их
	\param gc			- контур секти
	\param bClosed		- замкнут или нет
	\param iActivePoint	- активная точка
	\param bDrwPoints	- рисовать точки или только линии контура
	\param line_clr		- цвет линии контура
*/
void CChildView::DrawGridZoneContour(const grid_contour_t& gc, bool bClosed, 
										int iActivePoint, bool bDrwPoints,
										COLORREF line_clr)
{
	if (gc.empty()) 
		return;

	grid_contour_t::const_iterator _beg = gc.begin(), _end = gc.end();
	
	qdCamera *pcam = get_camera();
	Vect2s scr_pos = GZContourPoint2Scr(pcam, *_beg);
	if (bDrwPoints) {
		if(iActivePoint == 0)
		{
			grDispatcher::instance()->
				Rectangle(scr_pos.x - (GZ_POINT_WIDTH>>1), 
				scr_pos.y - (GZ_POINT_HEIGHT>>1),
				GZ_POINT_WIDTH, GZ_POINT_HEIGHT,
				RGB(255, 255, 255), GZ_CUR_POINT_COLOR, GR_FILLED);
		}
		else
		{
			grDispatcher::instance()->
				Rectangle(scr_pos.x - (GZ_POINT_WIDTH>>1), 
				scr_pos.y - (GZ_POINT_HEIGHT>>1),
				GZ_POINT_WIDTH, GZ_POINT_HEIGHT,
				RGB(255, 255, 255), GZ_POINT_COLOR, GR_FILLED);
		}
	}
	
	int i = 1;
	for(++_beg; _beg != _end; ++_beg, ++i)
	{
		Vect2s scr_pos1(GZContourPoint2Scr(pcam, *_beg));
		grDispatcher::instance()->
			Line(scr_pos.x, scr_pos.y, scr_pos1.x, scr_pos1.y,line_clr, 4, true);

		if (bDrwPoints) {
			if(iActivePoint == i)
			{
				grDispatcher::instance()->
					Rectangle(scr_pos1.x - (GZ_POINT_WIDTH>>1), 
					scr_pos1.y - (GZ_POINT_HEIGHT>>1),
					GZ_POINT_WIDTH, GZ_POINT_HEIGHT,
					RGB(255, 255, 255), GZ_CUR_POINT_COLOR, GR_FILLED);
			}
			else
			{
				grDispatcher::instance()->
					Rectangle(scr_pos1.x - (GZ_POINT_WIDTH>>1), 
					scr_pos1.y - (GZ_POINT_HEIGHT>>1),
					GZ_POINT_WIDTH, GZ_POINT_HEIGHT,
					RGB(255, 255, 255), GZ_POINT_COLOR, GR_FILLED);
			}
		}
		scr_pos = scr_pos1;
	}
	if (bClosed) {
		Vect2s scr_pos1(GZContourPoint2Scr(pcam, *gc.begin()));
		grDispatcher::instance()->
			Line(scr_pos.x, scr_pos.y, scr_pos1.x, scr_pos1.y, line_clr, 4, true);
	}
}

/*!
 	Двигаяся по маске зоны отрисовывает сначала вертикальню часть контура, 
	а затем, во-второй проход, горизонтальную
*/
void CChildView::DrawGridZoneContour2(const qdGridZone* ptrZone, COLORREF line_clr){

	if (ptrZone->is_mask_empty())
		return;

	const Vect2s& size = ptrZone->mask_size();
	const Vect2s& mask_pos = ptrZone->mask_pos();
	
	const Vect2s half_size(size.x>>1, size.y>>1);

	bool bIn = false;
	qdCamera* pcam = get_camera();
	Vect2s cell_index(0, 0);

	Vect2s pos = ptrZone->mask_pos();
	pos.x -= ptrZone->mask_size().x/2;
	pos.y -= ptrZone->mask_size().y/2;

	for(short y = 0; y < size.y; ++y){
		for(short x = 0; x < size.x; ++x){
			const BYTE& val = ptrZone->is_inside(pos + Vect2s(x,y));
			if ((bIn&&!val)||(!bIn&&val)){
				bIn = !bIn;
				cell_index.x = x;
				cell_index.y = y;
				cell_index += mask_pos - half_size;
				sv_helpers::DrawXPart(pcam, cell_index, line_clr);
			}
		}
		if(bIn)//дошли до конца, но не нашли ячейку не входщую в маску
		{
			bIn = false;//мы же всетаки выходим за границу
			//пририсовываем херню последней
			
			cell_index.x = size.x;
			cell_index.y = y;
			cell_index += mask_pos - half_size;

			sv_helpers::DrawXPart(pcam, cell_index, line_clr);
		}
	}

	for(short x = 0; x < size.x; ++x){
		for(short y = 0; y < size.y; ++y){
			const BYTE& val = ptrZone->is_inside(pos + Vect2s(x,y));
			if ((bIn&&!val)||(!bIn&&val)){
				bIn = !bIn;
				cell_index.x = x;
				cell_index.y = y;
				cell_index += mask_pos - half_size;
				sv_helpers::DrawYPart(pcam, cell_index, line_clr);
			}
		}
		if(bIn)//дошли до конца, но не нашли ячейку не входщую в маску
		{
			bIn = false;//мы же всетаки выходим за границу
			//пририсовываем херню последней

			cell_index.x = x;
			cell_index.y = size.y;
			cell_index += mask_pos - half_size;
			sv_helpers::DrawYPart(pcam, cell_index,line_clr);			
		}
	}
}

/*!
 	Проверяет нужно ли рисовать контур зоны, если нужно, то вызывает
	CChildView::DrawGridZoneContour (const grid_contour_t& gc, bool bClosed, 
	int iActivePoint, bool bDrwPoints, COLORREF line_clr = GZ_LINE_COLOR)
	и CChildView::DrawGridZoneContour2(const qdGridZone* ptrZone, COLORREF line_clr)
*/
void CChildView::DrawGridZoneContour()
{
	if (m_eViewState != VS_EDIT_NET
		||!m_ptrGZEdited
		||m_ptrGZEdited->check_flag(QD_OBJ_HIDDEN_FLAG)
		) {
		return;
	}
	DrawGridZoneContour(m_ptrGZEdited->get_contour(), m_bConturClosed, 
													m_iActivePoint, true, 
													GZ_EDITED_ZONE_LINE_COLOR);
	DrawGridZoneContour2(m_ptrGZEdited, GZ_EDITED_ZONE_LINE_COLOR);
}

bool CChildView::GZInsertPoint(const Vect3f& planePoint)
{
	qdCamera* ptrCam = get_camera();
	std::auto_ptr<Vect3fVect_t> contour = 
		GZIndexes2Plane(m_ptrGZEdited->get_contour(), ptrCam);
	int iSide = GetNearestSide(planePoint, *contour.get(), GZ_MAX_DIST_VALUE);
	if (iSide != -1) {
		m_ptrGZEdited->insert_contour_point(
								ptrCam->get_cell_index(planePoint), iSide + 1);
		m_iActivePoint = iSide + 1;
		m_bWorkChanged = true;
		return true;
	}
	return false;
}


qdGridZone* CChildView::GZCreate()
{
	std::auto_ptr<qdGridZone> ptrZone(new qdGridZone);
	
	if(!ptrZone.get())
		return NULL;
	
	return ptrZone.release();
}

/*!
 	Создает зону. Устанавливает в false CChildView::m_bWorkChanged, 
	CChildView::m_bConturClosed.
	Вызывается из дерева проекта
*/
void CChildView::OnCreateGridZone()
{
	m_ptrGZEdited = GZCreate();
	m_ptrGZEdited->set_state(true);
	//пока не добавили ни одной точки к контуру,
	//не будем волноваться по поводу сохранения результата

	m_bWorkChanged = false;
	m_bConturClosed = false;

	SetFocus();
}

/*!
 	\param ptrZone - зона, которую будем редактировать
*/
void CChildView::OnEditGridZone(qdGridZone* ptrZone, IWorkOwner* workOwner)
{
	workOwner_ = workOwner;
	m_ptrGZEdited = ptrZone;
	m_bWorkChanged = false;
	m_bConturClosed = true;
	if (m_iActivePoint == -1) 
		m_iActivePoint = 0;

	m_ptrGZEdited->select(get_camera(), true);
	SetFocus();
}

/*!
	Вызывается из класса дерева проекта
	\param bSave - сохранить ли сделанные изменения
*/
void CChildView::OnAbortGridZoneEdit(bool bSave)
{

	GZAbortEdit(bSave);
}

void CChildView::GZAbortEdit(bool bSave)
{
	if (bSave) 
		GZSaveWork();

	if (m_ptrGZEdited) 
		m_ptrGZEdited->select(get_camera(), false);

	if(m_eSubTools == SSTE_CREATE_ZONE)
		delete m_ptrGZEdited;

	m_ptrGZEdited = NULL;
	m_bWorkChanged = false;
	m_bConturClosed = false;
	m_iActivePoint = -1;
	workOwner_ = NULL;
	m_rectHilited.SetRectEmpty();
}

void CChildView::GZSaveWork()
{
	ASSERT(m_bConturClosed);
	if (m_bWorkChanged) {

		m_bWorkChanged = false;
		
		if(m_eSubTools == SSTE_CREATE_ZONE)
		{
			qdGridZone* ptrZone = m_ptrGZEdited;
			m_ptrGZEdited = NULL;
#ifdef USE_NEW_PROJ
			if(!getProject()->OnCompleteCreateZone(ptrZone))
#else
			if(!m_ptrMainFrame->OnCompleteCreateZone(ptrZone))
#endif // USE_NEW_PROJ
			{
				//если не удалось создать, то возвращаем все на места
				m_bWorkChanged = true;
				m_ptrGZEdited = ptrZone;
			}
			else
			{
				static_cast<CFrameWnd*>(AfxGetMainWnd())->
					SetMessageText(CString((LPTSTR)IDS_EDIT_WORK_SAVED_SUCC));
				if (m_ptrGZEdited->state()) {
					m_ptrGZEdited->set_state(true);
				}
			}
		}
		else
		{
#ifdef USE_NEW_PROJ
//			if (getProject()->OnSaveGZChanges(true)) {
			if (workOwner_->onSave(true)) { 
#else
			if (m_ptrMainFrame->OnSaveGZChanges(true)) {
#endif // USE_NEW_PROJ
				static_cast<CFrameWnd*>(AfxGetMainWnd())->
					SetMessageText(CString((LPTSTR)IDS_EDIT_WORK_SAVED_SUCC));
				SetProjChange();
			}
		}
	}
	else
		PlayError();
}

void CChildView::GZKillWork()
{
	if (m_bWorkChanged)
	{
		if(m_eSubTools == SSTE_CREATE_ZONE)
		{
			*m_ptrGZEdited = qdGridZone();
			m_bConturClosed = false;
		}
		else if (m_eSubTools == SSTE_EDIT_ZONE) {
			m_ptrGZEdited->select(get_camera(), false);

			bool bState = m_ptrGZEdited->state();
			if (bState) 
				m_ptrGZEdited->set_state(false);
#ifdef USE_NEW_PROJ
//			getProject()->OnSaveGZChanges(false);
			workOwner_->onSave(false);
#else
			m_ptrMainFrame->OnSaveGZChanges(false);
#endif // USE_NEW_PROJ
			
			if (bState) 
				m_ptrGZEdited->set_state(true);
			m_ptrGZEdited->select(get_camera(), true);
		}
		m_bWorkChanged = false;
	}
}

qdGridZone* CChildView::GZFindZoneByCntrPnt(const Vect2s&vPoint,
											const qdGridZone* ptrExcludeZone)
{
	qdGameScene * ptrScene = get_active_scene();
	const qdGridZoneList& gzl = ptrScene->grid_zone_list();
	qdGridZoneList::const_iterator _itr = gzl.begin(), _end = gzl.end();
	for(; _itr != _end; ++_itr)
	{
		qdGridZone* ptrZone = *_itr;
		if(!ptrZone->check_flag(QD_OBJ_HIDDEN_FLAG)
			&&(GZFindPoint(vPoint, ptrZone) != -1)
			&&(!ptrExcludeZone || strcmp(ptrZone->name(), ptrExcludeZone->name())))
			return ptrZone;
	}
	return NULL;
}

qdGridZone* CChildView::GZFindZone(const Vect3f& vPoint, const qdGridZone* ptrExcludeZone)
{
	qdGameScene * ptrScene = get_active_scene();
	const qdGridZoneList& gzl = ptrScene->grid_zone_list();
	qdGridZoneList::const_iterator _itr = gzl.begin(), _end = gzl.end();
	for(; _itr != _end; ++_itr)
	{
		qdGridZone* ptrZone = *_itr;
		if(!ptrZone->check_flag(QD_OBJ_HIDDEN_FLAG)
			&&ptrZone->is_point_in_zone(vPoint)
			&&(!ptrExcludeZone ||strcmp(ptrZone->name(), ptrExcludeZone->name())))
			return ptrZone;
	}
	return NULL;
}

int CChildView::GZFindPoint(const Vect2s& vPoint, qdGridZone* ptrZone)
{
	const grid_contour_t& _cont = ptrZone->get_contour();

	grid_contour_t::const_iterator _res = 
		std::find_if(_cont.begin(), _cont.end(), Vect2s_point_ht(vPoint, 0));
	if (_res == _cont.end()) 
		return -1;
	return std::distance(_cont.begin(), _res);
}

int CChildView::GZAddPoint(const Vect2s& vPoint)
{
	m_bWorkChanged = true;
	m_ptrGZEdited->insert_contour_point(vPoint, m_ptrGZEdited->contour_size());
	return m_ptrGZEdited->contour_size()-1;
}

//////////////////////////////////////////////////////////////////////////

void CChildView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_eViewState != VS_EDIT_SCREEN_LAYOUT) {
		if(m_bCanDragScene&&nChar == VK_CONTROL)
		{
			m_bCanDragScene = false;
			m_hCurrCursor = m_hPrevCursor;
			SetCursor(m_hCurrCursor);
			ReleaseCapture();
		}
		else if(!m_bCanDragScene&&nChar == VK_SHIFT)
		{
			if(IsKeyPressed(VK_CONTROL)
				&&!IsKeyPressed(VK_LBUTTON)
				&&!IsKeyPressed(VK_RBUTTON))
			{
				m_bCanDragScene = true;
				m_hPrevCursor = m_hCurrCursor;
				m_hCurrCursor = m_hHandCur;
				SetCursor(m_hCurrCursor);
			}
		}
	}
	switch(m_eViewState) {
	case VS_EDIT_SCENE:
		if (get_active_scene())
			ESOnKeyUp(nChar, nRepCnt, nFlags);
		break;
	case VS_EDIT_COORD_ANIM:
		CAOnKeyUp(nChar, nRepCnt, nFlags);
		break;
	case VS_EDIT_STACK_STATE:
		SSOnKeyUp(nChar, nRepCnt, nFlags);
		break;
	case VS_EDIT_INTERFACE:
		//m_ptrIntefEditMode->OnKeyUp(nChar, nRepCnt, nFlags);
		m_ptrCurrentSceneViewMode->OnKeyUp(nChar, nRepCnt, nFlags);
		break;
	}
	CWnd ::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	qdGameScene* pscene = NULL;
	if(!getGameDispatcher()
		||!(pscene = get_active_scene()))
	{
		CWnd ::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	if(m_eViewState != VS_EDIT_SCREEN_LAYOUT)
	{
		if(!m_bCanDragScene&&nChar == VK_CONTROL)
		{
			if(!IsKeyPressed(VK_SHIFT)
			&&!IsKeyPressed(VK_LBUTTON)
			&&!IsKeyPressed(VK_RBUTTON))
			{
				m_bCanDragScene = true;
				m_hPrevCursor = m_hCurrCursor;
				m_hCurrCursor = m_hHandCur;
				SetCursor(m_hCurrCursor);
			}
		}
		else if(m_bCanDragScene&&nChar == VK_SHIFT)
		{
			m_bCanDragScene = false;
			m_hCurrCursor = m_hPrevCursor;
			SetCursor(m_hCurrCursor);
			ReleaseCapture();
		}
	}

	if (m_eViewState == VS_EDIT_SCENE) {
		ESOnKeyDown(nChar, nRepCnt, nFlags);
	}
	else if(m_eViewState == VS_EDIT_NET)
	{
		NTOnKeyDown(nChar, nRepCnt, nFlags);
	}
	else if(m_eViewState == VS_EDIT_COORD_ANIM)
	{
		CAOnKeyDown(nChar, nRepCnt, nFlags);
	}
	else if (m_eViewState == VS_EDIT_MASK) {
		SMOnKeyDown(nChar, nRepCnt, nFlags);
	}
	else if (m_eViewState == VS_EDIT_ANIMATED) {
		EAOnKeyDown(nChar, nRepCnt, nFlags);
	}
	else if (m_eViewState == VS_EDIT_STATE) {
		ESTOnKeyDown(nChar, nRepCnt, nFlags);
	}
	else if (m_eViewState == VS_EDIT_INVENTORY){
		EIOnKeyDown(nChar, nRepCnt, nFlags);
	}
	else if (m_eViewState == VS_EDIT_STACK_STATE){
		SSOnKeyDown(nChar, nRepCnt, nFlags);
	}
	else if (m_eViewState == VS_EDIT_INTERFACE)
	{
//		m_ptrIntefEditMode->OnKeyDown(nChar, nRepCnt, nFlags);
		m_ptrCurrentSceneViewMode->OnKeyDown(nChar, nRepCnt, nFlags);
	}
	CWnd ::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CChildView::SelectInventory(qdInventory* ptrInv)
{
#ifndef USE_NEW_PROJ
	CSingleLock __sl(&m_csDrawGuard, TRUE);
#endif // USE_NEW_PROJ
	m_ptrActiveInventory = ptrInv;
}

void CChildView::SetActiveState(qdGameObjectState* pstate){
	m_ptrActiveState = pstate;
}

void CChildView::SelectObj(qdGameObject* ptrObj)
{
#ifndef USE_NEW_PROJ
	CSingleLock __sl(&m_csDrawGuard, TRUE);
#endif // USE_NEW_PROJ
	m_ptrSelObj = ptrObj;
	m_bSelRectChanged = true;
	if (ptrObj&&ptree_helpers::is_obj_moving(ptrObj)) 
	{
		if (qdGameScene* const pscene = get_active_scene())
			pscene->set_active_personage(static_cast<qdGameObjectMoving*>(ptrObj));
	}
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bCanDragScene||m_eViewState == VS_EDIT_SCREEN_LAYOUT)
	{
		ReleaseCapture();
		m_hCurrCursor = m_hHandCur;
		UpdateScrollers();
		Invalidate();
		CWnd ::OnLButtonUp(nFlags, point);
		return;
	}
	scr2log(&point, 1);

	switch(m_eViewState) {
	case VS_EDIT_GAME_AREA:
		if (get_active_scene())
			GAOnLButtonUp(nFlags, point);
		break;
	case VS_EDIT_COORD_ANIM:
		{
			m_bCACanDrag = false;
			m_hCurrCursor = m_hHandCur;
			
			m_refCAnim.reset_cur_point();

			RedrawScene(get_active_scene());
			Invalidate();
		}
		break;
	case VS_EDIT_NET:
		if (get_active_scene())
			NTOnLButtonUp(nFlags, point);
		break;
	case VS_EDIT_SCENE:
		if (get_active_scene())
			ESOnLButtonUp(nFlags, point);
		break;
	case VS_EDIT_ANIMATED:
		if (get_active_scene())
			EAOnLButtonUp(nFlags, point);
		break;
	case VS_EDIT_STACK_STATE:
		if (get_active_scene())
			SSOnLButtonUp(nFlags, point);
		break;
	case VS_EDIT_MASK:
		if (get_active_scene())
			SMOnLButtonUp(nFlags, point);
		break;
	case VS_EDIT_STATE:
		if (get_active_scene())
			ESTOnLButtonUp(nFlags, point);
		break;
	case VS_EDIT_INVENTORY:
		if (get_active_scene())
			EIOnLButtonUp(nFlags, point);
		break;
	case VS_EDIT_INTERFACE:
//		m_ptrIntefEditMode->OnLButtonUp(nFlags, point);
		m_ptrCurrentSceneViewMode->OnLButtonUp(nFlags, point);
		break;
	}
	CWnd ::OnLButtonUp(nFlags, point);
}

void CChildView::UpdateScrollers()
{
	SetHorzScroller();
	SetVertScroller();
}

bool CChildView::SetHorzScroller()
{
	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nPage = 0;
	si.nMax = m_iMaxScrollLimX - m_iMinScrollLimX;
	si.nPos = m_iMaxScrollLimX - m_pointBmpViewOrg.x;
	if(si.nMax)
	{
		si.nMin = 0;
		si.nPage = 10;
	}
	SetScrollInfo(SB_HORZ, &si);
	
	return true;
}

bool CChildView::SetVertScroller()
{
	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nPage = 0;
	si.nMax = m_iMaxScrollLimY - m_iMinScrollLimY;
	si.nPos = m_iMaxScrollLimY - m_pointBmpViewOrg.y;
	if(si.nMax)
	{
		si.nMin = 0;
		si.nPage = 10;
	}
	SetScrollInfo(SB_VERT, &si);
	
	return true;
}

bool CChildView::AdjustBitmapSize()
{
#ifndef USE_NEW_PROJ
	CSingleLock __sl(&m_csDrawGuard, TRUE);
#endif // USE_NEW_PROJ
	CRect r;
	GetClientRect(r);
	qdGameScene*pscene = get_active_scene();
	int const HORZ_INCREASE = 
		GetMyApp()->GetIntSetting(SETTING_NAME(SV_LEFT_RIGHT_OFFSET));
	int const VERT_INCREASE = 
		GetMyApp()->GetIntSetting(SETTING_NAME(SV_TOP_BOTTOM_OFFSET));
	if(m_eViewState != VS_EDIT_INTERFACE&&pscene)
	{
		qdCamera* pcam = get_camera(pscene);
		
		//прибавка для того, чтобы была возможность скролировать
		//за пределы сцены
		int sx = (std::max)(r.Width(), pcam->get_scr_sx()  + HORZ_INCREASE);
		int sy = (std::max)(r.Height(), pcam->get_scr_sy() + VERT_INCREASE);
		
		
		if (grDispatcher::instance()->Get_SizeX() != sx
			||   grDispatcher::instance()->Get_SizeX() != sy)
		{
			const Vect2i scr_offset = pcam->get_scr_offset();
			pcam->set_scr_center((sx>>1)-scr_offset.x, (sy>>1)-scr_offset.y);

			grDispatcher::instance()->
				init(sx, sy,GR_RGB888,static_cast<void*>(m_hWnd));
			grDispatcher::instance()->SetClip();
		}
	}
	else
	{
		int sx = (std::max)(r.Width(), 
			qdGameConfig::get_config().screen_sx() + (HORZ_INCREASE>>1));
		int sy = (std::max)(r.Height(), 
			qdGameConfig::get_config().screen_sy() + (VERT_INCREASE>>1));
		if (grDispatcher::instance()->Get_SizeX() != sx
			||   grDispatcher::instance()->Get_SizeX() != sy)
		{
			grDispatcher::instance()->init(sx, sy, 
					GR_RGB888, static_cast<void*>(m_hWnd));
			grDispatcher::instance()->SetClip();
		}
		m_fScale = 1.f;
	}
	SetScale(m_fScale);
	return true;
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_ptrViewDC = ::GetDC(m_hWnd);
	if(!m_ptrViewDC)
		return -1;

//	m_ptrIntefEditMode->Init(this, NULL);
//	m_ptrEditDialogsFrameMode->Init(this, NULL);
	return 0;
}

void CChildView::OnBtnHand() 
{
	if(m_eViewState == VS_EDIT_SCREEN_LAYOUT)
		SetViewState(m_eOldViewState);	
	else
	{
		m_eOldViewState = m_eViewState;
		SetViewState(VS_EDIT_SCREEN_LAYOUT);	
	}
	RedrawScene(get_active_scene());
	Invalidate();
}

void CChildView::OnUpdateBtnHand(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_eViewState == VS_EDIT_SCREEN_LAYOUT);
	pCmdUI->Enable(get_active_scene() != NULL&&((m_iMaxScrollLimX != m_iMinScrollLimX)
		||(m_iMaxScrollLimY != m_iMinScrollLimY)));
}

void CChildView::OnDestroy() 
{
	::ReleaseDC(m_hWnd, m_ptrViewDC);
	CWnd ::OnDestroy();
		
}

void CChildView::GADrawDragRect()
{
	int iOldMode = SetROP2(m_ptrViewDC, R2_NOT);
	HGDIOBJ oldPen	= SelectObject(m_ptrViewDC, GetStockObject(WHITE_PEN));
	HGDIOBJ oldBr	= SelectObject(m_ptrViewDC, GetStockObject(NULL_BRUSH));

	::Rectangle(m_ptrViewDC, m_rDraggedGameArea.left,
							m_rDraggedGameArea.top,
							m_rDraggedGameArea.right,
							m_rDraggedGameArea.bottom);

	SelectObject(m_ptrViewDC, oldBr);
	SelectObject(m_ptrViewDC, oldPen);

	SetROP2(m_ptrViewDC, iOldMode);
}

void CChildView::GAFindAnchor(const CPoint & point)
{
	qdCamera* ptrCam = get_camera();
	if (!ptrCam)
		return;
	CPoint pCenter(ptrCam->get_scr_center_x(), ptrCam->get_scr_center_y());
	const Vect2i& scr_offset = ptrCam->get_scr_offset();
	pCenter.x += scr_offset.x;
	pCenter.y += scr_offset.y;
	
	CPoint vLeftTop
		(pCenter.x - (ptrCam->get_scr_sx()>>1) - (WORK_AREA_BOUND_THICKNESS>>1),
		pCenter.y - (ptrCam->get_scr_sy()>>1) - (WORK_AREA_BOUND_THICKNESS>>1));
	
	CPoint vRightBottom
		(pCenter.x + (ptrCam->get_scr_sx()>>1) + (WORK_AREA_BOUND_THICKNESS>>1),
		pCenter.y + (ptrCam->get_scr_sy()>>1) + (WORK_AREA_BOUND_THICKNESS>>1));
	
	CRect r = make_lefttop(vLeftTop, vRightBottom, ANCHOR_WIDTH, ANCHOR_HEIGHT);
	m_iDraggedAnchor = -1;
	if(r.PtInRect(point))
	{
		m_iDraggedAnchor = 0;
		return;
	}

	r = make_leftmidle(vLeftTop, vRightBottom, ANCHOR_WIDTH, ANCHOR_HEIGHT);
	if(r.PtInRect(point))
	{
		m_iDraggedAnchor = 1;
		return;
	}
	
	r = make_leftbottom(vLeftTop, vRightBottom, ANCHOR_WIDTH, ANCHOR_HEIGHT);
	if(r.PtInRect(point))
	{
		m_iDraggedAnchor = 2;
		return;
	}
	
	r = make_bottommidle(vLeftTop, vRightBottom, ANCHOR_WIDTH, ANCHOR_HEIGHT);
	if(r.PtInRect(point))
	{
		m_iDraggedAnchor = 3;
		return;
	}
	
	r = make_rightbottom(vLeftTop, vRightBottom, ANCHOR_WIDTH, ANCHOR_HEIGHT);
	if(r.PtInRect(point))
	{
		m_iDraggedAnchor = 4;
		return;
	}
	
	r = make_rightmidle(vLeftTop, vRightBottom, ANCHOR_WIDTH, ANCHOR_HEIGHT);
	if(r.PtInRect(point))
	{
		m_iDraggedAnchor = 5;
		return;
	}
	
	r = make_righttop(vLeftTop, vRightBottom, ANCHOR_WIDTH, ANCHOR_HEIGHT);
	if(r.PtInRect(point))
	{
		m_iDraggedAnchor = 6;
		return;
	}
	
	r = make_topmidle(vLeftTop, vRightBottom, ANCHOR_WIDTH, ANCHOR_HEIGHT);
	if(r.PtInRect(point))
	{
		m_iDraggedAnchor = 7;
		return;
	}
	
	return;
}

/*!
 	Рисует прямоугольник и, если режим работы VS_EDIT_GAME_AREA, якоря для редктирования
	рабочей области
	\remarks Рабочая область - это область охватывающая всю сцену. Она задает пределы
	скроллирования
*/
void CChildView::DrawWorkAreaBound()
{
	qdGameScene* pScene = get_active_scene();
	if(!pScene)
		return;
	
	qdCamera *ptrCam = get_camera(pScene);
	if (!ptrCam) 
		return;

	Vect2s vCenter = ptrCam->get_scr_center();
	vCenter += ptrCam->get_scr_offset();

	Vect2s vLeftTop(vCenter.x - (ptrCam->get_scr_sx()>>1) 
					- (WORK_AREA_BOUND_THICKNESS>>1),
					vCenter.y - (ptrCam->get_scr_sy()>>1) 
					- (WORK_AREA_BOUND_THICKNESS>>1));

	Vect2s vRightBottom(vCenter.x + 
				(ptrCam->get_scr_sx()>>1) + (WORK_AREA_BOUND_THICKNESS>>1),
				vCenter.y + (ptrCam->get_scr_sy()>>1) + (WORK_AREA_BOUND_THICKNESS>>1));

	DrawRectangle(vLeftTop, vRightBottom, WORK_AREA_BOUND_COLOR);

	if(m_eViewState == VS_EDIT_GAME_AREA)
		DrawAnchors(vLeftTop, vRightBottom, ANCHOR_WIDTH, ANCHOR_COLOR);
}

void CChildView::DrawActiveCellsetBound(const qdInventoryCellSet& set, 
										const CPoint& SceneLeftTop)
{
	CRect rc = GetCellsetRect(&set);
	rc.OffsetRect(SceneLeftTop);
	
	POINT* pts = reinterpret_cast<POINT*>(&rc);
	
	DrawRectangle(*pts, *(pts+1), CELL_SET_COLOR);
	DrawAnchors(*pts, *(pts + 1), CELL_SET_ANCHOR_WIDTH, CELL_SET_ANCHOR_COLOR);
}

void CChildView::DrawCellsetBound(const qdInventoryCellSet& set, const CPoint& SceneLeftTop)
{
	CRect rc = GetCellsetRect(&set);
	rc.OffsetRect(SceneLeftTop);
	
	POINT* pts = reinterpret_cast<POINT*>(&rc);
	
	DrawRectangle(*pts, *(pts+1), CELL_SET_COLOR, 4, IM_INVERSE_ON);
}

void CChildView::DrawCellsetBounds(qdInventory* ptrInv)//обводим рамкой отдельные наборы ячеек
{
	ASSERT(ptrInv);

	CPoint SceneLeftTop(get_scene_lefttop(get_camera(), WORK_AREA_BOUND_THICKNESS));

	typedef qdInventoryCellSetVector::const_iterator icsv_ci_t;
	const qdInventoryCellSetVector& v = ptrInv->cell_sets();
	icsv_ci_t i = v.begin(), e = v.end();
	int num = 0;
	int iSelectedCellset = ptrInv->selected_cellset_num();
	for(; i != e; ++i, ++num)
	{
		const qdInventoryCellSet& set = *i;
		if (num == iSelectedCellset)
			DrawActiveCellsetBound(set, SceneLeftTop);
		else
			DrawCellsetBound(set, SceneLeftTop);
	}

}

/*!
 	От левого верхнего узла сцены рисуем прямоугольник шириной и высотой, равными 
	разрешению экрана в игре
*/
void CChildView::DrawVisibleAreaForInventory()
{
	CPoint vLeftTop(get_scene_lefttop(get_camera(), WORK_AREA_BOUND_THICKNESS));
	
	CPoint vRightBottom(vLeftTop.x + qdGameConfig::get_config().screen_sx(),
						vLeftTop.y + qdGameConfig::get_config().screen_sy());

	DrawRectangle(vLeftTop, vRightBottom, GAME_AREA_COLOR);
}


void CChildView::DrawInterface()
{
	grDispatcher::instance()->Fill(0);
	qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
	pd->redraw();
	if (m_ptrIntefEditMode.get())
	{
		m_ptrIntefEditMode->Draw(m_ptrViewDC);

		//! Отрисовываем видимую область
		CPoint vLeftTop(0, 0);
		CPoint vRightBottom(vLeftTop.x + qdGameConfig::get_config().screen_sx(),
			vLeftTop.y + qdGameConfig::get_config().screen_sy());

		DrawRectangle(vLeftTop, vRightBottom, GAME_AREA_COLOR);
	}
}

void CChildView::DrawInventory()
{
	if (m_ptrActiveInventory)
	{
		CPoint vLeftTop(get_scene_lefttop(get_camera(), WORK_AREA_BOUND_THICKNESS));
		qdInventoryCell::set_screen_offset(Vect2i(0, 0));
		m_ptrActiveInventory->redraw(vLeftTop.x, vLeftTop.y);
		DrawCellsetBounds(m_ptrActiveInventory);
	}
}

/*!
 	Прорисовка опирается на положение персонажа. Положение видимой области примерно
	такое же как в игре
	\remarks Видимая область - это область, которую видоно на экране в игре
*/
void CChildView::DrawVisibleArea()
{
	qdGameDispatcher* ptrDisp = getGameDispatcher();
	if(!ptrDisp)
		return;
	VAMove(VATest(ptrDisp->get_active_personage()));
	DrawRectangle(m_rcVisibleArea.TopLeft(), m_rcVisibleArea.BottomRight(), 
					GAME_AREA_COLOR, 0, IM_INVERSE_OFF);
}

/************************************************************************/
/* Работа с координатной анимацией                                      */
/************************************************************************/
int CChildView::CAFindPoint(const Vect2s& vPos)
{
	qdCamera* ptrCam = get_camera();
	_ASSERT (ptrCam);
	
	Vect3f planePos = ptrCam->scr2plane(vPos);
	const sGridCell* pCell = ptrCam->get_cell(planePos.x, planePos.y);
	
	Vect3f pos (planePos.x, planePos.y, (pCell?pCell->height():0)+planePos.z);
	
	Vect2s scr = ptrCam->global2scr(pos);
	
	int iCount = m_refCAnim.size();
	for(int i = 0; i < iCount; ++i)
	{
		Vect2s __t = 
			scr - ptrCam->global2scr(m_refCAnim.get_point(i)->dest_pos());
		if(__t.norm() < (CA_POINT_WIDTH>>1))
			return i;
	}
	return -1;
}

bool CChildView::CABeginEditPoint(const Vect2s& vPos)
{
	int iPoint = CAFindPoint(vPos);
	if(iPoint == -1)
		return false;
	m_refCAnim.set_cur_point(iPoint);
	
	m_bCACanDrag = true;
	m_ptrCADlg->OnChangeCurrPoint();
	return true;
}


void CChildView::CADelPoint(const Vect2s& vPos)
{
	int iPos = CAFindPoint(vPos);
	if (iPos != -1) {
		m_refCAnim.remove_point(iPos);
		m_refCAnim.reset_cur_point();

		if (m_bConturClosed&&!CAIsEndEqlBeg(m_refCAnim))
			m_bConturClosed = false;
		m_ptrCADlg->OnChangeCurrPoint();
	}
}

void CChildView::CADelPoint(int iPoint)
{
	m_refCAnim.remove_point(iPoint);
	m_refCAnim.reset_cur_point();
	if (m_bConturClosed&&!CAIsEndEqlBeg(m_refCAnim))
		m_bConturClosed = false;
	m_ptrCADlg->OnChangeCurrPoint();
}

void CChildView::CACheckPointsCount()
{
	if (m_refCAnim.is_empty()) {
		qdCoordsAnimationPoint caPoint;
		
		const Vect3f& R = 
			static_cast<qdGameObject*>(m_refCAnim.owner()->owner())->R();

		if (m_refCAnim.type()==qdCoordsAnimation::CA_WALK) {
			caPoint.set_dest_pos(Vect3f(R.x, R.y, get_camera()->get_grid_center().z));
		}
		else
		{
			caPoint.set_dest_pos(R);
		}
		m_refCAnim.add_point(&caPoint);
		m_refCAnim.set_cur_point(0);
	}
}

qdCoordsAnimationPoint CChildView::CAV2s2CAPnt(const Vect2s& vPos)
{
	qdCamera* ptrCam = get_camera();
	_ASSERT (ptrCam);
	
	qdCoordsAnimationPoint caPoint;
	
	if (m_refCAnim.type() == qdCoordsAnimation::CA_WALK) {
		Vect3f planePos = ptrCam->scr2plane(vPos);
		caPoint.set_dest_pos(Vect3f(planePos.x, planePos.y, planePos.z));
	}
	else
	{
		Vect3f oldGrCenter = ptrCam->get_grid_center();
		Vect3f grCenter(0, 0, m_refCAnim.get_point(m_refCAnim.get_cur_point())->dest_pos().z);
		
		ptrCam->set_grid_center(grCenter);
		Vect3f planePos = ptrCam->scr2plane(vPos);
		ptrCam->set_grid_center(oldGrCenter);
		caPoint.set_dest_pos(Vect3f(planePos.x, planePos.y, planePos.z));
	}
	return caPoint;
}

bool CChildView::CAInsertPoint(const Vect2s& vPos)
{
	std::auto_ptr<Vect3fVect_t> contour = CACAPts2Vect3f(m_refCAnim);

	qdCoordsAnimationPoint p = CAV2s2CAPnt(vPos);
	int iSide = GetNearestSide(p.dest_pos(), *contour.get(), SM_MAX_DIST_VALUE);
	
	if (iSide != -1) {
		m_refCAnim.insert_point(&p, iSide + 1);
		m_refCAnim.set_cur_point(iSide + 1);
		m_ptrCADlg->OnChangeCurrPoint();
		return true;
	}
	return false;
}

void CChildView::CAAddPoint(const Vect2s& vPos)
{
	CACheckPointsCount();

	m_refCAnim.add_point(&CAV2s2CAPnt(vPos));
	m_refCAnim.set_cur_point(m_refCAnim.size()-1);
	
	m_ptrCADlg->OnChangeCurrPoint();
}

/*!
	 \param ca			- Координатная анимация
	 \param bClosed		- Замкнут ли контур
	 \param ptrCam		- Камера текущей сцены
	 \param line_clr	- цвет линии
*/
void CChildView::DrawCoordsAnimPts(const qdCoordsAnimation& ca,bool bClosed, 
								   qdCamera* ptrCam, COLORREF line_clr)
{
	int const iCount = ca.size();
	ASSERT(iCount);

	bool const bCheckImpassable = (ca.type() == qdCoordsAnimation::CA_WALK);
	//! Походка привязывается к сетке
	bool const bUseNetZ = ca.type() == qdCoordsAnimation::CA_WALK;
	float const fNetZ = ptrCam->get_grid_center().z;

	Vect3f pos = ca.get_point(0)->dest_pos();
	if (bUseNetZ)
		pos.z = fNetZ;

	Vect2s scr_pos = ptrCam->global2scr(pos);

	int c = CA_POINT_COLOR;
	if (bCheckImpassable) {
		if(!static_cast<qdGameObjectMoving*>(ca.owner()->owner())
					->is_walkable(ca.get_point(0)->dest_pos()))
			c = CA_IMPASSABLE_POINT_COLOR;
	}
	
	grDispatcher* pgrDisp = grDispatcher::instance();
	if(ca.get_cur_point() == 0){
		pgrDisp->Rectangle(scr_pos.x - (CA_POINT_OUT_WIDTH>>1), 
			scr_pos.y - (CA_POINT_OUT_HEIGHT>>1),
			CA_POINT_OUT_WIDTH, CA_POINT_OUT_HEIGHT,
			c, c, GR_FILLED);
		pgrDisp->Rectangle(scr_pos.x - (CA_POINT_WIDTH>>1), 
			scr_pos.y - (CA_POINT_HEIGHT>>1),
			CA_POINT_WIDTH, CA_POINT_HEIGHT,
			c, CA_CUR_POINT_COLOR, GR_FILLED);
	}
	else {
		pgrDisp->Rectangle(scr_pos.x - (CA_POINT_WIDTH>>1), 
			scr_pos.y - (CA_POINT_HEIGHT>>1),
			CA_POINT_WIDTH, CA_POINT_HEIGHT,c, c, GR_FILLED);
	}
	
	for(int i = 1; i < iCount; ++i)
	{
		pos = ca.get_point(i)->dest_pos();
		if (bUseNetZ)
			pos.z = fNetZ;

		Vect2s scr_pos1 = ptrCam->global2scr(pos);
		pgrDisp->Line(scr_pos.x, scr_pos.y, 
							scr_pos1.x, scr_pos1.y,line_clr);
		
		c = CA_POINT_COLOR;
		if (bCheckImpassable) {
			if(!static_cast<qdGameObjectMoving*>(ca.owner()->owner())
				->is_walkable(ca.get_point(i)->dest_pos()))
				c = CA_IMPASSABLE_POINT_COLOR;
		}
		
		if(ca.get_cur_point() == i)
		{
			pgrDisp->Rectangle(scr_pos1.x - (CA_POINT_OUT_WIDTH>>1), 
				scr_pos1.y - (CA_POINT_OUT_HEIGHT>>1),
				CA_POINT_OUT_WIDTH, CA_POINT_OUT_HEIGHT,c, c, GR_FILLED);
			pgrDisp->Rectangle(scr_pos1.x - (CA_POINT_WIDTH>>1), 
				scr_pos1.y - (CA_POINT_HEIGHT>>1),
				CA_POINT_WIDTH, CA_POINT_HEIGHT, c, CA_CUR_POINT_COLOR, GR_FILLED);
		}
		else
		{
			pgrDisp->Rectangle(scr_pos1.x - (CA_POINT_WIDTH>>1), 
				scr_pos1.y - (CA_POINT_HEIGHT>>1),
				CA_POINT_WIDTH, CA_POINT_HEIGHT,c, c, GR_FILLED);
		}
		scr_pos = scr_pos1;
	}
	if (bClosed) {
		pos = ca.get_point(0)->dest_pos();
		if (bUseNetZ)
			pos.z = fNetZ;
			
		Vect2s scr_pos1 = ptrCam->global2scr(pos);
		pgrDisp->Line(scr_pos.x, scr_pos.y, scr_pos1.x, scr_pos1.y,line_clr);
	}
}

/*!
 	Проверяет рабочий режим и непустая ли координатная анимация. Вызывает
	CChildView::DrawCoordsAnimPts(). Затем отрисовывает размеры и проекцию на сетку
*/
void CChildView::DrawCoordAnimationPoints()
{
	if(m_eViewState != VS_EDIT_COORD_ANIM)
		return;
	
	if(m_refCAnim.is_empty())
		return;

	DrawCoordsAnimPts(m_refCAnim, m_bConturClosed, get_camera(), CA_LINE_COLOR);
	ASSERT(m_refCAnim.owner()->owner()); 
	qdGameObjectAnimated* pobj = 
		static_cast<qdGameObjectAnimated*>(m_refCAnim.owner()->owner());
	if (pobj->has_bound())
		pobj->draw_bound();
	DrawNetProj(pobj);
}

/*!
	Копирует ptrCA поверх m_refCAnim
	\param ptrCA - КА, которую будем редактировать
*/
void CChildView::BeginEditCoordAnim(qdCoordsAnimation* ptrCA)
{
	m_refCAnim = *ptrCA;
	//проверяем есть ли точки и добавляем текущее полоежние
	//объекта, если список точке пустой
	CACheckPointsCount();
	m_refCAnim.set_cur_point(0);

	//договорились, что если к.анимация зациклена
	//занчит и контур должен быть замкнут
	m_bConturClosed = m_refCAnim.check_flag(QD_COORDS_ANM_LOOP_FLAG);
}

/*!
 	Просто делаем зачистку данных
*/
void CChildView::EndEditCoordAnim()
{
	m_ptrCADlg = NULL;
	m_rectHilited.SetRectEmpty();
	m_refCAnim.clear();
}

void CChildView::UpdateCoordsAnimation()
{
	Redraw(RM_REDRAW_CONTENT);
}

CRect CChildView::CAGetFlushRect(const qdCoordsAnimation& ca, qdCamera* ptrCam,
											bool bClosed)
{
	if (ca.is_empty()) 
		return CRect(0, 0, 0, 0);

	Vect2s scr = ptrCam->global2scr(ca.get_point(ca.get_cur_point())->dest_pos());

	Vect2s scr_prev = scr;
	if (ca.get_cur_point()) {//если непервая точка
		scr_prev = ptrCam->global2scr(ca.get_point(ca.get_cur_point()-1)->dest_pos());
	}
	else if (bClosed) {
		scr_prev = ptrCam->global2scr(ca.get_point(ca.size()-1)->dest_pos());
	}
	
	Vect2s scr_next = scr;
	if (ca.get_cur_point() != ca.size()-1) {//если не последняя
		scr_next = ptrCam->global2scr(ca.get_point(ca.get_cur_point()+1)->dest_pos());
	}
	else if (bClosed) {
		scr_next = ptrCam->global2scr(ca.get_point(0)->dest_pos());
	}
	
	CRect r(scr_prev.x, scr_prev.y, scr_next.x, scr_next.y);
	r.NormalizeRect();
	
	CRect rectFlush(scr.x - (CA_POINT_WIDTH>>1),
		scr.y - (CA_POINT_HEIGHT>>1),
		scr.x + (CA_POINT_WIDTH>>1), 
		scr.y + (CA_POINT_HEIGHT>>1));
	
	rectFlush.UnionRect(&rectFlush, &r);

	return rectFlush;
}

/*
 *	обработчики событий
 */
void CChildView::CAOnLButtonDown(UINT nFlags, const CPoint &point)
{
	HiliteRect(m_rectHilited);
	m_rectHilited.SetRectEmpty();
	if (!CABeginEditPoint(Vect2s(point.x, point.y))) {
		if (m_bConturClosed) 
			CAInsertPoint(Pnt2V2s(point));
		else
			CAAddPoint(Pnt2V2s(point));
	}
	else
		m_base_point = point;
}

void CChildView::CAOnRButtonUp(UINT nFlags, const CPoint& point)
{
	if (m_refCAnim.size()>1)
	{
		if (m_bConturClosed) 
		{
			m_refCAnim.drop_flag(QD_COORDS_ANM_LOOP_FLAG);
			CADelPoint(m_refCAnim.size()-1);
			m_bConturClosed = false;
		}
		else
		{
			m_refCAnim.add_point(m_refCAnim.get_point(0));
			m_refCAnim.set_cur_point(m_refCAnim.size()-1);
			m_ptrCADlg->OnChangeCurrPoint();

			m_refCAnim.set_flag(QD_COORDS_ANM_LOOP_FLAG);
			m_bConturClosed = true;
		}
		Redraw(RM_REDRAW_CONTENT);
	}
}

void CChildView::CAOnMouseMove(UINT nFlags, const CPoint& point)
{
	if(m_bCACanDrag)
	{
		Vect3f vecOldPos = m_refCAnim.get_point(m_refCAnim.get_cur_point())->dest_pos(), 
			pos = vecOldPos;

		qdGameObjectAnimated* pobj = 
			static_cast<qdGameObjectAnimated*>(m_refCAnim.owner()->owner());

		//координатная анимация редактируется у выбранного объекта
		Vect3f vecOldObjPos = pobj->R();
		
		qdCamera* ptrCam = get_camera();
		if(drag_point(pobj, pos, point, nFlags, ptrCam))
		{
			m_refCAnim.get_point(m_refCAnim.get_cur_point())->set_dest_pos(pos);
			m_refCAnim.reset_cur_point();
			m_ptrCADlg->OnChangeCurrPoint();
			Redraw(RM_REDRAW_CONTENT);
			m_base_point = point;
			SetProjChange();
		}				
	}
	else
	{
		
		HiliteRect(m_rectHilited);

		int iPoint = CAFindPoint(Pnt2V2s(point));
		if (iPoint != -1) {
			qdCamera* ptrCam = get_camera();
			CPoint hpoint = V2s2Pnt(
				ptrCam->global2scr(m_refCAnim.get_point(iPoint)->dest_pos()));

			m_rectHilited.left		= hpoint.x - (CA_POINT_WIDTH>>1);
			m_rectHilited.top		= hpoint.y - (CA_POINT_HEIGHT>>1);
			m_rectHilited.right		= hpoint.x + (CA_POINT_WIDTH>>1);
			m_rectHilited.bottom	= hpoint.y + (CA_POINT_HEIGHT>>1);

			log2scr(reinterpret_cast<POINT*>(&m_rectHilited), 2);
			HiliteRect(m_rectHilited);
		}
		else
			m_rectHilited.SetRectEmpty();
	}
}

BOOL CChildView::CAOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (!get_active_scene())
		return FALSE;
	if (m_bCACanDrag) {
		m_hCurrCursor = m_hDragHandCur;
	}
	else
	{
		if(CAFindPoint(GetLogCurPos()) == m_refCAnim.get_cur_point())
		{
			if(IsKeyPressed(VK_SHIFT))
				m_hCurrCursor = m_hHandCur;
		}
		else
			m_hCurrCursor = m_hFootCursor;
	}
	return TRUE;
}

void CChildView::CAOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bInvalidate = false;
	switch (nChar)
	{
	case VK_RETURN:
#ifdef USE_NEW_PROJ
		getProject()->EndEditCoordAnimation(EESTM_OK);
#else
		ptree_api::EndEditCoordAnimationMode(ptree_api::EESTM_OK);
#endif // USE_NEW_PROJ
		break;
	case VK_ESCAPE:
#ifdef USE_NEW_PROJ
		getProject()->EndEditCoordAnimation(EESTM_CANCEL);
#else
		ptree_api::EndEditCoordAnimationMode(ptree_api::EESTM_CANCEL);
#endif // USE_NEW_PROJ
		break;
	case VK_BACK:
		{
			if(m_refCAnim.is_empty())
				PlayError();
			else
			{
				CADelPoint(m_refCAnim.size()-1);
				m_rectHilited.SetRectEmpty();
			}
			
			bInvalidate = true;
			break;
		}
	case VK_DELETE:
		{
			if(m_refCAnim.is_empty())
				PlayError();
			else
			{
				CADelPoint(m_refCAnim.get_cur_point());
				m_rectHilited.SetRectEmpty();
			}
			
			bInvalidate = true;
			break;
		}
	case 'A':
		{
			Vect3f p = m_refCAnim.get_point(m_refCAnim.get_cur_point())->dest_pos();
			p.z += Z_CHANGE_DELTA;
			m_refCAnim.get_point(m_refCAnim.get_cur_point())->set_dest_pos(p);
			m_ptrCADlg->OnChangeCurrPoint();
		}
		break;
	case 'Z':
		{
			Vect3f p = m_refCAnim.get_point(m_refCAnim.get_cur_point())->dest_pos();
			p.z -= Z_CHANGE_DELTA;
			m_refCAnim.get_point(m_refCAnim.get_cur_point())->set_dest_pos(p);
			m_ptrCADlg->OnChangeCurrPoint();
		}
		break;
	}
	if (bInvalidate) 
		Redraw(RM_REDRAW_CONTENT);
}

void CChildView::CAOnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
	case 'A':
	case 'Z':
		m_refCAnim.reset_cur_point();
		Redraw(RM_REDRAW_CONTENT);
		break;
	}
}

/************************************************************************/
/* Редактирование и создание масок                                      */
/************************************************************************/
/*!
	Создает новый объект (qdGameObjectStateMask). Устанавливает значения для
	CChildView::m_ptrSMParent, CChildView::m_ptrSourceMask, CChildView::m_qdStateMask.

	Вызывается из класса дерева проекта

	\param ptrGO	- Объект, к которому приявязана маска
	\retval true	- если началось удачно
*/
bool CChildView::OnCreateStateMask(qdGameObject* ptrGO)
{
	m_ptrSourceMask = new qdGameObjectStateMask;
	if (!m_ptrSourceMask)
		return false;

	if (!ptrGO||
		ptrGO == reinterpret_cast<void*>(-1)||
		ptree_helpers::is_obj_animated(ptrGO))
	{
		std::pair<bool, qdGameObject*> res = 
			AskForObject(get_active_scene(), QD_NAMED_OBJECT_STATIC_OBJ, false);

		if (!res.first)
			return false;
		m_ptrSMParent = res.second;
		m_ptrSourceMask->set_owner(ptrGO);
	}
	else if (ptree_helpers::is_obj_static(ptrGO))
		m_ptrSMParent = ptrGO;

	m_ptrSourceMask->set_parent_name(m_ptrSMParent->name());
	m_qdStateMask.copy_contour(m_ptrSourceMask);
	m_qdStateMask.set_parent_name(m_ptrSourceMask->parent_name());
	m_qdStateMask.set_owner(m_ptrSourceMask->owner());

	m_bConturClosed = false;
	m_bWorkChanged = false;

	SetFocus();
	return (m_ptrSourceMask != NULL);
}

/*!
	\param ptrMask	- Состояние маска, которое мы будем редактировать
	\retval true	- если началось удачно
*/
bool CChildView::OnEditStateMask(qdGameObjectStateMask* ptrMask)
{
	ASSERT(ptrMask);
	m_ptrSMParent = get_active_scene()->get_object(ptrMask->parent_name());
	if(!m_ptrSMParent)
		return false;
	
	m_ptrSourceMask = ptrMask;
	m_qdStateMask.copy_contour(ptrMask);
	m_bConturClosed = true;
	m_bWorkChanged = false;

	ASSERT(m_qdStateMask.contour_size());
	if (m_iActivePoint == -1) 
		m_iActivePoint = 0;

	CString cap;
	cap.Format(IDS_EDT_CNTR_DLG_CAP4, ptrMask->name());
	SetFocus();
	
	return true;
}

void CChildView::SMAbortEdit(bool bSave)
{
	if (bSave) 
		SMSaveWork();
	else
		SMKillWork();

	if (m_eSubTools == SSTE_CREATE_MASK) {
		delete m_ptrSourceMask;
		m_ptrSourceMask = NULL;
	}
	m_iActivePoint = -1;
	if(m_qdStateMask.contour_size())
		m_qdStateMask.clear_contour();
	m_rectHilited.SetRectEmpty();
	m_ptrSourceMask = NULL;
}

/*!
	Вызывается из класса дерева проекта
	\param bSave - сохранять ли изменения
*/
void CChildView::OnAbortStateMaskEdit(bool bSave)
{
	SMAbortEdit(bSave);	
}

bool CChildView::SMCreateByteMask()
{						
	return m_ptrSourceMask->update_contour();
}

void CChildView::SMSaveWork()
{
	if (m_ptrSourceMask->can_be_closed())
	{
		if (m_bWorkChanged) {

			m_qdStateMask.copy_contour(m_ptrSourceMask);//сохраняем изменения
			
			if (m_eSubTools == SSTE_CREATE_MASK) {

				qdGameObjectStateMask* ptrState = m_ptrSourceMask;
				//чтобы не убить работу при смене узлов в дереве
				m_ptrSourceMask = NULL;
				//реакция на создание маски
#ifdef USE_NEW_PROJ
				if (getProject()->OnEndStateMaskCreate(ptrState))
#else
				if (m_ptrMainFrame->OnEndStateMaskCreate(ptrState))
#endif // USE_NEW_PROJ
				{
					SetSubTools(SSTE_EDIT_MASK);
					static_cast<CFrameWnd*>(AfxGetMainWnd())->
						SetMessageText(CString((LPTSTR)IDS_EDIT_WORK_SAVED_SUCC));
					m_bWorkChanged = false;
					SetProjChange();
				}
				else
				{
					SMAbortEdit(false);
					SetSubTools(SSTE_NONE);
					SetViewState(VS_EDIT_SCENE);
					SetFocus();
				}
			}
			else
			{
				static_cast<CFrameWnd*>(AfxGetMainWnd())->
					SetMessageText(CString((LPTSTR)IDS_EDIT_WORK_SAVED_SUCC));

				SetProjChange();
				m_bWorkChanged = false;
			}
		}
		else
		{
			if (m_ptrSourceMask)
			{
				//! Копируем имя, чтобы потом восстановить его
//				*m_ptrSourceMask = m_qdStateMask;
				m_ptrSourceMask->copy_contour(&m_qdStateMask);
			}
		}
	}
}

void CChildView::SMKillWork()
{
	if (m_bWorkChanged&&m_ptrSourceMask) {
		m_bWorkChanged = false;
		if(m_eSubTools == SSTE_EDIT_MASK)
		{
			m_bConturClosed = true;
//			*m_ptrSourceMask = m_qdStateMask;
			m_ptrSourceMask->copy_contour(&m_qdStateMask);
		}
		else if (m_eSubTools == SSTE_CREATE_MASK) {
			*m_ptrSourceMask = qdGameObjectStateMask();
			m_ptrSourceMask->set_parent_name(m_qdStateMask.parent_name());
			m_ptrSourceMask->set_owner(m_qdStateMask.owner());
			m_bConturClosed = false;
		}
	}
}

/*!
 	Если CChildView::m_ptrSourceMask == NULL или контур CChildView::m_ptrSourceMask
	пуст, то ничего не делаем. Иначе оббегаем все точки и отрисовываем их на экран
*/
void CChildView::DrawMask()
{
	if (!m_ptrSourceMask||m_ptrSourceMask->get_contour().empty()) 
		return;

	const std::vector<Vect2s>& mc = m_ptrSourceMask->get_contour();
	std::vector<Vect2s>::const_iterator _beg = mc.begin(), _end = mc.end();

	Vect2s scr_pos (*_beg);
	SMMask2Scr(scr_pos);
	grDispatcher* const pdisp = grDispatcher::instance();
	if(m_iActivePoint == 0)
	{
		pdisp->Rectangle(scr_pos.x - (SM_POINT_WIDTH>>1), 
			scr_pos.y - (SM_POINT_HEIGHT>>1),
			SM_POINT_WIDTH, SM_POINT_HEIGHT,
			SM_POINT_FRAME_COLOR, SM_CUR_POINT_COLOR, GR_FILLED);
	}
	else
	{
		pdisp->Rectangle(scr_pos.x - (SM_POINT_WIDTH>>1), 
			scr_pos.y - (SM_POINT_HEIGHT>>1),
			SM_POINT_WIDTH, SM_POINT_HEIGHT,
			SM_POINT_FRAME_COLOR, SM_POINT_COLOR, GR_FILLED);
	}

	int i = 1;
	Vect2s scr_pos1;
	for(++_beg; _beg != _end; ++_beg, ++i)
	{
		scr_pos1 =  *_beg;
		SMMask2Scr(scr_pos1);
		pdisp->Line(scr_pos.x, scr_pos.y, scr_pos1.x, scr_pos1.y,SM_LINE_COLOR);
		
		if(m_iActivePoint == i)
		{
			pdisp->Rectangle(scr_pos1.x - (SM_POINT_WIDTH>>1), 
				scr_pos1.y - (SM_POINT_HEIGHT>>1),
				SM_POINT_WIDTH, SM_POINT_HEIGHT,
				SM_POINT_FRAME_COLOR, SM_CUR_POINT_COLOR, GR_FILLED);
		}
		else
		{
			pdisp->Rectangle(scr_pos1.x - (SM_POINT_WIDTH>>1), 
				scr_pos1.y - (SM_POINT_HEIGHT>>1),
				SM_POINT_WIDTH, SM_POINT_HEIGHT,
				SM_POINT_FRAME_COLOR, SM_POINT_COLOR, GR_FILLED);
		}
		scr_pos = scr_pos1;
	}
	if (m_bConturClosed) {
		scr_pos1 =  *mc.begin();
		SMMask2Scr(scr_pos1);
		pdisp->Line(scr_pos.x, scr_pos.y, scr_pos1.x, scr_pos1.y,SM_LINE_COLOR);
	}
}

//полагаемся на m_ptrSMParent, потому что его не всегда можно получить из состояния
//в нужный момент. Например в момент создания маски не всегда известен владелец, поэтому
//parent получить не удается
inline const Vect2s CChildView::SMScr2Mask(const Vect2s& point)
{
	Vect2s vPoint(point);
	vPoint -= m_ptrSMParent->screen_pos();
	return vPoint;
}

inline const Vect2s CChildView::SMMask2Scr(const Vect2s& point)
{
	Vect2s vPoint(point);
	vPoint += m_ptrSMParent->screen_pos();
	return vPoint;
}

inline void CChildView::SMScr2Mask(Vect2s& point)
{
	point -= m_ptrSMParent->screen_pos();
}

inline void CChildView::SMMask2Scr(Vect2s& point)
{
	point += m_ptrSMParent->screen_pos();
}

std::auto_ptr<std::vector<Vect2s> > CChildView::SMPts2Scr(const std::vector<Vect2s>& mc)
{
	std::auto_ptr<std::vector<Vect2s> > res(new std::vector<Vect2s>);
	if(!res.get())
		return res;
	
	res->reserve(mc.size());
	std::vector<Vect2s>::const_iterator _itr = mc.begin(), _end = mc.end();
	for(; _itr != _end; ++_itr)
		res->push_back(SMMask2Scr(*_itr));
	
	return res;
}

BOOL CChildView::SMOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (!get_active_scene())
		return FALSE;
	if (m_eSubTools == SSTE_NONE) 
		m_hCurrCursor = m_hPenCursor;
	else
	{
		if (m_iActivePoint != -1
			&&IsKeyPressed(VK_SHIFT)
			&&IsKeyPressed(VK_LBUTTON)) 
		{
			m_hCurrCursor = m_hDragHandCur;
		}
		else
		{
			if (m_iActivePoint != -1
				&& SMFindPoint(SMScr2Mask(
					GetLogCurPos())) == m_iActivePoint) {
				if (IsKeyPressed(VK_SHIFT)) {
					m_hCurrCursor = m_hHandCur;
				}
			}
			else
				m_hCurrCursor = m_hPenCursor;
		}
	}
	return TRUE;
}

void CChildView::SMOnLButtonUp(UINT nFlags, const CPoint& point)
{
	if (m_eSubTools == SSTE_EDIT_MASK) {
		if (m_bConturClosed)
			SMCreateByteMask();
	}
}

void CChildView::SMOnLButtonDown(UINT nFlags, const CPoint& point)
{
	Vect2s vPoint (point.x, point.y); 
	SMScr2Mask(vPoint);
	
	HiliteRect(m_rectHilited);//склоняюсь к мысли о том, что эта строчка здесь не нужна
	m_rectHilited.SetRectEmpty();
	m_iActivePoint = SMFindPoint(vPoint);
	if(m_iActivePoint == -1)
	{
		if (m_bConturClosed)
			SMInsertPoint(vPoint);
		else //if(m_eSubTools == SSTE_CREATE_MASK)
			SMAddPoint(vPoint);
	}
	m_base_point = point;
	
	Redraw(RM_REDRAW_CONTENT);
}

bool CChildView::SMAddPoint(const Vect2s& mask_pos)
{
	m_iActivePoint = m_ptrSourceMask->contour_size();
	m_ptrSourceMask->insert_contour_point(mask_pos, m_iActivePoint);
	m_bWorkChanged = true;
	return true;
}

bool CChildView::SMInsertPoint(const Vect2s &mask_pos)
{
	int iSide = GetNearestSide(mask_pos, m_ptrSourceMask->get_contour(), 
													SM_MAX_DIST_VALUE);
	if (iSide != -1) {
		m_ptrSourceMask->insert_contour_point(mask_pos, iSide + 1);
		m_iActivePoint = iSide + 1;
		m_bWorkChanged = true;
		return true;
	}
	return false;
}

void CChildView::SMOnLButtonDblClk(UINT nFlags, const CPoint& point)
{
}

void CChildView::SMOnMouseMove(UINT nFlags, const CPoint &point)
{
	qdCamera* ptrCam = get_camera();
	if((nFlags&MK_SHIFT)&&(nFlags&MK_LBUTTON))
	{
		int dx = point.x - m_base_point.x;
		int dy = point.y - m_base_point.y;
	
		if (m_iActivePoint != -1)
		{
			Vect2s scr_point =  
				SMMask2Scr(m_ptrSourceMask->get_point(m_iActivePoint));

			Vect2s scr_prev = scr_point;
			if (m_iActivePoint) 
				scr_prev = SMMask2Scr(m_ptrSourceMask->
									get_point(m_iActivePoint-1));
			else if (m_bConturClosed) 
				scr_prev = SMMask2Scr(m_ptrSourceMask->get_point
								(m_ptrSourceMask->contour_size()-1));
			
			Vect2s scr_next = scr_point;
			if (m_iActivePoint != m_ptrSourceMask->contour_size()-1) 
				scr_next = SMMask2Scr(m_ptrSourceMask->
							get_point(m_iActivePoint+1));
			else if (m_bConturClosed) 
				scr_next = SMMask2Scr(m_ptrSourceMask->get_point(0));

			scr_point.x += dx;
			scr_point.y += dy;

			Vect2s newPt = scr_point;

			SMScr2Mask(newPt);
			if (!(newPt == m_ptrSourceMask->get_point(m_iActivePoint))) {
				m_bWorkChanged = true;
				m_ptrSourceMask->update_contour_point(newPt, m_iActivePoint);
			}

		}
		else if (m_eSubTools == SSTE_EDIT_MASK)
		{
			MoveOCntr mc(Vect2i(dx, dy), m_ptrSourceMask);
			Vect2s const v(mc(ptrCam));
			OffsetNotCAStates(
				static_cast<qdGameObjectAnimated*>(m_ptrSourceMask->owner()),
				v, m_ptrSourceMask);
			SetProjChange();
		}
		Redraw(sv_api::RM_REDRAW_CONTENT);
		
		m_base_point = point;
	}
	else
	{
		HiliteRect(m_rectHilited);
		
		int iPoint = SMFindPoint(SMScr2Mask(Pnt2V2s(point)));
		if (iPoint != -1) {
			Vect2s scr_point = 
				SMMask2Scr(m_ptrSourceMask->get_point(iPoint));

			m_rectHilited = CRect(scr_point.x - (SM_POINT_WIDTH>>1),
				scr_point.y - (SM_POINT_HEIGHT>>1),
				scr_point.x + (SM_POINT_WIDTH>>1), 
				scr_point.y + (SM_POINT_HEIGHT>>1));

			log2scr(reinterpret_cast<POINT*>(&m_rectHilited), 2);
			HiliteRect(m_rectHilited);
		}
		else
			m_rectHilited.SetRectEmpty();
	}
}

int CChildView::SMFindPoint(const Vect2s& point)
{
	const std::vector<Vect2s>& mc = m_ptrSourceMask->get_contour();
	std::vector<Vect2s>::const_iterator _res = 
		std::find_if(mc.begin(), mc.end(), Vect2s_point_ht(point, (SM_POINT_WIDTH<<1)/3));
	if (_res == mc.end()) {
		return -1;
	};
	
	return std::distance(mc.begin(), _res);
}

void CChildView::SMOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bInvalidate = false;
	if (nChar == VK_ESCAPE) {
		if (m_eSubTools == SSTE_CREATE_MASK) {
			if (!m_ptrSourceMask->contour_size()) {
				//прекращаем процесс создания маски
				SMAbortEdit(false);
				SetSubTools(SSTE_NONE);
				SetViewState(VS_EDIT_SCENE);
			}
			else
				//откатываемся назад, к пустой
				SMKillWork();
		}
		else//откатываемся назад к последней сохраненной
			SMKillWork();
		m_rectHilited.SetRectEmpty();
		bInvalidate = true;
	}
	if (nChar == VK_RETURN) {
		if (m_bConturClosed) {
			SMCreateByteMask();
			SMSaveWork();
		}
		else
			PlayError();

		bInvalidate = true;
	}
	if (nChar == VK_BACK) {
		//нельзя удалить крайнюю точку в закмкнутом или пустом
		//контуре                                             
		if (m_bConturClosed||m_ptrSourceMask->get_contour().empty())
			PlayError();
		else
		{
			m_ptrSourceMask->remove_contour_point(m_ptrSourceMask->contour_size()-1);
			m_bWorkChanged = true;

			if (m_iActivePoint >= m_ptrSourceMask->contour_size()) 
				m_iActivePoint = m_ptrSourceMask->contour_size()-1;
			
			if (m_bConturClosed) {//сохраняем, если замкнут
				SMCreateByteMask();
				SMSaveWork();
			}

			bInvalidate = true;
			m_rectHilited.SetRectEmpty();
		}
	}

	if (nChar == VK_DELETE) {
		if (m_iActivePoint == -1
			//замктнутый контур не может иметь меньше 3х точек
			||(m_bConturClosed&&m_ptrSourceMask->contour_size()==3)) {
			PlayError();
		}
		else
		{
			m_ptrSourceMask->remove_contour_point(m_iActivePoint);
			m_bWorkChanged = true;
			if (m_iActivePoint == m_ptrSourceMask->contour_size()) 
				--m_iActivePoint;

			//если замкнут, то сразу же сохраняем изменения
			if (m_bConturClosed) {
				SMCreateByteMask();
				SMSaveWork();
			}
			bInvalidate = true;
			m_rectHilited.SetRectEmpty();
		}
	}
	if (bInvalidate) 
		Redraw(RM_REDRAW_CONTENT);
}

void CChildView::SMOnRButtonUp(UINT nFlags, const CPoint& point)
{
	if (!m_ptrSourceMask->can_be_closed()) 
		return;
	
	m_bConturClosed = !m_bConturClosed;
	if (m_bConturClosed) {
		SMCreateByteMask();
		SMSaveWork();
	}
	Redraw(RM_REDRAW_CONTENT);
}

//#ifdef USE_NEW_PROJ
void CChildView::Redraw(eRedrawMode emode)
{
	if(emode == RM_REDRAW_CONTENT)
	{
		if (m_eViewState == VS_EDIT_INTERFACE)
			DrawInterface();
		else if (m_eViewState != VS_NONE){
			if (qdGameScene* ptrScene = get_active_scene()) 
				RedrawScene(ptrScene);
		}
		else
			grDispatcher::instance()->Fill(0);
  }
	Invalidate(FALSE);
	if (m_dwMainThreadID == GetCurrentThreadId()) 
		UpdateWindow();
}
//#endif // _DEBUG

//void CChildView::Redraw(sv_api::eRedrawMode emode)
//{
//	if(emode == RM_REDRAW_CONTENT)
//	{
//		if (m_eViewState == VS_EDIT_INTERFACE)
//			DrawInterface();
//		else if (m_eViewState != VS_NONE){
//			if (qdGameScene* ptrScene = get_active_scene()) 
//				RedrawScene(ptrScene);
//		}
//		else
//			grDispatcher::instance()->Fill(0);
//	}
//	Invalidate(FALSE);
//	if (m_dwMainThreadID == GetCurrentThreadId()) 
//		UpdateWindow();
//}

/*!
	Возвращает координаты мыши в логических оконых через Vect2s
*/
const Vect2s CChildView::GetLogCurPos()
{
	CPoint point(GetLogCurPos(CPoint()));
	return Vect2s(point.x, point.y);
}

/*!
 	Возвращает координаты мыши в логических оконных
*/
const CPoint& CChildView::GetLogCurPos(CPoint& point)
{
	GetCursorPos(&point);
	ScreenToClient(&point);
	scr2log(&point, 1);
	return point;
}

//! Возвращает позицию курсора в оконных координатах
const CPoint CChildView::GetCurPos()
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	return point;
}
/************************************************************************/
/* Работа со сценой                                                     */
/************************************************************************/

bool MoveStateTo(qdGameObjectState* ptrState, const Vect3f& pos)
{
	qdCoordsAnimation* ptrCA = ptrState->coords_animation();
	if(ptrCA)
	{
		if (!ptrCA->is_empty()) {
			ptrCA->get_point(0)->set_dest_pos(pos);
			return true;
		}
	}
	return false;
}

/*!
 	Если у активного состояния(CChildView::m_ptrActiveState) есть КА, и она(КА)
	имеет только одну точку, то изменяем положение этой точки.
	Если это динамический объект или персонаж, то устанавливаем еще и позицию 
	по умолчанию в эту точку
	\retval true - если положение изменено
*/
bool CChildView::MoveObjectTo(qdGameObject* ptrObj, Vect3f pos)
{
	if (!ptree_helpers::is_obj_static(ptrObj))
	{
		if (m_ptrActiveState)//прокручиваем состояние
		{
#ifndef USE_NEW_PROJ
			CSingleLock __sl(&m_csDrawGuard, TRUE);
#endif // USE_NEW_PROJ
			if (!m_ptrActiveState->coords_animation()->is_empty())
			{
				qdCoordsAnimation* const pca = 
					m_ptrActiveState->coords_animation();
				pca->get_point()->set_dest_pos(pos);
				pca->stop();
				pca->reset_cur_point();
				return true;
			}
			return false;
		}
		else
		{
			qdGameObjectAnimated* const ptrAObj = 
				static_cast<qdGameObjectAnimated*>(ptrObj);
			ptrAObj->set_default_pos(pos);
		}
	}
	ptrObj->set_pos(pos);
	if (ptree_helpers::is_obj_moving(ptrObj)) {
		static_cast<qdGameObjectMoving*>(ptrObj)->adjust_z();
		static_cast<qdGameObjectMoving*>(ptrObj)->set_default_pos(ptrObj->R());
		pos = ptrObj->R();
	}
	m_ptrUbeObjPos->set_new_pos(pos);
	return true;
}

void CChildView::ESOnMouseMove(UINT nFlags, const CPoint& point)
{
	if(m_bCACanDrag && MK_LBUTTON & nFlags)
	{
		ASSERT(m_ptrSelObj);
		bool bRedraw = false;

		ASSERT(getGameDispatcher());
		ASSERT(get_active_scene());
		qdCamera* ptrCam = get_camera();
		
		Vect3f vecOldPos = m_ptrSelObj->R();
		if (m_ptrActiveState&&!m_ptrActiveState->coords_animation()->is_empty())
		{
			vecOldPos = m_ptrActiveState->coords_animation()->get_point(0)->dest_pos();
		}
		Vect3f pos = vecOldPos;

		if(drag_point(m_ptrSelObj, pos, point, nFlags, ptrCam))
		{
			bRedraw = true;
			if (MoveObjectTo(m_ptrSelObj, pos))
			{
				m_bSelRectChanged = true;
				m_ptrSelObj->update_screen_pos();
				m_strToolTip.Format(_T("%s, D проекции на сетку = %d"), 
										m_ptrSelObj->name(), 
										round(m_ptrSelObj->screen_depth()));
				m_tt_point = point;
				log2scr(&m_tt_point, 1);
			}
		}
		if (bRedraw) {
			m_base_point = point;
			Redraw(RM_REDRAW_CONTENT);
			SetProjChange();
		}
	}
	else if (!nFlags) {
		if(qdGameScene* ptrScene=get_active_scene())
		{
			qdGameObject* ptrO = GetObjectFromPoint(point);
#ifndef USE_NEW_PROJ
			CSingleLock __sl(&m_csDrawGuard, TRUE);
#endif // USE_NEW_PROJ
			if (ptrO)
				m_strToolTip.Format(_T("%s, D проекции на сетку = %d"), ptrO->name(), 
								round(ptrO->screen_depth()));
			else
				m_strToolTip.Empty();

			m_tt_point = point;
			log2scr(&m_tt_point, 1);
			//закрашиваем предыдущее положение текста
			PaintRect(m_tt_rect);
		}
	}
}

BOOL CChildView::ESOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (!get_active_scene())
		return FALSE;
	if (m_ptrSelObj) {
		if (m_bCACanDrag&&IsKeyPressed(VK_SHIFT)&&IsKeyPressed(VK_LBUTTON)) 
			m_hCurrCursor = m_hDragHandCur;
		else
		{
			if (m_bSelRectChanged) 
				CalcSelRect();

			if (IsMouseOverSelected(GetCurPos())&&IsKeyPressed(VK_SHIFT)) {
				m_hCurrCursor = m_hHandCur;
			}
			else
				m_hCurrCursor = m_hCrossCursor;
		}
	}
	else
		m_hCurrCursor = m_hCrossCursor;
	return TRUE;
}	

void CChildView::ESOnLButtonDown(UINT nFlags, const CPoint &point)
{
	//присваиваем и здесь m_base_point для того, 
	//чтобы небыло дерганья при перетаскивании мышкой
	m_base_point = point;
	
	//если нажато еще что-то кроме                    
	//левой кнопки не выбираем объект, чтобы избежать
	//неприятностей
	qdGameObject *ptrObj = GetObjectFromPoint(point);
	
	if(nFlags!=MK_LBUTTON)
	{
		m_bCACanDrag = m_ptrSelObj != NULL;
		if (m_ptrActiveState&&m_ptrActiveState->coords_animation()->size()>1)
			m_bCACanDrag = false;

		if (m_bCACanDrag&&nFlags&MK_SHIFT) {
			SetCursor(m_hDragHandCur);

			m_ptrUbeObjPos->set_name(m_ptrSelObj->name());
			m_ptrUbeObjPos->set_old_pos(m_ptrSelObj->R());
			m_ptrUbeObjPos->set_new_pos(m_ptrSelObj->R());
		}
		return;
	}
	
	if(get_camera(get_active_scene()))
	{
		SelectObj( ptrObj);
		if (m_ptrSelObj)
#ifdef USE_NEW_PROJ
			getProject()->ShowObjectInTree(m_ptrSelObj);
#else
			ptree_api::ShowObjectInTree(m_ptrSelObj);
#endif // USE_NEW_PROJ
	}
}

void CChildView::ESOnLButtonUp(UINT nFlags, const CPoint &point)
{
	m_bCACanDrag = false;
	m_hCurrCursor = m_hCrossCursor;
	if (IsKeyPressed(VK_SHIFT)) {
		if (IsMouseOverSelected(point)) {
			m_hCurrCursor = m_hHandCur;
		}
	}
	if (!m_ptrUbeObjPos->empty()) {
		if (!(m_ptrUbeObjPos->old_pos() == m_ptrUbeObjPos->new_pos())) {
			m_ptrUndo->add(new ubeObjPos(*m_ptrUbeObjPos.get()));
		}
	}
	m_ptrUbeObjPos->set_name(_T(""));
	
	SetCursor(m_hCurrCursor);
}

void CChildView::ESOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_SHIFT&&!(nFlags>>13)) {
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		if (m_ptrSelObj&&m_SelObjRect.PtInRect(point)) 
			SetCursor(m_hHandCur);
	}
	bool bFirstTime = !(nFlags>>13);
	if (m_ptrSelObj&&IsKeyPressed(VK_SHIFT))
	{
		Vect3f shift(m_ptrSelObj->R());
		if (IsKeyPressed(VK_CONTROL)) {
			//сдвигаем по глубине
			qdCamera* pcam = get_camera();
			shift = pcam->global2camera_coord(shift);
			Vect2s rscr = pcam->camera_coord2rscr(shift);
			if(nChar == VK_UP) 
				shift.z += Z_CHANGE_DELTA;

			if(nChar == VK_DOWN) 
				shift.z -= Z_CHANGE_DELTA;
			shift = pcam->rscr2global(rscr, shift.z);
		}
		else
		{
			if(nChar == VK_LEFT) 
				shift.x += -1;

			if(nChar == VK_RIGHT) 
				shift.x += 1;

			if(nChar == VK_UP) 
				shift.y += 1;

			if(nChar == VK_DOWN) 
				shift.y += -1;
		}
		switch(nChar) {
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			if (bFirstTime) {
				m_ptrUbeObjPos->set_name(m_ptrSelObj->name());
				m_ptrUbeObjPos->set_old_pos(m_ptrSelObj->R());
			}

			MoveObjectTo(m_ptrSelObj, shift);

			m_ptrUbeObjPos->set_new_pos(m_ptrSelObj->R());
			SetProjChange();
			Redraw(RM_REDRAW_CONTENT);
		};
	}
	else if (!IsKeyPressed(VK_SHIFT)) 
	{
		int dx = 0, dy = 0;
		if(nChar == VK_LEFT) 
			dx = 1;

		if(nChar == VK_RIGHT) 
			dx = -1;

		if(nChar == VK_UP) 
			dy = 1;

		if(nChar == VK_DOWN) 
			dy = -1;
		if (dx || dy) {
			scrollbmp(dx, dy);
			UpdateScrollers();
		}
	}

	if (nChar == 'A'&&m_ptrSelObj&&!ptree_helpers::is_obj_moving(m_ptrSelObj)){
		if (bFirstTime) {
			m_ptrUbeObjPos->set_name(m_ptrSelObj->name());
			m_ptrUbeObjPos->set_old_pos(m_ptrSelObj->R());
		}
		Vect3f p = m_ptrSelObj->R();
		p.z += Z_CHANGE_DELTA;
		MoveObjectTo(m_ptrSelObj, p);
		m_ptrUbeObjPos->set_new_pos(m_ptrSelObj->R());
		SetProjChange();

		Redraw(RM_REDRAW_CONTENT);
	}
	else if (nChar == 'Z'&&m_ptrSelObj&&!ptree_helpers::is_obj_moving(m_ptrSelObj)){
		if (bFirstTime) {
			m_ptrUbeObjPos->set_name(m_ptrSelObj->name());
			m_ptrUbeObjPos->set_old_pos(m_ptrSelObj->R());
		}
		Vect3f p = m_ptrSelObj->R();
		p.z -= Z_CHANGE_DELTA;
		MoveObjectTo(m_ptrSelObj, p);
		m_ptrUbeObjPos->set_new_pos(m_ptrSelObj->R());
		SetProjChange();
		Redraw(RM_REDRAW_CONTENT);
	}

	if (m_ptrSelObj&&nChar == VK_RETURN) {
#ifdef USE_NEW_PROJ
		getProject()->ShowProperties(m_ptrSelObj);
#else
		ptree_api::ShowProperties(m_ptrSelObj);
#endif // USE_NEW_PROJ
	}
	if (nChar == VK_DELETE) {
		if(m_ptrSelObj)
#ifdef USE_NEW_PROJ
			getProject()->DeleteObject(m_ptrSelObj, true);
#else
			ptree_api::DeleteObject(m_ptrSelObj, true);
#endif // USE_NEW_PROJ
	}
}

void CChildView::ESOnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SetCursor(m_hCrossCursor);
	bool bRedraw = false;
	if (m_ptrSelObj&&IsKeyPressed(VK_SHIFT))
	{
		switch(nChar) {
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			{
				if (!m_ptrUbeObjPos->empty()) {
					if (!(m_ptrUbeObjPos->old_pos() == m_ptrUbeObjPos->new_pos())) 
					{
						m_ptrUndo->add(new ubeObjPos(*m_ptrUbeObjPos.get()));
					}
				}
				m_ptrUbeObjPos->set_name("");
			}
		};
	}	
	if (nChar == 'A'&&m_ptrSelObj&&!ptree_helpers::is_obj_moving(m_ptrSelObj)){
		if (!m_ptrUbeObjPos->empty()) {
			if (!(m_ptrUbeObjPos->old_pos() == m_ptrUbeObjPos->new_pos())) 
			{
				m_ptrUndo->add(new ubeObjPos(*m_ptrUbeObjPos.get()));
			}
		}
		m_ptrUbeObjPos->set_name("");
	}
	else if (nChar == 'Z'&&m_ptrSelObj&&!ptree_helpers::is_obj_moving(m_ptrSelObj)){
		if (!m_ptrUbeObjPos->empty()) {
			if (!(m_ptrUbeObjPos->old_pos() == m_ptrUbeObjPos->new_pos())) 
			{
				m_ptrUndo->add(new ubeObjPos(*m_ptrUbeObjPos.get()));
			}
		}
		m_ptrUbeObjPos->set_name("");
	}
	if (bRedraw)
	{
		Redraw(RM_REDRAW_CONTENT);
	}
}
//////////////////Работа со сценой////////////////////////////////////////////////////////

void CChildView::CalcSelRect()
{
	ASSERT(m_bSelRectChanged);
	if (m_ptrSelObj&&get_active_scene()) {
		Vect2s scr_pos = m_ptrSelObj->screen_pos();
		Vect2s scr_size = m_ptrSelObj->screen_size();
		m_SelObjRect = CRect(scr_pos.x - (scr_size.x>>1), scr_pos.y - (scr_size.y>>1),
			scr_pos.x + (scr_size.x>>1), scr_pos.y + (scr_size.y>>1));
		log2scr(reinterpret_cast<POINT*>(&m_SelObjRect), 2);
	}
	else
		m_SelObjRect.SetRectEmpty();
}

BOOL CChildView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (nHitTest == HTHSCROLL || nHitTest == HTVSCROLL)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		return TRUE;
	}
	if (!m_bCanDragScene) {
		switch(m_eViewState) {
		case VS_NONE:
			m_hCurrCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
			break;
		case VS_EDIT_SCENE:
			ESOnSetCursor(pWnd, nHitTest, message);
			break;
		case VS_EDIT_NET:
			NTOnSetCursor(pWnd, nHitTest, message);
			break;
		case VS_EDIT_COORD_ANIM:
			CAOnSetCursor(pWnd, nHitTest, message);
			break;
		case VS_EDIT_GAME_AREA:
			GAOnSetCursor(pWnd, nHitTest, message);
			break;
		case VS_EDIT_SCREEN_LAYOUT:
			m_hCurrCursor = m_hHandCur;
			break;
		case VS_EDIT_MASK:
			SMOnSetCursor(pWnd, nHitTest, message);
			break;
		case VS_EDIT_ANIMATED:
			EAOnSetCursor(pWnd, nHitTest, message);
			break;
		case VS_EDIT_STACK_STATE:
			SSOnSetCursor(pWnd, nHitTest, message);
			break;
		case VS_EDIT_STATE:
			ESTOnSetCursor(pWnd, nHitTest, message);
			break;
		case VS_EDIT_INVENTORY:
			EIOnSetCursor(pWnd, nHitTest, message);
			break;
		case VS_EDIT_INTERFACE:
//			m_ptrIntefEditMode->OnSetCursor(pWnd, nHitTest, message);
			m_ptrCurrentSceneViewMode->OnSetCursor(pWnd, nHitTest, message);
			break;
		}
	}
	::SetCursor(m_hCurrCursor);
	return TRUE;
}

/************************************************************************/
/* Редактирование размера и направления персонажа                       */
/************************************************************************/

/*!
	Вызывается из класса дерева проекта
	\param ptrPers - Объект, для которого проводится редактирование
*/
bool CChildView::OnEditAnimated(qdGameObjectAnimated* ptrPers)
{
	m_ptrEditedAnimated = ptrPers;
	return true;
}

/*!
	Вызывается из класса дерева проекта
	\param bSave - игнорируется
*/
void CChildView::OnAbortAnimatedEdit(bool bSave)
{
	m_ptrEditedAnimated = NULL;
}

void CChildView::EAOnLButtonDown(UINT nFlags, const CPoint &point)
{
	if (m_eSubTools == SSTE_EDIT_DIR) 
	{
		m_base_point = point;
		m_bCACanDrag = true;
	}
	else
	{
		Vect3f R(m_ptrEditedAnimated->R());
		R.z = get_camera()->get_grid_center().z;
		if (m_ptrEditedAnimated->has_bound())
			R.z += m_ptrEditedAnimated->bound().z/2.f;

//		CRect rect = GetObjBoundRect(get_camera(), m_ptrEditedAnimated,R);
//		if (rect.PtInRect(point)) {
			m_base_point = point;
			m_bCACanDrag = true;
			m_pers_bounds = m_ptrEditedAnimated->qdGameObjectAnimated::bound();
			SetCapture();
//		}
	}
}

void CChildView::EAOnLButtonUp(UINT nFlags, const CPoint &point)
{
	if (m_bCACanDrag) {
		ReleaseCapture();
		m_bCACanDrag = false;
		if (m_eSubTools == SSTE_EDIT_DIR) {

			ASSERT(ptree_helpers::is_obj_moving(m_ptrEditedAnimated));

			qdCamera* ptrCam = get_camera();

			Vect3f pos = ptrCam->scr2plane(Pnt2V2s(point));
			pos = pos - m_ptrEditedAnimated->R();
			pos.z = 0.0f;
			if(pos.norm2() >= 0.01f) 
			{			
				float fAngle = pos.psi();
				if (fAngle < 0) {
					fAngle += 2*M_PI;
				}
				qdGameObjectMoving* pm = 
					static_cast<qdGameObjectMoving*>(m_ptrEditedAnimated);
				
				pm->set_default_direction(fAngle);
				if(pm->set_direction(fAngle))
				{
					Redraw(RM_REDRAW_CONTENT);
					SetProjChange();
				}
			}
		}
		else if (m_eSubTools == SSTE_EDIT_DIMS) {
			if (ptree_helpers::is_obj_moving(m_ptrEditedAnimated)) {
				static_cast<qdGameObjectMoving*>(m_ptrEditedAnimated)->adjust_z();
			}
			Redraw(RM_REDRAW_CONTENT);
			SetProjChange();
		}
	}
}

void CChildView::EAOnLButtonDblClk(UINT nFlags, const CPoint &point)
{
}

void CChildView::EAOnMouseMove(UINT nFlags, const CPoint& point)
{
	if (m_bCACanDrag) {
		CPoint dT = point - m_base_point;
		if (m_eSubTools == SSTE_EDIT_DIMS) {

			if (nFlags&MK_SHIFT&&nFlags&MK_CONTROL) {
				m_pers_bounds.z = ::max(m_pers_bounds.z - dT.y, 0.f);
			}
			else if (nFlags&MK_SHIFT) {
				m_pers_bounds.x = ::max(m_pers_bounds.x + dT.x, 0.f);
				m_pers_bounds.y = ::max(m_pers_bounds.y - dT.y, 0.f);
			}
			
			m_ptrEditedAnimated->set_flag(QD_OBJ_HAS_BOUND_FLAG);
			Vect3f const old_bnd(m_ptrEditedAnimated->obj_bound());
			m_ptrEditedAnimated->set_bound(m_pers_bounds);
			if (ptree_helpers::is_obj_moving(m_ptrEditedAnimated)) {
				SaveScrLayoutOnBndEdt(
					m_ptrEditedAnimated->get_state(m_ptrEditedAnimated->cur_state()),
					get_camera(), old_bnd, m_pers_bounds);
				static_cast<qdGameObjectMoving*>(m_ptrEditedAnimated)->adjust_z();
			}
			Redraw(RM_REDRAW_CONTENT);
			SetProjChange();
			
		}
		else if (m_eSubTools == SSTE_EDIT_DIR) {
			ASSERT(ptree_helpers::is_obj_moving(m_ptrEditedAnimated));
			CPoint pt = point;
			if(GetLastMouseMove(m_hWnd, nFlags, pt))
				scr2log(&pt, 1);

			qdCamera* ptrCam = get_camera();
			Vect3f pos = ptrCam->scr2plane(Pnt2V2s(pt));
			
			pos = pos - m_ptrEditedAnimated->R();
			pos.z = 0.0f;
			if(pos.norm2() >= 0.01f) 
			{			
				float fAngle = pos.psi();
				if (fAngle < 0) 
					fAngle += 2*M_PI;
				
				qdGameObjectMoving* pm =
					static_cast<qdGameObjectMoving*>(m_ptrEditedAnimated);
				pm->set_default_direction(fAngle);
				if(pm->set_direction(fAngle))
				{
					Redraw(RM_REDRAW_CONTENT);
					SetProjChange();
				}					
			}
		}
		m_base_point = point;
	}
}

void CChildView::EAOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE) {
		SetSubTools(SSTE_NONE);
		SetViewState(VS_EDIT_SCENE);
		Redraw(RM_NO_CONTENT_REDRAW);
	}
}

void CChildView::EAOnRButtonUp(UINT nFlags, const CPoint& point)
{
	SetSubTools(SSTE_NONE);
	SetViewState(VS_EDIT_SCENE);
	Redraw(RM_NO_CONTENT_REDRAW);
}

BOOL CChildView::EAOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (!get_active_scene())
		return FALSE;
	Vect3f R(m_ptrEditedAnimated->R());
	R.z = get_camera()->get_grid_center().z;
	if (m_ptrEditedAnimated->has_bound())
		R.z += m_ptrEditedAnimated->bound().z/2.f;
//	CRect r = GetObjBoundRect(get_camera(), m_ptrEditedAnimated,R);

	CPoint p = GetLogCurPos(CPoint());

	if (m_eSubTools == SSTE_EDIT_DIR) {
		m_hCurrCursor = GetMyApp()->LoadCursor(IDC_CUR_ROTATE);
	}
	else //if (r.PtInRect(p)) 
	{
			m_hCurrCursor = GetMyApp()->LoadCursor(IDC_CUR_RESIZE);
	}
//	else
//		m_hCurrCursor = GetMyApp()->LoadStandardCursor(IDC_ARROW);
	return TRUE;
}

/************************************************************************/
/* редактирование боунда у состояния                                    */
/************************************************************************/
/*!
	Вызывается из класса дерева проекта
	\param pdlg		- Диалог редактирования размеров состояния
	\param pstate	- Состояние, размеры которого будем редактировать
*/
void CChildView::OnEditStateDims(class StateBoundDlg* pdlg, qdGameObjectState* pstate)
{
	m_ptrEditedState = pstate;
	m_ptrStateBoundDlg = pdlg;
	if (!m_ptrEditedState->has_bound()) {
		m_ptrEditedState->set_bound(static_cast<qdGameObjectAnimated*>
					(m_ptrEditedState->owner())->qdGameObjectAnimated::bound());
		m_ptrEditedState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);
		if (ptree_helpers::is_obj_moving(m_ptrEditedState->owner())) {
			static_cast<qdGameObjectMoving*>(m_ptrEditedState->owner())->adjust_z();
		}
	}
	Redraw(RM_REDRAW_CONTENT);
}

/*!
	Вызывается из класса дерева проекта
*/
void CChildView::OnEndEditStateDims(bool bSave)
{
	m_ptrEditedState = NULL;
	m_ptrStateBoundDlg = NULL;
}


void CChildView::ESTOnLButtonDown(UINT nFlags, const CPoint &point)
{
	Vect3f R(get_state_pos(m_ptrEditedState));
	R.z = get_camera()->get_grid_center().z;
	if (m_ptrEditedState->has_bound())
		R.z += m_ptrEditedState->bound().z/2.f;
//	CRect rect = GetObjBoundRect(get_camera(), m_ptrEditedState, R);
//	if (rect.PtInRect(point)) {
		m_base_point = point;
		m_bCACanDrag = true;
		m_pers_bounds = m_ptrEditedState->bound();
		SetCapture();
//	}
}

void CChildView::ESTOnLButtonUp(UINT nFlags, const CPoint &point)
{
	if (m_bCACanDrag) {
		ReleaseCapture();
		m_bCACanDrag = false;

		if (ptree_helpers::is_obj_moving(m_ptrEditedState->owner())) 
		{
			static_cast<qdGameObjectMoving*>(m_ptrEditedState->owner())->adjust_z();

			Redraw(RM_REDRAW_CONTENT);
			SetProjChange();
		}
	}
}

void CChildView::ESTOnMouseMove(UINT nFlags, const CPoint& point)
{
	if (m_bCACanDrag) 
	{
		CPoint dT = point - m_base_point;
		if (nFlags&MK_SHIFT&&nFlags&MK_CONTROL) {
			m_pers_bounds.z = ::max(m_pers_bounds.z - dT.y, 0.f);
		}
		else if (nFlags&MK_SHIFT) {
			m_pers_bounds.x = ::max(m_pers_bounds.x + dT.x, 0.f);
			m_pers_bounds.y = ::max(m_pers_bounds.y - dT.y, 0.f);
		}
		
		m_ptrEditedState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);
		Vect3f const old_bnd(m_ptrEditedState->bound());
		m_ptrEditedState->set_bound(m_pers_bounds);

		if (ptree_helpers::is_obj_moving(m_ptrEditedState->owner())) 
		{
//			if (!m_ptrEditedState->coords_animation()->is_empty())
			SaveScrLayoutOnBndEdt(m_ptrEditedState, get_camera(), old_bnd);

			static_cast<qdGameObjectMoving*>(m_ptrEditedState->owner())->adjust_z();
		}

		ASSERT(m_ptrStateBoundDlg);
		m_ptrStateBoundDlg->UpdateStateData();

		Redraw(RM_REDRAW_CONTENT);
		SetProjChange();
			
		m_base_point = point;
	}
}

void CChildView::ESTOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE)
#ifdef USE_NEW_PROJ
		getProject()->EndResizeState(EESTM_CANCEL);
#else
		ptree_api::EndResizeState(ptree_api::EESTM_CANCEL);
#endif // USE_NEW_PROJ

	if (nChar == VK_RETURN) 
#ifdef USE_NEW_PROJ
		getProject()->EndResizeState(EESTM_OK);
#else
		ptree_api::EndResizeState(ptree_api::EESTM_OK);
#endif // USE_NEW_PROJ
}
void CChildView::ESTOnRButtonUp(UINT nFlags, const CPoint& point)
{
#ifdef USE_NEW_PROJ
	getProject()->EndResizeState(EESTM_OK);
#else
	ptree_api::EndResizeState(ptree_api::EESTM_OK);
#endif // USE_NEW_PROJ
}

BOOL CChildView::ESTOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (!get_active_scene())
		return FALSE;
//	Vect3f base_pos(static_cast<qdGameObject*>(m_ptrEditedState->owner())->R());
	Vect3f base_pos(get_state_pos(m_ptrEditedState));
	base_pos.z = get_camera()->get_grid_center().z;
	if (m_ptrEditedState->has_bound())
		base_pos.z += m_ptrEditedState->bound().z/2.f;

//	CRect r = GetObjBoundRect(get_camera(), m_ptrEditedState,base_pos);

	CPoint p = GetLogCurPos(CPoint());

	if (m_eSubTools == SSTE_EDIT_DIR) {
		m_hCurrCursor = GetMyApp()->LoadCursor(IDC_CUR_ROTATE);
	}
	else //if (r.PtInRect(p)) 
	{
			m_hCurrCursor = GetMyApp()->LoadCursor(IDC_CUR_RESIZE);
	}
//	else
//		m_hCurrCursor = GetMyApp()->LoadStandardCursor(IDC_ARROW);
	return TRUE;
}

/************************************************************************/
/* Стыковка состояний                                                   */
/************************************************************************/

void CChildView::SSOnLButtonUp(UINT nFlags, const CPoint &point)
{
	if (m_eSubTools == SSTE_EDIT_DIMS)
		ReleaseCapture();
	m_bCACanDrag = false;
	m_ptrStateStacker->UpdateStateInfo();
}

void CChildView::SSRedrawStackedState(qdGameObjectState* ptrState, 
									  int dx, int dy, 
									  const Vect3f& vecOldPos)
{
	RedrawScene(get_active_scene());
	CRect r;
	GetClientRect(r);
	PaintRect(r);
}


void CChildView::SSOnMouseMoveEditPos(UINT nFlags, const CPoint& point)
{
	qdCamera* ptrCam = get_camera();

	qdGameObjectAnimated* pobj = 
		static_cast<qdGameObjectAnimated*>(m_ptrStackedState->owner());
	Vect3f pos(get_state_pos(m_ptrStackedState));
	Vect3f vecOldPos(pos);

	int dx = point.x - m_base_point.x;
	int dy = point.y - m_base_point.y;

	if(drag_point(pobj, pos, point, nFlags, ptrCam))
	{
		set_state_pos(m_ptrStackedState, pos, ptrCam->get_grid_center().z);
		SSRedrawStackedState(m_ptrStackedState, dx, dy, vecOldPos);
	}
}

void CChildView::SSOnMouseMoveEditOffset(UINT nFlags, const CPoint& point)
{
	if (nFlags&MK_SHIFT) {
		int dx0 = point.x - m_base_point.x;
		int dy0 = point.y - m_base_point.y;

		int dx = dx0;
		int dy = dy0;

		const eWHAT_SHIFT shift = m_ptrStateStacker->get_shift();
		if(shift == WS_IMAGE){
			float angle = static_cast<qdGameObjectAnimated*>(m_ptrStackedState->owner())->screen_rotation();

			float sn = sinf(angle);
			float cs = cosf(angle);

			dx = round(float(dx0) * cs + float(dy0) * sn);
			dy = round(-float(dx0) * sn + float(dy0) * cs);
		}
		
		if (m_ptrStateStacker->get_back_obj_state() != m_ptrStackedState)
		{
			Vect2s v = m_ptrStackedState->center_offset();
			if (shift == WS_IMAGE) 
			{
				v.x += dx;
				v.y += dy;
			}
			else if (shift == WS_CENTER) {
				MoveOCntr mc(Vect2i(dx, dy), m_ptrStackedState);
				v = m_ptrStackedState->center_offset() - mc(get_camera());
			}
			m_ptrStackedState->set_center_offset(v);
		}
		else//значит стыкуем два направления в походке
		{
			ASSERT(ptree_helpers::IsMovementState(m_ptrStackedState));
			int const iESAINum = m_ptrStateStacker->edited_state_ai_num();
			qdGameObjectStateWalk * const pws = 
				static_cast<qdGameObjectStateWalk*>(m_ptrStackedState);
			Vect2i v = pws->center_offset(iESAINum, m_ptrStateStacker->edited_state_type());
			v.x += dx;
			v.y += dy;
			pws->set_center_offset(iESAINum, v, m_ptrStateStacker->edited_state_type());
		}
		SSRedrawStackedState(m_ptrStackedState, dx, dy, 
			static_cast<qdGameObjectAnimated*>(m_ptrStackedState->owner())->R());
	}
}


void CChildView::SSOnMouseMoveEditBound(UINT nFlags,const CPoint& point)
{
	CPoint dT = point - m_base_point;
	if (nFlags&MK_SHIFT&&nFlags&MK_CONTROL) {
		m_pers_bounds.z = (std::max)(m_pers_bounds.z - dT.y, 0.f);
	}
	else if (nFlags&MK_SHIFT) {
		m_pers_bounds.x = (std::max)(m_pers_bounds.x + dT.x, 0.f);
		m_pers_bounds.y = (std::max)(m_pers_bounds.y - dT.y, 0.f);
	}
	
	m_ptrStackedState->set_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HAS_BOUND);
	Vect3f const old_bnd(m_ptrStackedState->bound());
	m_ptrStackedState->set_bound(m_pers_bounds);
	
	if (ptree_helpers::is_obj_moving(m_ptrStackedState->owner()))
	{
		SaveScrLayoutOnBndEdt(m_ptrStackedState, get_camera(), old_bnd);

		static_cast<qdGameObjectMoving*>(m_ptrStackedState->owner())->adjust_z();
	}

	Redraw(RM_REDRAW_CONTENT);
}

void CChildView::SSOnMouseMove(UINT nFlags, const CPoint& point)
{
	if(m_bCACanDrag && MK_LBUTTON & nFlags)
	{
		switch (m_eSubTools)
		{
		case SSTE_EDIT_STATE_POS: 
			SSOnMouseMoveEditPos(nFlags, point);
			break;
		case SSTE_EDIT_STATE_CENTER_OFFSET:
			SSOnMouseMoveEditOffset(nFlags, point);
			break;
		case SSTE_EDIT_DIMS:
			SSOnMouseMoveEditBound(nFlags, point);
			break;
		};
		m_base_point = point;
	}
}

void CChildView::SSOnLButtonDown(UINT nFlags, const CPoint &point)
{
	if (m_eSubTools == SSTE_EDIT_DIMS)
	{
//		Vect3f R = static_cast<qdGameObject*>(m_ptrStackedState->owner())->R();
		Vect3f R = get_state_pos(m_ptrStackedState);
		R.z = get_camera()->get_grid_center().z;
		if (m_ptrStackedState->has_bound())
			R.z += m_ptrStackedState->bound().z/2.f;
		CRect rect = GetObjBoundRect(get_camera(), m_ptrStackedState,R);
		if (rect.PtInRect(point)) {
			m_bCACanDrag = true;
			m_pers_bounds = m_ptrStackedState->bound();
			SetCapture();
		}
	}
	else
		m_bCACanDrag = true;
	m_base_point = point;
}

BOOL CChildView::SSOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (!get_active_scene())
		return FALSE;
	if (m_eSubTools == SSTE_EDIT_DIMS)
	{
		;
		Vect3f R (get_state_pos(m_ptrStackedState));
		//= static_cast<qdGameObject*>(m_ptrStackedState->owner())->R();
		R.z = get_camera()->get_grid_center().z;
		if (m_ptrStackedState->has_bound())
			R.z += m_ptrStackedState->bound().z/2.f;

		CRect r = GetObjBoundRect(get_camera(), m_ptrStackedState, R);

		if (r.PtInRect(GetLogCurPos(CPoint()))) 
			m_hCurrCursor = GetMyApp()->LoadCursor(IDC_CUR_RESIZE);
		else
			m_hCurrCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	}
	else 
	{
		if (m_bCACanDrag&&IsKeyPressed(VK_SHIFT)) {
			m_hCurrCursor = m_hDragHandCur;
		}
		else
			m_hCurrCursor = m_hHandCur;
	}
	return TRUE;
}


void CChildView::SSOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN)
#ifdef USE_NEW_PROJ
		getProject()->EndStateStack(EESTM_OK);
#else
		ptree_api::EndStateStack(ptree_api::EESTM_OK);
#endif // USE_NEW_PROJ
	else if (nChar == VK_ESCAPE)
#ifdef USE_NEW_PROJ
		getProject()->EndStateStack(EESTM_CANCEL);
#else
		ptree_api::EndStateStack(ptree_api::EESTM_CANCEL);
#endif // USE_NEW_PROJ

	if (nChar == 'A'&&m_eSubTools == SSTE_EDIT_STATE_POS) 
	{
		Vect3f pos = get_state_pos(m_ptrStackedState);
		pos.z += Z_CHANGE_DELTA;
		set_state_pos(m_ptrStackedState, pos, get_camera()->get_grid_center().z);

		m_ptrStateStacker->UpdateStateInfo();
		Redraw(RM_REDRAW_CONTENT);
	}
	else if (nChar == 'Z' && m_eSubTools == SSTE_EDIT_STATE_POS) 
	{
		Vect3f pos = get_state_pos(m_ptrStackedState);
		pos.z -= Z_CHANGE_DELTA;

		set_state_pos(m_ptrStackedState, pos, get_camera()->get_grid_center().z);

		m_ptrStateStacker->UpdateStateInfo();
		Redraw(RM_REDRAW_CONTENT);
	}
	if (IsKeyPressed(VK_SHIFT)
		&&(nChar == VK_LEFT
		||nChar == VK_RIGHT
		||nChar == VK_UP
		||nChar == VK_DOWN))
	{
		GetCursorPos(&m_base_point);
		scr2log(&m_base_point, 1);

		CPoint pt(m_base_point);
		switch(nChar) {
		case VK_LEFT:
			pt.x -= 1;
			break;
		case VK_RIGHT:
			pt.x += 1;
			break;
		case VK_UP:
			pt.y -= 1;
			break;
		case VK_DOWN:
			pt.y += 1;
			break;
		} 
		if (m_eSubTools == SSTE_EDIT_DIMS)
			m_pers_bounds = m_ptrStackedState->bound();
		UINT nFlags = MK_LBUTTON|MK_SHIFT;
		if (IsKeyPressed(VK_CONTROL))
			nFlags |= MK_CONTROL;
		m_bCACanDrag = true;
		SSOnMouseMove(nFlags, pt);
		m_bCACanDrag = false;
		m_ptrStateStacker->UpdateStateInfo();
	}
}

void CChildView::SSOnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

/************************************************************************/
/* UNDO                                                                 */
/************************************************************************/

BOOL CChildView::OnEditUndo(UINT nID) 
{
	if (!IsWindowVisible()) 
		return FALSE;
	ASSERT(!m_ptrUndo->undo_empty());
	if (!m_ptrUndo->undo_empty()) {
		ubentry* pube = m_ptrUndo->get_undo();
		ASSERT(pube);
		Undo(pube);
		delete pube;
	}
	//! операция отработана
	return TRUE;
}

BOOL CChildView::OnEditRedo(UINT nID) 
{
	if (!IsWindowVisible()) 
		return FALSE;
	ASSERT(!m_ptrUndo->redo_empty());
	if (!m_ptrUndo->redo_empty()) {
		ubentry* pube = m_ptrUndo->get_redo();
		ASSERT(pube);
		Undo(pube);
		delete pube;
	}
	return TRUE;
}

void CChildView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	if (IsWindowVisible()) 
		pCmdUI->Enable(!m_ptrUndo->undo_empty());	
	else
		pCmdUI->ContinueRouting();
}

void CChildView::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	if (IsWindowVisible()) 
		pCmdUI->Enable(!m_ptrUndo->redo_empty());
	else
		pCmdUI->ContinueRouting();
}

/*!
 	Определяет тип операции и вызывает соответствующуй функцию для обработки
	\param pube - данный для операции Undo
*/
void CChildView::Undo(ubentry* pube)
{
	switch(pube->type()) {
	case UBE_OBJ_POS:
		undoObjPos(static_cast<ubeObjPos*>(pube));
		break;
	}
}

void CChildView::undoObjPos(ubeObjPos* pe)
{
	qdGameScene* ptrScene = get_active_scene();
	ASSERT(ptrScene);

	qdGameObject* pobj = ptrScene->get_object(pe->obj_name());
	if(pobj)
	{
		pobj->set_pos(pe->old_pos());
		if(!ptree_helpers::is_obj_static(pobj))
		{
			qdGameObjectAnimated* pa = static_cast<qdGameObjectAnimated*>(pobj);
			pa->set_default_pos(pe->old_pos());
			MoveStateTo(pa->get_cur_state(), pe->old_pos());
		}
	}
	Redraw(RM_REDRAW_CONTENT);
}

//////////////////////////////////////////////////////////////////////////

/*!
	Производит расчет положения видимой области. И вычисляет насколько надо 
	её сдвинуть, чтобы она накрывала персонаж. Ни какие ограничения при рассчете 
	не учитываются
	\remarks Под видимой областью понимается часть сцены, видимая 
				в игре при заданном разрешении
*/
CSize CChildView::VATest(qdGameObjectMoving* ppers)
{
	qdCamera *ptrCam = get_camera();
		
	Vect2i vDiff = ptrCam->get_scr_center() - ptrCam->get_scr_center_initial();
	//! Вычисляем область по умолчанию
	m_rcVisibleArea.left = vDiff.x;
	m_rcVisibleArea.top = vDiff.y;
	m_rcVisibleArea.right = vDiff.x + qdGameConfig::get_config().screen_sx();
	m_rcVisibleArea.bottom = vDiff.y + qdGameConfig::get_config().screen_sy();

	if (!ppers) {
		return CSize(0,0);
	}

	Vect2s v(ppers->screen_pos());

	int fradius = round(ppers->radius()) + 20;

	CSize result(0, 0);
	//сначала левую границы
	if (m_rcVisibleArea.left > v.x - fradius) {
		result.cx = v.x - fradius - m_rcVisibleArea.left;
	}
	//если не выходит за левую, проверяем правую
	else if (m_rcVisibleArea.right < v.x  + fradius) {
		result.cx = v.x + fradius - m_rcVisibleArea.right;
	}
	if (m_rcVisibleArea.top > v.y - fradius) {
		result.cy = v.y - fradius - m_rcVisibleArea.top;
	}
	else if (m_rcVisibleArea.bottom < v.y + fradius) {
		result.cy = v.y + fradius - m_rcVisibleArea.bottom;
	}
	
	return result;
}

/*!
 	Производит сдвиг, не позволяя видимой области выходить за игровую( рабочую)
	область.
*/
bool CChildView::VAMove(CSize sz)
{
	qdCamera *ptrCam = get_camera();
	CPoint vCenter(V2s2Pnt(ptrCam->get_scr_center()));
	vCenter += V2s2Pnt(ptrCam->get_scr_offset());
	
	CPoint vLeftTop(vCenter.x - (ptrCam->get_scr_sx()>>1) 
		- (WORK_AREA_BOUND_THICKNESS>>1),
		vCenter.y - (ptrCam->get_scr_sy()>>1) 
		- (WORK_AREA_BOUND_THICKNESS>>1));
	
	CPoint vRightBottom(vCenter.x + (ptrCam->get_scr_sx()>>1) 
		+ (WORK_AREA_BOUND_THICKNESS>>1),
		vCenter.y + (ptrCam->get_scr_sy()>>1) 
		+ (WORK_AREA_BOUND_THICKNESS>>1));
	
	if(m_rcVisibleArea.left + sz.cx < vLeftTop.x)
		sz.cx = vLeftTop.x - m_rcVisibleArea.left;

	if (m_rcVisibleArea.right + sz.cx > vRightBottom.x)
		sz.cx = vRightBottom.x - m_rcVisibleArea.right;

	if (m_rcVisibleArea.top + sz.cy < vLeftTop.y) {
		sz.cy = vLeftTop.y - m_rcVisibleArea.top;
	}

	if (m_rcVisibleArea.bottom + sz.cy > vRightBottom.y) {
		sz.cy = vRightBottom.y - m_rcVisibleArea.bottom;
	}
	
	m_rcVisibleArea.OffsetRect(sz);
	return true;
}
/************************************************************************/
/* Редактирование инвентори                                             */
/************************************************************************/
BOOL CChildView::EIOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	m_hCurrCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	return TRUE;
}

void CChildView::EIOnLButtonDown(UINT nFlags, const CPoint &point)
{
	ASSERT(m_ptrActiveInventory);

	CPoint inv_point(point);
	log2scr_inv(&inv_point,1);

	int iActiveCellset = m_ptrActiveInventory->selected_cellset_num();
	if (!(nFlags&MK_SHIFT))
	{
		int res = m_ptrActiveInventory->hit(inv_point.x, inv_point.y);

		bool bRedraw = false;
		if (iActiveCellset != res)
		{
			if (iActiveCellset == -1){
				m_ptrActiveInventory->select_cellset(res);
				bRedraw = true;
			}
			else
			{
				CRect r = 
					GetCellsetRect(&m_ptrActiveInventory->cell_sets()[iActiveCellset]);
				scale_pts(&r);
				//ищем по-реальному прямоугольнику
				m_iDraggedAnchor = EIFindAnchor(inv_point, r);
				if (m_iDraggedAnchor == -1)
				{
					m_ptrActiveInventory->select_cellset(res);
					bRedraw = true;
				}
			}
		}
  		else if (iActiveCellset != -1)
		{
			CRect r = GetCellsetRect(
				&m_ptrActiveInventory->get_cell_set(iActiveCellset));

			scale_pts(&r);
			//ищем по-реальному прямоугольнику
			m_iDraggedAnchor = EIFindAnchor(inv_point, r);
		}
		if (bRedraw)
		{
			RedrawScene(get_active_scene());
			Invalidate();
		}
	}
	else if (nFlags&MK_SHIFT&&iActiveCellset != -1)
	{
		CPoint scene_lt(get_scene_lefttop(get_camera(), WORK_AREA_BOUND_THICKNESS));

		//прямоугольник в экранных координатах
		m_ActiveCellSetRect = 
			GetCellsetRect(&m_ptrActiveInventory->get_cell_set(iActiveCellset));

		scale_pts(&m_ActiveCellSetRect);

		m_iDraggedAnchor = EIFindAnchor(inv_point, m_ActiveCellSetRect);


		log2scr(&scene_lt, 1);
		m_ActiveCellSetRect.OffsetRect(scene_lt);
		m_rcHelper = m_ActiveCellSetRect;

		m_bCACanDrag = true;
		m_base_point = point;
		log2scr(&m_base_point, 1);
		DrawDraggedRect(m_ptrViewDC, m_ActiveCellSetRect, 0);
		GetCapture();
	}
}

void CChildView::EIOnLButtonUp(UINT nFlags, const CPoint &point)
{
	if (m_bCACanDrag)
	{
		ReleaseCapture();
		m_bCACanDrag = false;

		ASSERT(m_ptrActiveInventory->selected_cellset());

		const qdInventoryCellSet& set = 
				*m_ptrActiveInventory->selected_cellset();

		//прямоугольник в экранынх координатах
		CRect r = GetCellsetRect(&set);
		scale_pts(&r);

		if (r != m_ActiveCellSetRect)
		{
			CPoint scene_lt(get_scene_lefttop(get_camera(), WORK_AREA_BOUND_THICKNESS));
			log2scr(&scene_lt, 1);

			r.OffsetRect(scene_lt);
			Vect2s dT (m_ActiveCellSetRect.left - r.left,
						m_ActiveCellSetRect.top - r.top);

			CPoint cell_size(set.cell_size().x,set.cell_size().y);
			scale_pts(&cell_size, 1);

			Vect2s new_size;
			new_size.x = m_ActiveCellSetRect.Width()/cell_size.x;
			if (!new_size.x) new_size.x = 1;
			if (m_ActiveCellSetRect.Width() 
				- new_size.x*cell_size.x > (cell_size.x>>1))
				++new_size.x;
			
			new_size.y = m_ActiveCellSetRect.Height()/cell_size.y;
			if (!new_size.y) new_size.y = 1;
			if (m_ActiveCellSetRect.Height()-new_size.y*cell_size.y > (cell_size.y>>1))
				++new_size.y;
			
			Vect2s size(set.size());
			if (!(size == new_size))
				const_cast<qdInventoryCellSet&>(set).set_size(new_size);

			cell_size = V2s2Pnt(set.screen_pos());
			scale_pts(&cell_size, 1);
			cell_size.x += dT.x;
			cell_size.y += dT.y;
			descale_pts(&cell_size, 1);

			const_cast<qdInventoryCellSet&>(set).set_screen_pos(Pnt2V2s(cell_size));
			RedrawScene(get_active_scene());
			Invalidate();
			SetProjChange();
		}
	}
}

const CPoint CChildView::EICalcOffset(const CPoint& dt, 
									const CRect& rcDragged, const CRect &bound)
{
	CPoint p(dt);	
	if (m_iDraggedAnchor == -1)
	{
		if (rcDragged.left + dt.x < bound.left)
			p.x = bound.left - rcDragged.left;
		else if (rcDragged.right + dt.x > bound.right)
			p.x = bound.right - rcDragged.right;
		
		if (rcDragged.top + dt.y < bound.top)
			p.y = bound.top - rcDragged.top;
		else if (rcDragged.bottom + dt.y > bound.bottom)
			p.y = bound.bottom - rcDragged.bottom;
	}
	else
	{
		CRect cp(rcDragged);
		EIApplyAnchorChangeToRect(dt, cp);

		if (cp.left > cp.right)
			p.x = 0;
		else if (cp.left < bound.left)
			p.x = bound.left - rcDragged.left;
		else if (cp.right > bound.right)
			p.x = bound.right - rcDragged.right;
		
		if (cp.top > cp.bottom)
			p.y = 0;
		else if (cp.top < bound.top)
			p.y = bound.top - rcDragged.top;
		else if (cp.bottom > bound.bottom)
			p.y = bound.bottom - rcDragged.bottom;
	}
	
	return p;
}

void CChildView::EIFromHelperToVisible(const CRect &bound)
{
	if (m_iDraggedAnchor == -1)
	{
		int dt = 0;
		m_ActiveCellSetRect = m_rcHelper;

		CPoint scene_lt(get_scene_lefttop(get_camera(), WORK_AREA_BOUND_THICKNESS));
		log2scr(&scene_lt, 1);
		int stick_coord = 0;

		if (EIFindStickBound(scene_lt, m_rcHelper, STB_RIGHT, &stick_coord))
			dt = stick_coord - m_rcHelper.left;
		else if (EIFindStickBound(scene_lt, m_rcHelper, STB_LEFT, &stick_coord))
			dt = stick_coord - m_rcHelper.right;
		
		if (m_rcHelper.left - bound.left <= STICK_DISTANCE)
			dt = bound.left - m_rcHelper.left;
		else if (bound.right - m_rcHelper.right <= STICK_DISTANCE)
			dt = bound.right - m_rcHelper.right;

		if (dt)
			m_ActiveCellSetRect.OffsetRect(dt, 0);
		
		if (EIFindStickBound(scene_lt, m_rcHelper, STB_BOTTOM, &stick_coord))
			dt = stick_coord - m_rcHelper.top;
		else if (EIFindStickBound(scene_lt, m_rcHelper, STB_TOP, &stick_coord))
			dt = stick_coord - m_rcHelper.bottom;
		
		if (m_rcHelper.top - bound.top <= STICK_DISTANCE)
			dt = bound.top - m_rcHelper.top;
		else if (bound.bottom - m_rcHelper.bottom <= STICK_DISTANCE)
			dt = bound.bottom - m_rcHelper.bottom;

		if (dt)
			m_ActiveCellSetRect.OffsetRect(0, dt);
	}
	else
	{
		ASSERT(m_ptrActiveInventory->selected_cellset());
		const qdInventoryCellSet& set = *m_ptrActiveInventory->selected_cellset();

		CPoint cell_size(set.cell_size().x, set.cell_size().y);
		scale_pts(&cell_size, 1);

		CPoint scene_lt(get_scene_lefttop(get_camera(), WORK_AREA_BOUND_THICKNESS));
		log2scr(&scene_lt, 1);

		CRect rcSet = GetCellsetRect(&set);
		scale_pts(&rcSet);
		rcSet.OffsetRect(scene_lt);

		//ограничиваем сЪужение до размера одной ячейки
		if (m_rcHelper.left > rcSet.left)
			m_rcHelper.left = rcSet.left;
		if (m_rcHelper.top > rcSet.top)
			m_rcHelper.top = rcSet.top;
		if (m_rcHelper.right < rcSet.left + cell_size.x)
			m_rcHelper.right = rcSet.left + cell_size.x;
		if (m_rcHelper.bottom < rcSet.top + cell_size.y)
			m_rcHelper.bottom = rcSet.top + cell_size.y;

		m_ActiveCellSetRect.left = rcSet.left + 
			(m_rcHelper.left - rcSet.left)/cell_size.x * cell_size.x;

		m_ActiveCellSetRect.top = rcSet.top + 
			(m_rcHelper.top - rcSet.top)/cell_size.y * cell_size.y;

		m_ActiveCellSetRect.right = rcSet.right + 
			(m_rcHelper.right - rcSet.right)/cell_size.x * cell_size.x;

		m_ActiveCellSetRect.bottom = rcSet.bottom + 
			(m_rcHelper.bottom - rcSet.bottom)/cell_size.y * cell_size.y;

		if(m_ActiveCellSetRect.left == m_ActiveCellSetRect.right)
			m_ActiveCellSetRect.right += cell_size.x;

		if(m_ActiveCellSetRect.top == m_ActiveCellSetRect.bottom)
			m_ActiveCellSetRect.bottom += cell_size.y;
	}
}

void CChildView::EIOnMouseMove(UINT nFlags, const CPoint& point)
{
	if (m_bCACanDrag&&nFlags&MK_SHIFT)
	{

		CRect rcVisibleArea(0, 0, qdGameConfig::get_config().screen_sx(), 
			qdGameConfig::get_config().screen_sy());
		CPoint scene_lt = get_scene_lefttop(get_camera(), WORK_AREA_BOUND_THICKNESS);
		rcVisibleArea.OffsetRect(scene_lt);
		log2scr(reinterpret_cast<POINT*>(&rcVisibleArea), 2);

		CPoint temp(point);
		log2scr(&temp, 1);
		CPoint dT = temp - m_base_point;
		dT = EICalcOffset(dT, m_rcHelper, rcVisibleArea);

		DrawDraggedRect(m_ptrViewDC, m_ActiveCellSetRect, 0);
		if (m_iDraggedAnchor == -1)
			m_rcHelper.OffsetRect(dT);
		else
			EIApplyAnchorChangeToRect(dT, m_rcHelper);

		EIFromHelperToVisible(rcVisibleArea);
		DrawDraggedRect(m_ptrViewDC, m_ActiveCellSetRect, 0);
		m_base_point = temp;
	}
}

void CChildView::EIOnLButtonDblClk(UINT nFlags, const CPoint &point)
{
}

void CChildView::EIOnRButtonUp(UINT nFlags, const CPoint& point)
{
	ASSERT(m_ptrActiveInventory);
#ifdef USE_NEW_PROJ
	getProject()->ShowCorrespondingMenu(m_ptrActiveInventory);
#else
	ptree_api::ShowCorrespondingMenu(m_ptrActiveInventory);
#endif // USE_NEW_PROJ
}

void CChildView::EIOnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ASSERT(m_ptrActiveInventory);
	if (nChar == VK_DELETE)
	{
#ifdef USE_NEW_PROJ
		if (getProject()->DeleteInventoryCellset(m_ptrActiveInventory,
			m_ptrActiveInventory->selected_cellset_num()))
#else
		if (ptree_api::DeleteInventoryCellset(m_ptrActiveInventory,
			m_ptrActiveInventory->selected_cellset_num()))
#endif // USE_NEW_PROJ
		{
			RedrawScene(get_active_scene());
			Invalidate(FALSE);
		}
		else
			PlayError();
	}
	else if (nChar == VK_UP
		||nChar == VK_DOWN
		||nChar == VK_LEFT
		||nChar == VK_RIGHT)
	{
		qdInventoryCellSet& set = 
			const_cast<qdInventoryCellSet&>(*m_ptrActiveInventory->selected_cellset());
		Vect2s cellSetPos = set.screen_pos();
		Vect2i const cellSize = set.cell_size();

		Vect2i const cellSetSize = set.size();

		switch(nChar) {
		case VK_UP:
			cellSetPos.y -= 1;
			break;
		case VK_DOWN:
			cellSetPos.y += 1;
			break;
		case VK_LEFT:
			cellSetPos.x -= 1;
			break;
		case VK_RIGHT:
			cellSetPos.x += 1;
			break;
		}
		if (cellSetPos.x < cellSize.x>>1)
			cellSetPos.x = cellSize.x>>1;

		if (cellSetPos.y < cellSize.y>>1)
			cellSetPos.y = cellSize.y>>1;

		int right = cellSetPos.x - (cellSize.x>>1) + cellSize.x*cellSetSize.x;
		int bottom = cellSetPos.y - (cellSize.y>>1) + cellSize.y*cellSetSize.y;

		if (right > qdGameConfig::get_config().screen_sx())
			cellSetPos.x = qdGameConfig::get_config().screen_sx() 
								+ (cellSize.x>>1) - cellSize.x*cellSetSize.x;

		if (bottom > qdGameConfig::get_config().screen_sy())
			cellSetPos.y = qdGameConfig::get_config().screen_sy() +
							(cellSize.y>>1) - cellSize.y*cellSetSize.y;

		SetProjChange();
		set.set_screen_pos(cellSetPos);
		RedrawScene(get_active_scene());
		Invalidate(FALSE);
	}
}

void CChildView::log2scr_inv(POINT* p, int count)
{
	CPoint scene_lt(get_scene_lefttop(get_camera(), WORK_AREA_BOUND_THICKNESS));
	for(int i = 0; i < count; ++i){
		p[i].x -= scene_lt.x;
		p[i].y -= scene_lt.y;
	}
}

void CChildView::scr2log_inv(POINT* p, int count)
{
	CPoint scene_lt(get_scene_lefttop(get_camera(), WORK_AREA_BOUND_THICKNESS));
	for(int i = 0; i < count; ++i)
	{
		p[i].x += scene_lt.x;
		p[i].y += scene_lt.y;
	}
}

int CChildView::EIFindAnchor(const CPoint& point, const CRect &set_rc)
{
	CPoint const& lt(set_rc.TopLeft());
	CPoint const& rb(set_rc.BottomRight());

	CRect r = make_lefttop(lt, rb, CELL_SET_ANCHOR_WIDTH, CELL_SET_ANCHOR_WIDTH);
	if(r.PtInRect(point))
		return 0;
	
	r = make_leftmidle(lt, rb, CELL_SET_ANCHOR_WIDTH, CELL_SET_ANCHOR_WIDTH);
	if(r.PtInRect(point))
		return 1;
	
	r = make_leftbottom(lt, rb, CELL_SET_ANCHOR_WIDTH, CELL_SET_ANCHOR_WIDTH);
	if(r.PtInRect(point))
		return 2;
	
	r = make_bottommidle(lt, rb, CELL_SET_ANCHOR_WIDTH, CELL_SET_ANCHOR_WIDTH);
	if(r.PtInRect(point))
		return 3;
	
	r = make_rightbottom(lt, rb, CELL_SET_ANCHOR_WIDTH, CELL_SET_ANCHOR_WIDTH);
	if(r.PtInRect(point))
		return 4;
	
	r = make_rightmidle(lt, rb, CELL_SET_ANCHOR_WIDTH, CELL_SET_ANCHOR_WIDTH);
	if(r.PtInRect(point))
		return 5;
	
	r = make_righttop(lt, rb, CELL_SET_ANCHOR_WIDTH, CELL_SET_ANCHOR_WIDTH);
	if(r.PtInRect(point))
		return 6;
	
	r = make_topmidle(lt, rb, CELL_SET_ANCHOR_WIDTH, CELL_SET_ANCHOR_WIDTH);
	if(r.PtInRect(point))
		return 7;
	return -1;
}

bool CChildView::EIFindStickBound(const CPoint& scene_lt, const CRect& rcSrc,
								  eStickBound bound, int *pstick_coord)
{
	typedef qdInventoryCellSetVector::const_iterator ci_t;
	const qdInventoryCellSetVector& v = m_ptrActiveInventory->cell_sets();
	const qdInventoryCellSet* pactive = m_ptrActiveInventory->selected_cellset();

	CPoint c1(rcSrc.CenterPoint());
	ci_t i = v.begin(), e = v.end();
	for(; i != e; ++i)
	{
		const qdInventoryCellSet& set = *i;
		if (&set != pactive)
		{
			CRect r(GetCellsetRect(&set));
			scale_pts(&r);
			r.OffsetRect(scene_lt.x, scene_lt.y);

			CPoint c2 (r.CenterPoint());

			if (bound == STB_LEFT || bound == STB_RIGHT)
			{
				if (abs(c1.y - c2.y)>((r.Height()+rcSrc.Height())>>1))
					continue;
			}
			if (bound == STB_TOP || bound == STB_BOTTOM)
			{
				if (abs(c1.x - c2.x)>((r.Width()+rcSrc.Width())>>1))
					continue;
			}
			switch(bound) {
			case STB_LEFT:
				if (abs(r.left - rcSrc.right) <= STICK_DISTANCE){
					*pstick_coord = r.left;
					return true;
				}
				break;
			case STB_RIGHT:
				if (abs(r.right - rcSrc.left) <= STICK_DISTANCE){
					*pstick_coord = r.right;
					return true;
				}
				break;
			case STB_TOP:
				if (abs(r.top - rcSrc.bottom) <= STICK_DISTANCE){
					*pstick_coord = r.top;
					return true;
				}
				break;
			case STB_BOTTOM:
				if (abs(r.bottom - rcSrc.top) <= STICK_DISTANCE){
					*pstick_coord = r.bottom;
					return true;
				}
				break;
			}
		}
	}
	return false;
}

void CChildView::EIApplyAnchorChangeToRect(const CPoint &dt, RECT* lprc)
{
	switch(m_iDraggedAnchor) {
	case 0://левый верхний
		lprc->left		+= dt.x;
		lprc->top		+= dt.y;
		break;
	case 1://серединный левый
		lprc->left		+= dt.x;
		break;
	case 2://нижний левый
		lprc->left		+= dt.x;
		lprc->bottom	+= dt.y;
		break;
	case 3://серединный нижний
		lprc->bottom	+= dt.y;
		break;
	case 4://правый нижний
		lprc->right		+= dt.x;
		lprc->bottom	+= dt.y;
		break;
	case 5://серединный правый
		lprc->right		+= dt.x;
		break;
	case 6://правый верхний
		lprc->right		+= dt.x;
		lprc->top		+= dt.y;
		break;
	case 7://серединыый верхний
		lprc->top		+= dt.y;
		break;
	}
}

void CChildView::SetCursor(HCURSOR hCur)
{
	m_hCurrCursor = hCur;
}

void CChildView::SetScaleMgr(IScaleMgr* psmgr)
{
	m_ptrIScaleMgr = psmgr;
}

float CChildView::CurrentScale()
{
	return m_fScale;
}

void CChildView::IncreaseScale()
{
	SetScale(m_fScale/.85f);
}

void CChildView::DecreaseScale()
{
	SetScale(m_fScale*.85f);
}

/*!
 	Рассчитывает заново CChildView::m_pointBmpViewOrg, пределы масштабирования и
	скроллеры, обновляет данные в менеджере масштаба
*/
void CChildView::SetScale(float fScale)
{
	float fOldScale = m_fScale;
	m_fScale = fScale;
	
	CRect rectClient;
	GetClientRect(rectClient);

	//левый верхний угол окна в бмп до изменения масштаба
	CPoint ptLTWndInBmp = (-m_pointBmpViewOrg);
	ptLTWndInBmp.x = round(ptLTWndInBmp.x/fOldScale);
	ptLTWndInBmp.y = round(ptLTWndInBmp.y/fOldScale);
	
	//не подвижная точка это центр сцены
	CPoint ptConst(grDispatcher::instance()->Get_SizeX()>>1,
					grDispatcher::instance()->Get_SizeY()>>1);

	CPoint ptNewLTWndInBmp;
	if(grDispatcher::instance()->Get_SizeX() <= rectClient.Width()/m_fScale)
	{
		//если изображение полностью влазит в окно, 
		//то просто ставим его в центер
		ptNewLTWndInBmp.x = (grDispatcher::instance()->Get_SizeX() - 
			round(rectClient.Width()/m_fScale))>>1;
	}
	else
	{
		//иначе стягиваем(расстягиваем) к(от) центра(у)
		ptNewLTWndInBmp.x = ptConst.x - 
			round((ptConst.x- ptLTWndInBmp.x)*fOldScale/m_fScale);
		
		if (ptNewLTWndInBmp.x > grDispatcher::instance()->Get_SizeX() - 
				rectClient.Width()/m_fScale) {
			ptNewLTWndInBmp.x = round(grDispatcher::instance()
				->Get_SizeX() - rectClient.Width()/m_fScale);
		}

		if(ptNewLTWndInBmp.x < 0)
			ptNewLTWndInBmp.x = 0;

	}
	if(grDispatcher::instance()->Get_SizeY() <= rectClient.Height()/m_fScale)
	{
		ptNewLTWndInBmp.y = (grDispatcher::instance()->Get_SizeY()
			- round(rectClient.Height()/m_fScale))>>1;
	}
	else
	{

		ptNewLTWndInBmp.y = ptConst.y - 
			round((ptConst.y - ptLTWndInBmp.y)*fOldScale/m_fScale);
		
		if (ptNewLTWndInBmp.y > grDispatcher::instance()->Get_SizeY() - 
				rectClient.Height()/m_fScale) {
			ptNewLTWndInBmp.y = round(grDispatcher::instance()
				->Get_SizeY() - rectClient.Height()/m_fScale);
		}

		if(ptNewLTWndInBmp.y < 0)
			ptNewLTWndInBmp.y = 0;
	}
	m_pointBmpViewOrg.x = -round(ptNewLTWndInBmp.x*m_fScale);
	m_pointBmpViewOrg.y = -round(ptNewLTWndInBmp.y*m_fScale);

	m_bSelRectChanged = true;

	SetScrollRanges();
	UpdateScrollers();
	UpdateScaleInfo();
	Redraw(RM_REDRAW_CONTENT);
	Invalidate();
}

void CChildView::UpdateScaleInfo()
{
	if (m_ptrIScaleMgr)
		m_ptrIScaleMgr->UpdateScaleInfo();
}

void CChildView::SettingsChanged()
{
	AdjustBitmapSize();
}

boost::shared_ptr<AppSettingsBasePage> CChildView::getPage()
{
	boost::shared_ptr<AppSettingsBasePage> ptrPage(new SceneViewStngsPP);
	ptrPage->AddListener(this);
	return ptrPage;
}

BOOL CChildView::OnIdle(LONG lCount)
{
	ShowAnimation();
	return !getStopAnimation();
}

void CChildView::PauseAnimation()
{
	if (getStopAnimation()) 
		GetMyApp()->setIdleProcessor(this);
	else
		GetMyApp()->setIdleProcessor(NULL);
}

void CChildView::StartAnimation(qdGameObjectState* state)
{
	if(sndDispatcher* p = sndDispatcher::get_dispatcher())
		p -> enable();
	playedState_ = state;
	playedObject_ = static_cast<qdGameObjectAnimated*>(state->owner());
	playedObject_->set_state(state);
	setStopAnimation(FALSE);
	//! чтобы анимация не делала скачка
	getGameDispatcher()->update_time();
	GetMyApp()->setIdleProcessor(this);
}

void CChildView::StopAnimation(qdGameObjectState* state)
{
	setStopAnimation(TRUE);
	GetMyApp()->setIdleProcessor(NULL);
	if(sndDispatcher* p = sndDispatcher::get_dispatcher())
		p -> disable();
	playedState_ = NULL;
	playedObject_ = NULL;
	getGameDispatcher()->update_time();
	getGameDispatcher()->quant();
}

void CChildView::ShowAnimation()
{
	qdGameScene* ptrScene = getGameDispatcher()->get_active_scene();
	ASSERT(!IsBadReadPtr(ptrScene, sizeof(qdGameScene)));

	ASSERT(!IsBadReadPtr(getGameDispatcher(), sizeof(qdGameDispatcher)));

	ASSERT(playedObject_);
	ASSERT(playedState_);

	//активный объект используется при прорисовке
	//поэтому его переназначение должно принадлежать одному потоку
	ptrScene->activate_object(playedObject_);
	getGameDispatcher()->quant();

	Redraw(sv_api::RM_REDRAW_CONTENT);

	ptrScene->activate_object(NULL);

	if(playedObject_->state_status(playedState_)== qdGameObjectAnimated::STATE_DONE)
		playedObject_->set_state(playedState_);
}

void CChildView::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == TIMER_EVENT_PLAY_VIDEO) {
		if(getGameDispatcher()->is_video_playing())
			if(getGameDispatcher()->is_video_finished())
			{
				getGameDispatcher()->close_video();
				KillTimer(showVideoTimer_);
				showVideoTimer_ = (UINT)-1;
				Redraw(sv_api::RM_REDRAW_CONTENT);
			}
	}

	__super::OnTimer(nIDEvent);
}

bool CChildView::isVideoPlaying() const{
	return getGameDispatcher()->is_video_playing();
}

void CChildView::PlayStopVideo(qdVideo* video)
{
	if (getGameDispatcher()->is_video_playing()) 
	{
		getGameDispatcher()->stop_video();
		getGameDispatcher()->close_video();
	}
	getGameDispatcher()->play_video(video);
	showVideoTimer_ = SetTimer(TIMER_EVENT_PLAY_VIDEO, 1000, NULL);
}

void CChildView::StopVideo(qdVideo* video)
{
	if (getGameDispatcher()->is_video_playing(video)) {
		getGameDispatcher()->stop_video();
		getGameDispatcher()->close_video();
		Redraw(RM_REDRAW_CONTENT);
	}
	KillTimer(showVideoTimer_);
	showVideoTimer_ = (UINT)-1;
}

void CChildView::activate()
{
	ShowWindow(SW_SHOW);
	getSplitterClient()->SetResizedChild(this);
}

void CChildView::deactivate()
{
	ShowWindow(SW_HIDE);
	getSplitterClient()->SetResizedChild(NULL);
}

