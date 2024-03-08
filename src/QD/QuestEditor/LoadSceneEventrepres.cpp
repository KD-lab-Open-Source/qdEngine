#include "stdafx.h"
#include "resource.h"
#include ".\LoadSceneEventRepres.h"
#include <boost/rtl.hpp>

class LoadSceneEventRepres::AddToCombo
{
public:
	AddToCombo(CCJFlatComboBox& box):box_(box){
	}
	void operator()(qdGameScene const* scene) const{
		int iItem = box_.AddString(scene->name());
		if (iItem != CB_ERR)
			box_.SetItemDataPtr(iItem, const_cast<qdGameScene*>(scene));
	}
private:
	CCJFlatComboBox &box_;
};

LoadSceneEventRepres::LoadSceneEventRepres(UINT_PTR descr_id)
:  InterfaceEventWithDataRepres(qdInterfaceEvent::EVENT_LOAD_SCENE, 
								IDS_INTERF_EVENT_DESCR_LOAD_SCENE)
{
}

LoadSceneEventRepres::~LoadSceneEventRepres(void)
{
}

LPCTSTR LoadSceneEventRepres::getDefaultValue() const
{
	qdGameDispatcher* const pgd = qdGameDispatcher::get_dispatcher();
	if (!pgd)
		return NULL;
	if (pgd->scene_list().empty())
		return NULL;
	return pgd->scene_list().front()->name();
}

void LoadSceneEventRepres::loadData(CWnd* pwnd) const
{
	CCJFlatComboBox* const pbox = static_cast<CCJFlatComboBox*>(pwnd);
	qdGameDispatcher* const pgd = qdGameDispatcher::get_dispatcher();
	if (!pgd)
		return;
	boost::rtl::rng::for_each(pgd->scene_list(), AddToCombo(*pbox));
	if (pbox->GetCount())
		pbox->SetCurSel(0);
}
