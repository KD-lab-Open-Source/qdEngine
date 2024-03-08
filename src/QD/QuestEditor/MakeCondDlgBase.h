#ifndef _MAKE_COND_DLG_BASE_
#define _MAKE_COND_DLG_BASE_

#pragma once

/*!
 ��������� ��� ������-�������, � ������� �������� �������� �������.(StateCondDlg 
 � CO_ConditionDlg)
*/

class MakeCondDlgBase{
public:
	//! ���������� �� ��������, ������������� ������ ��� �������
	virtual void OnCondDataComplete(bool bComplete) = 0;
	//! ���������� �����, � ������� ��������� ������, ������� �������� �������������
	virtual class qdGameScene* get_scene() const = 0;
	//! ���������� ��������� ����
	virtual class qdGameDispatcher* get_disp() const = 0;
};

#endif
