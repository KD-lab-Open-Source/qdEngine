
#ifndef __QD_INTERFACE_BACKGROUND_H__
#define __QD_INTERFACE_BACKGROUND_H__

#include "qd_interface_element.h"
#include "qd_interface_element_state.h"

//! ������������ ������� - ���.
class qdInterfaceBackground : public qdInterfaceElement
{
public:
	qdInterfaceBackground();
	qdInterfaceBackground(const qdInterfaceBackground& bk);
	~qdInterfaceBackground();

	qdInterfaceBackground& operator = (const qdInterfaceBackground& bk);

	//! ���������� ��� ��������.
	qdInterfaceElement::element_type get_element_type() const { return qdInterfaceElement::EL_BACKGROUND; }

	//! ���������� ������� ����.
	bool mouse_handler(int x,int y,mouseDispatcher::mouseEvent ev);
	//! ���������� ����� � ����������.
	bool keyboard_handler(int vkey);

	//! ������������� ��������. 
	/**
	���������� ������ ��� ��� ������ �� �����, �������� ����������� �������.
	*/
	bool init(bool is_game_active = true);

	//! ������������� ��� ����� ��� ��������.
	/**
	���� ���� ������ �������� - �������� NULL � �������� ����� �����.
	*/
	void set_animation_file(const char* name){ state_.set_animation_file(name); }
	//! ���������� ��� ����� ��� ��������.
	const char* animation_file() const { return state_.animation_file(); }
	//! ���������� ����� ��������.
	int animation_flags() const { return state_.animation_flags(); }

	//! ������������� ���� ��������.
	void set_animation_flag(int fl){ state_.set_animation_flag(fl); }
	//! ��������� ���� ��������.
	void drop_animation_flag(int fl){ state_.drop_animation_flag(fl); }
	//! ���������� true, ���� ��� �������� ���������� ���� fl.
	bool check_animation_flag(int fl) const { return state_.check_animation_flag(fl); }

	//! ���������� true, ���� � ���� ��������� ��������.
	bool has_animation() const { return state_.has_animation(); }

protected:

	//! ������ ������ � ������.
	bool save_script_body(XStream& fh,int indent = 0) const;
	//! �������� ������ �� �������.
	bool load_script_body(const xml::tag* p);

private:

	//! ���������, � ������� �������� ��� ����������� ���������.
	qdInterfaceElementState state_;
};

#endif /* __QD_INTERFACE_BACKGROUND_H__ */
