#include "stdafx.h"
#include "resource.h"
#include "activatepersonageeventrepres.h"
#include <set>

#include "qd_game_object.h"

ActivatePersonageEventRepres::ActivatePersonageEventRepres(void)
:InterfaceEventWithDataRepres(qdInterfaceEvent::EVENT_ACTIVATE_PERSONAGE,
							  IDS_EVENT_ACTIVATE_PERSONAGE)
{
}

ActivatePersonageEventRepres::~ActivatePersonageEventRepres(void)
{
}

LPCTSTR ActivatePersonageEventRepres::getDefaultValue() const{
	qdGameDispatcher* const pgd = qdGameDispatcher::get_dispatcher();
	if (pgd->scene_list().empty())
		return NULL;
	const qdGameSceneList& lst = pgd->scene_list();
	qdGameSceneList::const_iterator sl_itr = lst.begin(),sl_end = lst.end();
	for(; sl_itr != sl_end; ++sl_itr)
	{
		const qdGameObjectList& go_lst = (*sl_itr)->object_list();
		qdGameObjectList::const_iterator ix = go_lst.begin(), ex = go_lst.end();
		for(; ix != ex; ++ix)
			if ((*ix)->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ)
				return (*ix)->name();
	}
	return NULL;
}

void ActivatePersonageEventRepres::loadData(CWnd*pwnd) const{
	qdGameDispatcher* const pgd = qdGameDispatcher::get_dispatcher();
	if (pgd->scene_list().empty())
		return;

	CCJFlatComboBox* pcombo = static_cast<CCJFlatComboBox*>(pwnd);

	const qdGameSceneList& lst = pgd->scene_list();
	qdGameSceneList::const_iterator sl_itr = lst.begin(),
		sl_end = lst.end();
	CString str;
	std::set<CString> idx;
	for(; sl_itr != sl_end; ++sl_itr)
	{
		const qdGameObjectList& go_lst = (*sl_itr)->object_list();
		qdGameObjectList::const_iterator ix = go_lst.begin(), ex = go_lst.end();
		for(; ix != ex; ++ix)
			if ((*ix)->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ)
				idx.insert(idx.end(), (*ix)->name());
	}
	std::for_each(idx.begin(), idx.end(), 
		std::bind1st(std::mem_fun(CCJFlatComboBox::AddString),pcombo));
}
