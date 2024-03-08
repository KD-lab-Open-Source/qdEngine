#include "StdAfx.h"
#include ".\NullChangeItemLabelNotifyListener.h"
#include "../TreeListColumnInfo.h"

NullChangeItemLabelNotifyListener::NullChangeItemLabelNotifyListener(void)
{
}

NullChangeItemLabelNotifyListener::~NullChangeItemLabelNotifyListener(void)
{
}

//! ¬ызываетс€ перед началам редактировани€ текста.
bool NullChangeItemLabelNotifyListener::onBeginLabelEdit(CTreeListCtrl& source, 
							  CHANGE_LABEL_NOTIFY_INFO* info)
{
	return true;
}


//! ”ведомл€ет о том, что редактирование закончено
bool NullChangeItemLabelNotifyListener::onEndLabelEdit(CTreeListCtrl& source, 
							CHANGE_LABEL_NOTIFY_INFO* info)
{
	return false;
}

//! —прашиваем контрол, которым будет редактироватьс€ текст
DWORD NullChangeItemLabelNotifyListener::onRequestCtrl(CTreeListCtrl& source, 
						   CHANGE_LABEL_NOTIFY_INFO* info)
{
	return TLM_STATIC;
}
//! ”ведомление приходит перед тем, как показать контрол редактировани€
/*!
	”ведомление приходит перед тем, как показать контрол редактировани€
	¬ ответ на это пользователь может произвести настройку контрола(вставку 
	необходимх данных) или вообще заменить на свой контрол
	¬озвращаемое значение игнорируетс€ 
*/
bool NullChangeItemLabelNotifyListener::onBeginControl(CTreeListCtrl& source, 
							CHANGE_LABEL_NOTIFY_INFO* info)
{
	return false;
}

//! ”ведомление приходит после закрыти€ контрола
void NullChangeItemLabelNotifyListener::onEndControl(CTreeListCtrl& source, 
						  CHANGE_LABEL_NOTIFY_INFO* info)
{
}
