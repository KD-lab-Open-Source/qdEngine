#include "stdafx.h"
#include "resource.h"
#include "directionnamebyangle.h"

DirectionNameByAngle::DirectionNameByAngle(void)
{
}

DirectionNameByAngle::~DirectionNameByAngle(void)
{
}

CString DirectionNameByAngle::get(float fAngle)
{
	UINT uiStrID = 0;
	if(fAngle < 1.f)
	{
		uiStrID = IDS_GAITS_RIGHT;
	}
	else if (abs(90.f - fAngle) < 1.f) {
		uiStrID = IDS_GAITS_BACK;
	}
	else if (abs(180.f - fAngle) < 1.f) {
		uiStrID = IDS_GAITS_LEFT;
	}
	else if (abs(270.f - fAngle) < 1.f) {
		uiStrID = IDS_GAITS_FORWARD;
	}
	else if (abs(360.f-fAngle) < 1.f) {
		uiStrID = IDS_GAITS_RIGHT;
	}
	if(uiStrID)
		return CString(reinterpret_cast<LPTSTR>(uiStrID));
	return CString();
}
