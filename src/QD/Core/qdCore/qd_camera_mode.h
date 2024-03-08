
#ifndef __QD_CAMERA_MODE_H__
#define __QD_CAMERA_MODE_H__

#include "xml_fwd.h"

class qdGameObjectState;
class qdGameObjectAnimated;

class XStream;

//! ����� ������ ������.
class qdCameraMode
{
public:
	qdCameraMode();
	~qdCameraMode(){ }

	//! �������������� ������� ������
	enum camera_mode_t {
		//! ������ �� ��������� ������ � ������
		MODE_OBJECT_ON_SCREEN,
		//! ������ ������ ��������� � �������
		MODE_CENTER_OBJECT,
		//! ������ ��� ����� ���������� �� �������
		MODE_FOLLOW_OBJECT,
		//! ����� ������ �������� � ���� ������ ������ ������ �� ��� ����������
		MODE_CENTER_OBJECT_WHEN_LEAVING,
		//! ����� �� ����������
		MODE_UNASSIGNED
	};

	void set_camera_mode(camera_mode_t mode){ camera_mode_ = mode; }
	camera_mode_t camera_mode() const { return camera_mode_; }

	void set_work_time(float tm){ work_time_ = tm; }
	float work_time() const { return work_time_; }
	bool has_work_time() const { return work_time_ > 0.001f; }
	
	void set_scrolling_speed(float v){ scrolling_speed_ = v; }
	float scrolling_speed() const { return scrolling_speed_; }

	void set_scrolling_distance(int dist){ scrolling_distance_ = dist; }
	int scrolling_distance() const { return scrolling_distance_; }

	bool smooth_switch() const { return smooth_switch_; }
	void set_smooth_switch(bool v){ smooth_switch_ = v; }

	const Vect2i& center_offset() const { return center_offset_; }
	void set_center_offset(const Vect2i& offs){
		center_offset_ = offs;
	}

	bool load_script(const xml::tag* p);
	bool save_script(XStream& fh,int indent = 0) const;

	//! �������� ������ �� �����.
	bool load_data(qdSaveStream& fh,int save_version);
	//! ������ ������ � ����.
	bool save_data(qdSaveStream& fh) const;

private:

	//! ������������� ������.
	camera_mode_t camera_mode_;

	//! ����� ������ ������ (� ��������).
	/**
	�� ��������� ����� ������� ������ ������������� 
	�� ��������� ��������� (���� �� ����).

	���� ��� ����� �������, �� ������� ����� ����� �����
	������ � �������������� ������� �� ��������.
	*/
	float work_time_;
	
	//! ��������, � ������� ������ ������������� (� �������� � �������).
	float scrolling_speed_;

	//! ���������� �� ������ ������� �� ���� ������, ��� ������� ���������� ���������.
	/**
	� �������, ����� ������ �� ���� ������� ����� �� ������� ������ ���
	���������� �� ������� ����� �� ������ � ���� ������.

	�������� � ��������.
	*/
	int scrolling_distance_;

	//! �������� ������ ������ � ��������.
	/**
	��������� �������� �����, ����� ������, ��������� �� �������,
	������ ��� � ������������ ����� ������.
	*/
	Vect2i center_offset_;

	//! ������� ������������ � �����, ���� true.
	bool smooth_switch_;
};

#endif /* __QD_CAMERA_MODE_H__ */
