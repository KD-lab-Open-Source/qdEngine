
#ifndef __QD_INTERFACE_DISPATCHER_H__
#define __QD_INTERFACE_DISPATCHER_H__

#include "qd_resource_container.h"
#include "qd_interface_object_base.h"
#include "qd_object_list_container.h"

class qdInterfaceScreen;
class qdInterfaceElementState;

class qdInterfaceDispatcher : public qdInterfaceObjectBase
{
public:
	qdInterfaceDispatcher();
	~qdInterfaceDispatcher();

	/// ��� ���� ������ ��������� �����
	enum ModalScreenMode
	{
		MODAL_SCREEN_SAVE_OVERWRITE,
		MODAL_SCREEN_SAVE_NAME_EDIT,
		MODAL_SCREEN_OTHER
	};

	//! ��������� ����������.
	bool redraw(int dx = 0,int dy = 0) const;
	bool pre_redraw();
	bool post_redraw();

	//! ������ ������, �������� - ����� � ��������.
	bool quant(float dt);

	//! ���������� ������.
	bool add_screen(qdInterfaceScreen* scr);
	//! ��������� ����� ������.
	bool rename_screen(qdInterfaceScreen* scr,const char* name);
	//! �������� ������ �� ������.
	bool remove_screen(qdInterfaceScreen* scr);
	//! ����� ������ �� �����.
	qdInterfaceScreen* get_screen(const char* screen_name);
	//! ���������� true, ���� ����� ���� � ������.
	bool is_screen_in_list(const qdInterfaceScreen* scr);

	typedef std::list<qdInterfaceScreen*> screen_list_t;
	//! ���������� ������ �������.
	const screen_list_t& screen_list() const { return screens_.get_list(); }

	//! ��������� ��������� ������.
	/**
	��� ����� ���������� �������, ��� ������ ��������� ������.
	*/
	bool select_screen(const char* screen_name,bool lock_resources = false);
	/// ��������� �������� ������.
	bool select_background_screen(qdInterfaceScreen* p);

	//! �������� ������������� ���������.
	bool select_ingame_screen(bool inventory_state = false);

	//! �������� ������, ��������� � ���������� p, ��������� ������ ��������� ����������.
	void update_personage_buttons();

	qdInterfaceScreen* selected_screen() const{ return cur_screen_;}

	//! ���������� true, ���� ����� p � ������ ������ �������.
	bool is_screen_active(const qdInterfaceScreen* p) const { return (cur_screen_ == p); }

	//! ��������� ������ file_name � ���������� owner.
	qdResource* add_resource(const char* file_name,const qdInterfaceElementState* owner);
	//! ������� ������ file_name � ���������� owner.
	bool remove_resource(const char* file_name,const qdInterfaceElementState* owner);
	//! ���������� ��������� �� ������ file_name.
	qdResource* get_resource(const char* file_name) const { return resources_.get_resource(file_name); }

	//! ���������� ������� ����.
	bool mouse_handler(int x,int y,mouseDispatcher::mouseEvent ev);
	//! ���������� ����� � ����������.
	bool keyboard_handler(int vkey);
	bool char_input_handler(int vkey);

	//! ������ ������ � ������.
	bool save_script(XStream& fh,int indent = 0) const;
	//! �������� ������ �� �������.
	bool load_script(const xml::tag* p);

	//! ��������� ����� ������ � ������� ���� ����.
	void set_main_menu_screen(const char* name);
	//! ���������� ��� ������ � ������� ���� ����.
	const char* main_menu_screen_name() const { return main_menu_screen_name_.c_str(); }
	//! ���������� true, ���� ����������� ��� ������ � ������� ���� ����.
	bool has_main_menu() const { return !main_menu_screen_name_.empty(); }

	//! ��������� ����� �������������� ������.
	void set_ingame_screen(const char* name,bool inventory_state = false);
	//! ���������� ��� �������������� ������.
	const char* ingame_screen_name(bool inventory_state = false) const { return ingame_screen_names_[inventory_state].c_str(); }
	//! ���������� true, ���� ����������� ��� �������������� ������.
	bool has_ingame_screen(bool inventory_state = false) const { return !ingame_screen_names_[inventory_state].empty(); }

	//! ��� ���������� ������ ���������: �������������� ���� ��� ���.
	const char* save_prompt_screen_name() const { return save_prompt_screen_name_.c_str(); }
	void set_save_prompt_screen_name(const char* str) { if(str) save_prompt_screen_name_ = str; else save_prompt_screen_name_ = ""; }
	bool has_save_prompt_screen() const { return !save_prompt_screen_name_.empty(); }

	const char* save_title_screen_name() const { return save_title_screen_name_.c_str(); }
	void set_save_title_screen_name(const char* name){ save_title_screen_name_ = name; }
	bool has_save_title_screen() const { return !save_title_screen_name_.empty(); }

	//! ����� �� �������� �������� � ����������
	bool need_save_screenshot() const { return need_save_screenshot_; }
	void toggle_save_screenshot(bool state) { need_save_screenshot_ = state; }
	//! ����� �� �������� ���� � ����� ��� ����������� �����
	bool need_show_save_time() const { return need_show_save_time_; }
	void toggle_show_save_time(bool state) { need_show_save_time_ = state; }
	bool need_show_save_title() const { return need_show_save_title_; }
	void toggle_show_save_title(bool state) { need_show_save_title_ = state; }
	//! ��� �����, ������� ��������� ����� ����� (� ��������� ���� � �����)
	int save_font_type() const { return save_font_type_; }
	void set_save_font_type(int type) { save_font_type_ = type; }
	//! ����, ������� ��������� ����� ����� (� ��������� ���� � �����)
	int save_font_color() const { return save_font_color_; }
	void set_save_font_color(int clr) { save_font_color_ = clr; }

	//! ���������� true, ���� ��������� �������������� ������ �����.
	bool need_scene_redraw() const { return need_scene_redraw_; }
	//! �������������, ���� �� ���� ���������� �������������� ������ �����.
	void set_scene_redraw(bool state){ need_scene_redraw_ = state; }

	static void set_dispatcher(qdInterfaceDispatcher* p){ dispatcher_ = p; }
	static qdInterfaceDispatcher* get_dispatcher(){ return dispatcher_; }

	void activate(){ is_active_ = true; }
	void deactivate(){ is_active_ = false; }
	bool is_active() const { return is_active_; }

	bool is_mouse_hover() const { return is_mouse_hover_; }
	void toggle_mouse_hover(){ is_mouse_hover_ = true; }
	bool is_autohide_enabled() const { return !autohide_disable_; }
	void disable_autohide(){ autohide_disable_ = true; }

	ModalScreenMode modalScreenMode() const { return modalScreenMode_; }
	void setModalScreenMode(ModalScreenMode mode){ modalScreenMode_ = mode; }

	bool set_save_title(const char* title);
	const char* get_save_title() const;

	/** 
	��������� �������. 
	sender - ���, ��� ������ ������� �� ����������. NULL - sender �� ��������. 
	*/
	bool handle_event(int event_code,const char* event_data,qdInterfaceObjectBase* sender=NULL);

	bool adjust_pack_files(const char* pack_dir, bool can_overwrite = false);
	//! ����� ������ ���� ������ �������� � list.
	bool get_file_list(qdFileNameList& files_to_copy, qdFileNameList& files_to_pack) const;

	void toggle_end_game_mode(bool state){ end_game_mode_ = state; }

#ifndef _QUEST_EDITOR
	//! ������������� ��������� �����.
	void set_next_screen(const char* screen_name){ next_screen_ = screen_name; }
#endif

	static int option_value(int option_id,const char* option_data = NULL);
	static bool set_option_value(int option_id,int value,const char* oprtion_data = NULL);

#ifdef __QD_DEBUG_ENABLE__
	bool get_resources_info(qdResourceInfoContainer& infos) const;
#endif

private:

	//! �������� ������������ �����.
	qdInterfaceScreen* cur_screen_;

	/// ������� �����, �������� ��� ��������
	qdInterfaceScreen* background_screen_;
	/// true ���� ������� �������� ������ �� ���� ���������
	bool background_screen_lock_;

	ModalScreenMode modalScreenMode_;

	//! ��� ������ � ������� ���� ����
	std::string main_menu_screen_name_;

	//! ����� ������� � ������������� �����������.
	/**
	������ ����� ������������ ����� ��������� ��������, ������ - ����� ��� �������.
	*/
	std::string ingame_screen_names_[2];

	/// ����� ����� ����� �����
	std::string save_title_screen_name_;
	std::string save_title_;

	//! �����, ������������ ��� ������������� ���������� ����� �����
	std::string save_prompt_screen_name_;

	//! ����� �� �������� �������� � ����������
	bool need_save_screenshot_;
	//! ����� �� �������� ���� � ����� ��� ����������� �����
	bool need_show_save_time_;
	//! ����� �� �������� ��� ��� ����������� �����
	bool need_show_save_title_;
	//! ��� ������, ������� ��������� ����� ����� (� ��������� ���� � �����)
	int save_font_type_;
	//! ����, ������� ��������� ����� ����� (� ��������� ���� � �����)
	int save_font_color_;

	//! ������������ ������.
	qdObjectListContainer<qdInterfaceScreen> screens_;

	typedef qdResourceContainer<qdInterfaceElementState> resource_container_t;
	//! ������������ �������.
	resource_container_t resources_;

	//! ����� true, ���� ��������� �������.
	bool is_active_;

	//! ����� true, ���� ������ ���� �������� � ���������.
	bool is_mouse_hover_;
	/// true ���� ���� ���������� 
	bool autohide_disable_;

	//! ����� true, ���� ��������� ������ ��������� ����������, ����� ��������� �����������.
	bool need_full_redraw_;

	//! ����� true, ���� ��������� ����� ��� �����������.
	bool need_scene_redraw_;

	//! ����� ��������� ���� - ����� ������ ����� ������� � �������� ������ � ������� ����.
	//! ����� �������� ���� ����������� � false.
	bool end_game_mode_;

#ifndef _QUEST_EDITOR
	//! ��� ������, �� ������� ���� �������.
	const char* next_screen_;
#endif

	//! ������� ��������� ����������.
	static qdInterfaceDispatcher* dispatcher_;
};

#endif /* __QD_INTERFACE_DISPATCHER_H__ */
