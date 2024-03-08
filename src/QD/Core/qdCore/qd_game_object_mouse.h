#ifndef __QD_GAME_OBJECT_MOUSE_H__
#define __QD_GAME_OBJECT_MOUSE_H__

#include "qd_game_object_animated.h"

//! ����.
class qdGameObjectMouse : public qdGameObjectAnimated
{
public:
	qdGameObjectMouse();
	qdGameObjectMouse(const qdGameObjectMouse& obj);
	~qdGameObjectMouse();

	qdGameObjectMouse& operator = (const qdGameObjectMouse& obj);

	int named_object_type() const { return QD_NAMED_OBJECT_MOUSE_OBJ; }

	bool load_script(const xml::tag* p);
	bool save_script(XStream& fh,int indent = 0) const;

	bool load_data(qdSaveStream& fh,int save_version);
	bool save_data(qdSaveStream& fh) const;

	void take_object(qdGameObjectAnimated* p);
	qdGameObjectAnimated* object(){ return object_; }

	//! �������������� ��� �������� ��-���������.
	enum cursor_ID_t {
		//! ����� ������ ��-���������
		DEFAULT_CURSOR,
		//! ������ ��� ������������� ��������
		OBJECT_CURSOR,
		//! ������ ��� ��� �� �����
		ZONE_CURSOR,
		//! ������ ��� main menu
		MAIN_MENU_CURSOR,
		//! ������ ��� �������������� ����������
		INGAME_INTERFACE_CURSOR,
		//! ������ ��� �������� � ���������
		INVENTORY_OBJECT_CURSOR,

		MAX_CURSOR_ID
	};

	//! ��������� ������� ��-���������. 
	/**
	����� ��������� ������������� � ����.
	*/
	void set_default_cursor(cursor_ID_t id,int state_index){ default_cursors_[id] = state_index; }
	//! ���������� ������ ��-���������.
	int default_cursor(cursor_ID_t id) const { return default_cursors_[id]; }

	void set_cursor(cursor_ID_t id);
	void set_cursor_state(int state_idx){ if(cur_state() != state_idx) set_state(state_idx); }

	void pre_redraw();
	void redraw(int offs_x = 0,int offs_y = 0) const;
	void post_redraw();

	void quant(float dt);

	bool update_screen_pos();

	const Vect2f& screen_pos_offset() const { return screen_pos_offset_; }

protected:
	
	bool load_script_body(const xml::tag* p);
	bool save_script_body(XStream& fh,int indent = 0) const;

private:
	
	//! ������, ������� ��������� � �������.
	qdGameObjectAnimated* object_;

	//! ������ ���������, ��������������� �������� ��-���������.
	int default_cursors_[MAX_CURSOR_ID];

	grScreenRegion object_screen_region_;

	Vect2f screen_pos_offset_;
	Vect2f screen_pos_offset_delta_;

	void update_object_position() const;
};

#endif /* __QD_GAME_OBJECT_MOUSE_H__ */
