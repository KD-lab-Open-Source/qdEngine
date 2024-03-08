
#ifndef __QD_INTERFACE_SLIDER_H__
#define __QD_INTERFACE_SLIDER_H__

#include "xml_fwd.h"

#include "qd_interface_element.h"
#include "qd_interface_element_state.h"

//! ������������ ������� - �������.
/**
������� �� ���� ������ - ����������� �������� � "��������".
�������� �� �����������, ��� ����� ���� ���������� ����� �� ����.
*/
class qdInterfaceSlider : public qdInterfaceElement
{
public:
	//! ���������� �������� (��-��������� - �������������).
	enum orientation_t {
		SL_HORIZONTAL,
		SL_VERTICAL
	};

	qdInterfaceSlider();
	qdInterfaceSlider(const qdInterfaceSlider& sl);
	~qdInterfaceSlider();

	qdInterfaceSlider& operator = (const qdInterfaceSlider& sl);

	//! ���������� ��� ��������.
	qdInterfaceElement::element_type get_element_type() const { return qdInterfaceElement::EL_SLIDER; }

	//! ���������� ������� ����.
	bool mouse_handler(int x,int y,mouseDispatcher::mouseEvent ev);
	//! ���������� ����� � ����������.
	bool keyboard_handler(int vkey);

	int option_value() const;
	bool set_option_value(int value);

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

	bool inverse_direction() const { return inverse_direction_; }
	void set_inverse_direction(bool state){ inverse_direction_ = state; }

	const Vect2i& active_rectangle() const { return active_rectangle_; }
	void set_active_rectangle(const Vect2i& rect){ active_rectangle_ = rect; }

	const qdInterfaceElementState* background() const { return &background_; }
	void update_background(const qdInterfaceElementState& st){ background_ = st; }

	const Vect2i& background_offset() const { return background_offset_; }
	void set_background_offset(const Vect2i& offs){ background_offset_ = offs; }

	const qdInterfaceElementState* slider() const { return &slider_; }
	void update_slider(const qdInterfaceElementState& st){ slider_ = st; }

	float phase() const { return phase_; }
	void set_phase(float ph){ phase_ = ph; }

	orientation_t orientation() const { return orientation_; }
	void set_orientation(orientation_t v){ orientation_ = v; }

	//! ���������� true, ���� ����� � ��������� ������������ (x,�) �������� � �������.
	bool hit_test(int x,int y) const;

protected:
	
	//! ������ ������ � ������.
	bool save_script_body(XStream& fh,int indent = 0) const;
	//! �������� ������ �� �������.
	bool load_script_body(const xml::tag* p);

private:
	//! �������� ��� ���������.
	qdInterfaceElementState background_;
	/// �������� �������� ������������ ������ ��������.
	Vect2i background_offset_;

	//! ��������.
	qdInterfaceElementState slider_;

	//! �������� ��������.
	qdAnimation slider_animation_;

	//! �������������, ������ �������� ����� ��������.
	/**
	�� ���� �� ������������� ������� �����.
	������������� �� ������ ��������, � �������� �����������.
	*/
	Vect2i active_rectangle_;

	//! ������� ����, �������� �������� - [0.0, 1.0]. 
	float phase_;

	//! ���������� - �� ����������� ��� �� ���������.
	orientation_t orientation_;
	bool inverse_direction_;

	//! true, ���� ������� ������ �� ������������� �������� �������
	bool track_mouse_;

	bool set_slider_animation(const qdAnimation* anm,int anm_flags = 0);

	//! �������� ���� � �������� �� ������ �������� � �������� �����������.
	Vect2i phase2offset(float ph) const;
	//! �������� �������� �� ������ �������� � �������� ����������� � ����.
	float offset2phase(const Vect2i& offs) const;
	
};

#endif /* __QD_INTERFACE_SLIDER_H__ */