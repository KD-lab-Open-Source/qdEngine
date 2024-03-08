#include "stdafx.h"
#include "resource.h"
#include ".\mouseobjectzoneclickcond.h"
#include "qd_grid_zone.h"

MouseObjectHoverZoneCond::MouseObjectHoverZoneCond(DWORD id, CWnd* parent /*= NULL*/):
MouseObjectZoneClickCond(id, parent)
{
	CString str;
	str.LoadString(IDS_COND_DLG_OBJ_ZONE_HOVER_STATIC_CAP);
	setSecondObjectStaticText(str);
}
MouseObjectHoverZoneCond::~MouseObjectHoverZoneCond(void)
{
}

MouseObjectZoneClickCond::MouseObjectZoneClickCond(DWORD id, CWnd* parent /*= NULL*/)
: TwoObjectCond(id, parent)
{
	CString str;
	str.LoadString(IDS_COND_DLG_OBJ_ZONE_CLICK_STATIC_CAP);
	setSecondObjectStaticText(str);
}

MouseObjectZoneClickCond::~MouseObjectZoneClickCond(void)
{
}

bool MouseObjectZoneClickCond::LoadObjects()
{
	if (qdGameScene* pscene = get_scene2()) 
	{
		m_wndObj2.ResetContent();
		const qdGridZoneList& lst = pscene->grid_zone_list();
		qdGridZoneList::const_iterator itr = lst.begin(), end = lst.end();
		for(; itr != end; ++itr)
		{
			qdGridZone* const pGZ = *itr;
			int iIndex = m_wndObj2.AddString(pGZ->name());
			if (iIndex != CB_ERR)
				m_wndObj2.SetItemDataPtr(iIndex, pGZ);
		}
		m_wndObj2.SetCurSel(0);
		OnSelchangeComboObject2();
		return true;
	}
	return false;
}

void MouseObjectZoneClickCond::OnSelchangeComboObject2() 
{
	int iSel = m_wndObj2.GetCurSel();
	if(iSel == -1)
		return;
	m_qdCondition.put_object(qdCondition::CLICK_ZONE_NAME, 
		reinterpret_cast<qdNamedObject*>(m_wndObj2.GetItemDataPtr(iSel)));
	
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

bool MouseObjectZoneClickCond::IsCondDataComplete()
{
	if(!m_qdCondition.get_object(qdCondition::CLICK_ZONE_NAME))
		return false;

	if(!m_qdCondition.get_object(qdCondition::MOUSE_OBJECT_NAME))
		return false;

	return true;
}

