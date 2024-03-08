#include "stdafx.h"
#include "QuestEditor.h"
#include "ObjPrevStateCond.h"
#include "MakeCondDlgBase.h"
#include "qd_game_object_animated.h"

ObjPrevStateCond::ObjPrevStateCond(DWORD id):ObjInStateCond(id)
{
}

bool ObjPrevStateCond::LoadStates(const qdGameObjectAnimated* pa)
{
	if (ObjInStateCond::LoadStates(pa))
	{
		if (!m_qdCondition.is_inversed())
		{
			CCJFlatComboBox& wnd = getStatesCombo();
			wnd.InsertString(0, CString((LPTSTR)IDS_NOTHING));
		}
		return true;
	}
	return false;
}

bool ObjPrevStateCond::IsCondDataComplete()
{
	if(!m_qdCondition.get_object(qdCondition::OBJECT_NAME))
		return false;

	return true;
}
