#pragma once
#include "twoobjectcond.h"

class MouseObjectClickCond :
	public TwoObjectCond
{

public:
	MouseObjectClickCond(DWORD id, CWnd* parent = NULL);
	~MouseObjectClickCond(void);
protected:
	virtual bool LoadObjects();
	virtual void OnSelchangeComboObject2();
	virtual bool MouseObjectClickCond::IsCondDataComplete();
};

class MouseObjectHoverObjectCond : public MouseObjectClickCond
{
public:
	MouseObjectHoverObjectCond(DWORD id, CWnd* parent = NULL);
	~MouseObjectHoverObjectCond(void);

};