#pragma once

#include "conddlgbase.h"
#include "mouseclickcond.h"
#include "mouseobjectclickcond.h"
#include "objonnetcond.h"
#include "timercond.h"
#include "WalkStopDirCond.h"
#include "NoDataCondDlg.h"
#include "ObjInStateCond.h"
#include "objectdistancecond.h"
#include "MovingActiveCond.h"
#include "StateAnimInPhaseDlg.h"
#include "ObjPrevStateCond.h"
#include "WTGreateWTCond.h"
#include "WTGreaterValue.h"
#include "WTInIntervalCond.h"
#include "CounterGreaterCounterCondDlg.h"
#include "CounterLGValue.h"
#include "CounterInIntervalCondDlg.h"
#include "ObjOnPersonageWayCond.h"
#include "KeyPressedCondDlg.h"
#include "AnyPersonageOnNetDlgCond.h"
#include ".\mousehoverzonecond.h"
#include ".\objecthiddencond.h"
#include ".\mouseobjectzoneclickcond.h"
#include "MouseStatePhraseClickCondDlg.h"
#include ".\objectiscloserconddlg.h"
#include "MouseObjectClickEventCond.h"
#include "AnimObjIdleCondDlg.h"
#include "TwoObjIntersCondDlg.h"

#include "mousehovercond.h"
#include "MouseZoneClickCond.h"

#include "MakeCondDlgBase.h"

//! Объявляет переменную диалога для условия и для его отрицания
#define DECLARE_COND_DLG(type, var)\
	type var;						\
	type var##NOT

//! Объявляет переменную диалога только для условия
#define SIMPLE_DECLARE_COND_DLG(type, var)\
	type var;						\


/*!
 Управляет созданием диалогов конкретных условий. Возвращает по идентификатору условия
 соответствующий класс диалога
*/
class CondDlgMgr
{
	//диалоги данных для условий
	TimerCond						m_dlgTimerCond;
	DECLARE_COND_DLG(MouseZoneClickCond, m_dlgMouseClickZoneCond);
	DECLARE_COND_DLG(MouseZoneClickCond, m_dlgMouseRightClickZoneCond);
	DECLARE_COND_DLG(MouseHoverCond,	m_dlgMouseHoverCond);
	DECLARE_COND_DLG(MouseClickCond,	m_dlgMouseRightClickObjCond);
	DECLARE_COND_DLG(MouseClickCond,	m_dlgMouseClickObjCond);

	DECLARE_COND_DLG(MouseHoverZoneCond,	m_dlgMouseHoverZoneCond);
	DECLARE_COND_DLG(ObjectHiddenCond,		m_dlgObjectHiddenCond);

	DECLARE_COND_DLG(MouseObjectClickCond, m_dlgMouseObjClickCond);
	DECLARE_COND_DLG(MouseObjectZoneClickCond, m_dlgMouseObjClickZoneCond);
	DECLARE_COND_DLG(MouseObjectClickCond, m_dlgMouseRightObjClickCond);
	DECLARE_COND_DLG(MouseObjectZoneClickCond, m_dlgMouseRightObjClickZoneCond);
	DECLARE_COND_DLG(MouseObjectHoverObjectCond, m_dlgMouseObjHoverCond);
	DECLARE_COND_DLG(MouseObjectHoverZoneCond, m_dlgMouseObjHoverZoneCond);

	DECLARE_COND_DLG(NoDataCondDlg, m_dlgMouseClickFailedCond);
	DECLARE_COND_DLG(NoDataCondDlg, m_dlgMouseClickObjFailedCond);

	SIMPLE_DECLARE_COND_DLG(NoDataCondDlg, m_dlgMouseClickEventCond);
	SIMPLE_DECLARE_COND_DLG(NoDataCondDlg, m_dlgMouseRightClickEventCond);
	SIMPLE_DECLARE_COND_DLG(MouseObjectClickEventCond, m_dlgMouseClickObjEventCond);
	SIMPLE_DECLARE_COND_DLG(MouseObjectClickEventCond, m_dlgMouseRightClickObjEventCond);

	DECLARE_COND_DLG(ObjOnNetCond, m_dlgObjOnNetCond);
	DECLARE_COND_DLG(WalkStopDirCond, m_dlgPersStopDir);
	DECLARE_COND_DLG(WalkStopDirCond, m_dlgPersWalkDir);
	DECLARE_COND_DLG(NoDataCondDlg, m_dlgMouseDlgClickCond);
//	DECLARE_COND_DLG(MGInStateCond, m_dlgMGInStateCond);
	DECLARE_COND_DLG(ObjInStateCond, m_dlgObjInState);
	DECLARE_COND_DLG(ObjInStateCond, m_dlgObjWasInState);

	DECLARE_COND_DLG(ObjectDistanceCond, m_dlgObjectDistance);
	DECLARE_COND_DLG(MovingActiveCond, m_dlgMovingActive);
	DECLARE_COND_DLG(ObjInStateCond, m_dlgObjStateWaiting);
	DECLARE_COND_DLG(StateAnimInPhaseDlg, m_dlgStateAnimInPhaseDlg);
	DECLARE_COND_DLG(ObjPrevStateCond, m_dlgObjPrevState);
	DECLARE_COND_DLG(WTGreateWTCond, m_dlgWTGreateWTCond);
	DECLARE_COND_DLG(WTGreaterValue, m_dlgWTGreaterValue);
	DECLARE_COND_DLG(WTInIntervalCond, m_dlgWTInIntervalCond);
	DECLARE_COND_DLG(CounterGreaterCounterCondDlg, m_dlgCounterGrCounter);
	DECLARE_COND_DLG(CounterLGValue, m_dlgCounterGrValue);
	DECLARE_COND_DLG(CounterLGValue, m_dlgCounterLessValue);
	DECLARE_COND_DLG(CounterInIntervalCondDlg, m_dlgCounterInInterval);
	DECLARE_COND_DLG(ObjOnPersonageWayCond, m_dlgObjOnPersonageWay);
	DECLARE_COND_DLG(KeyPressedCondDlg, m_dlgKeyPressedCondDlg);
	DECLARE_COND_DLG(AnyPersonageOnNetDlgCond, m_dlgAnyPersonageOnGridZone);

	DECLARE_COND_DLG(MouseStatePhraseClickCondDlg, m_dlgMouseStatePhraseClickCondDlg);
	DECLARE_COND_DLG(ObjectIsCloserCondDlg, m_dlgObjectIsCloseCondDlg);
	SIMPLE_DECLARE_COND_DLG(AnimObjIdleCondDlg, m_dlgAnimObjIdleCond);
	DECLARE_COND_DLG(TwoObjIntersCondDlg, m_dlgTwoObjIntersCond);

	//! Диалог текущего условия
	CondDlgBase*			m_ptrCurrDlg;

#ifdef _DEBUG
	bool m_bInitCalled;
#endif

public:

	CondDlgMgr(void);
	~CondDlgMgr(void);

	//! Инициализирует диалоги условий
	void init(MakeCondDlgBase* pdlg, CWnd* parent, CRect const& rcDlgPlace);

	//! Выставляет текущий диалог по номеру условия
	/*!
	\param id - в младшем слове содержит идентификатор условия. В старшем содежит 
	признак того, является ли состояние отрицанием
	*/
	void set_current(DWORD id);
	CondDlgBase* get_current(){
		return m_ptrCurrDlg;
	}
private:
	//! Инициализирует(создает его и позиционирует) отдельный диалог условий
	template<class _Dlg>
	void init_dlg(_Dlg& dlg, MakeCondDlgBase* pdlg, 
		CWnd* parent, CRect const& rcDlgPlace);
};
