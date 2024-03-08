#include "stdafx.h"
#include ".\isobjectinconditiontest.h"
#include "qd_condition.h"

IsObjectInConditionTest::IsObjectInConditionTest(qdNamedObject const* obj):
  obj_(obj)
{
}

IsObjectInConditionTest::~IsObjectInConditionTest()
{
}

bool IsObjectInConditionTest::operator()(qdCondition const* cond) const
{
	return run(cond, obj_);
}

bool IsObjectInConditionTest::run(qdCondition const* cond, qdNamedObject const* obj)
{
	return generalTest(cond, obj);
}

bool IsObjectInConditionTest::generalTest(qdCondition const* condition, 
										  qdNamedObject const* obj)
{
	switch(condition->type()) {
		//! клик мышью по объекту
	case qdCondition::CONDITION_MOUSE_CLICK:
		//! персонаж движется в определенном направлении
	case qdCondition::CONDITION_PERSONAGE_WALK_DIRECTION:
		//! персонаж стоит и ориентирован в определенном направлении
	case qdCondition::CONDITION_PERSONAGE_STATIC_DIRECTION:
		//! мини-игра находится в определенном состоянии
	case qdCondition::CONDITION_MINIGAME_STATE:
		//! клик мышью по зоне на сетке сцены
	case qdCondition::CONDITION_MOUSE_ZONE_CLICK:	
		//! персонаж активен
	case qdCondition::CONDITION_PERSONAGE_ACTIVE:
		//! время работы состояния больше значения
	case qdCondition::CONDITION_STATE_TIME_GREATER_THAN_VALUE:
		//! время работы состояния в интервале
	case qdCondition::CONDITION_STATE_TIME_IN_INTERVAL:
		//! значние счетчика больше числа
	case qdCondition::CONDITION_COUNTER_GREATER_THAN_VALUE:
		//! значние счетчика меньше числа
	case qdCondition::CONDITION_COUNTER_LESS_THAN_VALUE:
		//! значние счетчика в интервале
	case qdCondition::CONDITION_COUNTER_IN_INTERVAL:
		//! в зоне хотя бы один персонаж
	case qdCondition::CONDITION_ANY_PERSONAGE_IN_ZONE:
		//! время простоя больше заданного порога
	case qdCondition::CONDITION_ANIMATED_OBJECT_IDLE_GREATER_THAN_VALUE:
		//! мышь наведена на зону
	case qdCondition::CONDITION_MOUSE_HOVER_ZONE:
		//! клик по фразе определённого состояния
	case qdCondition::CONDITION_MOUSE_STATE_PHRASE_CLICK:
		return test1(const_cast<qdCondition*>(condition), obj);
		//! клик объектом на мыши по объекту
	case qdCondition::CONDITION_MOUSE_OBJECT_CLICK:
		//! персонаж или объект в определенном месте сетки
	case qdCondition::CONDITION_OBJECT_IN_ZONE:
		//! объект находится в определенном состоянии
	case qdCondition::CONDITION_OBJECT_STATE:
		//! объект не находится в определенном состоянии
	case qdCondition::CONDITION_OBJECT_NOT_IN_STATE:
		//! клик объектом по зоне на сетке сцены
	case qdCondition::CONDITION_MOUSE_OBJECT_ZONE_CLICK:
		//! состояние объекта активировалось
	case qdCondition::CONDITION_OBJECT_STATE_WAS_ACTIVATED:
		//! состояние объекта не активировалось
	case qdCondition::CONDITION_OBJECT_STATE_WAS_NOT_ACTIVATED:
		//! расстояние по сетке между двумя объектами меньше заданного
	case qdCondition::CONDITION_OBJECTS_DISTANCE:
		//! состояние объекта ждет активации
	case qdCondition::CONDITION_OBJECT_STATE_WAITING:
		//! состояние в определенной фазе анимации
	case qdCondition::CONDITION_OBJECT_STATE_ANIMATION_PHASE:
		//! предыдущее состояние объекта
	case qdCondition::CONDITION_OBJECT_PREV_STATE:
		//! время работы одного состояния больше времени работы другого
	case qdCondition::CONDITION_STATE_TIME_GREATER_THAN_STATE_TIME:
		//! значние счетчика больше значения другого счетчика
	case qdCondition::CONDITION_COUNTER_GREATER_THAN_COUNTER:
		//! объект на пути персонажа
	case qdCondition::CONDITION_OBJECT_ON_PERSONAGE_WAY:
		//! баунды двух объектов пересекаются
	case qdCondition::CONDITION_ANIMATED_OBJECTS_INTERSECTIONAL_BOUNDS:
		//! мышь наведена на объект, на мыши объект
	case qdCondition::CONDITION_MOUSE_OBJECT_HOVER:
		//! мышь наведена на зону, на мыши объект
	case qdCondition::CONDITION_MOUSE_OBJECT_HOVER_ZONE:
		//! был клик правой кнопкой объектом на мыши
	case qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_CLICK_EVENT:

		return test2(const_cast<qdCondition*>(condition), obj);
	//! расстояние до одного объекта меньше чем до другого
	case qdCondition::CONDITION_OBJECT_IS_CLOSER:
		return test3(const_cast<qdCondition*>(condition), obj);
	default:
		return false;
	}
}

//! проверяет только OBJECT_NAME
bool IsObjectInConditionTest::test1(qdCondition* condition, 
									qdNamedObject const* obj)
{
	return (condition->get_object(qdCondition::OBJECT_NAME) == obj);
}
//! проверяет только OBJECT2_NAME
bool IsObjectInConditionTest::test2(qdCondition* condition, 
									qdNamedObject const* obj)
{
	return (condition->get_object(qdCondition::OBJECT_NAME)== obj
			||condition->get_object(qdCondition::OBJECT2_NAME)== obj);
}

//! проверяет только OBJECT2_NAME
bool IsObjectInConditionTest::test3(qdCondition* condition, 
									qdNamedObject const* obj)
{
	return (condition->get_object(qdCondition::OBJECT_NAME)== obj
		||condition->get_object(qdCondition::OBJECT2_NAME)== obj
		||condition->get_object(2)== obj);
}
