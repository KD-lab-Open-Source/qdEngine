/********************************************************************
	created:	2002/09/24
	created:	24:9:2002   18:12
	filename: 	D:\Projects\Quest\QuestEditor\te_helpers.cpp
	file path:	D:\Projects\Quest\QuestEditor
	file base:	te_helpers
	file ext:	cpp
	Powerd by:	Илюха
	
	purpose:	невстроенные вспомогательные функции
*********************************************************************/
#include "stdafx.h"
#include "questeditor.h"

#include "qd_trigger_chain.h"
#include <algorithm>

#include "te_helpers.h"
#include "helpers.h"

namespace te_h{

/**
	Удаляются сначала дочерние связи
	А потом родительские
 */
void RemoveLinks(qdTriggerChain* pc, qdTriggerElementPtr const& pe)
{
	qdTriggerLinkList& tll = pe->children();
	qdTriggerLinkList::iterator i = tll.begin();
	for(; i != tll.end(); i = tll.begin())
	{
		qdTriggerLink* pl = &*i;
		
		pc->remove_link(pe, pl->element());
	}
	
	qdTriggerLinkList& tll2 = pe->parents();
	for(i = tll2.begin(); i != tll2.end(); i = tll2.begin())
	{
		qdTriggerLink* pl = &*i;
		pc->remove_link(pl->element(), pe);
	}
}

BYTE check_inters(const CPoint& a, const CPoint& b, const CPoint* pts, 
				  float* pkx, float *pky)
{
	for(BYTE i = 0; i < 3; ++i)
	{
		if(geom_helper::cross_segms(a, b, pts[i], pts[i+1],pkx, pky))
			return i;
	}
	//какая-то одна сторона должна пересекаться. Если не первые 3 то точно последняя
	VERIFY(geom_helper::cross_segms(a,b, pts[3], pts[0], pkx, pky));
	return 3;
}

qdTriggerElementConstPtr get_active_ele(const qdTriggerChain* ptrChain){
	if (ptrChain->root_element()->is_active()) 
		return ptrChain->root_element();
	const qdTriggerElementList& l = ptrChain->elements_list();
	qdTriggerElementList::const_iterator i = l.begin(), e = l.end();
	for(; i != e; ++i)
		if ((*i)->is_active())
			return *i;
	return NULL;
}

LPCTSTR get_scene_name(qdNamedObject* po){
	qdNamedObject* p = po->ref_owner();
	while (p&&p->named_object_type() != QD_NAMED_OBJECT_DISPATCHER) {
		if (p->named_object_type() == QD_NAMED_OBJECT_SCENE) 
			return p->name();
		p = p->ref_owner();
	}
	return NULL;
}

/// Возвращает знак \a value
int Sign(int value)
{
	return value < 0? -1 : 1;
}
/// \brief По начальной и конечной точке вычисляет коэффициэнты уравнения прямой
/// и вычисляет это уравнение для точки \a p
int CalcLineEquation(POINT const& a, POINT const& b, POINT const& p)
{
	int const A = b.y - a.y;
	int const B = b.x - a.x;
	int const C = -A*a.x + B*a.y;
	return A*p.x - B*p.y + C;
}
bool PointInTrg(CPoint const& test, TRIANGLE const& trg)
{
	int testRes = CalcLineEquation(trg[0], trg[1], test);
	int vertexRes = CalcLineEquation(trg[0], trg[1], trg[2]);
	if (Sign(testRes) != Sign(vertexRes))
		return false;
	testRes = CalcLineEquation(trg[1], trg[2], test);
	vertexRes = CalcLineEquation(trg[1], trg[2], trg[0]);
	if (Sign(testRes) != Sign(vertexRes))
		return false;
	testRes = CalcLineEquation(trg[2], trg[0], test);
	vertexRes = CalcLineEquation(trg[2], trg[0], trg[1]);
	if (Sign(testRes) != Sign(vertexRes))
		return false;
	return true;
}

void get_link_triangle(CPoint const& pt_from, CPoint const& pt_to, TRIANGLE &trg)
{
	Vect2f ort_f(pt_to.x - pt_from.x, pt_to.y - pt_from.y);
	ort_f.normalize(10.f);//получаем  единичные вектор * параметр функции
	Vect2f normal(ort_f.y, -ort_f.x);

	const CPoint ort_x2(round(ort_f.x)<<1, round(ort_f.y)<<1);
//	TRIANGLE trg;
	//точка, к которой будет сходиться стрела
	trg[0] = pt_to;
	trg[1] = CPoint(pt_to - ort_x2);
	trg[2] = CPoint(pt_to - ort_x2);

	trg[1].x += round(normal.x)>>1;
	trg[1].y += round(normal.y)>>1;

	trg[2].x += round(-normal.x)>>1;
	trg[2].y += round(-normal.y)>>1;
}
/*
CRect get_sens_lk_rc(const CPoint& f, const CPoint& t)
{
	//	CPoint cntr(get_center(f, t));
	Vect2f ort_f(t.x - f.x, t.y - f.y);


	ort_f.normalize(10.f);//получаем удвоенный единичные вектор
	Vect2f normal(ort_f.y, -ort_f.x);
	CRect r(round(ort_f.x)<<1, round(ort_f.y)<<1, 0, 0);

	CPoint trg[3];
	//точка, к которой будет сходиться стрела
	trg[0] = t;
	trg[1] = CPoint(trg[0] - r.TopLeft());
	trg[2] = CPoint(trg[0] - r.TopLeft());

	trg[1].x += round(normal.x)>>1;
	trg[1].y += round(normal.y)>>1;

	trg[2].x += round(-normal.x)>>1;
	trg[2].y += round(-normal.y)>>1;

	r.left = (std::min)(trg[0].x, (std::min)(trg[1].x, trg[2].x));	
	r.right = (std::max)(trg[0].x, (std::max)(trg[1].x, trg[2].x));	

	r.top = (std::min)(trg[0].y, (std::min)(trg[1].y, trg[2].y));	
	r.bottom = (std::max)(trg[0].y, (std::max)(trg[1].y, trg[2].y));	

	return r;
}
*/

}//end of namespace te_h