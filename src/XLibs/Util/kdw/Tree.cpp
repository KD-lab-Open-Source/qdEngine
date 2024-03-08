#include "StdAfx.h"
#include <string>
#include <algorithm>
#include "kdw/Tree.h"
#include "kdw/TreeModel.h"
#include "kdw/TreeView.h"
#include "kdw/Window.h"
#include "kdw/DragManager.h"

#include "ExcludeClipRectVisitor.h"

#include "kdw/_WidgetWindow.h"
#include "kdw/Win32/Handle.h"
#include "kdw/Win32/MemoryDC.h"

#include "kdw/Serialization.h"
#include "Serialization/SerializationFactory.h"
#include "kdw/ImageStore.h"
#include <CommCtrl.h>
#include <windows.h>


namespace kdw{

REGISTER_CLASS(Widget, Tree, "Дерево");

KDW_API DragManager dragManager;

class TreeImpl;

class DragWindow : public Win32::Window32{
public:
	const char* className() const{ return "kdw.DragWindow"; }
	DragWindow(TreeImpl* treeImpl);

	void set(TreeImpl* treeImpl, TreeRow* row, const Recti& rowRect);
	void show();
	void hide();
	void move(int deltaX, int deltaY);

	void onMessagePaint();
	BOOL onMessageEraseBkgnd(HDC dc);
protected:
	void setWindowPos(bool visible);
	void drawRow(HDC dc);
	void updateLayeredWindow();

	bool useLayeredWindows_;
	TreeRow* row_;
	Recti rect_;
	TreeImpl* treeImpl_;
	Vect2i offset_;
	HBITMAP bitmap_;
	COLORREF* bitmapBits_;
};

// ---------------------------------------------------------------------------

class DragController{
public:
	DragController(TreeImpl* treeImpl);
	void beginDrag(TreeRow* row, POINT startPoint);
    bool dragOn(POINT screenPoint);
	void drop(POINT screenPoint);
	
	void drawUnder(HDC dc);
	void drawOver(HDC dc);
	void interrupt();
	bool trackRow(POINT point, TreeRow*& hoveredRow, TreeRow*& destination, TreeRow*& beforeRow);
	bool captured() const{ return captured_; }
protected:
	DragWindow window_;
	TreeImpl* treeImpl_;
	TreeRow* row_;
	TreeRow* hoveredRow_;
	TreeRow* destinationRow_;
	TreeRow* beforeRow_;
	POINT startPoint_;
	POINT lastPoint_;
	bool captured_;
	bool dragging_;
};


// ---------------------------------------------------------------------------

class TreeImpl: public _ContainerWindow{
public:
	TreeImpl(kdw::Tree* owner);
	~TreeImpl();

	TreeModel* model() { return owner_->model(); }
	const char* className() const { return "kdw.TreeImpl"; }

	void redraw(HDC dc);
	TreeHitTest hitTest(TreeRow* row, Vect2i pt, const RECT& rowRect, int& column);
	bool getRowRect(TreeRow* row, Recti& rect, bool onlyVisible = true);
	bool toggleRow(TreeRow* node);
	void updateScrollBar();
	void ensureVisible(TreeRow* row, bool update = true);


	int onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags);

	void onMessageMouseWheel(SHORT delta);
	void onMessageMouseMove(UINT button, int x, int y);

	void onMessageLButtonDblClk(int x, int y);
	void onMessageLButtonDown(UINT button, int x, int y);
	void onMessageLButtonUp(UINT button, int x, int y);
	void onMessageMButtonDown(UINT button, int x, int y);
	void onMessageRButtonDown(UINT button, int x, int y);

	BOOL onMessageEraseBkgnd(HDC dc);
	void onMessageScroll(UINT message, WORD type);
	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);
	BOOL onMessageSize(UINT type, USHORT width, USHORT height);

	int onMessageKillFocus(HWND newFocusedWindow);
	int onMessageSetFocus(HWND lastFocusedWindow);

	void onMessagePaint();

	TreeRow* rowByPoint(Vect2i point, RECT& outRowRect);
	TreeRow* hoveredRow() { return hoveredRow_; }
	bool hoverPushed() const{ return hoveredRow_ && hoverPushed_; };
	kdw::Tree* owner(){ return owner_; };
protected:
	friend class kdw::Tree;	
	int totalHeight_;
	int scrollOffset_;
	RECT area_;
	kdw::Tree* owner_;
	kdw::TreeRow* hoveredRow_;
	kdw::TreeRow* capturedRow_;
	bool hoverPushed_;
	DragController drag_;
};

// ---------------------------------------------------------------------------
//

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list
DragWindow::DragWindow(TreeImpl* treeImpl)
: useLayeredWindows_(true)
, treeImpl_(treeImpl)
, offset_(0, 0)

{
	DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
	if(useLayeredWindows_){
		exStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT;
		VERIFY(create("", WS_POPUP | WS_DISABLED, Recti(0, 0, 320, 320), 0, exStyle));
	}
	else{
		Recti treeRect(0, 0, 100, 100);
		VERIFY(create("", WS_POPUP | WS_DISABLED, treeRect, 0, exStyle));
	}
}
#pragma warning(pop)

void DragWindow::set(TreeImpl* treeImpl, TreeRow* row, const Recti& rowRect)
{
	RECT rect;
	GetClientRect(*treeImpl_, &rect);
	ClientToScreen(*treeImpl_, (POINT*)&rect);
	ClientToScreen(*treeImpl_, (POINT*)&rect + 1);

	offset_.x = rect.left;
	offset_.y = rect.top;

	row_ = row;
	rect_ = rowRect;
}

void DragWindow::setWindowPos(bool visible)
{
	SetWindowPos(*this, HWND_TOP, rect_.left() + offset_.x,  rect_.top() + offset_.y,
				 rect_.width(), rect_.height(), 
				 SWP_NOOWNERZORDER | (visible ? SWP_SHOWWINDOW : SWP_HIDEWINDOW)| SWP_NOACTIVATE);
}

void DragWindow::show()
{
	if(useLayeredWindows_){
		updateLayeredWindow();
		setWindowPos(true);
		updateLayeredWindow();
	}
	else{
		setWindowPos(true);
	}
}

void DragWindow::move(int deltaX, int deltaY)
{
	offset_ += Vect2i(deltaX, deltaY);
	setWindowPos(::IsWindowVisible(*this) ? true : false);
}

void DragWindow::hide()
{
	setWindowPos(false);
}

void DragWindow::drawRow(HDC dc)
{
	const Tree* tree = treeImpl_->owner();
	const TreeColumns& columns = tree->columns();
	TreeColumns::const_iterator it;
	int columnIndex = 0;
	RECT entireRowRect;
	entireRowRect.left = 0;
	entireRowRect.top = 0;
	entireRowRect.right = rect_.width();
	entireRowRect.bottom = rect_.height();

	HGDIOBJ oldBrush = ::SelectObject(dc, GetSysColorBrush(COLOR_BTNFACE));
	HGDIOBJ oldPen = ::SelectObject(dc, GetStockObject(BLACK_PEN));
	Rectangle(dc, entireRowRect.left, entireRowRect.top, entireRowRect.right, entireRowRect.bottom);
	::SelectObject(dc, oldBrush);
	::SelectObject(dc, oldPen);

	for(it = columns.begin(); it != columns.end(); ++it, ++columnIndex){
		const TreeColumn& column = columns[columnIndex];
		if(const kdw::TreeColumnDrawer* drawer = it->drawer()){
			drawer->drawRow(row_, columnIndex, dc, entireRowRect, row_->indent(),
							false, false, false);
		}
		else{
			HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
			FillRect(dc, &entireRowRect, brush);
			DeleteObject(brush);
		}
	}
}

BOOL DragWindow::onMessageEraseBkgnd(HDC dc)
{
	return useLayeredWindows_ ? TRUE : FALSE;
	return FALSE;
}

void DragWindow::updateLayeredWindow()
{
	BLENDFUNCTION blendFunction;
	blendFunction.BlendOp = AC_SRC_OVER;
	blendFunction.BlendFlags = 0;
	blendFunction.SourceConstantAlpha = 192;
	blendFunction.AlphaFormat = 0;	

	PAINTSTRUCT ps;
	HDC realDC = BeginPaint(*this, &ps);
	{
		Win32::MemoryDC dc(realDC);
		drawRow(dc);

		POINT leftTop = { rect_.left() + offset_.x, rect_.top() + offset_.y };
		SIZE size = { rect_.width(), rect_.height() };
		POINT point = { 0, 0 };
		UpdateLayeredWindow(*this, realDC, &leftTop, &size, dc, &point, 0, &blendFunction, ULW_ALPHA);
		SetLayeredWindowAttributes(*this, 0, 64, ULW_ALPHA);
	}
	EndPaint(*this, &ps);
}

void DragWindow::onMessagePaint()
{
	if(useLayeredWindows_)
		return;

	PAINTSTRUCT ps;
	HDC dc = BeginPaint(handle_, &ps);
	{
		Win32::MemoryDC memoryDC(dc);
		drawRow(memoryDC);
	}
	EndPaint(handle_, &ps);
}

// ---------------------------------------------------------------------------

DragController::DragController(TreeImpl* treeImpl)
: treeImpl_(treeImpl)
, captured_(false)
, dragging_(false)
, row_(0)
, window_(treeImpl)
, hoveredRow_(0)
, destinationRow_(0)
, beforeRow_(0)
{
}

void DragController::beginDrag(TreeRow* row, POINT pt)
{
	row_ = row;
	startPoint_ = pt;
	lastPoint_ = pt;
	captured_ = true;
	dragging_ = false;
}

bool DragController::dragOn(POINT screenPoint)
{
	window_.move(screenPoint.x - lastPoint_.x, screenPoint.y - lastPoint_.y);

	bool needCapture = false;
	if(!dragging_ && (Vect2i(startPoint_.x, startPoint_.y) - Vect2i(screenPoint.x, screenPoint.y)).norm2() >= 25)
		if(row_->canBeDragged()){
			needCapture = true;
			Recti rect;
			treeImpl_->getRowRect(row_, rect);
			window_.set(treeImpl_, row_, rect);
			window_.show();
			dragging_ = true;
		}

	if(dragging_){
		POINT point = screenPoint;
		ScreenToClient(*treeImpl_, &point);
		if(!trackRow(point, hoveredRow_, destinationRow_, beforeRow_)){
			hoveredRow_ = 0;
			destinationRow_ = 0;
			beforeRow_ = 0;
		}
	}
	lastPoint_ = screenPoint;
	return needCapture;
}

void DragController::interrupt()
{
	captured_ = false;
	dragging_ = false;
	row_ = 0;
	window_.hide();
}

TreeRow* nextRow(TreeRow* row)
{
	xassert(row);
	TreeRow* parent = row->parent();
	if(!parent)
		return 0;
	int index = parent->childrenIndex(row) + 1;
	if(index == parent->size())
		return 0;
	else
		return parent->childrenByIndex(index);
}

bool DragController::trackRow(POINT point, TreeRow*& hoveredRow, TreeRow*& destination, TreeRow*& beforeRow)
{
	RECT rectRow;
	TreeRow* row = treeImpl_->rowByPoint(Vect2i(point.x, point.y), rectRow);
	float pos;
	hoveredRow = row;
	destination = 0;
	beforeRow = 0;
	TreeModel* model = treeImpl_->owner()->model();
	Tree* tree = treeImpl_->owner();
	if(row){
		pos = (point.y - rectRow.top) / float(rectRow.bottom - rectRow.top);
		float previousBound = 0.25f;
		float nextBound = 0.75f;
		if(!row_->canBeDroppedOn(row, 0, tree, true)){
			previousBound = nextBound = 0.5f;
			if(!row->parent()){
				return false;
			}
		}
		if(pos < previousBound && row->parent()){
			if(row_->canBeDroppedOn(row->parent(), row, tree, false)){
				destination = row->parent();
				beforeRow = row;
			}
		}
		else if(pos >= nextBound && row->parent()){
			if(row->expanded()){
				if(row_->canBeDroppedOn(row->parent(), row, tree, false)){
					destination = row->parent();
					beforeRow = row;
				}
			}
			else{
				TreeRow* next = nextRow(row);
				if(row_->canBeDroppedOn(row->parent(), next, tree, false)){
					destination = row->parent();
					beforeRow = next;
				}
			}
		}
		else if(pos >= previousBound && pos < nextBound){
			destination = row;
			beforeRow = 0;
		}
		if(destination == row_ || beforeRow == row_/* || (beforeRow && nextRow(row_) == beforeRow)*/){
			hoveredRow = 0;
			destination = 0;
			beforeRow = 0;
		}

		return destination != 0;
	}
	return false;
}

static void adjustHoveredRect(TreeRow* row, Recti& hoveredRect)
{
	int delta = row->indent() + row->textIndent() - 2;
	hoveredRect.left(hoveredRect.left() + delta);
	hoveredRect.width(hoveredRect.width() - delta);

}

void DragController::drawUnder(HDC dc)
{
	if(!dragging_)
		return;

	if(hoveredRow_ == destinationRow_ && beforeRow_ == 0 && hoveredRow_ && hoveredRow_ != treeImpl_->owner()->model()->root()){
		Recti rowRect;
		treeImpl_->getRowRect(hoveredRow_, rowRect);
		adjustHoveredRect(hoveredRow_, rowRect);

		FillRect(dc, &Win32::Rect(rowRect), GetSysColorBrush(COLOR_HIGHLIGHT));
	}
}


void DragController::drawOver(HDC dc)
{
	if(!dragging_)
		return;
	Recti rowRect;
	treeImpl_->getRowRect(row_, rowRect);

	{
		Win32::StockSelector brush(dc, GetSysColorBrush(treeImpl_->owner()->flat() ? COLOR_BTNFACE : COLOR_WINDOW));
		Win32::StockSelector pen(dc, GetStockObject(NULL_PEN));
		Rectangle(dc, rowRect.left() - 1, rowRect.top() - 1, rowRect.right() + 3, rowRect.bottom() + 1);
	}

	Recti hoveredRect;
	const int tickSize = 4;
	if(destinationRow_ != hoveredRow_){
		if(hoveredRow_){
			treeImpl_->getRowRect(hoveredRow_, hoveredRect);
			adjustHoveredRect(hoveredRow_, hoveredRect);

			if(nextRow(hoveredRow_) == beforeRow_){ // previous
				RECT rect = { hoveredRect.left() - 1 , hoveredRect.bottom() - 1, hoveredRect.right() + 1, hoveredRect.bottom() + 1 };
				RECT rectLeft = { hoveredRect.left() - 1 , hoveredRect.bottom() - tickSize, hoveredRect.left() + 1, hoveredRect.bottom() + tickSize };
				RECT rectRight = { hoveredRect.right() - 1 , hoveredRect.bottom() - tickSize, hoveredRect.right() + 1, hoveredRect.bottom() + tickSize };
				FillRect(dc, &rect, GetSysColorBrush(COLOR_HIGHLIGHT));
				FillRect(dc, &rectLeft, GetSysColorBrush(COLOR_HIGHLIGHT));
				FillRect(dc, &rectRight, GetSysColorBrush(COLOR_HIGHLIGHT));
			}
			else{ // next
				RECT rect = { hoveredRect.left() - 1 , hoveredRect.top() - 1, hoveredRect.right() + 1, hoveredRect.top() + 1 };
				RECT rectLeft = { hoveredRect.left() - 1 , hoveredRect.top() - tickSize, hoveredRect.left() + 1, hoveredRect.top() + tickSize };
				RECT rectRight = { hoveredRect.right() - 1 , hoveredRect.top() - tickSize, hoveredRect.right() + 1, hoveredRect.top() + tickSize };
				FillRect(dc, &rect, GetSysColorBrush(COLOR_HIGHLIGHT));
				FillRect(dc, &rectLeft, GetSysColorBrush(COLOR_HIGHLIGHT));
				FillRect(dc, &rectRight, GetSysColorBrush(COLOR_HIGHLIGHT));
			}
		}
	}	
}

void DragController::drop(POINT screenPoint)
{
	TreeModel* model = treeImpl_->owner()->model();
	if(hoveredRow_){
		xassert(destinationRow_);
		row_->dropInto(destinationRow_, beforeRow_, treeImpl_->owner());
	}

	captured_ = false;
	dragging_ = false;
	row_ = 0;
	window_.hide();
	hoveredRow_ = 0;
	destinationRow_ = 0;
	beforeRow_ = 0;
}


// ---------------------------------------------------------------------------


#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list
TreeImpl::TreeImpl(kdw::Tree* owner)
: _ContainerWindow(owner)
, owner_(owner)
, totalHeight_(0)
, scrollOffset_(0)
, hoveredRow_(0)
, hoverPushed_(false)
, capturedRow_(0)
, drag_(this)
{
	area_.left = 0;
	area_.top = 0;
	area_.right = 0;
	area_.bottom = 0;
	VERIFY(create("", WS_CHILD | WS_TABSTOP | WS_VSCROLL, Recti(0, 0, 40, 40), *Win32::_globalDummyWindow));
}
#pragma warning(pop)


TreeImpl::~TreeImpl()
{
	owner_ = 0;
}

int TreeImpl::onMessageKillFocus(HWND newFocusedWindow)
{
	RedrawWindow(handle_, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
	return Window32::onMessageKillFocus(newFocusedWindow);
}

int TreeImpl::onMessageSetFocus(HWND lastFocusedWindow)
{
	bool enabled = ::IsWindowEnabled(*this);
	xassert(enabled);
	if(!creating()){
		kdw::Widget::_setFocusedWidget(owner_);
		RedrawWindow(handle_, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
	}
	return Window32::onMessageSetFocus(lastFocusedWindow);
}

void TreeImpl::onMessageMouseWheel(SHORT delta)
{
	scrollOffset_ = scrollOffset_ - delta;
	updateScrollBar();
	owner_->_arrangeChildren();
	RedrawWindow(handle_, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
	__super::onMessageMouseWheel(delta);
}

void TreeImpl::onMessageLButtonDblClk(int x, int y)
{
	::SetFocus(handle_);

	Vect2i point(x, y);
	RECT rowRect;
	TreeRow* row = rowByPoint(point, rowRect);
	Recti rowRecti(rowRect.left, rowRect.top, rowRect.right - rowRect.left, rowRect.bottom - rowRect.top);
	if(!owner_->onRowLMBDoubleClick(row, rowRecti, Vect2i(x, y)) && row){
        int column = 0;		
		
		xassert(row->numRef() > 0);
		if(owner_->hitActivationRect(row, point, rowRecti, column)){
			if(!model()->activateRow(owner_, row, column))
				toggleRow(row);	
		}
		else{
			xassert(row->numRef() > 0);
			if(!toggleRow(row))
				model()->activateRow(owner_, row, column);
		}
	}
	__super::onMessageLButtonDblClk(x, y);
}

void TreeImpl::onMessageLButtonUp(UINT button, int x, int y)
{
	ASSERT(::IsWindow(handle_));
	HWND focusedWindow = GetFocus();
	if(focusedWindow){
		ASSERT(::IsWindow(focusedWindow));
		Win32::Window32 wnd(focusedWindow);
		if(Win32::Window32* window = reinterpret_cast<Win32::Window32*>(wnd.getUserDataLongPtr())){
			ASSERT(*window == focusedWindow);
		}
	}
	//::SetFocus(handle_);

	if(drag_.captured()){
		POINT pos;
		GetCursorPos(&pos);
		if(GetCapture() == *this)
			ReleaseCapture();
		drag_.drop(pos);
		RedrawWindow(handle_, 0, 0, RDW_INVALIDATE);
	}
	Vect2i point(x, y);
	RECT rowRect;
	TreeRow* row = rowByPoint(point, rowRect);
	if(row){
		TreeModel* model = owner_->model();
		int column = 0;
		switch(hitTest(row, point, rowRect, column)){
		case TREE_HIT_ROW:
			if(hoveredRow_){
				hoverPushed_ = false;
				RedrawWindow(handle_, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
			}
		}			
	}
	if(capturedRow_){
		Recti rowRecti;
		getRowRect(capturedRow_, rowRecti);
		owner_->onRowLMBUp(capturedRow_, rowRecti, Vect2i(x, y));
		capturedRow_ = 0;
	}
	
	dragManager.drop();

	__super::onMessageLButtonUp(button, x, y);
}

void TreeImpl::onMessageLButtonDown(UINT button, int x, int y)
{
	ASSERT(::IsWindow(handle_));
	::SetFocus(handle_);
	Vect2i point(x, y);
	RECT rowRect;
	TreeRow* row = rowByPoint(point, rowRect);
	if(row){
		TreeModel* model = owner_->model();
		int column = 0;

		Recti rowRecti(rowRect.left, rowRect.top, rowRect.right - rowRect.left, rowRect.bottom - rowRect.top);
		if(owner_->onRowLMBDown(row, rowRecti, Vect2i(x, y)))
			capturedRow_ = row;
		else{
			// row могла уже быть пересоздана	
			row = rowByPoint(point, rowRect);
			//if(!owner_->hitActivationRect(row, point, rowRecti, column)){
				POINT cursorPos;
				GetCursorPos(&cursorPos);
				drag_.beginDrag(row, cursorPos);
				row->onDrag(dragManager);
			//}
		}
	}

}

void TreeImpl::onMessageRButtonDown(UINT button, int x, int y)
{
	ASSERT(::IsWindow(handle_));
	::SetFocus(handle_);

	Vect2i point(x, y);
	RECT rowRect;
	TreeRow* row = rowByPoint(point, rowRect);
	if(row){
		owner_->model()->setFocusedRow(row);
		RedrawWindow(handle_, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);

		owner_->onRowRMBDown(row, Recti(rowRect.left, rowRect.top, rowRect.right - rowRect.left, rowRect.bottom - rowRect.top), point);
	}
	else{
		Win32::Rect rect;
		GetClientRect(*this, &rect);
		owner_->onRowRMBDown(model()->root(), rect.recti(), point);
	}
	__super::onMessageRButtonDown(button, x, y);
}

void TreeImpl::onMessageMButtonDown(UINT button, int x, int y)
{
	::SetFocus(handle_);

	Vect2i point(x, y);
	RECT rowRect;
	TreeRow* row = rowByPoint(point, rowRect);
	if(row){
		TreeColumns::iterator it;
		int column = 0;
		for(it = owner_->columns().begin(); it != owner_->columns().end(); ++it){
			TreeColumnDrawer* drawer = it->drawer();
			switch(drawer->hitTest(row, point, rowRect)){
			case TREE_HIT_PLUS:
				break;
			case TREE_HIT_NONE:
			default:
				owner_->model()->setFocusedRow(row);
				RedrawWindow(handle_, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
				model()->activateRowAlternative(owner_, row, column);
				break;
			}
			++column;
		}			
	}
	__super::onMessageMButtonDown(button, x, y);
}

void TreeImpl::onMessageMouseMove(UINT button, int x, int y)
{
	if(drag_.captured() && !(button & MK_LBUTTON))
		drag_.interrupt();
	if(drag_.captured()){
		POINT pos;
		GetCursorPos(&pos);
		if(drag_.dragOn(pos) && GetCapture() != *this)
			SetCapture(*this);
		RedrawWindow(*this, 0, 0, RDW_INVALIDATE);
	}
	else{
		RECT rowRect;
		Vect2i point(x, y);
		TreeRow* row = rowByPoint(point, rowRect);
		if(row){
			int column = 0;		
			switch(hitTest(row, point, rowRect, column)){
			case TREE_HIT_ROW:
				if(row != hoveredRow_){
					hoveredRow_ = row;
					RedrawWindow(handle_, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
				}
				break;
			case TREE_HIT_PLUS:
			case TREE_HIT_TEXT:
				if(hoveredRow_){
					hoveredRow_ = 0;
					RedrawWindow(handle_, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
				}
				break;
			case TREE_HIT_NONE:
			default:
				break;
			}
		}
		if(capturedRow_){
			Recti rect;
			getRowRect(capturedRow_, rect);
			owner_->onRowMouseMove(capturedRow_, rect, point);
		}
		__super::onMessageMouseMove(button, x, y);
	}
}

BOOL TreeImpl::onMessageEraseBkgnd(HDC dc)
{
	return FALSE;
}

void TreeImpl::onMessageScroll(UINT message, WORD type)
{
	SetFocus(handle_);
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask  = SIF_ALL;

	HDC dc = GetDC(*this);
	ASSERT(dc);
	HGDIOBJ oldFont = ::SelectObject(dc, Win32::defaultFont());
	TEXTMETRIC textMetric;
	VERIFY(GetTextMetrics(dc, &textMetric));
	::SelectObject(dc, oldFont);
	int lineScrollHeight = textMetric.tmHeight + 2;
	ReleaseDC(*this, dc);

	::GetScrollInfo(handle_, message == WM_VSCROLL ? SB_VERT : SB_HORZ, &info);
	int oldPosition = info.nPos;
	switch(type){
	case SB_TOP:        info.nPos = info.nMin; break;
	case SB_BOTTOM:     info.nPos = info.nMax; break;
	case SB_LINEUP:     info.nPos -= lineScrollHeight; break;
	case SB_LINEDOWN:   info.nPos += lineScrollHeight; break;
	case SB_PAGEUP:     info.nPos -= info.nPage; break;
	case SB_PAGEDOWN:   info.nPos += info.nPage; break;
	case SB_THUMBTRACK: info.nPos = info.nTrackPos; break;
	default:			break; 
	}

	info.fMask = SIF_POS;

	if(message == WM_VSCROLL){
		::SetScrollInfo(handle_, SB_VERT, &info, TRUE);
		::GetScrollInfo(handle_, SB_VERT, &info);
		scrollOffset_ = int(info.nPos);
	}
	else{
		::SetScrollInfo(handle_, SB_HORZ, &info, TRUE);
		::GetScrollInfo(handle_, SB_HORZ, &info);
		scrollOffset_ = int(info.nPos);
	}

	if(info.nPos != oldPosition){
		owner_->_arrangeChildren();
		RedrawWindow(handle_, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
		updateScrollBar();
	}
}

BOOL TreeImpl::onMessageSize(UINT type, USHORT width, USHORT height)
{
	if(!creating()){
		::GetClientRect(handle_, &area_);
		if(!owner_->compact_)
			area_.left += 4;
		area_.top += 4;
		area_.bottom -= 4;
		area_.right -= 4;
		owner_->_arrangeChildren();
		updateScrollBar();
	}
	return TRUE;
}

void TreeImpl::onMessagePaint()
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(handle_, &ps);
	{
		ExcludeClipRectVisitor visitor(dc);
		owner_->visitChildren(visitor);
		Win32::MemoryDC memoryDC(dc);
		redraw(memoryDC);
	}
	EndPaint(handle_, &ps);
	__super::onMessagePaint();
}

int TreeImpl::onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	TreeRow* row = model()->focusedRow();

	bool result = owner_->onRowKeyDown(row, sKey(keyCode, true));
	::RedrawWindow(*this, 0, 0, RDW_INVALIDATE);
	if(!result)
		return __super::onMessageKeyDown(keyCode, count, flags);
	else
		return 0;
}



LRESULT TreeImpl::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	ASSERT(::IsWindow(handle_));
    switch(message){
	case WM_DESTROY:
		{
		}
		break;
	case WM_HSCROLL:
	case WM_VSCROLL:
		onMessageScroll(message, LOWORD(wparam));
		break;
	case WM_ENABLE:
		xassert(wparam && "Disabling Window");
		break;
    }
    return __super::onMessage(message, wparam, lparam);
}

struct DrawVisitor : kdw::TreeRowVisitor{
	DrawVisitor(Tree* tree, HDC dc, RECT area, bool drawFocused, int scrollOffset)
	: area_(area)
	, dc_(dc)
	, offset_(0)
	, tree_(tree)
	, scrollOffset_(scrollOffset)
	{
		if(drawFocused){
			focusedRow_ = tree_->model()->focusedRow();
		}
		else{
			focusedRow_ = 0;
		}
		hoveredRow_ = tree_->impl()->hoveredRow();
		hoverPushed_ = tree_->impl()->hoverPushed();
	}
    bool operator()(TreeRow* row){
		TreeColumns::const_iterator it;
		int columnIndex = 0;
		if(offset_ + row->height() > scrollOffset_){
			for(it = tree_->columns().begin(); it != tree_->columns().end(); ++it, ++columnIndex){
				const TreeColumn& column = tree_->columns()[columnIndex];
				RECT entireRowRect;
				entireRowRect.left = area_.left;
				entireRowRect.right = area_.right;
				entireRowRect.top = area_.top + offset_ - scrollOffset_;
				entireRowRect.bottom = entireRowRect.top + row->height();
				if(const kdw::TreeColumnDrawer* drawer = it->drawer()){
					drawer->drawRow(row, columnIndex, dc_, entireRowRect, row->indent(),
									 row == focusedRow_, row == hoveredRow_, hoverPushed_ && row == hoveredRow_);
				}
				else{
					HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
					FillRect(dc_, &entireRowRect, brush);
					DeleteObject(brush);
				}
			}
		}
		offset_ += row->height();
		if(offset_ - scrollOffset_ > area_.bottom)
			return false;
		
		if(row->expanded())
			row->visitChildren(*this);
		return true;
	}

protected:
	const Tree* tree_;
	TreeRow* focusedRow_;
	TreeRow* hoveredRow_;
	HDC dc_;
	RECT area_;
	int offset_;
	int scrollOffset_;
	bool hoverPushed_;
};

TreeHitTest TreeImpl::hitTest(TreeRow* row, Vect2i point, const RECT& rowRect, int& column)
{
	int i = 0;
	TreeColumns::iterator it;
	for(it = owner_->columns().begin(); it != owner_->columns().end(); ++it){
		if(TreeColumnDrawer* drawer = it->drawer()){
			TreeHitTest result = drawer->hitTest(row, point, rowRect);
			if(result != TREE_HIT_NONE){
				column = i;
				return result;
			}
		}
		++i;
	}
	return TREE_HIT_NONE;
}

void TreeImpl::redraw(HDC dc)
{
	RECT clientRect;
	::GetClientRect(handle_, &clientRect);
	if(owner_->flat_){
		FillRect(dc, &clientRect, GetSysColorBrush(COLOR_BTNFACE));
		RECT rt = clientRect;
		rt.bottom = rt.top + 1;
		FillRect(dc, &rt, GetSysColorBrush(COLOR_3DSHADOW));
		rt = clientRect;
		rt.top = rt.bottom - 1;
		FillRect(dc, &rt, GetSysColorBrush(COLOR_3DSHADOW));
		::IntersectClipRect(dc, area_.left - 4, area_.top - 3, area_.right + 4, area_.bottom + 3);
	}
	else{
		FillRect(dc, &clientRect, GetSysColorBrush(COLOR_WINDOW));
		DrawEdge(dc, &clientRect, BDR_SUNKEN, BF_RECT);
		::IntersectClipRect(dc, area_.left - 2, area_.top - 2, area_.right + 2, area_.bottom + 2);
	}

	if(kdw::TreeModel* model = owner_->model()){
		if(drag_.captured())
			drag_.drawUnder(dc);
		model->root()->visitChildren(DrawVisitor(owner_, dc, area_, owner_->isFocused() && !drag_.captured(), scrollOffset_));
		if(drag_.captured()){
			drag_.drawOver(dc);
		}
		else{
			if(model->focusedRow() == model->root() && owner_->isFocused()){
				clientRect.left += 2; clientRect.top += 2;
				clientRect.right -= 2; clientRect.bottom -= 2;
				DrawFocusRect(dc, &clientRect);
			}
		}
	}
}

struct RowByPointFinder : TreeRowVisitor{
	RowByPointFinder(TreeModel* model, Vect2i point, RECT area, int scrollOffset)
	: offset_(area.top - scrollOffset)
	, point_(point)
	, result_(0)
	, model_(model)
	{
	}

    Vect2i point_;
	int offset_;
	TreeRow* result_;
	TreeModel* model_;
	bool operator()(TreeRow* row){
		if(row == model_->root())
			return false;
		int height = row->height();
		if(point_.y > offset_){
			if(point_.y <= offset_ + height){
				if(point_.x >= row->indent())
					result_ = row;
				return false; // stop iteration
			}
		}
		offset_ += height;
		if(row->expanded())
			if(!row->visitChildren(*this))
				return false;
		return true;
	}
};

TreeRow* TreeImpl::rowByPoint(Vect2i point, RECT& outRowRect)
{
	if(TreeModel* model = owner_->model()){
		int offset = 0;
		RowByPointFinder finder(model, point, area_, scrollOffset_);
		model->root()->visitChildren(finder);
		if(finder.result_){
			outRowRect.left = area_.left;
			outRowRect.right = area_.right;
			outRowRect.top = finder.offset_;
			outRowRect.bottom = finder.offset_ + finder.result_->height();
			return finder.result_;		
		}
		Win32::Rect clientRect;
		GetClientRect(*this, &clientRect);
		clientRect.left += 4;
		clientRect.top += 4;
		clientRect.right -= 4;
		clientRect.bottom -= 4;
		outRowRect = clientRect;
		return owner_->model()->root();
	}
	return 0;
}

/*
struct EnsureVisibleVisitor : TreeRowVisitor{
	EnsureVisibleVisitor(TreeRow* focusedRow)
	: focusedRow_(focusedRow)
	{}

	bool operator()(TreeRow* row){
		const TreeColumnDrawer* drawer = tree_->columns().front().drawer();
		int height = drawer ? drawer->rowHeight(row, 0) : 16;
		int indent = drawer ? drawer->rowIndent(row) : tree_->childIndent();
		row->adjust(height);
		if(visibleLevel_){
			totalHeight_ += row->height();
		}
	}

    TreeRow* focusedRow_;
};
*/


struct AdjustHeightVisitor : TreeRowVisitor{
	AdjustHeightVisitor(Tree* tree, int level = 0)
	: tree_(tree)
	, level_(level)
	, totalHeight_(0)
	, visibleLevel_(true)
	{
	}
	const Tree* tree_;
	int level_;
	int totalHeight_;
	bool visibleLevel_;
	bool operator()(TreeRow* row){
		const TreeColumnDrawer* drawer = tree_->columns().front().drawer();

		int height = drawer ? drawer->rowHeight(row, 0) : 16;
		int indent = drawer ? drawer->rowIndent(row) : tree_->childIndent();
		row->adjust(height, tree_);
		if(visibleLevel_){
			totalHeight_ += row->height();
			row->setLevel(level_);
			TreeRow* parent = row->parent();
			if(parent){
				row->inheritIndent(indent, tree_->rootIndent());
				if(tree_->compact()){
					// может быть не лучшим местом для этого
					if(tree_->rowVisible(row)){
						if(parent == tree_->model()->root())
							row->_setExpanded(true);
					}
					else
						row->_setExpanded(false);

					if(parent->parent() == tree_->model()->root())
						row->setIndent(0);
				}
			}
			else
				row->setIndent(tree_->rootIndent());
		}

		++level_;
		if(visibleLevel_){
			if(row->expanded())
				row->visitChildren(*this);
			else{
				visibleLevel_ = false;
				row->visitChildren(*this);
				visibleLevel_ = true;
			}
		}
		--level_;
		return true;
	}
};

bool TreeImpl::toggleRow(TreeRow* row)
{
	if(!owner_->canBeToggled(row))
		return false;
	if(row->empty())
		return false;
	owner_->expandRow(row, !row->expanded());

	updateScrollBar();
	::RedrawWindow(handle_, 0, 0, RDW_INVALIDATE);
	return true;
}

struct RowRectObtainer : public TreeRowVisitor{
	
	RowRectObtainer(TreeRow* row, const RECT& area, int scrollOffset, bool onlyVisible = true)
	: offset_(area.top - scrollOffset)
	, lastOffset_(area.top - scrollOffset)
	, area_(area)
	, row_(row)
	, onlyVisible_(onlyVisible)
	{
	}

	bool operator()(TreeRow* current){
		if(current == row_){
			offset_ = lastOffset_;
			if(lastOffset_ + current->height() > area_.top && lastOffset_ < area_.bottom){
				visible_ = true;
				if(onlyVisible_)
					return false;
			}
			else
				visible_ = false;
		}
		lastOffset_ += current->height();
		if(current->expanded())
			if(!current->visitChildren(*this))
				return false;
		return true;
	}

	bool visible_;
	bool onlyVisible_;
	int offset_;
	int lastOffset_;
	RECT area_;
	TreeRow* row_;
};

bool TreeImpl::getRowRect(TreeRow* row, Recti& outRect, bool onlyVisible)
{
	RowRectObtainer obtainer(row, area_, scrollOffset_, onlyVisible);
	model()->root()->visitChildren(obtainer);
	if(obtainer.visible_ || !onlyVisible)
		outRect.set(area_.left, obtainer.offset_, area_.right - area_.left, row->height());
	return obtainer.visible_;
}

void TreeImpl::ensureVisible(TreeRow* row, bool update)
{
	if(row == owner_->model()->root())
		return;

	owner_->expandParents(row);

	Recti rect;
	getRowRect(row, rect, false);
	if(rect.top() < area_.top){
		scrollOffset_ -= area_.top - rect.top();
	}
	else if(rect.bottom() > area_.bottom){
		scrollOffset_ += rect.bottom() - area_.bottom;
	}
	if(update)
		owner_->update();
}

void TreeImpl::updateScrollBar()
{
	int pageSize = area_.bottom - area_.top;
	scrollOffset_ = max(0, min(scrollOffset_, max(0, totalHeight_ - pageSize)));

	SCROLLINFO scrollInfo;

	memset((void*)&scrollInfo, 0, sizeof(scrollInfo));
	scrollInfo.cbSize = sizeof(SCROLLINFO);
	scrollInfo.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
	scrollInfo.nMin = 0;
	scrollInfo.nMax = totalHeight_;
	scrollInfo.nPos = scrollOffset_;
	scrollInfo.nPage = pageSize;
	SetScrollInfo(handle_, SB_VERT, &scrollInfo, TRUE);

}


// -----------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable: 4355) //  'this' : used in base member initializer list
Tree::Tree(TreeModel* model, int border)
: _ContainerWithWindow(new TreeImpl(this), border)
, columns_(new TreeColumns)
, model_(0)
, childIndent_(8)
, rootIndent_(0)
, emphasizeFolders_(false)
, selectFocused_(true)
, trackHovered_(false)
, flat_(false)
, compact_(false)
, imageStore_(0)
{
    _setMinimalSize(Vect2i(0, 0));
	if(model)
		setModel(model);
}
#pragma warning(pop)

Tree::~Tree()
{
	model_ = 0;
}

void Tree::update()
{
	if(model()){
		updateHeights();
		::RedrawWindow(*impl(), 0, 0, RDW_INVALIDATE);
	}
}

TreeImpl* Tree::impl() const
{
	return static_cast<TreeImpl*>(_window());
}

bool Tree::onRowKeyDown(TreeRow* row, sKey key)
{
	TreeRow* selectedRow = 0;
	switch(key.key){
	case VK_UP:
		if(TreeRow* row = model()->focusedRow()){
			if(TreeRow* parent = row->parent()){
				TreeRow::iterator it = parent->find(row);
				ASSERT(it != parent->end());
				if(it != parent->begin()){
					--it;
					row = *it;
					while(row->expanded() && !row->empty())
						row = row->back();
					selectedRow = row;
				}
				else if(parent->parent()){
					selectedRow = parent;
				}
			}
		}
		break;
	case VK_DOWN:
		if(TreeRow* row = model()->focusedRow()){
			if(row->expanded() && !row->empty()){
				selectedRow = row->front();
			}
			else{
				while(row->parent()){
					TreeRow* parent = row->parent();
					TreeRow::iterator it = parent->find(row);
					ASSERT(it != parent->end());
					++it;
					if(it != parent->end()){
						selectedRow = *it;
						break;
					}
					else
						row = row->parent();
				}
				ASSERT(parent != 0);
			}
		}
		break;
	case VK_LEFT:
		if(TreeRow* row = model()->focusedRow()){
			if(canBeToggled(row) && row->expanded()){
				expandRow(row, false);
				model()->requestUpdate();
			}
			else if(row->parent() && row->parent()->parent()){
				selectedRow = row->parent();
			}
		}
		break;
	case VK_RIGHT:
		if(TreeRow* row = model()->focusedRow()){
			if(!row->empty()){
				if(canBeToggled(row) && !row->expanded()){
					expandRow(row, true);
					model()->requestUpdate();
				}
				else{
					selectedRow = row->front();
				}
			}
		}
		break;
	case VK_HOME:
		if(!model()->root()->empty()){
			selectedRow = model()->root()->front();
		}
		break;
	case VK_END:
		if(!model()->root()->empty()){
			TreeRow* row = model()->root()->back();
			while(row->expanded() && !row->empty())
				row = row->back();
			selectedRow = row;
		}
		break;
	case VK_SPACE:
	//	case VK_RETURN:
		if(TreeRow* focusedRow = model()->focusedRow()){
			if(canBeToggled(focusedRow) && !focusedRow->empty() /* hasVisibleChildren */){
				expandRow(focusedRow, !focusedRow->expanded());
			}
			else{
				model()->activateRow(this, focusedRow, 0);
			}
		}
		break;
	}
	if(selectedRow){
		onRowSelected(selectedRow);	
		return true;
	}
	return false;
}

bool Tree::onRowLMBDown(TreeRow* row, const Recti& rowRect, Vect2i point)
{
	Recti plusRect;
	ASSERT(!columns().empty());
	if(TreeColumnDrawer* drawer = columns()[0].drawer()){
		if(row != model()->root()){
			drawer->getSubRectPlus(row, rowRect, plusRect);
			if(plusRect.point_inside(point)){
				impl()->toggleRow(row);
				return false;
			}
		}
		if(row)
			onRowSelected(row);	
	}
	RedrawWindow(*impl(), 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
	return false;
}

bool Tree::onRowLMBDoubleClick(TreeRow* row, const Recti& rowRect, Vect2i point)
{
	return false;
}

void Tree::onRowLMBUp(TreeRow* row, const Recti& rowRect, Vect2i point)
{
}

void Tree::onRowRMBDown(TreeRow* row, const Recti& rowRect, Vect2i point)
{
}

void Tree::onRowRMBUp(TreeRow* row, const Recti& rowRect, Vect2i point)
{
}

void Tree::expandParents(TreeRow* row)
{
	typedef std::vector<TreeRow*> Parents;
	Parents parents;
	TreeRow* p = row->parent();
	while(p){
		parents.push_back(p);
		p = p->parent();
	}
	Parents::iterator it;
	for(it = parents.begin(); it != parents.end(); ++it)
		expandRow(*it, true);
}

void Tree::expandAll(TreeRow* root)
{
	if(!root){
		root = model()->root();
		xassert(root);
		TreeRow::iterator it;
		FOR_EACH(*root, it){
			TreeRow* row = *it;
			row->expandAll(this);
		}
	}
	else
		root->expandAll(this);
	update();
}

void Tree::expandRow(TreeRow* row, bool expanded)
{
	row->_setExpanded(expanded);
	if(!row->expanded()){
		int i = 0;
		TreeRow::iterator it;
		FOR_EACH(*row, it){
			TreeRow* child = *it;
			if(canBeToggled(child))
				child->_setExpanded(false);
		}		
	}

	AdjustHeightVisitor visitor(this, row->level());
	row->visitChildren(visitor);

	if(row->expanded())
		impl()->totalHeight_ += visitor.totalHeight_;
	else{
		impl()->totalHeight_ -= visitor.totalHeight_;

		TreeRow* f = model()->focusedRow();
		while(f){
			if(row == f){
				model()->setFocusedRow(row);
				break;
			}
			f = f->parent();
		}
	}
	updateHeights(); // FIXME вызывается тут т.к. сломалось
}

void Tree::setModel(TreeModel* model)
{
	model_ = model;

	columns().clear();
	if(model){
		int count = model->columnsCount();
		columns().resize(count);
		for(int i = 0; i < count; ++i){
			const char* typeName = model->columnTypeName(i);
			TreeColumnDrawer* drawer = autoCreateColumnDrawer(typeName);
			if(drawer)
				setColumn(i, drawer, float((i + 1.0f) / float(count)));
		}

        updateHeights();
		model->signalUpdated().connect(this, &Tree::onModelUpdated);
	}
}

void Tree::interruptDrag()
{
	impl()->drag_.interrupt();
}

void Tree::updateHeights()
{
	ASSERT(model());
	ASSERT(model()->root());
	AdjustHeightVisitor visitor(this, 0);
	model()->root()->visitChildren(visitor);
	impl()->totalHeight_ = visitor.totalHeight_;
	impl()->updateScrollBar();
}

void Tree::setColumn(int column, TreeColumnDrawer* drawer, float position)
{
	ASSERT(drawer);
	ASSERT(column >= 0 && column < int(columns().size()));

	drawer->setTree(this);
	columns()[column].setDrawer(drawer);
	updateHeights();
}

void Tree::setFlat(bool flat)
{
	flat_ = flat;
	update();
}

void Tree::setCompact(bool compact)
{
	compact_ = compact;
	//rootIndent_ = compact ? -16 : 0;
	update();
}

void Tree::setRootIndent(int indent)
{
	rootIndent_ = indent;
	update();
}

void Tree::setEmphasizeFolders(bool emphasizeFolders)
{
	emphasizeFolders_ = emphasizeFolders;
	update();
}

void Tree::setSelectFocused(bool selectFocused)
{
	selectFocused_ = selectFocused;
	update();
}

struct RowStateObtainer : TreeRowVisitor{
	RowStateObtainer(std::vector<char>& states)
	: states_(states)
	{}
	bool operator()(TreeRow* row){
		states_.push_back(row->expanded() ? 1 : 0);
		if(row->expanded())
			row->visitChildren(*this);
		return true;
	}
protected:
	std::vector<char>& states_;
};

TreeState Tree::state()
{
	TreeState state;
	state.focusedRow = model()->pathFromRow(model()->focusedRow());
	state.scrollOffset = impl()->scrollOffset_;
	state.selection = model()->selection();
	RowStateObtainer obtainer(state.expanded);
	model()->root()->visitChildren(obtainer);
	return state;
}


struct RowExpander : TreeRowVisitor{
	RowExpander(const std::vector<char>& states, Tree* tree)
	: states_(states)
	, index_(0)
	, tree_(tree)
	{}

	bool operator()(TreeRow* row){
		if(index_ >= states_.size())
			return false;

		if(states_[index_]){
			++index_;
			if(tree_->canBeToggled(row)){
				row->_setExpanded(true);
				if(!row->visitChildren(*this))
					return false;
			}
			return true;
		}
		else{
			++index_;
		}
		return true;
	}
protected:
	int index_;
	Tree* tree_;
	const std::vector<char>& states_;
};

void Tree::setState(const TreeState& state)
{
	RowExpander expander(state.expanded, this);
	model()->root()->visitChildren(expander);
	impl()->scrollOffset_ = state.scrollOffset;
	TreeRow* focusedRow = model()->rowFromPath(state.focusedRow);
	if(focusedRow == 0 && !state.selection.empty())
		focusedRow = model()->rowFromPath(state.selection.front());
	model()->setFocusedRow(focusedRow);
	model()->setSelection(state.selection);
	ensureVisible(focusedRow);
	update();
}

void Tree::serialize(Archive& ar)
{
	__super::serialize(ar);
	if(ar.filter(SERIALIZE_STATE)){
		ASSERT(model() && model()->root());

		TreeState state;
		if(ar.isOutput())
			state = this->state();

		state.serialize(ar);

		if(ar.isInput())
			setState(state);
	}
	if(ar.filter(SERIALIZE_DESIGN)){
		ar.serialize(model_, "model", "Модель");
		ar.serialize(*columns_, "columns", "Колонки");
		ar.serialize(emphasizeFolders_, "emphasizeFolders", "Выделить папки");
		ar.serialize(childIndent_, "childIndent", "Отступ дочернего элемента");
		if(ar.isInput()){
			TreeColumns::iterator it;
			FOR_EACH(*columns_, it){
				if(it->drawer())
					it->drawer()->setTree(this);
			}
			update();
		}
	}
}




void Tree::onModelUpdated()
{
	update();
	setFocus();
}

bool Tree::hitActivationRect(TreeRow* row, Vect2i point, const Recti& rowRect, int column)
{
	return impl()->hitTest(row, point, Win32::Rect(rowRect), column) != TREE_HIT_TEXT;
}

bool Tree::getRowRect(TreeRow* row, Recti& rect, bool onlyVisible)
{
	return impl()->getRowRect(row, rect, onlyVisible);
}


void Tree::ensureVisible(TreeRow* row, bool update)
{
	impl()->ensureVisible(row, update);
}

void Tree::redraw()
{
	RedrawWindow(*impl(), 0, 0, RDW_INVALIDATE);
}

bool Tree::isFocused() const
{
	return GetFocus() == *impl();
}

void Tree::_setFocus()
{
	if(::IsWindowVisible(*impl())){
		xassert(IsWindowEnabled(*impl()));
		::SetFocus(*impl());	
		xassert(::GetFocus() == *impl());
	}
	Widget::_setFocus();
}

bool Tree::canBeToggled(TreeRow* row) const
{
	ASSERT(row);
	if(!rowVisible(row))
		return false;
	if((compact_ && row->parent() == model()->root()) || !row->canBeToggled())
		return false;
	return !row->empty();
}

void Tree::onRowSelected(TreeRow* row)
{
	TreeModel::Selection selection = model()->selection();
	if(selectFocused() && row != model()->root()){
		model()->deselectAll();
		model()->selectRow(row, true);
	}
	model()->setFocusedRow(row);
	ensureVisible(row);
}

void Tree::onRowDeselected(TreeRow* row)
{
	TreeModel::Selection selection = model()->selection();
	if(selectFocused() && row != model()->root()){
		model()->selectRow(row, false);
	}
	ensureVisible(row);
}

bool Tree::rowVisible(TreeRow* row) const
{
	return !row->hidden();
}

void Tree::setImageStore(ImageStore* store)
{
	imageStore_ = store;
}

// ---------------------------------------------------------------------------------
REGISTER_CLASS(TreeModel, StringTreeModel, "Модель дерева строк");

struct SelectionCollector : TreeRowVisitor{
	SelectionCollector(TreeModel* model, TreeModel::Selection& selection)
	: selection_(selection)
	, model_(model)
	{}
	bool operator()(TreeRow* row){
		if(row->selected())
			selection_.push_back(model_->pathFromRow(row));
		row->visitChildren(*this);
		return true;
	}
	TreeModel* model_;
	TreeModel::Selection& selection_;
};


TreePath TreeModel::pathFromRow(TreeRow* row)
{
    TreePath result;
	if(row)
		while(row->parent()){
			int index = row->index();
			result.insert(result.begin(), index);
			row = row->parent();
		}
	return result;
}

bool TreeSelection::serialize(Archive& ar, const char* name, const char* nameAlt)
{
	return ar.serialize(*static_cast<Base* const>(this), name, nameAlt);
}

void TreeModel::selectRow(TreeRow* row, bool select)
{
	Selection::iterator it = std::find(selection_.begin(), selection_.end(), pathFromRow(row));
	if(select){
		if(it == selection_.end())
			selection_.push_back(pathFromRow(row));
	}
	else{
		if(it != selection_.end()){
			TreeRow* it_row = rowFromPath(*it);
			ASSERT(it_row->numRef() > 0 && it_row->numRef() < 0xFFFF);
			selection_.erase(it);
		}
		else{
			if(row->selected()){
				ASSERT(0 && "No such element selected!");
				return;
			}
		}
	}
	row->setSelected(select);
}

struct Deselector : TreeRowVisitor{
	bool operator()(TreeRow* row){
		row->setSelected(false);
		row->visitChildren(*this);
		return true;
	}
};

void TreeModel::deselectAll(bool visit)
{
	Selection sel(selection());
	Selection::iterator it;
	FOR_EACH(sel, it){
		TreeRow* row = rowFromPath(*it);
		selectRow(row, false);
	}
	if(visit){
		Deselector()(root());
		selection_.clear();
	}
}

TreeRow* TreeModel::rowFromPath(const TreePath& path)
{
	TreeRow* row = root();
	TreePath::const_iterator it;
	for(it = path.begin(); it != path.end(); ++it){
		int index = it->index;
		if(index < int(row->size()) && index >= 0){
			TreeRow* nextRow = row->childrenByIndex(index);
			if(!nextRow)
				return row;
			else
				row = nextRow;
		}
		else
			return row;
	}
	return row;
}

void TreeModel::setFocusedRow(TreeRow* row)
{
	focusedRow_ = pathFromRow(row);
}


TreeRow* TreeModel::focusedRow()
{
	return rowFromPath(focusedRow_);
}

void TreeModel::setSelection(const Selection& selection)
{
	deselectAll(true);
	Selection::const_iterator it;
	FOR_EACH(selection, it){
		const TreePath& path = *it;
		TreeRow* row = rowFromPath(path);
		if(row)
			selectRow(row, true);
	}
}

void TreeModel::setRoot(kdw::TreeRow* root)
{
	root_ = root;
	//expandRow(root_);
}

void TreeModel::clear()
{
	if(root_)
		root_->clear();
	selection_.clear();
}

TreeModel::UpdateLock TreeModel::lockUpdate()
{
	if(updateLock_)
		return updateLock_;
	else{
		UpdateLock lock = new TreeModel::LockedUpdate(this);;
		updateLock_ = lock;
		lock->decrRef();
		return lock;
	}
}

void TreeModel::dismissUpdate()
{
	if(updateLock_)
		updateLock_->dismissUpdate();
}

void TreeModel::requestUpdate()
{
	if(updateLock_){
		updateLock_->requestUpdate();
	}
	else{
		onUpdated();
	}
}

void TreeModel::serialize(Archive& ar)
{
	if(0){
		//StringTreeModel model;
		//model.serialize(ar); // instantiate
	}
	ar.serialize(focusedRow_, "focusedRow", "Активная строка");
}

bool TreeColumns::serialize(Archive& ar, const char* name, const char* nameAlt)
{
	return ar.serialize(*static_cast<std::vector<TreeColumn>*>(this), name, nameAlt);
}

void TreeColumn::serialize(Archive& ar)
{
	ar.serialize(caption_, "caption", "&Заголовок");
	ar.serialize(rightCornerPosition_, "rightCornerPosition", "Ширина");
	ar.serialize(drawer_, "drawer", "&Отрисовщик");
}

/*
bool TreePath::serialize(Archive& ar, const char* name, const char* nameAlt)
{
	return ar.serialize(*static_cast<std::vector<int>*>(this), name, nameAlt);
}
*/


void TreeColumnDrawer::serialize(Archive& ar)
{

}

void TreeColumnDrawer::getSubRectPlus(TreeRow* row, const Recti& rowRect, Recti& outResult) const
{
	if(row->parent() == tree_->model()->root() && tree_->compact())
		outResult.set(rowRect.left() + row->indent(), rowRect.top(), 0, rowRect.height());
	else
		outResult.set(rowRect.left() + row->indent(), rowRect.top(), rowRect.height(), rowRect.height());
}

TreeModel* TreeColumnDrawer::model()
{
	return tree_->model();
}

const TreeModel* TreeColumnDrawer::model() const
{
	return tree_->model();
}
// ---------------------------------------------------------------------------------



StringTreeColumnDrawer::StringTreeColumnDrawer(Tree* tree)
: TreeColumnDrawer(tree)
{
}

int StringTreeColumnDrawer::rowHeight(TreeRow* row, int column) const
{
	int rHeight = Win32::calculateTextSize(*tree()->impl(), Win32::defaultFont(), " ").y + 6;
	if( tree_->imageStore() && tree_->imageStore()->slideSize().y > rHeight)
		rHeight = tree_->imageStore()->slideSize().y + 3;

	return rHeight;
}

TreeHitTest StringTreeColumnDrawer::hitTest(TreeRow* row, Vect2i point, const RECT& rowRect)
{
	RECT rect = rowRect;
	rect.left += row->indent();
	rect.left += 1;
	rect.right -= 1;
	rect.top += 1;
	::InflateRect(&rect, -2, -2);
	int plusSize = rect.bottom - rect.top;
	RECT plusRect;
	plusRect.left = rect.left;
	plusRect.top = rect.top;
	plusRect.right = plusRect.left + plusSize;
	plusRect.bottom = plusRect.top + plusSize;
	rect.left += plusSize;

	POINT pt = {point.x, point.y};
	if(!row->empty() && PtInRect(&plusRect, pt))
		return TREE_HIT_PLUS;
	else if(PtInRect(&rect, pt))
		return TREE_HIT_TEXT;
	else
		return TREE_HIT_NONE;
}

void StringTreeColumnDrawer::getSubRectImage(TreeRow* row, const Recti& rowRect, Recti& imageRect) const
{
	xassert(tree()->imageStore());
	int imageHeight = tree()->imageStore()->slideSize().y;
	int imageWidth = tree()->imageStore()->slideSize().x;
	imageRect = rowRect;
	imageRect.left(imageRect.left() + row->indent() + rowRect.height() + 2);
	imageRect.top(rowRect.top() + (rowRect.height()- imageHeight) / 2);
	imageRect.width(imageWidth);
	imageRect.height(imageHeight);
}


void StringTreeColumnDrawer::drawRow(TreeRow* row, int column, HDC dc, const RECT& rowRect, int indent, bool focused, bool hovered, bool activated) const
{
	const char* text = static_cast<StringTreeRow*>(row)->value();
	RECT rect = rowRect;
	rect.left += indent;
	RECT selectionRect = rowRect;
	selectionRect.left += indent + selectionRect.bottom - selectionRect.top - 2;
	COLORREF textColor = ::GetSysColor(COLOR_BTNTEXT);
	if(row->selected()){
		::FillRect(dc, &selectionRect, tree()->isFocused() ? ::GetSysColorBrush(COLOR_ACTIVECAPTION) : ::GetSysColorBrush(COLOR_INACTIVECAPTION));
		textColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	}

	HFONT normalFont = Win32::defaultFont();
	HFONT boldFont = Win32::defaultBoldFont();

	bool empty = row->empty();
	HFONT font_ = empty ? normalFont : (tree()->emphasizeFolders() ? boldFont : normalFont);

	HFONT oldFont = HFONT(::SelectObject(dc, (HGDIOBJ)font_));
	HBRUSH oldBrush = HBRUSH(::SelectObject(dc, (HGDIOBJ)(::GetStockObject(HOLLOW_BRUSH))));
	int oldBackMode = ::SetBkMode(dc, TRANSPARENT);
	COLORREF oldBkColor = ::SetBkColor(dc, ::GetSysColor(COLOR_BTNFACE));
	rect.left += 1;
	rect.right -= 1;
	rect.top += 1;
	::InflateRect(&rect, -2, -2);
	
	int plusSize = rect.bottom - rect.top;
	if(!empty){
		Recti plusRect(rect.left, rect.top, plusSize, plusSize);
		drawPlus(dc, plusRect, row->expanded(), row->selected(), false);
	}
	rect.left += plusSize + 2;

	if(tree_->imageStore() && row->imageIndex()!= -1){
		ImageStore* store = tree_->imageStore();
		int top = rect.top + (rect.bottom - rect.top - store->slideSize().y) / 2; 
		store->_draw(row->imageIndex(), dc, rect.left, top, false);
		rect.left += store->slideSize().x + 2;
	}

	COLORREF oldTextColor = ::SetTextColor(dc, textColor);
	RECT textRect = rect;
	textRect.top = rect.top + (rect.bottom - rect.top - Win32::calculateTextSize(*Win32::_globalDummyWindow, font_, text).y) / 2;;

	::DrawText(dc, text, int(strlen(text)), &textRect, DT_LEFT | DT_END_ELLIPSIS);
	::SetTextColor(dc, oldTextColor);

	if(focused){
		if(row->selected())
			rect = selectionRect;
		else{
			Vect2i textSize = Win32::calculateTextSize(*Win32::_globalDummyWindow, font_, text);
			rect.left -= 1;
			rect.top -= 1;
			rect.right = rect.left + textSize.x + 3;
			rect.bottom = rect.top + textSize.y + 2;
		}
		rect.left -= 1;
		rect.top -= 1;
		rect.right += 1;
		rect.bottom += 1;
		DrawFocusRect(dc, &rect);
	}
	::SelectObject(dc, oldFont);
	::SelectObject(dc, oldBrush);
	::SetBkMode(dc, oldBackMode);
	::SetBkColor(dc, oldBkColor);
}

REGISTER_CLASS(TreeColumnDrawer, StringTreeColumnDrawer, "Отрисовщик колонки дерева строк");
// ---------------------------------------------------------------------------------

TreeColumnDrawer* Tree::autoCreateColumnDrawer(const char* typeName)
{
	TreeColumnDrawer* result = 0;
	if(strcmp(typeName, typeid(std::string).name()) == 0)
		result = new StringTreeColumnDrawer;

	return result;
}

// ---------------------------------------------------------------------------------

void TreeColumnDrawer::drawPlus(HDC dc, const Recti& rect, bool expanded, bool selected, bool grayed) const
{	
	Vect2i size(9, 9);
	Vect2i center(rect.center());
	Win32::Rect r(Recti(center - Vect2i(4, 4), size));

	Win32::AutoSelector pen(dc, CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW)));
	Win32::StockSelector brush(dc, GetSysColorBrush(grayed ? COLOR_BTNFACE : COLOR_WINDOW));
	::RoundRect(dc, r.left, r.top, r.right, r.bottom, 2, 2);

	::MoveToEx(dc, center.x - 2, center.y, 0);
	::LineTo(dc, center.x + 3, center.y);
	if(!expanded){
		::MoveToEx(dc, center.x, center.y - 2, 0);
		::LineTo(dc, center.x, center.y + 3);
	}
}

// ----------------------------------------------------------------------------------
void TreeRow::dropInto(TreeRow* destination, TreeRow* beforeChild, Tree* tree)
{
	destination->receiveDrop(this, beforeChild, tree->model());
}

int TreeRow::index() const
{
	if(!parent())
		return 0;

	Rows::const_iterator it = std::find(parent()->begin(), parent()->end(), this);
	ASSERT(it != parent()->end());
	return std::distance(parent()->begin(), it);
}

TreeRow* TreeRow::childrenByIndex(int index)
{
	if(index >= 0 && index < int(children_.size()))
		return children_[index];
	else
		return 0;
}

const TreeRow* TreeRow::childrenByIndex(int index) const
{
	if(index >= 0 && index < int(children_.size()))
		return children_[index];
	else
		return 0;
}

struct Expander : TreeRowVisitor{
	Expander(Tree* tree)
	: tree_(tree) {}
	Tree* tree_;
    bool operator()(TreeRow* row){
		if(tree_->canBeToggled(row)){
			row->_setExpanded(true);
			return row->visitChildren(*this);
		}
		else
			return true;
	}
};

void TreeRow::expandAll(Tree* tree)
{
	tree->expandRow(this);
	visitChildren(Expander(tree));
}

int TreeRow::childrenIndex(TreeRow* row)
{
	xassert(row);
	Rows::iterator it = std::find(children_.begin(), children_.end(), row);
	xassert(it != children_.end());
	return std::distance(children_.begin(), it);
}

bool TreeRow::isChildOf(const TreeRow* row) const
{
	const TreeRow* p = parent();
	while(p){
		if(p == row)
			return true;
		p = p->parent();
	}
	return false;
}

// --------------------------------------------------------------------------------
void TreePathLeaf::serialize(Archive& ar)
{
	ar.serialize(name, "name", 0);
	ar.serialize(index, "index", 0);
}

// --------------------------------------------------------------------------------
void TreeState::serialize(Archive& ar)
{
	ar.serialize(expanded, "expanded", 0);
	ar.serialize(focusedRow, "focusedRow", 0);		
	ar.serialize(scrollOffset, "scrollOffset", 0);		
	ar.serialize(selection, "selection", 0);
}

}
