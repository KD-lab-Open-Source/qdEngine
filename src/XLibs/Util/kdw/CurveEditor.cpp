#include "StdAfx.h"
#include "kdw/_WidgetWindow.h"
#include <windowsx.h>
#include <windows.h>
#include "kdw/Win32/Rectangle.h"
#include "kdw/Win32/MemoryDC.h"
#include "kdw/Win32/Handle.h"

#include "kdw/CurveEditor.h"
#include "kdw/CurveEditorElement.h"

namespace {

Win32::Handle<HFONT> tickFont = CreateFont(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
										   "Verdana");
Win32::Handle<HPEN> timelinePen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
Win32::Handle<HPEN> timelinePenTinted = CreatePen(PS_DOT, 1, RGB(200, 200, 200));
Win32::Handle<HBRUSH> timelineBrush = CreateSolidBrush(RGB(235, 235, 235));

}

namespace kdw{

class CurveEditorImpl: public _WidgetWindow{
public:
	const char* className() const{ return "kdw.CurveEditor"; }
	CurveEditorImpl(CurveEditor* owner);

	void onMessagePaint();
	void onMessageLButtonDown(UINT button, int x, int y);
	void onMessageLButtonUp(UINT button, int x, int y);
	void onMessageRButtonDown(UINT button, int x, int y);
	void onMessageMouseMove(UINT button, int x, int y);
	int onMessageSize(UINT type, USHORT width, USHORT height);
	BOOL onMessageEraseBkgnd(HDC dc);

	void moveTo(HDC dc, float x, float y);
	void lineTo(HDC dc, float x, float y);
	void textOut(HDC dc, float x, float y, const char* text);
	void textOut(HDC dc, float x, float y, float value, int digits);

	void drawAxes(HDC dc);
	void redraw(HDC dc, const ViewTransform& view);
	void recalculateViewport();
	void rebuild();
protected:
	CurveEditor* owner_;

	ShareHandle<CurveEditorTimeline> timeline_;

	ShareHandle<CurveEditorElement> selectedElement_;
	ViewTransform view_;
	Vect2i lastMousePosition_;
	
	typedef std::vector<ShareHandle<CurveEditorElement> > Elements;
	Elements elements_;

	friend class CurveEditor;
};

CurveEditorImpl::CurveEditorImpl(CurveEditor* owner)
: _WidgetWindow(owner)
, owner_(owner)
, selectedElement_(0)
, timeline_(new CurveEditorTimeline(owner))
{
	create("", WS_CHILD, Recti(0, 0, 40, 40), *Win32::_globalDummyWindow, WS_EX_CLIENTEDGE);
}

void CurveEditorImpl::onMessagePaint()
{
	PAINTSTRUCT paintStruct;
	HDC dc = BeginPaint(*this, &paintStruct);
	{
		Win32::MemoryDC memoryDC(dc);
		redraw(memoryDC, view_);
	}
	EndPaint(*this, &paintStruct);
}


void CurveEditorImpl::onMessageLButtonDown(UINT button, int x, int y)
{
	Elements::iterator it;
	Vect2f point = view_.untransform(Vect2i(x, y));

	SetFocus(*this);

	CurveEditorElement* element = timeline_->hitTest(point, view_);
	if(element){
		selectedElement_ = element;
		SetCapture(*this);
	}

	FOR_EACH(elements_, it){
		CurveEditorElement* element = (*it)->hitTest(point, view_);
		if(element){
			selectedElement_ = element;
			SetCapture(*this);
		}
		else
			(*it)->click(point, view_);
	}

	if(selectedElement_){
		selectedElement_->click(point, view_);
	}

	InvalidateRect(*this, 0, FALSE);
}

void CurveEditorImpl::onMessageLButtonUp(UINT button, int x, int y)
{
	if(GetCapture() == *this)
		ReleaseCapture();


	if(selectedElement_){
  		selectedElement_->commit();
		//updateElements();
	}
	selectedElement_ = 0;
}

void CurveEditorImpl::onMessageRButtonDown(UINT button, int x, int y)
{
	Elements::iterator it;
	Vect2f point = view_.untransform(Vect2i(x, y));

	SetFocus(*this);

	selectedElement_ = 0;
	FOR_EACH(elements_, it){
		CurveEditorElement* element = (*it)->hitTest(point, view_);
		if(element)
			selectedElement_ = element;
	}
	if(selectedElement_){
		if(selectedElement_->removable()){
			selectedElement_->remove();
			selectedElement_ = 0;
		}
	}
}

void CurveEditorImpl::onMessageMouseMove(UINT button, int x, int y)
{
	POINT cursorPosition;
	GetCursorPos(&cursorPosition);
	ScreenToClient(*this, &cursorPosition);
	Vect2i position(cursorPosition.x, cursorPosition.y);
	if(selectedElement_){
		Vect2f delta = view_.untransform(position) - view_.untransform(lastMousePosition_);
		selectedElement_->move(delta, view_);
		//selectedElement_->commit();
		// updateElements();
		InvalidateRect(*this, 0, FALSE);
	}
    lastMousePosition_.set(cursorPosition.x, cursorPosition.y);
}

int CurveEditorImpl::onMessageSize(UINT type, USHORT width, USHORT height)
{
	//rebuild();
	recalculateViewport();
	return 0;
}

BOOL CurveEditorImpl::onMessageEraseBkgnd(HDC dc)
{
	return FALSE;
}

void CurveEditorImpl::redraw(HDC dc, const ViewTransform& view)
{
	RECT rect;
	GetClientRect(*this, &rect);

	//::SelectObject(dc, GetStockObject
	::FillRect(dc, &rect, GetSysColorBrush(COLOR_WINDOW));

	drawAxes(dc);

	Elements::iterator it;
	FOR_EACH(elements_, it){
		CurveEditorElement* element = *it;
		element->redraw(dc, view);
	}
	timeline_->redraw(dc, view);
}

void CurveEditorImpl::moveTo(HDC dc, float x, float y)
{
	Vect2i point = view_(Vect2f(x, y));
	MoveToEx(dc, point.x, point.y, 0);
}

void CurveEditorImpl::lineTo(HDC dc, float x, float y)
{
	Vect2i point = view_(Vect2f(x, y));
	LineTo(dc, point.x, point.y);
}

void CurveEditorImpl::textOut(HDC dc, float x, float y, const char* text)
{
	Vect2i point = view_(Vect2f(x, y));
	TextOut(dc, point.x, point.y, text, strlen(text));
}

void CurveEditorImpl::textOut(HDC dc, float x, float y, float value, int digits)
{
	char buf[15];
	sprintf(buf, "%.1f", value);
	Vect2i point = view_(Vect2f(x, y));
	TextOut(dc, point.x, point.y, buf, strlen(buf));
}

void CurveEditorImpl::drawAxes(HDC dc)
{
	static Win32::Handle<HPEN> penAxis(CreatePen(PS_SOLID, 2, RGB(0, 0, 0)));
	Rectf bn = view_.innerBounds(); 
	bool showYlabel = true;

	/*
	if (!_pDoc->ActiveEmitter()){
		int n = _pDoc->ActiveEffect()->EmittersSize();
		bn.max = n+1;
		bn.min = 0;
		yScale = work.Height()/(bn.max-bn.min);
		Y_Ox = 0;
		showYlabel = false;
	}
	*/

	SelectObject(dc, penAxis);

	moveTo(dc, 0.0f, bn.top());
	lineTo(dc, 0.0f, bn.bottom());
	moveTo(dc, bn.left(), 0.0f);
	lineTo(dc, bn.right(), 0.0f);

	SelectObject(dc, GetStockObject(BLACK_PEN));
	ASSERT(tickFont != 0);
	SelectObject(dc, tickFont);
	SetTextAlign(dc, TA_TOP|TA_CENTER);
	SetBkMode(dc, TRANSPARENT);

	float dt = 0.1f;
	if (bn.width() > 3.0f)
		dt = 0.5f;
	if(bn.width() > 10.0f)
		dt = 1.0f;
	if(bn.width() > 30.0f)
		dt = 10.0f;

	int tick_size = 6;
	xassert(bn.right() < 1e5f);
	for(float t = 0.0f; t <= bn.right(); t += dt){
		moveTo(dc, t, 0);
		lineTo(dc, t, float(tick_size) / view_.scale().y);
		if(t)
			textOut(dc, t, 0, t, 1);
	}

	if(showYlabel){
		float dy = 0.1f;
		if(bn.height() > 1) dy = 0.5f;
		if(bn.height() > 5) dy = 1;
		if(bn.height() > 10) dy = 2;
		if(bn.height() > 20) dy = 4;
		if(bn.height() > 40) dy = 8;
		if(bn.height() > 80) dy = 10;
		if(bn.height() > 100) dy = 20;
		if(bn.height() > 200) dy = 40;

		for(float y = 0.0f; y <= bn.bottom(); y += dy){
			moveTo(dc, 0, y);
			lineTo(dc, tick_size / view_.scale().x, y);
			textOut(dc, -2.3f * tick_size / view_.scale().x, y + 5 / view_.scale().y, y, 1);
		}
		for(float y = -dy; y >= bn.top(); y -= dy){
			moveTo(dc, 0, y);
			lineTo(dc, tick_size/view_.scale().x, y);
			textOut(dc, -2.3f*tick_size/view_.scale().x, y+5/view_.scale().y, y, 1);
		}
	}
}

void CurveEditorImpl::rebuild()
{
	OutputDebugString("CurveEditorImpl::updateElements()\n");
	Elements::iterator it;
	FOR_EACH(elements_, it){
		CurveEditorElement* element = *it;
		element->rebuild();
	}
	recalculateViewport();
}

void CurveEditorImpl::recalculateViewport()
{
	OutputDebugString("CurveEditorImpl::recalculateViewport()\n");
	Rectf bound(0.0f, 0.0f, max(owner_->duration(), 0.1f), 0.1f);

	Elements::iterator it;
	FOR_EACH(elements_, it){
		CurveEditorElement* element = *it;
		Rectf temp;
		if(element->getBoundingBox(temp))
			bound = bound.merge(temp);
	}
	Rectf temp;
	if(timeline_->getBoundingBox(temp))
		bound = bound.merge(temp);

	Vect2f scale(1.0f, 1.0f);
	Vect2f offset(0.0f, 0.0f);
	Vect2i size;
	
	if(bound.width() > 0.0f && bound.height() > 0.0f){
		Win32::Rect clientRect;
		GetClientRect(*this, &clientRect);
		size = Vect2i(clientRect.width(), clientRect.height());
		Rectf rect(float(clientRect.left),    float(clientRect.top),
			       float(clientRect.width()), float(clientRect.height()));

		float left_border = 30.0f;
		float right_border = 30.0f;

		float top_border = 30.0f;
		float bottom_border = 60.0f;

		scale = Vect2f(float(clientRect.width() - left_border - right_border) / bound.width(), -float(clientRect.height() - top_border - bottom_border) / bound.height());
		offset = bound.size() * Vect2f(left_border / (clientRect.width() - left_border - right_border), -top_border / (clientRect.height() - top_border - bottom_border))- bound.left_bottom();
	}
	float outerWidth = size.x / scale.x;
	float outerHeight = -size.y / scale.y;
	view_.set(offset, scale, bound, Rectf(Vect2f(-offset.x, -offset.y - outerHeight), Vect2f(outerWidth, outerHeight)));
	XBuffer buf;
	buf <=	

 	InvalidateRect(*this, 0, FALSE);
}

// ---------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable: 4355) //  'this' : used in base member initializer list

CurveEditor::CurveEditor(int border)
: _WidgetWithWindow(new CurveEditorImpl(this), border)
, time_(0.0f)
, duration_(1.0f)
{

}
#pragma warning(pop)

void CurveEditor::clear()
{
	impl().selectedElement_ = 0;
	impl().elements_.clear();
	impl().recalculateViewport();
}

void CurveEditor::addElement(CurveEditorElement* element)
{
	element->setEditor(this);
	impl().elements_.push_back(element);
	impl().selectedElement_ = 0;
	impl().recalculateViewport();
	RedrawWindow(impl(), 0, 0, RDW_INVALIDATE);
}

CurveEditorImpl& CurveEditor::impl()
{
	return static_cast<CurveEditorImpl&>(*_window());
}

void CurveEditor::recalculateViewport()
{
	impl().recalculateViewport();
}

void CurveEditor::rebuild()
{
	impl().rebuild();
}

float CurveEditor::time() const
{
	return time_;
}

void CurveEditor::setTime(float time)
{
	time_ = clamp(time, 0.0f, duration_);
	InvalidateRect(impl(), 0, FALSE);
}

void CurveEditor::setDuration(float duration)
{
	duration_ = duration;
}

void CurveEditor::setSelectedElement(CurveEditorElement* element, bool grab)
{
	impl().selectedElement_ = element;
	if(grab)
		::SetCapture(impl());
}

// ---------------------------------------------------------------------------
CurveEditorElement::CurveEditorElement()
: position_(Vect2f::ZERO)
, editor_(0)
{
}

void CurveEditorElement::redraw(HDC dc, const ViewTransform& transform)
{
}

// ---------------------------------------------------------------------------

CurveEditorTimeline::CurveEditorTimeline(CurveEditor* editor)
: editor_(editor)
{

}


bool CurveEditorTimeline::getBoundingBox(Rectf& rect)
{
	//rect = Rectf(0.0f, -0.2f, 0.1f, 0.1f);
	return false;
}



void CurveEditorTimeline::redraw(HDC dc, const ViewTransform& view)
{
	HGDIOBJ oldBrush = ::SelectObject(dc, HGDIOBJ(timelineBrush));
	HGDIOBJ oldPen = ::SelectObject(dc, HGDIOBJ(timelinePenTinted));

	float current_time = editor_->time();
	{
		Vect2i start(view.transformX(current_time), view.viewOuterBounds().top());
		Vect2i end(view.transformX(current_time), view.viewOuterBounds().bottom());
		MoveToEx(dc, start.x, start.y, 0);
		LineTo(dc, end.x, end.y);
	}

	// прямоугольник с рисочкой таймлайна
	::SelectObject(dc, HGDIOBJ(timelinePen));

	Recti timelineRect(this->timelineRect(view));
	::Rectangle(dc, timelineRect.left(), timelineRect.top(), timelineRect.right(), timelineRect.bottom());

	// сама рисочка
	Vect2i start(view.transformX(current_time), timelineRect.top());
	Vect2i end(view.transformX(current_time), timelineRect.bottom());
	MoveToEx(dc, start.x, start.y, 0);
	LineTo(dc, end.x, end.y);

	::SelectObject(dc, oldBrush);
	::SelectObject(dc, oldPen);
}


void CurveEditorTimeline::commit()
{
}

Recti CurveEditorTimeline::timelineRect(const ViewTransform& view)
{
	float height = view.viewOuterBounds().bottom() - view.viewInnerBounds().bottom();
	return Recti(view.viewInnerBounds().left(), view.viewInnerBounds().bottom() + round(height * 0.5f),
				 view.viewInnerBounds().width(), round(height * 0.4f));
}

void CurveEditorTimeline::move(Vect2f delta, const ViewTransform& view)
{
	float deltaTime = delta.x;
	editor_->setTime(editor_->time() + deltaTime);
	editor_->signalTimeChanged().emit();
}

CurveEditorElement* CurveEditorTimeline::hitTest(Vect2f position, const ViewTransform& view)
{
	Recti timelineRect = this->timelineRect(view);
	if(timelineRect.point_inside(view(position))){
		float time = position.x;
		editor_->setTime(time);
		editor_->signalTimeChanged().emit();
		return this;
	}
	return 0;
}

};
