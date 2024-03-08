#include "stdafx.h"
#include "resource.h"
#include "qd_grid_zone.h"
#include ".\MouseZoneClickCond.h"

MouseZoneClickCond::MouseZoneClickCond(DWORD id, CWnd* pParent /*=NULL*/)
:OneObjectCond(id, pParent)
{
	objectComboStaticText_.LoadString(IDS_MOUSE_CLICK_ON);
}

MouseZoneClickCond::~MouseZoneClickCond(void)
{
}


bool MouseZoneClickCond::IsCondDataComplete()
{
	return m_qdCondition.get_object(qdCondition::CLICK_ZONE_NAME);
}

void MouseZoneClickCond::LoadObjects()
{
	qdGameScene* ptrScene = get_scene();
	if (ptrScene) 
	{
		m_wndObj.ResetContent();
		const qdGridZoneList& lst = ptrScene->grid_zone_list();
		qdGridZoneList::const_iterator i = lst.begin(), e = lst.end();
		for(; i != e; ++i)
		{
			qdGridZone* const pZone = *i;
			int i = m_wndObj.AddString(pZone->name());
			if (i != CB_ERR)
				m_wndObj.SetItemDataPtr(i, pZone);
		}
	}
	m_wndObj.SetCurSel(0);
	OnSelchangeComboObject();
}

void MouseZoneClickCond::OnSelchangeComboObject() 
{
	int iSel = m_wndObj.GetCurSel();
	if(iSel == -1)
		return;
	DWORD dwData = m_wndObj.GetItemData(iSel);

	qdCondition::ConditionDataIndex eIdx = qdCondition::CLICK_ZONE_NAME;

	if(dwData)
	{
		qdNamedObject* const p = 
			reinterpret_cast<qdNamedObject*>(m_wndObj.GetItemDataPtr(iSel));
		m_qdCondition.put_object(eIdx, p);
	}

	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

