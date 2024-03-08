
#ifndef __QD_INTERFACE_SCREEN_H__
#define __QD_INTERFACE_SCREEN_H__

#include "mouse_input.h"
#include "xml_fwd.h"

#include "gr_screen_region.h"

#include "qd_interface_object_base.h"
#include "qd_object_list_container.h"
#include "qd_resource_dispatcher.h"
#include "qd_music_track.h"

class qdInterfaceElement;
class qdInterfaceElementState;

//! ������������ �����.
class qdInterfaceScreen : public qdInterfaceObjectBase
{
public:
	qdInterfaceScreen();
	~qdInterfaceScreen();

	//! ������ ������ � ������.
	bool save_script(XStream& fh,int indent = 0) const;
	//! �������� ������ �� �������.
	bool load_script(const xml::tag* p);

	//! ��������� ������.
	bool redraw(int dx = 0,int dy = 0) const;

	bool pre_redraw(bool force_full_redraw = false);
	bool post_redraw();

	//! ������ ������, �������� - ����� � ��������.
	bool quant(float dt);

	//! ���������� ��������.
	bool add_element(qdInterfaceElement* p);
	//! ��������� ����� ��������.
	bool rename_element(qdInterfaceElement* p,const char* name);
	//! �������� �������� �� ������.
	bool remove_element(qdInterfaceElement* p);
	//! ����� �������� �� �����.
	qdInterfaceElement* get_element(const char* el_name);
	//! ���������� true, ���� ������� ���� � ������.
	bool is_element_in_list(const qdInterfaceElement* el) const;

	typedef std::list<qdInterfaceElement*> element_list_t;
	//! ���������� ������ ��������� ������.
	const element_list_t& element_list() const { return elements_.get_list(); }

	//! ���������� ������� ����.
	bool mouse_handler(int x,int y,mouseDispatcher::mouseEvent ev);
	//! ���������� ����� � ����������.
	bool keyboard_handler(int vkey);
	bool char_input_handler(int vkey);

	//! ��������� ������ file_name � ���������� owner.
	qdResource* add_resource(const char* file_name,const qdInterfaceElementState* res_owner);
	//! ������� ������ file_name � ���������� owner.
	bool remove_resource(const char* file_name,const qdInterfaceElementState* res_owner);
	//! ���������� true, ���� �� ������ ���� ������.
	bool has_references(const qdResource* p) const { return resources_.is_registered(p); }

	//! ������ �������.
	bool hide_element(const char* element_name,bool temporary_hide = true);
	//! ������ �������.
	bool hide_element(qdInterfaceElement* p,bool temporary_hide = true);
	//! ���������� �������.
	bool show_element(const char* element_name);
	//! ���������� �������.
	bool show_element(qdInterfaceElement* p);

	//! ������������� ������ ��� ������ �� ����.
	bool init(bool is_game_active = true);

	//! ������ ������������� ������ ������� ���������.
	bool build_visible_elements_list();

	//! �������������, ��� ������� ������ �� ���� ��������� ��� ������ � ����.
	void lock_resources(){ is_locked_ = true; }
	//! �������������, ��� ������� ������ ���� ��������� ��� ������ � ����.
	void unlock_resources(){ is_locked_ = false; }
	//! ���������� true, ���� ������� ������ �� ���� ��������� ��� ������ � ����.
	bool is_locked() const { return is_locked_; }

	//! �������� ������, ��������� � ���������� p, ��������� ������ ��������� ����������.
	void activate_personage_buttons(const qdNamedObject* p);

	void update_personage_buttons();

	float autohide_time() const { return autohide_time_; }
	void set_autohide_time(float time){ autohide_time_ = time; }

	const Vect2i& autohide_offset() const { return autohide_offset_; }
	void set_autohide_offset(const Vect2i& offset){ autohide_offset_ = offset; }

	//! ������� ������.
	const qdMusicTrack& music_track() const { return music_track_; }
	//! ������������� ������� ������.
	void set_music_track(const qdMusicTrack& track){ music_track_ = track; }
	//! ���������� true, ���� ������ ������ ������� ������.
	bool has_music_track() const { return music_track_.has_file_name(); }

#ifdef _QUEST_EDITOR
	bool select_element(qdInterfaceElement* pele, bool bselect);
	bool is_selected(const qdInterfaceElement* p) const;
	const element_list_t& selected_element_list() const{ return selected_elements_;}
	void clear_selection();
#endif //_QUEST_EDITOR

	void set_autohide_phase(float ph){ autohide_phase_ = ph; }

	// ��������� �� ������, ��������� ��������� ������ ��� ��������� �����
	qdInterfaceObjectBase* modal_caller() { return modal_caller_; }
	void set_modal_caller(qdInterfaceObjectBase* caller) { modal_caller_ = caller; }

private:

#ifdef _QUEST_EDITOR
	//! ��������� ��������
	element_list_t selected_elements_;
#endif // _QUEST_EDITOR

	//! ������ ������������ ��������� ������.
	qdObjectListContainer<qdInterfaceElement> elements_;

	//! �������, �� ������� ��������� �����.
	qdResourceDispatcher<qdInterfaceElementState> resources_;

	typedef std::vector<qdInterfaceElement*> sorted_element_list_t;
	//! ��������������� �� ������� ������ ��������� ������.
	sorted_element_list_t sorted_elements_;

	//! ��������� �������� �� �������.
	bool sort_elements();

	//! true, ���� ������� ������ �� ���� ��������� ��� ������ � ����.
	bool is_locked_;

	//! ����� ���������� ������ � ��������.
	float autohide_time_;
	//! �������� ������, ����� �� �������������.
	Vect2i autohide_offset_;

	//! ������� ���� ���������� ������.
	float autohide_phase_;

	//! ������� ������.
	qdMusicTrack music_track_;

	//! ��������� �� ������, ������� ��������� ������ ������ ����� � �������� ���������� 
	qdInterfaceObjectBase* modal_caller_;
};

#endif /* __QD_INTERFACE_SCREEN_H__ */
