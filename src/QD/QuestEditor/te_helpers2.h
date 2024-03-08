/********************************************************************
	created:	2003/05/23
	created:	23:5:2003   13:58
	filename: 	d:\Projects\Quest\QuestEditor\te_helpers2.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	te_helpers2
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	Сюда вынесены функци, которые требуют информации о
				qdTrigger...
*********************************************************************/
#pragma once
#include "te_helpers.h"
#include "qd_trigger_element.h"

namespace te_h
{
	/// Вычисляет точку начала линка
	inline CPoint get_link_begin(qdTriggerElementConstPtr link_owner, 
				qdTriggerLink const* link)
	{
		return (get_center(link_owner->bounding_rect()) 
			+ link->get_owner_offset());
	}

	/// Вычисляет точку конца линка
	inline CPoint get_link_end(qdTriggerLink const* link)
	{
		return (get_center(link->element()->bounding_rect())
			+ link->get_child_offset());
	}

	/// Вычисляет заново номера ячеек для элемента
	inline void recalc_cell_number(qdTriggerElementPtr pe, 
		CPoint const& p, int cell_w, int cell_h)
	{
		pe->set_cell_number(get_cell_idx(p.x, cell_w),get_cell_idx(p.y, cell_h));
	}

}//namespace te_h