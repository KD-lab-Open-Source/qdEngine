/********************************************************************
	created:	2003/04/07
	created:	7:4:2003   16:35
	filename: 	d:\Projects\Quest\QuestEditor\sv_helpers.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	sv_helpers
	file ext:	h
	author:		Илюха
	
	purpose:	вспомогательные функции для отображения сцены в окне
*********************************************************************/
#pragma once

namespace sv_helpers{
/**
\name Функции создания якорей
	По левому верхнему и правому нижнему углам выдают прямоугольник 
	соответствующего якоря
*/
//@{
template <class _Point>
inline CRect make_lefttop(const _Point& vLeftTop, const _Point& vRightBottom, 
						  int width, int height)
{
	return CRect( vLeftTop.x - (width>>1), 
		vLeftTop.y - (height>>1),
		vLeftTop.x + (width>>1),
		vLeftTop.y + (height>>1));
}

template <class _Point>
inline CRect make_leftbottom(const _Point& vLeftTop, const _Point& vRightBottom, 
							 int width, int height)
{
	return CRect( vLeftTop.x - (width>>1), 
		vRightBottom.y - (height>>1),
		vLeftTop.x + (width>>1),
		vRightBottom.y + (height>>1));
}

template <class _Point>
inline CRect make_rightbottom(const _Point& vLeftTop, const _Point& vRightBottom, 
								int width, int height)
{
	return CRect( vRightBottom.x - (width>>1), 
		vRightBottom.y - (height>>1),
		vRightBottom.x + (width>>1),
		vRightBottom.y + (height>>1));
}

template <class _Point>
inline CRect make_righttop(const _Point& vLeftTop, const _Point& vRightBottom, 
						   int width, int height)
{
	return CRect( vRightBottom.x - (width>>1), 
		vLeftTop.y - (height>>1),
		vRightBottom.x + (width>>1),
		vLeftTop.y + (height>>1));
}

template <class _Point>
inline CRect make_leftmidle(const _Point& vLeftTop, const _Point& vRightBottom, 
							int width, int height)
{
	return CRect(vLeftTop.x - (width>>1),
		vLeftTop.y + ((vRightBottom.y - vLeftTop.y)>>1) - (height>>1),
		vLeftTop.x + (width>>1),
		vLeftTop.y + ((vRightBottom.y - vLeftTop.y)>>1) + (height>>1));
}

template <class _Point>
inline CRect make_rightmidle(const _Point& vLeftTop, const _Point& vRightBottom, 
							 int width, int height)
{
	return CRect(vRightBottom.x - (width>>1),
		vLeftTop.y + ((vRightBottom.y - vLeftTop.y)>>1) - (height>>1),
		vRightBottom.x + (width>>1),
		vLeftTop.y + ((vRightBottom.y - vLeftTop.y)>>1) + (height>>1));
}

template <class _Point>
inline CRect make_bottommidle(const _Point& vLeftTop, const _Point& vRightBottom, 
							  int width, int height)
{
	return CRect(vLeftTop.x + ((vRightBottom.x - vLeftTop.x)>>1) - (width>>1),
		vRightBottom.y - (height>>1),
		vLeftTop.x + ((vRightBottom.x - vLeftTop.x)>>1) + (width>>1),
		vRightBottom.y + (height>>1));
}

template <class _Point>
inline CRect make_topmidle(const _Point& vLeftTop, const _Point& vRightBottom, 
						   int width, int height)
{
	return CRect(vLeftTop.x + ((vRightBottom.x - vLeftTop.x)>>1) - (width>>1),
		vLeftTop.y - (height>>1),
		vLeftTop.x + ((vRightBottom.x - vLeftTop.x)>>1) + (width>>1),
		vLeftTop.y + (height>>1));
}
//@}

inline bool IsKeyPressed(int keyID){
	return (GetAsyncKeyState(keyID)>>15 != static_cast<short>(0));
}

enum eInverseMode {IM_INVERSE_OFF, IM_INVERSE_ON};
VOID DrawRectangle(int left, int top, int right, int bottom,
				   int color, int line_style, eInverseMode im);

template <class _Point>
VOID DrawRectangle(const _Point& vLeftTop, const _Point &vRightBottom, 
				   int color, int line_style = 0, eInverseMode im = IM_INVERSE_OFF){
	DrawRectangle(vLeftTop.x, vLeftTop.y, vRightBottom.x, vRightBottom.y,
		color, line_style, im);
}

VOID DrawAnchors(int left, int top, int right, int bottom,
				 int size, int color, int line_style);

template <class _Point>
VOID DrawAnchors(const _Point& lt, const _Point& rb, int size, 
								int color, int line_style = 0)
{
	DrawAnchors(lt.x, lt.y, rb.x, rb.y, size, color, line_style);
}


//находит якорь из набора якорей для прямоугольника
int FindAnchorFromRect(const CPoint& p, Vect2i const&lt, 
									   Vect2i const&rb, int const width);

//!	Отрисовывает нижнюю горизонтальную часть ячейки
void DrawYPart(qdCamera* pcam, const Vect2s& cell_index, COLORREF line_clr);
//! Отрисовывает левую вертикальную часть ячейки
void DrawXPart(qdCamera* pcam, const Vect2s& cell_index, COLORREF line_clr);

//! Изменение размера прямоугольника соответственно якорюц
void ChangeAnchorRect(CRect&r, int anchor, CPoint const &dl);
//! 
void AdjustDl(CPoint& dl);

//Vect2s to Point
template<class Vect2>
inline const CPoint V2_2Pnt(Vect2 const& v)
{
	return CPoint(v.x, v.y);
}
//Point to Vect2s
template<class Vect2>
inline Vect2 const Pnt2V2(const CPoint& p)
{
	return Vect2(p.x, p.y);
}

//Vect2s to Point
inline const CPoint V2s2Pnt(Vect2s const& v)
{
	return V2_2Pnt(v);
}
//Point to Vect2s
inline const Vect2s Pnt2V2s(CPoint const& p)
{
	return Pnt2V2<Vect2s>(p);
}

CPoint get_scene_lefttop(qdCamera const* pcam, int bound_thickness);

}//end of namespace	sv_helpers