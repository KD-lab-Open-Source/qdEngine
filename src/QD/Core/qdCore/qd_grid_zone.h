#ifndef __QD_GRID_ZONE_H__
#define __QD_GRID_ZONE_H__

#include "xml_fwd.h"

#include "qd_contour.h"
#include "qd_grid_zone_state.h"

class qdCamera;

const int QD_NO_SHADOW_ALPHA		= -1;
const int QD_SHADOW_ALPHA_MIN		= 0;
const int QD_SHADOW_ALPHA_MAX		= 255;

//! ���� �� ����� �����.
class qdGridZone : public qdNamedObject, public qdContour
{
public:
	qdGridZone();
	qdGridZone(const qdGridZone& gz);

	qdGridZone& operator = (const qdGridZone& gz);

	~qdGridZone();

	//! ������� ��� ��������� ��������� ���� � ���������.
	enum status_change {
		//! ��������� ����
		ZONE_OFF = 0,
		//! �������� ����
		ZONE_ON,
		//! ����������� ����
		ZONE_TOGGLE
	};

	//! �����
	enum {
		//! �.�. "���� ������" - ��� ��������� �� ��� ����� ������ ��������
		ZONE_EXIT_FLAG = 0x01
	};

	int named_object_type() const { return QD_NAMED_OBJECT_GRID_ZONE; }

	//! �������� ������ �� �������.
	bool load_script(const xml::tag* p);
	//! ������ ������ � ������.
	bool save_script(XStream& fh,int indent = 0) const;

	//! �������� ������ �� �����.
	bool load_data(qdSaveStream& fh,int save_version);
	//! ������ ������ � ����.
	bool save_data(qdSaveStream& fh) const;

	bool state() const { return state_; }
	bool set_state(bool st);
	bool toggle_state(){ return set_state(!state_); }

	unsigned height() const { return height_; }
	bool set_height(int _h);

	bool select(qdCamera* camera, bool bSelect) const;
	bool select(bool bSelect) const;

	bool is_object_in_zone(const qdGameObject* obj) const;
	bool is_point_in_zone(const Vect2f& vPoint) const;

	unsigned int update_timer() const { return update_timer_; }

	qdGridZoneState* get_state(const char* state_name);

	//! ������������� ������, ���������� ��� ������ � ����������� ����.
	bool init();

	bool is_any_personage_in_zone() const;

	int shadow_alpha() const { return shadow_alpha_; }
	void set_shadow_alpha(int alpha){ shadow_alpha_ = alpha; }

	unsigned shadow_color() const { return shadow_color_; }
	void set_shadow_color(unsigned color){ shadow_color_ = color; }
	bool has_shadow() const { return (shadow_alpha_ != QD_NO_SHADOW_ALPHA); }

private:

	//! ������� ��������� ���� (���/����).
	bool state_;
	//! ��������� ��������� ���� (���/����).
	bool initial_state_;

	//! ���� ��������� ���������, ����� �� ��������� � ����, RGB().
	unsigned shadow_color_;
	//! ������������ ��������� ���������, ����� �� ��������� � ����.
	/**
	�������� - [0, 255].
	����������� ����� QD_NO_SHADOW_ALPHA, ��������� ��� ���� �����������.
	*/
	int shadow_alpha_;

	//! ������ ����.
	unsigned int height_;

	//! ����� ��������� ���������.
	unsigned int update_timer_;

	//! ��������� ���������� ����.
	qdGridZoneState state_on_;
	//! ��������� ����������� ����.
	qdGridZoneState state_off_;

	bool apply_zone() const;
};

#endif /* __QD_GRID_ZONE_H__ */

