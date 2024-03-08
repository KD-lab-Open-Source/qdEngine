#include "stdafx.h"
#include "resource.h"
#include ".\filemgparamchanger.h"

#include "CurDirSafer.h"
#include "disk_op.h"

#include "Main/QETreeListCtrl.h"

namespace {
	CurDirSafer& getCurDirSafer(){
		static CurDirSafer safer;
		return safer;
	}
}
FileMGParamChanger::FileMGParamChanger(MGParamChanger* paramChanger)
: MGParamChangerCtrlStube(paramChanger)
{
}

FileMGParamChanger::~FileMGParamChanger(void)
{
}

//! Вызывается перед началам редактирования текста.
bool FileMGParamChanger::onBeginLabelEdit(CTreeListCtrl& source, 
							  CHANGE_LABEL_NOTIFY_INFO* pclns)
{
	CString str;
	str.LoadString(IDS_OPEN_ALL_FILES_FILTER);

	CString fileName = disk_op::Get1DiskFileName(getCurDirSafer, str, NULL, NULL);
	if (!fileName.IsEmpty())
	{
		pclns->text = fileName;
		onEndLabelEdit(source, pclns);
	}
	return false;
}

//! Уведомляет о том, что редактирование закончено
bool FileMGParamChanger::onEndLabelEdit(CTreeListCtrl& source, 
											 CHANGE_LABEL_NOTIFY_INFO* pclns)
{
	if (getMGParamChanger()->onEndLabelEdit(source, pclns))
	{
		source.SetItemText(pclns->pItem, pclns->iCol, pclns->text);
		source.SetColumnWidth(pclns->iCol, TLSCW_AUTOSIZE_USEHEADER);
		return true;
	}
	return false;
}
