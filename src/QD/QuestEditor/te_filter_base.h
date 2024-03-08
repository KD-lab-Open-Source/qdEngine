/********************************************************************
	created:	2002/09/17
	created:	17:9:2002   18:18
	filename: 	D:\Projects\Quest\QuestEditor\te_filter_base.h
	file path:	D:\Projects\Quest\QuestEditor
	file base:	te_filter_base
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	базовый класс для фильтров, предназначенных фильтровать
				элементы в цепочках триггеров.
*********************************************************************/

#ifndef __TE_FILTER_BASE__
#define __TE_FILTER_BASE__

#include "trigger_element_fwd.h"
//class qdTriggerElementConstPtr;
//! базовый класс для фильтров элементов цепочек триггеров
class te_filter_base{
public:
	virtual bool operator()(qdTriggerElementConstPtr const& pe) const = 0;
};

#endif//__TE_FILTER_BASE__
