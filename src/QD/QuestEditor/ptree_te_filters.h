/********************************************************************
	created:	2002/09/17
	created:	17:9:2002   18:50
	filename: 	D:\Projects\Quest\QuestEditor\ptree_te_filters.h
	file path:	D:\Projects\Quest\QuestEditor
	file base:	ptree_te_filters
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	фильтры, которые испльзуются деревом проекта для управления
				состоянием елементов в редакторе триггеров
*********************************************************************/

#ifndef _PTREE_TE_FILTERS__
#define _PTREE_TE_FILTERS__

#include "te_filter_base.h"
#include <boost/function.hpp>

class qdNamedObject;
//! Не пропускает ни одного элемента
class tef_null: public te_filter_base{
public:
	bool operator()(qdTriggerElementConstPtr const&) const{
		return false;
	}
};

//! Пропускает все элементы
class tef_no: public te_filter_base{
public:
	bool operator()(qdTriggerElementConstPtr const& ) const{
		return true;
	}
};

/*!
	Пропускает только те элементы, которые содержат объекты, принадлежащие сцене
	(tef_by_scene::pscene_)
*/ 
class tef_by_scene : public te_filter_base{
	qdNamedObject *pscene_;
public:
	tef_by_scene(qdNamedObject* ps):pscene_(ps){}

	bool operator()(qdTriggerElementConstPtr const& pe) const;
};

/*!
	Пропускает только те элементы, которые содержат объекты, принадлежащие сцене
	(tef_by_scene::pscene_) и удовлетворяющие фильтру объектов. 
*/ 

class tef_by_scenes_objs : public te_filter_base{
	qdNamedObject* pscene_;
	typedef boost::function1<bool, qdNamedObject const*> Filter;
	Filter obj_flt_;
public:
	tef_by_scenes_objs(qdNamedObject* pscene, Filter const&obj_flt)
		: pscene_(pscene)
		, obj_flt_(obj_flt){}
	  bool operator()(qdTriggerElementConstPtr const& pe) const;
};

/*!
	Пропускает только те элементы, которые содержат состояния, принадлежащие сцене
	(tef_by_scene::pscene_) и содержащие текст. 
*/ 
class tef_dlg_state : public te_filter_base{
	qdNamedObject* pscene_;
public:
	tef_dlg_state(qdNamedObject* ps):pscene_(ps){}
	bool operator()(qdTriggerElementConstPtr const& pe) const;
};

/*!
	Пропускает только те элементы, которые содержат объект(tef_by_obj::pobj_)
*/ 

class tef_by_obj : public te_filter_base
{
	qdNamedObject  const* const pobj_;
public:
	tef_by_obj(qdNamedObject const* const pobj) : pobj_(pobj){}
	bool operator() (qdTriggerElementConstPtr const& pe) const;
};

class tef_by_owner : public te_filter_base
{
	qdNamedObject const* const owner_;
public:
	tef_by_owner(qdNamedObject const* owner):owner_(owner){}
	bool operator() (qdTriggerElementConstPtr const& pe) const;
};
#endif//_PTREE_TE_FILTERS__