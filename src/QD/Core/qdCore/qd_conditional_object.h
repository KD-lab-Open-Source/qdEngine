#ifndef __CONDITIONAL_OBJECT_H__
#define __CONDITIONAL_OBJECT_H__

#include "xml_fwd.h"

#include "qd_condition.h"
#include "qd_condition_group.h"
#include "qd_named_object.h"

//! ������ � ���������.
class qdConditionalObject : public qdNamedObject
{
public:
	qdConditionalObject();
	qdConditionalObject(const qdConditionalObject& obj);
	~qdConditionalObject();

	qdConditionalObject& operator = (const qdConditionalObject& obj);

	//! ����� �������� �������.
	enum ConditionsMode {
		//! "�" - ������ ����������� ��� �������.
		CONDITIONS_AND,
		//! "���" - ���������� ���������� ������ �� �������.
		CONDITIONS_OR
	};

	typedef std::vector<qdCondition> conditions_container_t;
	typedef std::vector<qdConditionGroup> condition_groups_container_t;

	//! ���������� true, ���� ������� �����������.
	virtual bool check_conditions();
	//! ��������� �������. ���������� ���������� �����,-1 � ������ ������.
	int add_condition(const qdCondition* p);
	//! ��������� ������� ������� � ���������� ������� num.
	bool update_condition(int num,const qdCondition& p);

	//! ������� �������.
	bool remove_conditon(int idx);

#ifdef _QUEST_EDITOR
	//! ������� ��� �������
	void remove_all_conditions();
	//! ������� ��� ������
	void remove_all_groups();
#endif //_QUEST_EDITOR

	//! ���������� ��������� �� �������.
	const qdCondition* get_condition(int idx = 0) const { return &*(conditions_.begin() + idx); }

	//! ���������� ���������� �������.
	int conditions_count() const { return conditions_.size(); }

	//! ��������� ������ �������. ���������� ���������� �����,-1 � ������ ������.
	int add_condition_group(const qdConditionGroup* p);
	//! ��������� ������ ������� � ���������� ������� num.
	bool update_condition_group(int num,const qdConditionGroup& p);

	//! ������� ������ �������.
	bool remove_conditon_group(int idx);

	//! ���������� ��������� �� ������ �������.
	const qdConditionGroup* get_condition_group(int idx = 0) const { return &*(condition_groups_.begin() + idx); }

	//! ���������� ���������� ����� �������.
	int condition_groups_count() const { return condition_groups_.size(); }

	//! ���������� ����� �������� �������.
	ConditionsMode conditions_mode() const { return conditions_mode_; }
	//! ������������� ����� �������� �������.
	void set_conditions_mode(ConditionsMode m){ conditions_mode_ = m; }

	//! ���������� true, ���� ������ ������� �� ������.
	bool has_conditions() const { return !conditions_.empty(); }

	//! ������ ������ �������, dt - ����� � ��������.
	void conditions_quant(float dt);

#ifdef _QUEST_EDITOR
	bool backup_object();
	bool restore_object();
#endif

	//! ������������� �������, ���������� ��� ������ � ����������� ����.
	virtual bool init();

	//! �������� ������ �� �����.
	bool load_data(qdSaveStream& fh,int save_version);
	//! ������ ������ � ����.
	bool save_data(qdSaveStream& fh) const;

#ifdef _QUEST_EDITOR
	bool init_objects();
	static void toggle_global_list(bool state){ 
		enable_objects_list_ = state; 
		if(!state)
			objects_list_.clear();
	}
	static void global_init();
#endif

	bool is_condition_in_group(int condition_idx) const;

	//! �������� ������ ��������.
	enum trigger_start_mode {
		//! ������ �� ������
		TRIGGER_START_FAILED,
		//! �������� �������� ����� � ��������
		TRIGGER_START_ACTIVATE,
		//! �������� ������ ����� � ��������
		TRIGGER_START_WAIT
	};
	//! ��������� ��������.
	virtual trigger_start_mode trigger_start() = 0;

	//! ���������� true, ���� ������� ����� ���� ����������� ��� ���������� ��� ���������� �������.
	/**
	������ ������ ������������ �������� ����� (������� �������, ���� �� �����, ������)
	��� �� �������� �� ��� (�����, ����-���� � �.�.).
	*/
	virtual bool trigger_can_start() const;

protected:

	//! �������� ������ �� �������.
	bool load_conditions_script(const xml::tag* p);
	//! ������ ������ � ������.
	bool save_conditions_script(XStream& fh,int indent = 0) const;

private:

	//! ������ �������� ������� - �/���.
	ConditionsMode conditions_mode_;

	//! �������.
	conditions_container_t conditions_;

	//! ������ �������.
	condition_groups_container_t condition_groups_;

#ifdef _QUEST_EDITOR
	static conditions_container_t backup_;
	static condition_groups_container_t groups_backup_;
	static ConditionsMode backup_mode_;

	//! ������ ���� �������� (��� ������������� � ���������).
	static std::list<qdConditionalObject*> objects_list_;
	static bool enable_objects_list_;
#endif

	bool check_group_conditions(const qdConditionGroup& gr);
};

#endif /* __CONDITIONAL_OBJECT_H__ */