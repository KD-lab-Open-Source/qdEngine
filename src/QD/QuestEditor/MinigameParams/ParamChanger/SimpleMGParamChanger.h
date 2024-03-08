#pragma once
#include "mgparamchangerstube.h"

/*!
 Класс для редактирования параметров, которые можно редактировать обычной строкой
*/
class SimpleMGParamChanger :public MGParamChangerStube
{
public:
	SimpleMGParamChanger(qdMinigameConfigParameter const& param);
	~SimpleMGParamChanger(void);
	//! Уведомляет о том, что редактирование закончено
	virtual bool onEndLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);
};
