#pragma once
#include "MiniGameParams/ParamChanger/mgparamchangerctrlstube.h"
/*!
    �����-������ ���������� ��������� ����� Edit-�������
*/
class EditMGParamChanger :
	public MGParamChangerCtrlStube
{
public:
	EditMGParamChanger(MGParamChanger* parameterChanger);
	~EditMGParamChanger(void);

	//! ���������� ��� ��������, ������� ����� ��������������� �����
	virtual DWORD onRequestCtrl(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);

private:
	MGParamChanger* parameterChanger_;
};
