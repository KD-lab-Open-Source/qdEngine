#pragma once
#include "mgparamchangerctrlstube.h"

/*!
	�����-������ ���������� ��������� ����� CFileDialog
*/
class FileMGParamChanger : public MGParamChangerCtrlStube
{
public:
	FileMGParamChanger(MGParamChanger* paramChanger);
	~FileMGParamChanger(void);
	//! ���������� ����� ������� �������������� ������.
	bool onBeginLabelEdit(CTreeListCtrl& source, CHANGE_LABEL_NOTIFY_INFO* pclns);
	//! ���������� � ���, ��� �������������� ���������
	bool onEndLabelEdit(CTreeListCtrl& source, CHANGE_LABEL_NOTIFY_INFO* pclns);
private:
};
