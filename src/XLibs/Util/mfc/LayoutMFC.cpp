#include "stdafx.h"
#include "LayoutMFC.h"

LayoutWindow::LayoutWindow(CWnd* window)
: window_(window)
, children_(0)
{
}

void LayoutWindow::add(LayoutElement* children)
{
	children_ = children;
	children_->setParent(this);
}

void LayoutWindow::relayout(bool redraw)
{
	if(children_ && window_ && ::IsWindow(window_->GetSafeHwnd())){
		minimalSize_ = children_->minimalSize();

		Recti rect(0, 0, minimalSize_.x, minimalSize_.y);

		CRect clientRect;
		window_->GetClientRect(&clientRect);

		CRect windowPosition;
		window_->GetWindowRect(&windowPosition);
		if(window_->GetParent() && ::IsWindow(window_->GetParent()->GetSafeHwnd()))
			window_->GetParent()->ScreenToClient(&windowPosition);

		if(clientRect.Width() < rect.width()){
			windowPosition.right += rect.width() - clientRect.Width();
			clientRect.right += rect.width() - clientRect.Width();
		}

		if(clientRect.Height() < rect.height()){
			windowPosition.bottom += rect.height() - clientRect.Height();
			clientRect.bottom += rect.height() - clientRect.Height();
		}

		window_->MoveWindow(&windowPosition, FALSE);

		children_->setPosition(Recti(0, 0, clientRect.Width(), clientRect.Height()));

		if(redraw)
			RedrawWindow(window_->GetSafeHwnd(), 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
	}
}
//////////////////////////////////////////////////////////////////////////////
LayoutControl::LayoutControl(CWnd* control, int border)
: window_(control)
, dialog_(0)
, id_(0)
{
	xassert(control && ::IsWindow(control->GetSafeHwnd()));
	border_ = border;
	if(control){
		CRect rt;
		control->GetWindowRect(&rt);
		minimalSize_.set(rt.Width(), rt.Height());
	}
}

LayoutControl::LayoutControl(UINT id, CDialog* dialog, int border)
: window_(0)
, dialog_(dialog)
, id_(id)
{
	xassert(dialog && ::IsWindow(dialog->GetSafeHwnd()));
	border_ = border;

	if(dialog){
		window_ = dialog_->GetDlgItem(id_);
		CRect rt;
		window_->GetWindowRect(&rt);
		minimalSize_.set(rt.Width(), rt.Height());
	}
}

void LayoutControl::setPosition(const Recti& rect)
{
	if(dialog_)
		window_ = dialog_->GetDlgItem(id_);
	if(window_ && ::IsWindow(window_->GetSafeHwnd())){
		CRect rt(rect.left(), rect.top(), rect.right(), rect.bottom());
		window_->MoveWindow(&rt, FALSE);
	}
	else
 		xassert(0);
}
