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
		//! ���� ����� �� �������
	case qdCondition::CONDITION_MOUSE_CLICK:
		//! �������� �������� � ������������ �����������
	case qdCondition::CONDITION_PERSONAGE_WALK_DIRECTION:
		//! �������� ����� � ������������ � ������������ �����������
	case qdCondition::CONDITION_PERSONAGE_STATIC_DIRECTION:
		//! ����-���� ��������� � ������������ ���������
	case qdCondition::CONDITION_MINIGAME_STATE:
		//! ���� ����� �� ���� �� ����� �����
	case qdCondition::CONDITION_MOUSE_ZONE_CLICK:	
		//! �������� �������
	case qdCondition::CONDITION_PERSONAGE_ACTIVE:
		//! ����� ������ ��������� ������ ��������
	case qdCondition::CONDITION_STATE_TIME_GREATER_THAN_VALUE:
		//! ����� ������ ��������� � ���������
	case qdCondition::CONDITION_STATE_TIME_IN_INTERVAL:
		//! ������� �������� ������ �����
	case qdCondition::CONDITION_COUNTER_GREATER_THAN_VALUE:
		//! ������� �������� ������ �����
	case qdCondition::CONDITION_COUNTER_LESS_THAN_VALUE:
		//! ������� �������� � ���������
	case qdCondition::CONDITION_COUNTER_IN_INTERVAL:
		//! � ���� ���� �� ���� ��������
	case qdCondition::CONDITION_ANY_PERSONAGE_IN_ZONE:
		//! ����� ������� ������ ��������� ������
	case qdCondition::CONDITION_ANIMATED_OBJECT_IDLE_GREATER_THAN_VALUE:
		//! ���� �������� �� ����
	case qdCondition::CONDITION_MOUSE_HOVER_ZONE:
		//! ���� �� ����� ������������ ���������
	case qdCondition::CONDITION_MOUSE_STATE_PHRASE_CLICK:
		return test1(const_cast<qdCondition*>(condition), obj);
		//! ���� �������� �� ���� �� �������
	case qdCondition::CONDITION_MOUSE_OBJECT_CLICK:
		//! �������� ��� ������ � ������������ ����� �����
	case qdCondition::CONDITION_OBJECT_IN_ZONE:
		//! ������ ��������� � ������������ ���������
	case qdCondition::CONDITION_OBJECT_STATE:
		//! ������ �� ��������� � ������������ ���������
	case qdCondition::CONDITION_OBJECT_NOT_IN_STATE:
		//! ���� �������� �� ���� �� ����� �����
	case qdCondition::CONDITION_MOUSE_OBJECT_ZONE_CLICK:
		//! ��������� ������� ��������������
	case qdCondition::CONDITION_OBJECT_STATE_WAS_ACTIVATED:
		//! ��������� ������� �� ��������������
	case qdCondition::CONDITION_OBJECT_STATE_WAS_NOT_ACTIVATED:
		//! ���������� �� ����� ����� ����� ��������� ������ ���������
	case qdCondition::CONDITION_OBJECTS_DISTANCE:
		//! ��������� ������� ���� ���������
	case qdCondition::CONDITION_OBJECT_STATE_WAITING:
		//! ��������� � ������������ ���� ��������
	case qdCondition::CONDITION_OBJECT_STATE_ANIMATION_PHASE:
		//! ���������� ��������� �������
	case qdCondition::CONDITION_OBJECT_PREV_STATE:
		//! ����� ������ ������ ��������� ������ ������� ������ �������
	case qdCondition::CONDITION_STATE_TIME_GREATER_THAN_STATE_TIME:
		//! ������� �������� ������ �������� ������� ��������
	case qdCondition::CONDITION_COUNTER_GREATER_THAN_COUNTER:
		//! ������ �� ���� ���������
	case qdCondition::CONDITION_OBJECT_ON_PERSONAGE_WAY:
		//! ������ ���� �������� ������������
	case qdCondition::CONDITION_ANIMATED_OBJECTS_INTERSECTIONAL_BOUNDS:
		//! ���� �������� �� ������, �� ���� ������
	case qdCondition::CONDITION_MOUSE_OBJECT_HOVER:
		//! ���� �������� �� ����, �� ���� ������
	case qdCondition::CONDITION_MOUSE_OBJECT_HOVER_ZONE:
		//! ��� ���� ������ ������� �������� �� ����
	case qdCondition::CONDITION_MOUSE_RIGHT_OBJECT_CLICK_EVENT:

		return test2(const_cast<qdCondition*>(condition), obj);
	//! ���������� �� ������ ������� ������ ��� �� �������
	case qdCondition::CONDITION_OBJECT_IS_CLOSER:
		return test3(const_cast<qdCondition*>(condition), obj);
	default:
		return false;
	}
}

//! ��������� ������ OBJECT_NAME
bool IsObjectInConditionTest::test1(qdCondition* condition, 
									qdNamedObject const* obj)
{
	return (condition->get_object(qdCondition::OBJECT_NAME) == obj);
}
//! ��������� ������ OBJECT2_NAME
bool IsObjectInConditionTest::test2(qdCondition* condition, 
									qdNamedObject const* obj)
{
	return (condition->get_object(qdCondition::OBJECT_NAME)== obj
			||condition->get_object(qdCondition::OBJECT2_NAME)== obj);
}

//! ��������� ������ OBJECT2_NAME
bool IsObjectInConditionTest::test3(qdCondition* condition, 
									qdNamedObject const* obj)
{
	return (condition->get_object(qdCondition::OBJECT_NAME)== obj
		||condition->get_object(qdCondition::OBJECT2_NAME)== obj
		||condition->get_object(2)== obj);
}
