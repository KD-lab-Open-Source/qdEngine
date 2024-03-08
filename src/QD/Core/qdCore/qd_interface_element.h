#ifndef __QD_INTERFACE_ELEMENT_H__
#define __QD_INTERFACE_ELEMENT_H__

#include "snd_sound.h"
#include "mouse_input.h"

#include "gr_screen_region.h"

#include "qd_animation.h"
#include "qd_interface_object_base.h"
#include "qd_sound_handle.h"

//class qdInterfaceScreen;
class qdInterfaceElementState;

//! ������� ����� ��� ��������� GUI.
class qdInterfaceElement : public qdInterfaceObjectBase
{
public:
	qdInterfaceElement();
	qdInterfaceElement(const qdInterfaceElement& el);
	virtual ~qdInterfaceElement() = 0;

	qdInterfaceElement& operator = (const qdInterfaceElement& el);

	//! �������������� �������� ����.
	enum option_ID_t {
		OPTION_NONE = 0,
		/// ���� ���/����, 1/0
		OPTION_SOUND,
		/// ��������� �����, [0, 255]
		OPTION_SOUND_VOLUME,
		/// ������ ���/����, 1/0
		OPTION_MUSIC,
		/// ��������� ������, [0, 255]
		OPTION_MUSIC_VOLUME,
		/// ������������ �������� �������� �������/���������, 1/0
		OPTION_ACTIVE_PERSONAGE
	};
	
	//! ���� ��������� ����������.
	enum element_type {
		//! ���
		EL_BACKGROUND,
		//! ������
		EL_BUTTON,
		//! �������
		EL_SLIDER,
		//! ����
		EL_SAVE,
		//! ���� � ��������
		EL_TEXT_WINDOW,
		//! �������
		EL_COUNTER
	};

	//! ������ ���������.
	enum state_status_t {
		//! ��������� ���������
		STATE_INACTIVE,
		//! ��������� �������
		STATE_ACTIVE,
		//! ������ ��������� �����������
		STATE_DONE
	};

	//! �������� �������� ���������� ��������� ����.
	static qdInterfaceElement* create_element(element_type tp);
	//! ����������� �������� ����������.
	static void destroy_element(qdInterfaceElement* p);
	//! ���������� ��� ��������.
	virtual element_type get_element_type() const = 0;

	//! ���������� ������������� ��������� ����, ��������� � ���������.
	option_ID_t option_ID() const { return option_ID_; }
	//! ������������� ������������� ��������� ����, ��������� � ���������.
	void set_option_ID(option_ID_t id){ option_ID_ = id; }
	//! ���������� true, ���� � ��������� ������� ��������� ����.
	bool linked_to_option() const { return (option_ID_ != OPTION_NONE); }
	//! ���������� �������� ��������� ����, ��������� � ���������.
	virtual int option_value() const { return -1; }
	//! ������������� �������� ��������� ����, ��������� � ���������.
	virtual bool set_option_value(int value){ return false; }

	//! ���������� �������� ���������� ������ ��������.
	virtual const Vect2i r() const { return r_ + screen_offset_; }
	//! ������������� �������� ���������� ������ ��������.
	void set_r(const Vect2i& v){ r_ = v; }

	//! ���������� ������ �������� �� ����������� � ��������.
	virtual int size_x() const { return animation_.size_x(); }
	//! ���������� ������ �������� �� ��������� � ��������.
	virtual int size_y() const { return animation_.size_y(); }

	virtual grScreenRegion screen_region() const;
	const grScreenRegion& last_screen_region() const { return last_screen_region_; }

	//! ������������� �������� ������� ��������.
	void set_screen_depth(int v){ screen_depth_ = v; }
	//! ���������� �������� ������� ��������.
	int screen_depth() const { return screen_depth_; }

	//! ������ ������ � ������.
	bool save_script(XStream& fh,int indent = 0) const;
	//! �������� ������ �� �������.
	bool load_script(const xml::tag* p);

	//! ��������� ��������.
	virtual bool redraw() const;
	virtual bool need_redraw() const;
	virtual bool post_redraw();

	//! ���������� ������� ����.
	virtual bool mouse_handler(int x,int y,mouseDispatcher::mouseEvent ev) = 0;
	//! ���������� ����� � ����������.
	virtual bool keyboard_handler(int vkey) = 0;
	//! ���������� ����� �������� � ����������.
	virtual bool char_input_handler(int vkey){ return false; }

	virtual void hover_clear(){ }

	//! ������������� ��������. 
	/**
	���������� ������ ��� ��� ������ �� �����, �������� ����������� �������.
	*/
	virtual bool init(bool is_game_active = true) = 0;

	//! ������ ������, �������� - ����� � ��������.
	virtual bool quant(float dt);

	const qdAnimation& animation() const { return animation_; }

	//! ������������� �������� ��� ��������.
	bool set_animation(const qdAnimation* anm,int anm_flags = 0);
	//! �������� ��������� ��������.
	bool set_state(const qdInterfaceElementState* p);

	//! ��������� ������ file_name � ���������� owner.
	qdResource* add_resource(const char* file_name,const qdInterfaceElementState* res_owner);
	//! ������� ������ file_name � ���������� owner.
	bool remove_resource(const char* file_name,const qdInterfaceElementState* res_owner);

	//! ���������� true, ���� ����� � ��������� ������������ (x,�) �������� � �������.
	virtual bool hit_test(int x,int y) const;

	//! ���������� ������ ���������.
	state_status_t state_status(const qdInterfaceElementState* p) const;

	//! ������ �������.
	void hide(){ is_visible_ = false; }
	//! ���������� �������.
	void show(){ is_visible_ = true; }
	//! ���������� true, ���� ������� �� �������.
	bool is_visible() const { return is_visible_; }

	//! ���������� true, ���� ������� ������������.
	bool is_locked() const { return is_locked_; }
	//! ����������/������������� ��������.
	void set_lock(bool state){ is_locked_ = state; }

	static const Vect2i& screen_offset(){ return screen_offset_; }
	static void set_screen_offset(const Vect2i& offset){ screen_offset_ = offset; }

protected:

	//! ������ ������ � ������.
	virtual bool save_script_body(XStream& fh,int indent = 0) const = 0;
	//! �������� ������ �� �������.
	virtual bool load_script_body(const xml::tag* p) = 0;

	void clear_screen_region(){ last_screen_region_ = grScreenRegion::EMPTY; }

private:

	//! ������������� ��������� ����, ��������� � ���������.
	option_ID_t option_ID_;
	//! ������ ��� ��������� ����, ��������� � ���������.
	std::string option_data_;

	//! �������� ���������� ������ ��������.
	Vect2i r_;

	//! �������� �������. ��� ������ �������� - ��� ������ �������.
	int screen_depth_;

	//! �������� ��������.
	qdAnimation animation_;

	//! ����� ��� ���������� �������.
	qdSoundHandle sound_handle_;

	//! ����� false, ���� ������� �������.
	bool is_visible_;

	//! ����� true, ���� ������� ������������ (�� ��������� �� ����������/����).
	bool is_locked_;

	grScreenRegion last_screen_region_;
	const qdAnimationFrame* last_animation_frame_;

	static Vect2i screen_offset_;
};

#endif /* __QD_INTERFACE_ELEMENT_H__ */
