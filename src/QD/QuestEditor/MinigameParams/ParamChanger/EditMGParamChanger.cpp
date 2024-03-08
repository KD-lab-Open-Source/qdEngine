#include "stdafx.h"
#include ".\editmgparamchanger.h"

#include <TreeListCtrl/TreeListColumnInfo.h>

EditMGParamChanger::EditMGParamChanger(MGParamChanger* parameterChanger)
: MGParamChangerCtrlStube(parameterChanger)
{
}

EditMGParamChanger::~EditMGParamChanger(void)
{
}

//! Спрашиваем контрол, которым будет редактироваться текст
DWORD EditMGParamChanger::onRequestCtrl(CTreeListCtrl& source, 
							CHANGE_LABEL_NOTIFY_INFO* pclns)
{
	return TLM_EDIT;
}
