#include "stdafx.h"
#include "resource.h"
#include ".\mousehovercond.h"


MouseHoverCond::MouseHoverCond(DWORD id, CWnd* pParent /*=NULL*/):
OneObjectCond(id, pParent )
{
	objectComboStaticText_.LoadString(IDS_MOUSE_HOVER);
}

MouseHoverCond::~MouseHoverCond(void)
{
}

void MouseHoverCond::OnSelchangeComboObject()
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
	else
		m_qdCondition.put_object(eIdx, NULL);

	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void MouseHoverCond::LoadObjects()
{
//	qdGameScene* ptrScene = get_scene();
//	if (ptrScene) 
//		::LoadObjects(m_wndObj, ptrScene);
	__super::LoadObjects();
	m_wndObj.InsertString(0, CString((LPTSTR)IDS_COND_MOUSE_HOVER_OBJ_SELF));
	m_wndObj.SetCurSel(0);
	OnSelchangeComboObject();
}

