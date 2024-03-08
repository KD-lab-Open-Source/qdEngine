#include "stdafx.h"
#include "questeditor.h"

#include "CondDlgMgr.h"
#include "condition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CondDlgMgr::CondDlgMgr(void):
m_dlgPersWalkDir		(MAKE_NORMAL(qdCondition::CONDITION_PERSONAGE_WALK_DIRECTION)),
m_dlgPersStopDir		(MAKE_NORMAL(qdCondition::CONDITION_PERSONAGE_STATIC_DIRECTION)),

m_dlgMouseClickObjCond		(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_CLICK)),
m_dlgMouseClickZoneCond		(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_ZONE_CLICK)),
m_dlgMouseObjClickCond		(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_OBJECT_CLICK)),
m_dlgMouseObjClickZoneCond	(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_OBJECT_ZONE_CLICK)),

m_dlgMouseHoverCond			(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_HOVER)),
m_dlgMouseHoverZoneCond		(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_HOVER_ZONE)),
m_dlgMouseObjHoverCond		(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_OBJECT_HOVER)),
m_dlgMouseObjHoverZoneCond	(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_OBJECT_HOVER_ZONE)),

m_dlgObjInState			(MAKE_NORMAL(qdCondition::CONDITION_OBJECT_STATE)),
m_dlgObjWasInState		(MAKE_NORMAL(qdCondition::CONDITION_OBJECT_STATE_WAS_ACTIVATED)),
m_dlgObjOnNetCond(MAKE_NORMAL(qdCondition::CONDITION_OBJECT_IN_ZONE)),
m_dlgMouseDlgClickCond(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_DIALOG_CLICK)),
//m_dlgMGInStateCond(MAKE_NORMAL(qdCondition::CONDITION_MINIGAME_STATE)),
m_dlgObjectDistance(MAKE_NORMAL(qdCondition::CONDITION_OBJECTS_DISTANCE)),
m_dlgMovingActive(MAKE_NORMAL(qdCondition::CONDITION_PERSONAGE_ACTIVE)),
m_dlgObjStateWaiting(MAKE_NORMAL(qdCondition::CONDITION_OBJECT_STATE_WAITING)),
m_dlgStateAnimInPhaseDlg(
					MAKE_NORMAL(qdCondition::CONDITION_OBJECT_STATE_ANIMATION_PHASE)),
m_dlgObjPrevState(MAKE_NORMAL(qdCondition::CONDITION_OBJECT_PREV_STATE)),
m_dlgWTGreateWTCond(MAKE_NORMAL(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_STATE_TIME)),
m_dlgWTGreaterValue(MAKE_NORMAL(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_VALUE)),
m_dlgWTInIntervalCond(MAKE_NORMAL(qdCondition::CONDITION_STATE_TIME_IN_INTERVAL)),
m_dlgCounterGrCounter(MAKE_NORMAL(qdCondition::CONDITION_COUNTER_GREATER_THAN_COUNTER)),
m_dlgCounterGrValue(MAKE_NORMAL(qdCondition::CONDITION_COUNTER_GREATER_THAN_VALUE)),
m_dlgCounterLessValue(MAKE_NORMAL(qdCondition::CONDITION_COUNTER_LESS_THAN_VALUE)),
m_dlgCounterInInterval(MAKE_NORMAL(qdCondition::CONDITION_COUNTER_IN_INTERVAL)),
m_dlgObjOnPersonageWay(MAKE_NORMAL(qdCondition::CONDITION_OBJECT_ON_PERSONAGE_WAY)), 
m_dlgKeyPressedCondDlg(MAKE_NORMAL(qdCondition::CONDITION_KEYPRESS)),
m_dlgAnyPersonageOnGridZone(MAKE_NORMAL(qdCondition::CONDITION_ANY_PERSONAGE_IN_ZONE)), 

m_dlgMouseRightClickObjCond		(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_RIGHT_CLICK)),
m_dlgMouseRightClickZoneCond	(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_RIGHT_ZONE_CLICK)),
m_dlgMouseRightObjClickCond		(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_CLICK)),
m_dlgMouseRightObjClickZoneCond	(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_ZONE_CLICK)),

m_dlgObjectHiddenCond			(MAKE_NORMAL(qdCondition::CONDITION_OBJECT_HIDDEN)),

m_dlgTimerCond(MAKE_NORMAL(qdCondition::CONDITION_TIMER)),

m_dlgPersWalkDirNOT(MAKE_INVERS(qdCondition::CONDITION_PERSONAGE_WALK_DIRECTION)),
m_dlgPersStopDirNOT(MAKE_INVERS(qdCondition::CONDITION_PERSONAGE_STATIC_DIRECTION)),
m_dlgMouseClickObjCondNOT(MAKE_INVERS(qdCondition::CONDITION_MOUSE_CLICK)),
m_dlgMouseClickZoneCondNOT(MAKE_INVERS(qdCondition::CONDITION_MOUSE_ZONE_CLICK)),
m_dlgObjInStateNOT(MAKE_INVERS(qdCondition::CONDITION_OBJECT_STATE)),
m_dlgObjWasInStateNOT(MAKE_INVERS(qdCondition::CONDITION_OBJECT_STATE_WAS_ACTIVATED)),
m_dlgMouseObjClickCondNOT(MAKE_INVERS(qdCondition::CONDITION_MOUSE_OBJECT_CLICK)),
m_dlgMouseObjClickZoneCondNOT(MAKE_INVERS(qdCondition::CONDITION_MOUSE_OBJECT_ZONE_CLICK)),
m_dlgObjOnNetCondNOT(MAKE_INVERS(qdCondition::CONDITION_OBJECT_IN_ZONE)),
m_dlgMouseDlgClickCondNOT(MAKE_INVERS(qdCondition::CONDITION_MOUSE_DIALOG_CLICK)),
//m_dlgMGInStateCondNOT(MAKE_INVERS(qdCondition::CONDITION_MINIGAME_STATE)),
m_dlgObjectDistanceNOT(MAKE_INVERS(qdCondition::CONDITION_OBJECTS_DISTANCE)),
m_dlgMovingActiveNOT(MAKE_INVERS(qdCondition::CONDITION_PERSONAGE_ACTIVE)),
m_dlgObjStateWaitingNOT(MAKE_INVERS(qdCondition::CONDITION_OBJECT_STATE_WAITING)),
m_dlgStateAnimInPhaseDlgNOT(MAKE_INVERS(qdCondition::CONDITION_OBJECT_STATE_ANIMATION_PHASE)),
m_dlgObjPrevStateNOT(MAKE_INVERS(qdCondition::CONDITION_OBJECT_PREV_STATE)),
m_dlgWTGreateWTCondNOT(MAKE_INVERS(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_STATE_TIME)),
m_dlgWTGreaterValueNOT(MAKE_INVERS(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_VALUE)),
m_dlgWTInIntervalCondNOT(MAKE_INVERS(qdCondition::CONDITION_STATE_TIME_IN_INTERVAL)),
m_dlgCounterGrCounterNOT(MAKE_INVERS(qdCondition::CONDITION_COUNTER_GREATER_THAN_COUNTER)),
m_dlgCounterGrValueNOT(MAKE_INVERS(qdCondition::CONDITION_COUNTER_GREATER_THAN_VALUE)),
m_dlgCounterLessValueNOT(MAKE_INVERS(qdCondition::CONDITION_COUNTER_LESS_THAN_VALUE)),
m_dlgCounterInIntervalNOT(MAKE_INVERS(qdCondition::CONDITION_COUNTER_IN_INTERVAL)),
m_dlgObjOnPersonageWayNOT(MAKE_INVERS(qdCondition::CONDITION_OBJECT_ON_PERSONAGE_WAY)), 
m_dlgKeyPressedCondDlgNOT(MAKE_INVERS(qdCondition::CONDITION_KEYPRESS)),
m_dlgAnyPersonageOnGridZoneNOT(MAKE_INVERS(qdCondition::CONDITION_ANY_PERSONAGE_IN_ZONE)), 

m_dlgMouseRightClickObjCondNOT		 (
				MAKE_INVERS(qdCondition::CONDITION_MOUSE_RIGHT_CLICK)),
m_dlgMouseRightClickZoneCondNOT		 (
				MAKE_INVERS(qdCondition::CONDITION_MOUSE_RIGHT_ZONE_CLICK)),
m_dlgMouseRightObjClickCondNOT		 (
				MAKE_INVERS(qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_CLICK)),
m_dlgMouseRightObjClickZoneCondNOT	 (
				MAKE_INVERS(qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_ZONE_CLICK)),

m_dlgMouseHoverCondNOT			(MAKE_INVERS(qdCondition::CONDITION_MOUSE_HOVER)),
m_dlgMouseHoverZoneCondNOT		(MAKE_INVERS(qdCondition::CONDITION_MOUSE_HOVER_ZONE)),
m_dlgMouseObjHoverCondNOT		(MAKE_INVERS(qdCondition::CONDITION_MOUSE_OBJECT_HOVER)),
m_dlgMouseObjHoverZoneCondNOT	(MAKE_INVERS(qdCondition::CONDITION_MOUSE_OBJECT_HOVER_ZONE)),

m_dlgObjectHiddenCondNOT		(MAKE_INVERS(qdCondition::CONDITION_OBJECT_HIDDEN)),


m_dlgMouseClickObjFailedCond(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_OBJECT_CLICK_FAILED)),
m_dlgMouseClickObjFailedCondNOT(
				MAKE_INVERS(qdCondition::CONDITION_MOUSE_OBJECT_CLICK_FAILED)),

m_dlgMouseClickFailedCond(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_CLICK_FAILED)),
m_dlgMouseClickFailedCondNOT(MAKE_INVERS(qdCondition::CONDITION_MOUSE_CLICK_FAILED)),

m_dlgMouseClickEventCond(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_CLICK_EVENT)),
m_dlgMouseClickObjEventCond(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_OBJECT_CLICK_EVENT)),
m_dlgMouseRightClickEventCond(MAKE_NORMAL(qdCondition::CONDITION_MOUSE_RIGHT_CLICK_EVENT)),
m_dlgMouseRightClickObjEventCond(
				MAKE_NORMAL(qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_CLICK_EVENT)),
m_dlgMouseStatePhraseClickCondDlg(
				MAKE_NORMAL(qdCondition::CONDITION_MOUSE_STATE_PHRASE_CLICK)),
m_dlgMouseStatePhraseClickCondDlgNOT(
				MAKE_INVERS(qdCondition::CONDITION_MOUSE_STATE_PHRASE_CLICK)),
m_dlgObjectIsCloseCondDlg(
				MAKE_NORMAL(qdCondition::CONDITION_OBJECT_IS_CLOSER)),
m_dlgObjectIsCloseCondDlgNOT(
				MAKE_INVERS(qdCondition::CONDITION_OBJECT_IS_CLOSER)),
m_dlgAnimObjIdleCond(
				MAKE_NORMAL(qdCondition::CONDITION_ANIMATED_OBJECT_IDLE_GREATER_THAN_VALUE)),
m_dlgTwoObjIntersCond(
				MAKE_NORMAL(qdCondition::CONDITION_ANIMATED_OBJECTS_INTERSECTIONAL_BOUNDS)),
m_dlgTwoObjIntersCondNOT(
				MAKE_INVERS(qdCondition::CONDITION_ANIMATED_OBJECTS_INTERSECTIONAL_BOUNDS)),
m_ptrCurrDlg(NULL)
{
}

CondDlgMgr::~CondDlgMgr(){
}

template<class _Dlg>
void CondDlgMgr::init_dlg(_Dlg& dlg, MakeCondDlgBase* pdlg, CWnd* parent, 
						  CRect const& rcDlgPlace)
{
	dlg.set_owner(pdlg);
	dlg.Create(_Dlg::IDD, parent);
	dlg.MoveWindow(rcDlgPlace);
}

VOID CondDlgMgr::init(MakeCondDlgBase* pdlg, CWnd* parent, CRect const& rcDlgPlace){
	DEBUG_ONLY(m_bInitCalled = true);
	// Инициализирует диалог условий и диалог отрицания
#define INIT_DLG(dlg, dlg2, parent, rect) \
	init_dlg(dlg, dlg2, parent, rect);		\
	init_dlg(dlg##NOT, dlg2, parent, rect)

#define SIMPLE_INIT_DLG(dlg, dlg2, parent, rect) \
	init_dlg(dlg, dlg2, parent, rect);		\

	INIT_DLG(m_dlgWTGreateWTCond, pdlg, parent, rcDlgPlace);

	INIT_DLG(m_dlgMouseClickObjCond, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgMouseClickZoneCond, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgMouseObjClickCond, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgMouseObjClickZoneCond, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgMouseDlgClickCond, pdlg, parent, rcDlgPlace);

	INIT_DLG(m_dlgMouseRightClickObjCond,		pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgMouseRightClickZoneCond,		pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgMouseRightObjClickCond,		pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgMouseRightObjClickZoneCond,	pdlg, parent, rcDlgPlace);

	INIT_DLG(m_dlgMouseHoverCond,			pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgMouseHoverZoneCond,		pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgMouseObjHoverCond,		pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgMouseObjHoverZoneCond,	pdlg, parent, rcDlgPlace);

	INIT_DLG(m_dlgObjOnNetCond, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgPersStopDir, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgPersWalkDir, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgObjInState, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgObjWasInState, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgObjectDistance, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgMovingActive, pdlg, parent, rcDlgPlace);

	INIT_DLG(m_dlgObjStateWaiting, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgStateAnimInPhaseDlg, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgObjPrevState, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgWTGreaterValue, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgWTInIntervalCond, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgCounterGrCounter, pdlg, parent, rcDlgPlace);

	init_dlg(m_dlgTimerCond, pdlg, parent, rcDlgPlace);

	INIT_DLG(m_dlgCounterGrValue, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgCounterLessValue, pdlg, parent, rcDlgPlace);

	INIT_DLG(m_dlgCounterInInterval, pdlg, parent, rcDlgPlace);

	INIT_DLG(m_dlgObjOnPersonageWay, pdlg, parent, rcDlgPlace);

	INIT_DLG(m_dlgKeyPressedCondDlg, pdlg, parent, rcDlgPlace);

	INIT_DLG(m_dlgAnyPersonageOnGridZone, pdlg, parent, rcDlgPlace);

	INIT_DLG(m_dlgObjectHiddenCond, pdlg, parent, rcDlgPlace);

	INIT_DLG(m_dlgMouseClickFailedCond, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgMouseClickObjFailedCond, pdlg, parent, rcDlgPlace);

	INIT_DLG(m_dlgMouseStatePhraseClickCondDlg, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgObjectIsCloseCondDlg, pdlg, parent, rcDlgPlace);
	INIT_DLG(m_dlgTwoObjIntersCond, pdlg, parent, rcDlgPlace);

	SIMPLE_INIT_DLG(m_dlgMouseClickEventCond, pdlg, parent, rcDlgPlace);
	SIMPLE_INIT_DLG(m_dlgMouseClickObjEventCond, pdlg, parent, rcDlgPlace);
	SIMPLE_INIT_DLG(m_dlgMouseRightClickEventCond, pdlg, parent, rcDlgPlace);
	SIMPLE_INIT_DLG(m_dlgMouseRightClickObjEventCond, pdlg, parent, rcDlgPlace);
	SIMPLE_INIT_DLG(m_dlgAnimObjIdleCond, pdlg, parent, rcDlgPlace);
}

void CondDlgMgr::set_current(DWORD id){
	ASSERT(m_bInitCalled);

#define CONDITION_CASE(condition, var)  \
	case MAKE_NORMAL(condition):m_ptrCurrDlg = &var;break;	\
	case MAKE_INVERS(condition):m_ptrCurrDlg = &var##NOT;break

#define SIMPLE_CASE(condition, var)  \
	case MAKE_NORMAL(condition):m_ptrCurrDlg = &var;break;	\

	switch(id) {
		case MAKE_NORMAL(qdCondition::CONDITION_TIMER):
			m_ptrCurrDlg = &m_dlgTimerCond;
			break;
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_CLICK, m_dlgMouseClickObjCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_OBJECT_CLICK, 
						m_dlgMouseObjClickCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_ZONE_CLICK, m_dlgMouseClickZoneCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_OBJECT_ZONE_CLICK, 
						m_dlgMouseObjClickZoneCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_RIGHT_CLICK, 
						m_dlgMouseRightClickObjCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_CLICK,	
						m_dlgMouseRightObjClickCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_RIGHT_ZONE_CLICK,  
						m_dlgMouseRightClickZoneCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_ZONE_CLICK, 
						m_dlgMouseRightObjClickZoneCond);
		CONDITION_CASE(qdCondition::CONDITION_OBJECT_IN_ZONE,m_dlgObjOnNetCond);
		CONDITION_CASE(qdCondition::CONDITION_PERSONAGE_STATIC_DIRECTION,
						m_dlgPersStopDir);
		CONDITION_CASE(qdCondition::CONDITION_PERSONAGE_WALK_DIRECTION,
						m_dlgPersWalkDir);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_DIALOG_CLICK,m_dlgMouseDlgClickCond);
		CONDITION_CASE(qdCondition::CONDITION_OBJECT_STATE_WAITING,m_dlgObjStateWaiting);
		CONDITION_CASE(qdCondition::CONDITION_OBJECT_STATE, m_dlgObjInState);
		CONDITION_CASE(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_STATE_TIME, 
						m_dlgWTGreateWTCond);
		CONDITION_CASE(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_VALUE,
						m_dlgWTGreaterValue);
		CONDITION_CASE(qdCondition::CONDITION_STATE_TIME_IN_INTERVAL, 
						m_dlgWTInIntervalCond);
		CONDITION_CASE(qdCondition::CONDITION_OBJECT_STATE_ANIMATION_PHASE, 
						m_dlgStateAnimInPhaseDlg);
		CONDITION_CASE(qdCondition::CONDITION_OBJECT_PREV_STATE, m_dlgObjPrevState);
		CONDITION_CASE(qdCondition::CONDITION_OBJECT_STATE_WAS_ACTIVATED, 
						m_dlgObjWasInState);
		CONDITION_CASE(qdCondition::CONDITION_OBJECTS_DISTANCE, m_dlgObjectDistance);
		CONDITION_CASE(qdCondition::CONDITION_PERSONAGE_ACTIVE, m_dlgMovingActive);
		CONDITION_CASE(qdCondition::CONDITION_COUNTER_GREATER_THAN_COUNTER, 
						m_dlgCounterGrCounter);
		CONDITION_CASE(qdCondition::CONDITION_COUNTER_GREATER_THAN_VALUE, 
						m_dlgCounterGrValue);
		CONDITION_CASE(qdCondition::CONDITION_COUNTER_LESS_THAN_VALUE, 
						m_dlgCounterLessValue);
		CONDITION_CASE(qdCondition::CONDITION_COUNTER_IN_INTERVAL, 
						m_dlgCounterInInterval);
		CONDITION_CASE(qdCondition::CONDITION_OBJECT_ON_PERSONAGE_WAY, 
						m_dlgObjOnPersonageWay);
		CONDITION_CASE(qdCondition::CONDITION_KEYPRESS, m_dlgKeyPressedCondDlg);
		CONDITION_CASE(qdCondition::CONDITION_ANY_PERSONAGE_IN_ZONE, 
						m_dlgAnyPersonageOnGridZone);
//			//! объект скрыт
		CONDITION_CASE(qdCondition::CONDITION_OBJECT_HIDDEN, m_dlgObjectHiddenCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_HOVER, m_dlgMouseHoverCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_OBJECT_HOVER, 
						m_dlgMouseObjHoverCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_HOVER_ZONE, m_dlgMouseHoverZoneCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_OBJECT_HOVER_ZONE, 
						m_dlgMouseObjHoverZoneCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_CLICK_FAILED,
			m_dlgMouseClickFailedCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_OBJECT_CLICK_FAILED,
			m_dlgMouseClickObjFailedCond);
		SIMPLE_CASE(qdCondition::CONDITION_MOUSE_CLICK_EVENT, m_dlgMouseClickEventCond);
		SIMPLE_CASE(qdCondition::CONDITION_MOUSE_OBJECT_CLICK_EVENT, 
			m_dlgMouseClickObjEventCond);
		SIMPLE_CASE(qdCondition::CONDITION_MOUSE_RIGHT_CLICK_EVENT, 
			m_dlgMouseRightClickEventCond);
		SIMPLE_CASE(qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_CLICK_EVENT, 
			m_dlgMouseRightClickObjEventCond);
		CONDITION_CASE(qdCondition::CONDITION_MOUSE_STATE_PHRASE_CLICK,
			m_dlgMouseStatePhraseClickCondDlg);
		CONDITION_CASE(qdCondition::CONDITION_OBJECT_IS_CLOSER,
			m_dlgObjectIsCloseCondDlg);
		SIMPLE_CASE(qdCondition::CONDITION_ANIMATED_OBJECT_IDLE_GREATER_THAN_VALUE, 
			m_dlgAnimObjIdleCond);
		CONDITION_CASE(qdCondition::CONDITION_ANIMATED_OBJECTS_INTERSECTIONAL_BOUNDS,
			m_dlgTwoObjIntersCond);
		default:
			m_ptrCurrDlg = NULL;
	}
}
