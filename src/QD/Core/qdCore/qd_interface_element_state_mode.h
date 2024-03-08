
#ifndef __QD_INTERFACE_ELEMENT_STATE_MODE_H__
#define __QD_INTERFACE_ELEMENT_STATE_MODE_H__

#include "qd_contour.h"

//! �������, ����������� � ��������� ������������� ��������.
/**
�������� �������� � ���� ��� ���������� ��������� ������� (�������� ��� ���������
���� �� ������� ����������.
*/
class qdInterfaceElementStateMode
{
public:
	qdInterfaceElementStateMode();
	qdInterfaceElementStateMode(const qdInterfaceElementStateMode& ev);
	~qdInterfaceElementStateMode();

	qdInterfaceElementStateMode& operator = (const qdInterfaceElementStateMode& ev);

	//! ������������� ��� ����� �����.
	/**
	���� ���� ������ ���� - �������� NULL � �������� ����� �����.
	*/
	void set_sound_file(const char* name);
	//! ���������� ��� ����� �����.
	const char* sound_file() const { return sound_file_.c_str(); }
	//! ������������� ���� �������.
	void set_sound(const qdSound* p){ sound_ = p; }
	//! ���������� ���� �������.
	const qdSound* sound() const { return sound_; }
	//! ���������� true, ���� � ������� �������� ����.
	bool has_sound() const { return !sound_file_.empty(); }

	//! ������������� ��� ����� ��� ��������.
	/**
	���� ���� ������ �������� - �������� NULL � �������� ����� �����.
	*/
	void set_animation_file(const char* name);
	//! ���������� ��� ����� ��� ��������.
	const char* animation_file() const { return animation_file_.c_str(); }
	//! ���������� ����� ��������.
	int animation_flags() const { return animation_flags_; }

	//! ������������� ���� ��������.
	void set_animation_flag(int fl){ animation_flags_ |= fl; }
	//! ��������� ���� ��������.
	void drop_animation_flag(int fl){ animation_flags_ &= ~fl; }
	//! ���������� true, ���� ��� �������� ���������� ���� fl.
	bool check_animation_flag(int fl) const { if(animation_flags_ & fl) return true; return false; }

	//! ������������� ��������� �� �������� �������.
	void set_animation(const qdAnimation* p){ animation_ = p; }
	//! ���������� ��������� �� �������� �������.
	const qdAnimation* animation() const { return animation_; }
	//! ���������� true, ���� � ��������� ��������� ��������.
	bool has_animation() const { return !animation_file_.empty(); }
	//! ���������� true, ���� � ��������� ����� ������.
	bool has_contour() const { return !contour_.is_contour_empty(); }
	//! ���������� true, ���� ����� � ��������� ������������ (x,�) �������� ������ �������.
	bool hit_test(int x,int y) const { return contour_.is_inside(Vect2s(x,y)); }

	//! ���������� ������ � cnt.
	bool get_contour(qdContour& cnt) const { cnt = contour_; return true; }
	//! ������������� ������.
	bool set_contour(const qdContour& cnt){ contour_ = cnt; return true; }

	//! ������ ������ � ������.
	/**
	type_id - ��� �������
	*/
	bool save_script(XStream& fh,int type_id = 0,int indent = 0) const;
	//! �������� ������ �� �������.
	bool load_script(const xml::tag* p);

private:

	//! ��� ����� �����.
	std::string sound_file_;

	//! ����, ���������� ��������.
	const qdSound* sound_;

	//! ��� ����� ��� ��������
	std::string animation_file_;
	//! ����� ��� ��������.
	int animation_flags_;

	//! ��������, ���������� ��������.
	const qdAnimation* animation_;

	//! ������.
	qdContour contour_;
};

#endif /* __QD_INTERFACE_ELEMENT_STATE_MODE_H__ */
