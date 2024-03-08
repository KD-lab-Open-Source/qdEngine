
#ifndef __QD_INTERFACE_BUTTON_H__
#define __QD_INTERFACE_BUTTON_H__

#include "qd_interface_element.h"
#include "qd_interface_element_state.h"

#ifdef _QUEST_EDITOR
#define _QD_INTERFACE_BUTTON_PTR_CONTAINER
#endif

//! ������������ ������� - ������.
class qdInterfaceButton : public qdInterfaceElement
{
public:
	qdInterfaceButton();
	qdInterfaceButton(const qdInterfaceButton& bt);
	~qdInterfaceButton();

	qdInterfaceButton& operator = (const qdInterfaceButton& bt);

	//! ���������� ��� ��������.
	qdInterfaceElement::element_type get_element_type() const { return qdInterfaceElement::EL_BUTTON; }

	//! ���������� ���������� ��������� ������.
	int num_states() const { return states_.size(); }

	//! �������� ��������� ����� state_num.
	bool activate_state(int state_num);
	//! �������� ��������� � ������ state_name.
	bool activate_state(const char* state_name);

	int option_value() const { return cur_state_; }
	bool set_option_value(int value);

	//! ���������� ��������� �� ��������� ������.
	const qdInterfaceElementState* get_state(int state_num) const {
		assert(state_num >= 0 && state_num < states_.size());
#ifndef _QD_INTERFACE_BUTTON_PTR_CONTAINER
		return &states_[state_num];
#else
		return &*states_[state_num];
#endif
	}
	//! ���������� ��������� �� ��������� ������.
	qdInterfaceElementState* get_state(int state_num){
		assert(state_num >= 0 && state_num < states_.size());
#ifndef _QD_INTERFACE_BUTTON_PTR_CONTAINER
		return &states_[state_num];
#else
		return &*states_[state_num];
#endif
	}

	//! ��������� ��������� ������.
	/**
	��������� �� ��������� st ���������� �� ���������� ������.
	*/
	bool add_state(const qdInterfaceElementState& st);

	//! ��������� ��������� ������.
	/**
	����� ��������� ����������� � ������ ����� ���������� ����� insert_before.
	��������� �� ��������� st ���������� �� ���������� ������.
	*/
	bool insert_state(int insert_before,const qdInterfaceElementState& st);

	//! ������� ��������� ����� state_num.
	bool erase_state(int state_num);

	//! ������������ ��������� ������.
	/** 
	���������������� ��������� ������������� �������� st.
	*/
	bool modify_state(int state_num,const qdInterfaceElementState& st){
		assert(state_num >= 0 && state_num < states_.size());
#ifdef _QD_INTERFACE_BUTTON_PTR_CONTAINER
		*states_[state_num] = st;
		states_[state_num]->set_owner(this);
#else
		states_[state_num] = st;
		states_[state_num].set_owner(this);
#endif
		return true;
	}

	//! ����������� ��������� ������.
	/**
	���� direction == true ���������� ��������� ���������, ���� false - ����������.
	*/
	bool change_state(bool direction = true);

	//! ���������� ������� ����.
	bool mouse_handler(int x,int y,mouseDispatcher::mouseEvent ev);
	//! ���������� ����� � ����������.
	bool keyboard_handler(int vkey);

	//! ������������� ��������. 
	/**
	���������� ������ ��� ��� ������ �� �����, �������� ����������� �������.
	*/
	bool init(bool is_game_active = true);

	//! ������ ������, �������� - ����� � ��������.
	bool quant(float dt);

	//! ���������� true, ���� ����� � ��������� ������������ (x,�) �������� � �������.
	bool hit_test(int x,int y) const;

	const qdInterfaceEvent* find_event(qdInterfaceEvent::event_t type) const;
	bool has_event(qdInterfaceEvent::event_t type,const char* ev_data = NULL) const;

protected:

	//! ������ ������ � ������.
	bool save_script_body(XStream& fh,int indent = 0) const;
	//! �������� ������ �� �������.
	bool load_script_body(const xml::tag* p);

private:

#ifdef _QD_INTERFACE_BUTTON_PTR_CONTAINER
	typedef std::vector<qdInterfaceElementState*> state_container_t;
#else
	typedef std::vector<qdInterfaceElementState> state_container_t;
#endif

	//! ��������� ������.
	state_container_t states_;

	//! ������� ��������� ������.
	int cur_state_;

};

#endif /* __QD_INTERFACE_BUTTON_H__ */
