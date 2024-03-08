#include "stdafx.h"
#include "sv_helpers.h"

#include "gr_dispatcher.h"

namespace sv_helpers
{

VOID DrawRectangle(int left, int top, int right, int bottom,
				   int color, int line_style, eInverseMode im)
{
	grDispatcher* gd = grDispatcher::instance();
	gd->Line(left, top, left, bottom, color, line_style,im == IM_INVERSE_ON);
	gd->Line(left, top,	right, top, color, line_style, im == IM_INVERSE_ON);
	gd->Line(right,bottom,left,bottom,color, line_style, im == IM_INVERSE_ON);
	gd->Line(right, bottom, right,top, color,line_style, im == IM_INVERSE_ON);
}

VOID DrawAnchors(int left, int top, int right, int bottom,
				 int size, int color, int line_style)
{

	grDispatcher* gd = grDispatcher::instance();
	ASSERT(gd);
	//угловые якоря
	gd->Rectangle(left - (size>>1), top - (size>>1),size, size,
		color, color,GR_FILLED, line_style);
	gd->Rectangle(left - (size>>1), bottom - (size>>1), size, size,
		color, color, GR_FILLED, line_style);
	gd->Rectangle(right - (size>>1), bottom - (size>>1), size, size,
		color, color, GR_FILLED, line_style);
	gd->Rectangle(right - (size>>1), top - (size>>1), size, size,
		color, color,GR_FILLED, line_style);
	//срединные якоря
	int _t = (right - left)>>1;
	//верхний
	gd->Rectangle(left + _t - (size>>1), top - (size>>1), size, size,
		color, color, GR_FILLED, line_style);

	//нижний
	gd->Rectangle(left + _t - (size>>1), bottom - (size>>1), size, size,
		color, color, GR_FILLED, line_style);

	_t = (bottom - top)>>1;
	//левый
	gd->Rectangle(left - (size>>1), top + _t - (size>>1), size, size,
		color, color, GR_FILLED, line_style);

	//правый
	gd->Rectangle(right - (size>>1), top  + _t - (size>>1), size, size,
		color, color, GR_FILLED, line_style);

}

//находит якорь из набора якорей для прямоугольника
int FindAnchorFromRect(const CPoint& p, Vect2i const&lt, 
									   Vect2i const&rb, int const width)
{
	if (make_lefttop(lt, rb, width, width).PtInRect(p))
		return 0;

	if (make_leftmidle(lt, rb, width, width).PtInRect(p))
		return 1;

	if (make_leftbottom(lt, rb, width, width).PtInRect(p))
		return 2;

	if (make_bottommidle(lt, rb, width, width).PtInRect(p))
		return 3;

	if (make_rightbottom(lt, rb, width, width).PtInRect(p))
		return 4;
	
	if (make_rightmidle(lt, rb, width, width).PtInRect(p))
		return 5;
	
	if (make_righttop(lt, rb, width, width).PtInRect(p))
		return 6;
	
	if (make_topmidle(lt, rb, width, width).PtInRect(p))
		return 7;
	
	return -1;
}

//! Отрисовывает левую вертикальную часть ячейки
void DrawXPart(qdCamera* pcam, const Vect2s& cell_index, COLORREF line_clr){
	//! Нижний левый угол
	Vect3f v(pcam->get_cell_coords(cell_index));
	v.x -= pcam->get_cell_sx()>>1;
	v.y += pcam->get_cell_sy()>>1;

	//! Верхний левый угол
	Vect3f v2(v);
	v2.y -= pcam->get_cell_sy();

	Vect2s scr(pcam->global2scr(v));
	Vect2s scr2(pcam->global2scr(v2));
	grDispatcher::instance()->
		Line(scr.x, scr.y, scr2.x, scr2.y, line_clr, 0, true);
}

//!	Отрисовывает нижнюю горизонтальную часть ячейки
void DrawYPart(qdCamera* pcam, const Vect2s& cell_index, COLORREF line_clr){
	//! левый нижний угол
	Vect3f v(pcam->get_cell_coords(cell_index));
	v.x -= pcam->get_cell_sx()>>1;
	v.y -= pcam->get_cell_sy()>>1;

	//! правый нижний угол
	Vect3f v2(v);
	v2.x += pcam->get_cell_sx();

	Vect2s scr(pcam->global2scr(v));
	Vect2s scr2(pcam->global2scr(v2));
	grDispatcher::instance()->
		Line(scr.x, scr.y, scr2.x, scr2.y, line_clr, 0, true);
}

void ChangeAnchorRect(CRect&r, int anchor, CPoint const &dl)
{
	switch(anchor) {
	case 0://левый верхний
		r.left		+= dl.x;
		r.top		+= dl.y;
		r.left		= (std::min)(r.left, r.right);
		r.top		= (std::min)(r.top, r.bottom);
		break;
	case 1://серединный левый
		r.left		+= dl.x;
		r.left		= (std::min)(r.left, r.right);
		break;
	case 2://нижний левый
		r.left		+= dl.x;
		r.bottom	+= dl.y;
		r.left		= (std::min)(r.left, r.right);
		r.bottom	= (std::max)(r.top, r.bottom);
		break;
	case 3://серединный нижний
		r.bottom	+= dl.y;
		r.bottom	= (std::max)(r.top, r.bottom);
		break;
	case 4://правый нижний
		r.right		+= dl.x;
		r.bottom	+= dl.y;
		r.right		= (std::max)(r.left, r.right);
		r.bottom	= (std::max)(r.top, r.bottom);
		break;
	case 5://серединный правый
		r.right		+= dl.x;
		r.right		= (std::max)(r.left, r.right);
		break;
	case 6://правый верхний
		r.right		+= dl.x;
		r.top		+= dl.y;
		r.right		= (std::max)(r.left, r.right);
		r.top		= (std::min)(r.top, r.bottom);
		break;
	case 7://серединыый верхний
		r.top		+= dl.y;
		r.top		= (std::min)(r.top, r.bottom);
		break;
	}
}

void AdjustDl(CPoint& dl)
{
	int mod = dl.x%2;
	if (mod>0)
		++dl.x;
	else if (mod < 0)
		--dl.x;

	mod = dl.y%2;
	if (mod>0)
		++dl.y;
	else if (mod < 0)
		--dl.y;
}

CPoint get_scene_lefttop(qdCamera const* pcam, int bound_thickness)
{
	Vect2s vCenter(pcam->get_scr_center());
	vCenter += pcam->get_scr_offset();

	return CPoint (vCenter.x - (pcam->get_scr_sx()>>1) 
		- (bound_thickness>>1),
		vCenter.y - (pcam->get_scr_sy()>>1) 
		- (bound_thickness>>1));
}

}