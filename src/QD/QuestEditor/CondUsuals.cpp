#include "stdafx.h"
#include "CondUsuals.h"
#include "qd_game_object.h"
#include "qd_fwd.h"
#include "obj_type_detectors.h"

bool CondUsuals::LoadScenes(CCJFlatComboBox& box, CWnd& sceneStatic)
{
	qdGameDispatcher* ptrGD = qd_get_game_dispatcher();
	ASSERT(ptrGD);
	const qdGameSceneList& gsl = ptrGD->scene_list();
	if(gsl.empty())
	{
		box.SetCurSel(-1);
		box.EnableWindow(FALSE);
		return false;
	}

	qdGameSceneList::const_iterator itr = gsl.begin(), end = gsl.end();
	for(;itr != end; ++itr){
		int ires = box.AddString((*itr)->name());
		if (ires != CB_ERR)
			box.SetItemDataPtr(ires, *itr);
	}
	return true;
}

bool CondUsuals::LoadAnimObjects(CCJFlatComboBox& wndCombo, const qdGameScene* ptrScene)
{
	wndCombo.ResetContent();
	const qdGameObjectList& gol = ptrScene->object_list();
	qdGameObjectList::const_iterator _itr = gol.begin(), _end = gol.end();
	while((_itr = std::find_if(_itr, _end, ptree_helpers::is_animated)) != _end)
	{
		int iPos = wndCombo.AddString((*_itr)->name());
		if(iPos != -1)
			wndCombo.SetItemDataPtr(iPos, *_itr);
		++_itr;
	}
	return true;
}

const void* CondUsuals::GetSelPtr(const CCJFlatComboBox& wndCombo)
{
	if (CB_ERR == wndCombo.GetCurSel())
		return NULL;
	return wndCombo.GetItemDataPtr(wndCombo.GetCurSel());
}
