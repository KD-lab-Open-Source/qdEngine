#include "stdafx.h"
#include "resource.h"
#include "settingDefs.h"
#include "TEWnd.h"
#include "teconditioncoloredeledrawer.h"
#include <Additional/qls.hpp>

#include "qd_trigger_element.h"
#include "qd_condition.h"
#include "qd_game_object_state.h"

int const LINK_PEN_WIDTH = 2;
TEConditionColoredEleDrawer::BrushVector_t 
	TEConditionColoredEleDrawer::m_vecLinkBrushs = {
		CreateSolidBrush(TE_TYPE0_COLOR),
		CreateSolidBrush(TE_TYPE1_COLOR),
		CreateSolidBrush(TE_TYPE2_COLOR),
		CreateSolidBrush(TE_TYPE3_COLOR),
		CreateSolidBrush(TE_TYPE4_COLOR),
		CreateSolidBrush(TE_TYPE5_COLOR),
		CreateSolidBrush(TE_TYPE6_COLOR),
		CreateSolidBrush(TE_TYPE7_COLOR),
		CreateSolidBrush(TE_TYPE8_COLOR),
		CreateSolidBrush(TE_TYPE9_COLOR),
		CreateSolidBrush(TE_TYPE10_COLOR)
};

TEConditionColoredEleDrawer::PenVector_t 
	TEConditionColoredEleDrawer::m_vecLinkPens = {
	CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE0_COLOR),
	CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE1_COLOR),
	CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE2_COLOR),
	CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE3_COLOR),
	CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE4_COLOR),
	CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE5_COLOR),
	CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE6_COLOR),
	CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE7_COLOR),
	CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE8_COLOR),
	CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE9_COLOR),
	CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE10_COLOR)
};

CBrush TEConditionColoredEleDrawer::
	m_brHideObj(DEFAULT_VALUE(STATE_FLAG_HIDE_OBJECT_COLOR));

CBrush TEConditionColoredEleDrawer::
	m_brInversedCond(DEFAULT_VALUE(INVERSE_CONDITION_COLOR));

CBrush TEConditionColoredEleDrawer::
	m_brDonedLinkBrush(TE_DONED_LINK_COLOR);

CPen TEConditionColoredEleDrawer::m_penDonedLink(PS_SOLID, 2, TE_DONED_LINK_COLOR);

TEConditionColoredEleDrawer::BrushIdx_t 
	TEConditionColoredEleDrawer::m_BrushIdx(TEConditionColoredEleDrawer::initBrushIdx());

TEConditionColoredEleDrawer::LinkPen::~LinkPen()
{
	DeleteObject(pen_);
}
TEConditionColoredEleDrawer::LinkPen::operator HPEN(){
	return pen_;
}
TEConditionColoredEleDrawer::Brush::operator HBRUSH(){
	return brush_;
}
TEConditionColoredEleDrawer::Brush::~Brush(){
	DeleteObject(brush_);
}
TEConditionColoredEleDrawer::TEConditionColoredEleDrawer(void)
{
}

TEConditionColoredEleDrawer::~TEConditionColoredEleDrawer(void)
{
}

HBRUSH	TEConditionColoredEleDrawer::SelectEleBackBrush(
	qdTriggerElementConstPtr const& pele) const
{
	if (!pele->object())
		return reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));

	qdConditionalObject* pco = static_cast<qdConditionalObject*>(pele->object());
	if (pco->named_object_type() == QD_NAMED_OBJECT_OBJ_STATE)
	{/// Если это не состояние, то проверяем не скрыт ли объект
		const qdGameObjectState* pState = static_cast<const qdGameObjectState*>(pco);
		if (pState->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HIDE_OBJECT))
			return m_brHideObj;
	}
	if (pco->has_conditions())
	{
		const qdCondition * const pc = pco->get_condition();
		if (pc->is_inversed())
			return m_brInversedCond;

		/// ищем по id-условия запись в индексе
		int type = static_cast<int>(pc->type());
		BrushIdx_t::const_iterator res = 
			qls::dough_find(m_BrushIdx.begin(),m_BrushIdx.end(), type);
		if (res != m_BrushIdx.end()) 
			return (*res).second;
		TRACE0(_T("No color entry for condition\n"));
	}
	return reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
}

HBRUSH	TEConditionColoredEleDrawer::SelectLinkBrColor(qdTriggerLink const* pl) const
{
	if (pl->status() == qdTriggerLink::LINK_DONE) 
		return m_brDonedLinkBrush;
	return getLinkTypeBrush(pl->type());
}

HPEN	TEConditionColoredEleDrawer::SelectLinkPenColor(qdTriggerLink const* pl) const
{
	if (pl->status() == qdTriggerLink::LINK_DONE) 
		return m_penDonedLink;
	return getLinkTypePen(pl->type());
}

TEConditionColoredEleDrawer::BrushIdx_t const TEConditionColoredEleDrawer::initBrushIdx()
{
	BrushIdx_t BrushIdx;
	BrushIdx.reserve(20);
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_MOUSE_CLICK,
		DEFAULT_VALUE(COND_MOUSE_CLICK_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_MOUSE_OBJECT_CLICK,
		DEFAULT_VALUE(COND_MOUSE_OBJECT_CLICK_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_IN_ZONE,
		DEFAULT_VALUE(COND_OBJECT_IN_ZONE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_PERSONAGE_WALK_DIRECTION,
		DEFAULT_VALUE(COND_PERSONAGE_WALK_DIRECTION_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_PERSONAGE_STATIC_DIRECTION,
		DEFAULT_VALUE(COND_PERSONAGE_STATIC_DIRECTION_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_TIMER,
		DEFAULT_VALUE(COND_TIMER_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_MOUSE_DIALOG_CLICK,
		DEFAULT_VALUE(COND_MOUSE_DIALOG_CLICK_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_MINIGAME_STATE,
		DEFAULT_VALUE(COND_MINIGAME_IN_STATE_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_STATE,
		DEFAULT_VALUE(COND_OBJECT_STATE_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_MOUSE_ZONE_CLICK,
		DEFAULT_VALUE(COND_MOUSE_ZONE_CLICK_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_MOUSE_OBJECT_ZONE_CLICK,
		DEFAULT_VALUE(COND_MOUSE_OBJECT_ZONE_CLICK_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_STATE_WAS_ACTIVATED,
		DEFAULT_VALUE(COND_OBJECT_STATE_WAS_ACTIVATED_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_STATE_WAS_NOT_ACTIVATED,
		DEFAULT_VALUE(COND_OBJECT_STATE_WAS_NOT_ACTIVATED_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECTS_DISTANCE,
		DEFAULT_VALUE(COND_OBJECTS_DISTANCE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_PERSONAGE_ACTIVE,
		DEFAULT_VALUE(COND_PERSONAGE_ACTIVE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_STATE_WAITING,
		DEFAULT_VALUE(COND_OBJECT_STATE_WAITING_COLOR)));

	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_STATE_ANIMATION_PHASE,
		DEFAULT_VALUE(COND_OBJECT_STATE_ANIMATION_PHASE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_PREV_STATE,
		DEFAULT_VALUE(COND_OBJECT_PREV_STATE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_VALUE,
		DEFAULT_VALUE(COND_STATE_TIME_GREATER_THAN_VALUE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_STATE_TIME,
		DEFAULT_VALUE(COND_STATE_TIME_GREATER_THAN_STATE_TIME_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_STATE_TIME_IN_INTERVAL,
		DEFAULT_VALUE(COND_STATE_TIME_IN_INTERVAL_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_COUNTER_GREATER_THAN_VALUE,
		DEFAULT_VALUE(COND_COUNTER_GREATER_THAN_VALUE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_COUNTER_LESS_THAN_VALUE,
		DEFAULT_VALUE(COND_COUNTER_LESS_THAN_VALUE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_COUNTER_GREATER_THAN_COUNTER,
		DEFAULT_VALUE(COND_COUNTER_GREATER_THAN_COUNTER_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_COUNTER_IN_INTERVAL,
		DEFAULT_VALUE(COND_COUNTER_IN_INTERVAL_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_ON_PERSONAGE_WAY,
		DEFAULT_VALUE(COND_OBJECT_ON_PERSONAGE_WAY_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_KEYPRESS,
		DEFAULT_VALUE(COND_KEYPRESS_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_ANY_PERSONAGE_IN_ZONE,
		DEFAULT_VALUE(COND_ANY_PERSONAGE_IN_ZONE_COLOR)));

	return BrushIdx;
}
