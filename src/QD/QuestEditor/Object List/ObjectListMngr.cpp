#include "stdafx.h"
#include "../resource.h"
#include "..\CoolDialogBar.h"
#include ".\objectlistmngr.h"
#include "ObjectListDlg.h"
#include "./ObjectListImpl.h"
#include <boost/rtl.hpp>
#include "qd_game_object.h"


ObjectListMngr::ObjectListMngr(void)
{
}

ObjectListMngr::~ObjectListMngr(void)
{
}

bool ObjectListMngr::create(CFrameWnd* pParent, DWORD dwStyle /*= 
															  WS_CHILD 
															  |CBRS_FLYBY 
															  |CBRS_RIGHT 
															  | CBRS_SIZE_DYNAMIC*/)
{
	parentFrame_ = pParent;

	dlg_.reset(new ObjectListDlg);
	if (!dlg_)
		return false;

	dlgBar_.reset(new CCoolDialogBar);
	if (!dlgBar_)
		return false;

	if (!dlgBar_->Create(pParent, dlg_.get(), dwStyle, ObjectListDlg::IDD))
		return false;

	objectListImpl_.reset(new ObjectListImpl(&dlg_->getList()));
	if (!objectListImpl_)
		return false;
	if (!objectListImpl_->init())
		return false;

	dlg_->setObjectListImpl(objectListImpl_.get());

	dlgBar_->EnableDocking(CBRS_ALIGN_ANY);
	pParent->DockControlBar(dlgBar_.get(), AFX_IDW_DOCKBAR_TOP);
	return true;
}

bool ObjectListMngr::isVisible() const{
	return dlgBar_->IsWindowVisible();
}

void ObjectListMngr::show(){
	parentFrame_->ShowControlBar(dlgBar_.get(), TRUE, FALSE); 
}

void ObjectListMngr::hide(){
	clearList();
	parentFrame_->ShowControlBar(dlgBar_.get(), FALSE, FALSE); 
}

bool ObjectListMngr::showObjectInList(qdNamedObjectReference const* objRef)
{
	return objectListImpl_->showObjectInList(objRef);
}

HITEM ObjectListMngr::addObjectToList(qdNamedObjectReference const* objRef, 
									  HITEM hItemRoot)
{
	return objectListImpl_->addObjectToList(objRef, hItemRoot);
}

bool ObjectListMngr::removeObjectFromList(qdNamedObjectReference const* objRef)
{
	return objectListImpl_->removeObjectFromList(objRef);
}

bool ObjectListMngr::addSubObjectFromList(HITEM hObjectItem, 
										  qdNamedObjectReference const* objRef)
{
	return false;
}

void ObjectListMngr::clearList()
{
	objectListImpl_->clearList();
}

void ObjectListMngr::setListCaption(CString const& str)
{
	if (IsWindow(dlg_->m_hWnd))
		dlgBar_->SetWindowText(str);
}
