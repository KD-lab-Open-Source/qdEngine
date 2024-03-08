#include "StdAfx.h"
#include "kdw/ScrolledWindow.h"
#include "kdw/_WidgetWindow.h"
#include "ExcludeClipRectVisitor.h"

#include "kdw/Serialization.h"
#include "Serialization/SerializationFactory.h"


namespace kdw{

REGISTER_CLASS(Widget, ScrolledWindow, "Разметка\\Окно прокрутки")
REGISTER_CLASS(Container, ScrolledWindow, "Окно прокрутки")

class ScrolledWindowImpl: public _ContainerWindow{
public:
	ScrolledWindowImpl(ScrolledWindow* owner);
	BOOL onMessageSize(UINT type, USHORT width, USHORT height);
	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);

	void updateScrollBars();
protected:
	kdw::ScrolledWindow* owner_;
};

ScrolledWindowImpl::ScrolledWindowImpl(ScrolledWindow* owner)
: _ContainerWindow(owner)
, owner_(owner)
{
	VERIFY(create("scrolledWindow", WS_CHILD | WS_TABSTOP | WS_HSCROLL | WS_VSCROLL | WS_CLIPCHILDREN, Recti(0, 0, 40, 40), *Win32::_globalDummyWindow));
}


BOOL ScrolledWindowImpl::onMessageSize(UINT type, USHORT width, USHORT height)
{
	if(!creating())
		updateScrollBars();
	return TRUE;
}

LRESULT ScrolledWindowImpl::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message){
	case WM_ERASEBKGND:
		{
			HDC dc = HDC(wparam);
			ExcludeClipRectVisitor visitor(dc);
			owner_->visitChildren(visitor);
		}
		return Win32::Window32::onMessage(message, wparam, lparam);
	case WM_HSCROLL:
	case WM_VSCROLL:
		{
			SCROLLINFO info;
			info.cbSize = sizeof(info);
			info.fMask  = SIF_ALL;
			::GetScrollInfo(handle_, message == WM_VSCROLL ? SB_VERT : SB_HORZ, &info);
			int oldPosition = info.nPos;
			switch(LOWORD(wparam)){
			case SB_TOP:        info.nPos = info.nMin; break;
			case SB_BOTTOM:     info.nPos = info.nMax; break;
			case SB_LINEUP:     info.nPos -= 1; break;
			case SB_LINEDOWN:   info.nPos += 1; break;
			case SB_PAGEUP:     info.nPos -= info.nPage; break;
			case SB_PAGEDOWN:   info.nPos += info.nPage; break;
			case SB_THUMBTRACK: info.nPos = info.nTrackPos; break;
			default:
				break; 
			}

			info.fMask = SIF_POS;

			if(message == WM_VSCROLL){
				::SetScrollInfo(handle_, SB_VERT, &info, TRUE);
				::GetScrollInfo(handle_, SB_VERT, &info);
				owner_->offset_.y = int(info.nPos);
			}
			else{
				::SetScrollInfo(handle_, SB_HORZ, &info, TRUE);
				::GetScrollInfo(handle_, SB_HORZ, &info);
				owner_->offset_.x = int(info.nPos);
			}

			if(info.nPos != oldPosition){                    
				owner_->_arrangeChildren();
				updateScrollBars();
			}
		}
		return 0;
	default:
		break;
	}
	return Win32::Window32::onMessage(message, wparam, lparam);
}




void ScrolledWindowImpl::updateScrollBars()
{
	kdw::Widget* widget = owner_->child_;
	if(widget){
		SCROLLINFO scrollInfo;

		memset((void*)&scrollInfo, 0, sizeof(scrollInfo));
		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
		scrollInfo.nMin = 0;
		if(owner_->policyVertical_ == kdw::SCROLL_NEVER){
			scrollInfo.nMax = 0;
			scrollInfo.nPos = 1;
		}
		else{
			scrollInfo.nMax = widget->_minimalSize().y;
			scrollInfo.nPos = owner_->offset_.y;
		}
		scrollInfo.nPage = owner_->clientAreaSize().y;
		SetScrollInfo(handle_, SB_VERT, &scrollInfo, TRUE);

		memset((void*)&scrollInfo, 0, sizeof(scrollInfo));
		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
		scrollInfo.nMin = 0;
		if(owner_->policyHorizontal_ == kdw::SCROLL_NEVER){
			scrollInfo.nMax = 0;
			scrollInfo.nPos = 1;
		}
		else{
			scrollInfo.nMax = widget->_minimalSize().x;
			scrollInfo.nPos = owner_->offset_.x;
		}
		scrollInfo.nPage = owner_->clientAreaSize().x;
		SetScrollInfo(handle_, SB_HORZ, &scrollInfo, TRUE);
	}
}

// ------------------------------------------------------------------


#pragma warning(push)
#pragma warning(disable: 4355) //  'this' : used in base member initializer list
ScrolledWindow::ScrolledWindow(int border)
: _ContainerWithWindow(new ScrolledWindowImpl(this), border)
, offset_(0, 0)
, policyHorizontal_(SCROLL_AUTOMATIC)
, policyVertical_(SCROLL_AUTOMATIC)
{
	setBorder(border);
	_setMinimalSize(Vect2i(0, 0));
}
#pragma warning(pop)

ScrolledWindow::~ScrolledWindow()
{
	if(child_)
		child_->_setParent(0);
}

void ScrolledWindow::setPolicy(ScrollPolicy horizontalPolicy, ScrollPolicy verticalPolicy)
{
	policyHorizontal_ = horizontalPolicy;
	policyVertical_ = verticalPolicy;
}

void ScrolledWindow::add(Widget* widget)
{	
	child_ = widget;
	widget->_setParent(this);
	_arrangeChildren();
	impl()->updateScrollBars();
}

void ScrolledWindow::remove()
{
	child_ = 0;
}

void ScrolledWindow::visitChildren(WidgetVisitor& visitor) const
{
	if(child_)
		visitor(*child_);
}

void ScrolledWindow::_arrangeChildren()
{
	if(child_){
		Vect2i clientSize = clientAreaSize();
		Vect2i size(max(child_->_minimalSize().x, clientSize.x), max(child_->_minimalSize().y, clientSize.y));
		offset_.set(min(offset_.x, max(0, child_->_minimalSize().x - clientSize.x)), min(offset_.y, max(0, child_->_minimalSize().y - clientSize.y)));

		Recti pos(-offset_, size);
		child_->_setPosition(pos);
	}
}

void ScrolledWindow::_relayoutParents()
{
	Vect2i oldMinimalSize = _minimalSize();
	int h = (policyHorizontal_ == SCROLL_NEVER && child_)
		? child_->_minimalSize().x + GetSystemMetrics(SM_CXVSCROLL)
		: GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXHSCROLL) * 2;
	int v = (policyVertical_ == SCROLL_NEVER && child_)
		? child_->_minimalSize().x + GetSystemMetrics(SM_CYHSCROLL)
		: GetSystemMetrics(SM_CYHSCROLL) + GetSystemMetrics(SM_CYVSCROLL) * 2;
	_setMinimalSize(Vect2i(h + border_ * 2, v + border_ * 2));
	impl()->updateScrollBars();
	_arrangeChildren();
	Container::_relayoutParents(oldMinimalSize != _minimalSize());
}

ScrolledWindowImpl* ScrolledWindow::impl()
{
	return static_cast<ScrolledWindowImpl*>(_window());
}

Vect2i ScrolledWindow::clientAreaSize() const
{
	Vect2i result(_position().width() - border() * 2, _position().height() - border() * 2);
	if(policyHorizontal_ == SCROLL_ALWAYS){
		result.y -= GetSystemMetrics(SM_CYHSCROLL);
	}
	else{
		if(policyVertical_ == SCROLL_AUTOMATIC && child_ && child_->_minimalSize().y > result.y){
			result.x -= GetSystemMetrics(SM_CXVSCROLL);
		}
	}
	if(policyVertical_ == SCROLL_ALWAYS){
		result.x -= GetSystemMetrics(SM_CXVSCROLL);
	}
	else{
		if(policyHorizontal_ == SCROLL_AUTOMATIC && child_ && child_->_minimalSize().x > result.x){
			result.y -= GetSystemMetrics(SM_CYVSCROLL);
		}
	}
	return result;	
}

bool ScrolledWindow::horizontalScrollVisible() const
{
	return true;
}

bool ScrolledWindow::verticalScrollVisible() const
{
	return true;
}

Widget* ScrolledWindow::_nextWidget(Widget* last, FocusDirection direction)
{
	switch(direction)
	{
	case FOCUS_NEXT:
	case FOCUS_PREVIOUS:
		if(last == child_)
			return 0;
		else
			return child_;
	case FOCUS_FIRST:
	case FOCUS_LAST:
		if(last == child_)
			return 0;
		else
			return child_;
	default:
		return 0;
	}
}

void ScrolledWindow::serialize(Archive& ar)
{
    ar.serialize(child_, "widget", "Контрол");
}

}
