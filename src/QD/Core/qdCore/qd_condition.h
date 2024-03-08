#ifndef __QD_CONDITION_H__
#define __QD_CONDITION_H__

#include "xml_fwd.h"

#include "qd_named_object.h"
#include "qd_condition_data.h"
#include "qd_condition_object_reference.h"

//! �������.
/**
��������� ������ �� ����� �������.

������� ��� ������� � �������� (���� ��� ������������ � �������)
��������� � ��������� ��� ������� � ������ �� ������ ��������.

CONDITION_TRUE
CONDITION_FALSE
������: �����������

CONDITION_MOUSE_CLICK
������:
	string
�������:
	qdGameObject
������� ������:
	OBJECT_NAME
	��� �������, �� �������� ���� ��������.
	���� ������ - ����������� ���� �� �������, �������� �����������
	���������, � ������� ���������� ��� �������.

CONDITION_MOUSE_OBJECT_CLICK
������:
	string, string
�������:
	qdGameObject, qdGameObject
������� ������:
	OBJECT_NAME, MOUSE_OBJECT_NAME
	��� �������, �� �������� ���� ��������
	� ��� �������, ������� ������ ���� �� ����.

CONDITION_OBJECT_IN_ZONE
������:
	string, string
�������:
	qdGameObject, qdGridZone
������� ������:
	OBJECT_NAME, ZONE_NAME
	��� ������� � ��� ���� �� �����,
	� ������� �� ������ �������.

CONDITION_PERSONAGE_WALK_DIRECTION
CONDITION_PERSONAGE_STATIC_DIRECTION
������:
	string, float[1]
�������:
	qdGameObjectMoving
������� ������:
	PERSONAGE_NAME, DIRECTION_ANGLE
	��� ��������� � ���� ����������� � ��������.
	���� �������� ��� ��, ��� � ����������� � ��������
	(�.�. �� ����������� ������� ������ ������� �������)

CONDITION_TIMER
������:
	float[2], int[2]
������� ������:
	TIMER_PERIOD, TIMER_RND
	������ � ������� ����� ������� � ��������,
	����������� ���������� ������� �� ��������� �������
	������� � ��������� � ������� ��������� �������.

CONDITION_MOUSE_DIALOG_CLICK
������: �����������

CONDITION_MINIGAME_STATE
������:
	string, string
�������:
	qdMiniGame
������� ������:
	MINIGAME_NAME, MINIGAME_STATE_NAME
	��� ����-���� � ��� ���������, � ������� ��� ������ ����������
	��� ���������� �������.

CONDITION_OBJECT_STATE
������:
	string, string
�������:
	qdGameObjectAnimated, qdGameObjectState
������� ������:
	OBJECT_NAME, OBJECT_STATE_NAME
	��� ������� � ��� ���������, � ������� �� ������ ����������
	��� ���������� �������.

CONDITION_MOUSE_ZONE_CLICK
������:
	string
�������:
	qdGridZone
������� ������:
	CLICK_ZONE_NAME
	��� ���� �� �����,
	� ������� ���� ��������.

CONDITION_MOUSE_OBJECT_ZONE_CLICK
������:
	string, string
�������:
	qdGridZone, qdGameObject
������� ������:
	CLICK_ZONE_NAME, MOUSE_OBJECT_NAME
	��� ���� �� �����, � ������� ���� ��������
	� ��� �������, ������� ������ ���� �� ����.

CONDITION_OBJECT_STATE_WAS_ACTIVATED
CONDITION_OBJECT_STATE_WAS_NOT_ACTIVATED
������:
	string, string
�������:
	qdGameObjectAnimated, qdGameObjectState
������� ������:
	OBJECT_NAME, OBJECT_STATE_NAME
	��� ������� � ��� ���������.

CONDITION_OBJECT_NOT_IN_STATE
��. CONDITION_OBJECT_STATE

CONDITION_OBJECTS_DISTANCE
������:
	string, string, float
�������:
	qdGameObject, qdGameObject
������� ������:
	OBJECT_NAME, OBJECT2_NAME, OBJECTS_DISTANCE
	����� �������� � ���������� �� �����.

CONDITION_PERSONAGE_ACTIVE
������:
	string
�������:
	qdGameObjectMoving
������� ������:
	PERSONAGE_NAME
	��� ���������, ������� ������ ���� ��������
	��� ���������� �������.

CONDITION_OBJECT_STATE_WAITING
������:
	string, string
�������:
	qdGameObjectAnimated, qdGameObjectState
������� ������:
	OBJECT_NAME, OBJECT_STATE_NAME
	��� ������� � ��� ���������.

CONDITION_OBJECT_STATE_ANIMATION_PHASE
������:
	string, string, float[2]
�������:
	qdGameObjectAnimated, qdGameObjectState
������� ������:
	OBJECT_NAME, OBJECT_STATE_NAME, ANIMATION_PHASE
	��� �������, ��� ��������� � �������� ���� ��������.

CONDITION_OBJECT_PREV_STATE
������:
	string, string
�������:
	qdGameObjectAnimated, qdGameObjectState
������� ������:
	OBJECT_NAME, OBJECT_STATE_NAME
	��� ������� � ��� ���������.

CONDITION_STATE_TIME_GREATER_THAN_VALUE
������:
	float
	����� ������ ���������.
�������:
	qdGameObjectState

CONDITION_STATE_TIME_GREATER_THAN_STATE_TIME
������: �����������
�������:
	qdGameObjectState, qdGameObjectState
	���������, ����� ������ ������� ������������.

CONDITION_STATE_TIME_IN_INTERVAL
������:
	float[2]
	��������, � ������� ������ ���������� ���� ������ ���������.
�������:
	qdGameObjectState

CONDITION_COUNTER_GREATER_THAN_VALUE
CONDITION_COUNTER_LESS_THAN_VALUE
������:
	int
	�����, � ������� ������������ �������� ��������.
�������:
	qdCounter

CONDITION_COUNTER_GREATER_THAN_COUNTER
������: �����������
�������:
	qdCounter, qdCounter
	��������, �������� ������� ������������.

CONDITION_COUNTER_IN_INTERVAL
������:
	int[2]
	��������, � ������� ������ ���������� �������� ��������.
�������:
	qdCounter

CONDITION_OBJECT_ON_PERSONAGE_WAY
������:
	float[1]
	������������ ���������� ����� �������� � ���������� �� �����.
�������:
	qdGameObjectMoving, qdGameObject

CONDITION_KEYPRESS
������:
	int[1]
	vkey ������.

CONDITION_ANY_PERSONAGE_IN_ZONE
�������:
	qdGridZone

CONDITION_MOUSE_RIGHT_CLICK
�������:
	qdGameObject
������� ������:
	OBJECT_NAME
	������, �� �������� ���� ��������.
	���� ������� - ����������� ���� �� �������, �������� �����������
	���������, � ������� ���������� ��� �������.

CONDITION_MOUSE_RIGHT_OBJECT_CLICK
�������:
	qdGameObject, qdGameObject
������� ������:
	OBJECT_NAME, MOUSE_OBJECT_NAME
	������, �� �������� ���� ��������
	� ������, ������� ������ ���� �� ����.

CONDITION_MOUSE_RIGHT_ZONE_CLICK
�������:
	qdGridZone
������� ������:
	CLICK_ZONE_NAME
	���� �� �����,
	� ������� ���� ��������.

CONDITION_MOUSE_RIGHT_OBJECT_ZONE_CLICK
�������:
	qdGridZone, qdGameObject
������� ������:
	CLICK_ZONE_NAME, MOUSE_OBJECT_NAME
	���� �� �����, � ������� ���� ��������
	� ������, ������� ������ ���� �� ����.

CONDITION_OBJECT_HIDDEN
�������:
	qdGameObject
	������, ������� ������ ���� �����

CONDITION_MOUSE_HOVER
�������:
	qdGameObject
	������, �� ������� ��������� ����

CONDITION_MOUSE_OBJECT_HOVER
�������:
	qdGameObject, qdGameObject
	������, �� ������� ��������� ����,
	������ ������� �� ����

CONDITION_MOUSE_HOVER_ZONE
�������:
	qdGridZone
	����, �� ������� ��������� ����

CONDITION_MOUSE_OBJECT_HOVER_ZONE
�������:
	qdGridZone, qdGameObject
	����, �� ������� ��������� ����,
	������ ������� �� ����

CONDITION_MOUSE_CLICK_FAILED
CONDITION_MOUSE_OBJECT_CLICK_FAILED
CONDITION_MOUSE_CLICK_EVENT
//CONDITION_MOUSE_OBJECT_CLICK_EVENT
CONDITION_MOUSE_RIGHT_CLICK_EVENT
//CONDITION_MOUSE_RIGHT_OBJECT_CLICK_EVENT
������ ���.

CONDITION_MOUSE_OBJECT_CLICK_EVENT
CONDITION_MOUSE_RIGHT_OBJECT_CLICK_EVENT
�������:
	qdGameObject
	������, ������� ���� �������� ��� ���������� �������.
	���� ������, �� ���������� ����� ����� ��������.

CONDITION_MOUSE_STATE_PHRASE_CLICK
�������:
	qdGameObjecState
	���������, �� ����� �������� ���� ��������.

CONDITION_OBJECT_IS_CLOSER
�������:
	qdGameObject, qdGameObject, qdGameObject
	���������� �� ������� ������� �� ������� ������ ���� ������,
	��� ���������� �� ������� �� ��������.

CONDITION_ANIMATED_OBJECT_IDLE_GREATER_THAN_VALUE
�������:
	qdGameObjectAnimated, int
	������������� ������, ������� �����������, � ����� �������,
	������� ������ ���� �������� �������� �������� ��������� �������.

CONDITION_ANIMATED_OBJECTS_INTERSECTIONAL_BOUNDS
�������:
	qdGameObjectAnimated, qdGameObjectAnimated
	��� �������, ������ ������� ������������ �� �����������.
*/

class qdCondition
{
public:
	//! ���� �������.
	enum ConditionType {
		//! ������ true
		CONDITION_TRUE,
		//! ������ false
		CONDITION_FALSE,

		//! ���� ����� �� �������
		CONDITION_MOUSE_CLICK,
		//! ���� �������� �� ���� �� �������
		CONDITION_MOUSE_OBJECT_CLICK,
		//! �������� ��� ������ � ������������ ����� �����
		CONDITION_OBJECT_IN_ZONE,
		//! �������� �������� � ������������ �����������
		CONDITION_PERSONAGE_WALK_DIRECTION,
		//! �������� ����� � ������������ � ������������ �����������
		CONDITION_PERSONAGE_STATIC_DIRECTION,
		//! ������� ���������� � ��������� ��������������
		CONDITION_TIMER,			
		//! ���� ����� �� ����� � �������
		CONDITION_MOUSE_DIALOG_CLICK,
		//! ����-���� ��������� � ������������ ���������
		CONDITION_MINIGAME_STATE,
		//! ������ ��������� � ������������ ���������
		CONDITION_OBJECT_STATE,
		//! ���� ����� �� ���� �� ����� �����
		CONDITION_MOUSE_ZONE_CLICK,
		//! ���� �������� �� ���� �� ����� �����
		CONDITION_MOUSE_OBJECT_ZONE_CLICK,
		//! ��������� ������� ��������������
		CONDITION_OBJECT_STATE_WAS_ACTIVATED,
		//! ��������� ������� �� ��������������
		CONDITION_OBJECT_STATE_WAS_NOT_ACTIVATED,
		//! ������ �� ��������� � ������������ ���������
		CONDITION_OBJECT_NOT_IN_STATE,
		//! ���������� �� ����� ����� ����� ��������� ������ ���������
		CONDITION_OBJECTS_DISTANCE,
		//! �������� �������
		CONDITION_PERSONAGE_ACTIVE,
		//! ��������� ������� ���� ���������
		CONDITION_OBJECT_STATE_WAITING,
		//! ��������� � ������������ ���� ��������
		CONDITION_OBJECT_STATE_ANIMATION_PHASE,
		//! ���������� ��������� �������
		CONDITION_OBJECT_PREV_STATE,
		//! ����� ������ ��������� ������ ��������
		CONDITION_STATE_TIME_GREATER_THAN_VALUE,
		//! ����� ������ ������ ��������� ������ ������� ������ �������
		CONDITION_STATE_TIME_GREATER_THAN_STATE_TIME,
		//! ����� ������ ��������� � ���������
		CONDITION_STATE_TIME_IN_INTERVAL,
		//! ������� �������� ������ �����
		CONDITION_COUNTER_GREATER_THAN_VALUE,
		//! ������� �������� ������ �����
		CONDITION_COUNTER_LESS_THAN_VALUE,
		//! ������� �������� ������ �������� ������� ��������
		CONDITION_COUNTER_GREATER_THAN_COUNTER,
		//! ������� �������� � ���������
		CONDITION_COUNTER_IN_INTERVAL,
		//! ������ �� ���� ���������
		CONDITION_OBJECT_ON_PERSONAGE_WAY,
		//! ������ ������
		CONDITION_KEYPRESS,
		//! � ���� ���� �� ���� ��������
		CONDITION_ANY_PERSONAGE_IN_ZONE,
		//! ������ ���� ���� �� �������
		CONDITION_MOUSE_RIGHT_CLICK,
		//! ������ ���� �������� �� ���� �� �������
		CONDITION_MOUSE_RIGHT_OBJECT_CLICK,
		//! ������ ���� ����� �� ���� �� ����� �����
		CONDITION_MOUSE_RIGHT_ZONE_CLICK,
		//! ������ ���� �������� �� ���� �� ����� �����
		CONDITION_MOUSE_RIGHT_OBJECT_ZONE_CLICK,
		//! ������ �����
		CONDITION_OBJECT_HIDDEN,
		//! ���� �������� �� ������
		CONDITION_MOUSE_HOVER,
		//! ���� �������� �� ������, �� ���� ������
		CONDITION_MOUSE_OBJECT_HOVER,
		//! ���� �������� �� ����
		CONDITION_MOUSE_HOVER_ZONE,
		//! ���� �������� �� ����, �� ���� ������
		CONDITION_MOUSE_OBJECT_HOVER_ZONE,
		//! ��������� ���� ����
		CONDITION_MOUSE_CLICK_FAILED,
		//! ��������� ���� �������� �� ����
		CONDITION_MOUSE_OBJECT_CLICK_FAILED,
		//! ��� ���� ����
		CONDITION_MOUSE_CLICK_EVENT,
		//! ��� ���� �������� �� ����
		CONDITION_MOUSE_OBJECT_CLICK_EVENT,
		//! ��� ���� ������ ������� ����
		CONDITION_MOUSE_RIGHT_CLICK_EVENT,
		//! ��� ���� ������ ������� �������� �� ����
		CONDITION_MOUSE_RIGHT_OBJECT_CLICK_EVENT,
		//! ���� �� ����� ������������ ���������
		CONDITION_MOUSE_STATE_PHRASE_CLICK,
		//! ���������� �� ������ ������� ������ ��� �� �������
		CONDITION_OBJECT_IS_CLOSER,
		//! ����� ������� ������ ��������� ������
		CONDITION_ANIMATED_OBJECT_IDLE_GREATER_THAN_VALUE,
		//! ������ ���� �������� ������������
		CONDITION_ANIMATED_OBJECTS_INTERSECTIONAL_BOUNDS,
	};

	//! ������� ��� ��������� � ������ � ��������.
	enum ConditionDataIndex {
		OBJECT_NAME = 0,
		MOUSE_OBJECT_NAME = 1,
		ZONE_NAME = 1,
		DIRECTION_ANGLE = 1,
		PERSONAGE_NAME = 0,
		TIMER_PERIOD = 0,
		TIMER_RND = 1,
		MINIGAME_NAME = 0,
		MINIGAME_STATE_NAME = 1,
		OBJECT_STATE_NAME = 1,
		CLICK_ZONE_NAME = 0,
		OBJECT2_NAME = 1,
		OBJECTS_DISTANCE = 2,
		ANIMATION_PHASE = 2,
		STATE_TIME = 2
	};

	qdCondition();
	qdCondition(ConditionType tp);
	qdCondition(const qdCondition& cnd);
	~qdCondition();

	qdCondition& operator = (const qdCondition& cnd);

	ConditionType type() const { return type_; }
	void set_type(ConditionType tp);

	bool put_value(int idx,const char* str);

	bool is_click_condition() const { 
		return (type_ == CONDITION_MOUSE_CLICK || type_ == CONDITION_MOUSE_DIALOG_CLICK || type_ == CONDITION_MOUSE_ZONE_CLICK);
	}
	bool is_object_click_condition() const {
		return (type_ == CONDITION_MOUSE_OBJECT_CLICK || type_ == CONDITION_MOUSE_OBJECT_ZONE_CLICK);
	}

	// val_index - ������, �� �������� ���������� �������� 
	// � qdConditionDataInt::data_ ��� qdConditionDataFloat::data_
	bool put_value(int idx,int val,int val_index = 0);
	bool put_value(int idx,float val,int val_index = 0);

	bool get_value(int idx,const char*& str) const;
	bool get_value(int idx,int& val,int val_index = 0) const;
	bool get_value(int idx,float& val,int val_index = 0) const;

	bool put_object(int idx,const qdNamedObject* obj);
	const qdNamedObject* get_object(int idx) ;

	const qdNamedObject* owner() const { return owner_; }
	void set_owner(const qdNamedObject* p){ owner_ = p; }

	bool load_script(const xml::tag* p);
	bool save_script(XStream& fh,int indent = 0) const;

	void quant(float dt);

	//! �������� ������ �� �����.
	bool load_data(qdSaveStream& fh,int save_version);
	//! ������ ������ � ����.
	bool save_data(qdSaveStream& fh) const;

	//! ������������� �������, ���������� ��� ������ � ����������� ����.
	bool init();

	bool is_inversed() const { return is_inversed_; }
	void inverse(bool inverse_mode = true){ is_inversed_ = inverse_mode; }

	bool check();

#ifndef _QUEST_EDITOR
	bool is_in_group() const { return is_in_group_; }
	void add_group_reference(){ is_in_group_ = true; }
	void remove_group_reference(){ is_in_group_ = false; }

	static bool successful_click(){ return successful_click_; }
	static bool successful_object_click(){ return successful_object_click_; }
	static void clear_successful_clicks(){ successful_click_ = successful_object_click_ = false; }
#else
	bool init_objects();
#endif

private:

	ConditionType type_;
	const qdNamedObject* owner_;

	typedef std::vector<qdConditionData> data_container_t;
	data_container_t data_;

	typedef std::vector<qdConditionObjectReference> objects_container_t;
	objects_container_t objects_;

	bool is_inversed_;

#ifndef _QUEST_EDITOR
	bool is_in_group_;

	static bool successful_click_;
	static bool successful_object_click_;
#endif

	bool init_data(int data_index,qdConditionData::data_t data_type,int data_size = 0){
		assert(data_index >= 0 && data_index < data_.size());

		data_[data_index].set_type(data_type);
		if(data_size)
			data_[data_index].alloc_data(data_size);

		return true;
	}
};

#endif /* __QD_CONDITION_H__ */
