#ifndef __QD_CONTOUR_H__
#define __QD_CONTOUR_H__

#include <vector>

#include "xml_fwd.h"

class XStream;

//! ������.
/**
������������ ��� ������� ����� �� ����������� ��������,
��� �� ����� � ����� ��������� GUI.
*/
class qdContour
{
public:
	enum qdContourType {
		//! �������������
		CONTOUR_RECTANGLE,
		//! ����������
		CONTOUR_CIRCLE,
		//! ������������ �������������
		CONTOUR_POLYGON
	};
		
	qdContour(qdContourType tp = CONTOUR_POLYGON);
	qdContour(const qdContour& ct);
	virtual ~qdContour();

	qdContour& operator = (const qdContour& ct);

	//! ���������� ��� �������.
	qdContourType contour_type() const { return contour_type_; }
	//! ������������� ��� �������.
	void set_contour_type(qdContourType tp){ contour_type_ = tp; }

	//! ���������� �������������� ������ �������.
	int size_x() const { return size_.x; }
	//! ���������� ������������ ������ �������.
	int size_y() const { return size_.y; }

	//! ���������� true, ���� ����� � ������������ pos ����� ������ �������.
	bool is_inside(const Vect2s& pos) const;

	//! ������ ������ � ������.
	virtual bool save_script(XStream& fh,int indent = 0) const;
	//! ������ ������ �� �������.
	virtual bool load_script(const xml::tag* p);

	//! ��������� ����� � ������.
	/**
	pt - ���������� �����
	*/
	void add_contour_point(const Vect2s& pt);
	//! ��������� ����� � ������.
	/**
	pt - ���������� �����, insert_pos - ����� �����, ����� ������� ��������� �����.
	*/
	void insert_contour_point(const Vect2s& pt,int insert_pos = 0);
	//! ������� ����� ����� pos �� �������.
	bool remove_contour_point(int pos);
	//! ����������� ����� ����� pos ������� ���������� pt.
	bool update_contour_point(const Vect2s& pt,int pos);

	//! ������� ��� ����� �� �������.
	void clear_contour(){ 
		contour_.clear();
#ifdef _QUEST_EDITOR
		contour_updated_.clear();
#endif
	}

	//! ����� ���������� ������� �� 2 � ������� shift.
	void divide_contour(int shift = 1);
	//! �������� ���������� ������� �� 2 � ������� shift.
	void mult_contour(int shift = 1);
	//! ��������� � ��������������� ����������� dx, dy.
	void shift_contour(int dx, int dy);

	//! ���������� ���������� ����� ������� ����� pos.
	const Vect2s& get_point(int pos) const { return contour_[pos]; }

	//! ���������� ������� �����.
	const Vect2s& mask_size() const { return size_; }

	//! ���������� ���������� ������ �����.
	const Vect2s& mask_pos() const { return mask_pos_; }

	//! ���������� ���������� ����� � �������.
	int contour_size() const { return contour_.size(); }
	//! ���������� ������ ����� �������.
	const std::vector<Vect2s>& get_contour() const { return contour_; }
#ifdef _QUEST_EDITOR
	void set_contour(std::vector<Vect2s> const& contour);
#endif // _QUEST_EDITOR
	bool is_mask_empty() const { return contour_.empty(); }

	bool update_contour();

	// ����� �� �������� ������� ������.
	// ��� ����� ������� CONTOUR_CIRCLE � CONTOUR_RECTANGLE
	// ������ ������������ false
	bool can_be_closed() const;

	//! ���������� true, ���� ������ ������.
	bool is_contour_empty() const;

private:
	//! ��� �������.
	qdContourType contour_type_;

	//! ������� �������.
	Vect2s size_;

	Vect2s mask_pos_;

	//! ������.
	/**
	������������ ��������� ������. �������� ��� CONTOUR_POLYGON.
	*/
	std::vector<Vect2s> contour_;
#ifdef _QUEST_EDITOR
	std::vector<Vect2s> contour_updated_;
#endif
};

#endif /* __QD_CONTOUR_H__ */

