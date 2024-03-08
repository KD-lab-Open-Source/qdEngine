#include "stdafx.h"
#include "QuestEditor.h"
#include "CondUsuals.h"
#include "TwoObjIntersCondDlg.h"

TwoObjIntersCondDlg::TwoObjIntersCondDlg(DWORD id, CWnd* pParent /*= NULL*/):
	PureTwoObjCondBase(id, pParent)
{
}

TwoObjIntersCondDlg::~TwoObjIntersCondDlg()
{
}

bool TwoObjIntersCondDlg::LoadToObjCombo1(const qdGameScene* scene)
{
	return CondUsuals::LoadAnimObjects(m_cbObjects1, scene);
}

bool TwoObjIntersCondDlg::LoadToObjCombo2(const qdGameScene* scene)
{
	return CondUsuals::LoadAnimObjects(m_cbObjects2, scene);
}

bool TwoObjIntersCondDlg::ValidObjects(const qdGameObject* obj1, const qdGameObject* obj2) const
{
	if ((NULL == obj1) || (NULL == obj2) || (obj1 == obj2))
		return false;
	else 
		return true;
}