#include "stdafx.h"
#include "resource.h"
#include ".\mouseobjectclickeventcond.h"

MouseObjectClickEventCond::MouseObjectClickEventCond(DWORD id):
OneObjectCond(id)
{
}

MouseObjectClickEventCond::~MouseObjectClickEventCond(void)
{
}

BOOL MouseObjectClickEventCond::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();
	
	OnSelchangeComboObject();
	return res;
}

bool MouseObjectClickEventCond::IsCondDataComplete()
{
	return (m_qdCondition.get_object(0) != NULL);
}

void MouseObjectClickEventCond::OnSelchangeComboObject()
{
	int curSel = m_wndObj.GetCurSel();
	if (curSel == CB_ERR)
		return;
	qdNamedObject *obj = 
		reinterpret_cast<qdNamedObject*>(m_wndObj.GetItemDataPtr(curSel));
	m_qdCondition.put_object(0, obj);
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}
