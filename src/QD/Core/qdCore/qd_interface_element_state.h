#ifndef __QD_INTERFACE_ELEMENT_STATE_H__
#define __QD_INTERFACE_ELEMENT_STATE_H__

#include "xml_fwd.h"

#include "qd_animation_info.h"
#include "qd_interface_object_base.h"
#include "qd_interface_element_state_mode.h"

//! ������������ �������.
class qdInterfaceEvent
{
public:
	//! �������, ������������ ���������� ����������.
	enum event_t {
		EVENT_NONE = 0,
		//! ����� �� ����
		EVENT_EXIT,				// 1
		//! ������� �� �����, �������� - ��� �����
		EVENT_LOAD_SCENE,			// 2
		//! �������� ���� [�� ������������]
		EVENT_SAVE_GAME,			// 3
		//! ����� ����
		EVENT_NEW_GAME,				// 4
		//! ������� �� ������ ������������ ����� (�������� ��� ������)
		EVENT_CHANGE_INTERFACE_SCREEN,		// 5
		//! ������� ��������� ���������
		EVENT_CHANGE_PERSONAGE,			// 6
		//! �������� �������� ������� ��������� ������ (�� �������� �� ������ �����)
		// (�������� ��� ��������)
		EVENT_TMP_HIDE_ELEMENT,			// 7
		//! �������� ������� ��������� ������ (��������)
		// (�������� ��� ��������)
		EVENT_HIDE_ELEMENT,			// 8
		//! �������� ������� ��������� ������
		// (�������� ��� ��������)
		EVENT_SHOW_ELEMENT,			// 9
		//! ���������� ����
		EVENT_RESUME_GAME,			// 10
		//! �������� ����� ������ ����
		/**
		��� ������ �� ����� � ������� ����� �����������
		������ ���� ��� ����� �� �����
		*/
		EVENT_SET_SAVE_MODE,			// 11
		//! �������� ����� �������� ����
		/**
		��� ������ �� ����� � ������� ����� �����������
		�������� ����������� ���� ��� ����� �� �����
		*/
		EVENT_SET_LOAD_MODE,			// 12
		//! ������� ��������� ��������, ��� ��������� - � ������ �������
		EVENT_ACTIVATE_PERSONAGE,		// 13
		//! �������� ���������� ��������� ������ (�������� ��� ������)
		EVENT_PREV_ELEMENT_STATE,		// 14
		//! �������� ��������� ��������� ������ (�������� ��� ������)
		EVENT_NEXT_ELEMENT_STATE,		// 15
		//! ������� � main menu
		EVENT_MAIN_MENU,			// 16
		//! ������ ���������� (�������� - ��� ����������� �� ������ � qdGameDispatcher)
		EVENT_PLAY_VIDEO,			// 17
		//! �������� ������������ ��������� ������, �������� - ��� ������
		EVENT_BUTTON_STATE,			// 18
		//! ������� � ���� ������ � ���������
		EVENT_CLEAR_MOUSE,			// 19
		//! ���������
		EVENT_SCROLL_LEFT,			// 20
		EVENT_SCROLL_RIGHT,			// 21
		EVENT_SCROLL_UP, 			// 22
		EVENT_SCROLL_DOWN,			// 23
		//! �������� ����� ����������, ��� ���������
		EVENT_SHOW_INTERFACE_SCREEN_AS_MODAL,	// 24
		//! ������������ �������� ������ � ��������� ����� � ������� ��
		EVENT_MODAL_OK,				// 25
		//! ������������ �������� ������ � ��������� ����� � ������� ������
		EVENT_MODAL_CANCEL,			// 26
		/// ��� ������ � ������� ��������, �������� - ����� �����
		EVENT_HALL_OF_FAME_PLAYER,	// 27
		/// ���� ������ � ������� ��������, �������� - ����� �����
		EVENT_HALL_OF_FAME_SCORE,	// 28
		/// ������� ���� ������ �� ������ ������� ��������
		EVENT_HALL_OF_FAME_CUR_SCORE	// 29
	};

	/// ������� ������� �������
	enum activation_t {
		/// ����� ���� �� ������
		EVENT_ACTIVATION_CLICK = 0,
		/// ��������� �������� �������
		EVENT_ACTIVATION_HOVER
	};
	
	qdInterfaceEvent(event_t id, const char* data, bool anm_flag, activation_t activation = EVENT_ACTIVATION_CLICK) : event_(id), event_data_(data), is_before_animation_(anm_flag), activation_(activation) { }
	~qdInterfaceEvent(){}

	bool operator == (event_t id) const { return (event_ == id); }

	//! ��� �������.
	event_t event() const { return event_; }
	//! ���������� true, ���� � ������� ���� ������.
	bool has_data() const { return !event_data_.empty(); }
	//! ���������� ������ ��� �������.
	const char* event_data() const { return event_data_.c_str(); }
	//! ���������� true, ���� ������� ������ ����������� �� ��������� ��������.
	bool is_before_animation() const { return is_before_animation_; }
	activation_t activation() const { return activation_; }

#ifdef _QUEST_EDITOR
	//! ������������� �� ��� ����� ��������� �������� ������ ��������� �������
	void set_before_animation(bool bFlag) { is_before_animation_ = bFlag;}
	void set_event_data(const char* data){ 
		if (data) 
			event_data_ = data; 
		else
			event_data_.clear();
	}
	void set_activation(activation_t act){ activation_ = act; }
#endif // _QUEST_EDITOR

private:

	//! ��� �������.
	event_t event_;
	//! ������ ��� �������.
	std::string event_data_;
	//! ����� true, ���� ������� ������ ����������� �� ��������� ��������.
	bool is_before_animation_;
	//! ��� ������� ������������
	activation_t activation_;
};

//! ��������� �������� ����������.
class qdInterfaceElementState : public qdInterfaceObjectBase
{
public:
	//! �������������� ��� ������� ������ ���������.
	enum state_mode_t {
		//! ����� �� ���������
		DEFAULT_MODE = 0,
		//! �����, ������� ���������� ����� ������� ������ ��������� ��� ���������-����������
		MOUSE_HOVER_MODE,
		//! �����, ������� ���������� ��� ����� �� ��������-���������
		EVENT_MODE
	};

	enum {
		//! ���������� ������� ������ ���������
		NUM_MODES = 3
	};

	qdInterfaceElementState();
	qdInterfaceElementState(const qdInterfaceElementState& st);
	~qdInterfaceElementState();

	qdInterfaceElementState& operator = (const qdInterfaceElementState& st);

	//! ������������� ��� ����� ��������� �������, ������������ � ���������.
	/**
	���� ���� ������ ���� - �������� NULL � �������� ����� �����.
	*/
	void set_sound_file(const char* str,state_mode_t snd_id = DEFAULT_MODE);
	//! ���������� ��� ����� ��������� �������, ������������ � ���������.
	const char* sound_file(state_mode_t snd_id = DEFAULT_MODE) const { return modes_[snd_id].sound_file(); }
	//! ���������� ��������� �� �������� ������, ����������� � ���������.
	const qdSound* sound(state_mode_t snd_id = DEFAULT_MODE) const { return modes_[snd_id].sound(); }
	//! ���������� true, ���� � ��������� �������� ����.
	bool has_sound(state_mode_t snd_id = DEFAULT_MODE) const { return modes_[snd_id].has_sound(); }

	//! ������ ��������� ����� ������� (���� ���� ������ � ����� ������)
	bool adjust_res_file(const char* res_file, const char* dir, const char*& new_path, bool can_overwrite = false);

	//! ������������� ��� ����� ��� ��������.
	/**
	���� ���� ������ �������� - �������� NULL � �������� ����� �����.
	*/
	void set_animation_file(const char* name,state_mode_t anm_id = DEFAULT_MODE);
	//! ���������� ��� ����� ��� ��������.
	const char* animation_file(state_mode_t anm_id = DEFAULT_MODE) const { return modes_[anm_id].animation_file(); }
	//! ���������� ����� ��������.
	int animation_flags(state_mode_t anm_id = DEFAULT_MODE) const { return modes_[anm_id].animation_flags(); }

	//! ������������� ���� ��������.
	void set_animation_flag(int fl,state_mode_t anm_id = DEFAULT_MODE){ modes_[anm_id].set_animation_flag(fl); }
	//! ��������� ���� ��������.
	void drop_animation_flag(int fl,state_mode_t anm_id = DEFAULT_MODE){ modes_[anm_id].drop_animation_flag(fl); }
	//! ���������� true, ���� ��� �������� ���������� ���� fl.
	bool check_animation_flag(int fl,state_mode_t anm_id = DEFAULT_MODE) const { return modes_[anm_id].check_animation_flag(fl); }

	//! ���������� ��������� �� ��������, ����������� � ���������.
	const qdAnimation* animation(state_mode_t anm_id = DEFAULT_MODE) const { return modes_[anm_id].animation(); }
	//! ���������� true, ���� � ��������� ��������� ��������.
	bool has_animation(state_mode_t anm_id = DEFAULT_MODE) const { return modes_[anm_id].has_animation(); }

	//! ������������ �������, ��������� � ���������� (�������� � ����).
	bool register_resources();
	//! ������������ �������, ��������� � ���������� (�������� � ����).
	bool unregister_resources();

	//! ������ ������ � ������.
	bool save_script(XStream& fh,int indent = 0) const;
	//! �������� ������ �� �������.
	bool load_script(const xml::tag* p);

	//! ������ ������, �������� - ����� � ��������.
	/**
	���������� ���������� ��� ��������� ���������.
	*/
	bool quant(float dt);

	//! ������������� ����� ������ ���������.
	void set_state_mode(state_mode_t mode){ state_mode_ = mode; }
	//! ���������� ����� ������ ���������.
	state_mode_t state_mode() const { return state_mode_; }
	//! ���������� true, ���� � ��������� ����� ���� ������� ����� ������ mode.
	bool has_state_mode(state_mode_t mode) const;

	//! ���������� true, ���� � ��������� ����� ������ ��� ������ mode.
	bool has_contour(state_mode_t mode) const { return modes_[mode].has_contour(); }
	//! ���������� ������ ������ mode � cnt.
	bool get_contour(state_mode_t mode,qdContour& cnt) const;
	//! ������������� ������ ��� ������ mode.
	bool set_contour(state_mode_t mode,const qdContour& cnt);

	//! ���������� true, ���� ����� � ��������� ������������ (x,�) �������� ������ ������� ������ mode.
	bool hit_test(int x,int y,state_mode_t mode) const { return modes_[mode].hit_test(x,y); }

	//! ���������� ������� ����.
	bool mouse_handler(int x,int y,mouseDispatcher::mouseEvent ev);
	//! ���������� ����� � ����������.
	bool keyboard_handler(int vkey);

	typedef std::vector<qdInterfaceEvent> events_container_t;
	//! ������ �������.
	const events_container_t& events() const { return events_; }
	//! ���������� �������.
	void add_event(const qdInterfaceEvent& ev){ events_.push_back(ev); }
	//! �������� �������
	void erase_event(int iNum){ events_.erase(events_.begin() + iNum);}
	//! �������� ���� �������
	void erase_all_events() { events_.clear();}

	/**
	���������� true, ���� ���� �� ��� ������ �� ������� ��������� ����������
	������� ����������� ����� (�.�. ��� ���������� ������ �� ����� ����).
	*/
	bool need_active_game() const;

	const qdInterfaceEvent* find_event(qdInterfaceEvent::event_t type) const;
	bool has_event(qdInterfaceEvent::event_t type,const char* ev_data = NULL) const;

private:

	//! �������, ������������ ��� ��������� ���������.
	events_container_t events_;

	//! ������� ���������.
	qdInterfaceElementStateMode modes_[NUM_MODES];

	//! ������� ����� ������ ���������.
	state_mode_t state_mode_;
	//! ����� ������ ��������� � ������� ����� �������.
	state_mode_t prev_state_mode_;

	//! ��������� ������� ���������.
	/**
	���������� ����������, ����� ������� ��������� - ������� ������ 
	����������� �� �������� (�������� true) ��� ����� ��� (�������� false).
	*/
	bool handle_events(qdInterfaceEvent::activation_t activation_type, bool before_animation);
};

#endif /* __QD_INTERFACE_ELEMENT_STATE_H__ */
