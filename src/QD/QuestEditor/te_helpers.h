/********************************************************************
	created:	2002/09/17
	created:	17:9:2002   12:41
	filename: 	D:\Projects\Quest\QuestEditor\te_helpers.h
	file path:	D:\Projects\Quest\QuestEditor
	file base:	te_helpers
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	вспомогательные функции для редактора триггеров
*********************************************************************/
#ifndef _TE_HELPERS_HEADER_
#define _TE_HELPERS_HEADER_

namespace te_h{

//Удалеят все линки у элемента
VOID RemoveLinks(class qdTriggerChain* pc, qdTriggerElementPtr const& pe);

/// определяет какую сторону перескает луч в прямоугольнике
/// луч исходит из точки ВНУТРИ!!! прямоугольника
BYTE check_inters(const CPoint& a, const CPoint& b, const CPoint* pts,
								float* pkx, float* pky);


/// Возвращает центр прямоугольника
inline CPoint const get_center(RECT const& r)
{
	return CPoint((r.right + r.left)>>1, (r.bottom + r.top)>>1);
}

/// Средную точку отрезка
inline CPoint const get_center(CPoint const& a, CPoint const& b)
{
	return CPoint((a.x + b.x)>>1, (a.y + b.y)>>1);
}
/// Левый верхний угол прямоугольника
inline CPoint const get_lt(RECT const& r)
{
	return CPoint(r.left, r.top);
}

/// Поворачивает точку на угол fAngle
inline CPoint const rotate(const CPoint& ort, const float fAngle)
{
	CPoint res;
	res.x = round(ort.x*cos(fAngle) + ort.y*sin(fAngle));
	res.y = round(ort.x*sin(fAngle) - ort.y*cos(fAngle));
	return res;
}

/// Находит в некотором контейнере элементов элемент, в который попадает точка
template<class _Cont>
qdTriggerElementPtr find_in_tc(const _Cont& c, const CPoint& p)
{
	BOOST_STATIC_ASSERT((boost::is_same<_Cont::value_type, 
							qdTriggerElementPtr>::value == 1));
	
	_Cont::const_iterator _itr = c.begin(), _end = c.end();
	for(; _itr != _end; ++_itr)
	{
		qdTriggerElementPtr const& ptr = *_itr;
		if (PtInRect(&ptr->bounding_rect(), p)) {
			return ptr;
		}
	}
	return qdTriggerElementPtr(NULL);
}

typedef CPoint TRIANGLE[3] ;
/// Вычисляет по начальной и конечной точкам треугольник для линка
void get_link_triangle(CPoint const& pt_from, CPoint const& pt_to, TRIANGLE &trg);

/// Выдает коориднату ячейки, в которую попадает \a x
inline int to_cell_begin(int x, int cell_sz, int among_cells_sz)
{
	int len = cell_sz + among_cells_sz;
	if (x >= 0)
		return x/len * len; 
	else
		return (x/len - 1)*len;
}

/// по координате возвращает индекс ячейки
inline int get_cell_idx(int x, int cell_sz)
{
	return x/cell_sz;
}

/// по индексу возвращает начало ячейки
inline int get_cell_begin(int idx, int cell_sz){
	return idx* cell_sz;
}

/// возвращает активный элемент в цепочке
qdTriggerElementConstPtr get_active_ele(const qdTriggerChain* ptrChain);
/// Возвращает имя сцены, которой принадлежин объект \a po
LPCTSTR get_scene_name(qdNamedObject* po);


/// Нажата ли указанная кнопка 
inline bool IsKeyPressed(int keyID)
{
	return static_cast<bool>(GetAsyncKeyState(keyID)>>15);
}

/// Находится ли точка в треугольнике
bool PointInTrg( CPoint const& test, TRIANGLE const& trg);
}//namespace te_h


#endif//_TE_HELPERS_HEADER_