#ifndef __QD_SCREEN_TEXT_H__
#define __QD_SCREEN_TEXT_H__

#include "xml_fwd.h"

#include "gr_screen_region.h"
#include "gr_font.h"

class qdGameObjectState;

const int QD_FONT_TYPE_NONE = -1; //! ���������� ���������� ������

//! ������ ��������� ������
class qdScreenTextFormat
{
public:
	//! ������������ ������������ ������ �������.
	enum arrangement_t {
		//! ������������ �� �����������.
		ARRANGE_HORIZONTAL,
		//! ������������ �� ���������.
		ARRANGE_VERTICAL
	};

	/// ������������ ������.
	enum alignment_t {
		/// �� ������ ����
		ALIGN_LEFT,
		/// �� ������
		ALIGN_CENTER,
		/// �� ������� ����
		ALIGN_RIGHT
	};

	qdScreenTextFormat();
	~qdScreenTextFormat(){ };

	bool operator == (const qdScreenTextFormat& fmt) const 
	{ 
		return (color_ == fmt.color_ && 
			    arrangement_ == fmt.arrangement_ && 
				alignment_ == fmt.alignment_ &&
				hover_color_ == fmt.hover_color_ &&
				font_type_ == fmt.font_type() &&
				global_depend_ == fmt.is_global_depend());
	}

	bool operator != (const qdScreenTextFormat& fmt) const { return !(*this == fmt); }

	void set_arrangement(arrangement_t al){ arrangement_ = al; }
	arrangement_t arrangement() const { return arrangement_; }

	void set_alignment(alignment_t al){ alignment_ = al; }
	alignment_t alignment() const { return alignment_; }

	void set_color(int color){ color_ = color; }
	int color() const { return color_; }

	void set_hover_color(int color){ hover_color_ = color; }
	int hover_color() const { return hover_color_; }

	//! ���������� ������ ������ �� ���������.
	static const qdScreenTextFormat& default_format(){ return default_format_; }

	/// ���������� ��������� ������� ������.
	static const qdScreenTextFormat& global_text_format() { return global_text_format_; }
	static void set_global_text_format(const qdScreenTextFormat& frmt) { global_text_format_ = frmt; }

	/// ���������� ��������� ������� ������ ��� ��� ��������.
	static const qdScreenTextFormat& global_topic_format() { return global_topic_format_; }
	static void set_global_topic_format(const qdScreenTextFormat& frmt) { global_topic_format_ = frmt; }

	void set_font_type(int tp) { font_type_ = tp; }
	int font_type() const { return font_type_; }

	bool is_global_depend() const { return global_depend_; }
	void toggle_global_depend(bool flag = true) { global_depend_ = flag; }

	bool load_script(const xml::tag* p);
	bool save_script(class XStream& fh,int indent = 0) const;

private:

	//! ������������ ������������ ������ �������.
	arrangement_t arrangement_;
	/// ������������ ������.
	alignment_t alignment_;
	//! ���� ������, RGB().
	int color_;
	//! ���� ������ ��� ��������� �� ���� ����, RGB().
	int hover_color_;

	//! ������ ������ �� ���������.
	static qdScreenTextFormat default_format_;

	/// ���������� ������.
	static qdScreenTextFormat global_text_format_;
	/// ���������� ������ ��� ��������.
	static qdScreenTextFormat global_topic_format_;

	//! ��� ������
	int font_type_;

	/** ����, ����������, ��� ������ ������ ������� �� global_text_format_ ���
	    global_topic_format_ ������ ������� ����������. */
	bool global_depend_;
};

//! �������� �����.
class qdScreenText
{
public:
	qdScreenText(const char* p,const Vect2i& pos = Vect2i(0,0),qdGameObjectState* owner = NULL);
	qdScreenText(const char* p,const qdScreenTextFormat& fmt,const Vect2i& pos = Vect2i(0,0),qdGameObjectState* owner = NULL);
	~qdScreenText();

	//! �������� ���������� ������ ������.
	Vect2i screen_pos(){ return pos_; }
	//! ������������� �������� ���������� ������ ������.
	void set_screen_pos(const Vect2i& pos){ pos_ = pos; }

	const char* data() const { return data_.c_str(); }
	//! ������������� �����.
	/**
	����� ���������� �� ���������� ������.
	*/
	void set_data(const char* p);

	//! �������������� ������ ������ � ��������.
	int size_x() const { return size_.x; }
	//! ������������ ������ ������ � ��������.
	int size_y() const { return size_.y; }

	qdScreenTextFormat::arrangement_t arrangement() const { return text_format_.arrangement(); }
	qdScreenTextFormat::alignment_t alignment() const { return text_format_.alignment(); }

	qdScreenTextFormat text_format() const { return text_format_; }
	void set_text_format(const qdScreenTextFormat& fmt){ text_format_ = fmt; }

	grScreenRegion screen_region() const { return grScreenRegion(pos_.x,pos_.y,size_.x,size_.y); }

	//! ��������� ������.
	void redraw(const Vect2i& owner_pos) const;

	unsigned color() const { return text_format_.color(); }
	void set_color(unsigned col){ text_format_.set_color(col); }

	//! ���������� ��������� �� ��������� ������.
	qdGameObjectState* owner() const { return owner_; }
	//! ������������� ��������� ������.
	void set_owner(qdGameObjectState* p){ owner_ = p; }
	bool is_owned_by(const qdNamedObject* p) const;

	//! �������� ��������� ����� � �����, ��������� � �������� �����������.
	bool hit(int x,int y) const {
		if(x >= pos_.x && x < pos_.x + size_.x){
			if(y >= pos_.y && y < pos_.y + size_.y)
				return true;
		}
		return false;
	}

	void set_hover_mode(bool state){ hover_mode_ = state; }

	// ����������� ����� ���, ����� ��� ������ �� ��������� max_width (������ � ��������)
	bool format_text(int max_width);

private:

	//! �������� ���������� ������.
	Vect2i pos_;
	//! ������� ������ � ��������.
	Vect2i size_;

	//! ����� (������).
	std::string data_;
	
	//! true, ���� ��� ������� ������ ���� 
	bool hover_mode_;

	//! C����� ������������ ������.
	qdScreenTextFormat text_format_;

	//! �������� ������.
	mutable qdGameObjectState* owner_;
};

#endif /* __QD_SCREEN_TEXT_H__ */
