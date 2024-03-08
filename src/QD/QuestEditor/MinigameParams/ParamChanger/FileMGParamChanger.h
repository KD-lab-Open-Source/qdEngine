#pragma once
#include "mgparamchangerctrlstube.h"

/*!
	Класс-ченжер получающий изменения через CFileDialog
*/
class FileMGParamChanger : public MGParamChangerCtrlStube
{
public:
	FileMGParamChanger(MGParamChanger* paramChanger);
	~FileMGParamChanger(void);
	//! Вызывается перед началам редактирования текста.
	bool onBeginLabelEdit(CTreeListCtrl& source, CHANGE_LABEL_NOTIFY_INFO* pclns);
	//! Уведомляет о том, что редактирование закончено
	bool onEndLabelEdit(CTreeListCtrl& source, CHANGE_LABEL_NOTIFY_INFO* pclns);
private:
};
