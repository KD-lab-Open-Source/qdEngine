#ifndef __KEYBOARD_INPUT_H__
#define __KEYBOARD_INPUT_H__

//! ���������� ����� � ����������.
class keyboardDispatcher
{
public:
	//! ���������� �������/������� ������.
	/**
	���������� true ��� �������� ��������� �������.
	*/
	typedef bool (*event_handler_t)(int key_vcode, bool event);

	keyboardDispatcher();
	~keyboardDispatcher();

	//! ���������� true, ���� ������ � ����� vkey � ������ ������ ������.
	bool is_pressed(int vkey) const { 
		assert(vkey >= 0 && vkey < 256);
		return key_states_[vkey];
	}

	//! ������������� ���������� �������/������� ������.
	event_handler_t set_handler(event_handler_t h){
		event_handler_t old_h = handler_;
		handler_ = h;
		return old_h;
	}

	//! ������������ ������� (event == true) ��� ������� (event == false) ������ � ����� vkey.
	bool handle_event(int vkey, bool event){
		key_states_[vkey] = event;
		if(handler_) return (*handler_)(vkey,event);
		return false;
	}

	//! ���������� ��������� ��-���������.
	static keyboardDispatcher* instance();

private:

	//! ���������� �������/������� ������.
	event_handler_t handler_;

	//! ��������� ������ - key_states_[vkey] == true ���� ������ � ����� vkey ������.
	bool key_states_[256];
};

#endif /* __KEYBOARD_INPUT_H__ */

