#include "stdafx.h"
#include ".\simplemgparamchanger.h"

SimpleMGParamChanger::SimpleMGParamChanger(qdMinigameConfigParameter const& param)
: MGParamChangerStube(param)
{
}

SimpleMGParamChanger::~SimpleMGParamChanger(void)
{
}

//! ”ведомл€ет о том, что редактирование закончено
bool SimpleMGParamChanger::onEndLabelEdit(CTreeListCtrl& source, 
							CHANGE_LABEL_NOTIFY_INFO* pclns)
{
	qdMinigameConfigParameter &param = getParameter();
	CString dataCopy(getValueString());
	param.set_data_string(pclns->text);
	if (isValid())
		return true;

	param.set_data_string(dataCopy);
	return false;
}
