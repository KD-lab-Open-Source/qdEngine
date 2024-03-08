#ifndef __QD_SCREEN_TEXT_SET_H__
#define __QD_SCREEN_TEXT_SET_H__

#include "qd_screen_text.h"
#include "xml_fwd.h"

//! ����� �������� �������.
class qdScreenTextSet
{
public:
	qdScreenTextSet();
	~qdScreenTextSet();

	//! ���������� ������������� ������.
	int ID() const { return ID_; }
	//! ������������� ������������� ������.
	void set_ID(int id){ ID_ = id; }

	//! ���������� �������� ���������� ������ ������.
	const Vect2i& screen_pos() const { return pos_; }
	//! ������������� �������� ���������� ������ ������.
	void set_screen_pos(const Vect2i& pos){ pos_ = pos; }

	//! ���������� ������� ������ �� ������.
	const Vect2i& screen_size() const { return size_; }
	//! ������������� ������� ������ �� ������.
	void set_screen_size(const Vect2i& sz){ size_ = sz; }

	//! ���������� ������ � �����.
	qdScreenText* add_text(const qdScreenText& txt);
	//! ������� ���� ������� ������.
	void clear_texts(){ texts_.clear(); arrange_texts(); }
	//! ������� ���� ������� � ���������� owner.
	void clear_texts(qdNamedObject* owner);

	//! ��������� ������.
	void redraw() const;
	bool pre_redraw() const;
	bool post_redraw();

	grScreenRegion screen_region() const;

	//! ����������� ������ � ������.
	bool arrange_texts();

	void load_script(const xml::tag* p);
	bool save_script(XStream& fh,int indent = 0) const;

	bool need_redraw() const { return need_redraw_; }

	bool is_empty() const { return texts_.empty(); }

	void set_max_text_width(int width){ max_text_width_ = width; format_texts(); arrange_texts(); }

	//! ���������� �����, � ������� �������� ����� � ��������� ������������ (x,y).
	qdScreenText* get_text(int x,int y);

	bool was_changed() const { return was_changed_; }
	void toggle_changed(bool state){ was_changed_ = state; }

	int new_texts_height() const { return new_texts_height_; }
	void clear_new_texts_height(){ new_texts_height_ = 0; }

	void clear_hover_mode();

private:

	//! ������������� ������.
	int ID_;

	//! �������� ���������� ������ ������.
	Vect2i pos_;
	//! ������� �������, ���������� ��� ����� �� ������.
	Vect2i size_;

	//! ���������� ����� ��������� �������� � ��������.
	int space_;

	typedef std::vector<qdScreenText> texts_container_t;
	//! ������.
	texts_container_t texts_;

	//! ��������������� � true ��� ����������/�������� �������.
	bool was_changed_;

	//! ������������ ������ ������ � ��������.
	/**
	���� ������� - �� �����������.
	*/
	int max_text_width_;

	int new_texts_height_;

	bool need_redraw_;
	grScreenRegion last_screen_region_;

	//! ����������� ������ �� ������, ����� �� �������� �� max_text_width_.
	void format_texts();
};

#endif /* __QD_SCREEN_TEXT_SET_H__ */
