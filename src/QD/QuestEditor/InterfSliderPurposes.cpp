#include "stdafx.h"
#include "resource.h"
#include "interfsliderpurposes.h"
#include <algorithm>

InterfSliderPurposes::interfsldr_purpose::interfsldr_purpose(UINT descr_id, 
												qdInterfaceElement::option_ID_t oid):
  descr_id_(descr_id)
, option_id_(oid)
{}

const CString& InterfSliderPurposes::interfsldr_purpose::get_descr() const{
	if (descr_.IsEmpty())
		descr_.LoadString(descr_id_);
	return descr_;
}

const qdInterfaceElement::option_ID_t 
InterfSliderPurposes::interfsldr_purpose::opt_id() const{
	return option_id_;
}

InterfSliderPurposes::InterfSliderPurposes()
{
	purposes_.reserve(2);
	purposes_.push_back(interfsldr_purpose(IDS_INTERF_SLDR_PURPOSE_SOUND_VOLUME,
		qdInterfaceElement::OPTION_SOUND_VOLUME));
	purposes_.push_back(interfsldr_purpose(IDS_INTERF_SLDR_PURPOSE_MUSIC_VOLUME, 
		qdInterfaceElement::OPTION_MUSIC_VOLUME));
}

InterfSliderPurposes::~InterfSliderPurposes(void)
{
}

size_t InterfSliderPurposes::count() const{
	return purposes_.size();
}

const qdInterfaceElement::option_ID_t InterfSliderPurposes::get_opt_id(int i) const{
	return purposes_[i].opt_id();
}
CString InterfSliderPurposes::get_descr(int i) const{
	return purposes_[i].get_descr();
}

CString InterfSliderPurposes::get_descr(const qdInterfaceElement::option_ID_t mode)const{
	interfsldr_purpose const* p = find(mode);
	if (p)
		return p->get_descr();
	return CString();
}

InterfSliderPurposes::interfsldr_purpose const* InterfSliderPurposes::find(
							const qdInterfaceElement::option_ID_t id) const
{
	std::vector<interfsldr_purpose>::const_iterator res = 
		std::find(purposes_.begin(), purposes_.end(), id);
	if (res == purposes_.end())
		return NULL;
	return &*res;
}
