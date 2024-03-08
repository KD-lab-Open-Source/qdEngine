#pragma once

#include "qd_camera_mode.h"
#include <vector>

/// ������������ ��� ��������� ������ � ���� ������ ��� ����� - ��������
class CamModeRep
{
	/// ������������ �����
	class CameraModeRepres_t
	{
		qdCameraMode mode_;
		UINT strId_;
	public:
		CameraModeRepres_t(UINT id, qdCameraMode::camera_mode_t m):strId_(id){
			mode_.set_camera_mode(m);
		}
		const qdCameraMode& mode() const{ return mode_;};
		UINT str_id() const{ return strId_;}
		bool operator == (const CameraModeRepres_t& rhs) const{
			return (mode().camera_mode() == rhs.mode().camera_mode());
		}
		bool operator == (const qdCameraMode& rhs) const{
			return (mode().camera_mode() == rhs.camera_mode());
		}
	};
	
	std::vector<CameraModeRepres_t> modes_;
public:
	CamModeRep(void);
	~CamModeRep(void);

	int modes_count() const;

	const qdCameraMode& get_mode(int i) const;

	//! �������� ������
	CString get_mode_string(int i) const;
	//! �������� ������
	CString get_mode_string(const qdCameraMode& mode) const;

	//! ��������� �� �������� ������
	bool useCenterOffset(const qdCameraMode& mode) const;
};
