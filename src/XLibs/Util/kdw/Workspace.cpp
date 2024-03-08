#include "StdAfx.h"

//#include <windows.h>
#include "kdw/Win32/Rectangle.h"
#include "kdw/Win32/MemoryDC.h"
#include "kdw/_WidgetWindow.h"

#include "kdw/Workspace.h"
#include "kdw/PopupMenu.h"
#include "SplitterImpl.h"
#include "kdw/ScrolledWindow.h"
#include "kdw/SpaceHeader.h"
#include "kdw/VBox.h"
#include "kdw/HBox.h"
#include "kdw/MenuBar.h"
#include "kdw/CommandManager.h"

#include "ClassMenu.h"
#include "ExcludeClipRectVisitor.h"
#include "SplitterImpl.h"

#include "kdw/Serialization.h"
#include "Serialization/XPrmArchive.h"
#include "Serialization/BinaryArchive.h"
#include "Serialization/SerializationFactory.h"
#include "FileUtils/FileUtils.h"

namespace kdw{


class FloatingSplitter : public Win32::Window32{
public:
	const char* className() const{ return "kdw.FloatingSplitter"; }
	FloatingSplitter();

	void set(const Recti& rect);

	void redraw(HDC dc);
	void onMessagePaint();
	BOOL onMessageEraseBkgnd(HDC dc);
	void show(bool visible = true);
protected:
	void drawRow(HDC dc);
	void updateLayeredWindow();

	Recti rect_;
	HBITMAP bitmap_;
	COLORREF* bitmapBits_;
};

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list
FloatingSplitter::FloatingSplitter()
{
	DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;

	exStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT;
	VERIFY(create("", WS_POPUP | WS_DISABLED, Recti(0, 0, 320, 320), 0, exStyle));
}
#pragma warning(pop)

void FloatingSplitter::redraw(HDC dc)
{
	RECT rt = { 0, 0, rect_.width(), rect_.height() };
	FillRect(dc, &rt, GetSysColorBrush(COLOR_BTNFACE));
	DrawEdge(dc, &rt, BDR_RAISEDINNER, BF_RECT);
}

void FloatingSplitter::set(const Recti& rect)
{
	rect_ = rect;
	if(::IsWindow(*this)){
		bool visible = ::IsWindowVisible(*this);
		show(visible);
	}
}

void FloatingSplitter::show(bool visible)
{
	updateLayeredWindow();
	SetWindowPos(*this, HWND_TOP, rect_.left(),  rect_.top(),
				 rect_.width(), rect_.height(), 
				 SWP_NOOWNERZORDER | (visible ? SWP_SHOWWINDOW : SWP_HIDEWINDOW)| SWP_NOACTIVATE);
	updateLayeredWindow();
}

BOOL FloatingSplitter::onMessageEraseBkgnd(HDC dc)
{
	return TRUE;
}

void FloatingSplitter::updateLayeredWindow()
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
		redraw(dc);

		POINT leftTop = { rect_.left(), rect_.top() };
		SIZE size = { rect_.width(), rect_.height() };
		POINT point = { 0, 0 };
		UpdateLayeredWindow(*this, realDC, &leftTop, &size, dc, &point, 0, &blendFunction, ULW_ALPHA);
		SetLayeredWindowAttributes(*this, 0, 192, ULW_ALPHA);
	}
	EndPaint(*this, &ps);
}

void FloatingSplitter::onMessagePaint()
{
}

// ---------------------------------------------------------------------------

static bool isVerticalSplit(Recti rect, Vect2i point)
{
	if(rect.left() != 0){
		point.x -= rect.left();
		rect.left(0);
	}
	if(rect.left() != 0){
		point.y -= rect.top();
		rect.top(0);
	}
	unsigned int vals[] = { abs(rect.left() - point.x), 
							abs(rect.right() - point.x),
							abs(rect.top() - point.y),
							abs(rect.bottom() - point.y) };
	unsigned int minValue = 0xffffffff;
	int minIndex = 0;
	for(int i = 0; i < 4; ++i){
		if(vals[i] < minValue){
			minValue = vals[i];
			minIndex = i;
		}
	}
	return minIndex < 2;
}

class WorkspaceImpl : public _ContainerWindow{
public:
	WorkspaceImpl(Workspace* owner);
	~WorkspaceImpl();

	const char* className() const{ return "kdw.WorkspaceImpl"; }

	BOOL onMessageSetCursor(HWND window, USHORT hitTest, USHORT message);
	BOOL setCursor();
	void onMessageLButtonDown(UINT button, int x, int y);
	void onMessageRButtonDown(UINT button, int x, int y);
	void onMessageMouseMove(UINT button, int x, int y);
	BOOL onMessageEraseBkgnd(HDC dc);

	void startSplit();
	Space* canBeSplitted(int x, int y, bool& outVertical);
protected:
	Workspace* owner_;
	FloatingSplitter* floatingSplitter_;
	bool splitting_;
};

WorkspaceImpl::WorkspaceImpl(Workspace* owner)
: _ContainerWindow(owner)
, owner_(owner)
, splitting_(false)
{
	floatingSplitter_ = new FloatingSplitter();
	VERIFY(create("", WS_CHILD, Recti(0,0, 20, 20), *Win32::_globalDummyWindow));
}

WorkspaceImpl::~WorkspaceImpl()
{
	delete floatingSplitter_;
}

BOOL WorkspaceImpl::setCursor()
{
	POINT pos;
	GetCursorPos(&pos);
	::ScreenToClient(*this, &pos);

	bool vertical = false;
	if(Space* space = canBeSplitted(pos.x, pos.y, vertical)){
		if(vertical){
			HCURSOR cursor = LoadCursor(0, IDC_SIZENS);
			ASSERT(cursor);
			SetCursor(cursor);
		}
		else{
			HCURSOR cursor = LoadCursor(0, IDC_SIZEWE);
			ASSERT(cursor);
			SetCursor(cursor);
		}
	}
	else{
		HCURSOR cursor = LoadCursor(0, IDC_NO);
		ASSERT(cursor);
		SetCursor(cursor);
	}
	return TRUE;
}

BOOL WorkspaceImpl::onMessageSetCursor(HWND window, USHORT hitTest, USHORT message)
{
	if(splitting_){
		return setCursor();
		//SetCursor();
	}
	else{
		return __super::onMessageSetCursor(window, hitTest, message);
	}
}

BOOL WorkspaceImpl::onMessageEraseBkgnd(HDC dc)
{
	ExcludeClipRectVisitor visitor(dc);
	owner_->visitChildren(visitor);
	return Win32::Window32::onMessageEraseBkgnd(dc);
	return FALSE;
}

void WorkspaceImpl::startSplit()
{
	splitting_ = true;
	floatingSplitter_->show();
	SetCapture(*this);
}


Space* WorkspaceImpl::canBeSplitted(int x, int y, bool& outVertical)
{
	POINT pt = { x, y };
	::ClientToScreen(*this, &pt);
	Vect2i point(pt.x, pt.y);

	if(Space* space = owner_->rootSpace_.spaceByPoint(point)){
		HWND wnd = *_findWindow(space->widget());

		Win32::Rect rect(space->widget()->_position());
		::ClientToScreen(wnd, (POINT*)&rect);
		::ClientToScreen(wnd, (POINT*)&rect + 1);
		outVertical = isVerticalSplit(rect.recti(), point);
		return space;
		//}
	}
	return 0;
}

void WorkspaceImpl::onMessageRButtonDown(UINT button, int x, int y)
{
	if(splitting_ && GetCapture() == *this){
		ReleaseCapture();
		splitting_ = false;
		floatingSplitter_->show(false);
	}
	return Win32::Window32::onMessageRButtonDown(button, x, y);
}

void WorkspaceImpl::onMessageLButtonDown(UINT button, int x, int y)
{
	POINT pt = { x, y };
	::ClientToScreen(*this, &pt);
	Vect2i point(pt.x, pt.y);

	if(splitting_){
		if(GetCapture() == *this){
			ReleaseCapture();

			bool vertical = false;
			if(Space* space = canBeSplitted(x, y, vertical)){
				Space* parent = space->parent();
				if(!parent->selfSplit(point)){
					if(vertical){
						Space* newBox = new SpaceVBox(space, point);
						parent->replaceSpace(space, newBox);
					}
					else{
						Space* newBox = new SpaceHBox(space, point);
						parent->replaceSpace(space, newBox);
					}
				}
			}
		}


		floatingSplitter_->show(false);
		splitting_ = false;
	}

	return Win32::Window32::onMessageLButtonDown(button, x, y);
}

static Recti calculateSplitterRect(const Recti& spaceRect, Vect2i point)
{
	int splitterWidth = Splitter::SPLITTER_WIDTH;
	int halfWidth = Splitter::SPLITTER_WIDTH / 2;

	if(isVerticalSplit(spaceRect, point))
		return Recti(spaceRect.left(), point.y - halfWidth,
		             spaceRect.width(), splitterWidth);
	else
		return Recti(point.x - halfWidth, spaceRect.top(),
		             splitterWidth, spaceRect.height());
}

void WorkspaceImpl::onMessageMouseMove(UINT button, int x, int y)
{
	if(splitting_){
		POINT pt = { x, y };
		::ClientToScreen(*this, &pt);

		setCursor();

		Space* space = owner_->rootSpace_.spaceByPoint(Vect2i(pt.x, pt.y));
		if(space){
			Widget* widget = space->widget();
			if(widget){
				Win32::Window32* window = _findWindow(widget);
				ASSERT(window);
				Win32::Rect rect(widget->_position());
				window->clientToScreen(rect);
				
				//if(isVerticalSplit(rect.recti(), point) == vertical()){

				Recti splitterRect = calculateSplitterRect(rect.recti(), Vect2i(pt.x, pt.y));
				floatingSplitter_->set(splitterRect);
				/*
				Win32::Rect rt;
				Win32::Window32* window = _findWindow(widget);
				GetClientRect(*window, &rt);
				//window->clientToScreen(rt);

				HDC dc = GetDCEx(*this, 0, DCX_CACHE | DCX_LOCKWINDOWUPDATE | DCX_CLIPSIBLINGS);
				//FrameRect(dc, &rt, GetSysColorBrush(COLOR_ACTIVECAPTION));
				HBRUSH brush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
				HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, brush);
				PatBlt(dc, rt.left, rt.top, rt.right, rt.bottom, PATINVERT);
				::SelectObject(dc, oldBrush);                
				ReleaseDC(*this, dc);
				*/
			}
		}
	}

	return Win32::Window32::onMessageMouseMove(button, x, y);
}

// ---------------------------------------------------------------------------

class SpaceEmpty : public Space{
public:
	SpaceEmpty();
};

SpaceEmpty::SpaceEmpty()
{
	add(new _DummyWidget());
}



REGISTER_CLASS(Space, SpaceEmpty, "Пустая область");

// ---------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable: 4355) //  'this' : used in base member initializer list
Workspace::Workspace(int border)
: _ContainerWithWindow(new WorkspaceImpl(this), border)
, rootSpace_(this)
{
	Space* space = new SpaceEmpty;
	setSpace(space);
}
#pragma warning(pop)

Workspace::~Workspace()
{
	rootSpace_.setSpace(0);
	rootWidget_ = 0;
}


WorkspaceImpl* Workspace::impl()
{
	return static_cast<WorkspaceImpl*>(_window());
}

void Workspace::visitChildren(WidgetVisitor& visitor) const
{
	if(rootWidget_)
		visitor(*rootWidget_);
}

void Workspace::revertState()
{
	if(::isFileExists(stateFile_.c_str()))
		loadState(stateFile_.c_str());
}

void Workspace::saveState()
{
	saveState(stateFile_.c_str());
}

void Workspace::setStateFile(const char* fileName)
{
	stateFile_ = fileName;
	if(::isFileExists(fileName))
		loadState(fileName);
}

void Workspace::loadState(const char* fileName)
{
	XPrmIArchive ia(fileName);
	ia.setFilter(kdw::SERIALIZE_STATE);
	ia.serialize(*this, "workspace", 0);
}

void Workspace::saveState(const char* fileName)
{
	XPrmOArchive oa(fileName);
	oa.setFilter(kdw::SERIALIZE_STATE);
	oa.serialize(*this, "workspace", 0);

}

void Workspace::startSplit()
{
	impl()->startSplit();
}

void Workspace::setRootWidget(Widget* widget)
{
	rootWidget_ = widget;
	if(rootWidget_){
		rootWidget_->_setParent(this);
		rootWidget_->showAll();
		_arrangeChildren();
	}
}

void Workspace::setSpace(Space* space)
{
	rootSpace_.setSpace(space);
}

void Workspace::_arrangeChildren()
{
	if(rootWidget_){
		RECT rect;
		VERIFY(::GetClientRect(*impl(), &rect));
		VERIFY(::InflateRect(&rect, -border_, -border_));
		rootWidget_->_setPosition(Recti(rect.left , rect.top, rect.right, rect.bottom - rect.top));
	}
}

void Workspace::_setParent(Container* container)
{
	__super::_setParent(container);
	rootSpace_.reparent();
}

void Workspace::serialize(Archive& ar)
{
	ar.serialize(rootSpace_, "rootSpace", "Корневая область");
}

void Workspace::_setFocus()
{
	__super::_setFocus();
}

Widget* Workspace::_nextWidget(Widget* last, FocusDirection direction)
{
	switch(direction)
	{
	case FOCUS_NEXT:
	case FOCUS_PREVIOUS:
		if(last == rootWidget_)
			return 0;
		else
			return rootWidget_;
	case FOCUS_FIRST:
	case FOCUS_LAST:
		if(last == rootWidget_)
			return 0;
		else
			return rootWidget_;
	default:
		return 0;
	}
}


// ---------------------------------------------------------------------------

Space::Space(float position)
: position_(position)
, parent_(0)
, commandManager_(new CommandManager)
{
}

Space::Space()
: position_(0.0f)
, parent_(0)
, commandManager_(new CommandManager)
{
}


Space::~Space()
{
	if(vbox_){
		xassert(!vbox_->parent() || vbox_->parent()->numRef() >= 0);
		xassert(!vbox_->parent() || vbox_->parent()->numRef() < 0xFFFF);
		vbox_->_setParent(0);
		vbox_ = 0;
	}
	delete commandManager_;
	commandManager_ = 0;
}

void Space::rebuildWidgets()
{
	if(!vbox_){
		vbox_ = new VBox;
		{
			HBox* hbox = new HBox;
			hbox->setClipChildren(true);
			vbox_->add(hbox);
			{
				hbox->add(new SpaceHeader(this), true, false, false);
				menuBar_ = new MenuBar(commandManager_, menuPath_.c_str());
				hbox->add(menuBar_, true, true, true);
				headerBox_ = new HBox;
				//headerBox_->setClipChildren(true);
				hbox->add(headerBox_);
			}
			
			scrolledWindow_ = new ScrolledWindow();
			vbox_->add(scrolledWindow_, true, true, true);
			{
			}
		}
	}

	if(widget_){
		scrolledWindow_->add(widget_);
	}
	headerBox_->clear();
	Widgets::iterator it;
	FOR_EACH(headerWidgets_, it){
		Widget* widget = *it;
		headerBox_->add(widget, true, false, false);
	}
}

void Space::add(Widget* widget)
{
	widget_ = widget;
	rebuildWidgets();
}

void Space::addToHeader(Widget* widget)
{
	headerWidgets_.push_back(widget);
	rebuildWidgets();
}

void Space::replaceSpace(Space* oldSpace, Space* newSpace)
{
	ASSERT(0);
}


Space* Space::clone()
{
	Space* result = Space::Factory::instance().create(typeid(*this).name());
	{
	ShareHandle<Space> thisRef(this);

	BinaryOArchive oa;
	oa.setFilter(SERIALIZE_STATE | SERIALIZE_DESIGN);
	oa.serialize(thisRef, "space", 0);

	ShareHandle<Space> resultRef(result);
	
	BinaryIArchive ia(oa);
	ia.setFilter(SERIALIZE_STATE | SERIALIZE_DESIGN);
	ia.serialize(resultRef, "space", 0);

	result->addRef();
	}
	result->decrRef();
	return result;
}
Space* Space::spaceByPoint(Vect2i screenPoint)
{
	if(Widget* widget = this->widget())
	{
		Win32::Rect rect(widget->_position());
		HWND wnd = *_findWindow(widget);
		::ClientToScreen(wnd, (POINT*)(&rect));
		::ClientToScreen(wnd, (POINT*)(&rect) + 1);
		POINT pt = { screenPoint.x, screenPoint.y };
		if(::PtInRect(&rect, pt)){
			return this;
		}
	}
	return 0;
}

kdw::Widget* Space::widget()
{
	return vbox_;
}

void Space::serialize(Archive& ar)
{
	if(ar.filter(SERIALIZE_STATE)){
		ar.serialize(position_, "position", "Position");
	}
}

Widget* Space::findWidget()
{
	Space* space = this;
	while(space){
		if(space->widget())
			return space->widget();
		space = space->parent();
	}
	return 0;
}

Workspace* Space::findWorkspace()
{
	Space* p = this;
	while(p->parent())
		p = p->parent();
	if(p){
		RootSpace* root = safe_cast<RootSpace*>(p);
		return root->workspace();
	}
	else
		return 0;
}

void Space::setParent(Space* parent)
{
	parent_ = parent;
	if(vbox_ && parent == 0)
		vbox_->_setParent(0);

	if(parent)
		if(kdw::Widget* widget = parent->widget()){
			if(HotkeyContext* hotkeyContext = findHotkeyContext(widget))
				commands().setHotkeyContext(hotkeyContext);
		}
}

void Space::setMenu(const char* menuPath)
{
	menuPath_ = menuPath;
	if(menuBar_){
		menuBar_->set(menuPath_.c_str());
	}
}

// ---------------------------------------------------------------------------

void RootSpace::setSpace(Space* space)
{
	child_ = space;
	if(space){
		space->setParent(this);
		workspace_->setRootWidget(space->widget());
	}
	else
		workspace_->setRootWidget(0);
}

void RootSpace::reparent()
{
	if(child_)
		child_->setParent(this);
}

Widget* RootSpace::widget()
{
	return workspace_;
}

void RootSpace::replaceSpace(Space* oldSpace, Space* newSpace)
{
	ASSERT(oldSpace == child_);
	setSpace(newSpace);
}

Space* RootSpace::spaceByPoint(Vect2i screenPoint)
{
	if(child_)
		return child_->spaceByPoint(screenPoint);
	return 0;
}

void RootSpace::serialize(Archive& ar)
{
	if(ar.filter(SERIALIZE_STATE)){
		ar.serialize(child_, "space", "Space");
		if(ar.isInput()){
			workspace_->setRootWidget(child_->widget());
			child_->setParent(this);
		}
	}
}

// ---------------------------------------------------------------------------
REGISTER_CLASS(Space, SpaceHBox, "SpaceHBox");
REGISTER_CLASS(Space, SpaceVBox, "SpaceVBox");

void SpaceBox::serialize(Archive& ar)
{
	Space::serialize(ar);
	if(ar.filter(SERIALIZE_STATE)){
		if(ar.isOutput()){
			Spaces::iterator it;
			for(it = spaces_.begin(); it != spaces_.end(); ++it){
				if(Space* space = *it){
					space->setPosition(splitter_->widgetPosition(space->widget()));
				}
			}
		}
		ar.serialize(spaces_, "spaces", "Области");
		if(ar.isInput()){
			splitter_->clear();
			Spaces::iterator it;
			for(it = spaces_.begin(); it != spaces_.end(); ++it){
				if(Space* space = *it){
					Widget* widget = space->widget() ? space->widget() : new _DummyWidget;
					splitter_->add(widget, space->position(), false);
					space->setParent(this);
				}
			}
		}
	}
}

void SpaceBox::setParent(Space* parent)
{
	__super::setParent(parent);
	Spaces::iterator it;
	for(it = spaces_.begin(); it != spaces_.end(); ++it){
		if(Space* space = *it)
			space->setParent(this);
	}
}

bool SpaceBox::selfSplit(Vect2i point)
{
	if(Widget* widget = splitter_->widgetByScreenPoint(point)){
		Win32::Window32* window = _findWindow(widget);
		ASSERT(window);
		Win32::Rect rect(widget->_position());
		window->clientToScreen(rect);
		
		if(isVerticalSplit(rect.recti(), point) == vertical()){
			Win32::Rect rect(splitter_->_position());
			window->clientToScreen(rect);
			float position = vertical()
				? max(0.0f, min(1.0f, float(point.y - rect.top) / float(rect.bottom - rect.top)))
				: max(0.0f, min(1.0f, float(point.x - rect.left) / float(rect.right - rect.left)));

			Spaces::iterator it;
			int index = 0;
			for(it = spaces_.begin(); it != spaces_.end(); ++it){
				if(Space* space = *it){
					if(space->widget() == widget){
						Space* clonedSpace = space->clone();
						clonedSpace->setParent(this);
						splitter_->add(clonedSpace->widget(), position, false, index);
						clonedSpace->widget()->showAll();
						spaces_.insert(it, clonedSpace);
						return true;
					}
				}
				++index;
			}
		}
	}
	return false;
}

void SpaceBox::removeSpace(int index)
{
	Spaces::iterator it = spaces_.begin() + index;
	xassert(index < spaces_.size());
	Space* space = *it;
	space->setParent(0);
	spaces_.erase(it);
	//updateMinimalSize();
}

void SpaceBox::replaceSpace(Space* oldSpace, Space* newSpace)
{
	ASSERT(newSpace);
	Spaces::iterator it;
	for(it = spaces_.begin(); it != spaces_.end(); ++it){
        Space* space = *it;
		if(space == oldSpace){
			ASSERT(newSpace->widget());
			splitter_->replace(oldSpace->widget(), newSpace->widget());
			*it = newSpace;
			newSpace->setParent(this);
			splitter_->showAll();
			return;
		}
	}
    ASSERT(0 && "Unable to find oldSpace");
}

Space* SpaceBox::spaceByPoint(Vect2i screenPoint)
{
	Spaces::iterator it;
	for(it = spaces_.begin(); it != spaces_.end(); ++it){
		if(Space* space = *it){
			if(Space* result = space->spaceByPoint(screenPoint))
				return result;
		}
	}
	//ASSERT(0 && "warning");
	return 0;
}


// ---------------------------------------------------------------------------
class DarkOverlay : public Win32::Window32{
public:
	const char* className() const{ return "kdw.DarkOverlay"; }
	DarkOverlay();

	void set(const Recti& rect);

	void redraw(HDC dc);
	void onMessagePaint();
	BOOL onMessageEraseBkgnd(HDC dc);
	void show(bool visible = true);
protected:
	void drawRow(HDC dc);
	void updateLayeredWindow();

	Recti rect_;
	HBITMAP bitmap_;
	COLORREF* bitmapBits_;
};

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list
DarkOverlay::DarkOverlay()
{
	DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;

	exStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT;
	VERIFY(create("", WS_POPUP | WS_DISABLED, Recti(0, 0, 320, 320), 0, exStyle));
}
#pragma warning(pop)

void DarkOverlay::redraw(HDC dc)
{
	RECT rt = { 0, 0, rect_.width(), rect_.height() };
	FillRect(dc, &rt, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

void DarkOverlay::set(const Recti& rect)
{
	rect_ = rect;
	if(::IsWindow(*this)){
		bool visible = ::IsWindowVisible(*this);
		show(visible);
	}
}

void DarkOverlay::show(bool visible)
{
	updateLayeredWindow();
	SetWindowPos(*this, HWND_TOP, rect_.left(),  rect_.top(),
				 rect_.width(), rect_.height(), 
				 SWP_NOOWNERZORDER | (visible ? SWP_SHOWWINDOW : SWP_HIDEWINDOW)| SWP_NOACTIVATE);
	updateLayeredWindow();
}

BOOL DarkOverlay::onMessageEraseBkgnd(HDC dc)
{
	return TRUE;
}

void DarkOverlay::updateLayeredWindow()
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
		redraw(dc);

		POINT leftTop = { rect_.left(), rect_.top() };
		SIZE size = { rect_.width(), rect_.height() };
		POINT point = { 0, 0 };
		UpdateLayeredWindow(*this, realDC, &leftTop, &size, dc, &point, 0, &blendFunction, ULW_ALPHA);
		SetLayeredWindowAttributes(*this, 0, 64, ULW_ALPHA);
	}
	EndPaint(*this, &ps);
}

void DarkOverlay::onMessagePaint()
{
}

class SpaceHSplitter;
class SpaceVSplitter;
class SpaceSplitterImpl: public SplitterImpl, public sigslot::has_slots{
public:
	typedef SpaceSplitterImpl Self;

	SpaceSplitterImpl(Splitter* owner, SpaceBox* spaceBox);
	~SpaceSplitterImpl();

	void onMenuSplit(int splitterIndex);
	void onMenuJoin(int splitterIndex);

	void startJoin(int splitterIndex);

	// 0/1
	int hoveredHalf(int x, int y);
protected:
	void onMessageLButtonDown(UINT button, int x, int y);
	void onMessageRButtonDown(UINT button, int x, int y);
	void onMessageMouseMove(UINT button, int x, int y);
	bool joining_;
	int splitterIndex_;
	SpaceBox* space_;
	DarkOverlay* darkOverlay_;
};

#pragma warning(push)
#pragma warning(disable: 4355) //  'this' : used in base member initializer list
class SpaceHSplitter : public HSplitter{
public:
	explicit SpaceHSplitter(SpaceBox* space, int splitterSpacing = 0, int border = 0)
	: HSplitter(splitterSpacing, border, new SpaceSplitterImpl(this, space))
	, space_(space)
	{}
protected:
	Space* space_;
};
class SpaceVSplitter : public VSplitter{
public:
	explicit SpaceVSplitter(SpaceBox* space, int splitterSpacing = 0, int border = 0)
	: VSplitter(splitterSpacing, border, new SpaceSplitterImpl(this, space))
	, space_(space)
	{}
protected:
	SpaceBox* space_;
};
#pragma warning(pop)

SpaceSplitterImpl::SpaceSplitterImpl(Splitter* owner, SpaceBox* space)
: SplitterImpl(owner)
, space_(space)
, joining_(false)
, splitterIndex_(-1)
, darkOverlay_(new DarkOverlay())
{

}

SpaceSplitterImpl::~SpaceSplitterImpl()
{
	delete darkOverlay_;
}

void SpaceSplitterImpl::onMenuSplit(int splitterIndex)
{
	Workspace* workspace = space_->findWorkspace();
	ASSERT(workspace);
	workspace->startSplit();
}

void SpaceSplitterImpl::onMenuJoin(int splitterIndex)
{
	startJoin(splitterIndex);
}


int SpaceSplitterImpl::hoveredHalf(int x, int y)
{
	ASSERT(splitterIndex_ >= 0);
	Recti rect = getSplitterRect(splitterIndex_);
	if(owner_->vertical()){
		int center = rect.top() + rect.height() / 2;
		return y > center ? 1 : 0;
	}
	else{
		int center = rect.left() + rect.width() / 2;
		return x > center ? 1 : 0;
	}
}


void SpaceSplitterImpl::startJoin(int splitterIndex)
{
	SetCapture(*this);
    joining_ = true;
	splitterIndex_ = splitterIndex;
	darkOverlay_->show(true);
}


void SpaceSplitterImpl::onMessageLButtonDown(UINT button, int x, int y)
{
	if(joining_){
		int half = hoveredHalf(x, y);
		owner_->remove(splitterIndex_ + half, half ? true : false);
		space_->removeSpace(splitterIndex_ + half);

		joining_ = false;
		splitterIndex_ = -1;
		darkOverlay_->show(false);
	}
	__super::onMessageLButtonDown(button, x, y);
}

void SpaceSplitterImpl::onMessageRButtonDown(UINT button, int x, int y)
{
	if(joining_){
		ReleaseCapture();
		joining_ = false;
		splitterIndex_ = 0;
		darkOverlay_->show(false);
	}
	else{
		int splitterIndex = splitterByPoint(Vect2i(x, y));
		if(splitterIndex >= 0){
			PopupMenu menu(100);

			menu.root().add(TRANSLATE("Разделить"), splitterIndex).connect(this, &Self::onMenuSplit);
			menu.root().add(TRANSLATE("Объеденить"), splitterIndex).connect(this, &Self::onMenuJoin);

			menu.spawn(owner_);
		}
	}
	__super::onMessageRButtonDown(button, x, y);
}

void SpaceSplitterImpl::onMessageMouseMove(UINT button, int x, int y)
{
	if(joining_){
		SetCursor(LoadCursor(0, IDC_ARROW));
		Widget* widget = owner_->widgetByIndex(splitterIndex_ + hoveredHalf(x, y));
		Win32::Rect rect(widget->_position());
		clientToScreen(rect);
        darkOverlay_->set(rect.recti());
	}
	__super::onMessageMouseMove(button, x, y);
}

// ---------------------------------------------------------------------------

SpaceHBox::SpaceHBox()	
{
	splitter_ = new SpaceHSplitter(this);
}

SpaceHBox::SpaceHBox(Space* oldSpace, Vect2i screenSplitPoint)
{
	Widget* widget = oldSpace->findWidget();
	ASSERT(widget);
	Win32::Window32* window = _findWindow(widget);
	ASSERT(window);
	HWND wnd = *window;
	ASSERT(::IsWindow(wnd));

	RECT rect;
	::GetClientRect(wnd, &rect);
	::ClientToScreen(wnd, (POINT*)&rect);
	::ClientToScreen(wnd, (POINT*)&rect + 1);
	float pos = max(0.0f, min(1.0f, (float(screenSplitPoint.x) - float(rect.left)) / float(rect.right - rect.left)));
    
	spaces_.push_back(oldSpace);
	oldSpace->setParent(this);

	
	Space* secondSpace = oldSpace->clone();
	spaces_.push_back(secondSpace);
	secondSpace->setParent(this);

	splitter_ = new SpaceHSplitter(this);
	{
		splitter_->add(oldSpace->widget(), pos, false);
		splitter_->add(secondSpace->widget(), pos, false);
	}
}

// ---------------------------------------------------------------------------

SpaceVBox::SpaceVBox()
{
	splitter_ = new SpaceVSplitter(this);
}

SpaceVBox::SpaceVBox(Space* oldSpace, Vect2i screenSplitPoint)
{
	Widget* widget = oldSpace->findWidget();
	ASSERT(widget);
	Win32::Window32* window = _findWindow(widget);
	ASSERT(window);
	HWND wnd = *window;
	ASSERT(::IsWindow(wnd));

	RECT rect;
	::GetClientRect(wnd, &rect);
	::ClientToScreen(wnd, (POINT*)&rect);
	::ClientToScreen(wnd, (POINT*)&rect + 1);
	float pos = max(0.0f, min(1.0f, (float(screenSplitPoint.y) - float(rect.top)) / float(rect.bottom - rect.top)));
    
	spaces_.push_back(oldSpace);
	oldSpace->setParent(this);

	Space* secondSpace = oldSpace->clone();
	spaces_.push_back(secondSpace);
	secondSpace->setParent(this);

	splitter_ = new SpaceVSplitter(this);
	{
		splitter_->add(oldSpace->widget(), pos, false);
		splitter_->add(secondSpace->widget(), pos, false);
	}
}

// ---------------------------------------------------------------------------

class SpaceHeaderImpl : public _WidgetWindow{
public:
	SpaceHeaderImpl(SpaceHeader* owner);
	void redraw(HDC dc);
	void onMessagePaint();
	void onMessageLButtonDown(UINT button, int x, int y);
	void onMessageRButtonDown(UINT button, int x, int y);
protected:

	SpaceHeader* owner_;
};

SpaceHeaderImpl::SpaceHeaderImpl(SpaceHeader* owner)
: _WidgetWindow(owner)
, owner_(owner)
{
	create("", WS_CHILD, Recti(0, 0, 20, 20), *Win32::_globalDummyWindow);
}


struct ClassMenuItemAdderSpace : ClassMenuItemAdder{
	ClassMenuItemAdderSpace(SpaceHeader* spaceHeader, int selected)
	: spaceHeader_(spaceHeader)
	, selectedIndex_(selected)
	{
	}
	void operator()(PopupMenuItem& root, int index, const char* text){
		if(index >= 0 && strcmp(text, "SpaceHBox") != 0 && strcmp(text, "SpaceVBox") != 0){
			root.add(text, index).connect(spaceHeader_, &SpaceHeader::onMenuReplaceSpace)
				.check(index == selectedIndex_);
		}
	}
protected:
	SpaceHeader* spaceHeader_;
	int selectedIndex_;
};


void SpaceHeaderImpl::onMessageLButtonDown(UINT button, int x, int y)
{
	ShareHandle<Widget> ownerReference(owner_);
	RECT rect;
	::GetClientRect(*this, &rect);
	::ClientToScreen(*this, (POINT*)&rect);
	::ClientToScreen(*this, (POINT*)&rect + 1);
	
	PopupMenu menu(100);
    
	int index = -1;
	if(owner_->space_)
		index = SerializationFactory<Space, FactoryArg0<Space> >::instance().typeIndexByName(typeid(*(owner_->space_)).name());

	ClassMenuItemAdderSpace(owner_, index).generateMenu(menu.root(), SerializationFactory<Space, FactoryArg0<Space> >::instance().comboStringsAlt());
	
	menu.spawn(Vect2i(rect.left, rect.bottom), owner_);
	
	Win32::Window32::onMessageLButtonDown(button, x, y);
}

void SpaceHeaderImpl::onMessageRButtonDown(UINT button, int x, int y)
{
	PopupMenu menu(100);
	menu.root().add(TRANSLATE("Разделить")).connect(owner_, &SpaceHeader::onMenuSplit);

	menu.spawn(owner_);
	Win32::Window32::onMessageRButtonDown(button, x, y);
}

void SpaceHeaderImpl::redraw(HDC dc)
{
	Win32::Rect clientRect;
	::GetClientRect(*this, &clientRect);

	RECT lineRect = clientRect;
	lineRect.top = lineRect.bottom - 1;
	::FillRect(dc, &lineRect, GetSysColorBrush(COLOR_3DSHADOW));
	
	lineRect.left = clientRect.right - 1;
	lineRect.top = clientRect.top;
	::FillRect(dc, &lineRect, GetSysColorBrush(COLOR_3DSHADOW));

	Win32::Rect buttonRect = clientRect;
	buttonRect.right = buttonRect.left + GetSystemMetrics(SM_CXVSCROLL);
	::DrawFrameControl(dc, &buttonRect, DFC_SCROLL, DFCS_SCROLLDOWN);

	Win32::Rect textRect = clientRect;
	textRect.right -= 1;
	textRect.left = buttonRect.right;
	::FillRect(dc, &textRect, GetSysColorBrush(COLOR_APPWORKSPACE));
	textRect.left += 2;
	textRect.right -= 2;
	const char* text = owner_->label();
	HFONT font = Win32::defaultFont();
	HFONT oldFont = (HFONT)::SelectObject(dc, font);
	int oldBkMode = ::SetBkMode(dc, TRANSPARENT);
	::DrawText(dc, text, strlen(text), &textRect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	::SelectObject(dc, oldFont);
	::SetBkMode(dc, oldBkMode);
}

void SpaceHeaderImpl::onMessagePaint()
{

	PAINTSTRUCT paintStruct;
	HDC dc = BeginPaint(*this, &paintStruct);

	redraw(dc);

	EndPaint(*this, &paintStruct);
}

// ---------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable: 4355) //  'this' : used in base member initializer list
SpaceHeader::SpaceHeader(Space* space, int border)
: _WidgetWithWindow(new SpaceHeaderImpl(this), border)
, space_(space)
{
	const char* label = SerializationFactory<Space, FactoryArg0<Space> >::instance().nameAlt(typeid(*space).name(), true);
	if(label)
		label_ = label;
	else
		label_ = typeid(*space).name();

	updateMinimalSize();
}
#pragma warning(pop)

void SpaceHeader::onMenuSplit()
{
	ASSERT(space());
	Workspace* workspace = space()->findWorkspace();
	ASSERT(workspace);
	workspace->startSplit();
}

void SpaceHeader::onMenuReplaceSpace(int index)
{
	Space::Factory& factory = Space::Factory::instance();
	
	if(factory.typeIndexByName(typeid(*space_).name(), true) != index){
		Space* newSpace = factory.createByIndex(index);
		ASSERT(space_->parent());
		space_->parent()->replaceSpace(space_, newSpace);
	}
}

void SpaceHeader::setLabel(const char* label)
{
	label_ = label;
	updateMinimalSize();
}

void SpaceHeader::serialize(Archive& ar)
{

}

void SpaceHeader::updateMinimalSize()
{
	Vect2i size = Win32::calculateTextSize(*Win32::_globalDummyWindow, Win32::defaultFont(), label_.c_str());
	size += Vect2i(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE)) * 2;
	size.x += GetSystemMetrics(SM_CXVSCROLL) + 4;
	_setMinimalSize(size);
}

};
