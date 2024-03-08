
#include "stdafx.h"
#include "cammoderep.h"
#include "resource.h"
#include <algorithm>

CamModeRep::CamModeRep(void)
{
	modes_.reserve(5);
	modes_.push_back(CameraModeRepres_t(IDS_CAM_MODE_OBJECT_ON_SCREEN,
												qdCameraMode::MODE_OBJECT_ON_SCREEN));
	modes_.push_back(CameraModeRepres_t(IDS_CAM_MODE_CENTER_OBJECT,
												qdCameraMode::MODE_CENTER_OBJECT));
	modes_.push_back(CameraModeRepres_t(IDS_CAM_MODE_FOLLOW_OBJECT,
												qdCameraMode::MODE_FOLLOW_OBJECT));
	modes_.push_back(CameraModeRepres_t(IDS_CAM_MODE_CENTER_OBJECT_WHEN_LEAVING, 
										qdCameraMode::MODE_CENTER_OBJECT_WHEN_LEAVING));
	modes_.push_back(CameraModeRepres_t(IDS_CAM_MODE_UNASSIGNED, 
												qdCameraMode::MODE_UNASSIGNED));
}

CamModeRep::~CamModeRep(void)
{
}

int CamModeRep::modes_count() const
{
	return modes_.size();
}

const qdCameraMode& CamModeRep::get_mode(int i) const{
	return modes_[i].mode();
}

CString CamModeRep::get_mode_string(int i) const{
	return CString((LPTSTR)modes_[i].str_id());
}

CString CamModeRep::get_mode_string(const qdCameraMode& mode) const
{
	std::vector<CameraModeRepres_t>::const_iterator i = 
									std::find(modes_.begin(), modes_.end(), mode);
	ASSERT (i != modes_.end()); 
	return CString((LPTSTR)(*i).str_id());
}

bool CamModeRep::useCenterOffset(const qdCameraMode& mode) const
{
	switch(mode.camera_mode()) {
	case qdCameraMode::MODE_CENTER_OBJECT_WHEN_LEAVING:
	case qdCameraMode::MODE_FOLLOW_OBJECT:
	case qdCameraMode::MODE_CENTER_OBJECT:
		return true;
	}
	return false;
}
