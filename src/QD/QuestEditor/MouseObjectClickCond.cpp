#include "stdafx.h"
#include "resource.h"
#include ".\mouseobjectclickcond.h"
#include "obj_type_detectors.h"
#include "qd_game_object_animated.h"

bool LoadObjects(CCJFlatComboBox& wndCombo, qdGameScene* ptrScene)
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


MouseObjectHoverObjectCond::MouseObjectHoverObjectCond(DWORD id, CWnd* parent /*= NULL*/)
:MouseObjectClickCond(id, parent)
{
	CString str;
	str.LoadString(	IDS_COND_DLG_MOUSE_OBJ_HOVER_OBJECT_STATIC_CAP);
	setSecondObjectStaticText(str);
}
MouseObjectHoverObjectCond::~MouseObjectHoverObjectCond(void)
{
}

MouseObjectClickCond::MouseObjectClickCond(DWORD id, CWnd* parent /*= NULL*/):
TwoObjectCond(id, parent)
{
	CString str;
	str.LoadString(IDS_OBJECT_ON);
	setSecondObjectStaticText(str);
}

MouseObjectClickCond::~MouseObjectClickCond(void)
{
}

bool MouseObjectClickCond::LoadObjects()
{
	qdGameScene* ptrScene = get_scene2();
	if(!ptrScene)
		return false;
	const bool res = ::LoadObjects(m_wndObj2, ptrScene);
	m_wndObj2.SetCurSel(0);
	OnSelchangeComboObject2();
	return res;
}

void MouseObjectClickCond::OnSelchangeComboObject2()
{
	int iSel = m_wndObj2.GetCurSel();
	if(iSel == -1)
		return;
	qdCondition::ConditionDataIndex eIdx = qdCondition::OBJECT_NAME;
	m_qdCondition.put_object(eIdx, 
		reinterpret_cast<qdNamedObject*>(m_wndObj2.GetItemDataPtr(iSel)));

	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

bool MouseObjectClickCond::IsCondDataComplete()
{
	qdCondition::ConditionDataIndex eIdx = qdCondition::OBJECT_NAME;

	if(!m_qdCondition.get_object(eIdx))
		return false;

	if(!m_qdCondition.get_object(qdCondition::MOUSE_OBJECT_NAME))
		return false;

	return true;
}

