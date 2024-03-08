#include "StdAfx.h"
#include ".\nulltreelistnotifylistener.h"

NullTreeListNotifyListener::NullTreeListNotifyListener(void)
{
}

NullTreeListNotifyListener::~NullTreeListNotifyListener(void)
{
}

void NullTreeListNotifyListener::onGetDispInfo(CTreeListCtrl& tree, TLGETDISPINFO* info)
{

}
void NullTreeListNotifyListener::onSetDispInfo(CTreeListCtrl& tree, TLGETDISPINFO* info)
{

}

bool NullTreeListNotifyListener::onKeyDown(CTreeListCtrl& tree, TLKEYDOWN* info)
{
	return true;
}
void NullTreeListNotifyListener::onGetInfoTip(CTreeListCtrl& tree, TLGETINFOTIP* info)
{

}
