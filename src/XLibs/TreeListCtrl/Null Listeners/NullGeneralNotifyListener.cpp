#include "StdAfx.h"
#include ".\nullgeneralnotifylistener.h"

NullGeneralNotifyListener::NullGeneralNotifyListener(void)
{
}

NullGeneralNotifyListener::~NullGeneralNotifyListener(void)
{
}

void NullGeneralNotifyListener::onClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info)
{

}
bool NullGeneralNotifyListener::onDBLClick(CTreeListCtrl& tree, 
										   GENERAL_NOTIFY_INFO* info)
{
	return true;
}
bool NullGeneralNotifyListener::onHover(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info)
{
	return true;
}

void NullGeneralNotifyListener::onKillFocus(CTreeListCtrl& tree)
{

}

void NullGeneralNotifyListener::onSetFocus(CTreeListCtrl& tree)
{

}

void NullGeneralNotifyListener::onRClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info)
{
}

void NullGeneralNotifyListener::onRDBLClick(CTreeListCtrl& tree, 
											GENERAL_NOTIFY_INFO* info)
{

}

void NullGeneralNotifyListener::onReleasedCapture(CTreeListCtrl& tree)
{

}

void NullGeneralNotifyListener::onReturn(CTreeListCtrl& tree)
{

}
