#include "stdafx.h"
#include "resource.h"
#include ".\objectmgparamchanger.h"
#include <Main/QETreeListCtrl.h>


std::pair<CString, int> selectGameObjectName(qdGameDispatcher* gameDisp, int count);

ObjectMGParamChanger::ObjectMGParamChanger(
	qdMinigameConfigParameter const& param,
	qdGameDispatcher* gameDisp)
	: MGParamChangerStube(param)
	, gameDisp_(gameDisp)
{
}

ObjectMGParamChanger::~ObjectMGParamChanger(void)
{
}

LPCTSTR ObjectMGParamChanger::getValueString() const{

	
	CString data = getParameter().data_string();
	if (data.IsEmpty())
	{
		data.LoadString(IDS_NOT_SELECTED);
	}
	strData_.Format(IDS_TMPL_MG_PARAM_OBJECT, 
					static_cast<LPCTSTR>(data),
					getParameter().data_count());
	return strData_;
}

//! Вызывается перед началам редактирования текста.
bool ObjectMGParamChanger::onBeginLabelEdit(CTreeListCtrl& source, 
							  CHANGE_LABEL_NOTIFY_INFO* pclns)
{
	if (pclns->iCol != 1)
		return false;

	std::pair<CString, int> res = 
		selectGameObjectName(gameDisp_, getParameter().data_count());
	if (!res.first.IsEmpty())
	{
		std::pair<CString, int> bak = 
			std::make_pair(getParameter().data_string(),
							getParameter().data_count());
		qdMinigameConfigParameter& param = getParameter();
		param.set_data_string(res.first);
		param.set_data_count(res.second);
		if (isValid())
		{
			source.SetItemText(pclns->pItem, pclns->iCol, getValueString());
			source.SetColumnWidth(pclns->iCol, TLSCW_AUTOSIZE_USEHEADER);
		}
		else
		{
			param.set_data_string(bak.first);
			param.set_data_count(bak.second);
		}
	}
	return false;
}
