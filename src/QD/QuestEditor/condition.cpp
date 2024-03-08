#include "stdafx.h"
#include "questeditor.h"
#include "directionnamebyangle.h"
// #include "obj_type_detectors.h"
#include "Main/ObjectTraits.h"
#include <cmath>

#include "qd_condition.h"
#include "condition.h"
#include "qd_animation_set.h"

#include <fstream>
//////////////////////////////////////////////////////////////////////////

//! Таблица условий(прямых и отрицательных)
CONDITION g_arrConds[] = {
	{
		_T("Клик мышью по объекту"),			
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_CLICK)
	},
	{
		_T("Клик мышью по зоне"),
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_ZONE_CLICK)
	},
	{
		_T("Клик мышью по фразе в диалоге"),
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_DIALOG_CLICK)
	},
	{
		_T("Клик объектом"),
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_OBJECT_CLICK)
	},
	{
		_T("Клик объектом по зоне"),
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_OBJECT_ZONE_CLICK)
	},

	{
		_T("Клик правой кнопкой мыши по объекту"),
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_RIGHT_CLICK)
	},
	{
		_T("Клик правой кнопкой мыши по зоне"),
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_RIGHT_ZONE_CLICK)
	},
//	{_T("Клик правой кнопкой мыши по фразе в диалоге"),	MAKE_NORMAL(qdCondition::CONDITION_MOUSE_DIALOG_CLICK)},
	{
		_T("Правый клик объектом"),
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_CLICK)
	},
	{
		_T("Правый клик объектом по зоне"),			
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_ZONE_CLICK)
	},

	{
		_T("Мышь над объектом"),
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_HOVER)
	},
	{
		_T("Мышь над зоной"),
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_HOVER_ZONE)
	},
	{
		_T("Объект на мыши над объектом"),	
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_OBJECT_HOVER)
	},
	{
		_T("Объект на мыши над зоной"),	
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_OBJECT_HOVER_ZONE)
	},

	{
		_T("Объект скрыт"),				
		MAKE_NORMAL(qdCondition::CONDITION_OBJECT_HIDDEN)
	},

	{
		_T("Объект в зоне сетки"),	
		MAKE_NORMAL(qdCondition::CONDITION_OBJECT_IN_ZONE)
	},
	{
		_T("Объект в состоянии"), 
		MAKE_NORMAL(qdCondition::CONDITION_OBJECT_STATE)
	},
	{
		_T("Состояние объекта ждет активации"), 
			MAKE_NORMAL(qdCondition::CONDITION_OBJECT_STATE_WAITING)
	},
	{
		_T("Объект был в состоянии"), 
		MAKE_NORMAL(qdCondition::CONDITION_OBJECT_STATE_WAS_ACTIVATED)
	},
	{
		_T("Персонаж идет в направлении"),
		MAKE_NORMAL(qdCondition::CONDITION_PERSONAGE_WALK_DIRECTION)
	},
	{
		_T("Персонаж направлен"),	
		MAKE_NORMAL(qdCondition::CONDITION_PERSONAGE_STATIC_DIRECTION)
	},
	{
		_T("Персонаж активен"), 
		MAKE_NORMAL(qdCondition::CONDITION_PERSONAGE_ACTIVE)
	},
	{
		_T("Таймер"),	
		MAKE_NORMAL(qdCondition::CONDITION_TIMER)
	},
//	{
//		_T("Мини-игра в состоянии"), 
//		MAKE_NORMAL(qdCondition::CONDITION_MINIGAME_STATE)
//	},
	{
		_T("Объекты находятся на расстоянии"), 
		MAKE_NORMAL(qdCondition::CONDITION_OBJECTS_DISTANCE)
	},
	{
		_T("Анимация состояния в фазе"), 
		MAKE_NORMAL(qdCondition::CONDITION_OBJECT_STATE_ANIMATION_PHASE)
	},
	{
		_T("Предыдущее состояние объекта"), 
		MAKE_NORMAL(qdCondition::CONDITION_OBJECT_PREV_STATE)
	},
	{
		_T("Время работы сост-я больше времени работы др. сост-я"), 
		MAKE_NORMAL(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_STATE_TIME)
	},
	{
		_T("Время работы состояния попадает в интервал"), 
		MAKE_NORMAL(qdCondition::CONDITION_STATE_TIME_IN_INTERVAL)
	},
	{
		_T("Время работы состояния больше чем"),
		MAKE_NORMAL(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_VALUE)
	},
	{
		_T("Счетчик больше счетчика"),
		MAKE_NORMAL(qdCondition::CONDITION_COUNTER_GREATER_THAN_COUNTER)
	},
	{
		_T("Счетчик больше заданного значения"),
		MAKE_NORMAL(qdCondition::CONDITION_COUNTER_GREATER_THAN_VALUE)
	},
	{
		_T("Счетчик меньше заданного значения"),
		MAKE_NORMAL(qdCondition::CONDITION_COUNTER_LESS_THAN_VALUE)
	},
	{
		_T("Счетчик в заданном интервале"),
		MAKE_NORMAL(qdCondition::CONDITION_COUNTER_IN_INTERVAL)
	},
	{
		_T("Объект на пути персонажа"),
		MAKE_NORMAL(qdCondition::CONDITION_OBJECT_ON_PERSONAGE_WAY)
	},
	{
		_T("Нажата клавиша"),
		MAKE_NORMAL(qdCondition::CONDITION_KEYPRESS)
	},
	{
		_T("В зоне есть хотя бы один песонаж"),
		MAKE_NORMAL(qdCondition::CONDITION_ANY_PERSONAGE_IN_ZONE)
	},

	{
		_T("Неудачный клик мыши"),
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_CLICK_FAILED)
	},

	{
		_T("Неудачный клик объектом на мыши"),
		MAKE_NORMAL(qdCondition::CONDITION_MOUSE_OBJECT_CLICK_FAILED)
	},

	{
		_T("Клик мышью куда угодно"),
			MAKE_NORMAL(qdCondition::CONDITION_MOUSE_CLICK_EVENT)
	},

	{
		_T("Клик объектом куда угодно"),
			MAKE_NORMAL(qdCondition::CONDITION_MOUSE_OBJECT_CLICK_EVENT)
	},

	{
		_T("Правый клик мышью куда угодно"),
			MAKE_NORMAL(qdCondition::CONDITION_MOUSE_RIGHT_CLICK_EVENT)
	},

	{
		_T("Правый клик объектом куда угодно"),
			MAKE_NORMAL(qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_CLICK_EVENT)
	},

	{
		_T("Клик по фразе состояния"),
			MAKE_NORMAL(qdCondition::CONDITION_MOUSE_STATE_PHRASE_CLICK)
	},

	{
		_T("Ближайший объект"),
			MAKE_NORMAL(qdCondition::CONDITION_OBJECT_IS_CLOSER)
	},

	{
		_T("Объект бездействует некоторое время"),
			MAKE_NORMAL(qdCondition::CONDITION_ANIMATED_OBJECT_IDLE_GREATER_THAN_VALUE)
	},

	{
		_T("Баунды объектов пересекаются"),
			MAKE_NORMAL(qdCondition::CONDITION_ANIMATED_OBJECTS_INTERSECTIONAL_BOUNDS)
	},

	{
		_T("Нет клика мышью по объекту"),
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_CLICK)
	},
	{
		_T("Нет клика мышью по зоне"),
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_ZONE_CLICK)
	},
	{
		_T("Нет клика мышью по фразе в диалоге"),	
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_DIALOG_CLICK)
	},
	{
		_T("Нет клика объектом"),
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_OBJECT_CLICK)
	},
	{
		_T("Нет клика объектом по зоне"),			
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_OBJECT_ZONE_CLICK)
	},

	{
		_T("Нет клика правой кнопкой мыши по объекту"),	
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_RIGHT_CLICK)
	},
	{
		_T("Нет клика правой кнопкой мыши по зоне"),		
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_RIGHT_ZONE_CLICK)
	},
//	{_T("Клик правой кнопкой мыши по фразе в диалоге"),	MAKE_INVERS(qdCondition::CONDITION_MOUSE_DIALOG_CLICK)},
	{
		_T("Нет правого клика объектом"),
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_CLICK)
	},
	{
		_T("Нет правого клика объектом по зоне"),
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_ZONE_CLICK)
	},

	{
		_T("Мышь не над объектом"),			
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_HOVER)
	},
	{
		_T("Мышь вне зоны"),
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_HOVER_ZONE)
	},
	{
		_T("Объект на мыши не над объектом"),	
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_OBJECT_HOVER)
	},
	{
		_T("Объект на мыши вне зоны"),		
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_OBJECT_HOVER_ZONE)
	},

	{
		_T("Объект не в зоне сетки"),	
		MAKE_INVERS(qdCondition::CONDITION_OBJECT_IN_ZONE)
	},
	{
		_T("Объект не в состоянии"), 
		MAKE_INVERS(qdCondition::CONDITION_OBJECT_STATE)
	},
	{
		_T("Состояние объекта не ждет активации"), 
		MAKE_INVERS(qdCondition::CONDITION_OBJECT_STATE_WAITING)
	},
	{
		_T("Объект не был в состоянии"), 
		MAKE_INVERS(qdCondition::CONDITION_OBJECT_STATE_WAS_ACTIVATED)
	},
	{
		_T("Персонаж не идет в направлении"),
		MAKE_INVERS(qdCondition::CONDITION_PERSONAGE_WALK_DIRECTION)
	},
	{
		_T("Персонаж не направлен"),	
		MAKE_INVERS(qdCondition::CONDITION_PERSONAGE_STATIC_DIRECTION)
	},
	{
		_T("Персонаж не активен"), 
		MAKE_INVERS(qdCondition::CONDITION_PERSONAGE_ACTIVE)
	},
//	{
//		_T("Мини-игра не в состоянии"), 
//		MAKE_INVERS(qdCondition::CONDITION_MINIGAME_STATE)
//	},
	{
		_T("Объекты не находятся на расстоянии"), 
		MAKE_INVERS(qdCondition::CONDITION_OBJECTS_DISTANCE)
	},
	{
		_T("Анимация состояния не в фазе"), 
		MAKE_INVERS(qdCondition::CONDITION_OBJECT_STATE_ANIMATION_PHASE)
	},
	{
		_T("Не предыдущее состояние объекта"), 
		MAKE_INVERS(qdCondition::CONDITION_OBJECT_PREV_STATE)
	},
	{
		_T("Время работы сост-я небольше времени работы др. сост-я"), 
		MAKE_INVERS(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_STATE_TIME)
	},
	{
		_T("Время работы состояния не попадает в интервал"), 
		MAKE_INVERS(qdCondition::CONDITION_STATE_TIME_IN_INTERVAL)
	},
	{
		_T("Время работы состояния небольше чем"),
		MAKE_INVERS(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_VALUE)
	},
	{
		_T("Счетчик небольше счетчика"),
		MAKE_INVERS(qdCondition::CONDITION_COUNTER_GREATER_THAN_COUNTER)
	},
	{
		_T("Счетчик небольше заданного значения"),
		MAKE_INVERS(qdCondition::CONDITION_COUNTER_GREATER_THAN_VALUE)
	},
	{
		_T("Счетчик неменьше заданного значения"),
		MAKE_INVERS(qdCondition::CONDITION_COUNTER_LESS_THAN_VALUE)
	},
	{
		_T("Счетчик вне заданного интервала"),
		MAKE_INVERS(qdCondition::CONDITION_COUNTER_IN_INTERVAL)
	},
	{
		_T("Объект не на пути персонажа"),
		MAKE_INVERS(qdCondition::CONDITION_OBJECT_ON_PERSONAGE_WAY)
	},
	{
		_T("Клавиша не нажата "),
		MAKE_INVERS(qdCondition::CONDITION_KEYPRESS)
	},
	{
		_T("В зоне нет ни одного персонажа"),
		MAKE_INVERS(qdCondition::CONDITION_ANY_PERSONAGE_IN_ZONE)
	},
	{
		_T("Объект нескрыт"),				
		MAKE_INVERS(qdCondition::CONDITION_OBJECT_HIDDEN)
	},

	{
		_T("Нет неудачного клика мышью"),
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_CLICK_FAILED)
	},

	{
		_T("Нет неудачного клика объектом на мыши"),
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_OBJECT_CLICK_FAILED)
	},

	{
		_T("Нет клик по фразе состояния"),
		MAKE_INVERS(qdCondition::CONDITION_MOUSE_STATE_PHRASE_CLICK)
	},

	{
		_T("Удаленный объект"),
		MAKE_INVERS(qdCondition::CONDITION_OBJECT_IS_CLOSER)
	},

	{
		_T("Баунды объектов не пересекаются"),
		MAKE_INVERS(qdCondition::CONDITION_ANIMATED_OBJECTS_INTERSECTIONAL_BOUNDS)
	},
};
int g_iCondsSize = sizeof(g_arrConds)/sizeof(CONDITION);
Permutation g_permutation(g_iCondsSize);
//////////////////////////////////////////////////////////////////////////

int get_max_dirs();

#define IS_VALID(xxx) (xxx&&(*xxx))

//! Возвращает имя объекта, с указанием сцены
CString getObjectName(qdNamedObject const* p){
	ASSERT(p);
	CString str(p->name());
	if (ObjectTraits::is_obj_scene(p)
		||p->named_object_type()==QD_NAMED_OBJECT_DISPATCHER)
		return str;
	qdNamedObject const* owner = p->owner();
	if (!owner)
		return str;
	str += _T("::");
	str += owner->name();
	if (ObjectTraits::is_obj_scene(owner)
		||owner->named_object_type()==QD_NAMED_OBJECT_DISPATCHER)
		return str;
	owner = owner->owner();
	if (!owner)
		return str;
	str += _T("::");
	str += owner->name();
	return str;
}

inline CString get_ini_val(const qdNamedObject *const p){
	return (p? getObjectName(p) : CString());
}
//! Возвращает имя владельца объекта
inline LPCTSTR getOwnerObjName(qdNamedObject const* p){
	return p? getObjectName(p->owner()):CString();
}

CString GetMouseClickText(qdCondition* ptrCond)
{
	CString res;
	const qdNamedObject* const p = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR lpstrData = NULL;
		ptrCond->get_value(qdCondition::OBJECT_NAME, lpstrData);
		strData = lpstrData;
	}

	UINT formatID = IDS_COND_TMPL_MOUSE_CLICK;
	if (ptrCond->is_inversed()) 
		formatID = IDS_COND_TMPL_MOUSE_CLICK_NOT;

	if(!strData.IsEmpty())
		res.Format(formatID, static_cast<LPCTSTR>(strData));
	else
	{
		CString str;
		str.LoadString(IDS_COND_MOUSE_CLICK_OBJ_SELF);
		res.Format(formatID, static_cast<LPCTSTR>(str));
	}

	return res;
}

CString GetMouseObjClickText(qdCondition* ptrCond)
{
	CString res;

	const qdNamedObject* const p = ptrCond->get_object(qdCondition::MOUSE_OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::MOUSE_OBJECT_NAME, data);
		strData = data;
	}

	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	const qdNamedObject* const p2 = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData1 = get_ini_val(p2);
	if (strData1.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_NAME, data);
		strData1 = data;
	}
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_MOUSE_OBJECT_CLICK_NOT, 
				(LPCTSTR)strData, (LPCTSTR)strData1);
	else
		res.Format(IDS_COND_TMPL_MOUSE_OBJECT_CLICK,  
				(LPCTSTR)strData, (LPCTSTR)strData1);
	return res;
}

CString GetObjInZoneText(qdCondition* ptrCond)
{
	CString res;

	const qdNamedObject* const p = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_NAME, data);
		strData = data;
	}

	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	const qdNamedObject* const p2 = ptrCond->get_object(qdCondition::ZONE_NAME);
	CString strData1 = get_ini_val(p2);
	if (strData1.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::ZONE_NAME, data);
		strData1 = data;
	}
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);
	
	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_OBJECT_IN_ZONE_NOT,  
						(LPCTSTR)strData, (LPCTSTR)strData1);
	else
		res.Format(IDS_COND_TMPL_OBJECT_IN_ZONE,  
						(LPCTSTR)strData, (LPCTSTR)strData1);
	return res;
}

CString GetTimerText(qdCondition* ptrCond)
{
	CString res;
	float fPeriod = 0.f;
	int iRnd = 0;

	ptrCond->get_value(qdCondition::TIMER_PERIOD, fPeriod, 0);
	ptrCond->get_value(qdCondition::TIMER_RND, iRnd, 0);

	res.Format(IDS_COND_TMPL_TIMER, fPeriod, iRnd);

	return res;
}

CString GetStaticDirText(qdCondition* ptrCond)
{
	CString res;
	float fAngle = 0.f;

	const qdNamedObject* const p = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_NAME, data);
		strData = data;
		if (strData.IsEmpty())
			strData.LoadString(IDS_STATE_OWNER);
	}
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	ptrCond->get_value(qdCondition::DIRECTION_ANGLE, fAngle, 0);
	fAngle = fAngle* 180.f/M_PI;

	CString strText;
	strText.Format(IDS_GAIT_ANGLE_FORMAT, fAngle);
	strText += DirectionNameByAngle::get(fAngle);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_STATIC_DIRECTION2_NOT, (LPCTSTR)strData,
			static_cast<LPCTSTR>(strText));
	else
		res.Format(IDS_COND_TMPL_STATIC_DIRECTION2, (LPCTSTR)strData,
			static_cast<LPCTSTR>(strText));

	return res;
}

CString GetWalkDirText(qdCondition* ptrCond)
{
	CString res;
	float fAngle = 0.f;

	const qdNamedObject* const p = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_NAME, data);
		strData = data;
		if (strData.IsEmpty())
			strData.LoadString(IDS_STATE_OWNER);
	}
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	ptrCond->get_value(qdCondition::DIRECTION_ANGLE, fAngle, 0);
	fAngle = fAngle* 180.f/M_PI;

	CString strText;
	strText.Format(IDS_GAIT_ANGLE_FORMAT, fAngle);
	strText += DirectionNameByAngle::get(fAngle);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_WALK_DIRECTION2_NOT, static_cast<LPCTSTR>(strData), 
			static_cast<LPCTSTR>(strText));
	else
		res.Format(IDS_COND_TMPL_WALK_DIRECTION2, static_cast<LPCTSTR>(strData),
			static_cast<LPCTSTR>(strText));

	return res;
}

CString GetMouseDlgClick(qdCondition* ptrCond){

	if (ptrCond->is_inversed()) 
		return CString((LPTSTR)IDS_COND_TMPL_MOUSE_DLG_CLICK_NOT);
	else
		return CString((LPTSTR)IDS_COND_TMPL_MOUSE_DLG_CLICK);
}

CString GetMGInStateText(qdCondition* ptrCond){
	CString res;

	const qdNamedObject* const p = ptrCond->get_object(qdCondition::MINIGAME_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::MINIGAME_NAME, data);
		strData = data;
	}
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	LPCTSTR ptrStrData2 = NULL;
	ptrCond->get_value(qdCondition::MINIGAME_STATE_NAME, ptrStrData2);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_MG_IN_STATE_NOT, 
					static_cast<LPCTSTR>(strData), ptrStrData2);
	else
		res.Format(IDS_COND_TMPL_MG_IN_STATE, 
					static_cast<LPCTSTR>(strData), ptrStrData2);
	return res;
}

CString GetObjectStateText(qdCondition* ptrCond){
	CString res;

	const qdNamedObject* const p = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_NAME, data);
		strData = data;
	}
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	const qdNamedObject* const p2 = ptrCond->get_object(qdCondition::OBJECT_STATE_NAME);
	CString strData1 = get_ini_val(p2);
	if (strData1.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_STATE_NAME, data);
		strData1 = data;
	}
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_OBJ_IN_STATE_NOT, 
								(LPCTSTR)strData, (LPCTSTR)strData1);
	else
		res.Format(IDS_COND_TMPL_OBJ_IN_STATE, 
								(LPCTSTR)strData, (LPCTSTR)strData1);
	return res;
}

CString GetObjectPrevStateText(qdCondition* ptrCond){
	CString res;

	const qdNamedObject* const p = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_NAME, data);
		strData = data;
	}
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	const qdNamedObject* const p2 = ptrCond->get_object(qdCondition::OBJECT_STATE_NAME);
	CString strData1 = get_ini_val(p2);
	if (strData1.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_STATE_NAME, data);
		strData1 = data;
	}
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_OBJ_PREV_STATE_NOT,  
						(LPCTSTR)strData, (LPCTSTR)strData1);
	else 
	{
		if (!strData1.IsEmpty())
			res.Format(IDS_COND_TMPL_OBJ_PREV_STATE,  
						(LPCTSTR)strData, (LPCTSTR)strData1);
		else 
			res.Format(IDS_COND_TMPL_OBJ_PREV_STATE_EMPTY, (LPCTSTR)strData);
	}

	return res;
}

CString GetMouseClickZoneText(qdCondition* ptrCond)
{
	CString res;
	const qdNamedObject* const p = ptrCond->get_object(qdCondition::CLICK_ZONE_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::CLICK_ZONE_NAME, data);
		strData = data;
	}
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_MOUSE_ZONE_CLICK_NOT, (LPCTSTR)strData);
	else
		res.Format(IDS_COND_TMPL_MOUSE_ZONE_CLICK, (LPCTSTR)strData);

	return res;
}

CString GetMouseObjZoneClickText(qdCondition* ptrCond)
{
	CString res;

	const qdNamedObject* const p  = ptrCond->get_object(qdCondition::MOUSE_OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::MOUSE_OBJECT_NAME, data);
		strData = data;
	}
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	const qdNamedObject* const p2 = ptrCond->get_object(qdCondition::CLICK_ZONE_NAME);
	CString strData1 = get_ini_val(p2);
	if (strData1.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::CLICK_ZONE_NAME, data);
		strData1 = data;
	}
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_MOUSE_OBJECT_ZONE_CLICK_NOT,  
						(LPCTSTR)strData, (LPCTSTR)strData1);
	else
		res.Format(IDS_COND_TMPL_MOUSE_OBJECT_ZONE_CLICK,  
						(LPCTSTR)strData, (LPCTSTR)strData1);
	return res;
}

CString GetObjectWasStateText(qdCondition* ptrCond){
	CString res;

	const qdNamedObject* const p  = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_NAME, data);
		strData = data;
	}
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	const qdNamedObject* const p2 = ptrCond->get_object(qdCondition::OBJECT_STATE_NAME);
	CString strData1 = get_ini_val(p2);
	if (strData1.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_STATE_NAME, data);
		strData1 = data;
	}
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_OBJ_WAS_IN_STATE_NOT,  
						(LPCTSTR)strData, (LPCTSTR)strData1);
	else
		res.Format(IDS_COND_TMPL_OBJ_WAS_IN_STATE,  
						(LPCTSTR)strData, (LPCTSTR)strData1);
	return res;
}

CString GetObjectsDistanceText(qdCondition* ptrCond){
	CString res;
	float fDist = 0.f;

	const qdNamedObject* const p  = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_NAME, data);
		strData = data;
	}
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	const qdNamedObject* const p2 = ptrCond->get_object(qdCondition::OBJECT2_NAME);
	CString strData1 = get_ini_val(p2);
	if (strData1.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT2_NAME, data);
		strData1 = data;
	}
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);
	
	ptrCond->get_value(qdCondition::OBJECTS_DISTANCE, fDist);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_OBJS_DISTANCE_NOT, (LPCTSTR)strData, 
					fDist, (LPCTSTR)strData1);
	else
		res.Format(IDS_COND_TMPL_OBJS_DISTANCE, (LPCTSTR)strData, 
					fDist, (LPCTSTR)strData1);
	return res;
}

CString GetPersonageActiveText(qdCondition* ptrCond)
{
	CString res;
	const qdNamedObject* const p = ptrCond->get_object(qdCondition::PERSONAGE_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::PERSONAGE_NAME, data);
		strData = data;
	}
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_PERS_ACTIVE_NOT, (LPCTSTR)strData);
	else
		res.Format(IDS_COND_TMPL_PERS_ACTIVE, (LPCTSTR)strData);
	return res;
}

CString GetObjStateWaitingText(qdCondition* ptrCond)
{
	CString res;
	const qdNamedObject* const p = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_NAME, data);
		strData = data;
	}
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	const qdNamedObject* const p2 = ptrCond->get_object(qdCondition::OBJECT_STATE_NAME);
	CString strData1 = get_ini_val(p2);
	if (strData1.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_STATE_NAME, data);
		strData1 = data;
	}
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_OBJ_STATE_WAITING_NOT,  
						(LPCTSTR)strData, (LPCTSTR)strData1);
	else
		res.Format(IDS_COND_TMPL_OBJ_STATE_WAITING,  
						(LPCTSTR)strData, (LPCTSTR)strData1);
	return res;
}

CString GetStateAnimPhaseText(qdCondition* ptrCond)
{
	CString res;
	const qdNamedObject* const p = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_NAME, data);
		strData = data;
	}
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	const qdNamedObject* const p2 = ptrCond->get_object(qdCondition::OBJECT_STATE_NAME);
	CString strData1 = get_ini_val(p2);
	if (strData1.IsEmpty())
	{
		LPCTSTR data = NULL;
		ptrCond->get_value(qdCondition::OBJECT_STATE_NAME, data);
		strData1 = data;
	}
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	float from = 0.f;
	ptrCond->get_value(qdCondition::ANIMATION_PHASE, from, 0);
	float to = 1.f;
	ptrCond->get_value(qdCondition::ANIMATION_PHASE, to, 1);

	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_STATE_ANIM_PHASE_NOT,  
						(LPCTSTR)strData1, (LPCTSTR)strData,
				round(from*100), round(to*100));
	else
		res.Format(IDS_COND_TMPL_STATE_ANIM_PHASE, (LPCTSTR)strData1, (LPCTSTR)strData,
				round(from*100), round(to*100));
	return res;
}

CString GetWTGreaterWTText(qdCondition* ptrCond)
{
	CString res;
	qdNamedObject const * const p = ptrCond->get_object(0);
	qdNamedObject const * const p2 = ptrCond->get_object(1);
	CString strData = get_ini_val(p);
	CString strData1 = get_ini_val(p2);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_STATE_WORK_TIME_GREATER_WORK_TIME_NOT, 
			(LPCTSTR)strData, getOwnerObjName(p), (LPCTSTR)strData, getOwnerObjName(p2));
	else
		res.Format(IDS_COND_TMPL_STATE_WORK_TIME_GREATER_WORK_TIME, 
			(LPCTSTR)strData, getOwnerObjName(p), (LPCTSTR)strData, getOwnerObjName(p2));

	return res;
}

CString GetWTInIntervalText(qdCondition* ptrCond)
{
	CString res;
	const qdNamedObject* const p = ptrCond->get_object(0);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);
	float fLo = 0.f;
	float fHi = 0.f;
	ptrCond->get_value(0, fLo, 0);
	ptrCond->get_value(0, fHi, 1);
	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_STATE_WORK_TIME_IN_INTERVAL_NOT, (LPCTSTR)strData, 
			getOwnerObjName(p), fLo, fHi);
	else
		res.Format(IDS_COND_TMPL_STATE_WORK_TIME_IN_INTERVAL, (LPCTSTR)strData, 
			getOwnerObjName(p), fLo, fHi);

	return res;
}

CString GetWTGreaterValueText(qdCondition* ptrCond)
{
	CString res;
	qdNamedObject const* const p = ptrCond->get_object(0);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	float fValue = 0.f;
	ptrCond->get_value(0, fValue, 0);
	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_STATE_WORK_TIME_GREATER_VALUE_NOT, (LPCTSTR)strData, 
				getOwnerObjName(p), fValue);
	else
		res.Format(IDS_COND_TMPL_STATE_WORK_TIME_GREATER_VALUE, (LPCTSTR)strData, 
				getOwnerObjName(p), fValue);
	return res;
}

CString GetCounterGreaterCounterText(qdCondition* ptrCond)
{
	CString res;
	qdNamedObject const* const p = ptrCond->get_object(0);
	qdNamedObject const* const p1 = ptrCond->get_object(1);
	CString strData = get_ini_val(p);
	CString strData1 = get_ini_val(p1);

	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);


	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_COUNTER_GREATER_COUNTER_NOT,  
						(LPCTSTR)strData, (LPCTSTR)strData1);
	else
		res.Format(IDS_COND_TMPL_COUNTER_GREATER_COUNTER,  
						(LPCTSTR)strData, (LPCTSTR)strData1);
	return res;
}
CString GetCounterGreaterValueText(qdCondition* ptrCond)
{
	CString res;
	qdNamedObject const* const p = ptrCond->get_object(0);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	int iValue = 0;
	ptrCond->get_value(0, iValue, 0);
	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_COUNTER_GREATER_VALUE_NOT, (LPCTSTR)strData, iValue);
	else
		res.Format(IDS_COND_TMPL_COUNTER_GREATER_VALUE, (LPCTSTR)strData, iValue);
	return res;
}
CString GetCounterLessValueText(qdCondition* ptrCond)
{
	CString res;
	qdNamedObject const* const p = ptrCond->get_object(0);
	CString strData = get_ini_val(p);

	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	int iValue = 0;
	ptrCond->get_value(0, iValue, 0);
	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_COUNTER_LESS_VALUE_NOT, (LPCTSTR)strData, iValue);
	else
		res.Format(IDS_COND_TMPL_COUNTER_LESS_VALUE, (LPCTSTR)strData, iValue);
	return res;
}

CString GetCounterInIntervalText(qdCondition* ptrCond)
{
	CString res;
	qdNamedObject const* const p = ptrCond->get_object(0);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);
	int iValue = 0;
	ptrCond->get_value(0, iValue, 0);
	int iValue1 = 0;
	ptrCond->get_value(0, iValue1, 1);
	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_COUNTER_IN_INTERVAL_NOT, 
					(LPCTSTR)strData, iValue, iValue1);
	else
		res.Format(IDS_COND_TMPL_COUNTER_IN_INTERVAL, 
					(LPCTSTR)strData, iValue, iValue1);
	return res;
}

CString GetObjectOnPersonageWayText(qdCondition* ptrCond)
{
	CString res;
	qdNamedObject const* const p = ptrCond->get_object(0);
	qdNamedObject const* const p2 = ptrCond->get_object(1);
	CString strData = get_ini_val(p);
	CString strData1 = get_ini_val(p2);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	float fMaxDist = 0.f;
	ptrCond->get_value(0, fMaxDist, 0);
	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_OBJECT_ON_PERSONAGE_WAY_NOT,
			(LPCTSTR)strData1, (LPCTSTR)strData, fMaxDist);
	else 
		res.Format(IDS_COND_TMPL_OBJECT_ON_PERSONAGE_WAY,
			(LPCTSTR)strData1, (LPCTSTR)strData, fMaxDist);
	return res;
}

CString GetKeypressedText(qdCondition* ptrCond)
{
	CString res;
	int vKey = 0;
	ptrCond->get_value(0, vKey, 0);
	int const BUFF_SIZE = 32;
	TCHAR buff[BUFF_SIZE];
	LONG skey = static_cast<LONG>(MapVirtualKey(vKey, 0))<<16;
	GetKeyNameText(skey, buff, BUFF_SIZE);
	if (ptrCond->is_inversed())
		res.Format(IDC_COND_TMPL_KEY_PRESSED_NOT, buff);
	else
		res.Format(IDC_COND_TMPL_KEY_PRESSED, buff);
	return res;
}

CString GetAnyPersonageInZoneText(qdCondition* ptrCond)
{
	qdNamedObject const* const p = ptrCond->get_object(0);
	CString res;
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);
	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_ANY_PERSONGE_IN_ZONE_NOT,(LPCTSTR)strData);
	else
		res.Format(IDS_COND_TMPL_ANY_PERSONGE_IN_ZONE, (LPCTSTR)strData);
	return res;
}

CString GetMouseRightClickText(qdCondition* ptrCond)
{
	CString res;
	const qdNamedObject* const p = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);

	UINT formatID = IDS_COND_TMPL_MOUSE_RIGHT_CLICK;
	if (ptrCond->is_inversed()) 
		formatID = IDS_COND_TMPL_MOUSE_RIGHT_CLICK_NOT;

	if(!strData.IsEmpty())
		res.Format(formatID, static_cast<LPCTSTR>(strData));
	else
	{
		CString str;
		str.LoadString(IDS_COND_MOUSE_CLICK_OBJ_SELF);
		res.Format(formatID, static_cast<LPCTSTR>(str));
	}

	return res;
}

CString GetMouseRightObjClickText(qdCondition* ptrCond)
{
	CString res;
	res.LoadString(IDS_NOTHING);

	const qdNamedObject* const p = ptrCond->get_object(qdCondition::MOUSE_OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	const qdNamedObject* const p2 = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData1 = get_ini_val(p2);
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_MOUSE_RIGHT_OBJECT_CLICK_NOT, 
		(LPCTSTR)strData, (LPCTSTR)strData1);
	else
		res.Format(IDS_COND_TMPL_MOUSE_RIGHT_OBJECT_CLICK, 
						(LPCTSTR)strData, (LPCTSTR)strData1);
	return res;
}

CString GetMouseRightClickZoneText(qdCondition* ptrCond)
{
	CString res;
	res.LoadString(IDS_NOTHING);

	const qdNamedObject* const p = ptrCond->get_object(qdCondition::CLICK_ZONE_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_MOUSE_RIGHT_ZONE_CLICK_NOT, (LPCTSTR)strData);
	else
		res.Format(IDS_COND_TMPL_MOUSE_RIGHT_ZONE_CLICK, (LPCTSTR)strData);

	return res;
}

CString GetMouseRightObjZoneClickText(qdCondition* ptrCond)
{
	CString res;
	res.LoadString(IDS_NOTHING);

	const qdNamedObject* const p  = ptrCond->get_object(qdCondition::MOUSE_OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	const qdNamedObject* const p2 = ptrCond->get_object(qdCondition::CLICK_ZONE_NAME);
	CString strData1 = get_ini_val(p2);
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_MOUSE_RIGHT_OBJECT_ZONE_CLICK_NOT,  
				(LPCTSTR)strData, (LPCTSTR)strData1);
	else
		res.Format(IDS_COND_TMPL_MOUSE_RIGHT_OBJECT_ZONE_CLICK,  
				(LPCTSTR)strData, (LPCTSTR)strData1);
	return res;
}

CString GetObjectHiddenText(qdCondition* ptrCond)
{
	CString res;
	res.LoadString(IDS_NOTHING);

	qdNamedObject const* const p  = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_OBJECT_HIDDEN_NOT, (LPCTSTR)strData);
	else
		res.Format(IDS_COND_TMPL_OBJECT_HIDDEN, (LPCTSTR)strData);

	return res;
}

CString GetMouseHoverText(qdCondition* ptrCond)
{
	CString res;
	res.LoadString(IDS_NOTHING);

	const qdNamedObject* const p = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);

	UINT formatID = IDS_COND_TMPL_MOUSE_HOVER;
	if (ptrCond->is_inversed()) 
		formatID = IDS_COND_TMPL_MOUSE_HOVER_NOT;

	if(!strData.IsEmpty())
		res.Format(formatID, static_cast<LPCTSTR>(strData));
	else
	{
		CString str;
		str.LoadString(IDS_COND_MOUSE_HOVER_OBJ_SELF);
		res.Format(formatID, static_cast<LPCTSTR>(str));
	}

	return res;

}

CString GetMouseObjectHoverText(qdCondition* ptrCond)
{
	CString res;
	res.LoadString(IDS_NOTHING);

	const qdNamedObject* const p = ptrCond->get_object(qdCondition::MOUSE_OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	const qdNamedObject* const p2 = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData1 = get_ini_val(p2);
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_MOUSE_OBJECT_HOVER_NOT, 
					(LPCTSTR)strData, (LPCTSTR)strData1);
	else
		res.Format(IDS_COND_TMPL_MOUSE_OBJECT_HOVER,  
					(LPCTSTR)strData, (LPCTSTR)strData1);
	return res;
}

CString GetMouseHoverZoneText(qdCondition* ptrCond)
{
	CString res;
	res.LoadString(IDS_NOTHING);

	const qdNamedObject* const p = ptrCond->get_object(qdCondition::CLICK_ZONE_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_MOUSE_ZONE_HOVER_NOT, (LPCTSTR)strData);
	else
		res.Format(IDS_COND_TMPL_MOUSE_ZONE_HOVER, (LPCTSTR)strData);

	return res;
}

CString GetMouseObjectHoverZoneText(qdCondition* ptrCond)
{
	CString res;
	res.LoadString(IDS_NOTHING);

	qdNamedObject const* const p  = ptrCond->get_object(qdCondition::MOUSE_OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	qdNamedObject const * const p2 = ptrCond->get_object(qdCondition::CLICK_ZONE_NAME);
	CString strData1 = get_ini_val(p2);
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed()) 
		res.Format(IDS_COND_TMPL_MOUSE_OBJECT_ZONE_HOVER_NOT,  
					(LPCTSTR)strData, (LPCTSTR)strData1);
	else
		res.Format(IDS_COND_TMPL_MOUSE_OBJECT_ZONE_HOVER,  
					(LPCTSTR)strData, (LPCTSTR)strData1);
	return res;
}

CString const GetMouseClickFailedText(qdCondition* pc)
{
	CString res;
	if (pc->is_inversed())
		res.LoadString(IDS_MOUSE_CLICK_FAILED_NOT);
	else
		res.LoadString(IDS_MOUSE_CLICK_FAILED);
	return res;
}

CString const GetMouseObjectClickFailedText(qdCondition* pc)
{
	CString res;

	if (pc->is_inversed())
		res.LoadString(IDS_MOUSE_OBJECT_CLICK_FAILED_NOT);
	else
		res.LoadString(IDS_MOUSE_OBJECT_CLICK_FAILED);
	return res;
}

CString const GetSimpleStringText(UINT_PTR strID)
{
	CString res;
	res.LoadString(strID);
	return res;
}

CString const GetMouseClickEventText(qdCondition* pc)
{
	return GetSimpleStringText(IDS_MOUSE_CLICK_EVENT_TEXT);
}

CString const GetMouseRightClickEventText(qdCondition* pc)
{
	return GetSimpleStringText(IDS_MOUSE_RIGHT_CLICK_EVENT_TEXT);
}

CString const GetMouseObjectClickEventText(qdCondition* pc)
{
//	return GetSimpleStringText(IDS_MOUSE_OBJECT_CLICK_EVENT_TEXT);
	CString res;
	res.LoadString(IDS_NOTHING);

	const qdNamedObject* const p = pc->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	ASSERT(!pc->is_inversed());
	res.Format(IDS_COND_TMPL_MOUSE_OBJECT_CLICK_EVENT_TEXT, (LPCTSTR)strData);

	return res;
}

CString const GetMouseRightObjectClickEventText(qdCondition* pc)
{
//	return GetSimpleStringText(IDS_MOUSE_RIGHT_OBJECT_CLICK_EVENT_TEXT);

	CString res;
	res.LoadString(IDS_NOTHING);

	const qdNamedObject* const p = pc->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	ASSERT(!pc->is_inversed());
	res.Format(IDS_COND_TMPL_MOUSE_RIGHT_OBJECT_CLICK_EVENT_TEXT, (LPCTSTR)strData);
	return res;
}

CString const GetMouseStatePhraseClick(qdCondition* ptrCond)
{
	CString res;
	const qdNamedObject* const p = ptrCond->get_object(qdCondition::OBJECT_NAME);
	CString strData = get_ini_val(p);
	if (strData.IsEmpty())
	{
		strData.LoadString(IDS_NOT_FOUND);
	}

	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_MOUSE_STATE_PHRASE_CLICK_NOT,  (LPCTSTR)strData);
	else
		res.Format(IDS_COND_TMPL_MOUSE_STATE_PHRASE_CLICK,  (LPCTSTR)strData);
	return res;
}

CString const GetObjectIsCloser(qdCondition* ptrCond)
{
	CString res;
	res.LoadString(IDS_NOT_FOUND);

	qdNamedObject const* obj = ptrCond->get_object(0);
	qdNamedObject const* cand1 = ptrCond->get_object(1);
	qdNamedObject const* cand2 = ptrCond->get_object(2);
	CString strData = get_ini_val(obj);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	CString strData1 = get_ini_val(cand1);
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	CString strData2 = get_ini_val(cand2);
	if (strData2.IsEmpty())
		strData2.LoadString(IDS_NOT_FOUND);

	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_OBJECT_IS_CLOSER_NOT,
					(LPCTSTR)strData1, (LPCTSTR)strData, (LPCTSTR)strData2);
	else
		res.Format(IDS_COND_TMPL_OBJECT_IS_CLOSER,  
					(LPCTSTR)strData1, (LPCTSTR)strData, (LPCTSTR)strData2);
	return res;
}

CString const GetAnimObjIdleGrtThanVal(qdCondition* ptrCond)
{
	const qdNamedObject* obj = ptrCond->get_object(0);
	CString strData = get_ini_val(obj);
	if (strData.IsEmpty())
		strData.LoadString(IDS_NOT_FOUND);

	int iVal;
	ptrCond->get_value(0, iVal);

	CString res;
	res.Format(IDS_COND_TMPL_ANIM_OBJ_IDLE_GRT_THAN_VAL, (LPCTSTR)strData, iVal); 

	return res;
}

CString const GetAnimObjsIntersBounds(qdCondition* ptrCond)
{
	const qdNamedObject* obj1 = ptrCond->get_object(0);
	CString strData1 = get_ini_val(obj1);
	if (strData1.IsEmpty())
		strData1.LoadString(IDS_NOT_FOUND);

	const qdNamedObject* obj2 = ptrCond->get_object(1);
	CString strData2 = get_ini_val(obj2);
	if (strData2.IsEmpty())
		strData2.LoadString(IDS_NOT_FOUND);

	CString res;
	if (ptrCond->is_inversed())
		res.Format(IDS_COND_TMPL_ANIM_OBJS_INTERS_BOUNDS_NOT, (LPCTSTR)strData1,
		             (LPCTSTR)strData2); 
	else
		res.Format(IDS_COND_TMPL_ANIM_OBJS_INTERS_BOUNDS, (LPCTSTR)strData1,
		             (LPCTSTR)strData2); 

	return res;
}
//////////////////////////////////////////////////////////////////////////

CString GetConditionText(const qdCondition* ptrCond)
{
	qdCondition* const pc = const_cast<qdCondition*>(ptrCond);
	switch(pc->type()) {
	case qdCondition::CONDITION_MOUSE_CLICK:
		return GetMouseClickText(pc);
	case qdCondition::CONDITION_MOUSE_OBJECT_CLICK:
		return GetMouseObjClickText(pc);
	case qdCondition::CONDITION_MOUSE_ZONE_CLICK:
		return GetMouseClickZoneText(pc);
	case qdCondition::CONDITION_MOUSE_OBJECT_ZONE_CLICK:
		return GetMouseObjZoneClickText(pc);
	case qdCondition::CONDITION_OBJECT_IN_ZONE:
		return GetObjInZoneText(pc);
	case qdCondition::CONDITION_PERSONAGE_STATIC_DIRECTION:
		return GetStaticDirText(pc);
	case qdCondition::CONDITION_PERSONAGE_WALK_DIRECTION:
		return GetWalkDirText(pc);
	case qdCondition::CONDITION_TIMER:
		return GetTimerText(pc);
	case qdCondition::CONDITION_MOUSE_DIALOG_CLICK:
		return GetMouseDlgClick(pc);
//	case qdCondition::CONDITION_MINIGAME_STATE:
//		return GetMGInStateText(pc);
	case qdCondition::CONDITION_OBJECT_STATE:
		return GetObjectStateText(pc);
	case qdCondition::CONDITION_OBJECT_STATE_WAS_ACTIVATED:
		return GetObjectWasStateText(pc);
	case qdCondition::CONDITION_OBJECTS_DISTANCE:
		return GetObjectsDistanceText(pc);
	case qdCondition::CONDITION_PERSONAGE_ACTIVE:
		return GetPersonageActiveText(pc);
	case qdCondition::CONDITION_OBJECT_STATE_WAITING:
		return GetObjStateWaitingText(pc);
	case qdCondition::CONDITION_OBJECT_STATE_ANIMATION_PHASE:
		return GetStateAnimPhaseText(pc);
	case qdCondition::CONDITION_OBJECT_PREV_STATE:
		return GetObjectPrevStateText(pc);
	case qdCondition::CONDITION_STATE_TIME_GREATER_THAN_STATE_TIME:
		return GetWTGreaterWTText(pc);
	case qdCondition::CONDITION_STATE_TIME_IN_INTERVAL:
		return GetWTInIntervalText(pc);
	case qdCondition::CONDITION_STATE_TIME_GREATER_THAN_VALUE:
		return GetWTGreaterValueText(pc);
	case qdCondition::CONDITION_COUNTER_GREATER_THAN_COUNTER:
		return GetCounterGreaterCounterText(pc);
	case qdCondition::CONDITION_COUNTER_GREATER_THAN_VALUE:
		return GetCounterGreaterValueText(pc);
	case qdCondition::CONDITION_COUNTER_LESS_THAN_VALUE:
		return GetCounterLessValueText(pc);
	case qdCondition::CONDITION_COUNTER_IN_INTERVAL:
		return GetCounterInIntervalText(pc);
	case qdCondition::CONDITION_OBJECT_ON_PERSONAGE_WAY:
		return GetObjectOnPersonageWayText(pc);
	case qdCondition::CONDITION_KEYPRESS:
		return GetKeypressedText(pc);
	case qdCondition::CONDITION_ANY_PERSONAGE_IN_ZONE:
		return GetAnyPersonageInZoneText(pc);
	case qdCondition::CONDITION_MOUSE_RIGHT_CLICK:
		return GetMouseRightClickText(pc);
	case qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_CLICK:
		return GetMouseRightObjClickText(pc);
	case qdCondition::CONDITION_MOUSE_RIGHT_ZONE_CLICK:
		return GetMouseRightClickZoneText(pc);
	case qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_ZONE_CLICK:
		return GetMouseRightObjZoneClickText(pc);
	case qdCondition::CONDITION_OBJECT_HIDDEN:
		return GetObjectHiddenText(pc);
	case qdCondition::CONDITION_MOUSE_HOVER:
		return GetMouseHoverText(pc);
	case qdCondition::CONDITION_MOUSE_OBJECT_HOVER:
		return GetMouseObjectHoverText(pc);
	case qdCondition::CONDITION_MOUSE_HOVER_ZONE:
		return GetMouseHoverZoneText(pc);
	case qdCondition::CONDITION_MOUSE_OBJECT_HOVER_ZONE:
		return GetMouseObjectHoverZoneText(pc);
	case qdCondition::CONDITION_MOUSE_CLICK_FAILED:
		return GetMouseClickFailedText(pc);
	case qdCondition::CONDITION_MOUSE_OBJECT_CLICK_FAILED:
		return GetMouseObjectClickFailedText(pc);
	case qdCondition::CONDITION_MOUSE_CLICK_EVENT:
		return GetMouseClickEventText(pc);
	case qdCondition::CONDITION_MOUSE_OBJECT_CLICK_EVENT:
		return GetMouseObjectClickEventText(pc);
	case qdCondition::CONDITION_MOUSE_RIGHT_CLICK_EVENT:
		return GetMouseRightClickEventText(pc);
	case qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_CLICK_EVENT:
		return GetMouseRightObjectClickEventText(pc);
	case qdCondition::CONDITION_MOUSE_STATE_PHRASE_CLICK:
		return GetMouseStatePhraseClick(pc);
	case qdCondition::CONDITION_OBJECT_IS_CLOSER:
		return GetObjectIsCloser(pc);
	case qdCondition::CONDITION_ANIMATED_OBJECT_IDLE_GREATER_THAN_VALUE:
		return GetAnimObjIdleGrtThanVal(pc);
	case qdCondition::CONDITION_ANIMATED_OBJECTS_INTERSECTIONAL_BOUNDS:
		return GetAnimObjsIntersBounds(pc);
	}
	return CString();
}

//////////////////////////////////////////////////////////////////////////

Permutation::Permutation(int size)
{
	if (size < 0) size = 0;
	idx_.resize(size);
	for (int i = 0; i < size; i++)
		idx_[i] = i;
}

void Permutation::load(const char* fname)
{
	std::fstream fin;
	fin.open(fname, std::ios::in);
	if (fin.is_open())
	{
		int size;
		fin >> size;
		if (size < 0) size = 0;
		idx_.resize(size);
		for (int i = 0; i < size; i++)
			fin >> idx_[i];
	}
}

void Permutation::save(const char* fname) const
{
	std::fstream fout;
	fout.open(fname, std::ios::out);
	if (fout.is_open())
	{
		fout << idx_.size() << "\n";
		for (int i = 0; i < idx_.size(); i++)
			fout << idx_[i] << "\n";
	}
}

void Permutation::swap(int i, int j)
{
	if (i < 0 || i >= idx_.size() || j < 0 || j >= idx_.size())
		return;
	int tmp = idx_[i];
	idx_[i] = idx_[j];
	idx_[j] = tmp;
}

int Permutation::get_idx(int i) const 
{ 
	if (i < 0) return 0;
	// Если находимся за пределами перестановки, то считаем, что эл-ты расположены по порядку
	else if (i >= idx_.size()) return i; 
	else return idx_[i]; 
}