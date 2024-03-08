
#ifndef __QD_INTERFACE_SAVE_H__
#define __QD_INTERFACE_SAVE_H__

#include "xml_fwd.h"

#include "qd_interface_element.h"
#include "qd_interface_element_state.h"

//! ������������ ������� - ����.
class qdInterfaceSave : public qdInterfaceElement
{
public:
	qdInterfaceSave();
	qdInterfaceSave(const qdInterfaceSave& sv);
	~qdInterfaceSave();

	qdInterfaceSave& operator = (const qdInterfaceSave& sv);

	//! ���������� ��� ��������.
	qdInterfaceElement::element_type get_element_type() const { return qdInterfaceElement::EL_SAVE; }

	//! ���������� ������� ����.
	bool mouse_handler(int x,int y,mouseDispatcher::mouseEvent ev);
	//! ���������� ����� � ����������.
	bool keyboard_handler(int vkey);

	//! ������������� ��������. 
	/**
	���������� ������ ��� ��� ������ �� �����, �������� ����������� �������.
	*/
	bool init(bool is_game_active = true);

	//! ��������� ��������.
	bool redraw() const;

	grScreenRegion screen_region() const;

	//! ���������� ������ �������� �� ����������� � ��������.
	int size_x() const;
	//! ���������� ������ �������� �� ��������� � ��������.
	int size_y() const;

	//! ���������� ������ �������-������ �� ����������� � ��������.
	int qdInterfaceSave::thumbnail_size_x() const {	return thumbnail_size_x_; }
	//! ���������� ������ �������-������ �� ��������� � ��������.
	int qdInterfaceSave::thumbnail_size_y() const {	return thumbnail_size_y_; }

	//! ��������� ������ �������-������ �� ����������� � ��������.
	void qdInterfaceSave::set_thumbnail_size_x(int sx){ thumbnail_size_x_ = sx; }
	//! ��������� ������ �������-������ �� ��������� � ��������.
	void qdInterfaceSave::set_thumbnail_size_y(int sy){ thumbnail_size_y_ = sy; }

	//! �������� ������, ���������� � �����, ������������ ������-�������� ����
	int text_dx() const { return text_dx_; }
	int text_dy() const { return text_dy_; }
	void set_test_dx(int val) { text_dx_ = val; }
	void set_test_dy(int val) { text_dy_ = val; }

	//! ������ ������, �������� - ����� � ��������.
	bool quant(float dt);

	//! ���������� true, ���� ����� � ��������� ������������ (x,�) �������� � �������.
	bool hit_test(int x,int y) const;

	//! ���������� ����� �����.
	int save_ID() const { return save_ID_; }
	//! ������������� ����� �����.
	void set_save_ID(int id){ save_ID_ = id; }

	//! ���������� ��� ����-�����.
	const char* save_file() const;
	//! ���������� ��� ����� ��������-������ �����.
	const char* thumbnail_file() const;
	//! ���������� ��� ����� �������� �����.
	const char* description_file() const;

	//! ������������� ����� ������ - ���������� (state == true) ��� ��������� (state == false) �����.
	static void set_save_mode(bool state){ save_mode_ = state; }

	//! ������������� ��� ����� ��� ��������.
	/**
	���� ���� ������ �������� - �������� NULL � �������� ����� �����.
	*/
	void set_frame_animation_file(const char* name,qdInterfaceElementState::state_mode_t mode = qdInterfaceElementState::MOUSE_HOVER_MODE){ frame_.set_animation_file(name,mode); }
	//! ���������� ��� ����� ��� ��������.
	const char* frame_animation_file(qdInterfaceElementState::state_mode_t mode = qdInterfaceElementState::MOUSE_HOVER_MODE) const { return frame_.animation_file(mode); }
	//! ���������� ����� ��������.
	int frame_animation_flags(qdInterfaceElementState::state_mode_t mode = qdInterfaceElementState::MOUSE_HOVER_MODE) const { return frame_.animation_flags(mode); }

	//! ������������� ���� ��������.
	void set_frame_animation_flag(int fl,qdInterfaceElementState::state_mode_t mode = qdInterfaceElementState::MOUSE_HOVER_MODE){ frame_.set_animation_flag(fl,mode); }
	//! ��������� ���� ��������.
	void drop_frame_animation_flag(int fl,qdInterfaceElementState::state_mode_t mode = qdInterfaceElementState::MOUSE_HOVER_MODE){ frame_.drop_animation_flag(fl,mode); }
	//! ���������� true, ���� ��� �������� ���������� ���� fl.
	bool check_frame_animation_flag(int fl,qdInterfaceElementState::state_mode_t mode = qdInterfaceElementState::MOUSE_HOVER_MODE) const { return frame_.check_animation_flag(fl,mode); }

	//! ���������� true, ���� � ����� ��������� �������� �����.
	bool frame_has_animation(qdInterfaceElementState::state_mode_t mode = qdInterfaceElementState::MOUSE_HOVER_MODE) const { return frame_.has_animation(mode); }

	void set_frame_sound_file(const char* name,qdInterfaceElementState::state_mode_t mode = qdInterfaceElementState::MOUSE_HOVER_MODE){ frame_.set_sound_file(name,mode);}
	const char* frame_sound_file(qdInterfaceElementState::state_mode_t mode = qdInterfaceElementState::MOUSE_HOVER_MODE) const { return frame_.sound_file(mode);}

	bool isAutosaveSlot() const { return isAutosaveSlot_; }
	void setAutosaveSlot(bool state){ isAutosaveSlot_ = state; }

	void set_title(const char* title){ save_title_ = title; }
	const char* title() const { return save_title_.c_str(); }

	//! ����������� ���������� �������� ��������� ���� � ������ �����.
	bool perform_save();

protected:

	//! ������ ������ � ������.
	bool save_script_body(XStream& fh,int indent = 0) const;
	//! �������� ������ �� �������.
	bool load_script_body(const xml::tag* p);

private:

	//! �������������� ������ ��������-������ �����.
	int thumbnail_size_x_;
	//! ������������ ������ ��������-������ �����.
	int thumbnail_size_y_;

	//! �������� ������, ���������� � �����, ������������ ������-�������� ����
	int text_dx_;
	int text_dy_;

	//! ����� �����, ��� ����� ����� ��� saveNNN.dat, ��� NNN - save_ID_.
	int save_ID_;

	/// ���� true, �� ����� ��� �� ����� �������� ���� � ���� ����
	bool isAutosaveSlot_;

	//! ������ �����.
	qdInterfaceElementState thumbnail_;

	std::string save_title_;

	//! ������������ ����� ������ ��������-������ �����.
	qdInterfaceElementState frame_;

	//! ����� ������ ������ - ������/������ (== false/true).
	static bool save_mode_;
	
	//! ������� ����� �����.
	static int current_save_ID_;
};

#endif /* __QD_INTERFACE_SAVE_H__ */

