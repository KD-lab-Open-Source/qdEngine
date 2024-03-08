#include "stdafx.h"
#include "ptree_te_filters.h"

#include "obj_type_detectors.h"
#include "ptree_hlprs.hpp"
#include "qd_trigger_element.h"


bool tef_by_scene::operator()(qdTriggerElementConstPtr const& pe) const{
	qdNamedObject* po = pe->object();
	if (!po||!pscene_)return false;
	if (po == pscene_) {//т.е. элемент содержит саму сцену
		return true;
	}
	return (po->owner()==po->ref_owner()&&po->owner()->owner() == pscene_);
}

bool tef_dlg_state::operator()(qdTriggerElementConstPtr const& pe) const{
	qdNamedObject* po = pe->object();
	if (!po) return false;
	if (ptree_helpers::is_dlg_state(po)) {
		if (pscene_) {
			return (po->owner()->owner() == pscene_);
			return true;
		}
	}
	return false;
}


bool tef_by_scenes_objs::operator()(qdTriggerElementConstPtr const& pe) const{
	qdNamedObject* po = pe->object();
	if (!po) return false;

	if (obj_flt_(po->owner()))
	{
		if (pscene_) 
			return (po->owner()->owner() == pscene_);
		return true;
	}
	return false;
}


bool tef_by_obj::operator() (qdTriggerElementConstPtr const& pe) const{
	qdNamedObject const* const po = pe->object();
	return (po&&po == pobj_);
}

bool tef_by_owner::operator() (qdTriggerElementConstPtr const& pe) const{
	qdNamedObject const* const po = pe->object();
	return (po&&po->owner() == owner_);
}
