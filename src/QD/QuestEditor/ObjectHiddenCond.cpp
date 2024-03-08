#include "stdafx.h"
#include "resource.h"
#include ".\objecthiddencond.h"

ObjectHiddenCond::ObjectHiddenCond(DWORD id, CWnd * parent /*= NULL*/)
: OneObjectCond(id, parent)
{
	objectComboStaticText_.LoadString(IDS_SET_OBJECT);
}

ObjectHiddenCond::~ObjectHiddenCond(void)
{
}

void ObjectHiddenCond::LoadObjects()
{
	__super::LoadObjects();
	m_wndObj.SetCurSel(0);
	OnSelchangeComboObject();
}

void ObjectHiddenCond::OnSelchangeComboObject() 
{
	int iSel = m_wndObj.GetCurSel();
	if(iSel == -1)
		return;
	DWORD dwData = m_wndObj.GetItemData(iSel);

	qdCondition::ConditionDataIndex eIdx = qdCondition::OBJECT_NAME;

	if(dwData)
	{
		qdNamedObject* const p = 
			reinterpret_cast<qdNamedObject*>(m_wndObj.GetItemDataPtr(iSel));
		m_qdCondition.put_object(eIdx, p);
	}

	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

bool ObjectHiddenCond::IsCondDataComplete()
{
	return (m_qdCondition.get_object(qdCondition::OBJECT_NAME) != NULL);
}
