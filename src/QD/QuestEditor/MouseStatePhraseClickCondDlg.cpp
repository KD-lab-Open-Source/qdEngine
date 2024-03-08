// MouseStatePhraseClickCondDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "MouseStatePhraseClickCondDlg.h"
#include "qd_game_object_animated.h"


// MouseStatePhraseClickCondDlg dialog

IMPLEMENT_DYNAMIC(MouseStatePhraseClickCondDlg, CDialog)
MouseStatePhraseClickCondDlg::MouseStatePhraseClickCondDlg(DWORD id)
	: ObjInStateCond(id)
{
}

MouseStatePhraseClickCondDlg::~MouseStatePhraseClickCondDlg()
{
}


bool MouseStatePhraseClickCondDlg::IsCondDataComplete()
{
	if(!m_qdCondition.get_object(qdCondition::OBJECT_NAME))
		return false;

	return true;
}

void MouseStatePhraseClickCondDlg::OnCbnSelchangeComboObject()
{
	int iSel = m_wndObjs.GetCurSel();
	if(iSel == -1)
		return;
	DWORD_PTR dwData = m_wndObjs.GetItemData(iSel);
	if(dwData)
	{
		qdNamedObject* const p = reinterpret_cast<qdNamedObject*>(dwData);
//		m_qdCondition.put_object(qdCondition::OBJECT_NAME, p);

		LoadStates(static_cast<qdGameObjectAnimated*>(p));
	}
//	else
//		m_qdCondition.put_object(qdCondition::OBJECT_NAME, NULL);

//	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void MouseStatePhraseClickCondDlg::OnCbnSelchangeComboStates()
{
	int iSel = m_wndStates.GetCurSel();
	if(iSel == -1)
		return;
	m_qdCondition.put_object(qdCondition::OBJECT_NAME, 
		reinterpret_cast<qdNamedObject*>(m_wndStates.GetItemDataPtr(iSel)));
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void MouseStatePhraseClickCondDlg::OnCbnSelchangeComboScenes()
{
	__super::OnCbnSelchangeComboScenes();
}

