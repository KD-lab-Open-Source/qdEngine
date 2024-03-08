/********************************************************************
	created:	2002/04/26
	created:	26:4:2002   12:34
	filename: 	D:\Projects\Quest\QuestEditor\helpers.cpp
	file path:	D:\Projects\Quest\QuestEditor
	file base:	helpers
	file ext:	cpp
	author:		Илюха
	
	purpose:	всякие вспогаловки
*********************************************************************/
#include "stdafx.h"
#include <vector>
#include <algorithm>
#include "helpers.h"

const float RAY_LEN = 10000.f;

bool sort_by_x(const Vect3f& p1, const Vect3f& p2)
{
	return (p1.x < p2.x);
}
namespace geom_helper{
//принимает плоский полигон и начальную точку,
//возвращаем набор точек пересечения горизонтального луча.
//луч идет в сторону возрастания Х
/*
bool DetectInts(const Vect3fVect_t& poly, const Vect3f& st_point, Vect3fVect_t& out)
{
	out.clear();
	Vect3fVect_t::const_iterator _beg = poly.begin(),_end = (poly.end()-1);
	//если первая и последняя точки совпадают, то не рассматриваем последнюю
	if(*_beg == *_end)
		--_end;

	Vect3f res(0, 0, 0);
	for(; _beg != _end; ++_beg)
	{
		if(cross_segms(*_beg, *(_beg+1), st_point, res, RAY_LEN))
			out.push_back(res);
	}

	if(cross_segms(*_end, *(poly.begin()), st_point, res, RAY_LEN))
		out.push_back(res);

	std::sort(out.begin(), out.end(), sort_by_x);

	ASSERT(out.size()%2 == 0);
	return !out.empty();
}
*/
bool cross_segms(const Vect3f& b, const Vect3f& e, const Vect3f& st_p, Vect3f& res, float len)
{
	float l = e.y - b.y;
	if(l == 0)//параллельны, так как лежат горизонтально
		return false;

	l = (st_p.y - b.y)/l;
	if(l < 0.f || l > 1.f)//точка пересечения лежит вне стороны полигона
		return false;

	l = (b.x - st_p.x + (e.x - b.x)*l)/len;
	if(l < 0.f || l > 1.f)//точка пересечения лежит вне луча
		return false;
	
	res.y = st_p.y;
	res.x = st_p.x + l*len;
	return true;
}

}