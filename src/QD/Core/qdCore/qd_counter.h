#ifndef __QD_COUNTER_H__
#define __QD_COUNTER_H__

#include <vector>

#include "xml_fwd.h"

#include "qd_named_object_reference.h"

class qdGameObjectState;

class qdCounterElement
{
public:
	qdCounterElement();
	~qdCounterElement();
	explicit qdCounterElement(const qdGameObjectState* p,bool inc_value = true);

	bool operator == (const qdGameObjectState* p) const{ return state_ == p; }

	const qdGameObjectState* state() const { return state_; }
	bool increment_value() const { return increment_value_; }

	bool init();
	bool quant();

	bool load_script(const xml::tag* p);
	bool save_script(XStream& fh,int indent = 0) const;

	//! �������� ������ �� �����.
	bool load_data(qdSaveStream& fh,int save_version);
	//! ������ ������ � ����.
	bool save_data(qdSaveStream& fh) const;
	
private:

	qdNamedObjectReference state_reference_;
	const qdGameObjectState* state_;
	bool last_state_status_;
	bool increment_value_;
};

//! ������� ���������.
class qdCounter : public qdNamedObject
{
public:
	qdCounter();
	~qdCounter();

	//! ����� ��������.
	enum {
		//! ��������� ������ ������������� ��������
		POSITIVE_VALUE			= 0x01
	};

	int named_object_type() const { return QD_NAMED_OBJECT_COUNTER; }

	int value() const { return value_; }
	void set_value(int value);
	void add_value(int value_delta);

	int value_limit() const { return value_limit_; }
	void set_value_limit(int limit){ value_limit_ = limit; }

	typedef std::vector<qdCounterElement> element_container_t;
	const element_container_t& elements() const { return elements_; }

	//! ���������� ���������. ���������� false, ���� ��� ��� ��������� �����.
	bool add_element(const qdGameObjectState* p,bool inc_value = true);

	bool remove_element(const qdGameObjectState* p);
	bool remove_element(int idx);
#ifdef _QUEST_EDITOR
	void remove_all_elements();
#endif // _QUEST_EDITOR

	void quant();

	void init();

	bool load_script(const xml::tag* p);
	bool save_script(XStream& fh,int indent = 0) const;

	//! �������� ������ �� �����.
	bool load_data(qdSaveStream& fh,int save_version);
	//! ������ ������ � ����.
	bool save_data(qdSaveStream& fh) const;

protected:
private:

	//! ���������, ������� ��� ��������� �������� �������� ��������.
	element_container_t elements_;

	//! ������� �������� ��������.
	int value_;
	//! ���������� ��������, �� ���������� �������� ������� ����������� � ����.
	/**
	���� ������ ��� ����� ���� - �� �����������.
	*/
	int value_limit_;
};

#endif // __QD_COUNTER_H__
