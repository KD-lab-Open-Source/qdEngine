#pragma once
#include "mgparamchangerstube.h"

/*!
 ����� ��� �������������� ����������, ������� ����� ������������� ������� �������
*/
class SimpleMGParamChanger :public MGParamChangerStube
{
public:
	SimpleMGParamChanger(qdMinigameConfigParameter const& param);
	~SimpleMGParamChanger(void);
	//! ���������� � ���, ��� �������������� ���������
	virtual bool onEndLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);
};
