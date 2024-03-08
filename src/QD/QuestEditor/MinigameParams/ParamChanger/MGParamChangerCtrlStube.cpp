#include "stdafx.h"
#include ".\mgparamchangerctrlstube.h"

MGParamChangerCtrlStube::MGParamChangerCtrlStube(MGParamChanger* paramChanger)
: paramChanger_(paramChanger)
{
}

MGParamChangerCtrlStube::~MGParamChangerCtrlStube(void)
{
}

void MGParamChangerCtrlStube::setMGParamChanger(MGParamChanger* paramChanger){
	paramChanger_.reset(paramChanger);
}
MGParamChanger* MGParamChangerCtrlStube::getMGParamChanger() const{
	return paramChanger_.get();
}

//! Уведомляет о том, что редактирование закончено
bool MGParamChangerCtrlStube::onEndLabelEdit(CTreeListCtrl& source, 
							CHANGE_LABEL_NOTIFY_INFO* pclns)
{
	return getMGParamChanger()->onEndLabelEdit(source, pclns);
}

//! Вызывается перед началам редактирования текста.
bool MGParamChangerCtrlStube::onBeginLabelEdit(CTreeListCtrl& source, 
										  CHANGE_LABEL_NOTIFY_INFO* pclns)
{
	return (pclns->iCol == 1);
}

bool MGParamChangerCtrlStube::isValid() const{
	return getMGParamChanger()->isValid();
}
qdMinigameConfigParameter const& MGParamChangerCtrlStube::getParameter() const{
	return getMGParamChanger()->getParameter();
}

LPCTSTR MGParamChangerCtrlStube::getValueString() const{
	return getMGParamChanger()->getValueString();
}

LPCTSTR MGParamChangerCtrlStube::getCommentString() const{
	return getMGParamChanger()->getCommentString();
}

LPCTSTR MGParamChangerCtrlStube::getParameterName() const{
	return getMGParamChanger()->getParameterName();
}
