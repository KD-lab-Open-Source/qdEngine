#include "stdafx.h"
#include "resource.h"
#include ".\mousehoverzonecond.h"

MouseHoverZoneCond::MouseHoverZoneCond(DWORD id, CWnd * parent /*= NULL*/):
MouseZoneClickCond(id, parent)
{
	objectComboStaticText_.LoadString(IDS_MOUSE_HOVER);
}

MouseHoverZoneCond::~MouseHoverZoneCond(void)
{
}
