#pragma once
#include "MiniGameParams/ParamChanger/mgparamchangerctrlstube.h"
/*!
    Класс-ченжер получающий изменения через Edit-контрол
*/
class EditMGParamChanger :
	public MGParamChangerCtrlStube
{
public:
	EditMGParamChanger(MGParamChanger* parameterChanger);
	~EditMGParamChanger(void);

	//! Спрашиваем тип контрола, которым будет редактироваться текст
	virtual DWORD onRequestCtrl(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);

private:
	MGParamChanger* parameterChanger_;
};
