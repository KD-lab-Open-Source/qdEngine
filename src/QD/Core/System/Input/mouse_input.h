#ifndef __MOUSE_INPUT_H__
#define __MOUSE_INPUT_H__

//! ���������� ����.
class mouseDispatcher
{
public:
	mouseDispatcher();
	~mouseDispatcher();

	//! ���������� �������.
	/**
	���������� true ��� �������� ��������� �������.
	*/
	typedef bool (*event_handler_t)(int x,int y,int flags);

	//! �������.
	enum mouseEvent {
		//! ������� ����� ������.
		EV_LEFT_DOWN = 0,
		//! ������� ������ ������.
		EV_RIGHT_DOWN,
		//! ������� ������� ����� ������.
		EV_LEFT_DBLCLICK,
		//! ������� ������� ������ ������.
		EV_RIGHT_DBLCLICK,
		//! ������� ����� ������.
		EV_LEFT_UP,
		//! ������� ������ ������.
		EV_RIGHT_UP,
		//! ����������� ����.
		EV_MOUSE_MOVE
	};

	//! �������������� ������.
	enum mouseButtonID {
		ID_BUTTON_LEFT,
		ID_BUTTON_MIDDLE,
		ID_BUTTON_RIGHT
	};

	//! ��������� ����������� �������.
	event_handler_t set_event_handler(mouseEvent ev,event_handler_t h){
		event_handler_t old_h = event_handlers_[ev];
		event_handlers_[ev] = h;
		return old_h;
	}

	//! ��������� �������.
	bool handle_event(mouseEvent ev,int x,int y,int flags);

	//! ���������� true, ���� ������� ����������� � ���� �� ����������.
	bool check_event(mouseEvent ev) const { if(events_ & (1 << ev)) return true; else return false; }
	//! ���������� true, ���� ������� ����������� � ������� ������ clear_events().
	bool is_event_active(mouseEvent ev) const { if(active_events_ & (1 << ev)) return true; else return false; }
	//! ������� ���������� � ��������.
	bool clear_events(){ events_ = active_events_ = 0; return true; }
	//! ������� ���������� � ������� ev.
	bool clear_event(mouseEvent ev){ events_ &= ~(1 << ev); return true; }
	//! �������� �������, ��� ���������������.
	bool deactivate_event(mouseEvent ev){ active_events_ &= ~(1 << ev); return true; }
	//! �������� ������� ��� ������������.
	void toggle_event(mouseEvent ev){ events_ |= (1 << ev); active_events_ |= (1 << ev); }

	//! ���������� �������������� ���������� �������� �������.
	int mouse_x() const { return mouse_x_; }
	//! ���������� ������������ ���������� �������� �������.
	int mouse_y() const { return mouse_y_; }

	//! ���������� true, ���� ������ bt_id ������.
	bool is_pressed(mouseButtonID bt_id){ return (button_status_ & (1 << bt_id)); }

	//! ���������� ���������� ��-���������.
	static mouseDispatcher* instance();

	//! ���������� ������������� ������� �������.
	static mouseEvent first_event_ID(){ return EV_LEFT_DOWN; }
	//! ���������� ������������� ���������� �������.
	static mouseEvent last_event_ID(){ return EV_MOUSE_MOVE; }

private:
	//! ������� - ��� �������� ��������� ������� �������� �� ��������� �������������� ����.
	int events_;
	//! ������� - ��� ��������, ����������� �� ������� � ������� ������ clear_events()
	int active_events_;

	//! ������ ������ - ������ ��� ���.
	int button_status_;

	//! �������������� ���������� �������� �������.
	int mouse_x_;
	//! ������������ ���������� �������� �������.
	int mouse_y_;

	//! ����������� �������.
	event_handler_t event_handlers_[EV_MOUSE_MOVE + 1];
};

#endif /* __MOUSE_INPUT_H__ */
