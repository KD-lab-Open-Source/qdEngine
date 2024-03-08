#include "stdafx.h"
#include "resource.h"
#include "StatesToCombo.h"
#include "qd_game_object_animated.h"
#include "obj_type_detectors.h"

StatesToCombo::DefFormat::DefFormat():m_uiId(IDS_FORMAT_STATE_PP_OBJ){
}
StatesToCombo::DefFormat::DefFormat(UINT_PTR uiId):m_uiId(uiId){
}
CString StatesToCombo::DefFormat::operator()(qdGameObjectState const* pstate) const
{
	CString res;
	res.Format(m_uiId, pstate->name(), pstate->owner()->name());
	return res;
}
StatesToCombo::FormatStateName::FormatStateName(){}
CString StatesToCombo::FormatStateName::operator()(qdGameObjectState const* pstate) const
{
	return CString(pstate->name());
}

StatesToCombo::ObjStates2Combo::ObjStates2Combo(CCJFlatComboBox& combo, 
												Formater const& formater)
:m_combo(combo)
,m_formater(formater)
{}

void StatesToCombo::ObjStates2Combo::operator()(qdGameObject const* pobj)
{
	if (ptree_helpers::is_obj_animated(pobj)
		||ptree_helpers::is_obj_moving(pobj))
	{
		qdGameObjectStateVector const& v = 
			static_cast<qdGameObjectAnimated const*>(pobj)->state_vector();
		std::for_each(v.begin(), v.end(), StateToCombo(m_combo, m_formater));
	}
}

StatesToCombo::StateToCombo::StateToCombo(CCJFlatComboBox& combo, 
												Formater const& formater)
:m_combo(combo)
,m_formater(formater)
{
}
void StatesToCombo::StateToCombo::operator()(qdGameObjectState const* pstate){
	int const iItem = m_combo.AddString(m_formater(pstate));
	if (iItem != CB_ERR)
		m_combo.SetItemDataPtr(iItem, const_cast<qdGameObjectState*>(pstate));
}

StatesToCombo::StatesToCombo(void)
{
}

StatesToCombo::~StatesToCombo(void)
{
}

VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);

bool StatesToCombo::loadFromScene(CCJFlatComboBox& combo, 
								 qdGameScene const* pscene, Formater formater)
{
	try
	{
		qdGameObjectList const& lst = pscene->object_list();
		std::for_each(lst.begin(), lst.end(), ObjStates2Combo(combo, formater));
		AdjustListWidth(&combo);
	}
	catch (...) {
		return false;
	}
	return true;
}
bool StatesToCombo::loadFromObject(CCJFlatComboBox& combo, qdGameObjectAnimated const* pobj,
						  Formater formater)
{
	try
	{
		ObjStates2Combo(combo, formater)(pobj);
		AdjustListWidth(&combo);
	}
	catch (...) {
		return false;
	}
	return true;
}
