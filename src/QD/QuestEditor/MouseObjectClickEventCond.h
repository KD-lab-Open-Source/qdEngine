#pragma once
#include "oneobjectcond.h"

/**
	CONDITION_MOUSE_OBJECT_CLICK_EVENT
 */
class MouseObjectClickEventCond :
	public OneObjectCond
{
public:
	MouseObjectClickEventCond(DWORD id);
	~MouseObjectClickEventCond(void);
protected:
	BOOL OnInitDialog();
	/// Записывает объект в условие
	virtual afx_msg void OnSelchangeComboObject();
	bool IsCondDataComplete();

};
