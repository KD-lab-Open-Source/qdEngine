#pragma once
#include "mgparamchanger.h"

#include <boost/scoped_ptr.hpp>

/*!
    �����, ��������������� ���������� �� ���������, ��� �������-��������, ����������
	������ ������ �������-��������. (EditMGParamChanger, FileMGParamChanger)
*/
class MGParamChangerCtrlStube : public MGParamChanger
{
public:
	MGParamChangerCtrlStube(MGParamChanger* paramChanger);
	~MGParamChangerCtrlStube(void);
	//! ���������� � ���, ��� �������������� ���������
	virtual bool onEndLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);
	//! ���������� ����� ������� �������������� ������.
	virtual bool onBeginLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);

	virtual bool isValid() const;
	virtual qdMinigameConfigParameter const& getParameter() const;
	virtual LPCTSTR getValueString() const;
	virtual LPCTSTR getCommentString() const;
	virtual LPCTSTR getParameterName() const;
protected:
	void setMGParamChanger(MGParamChanger* paramChanger);
	MGParamChanger* getMGParamChanger() const;
private:
	boost::scoped_ptr<MGParamChanger> paramChanger_;
};
