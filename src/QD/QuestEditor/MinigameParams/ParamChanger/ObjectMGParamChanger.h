#pragma once
#include "mgparamchangerstube.h"

class qdGameDispatcher;
/*!
    �����-������ ��� ���������	data_type_t::PRM_DATA_OBJECT
*/
class ObjectMGParamChanger :
	public MGParamChangerStube
{
public:
	ObjectMGParamChanger(qdMinigameConfigParameter const& param,
								qdGameDispatcher* gameDisp);
	~ObjectMGParamChanger(void);

	virtual LPCTSTR getValueString() const;
	//! ���������� ����� ������� �������������� ������.
	virtual bool onBeginLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);
private:
	mutable CString strData_;
	qdGameDispatcher* gameDisp_;
};
