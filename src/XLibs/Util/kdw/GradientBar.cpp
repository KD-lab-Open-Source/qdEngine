#include "StdAfx.h"
#include "kdw/GradientBar.h"
#include "kdw/ColorChooserDialog.h"
#include "Render/src/NParticle.h"

#include "kdw/Win32/Drawing.h"
#include "kdw/_WidgetWindow.h"
#include "kdw/Win32/Rectangle.h"
#include "kdw/Win32/MemoryDC.h"

void drawGradient(HDC dc, const RECT& rect, const KeysColor& gradient, bool alphaOnly)
{
	if (gradient.empty())
		return;

    KeysColor::const_iterator it;
    float last_time = 0.0f;

	Rectf wholeRect(float(rect.left), float(rect.top), float(rect.right - rect.left), float(rect.bottom - rect.top));

	KeyColor lastKey(gradient.front());
	if(gradient.size() == 1)
		Win32::drawColorBlend(dc, wholeRect, lastKey, lastKey);
	else{
		FOR_EACH (gradient, it) {
			Recti blendRect(round(wholeRect.left() + wholeRect.width() * lastKey.time), int(wholeRect.top()),
							max (1, round(wholeRect.width() * (it->time - lastKey.time))), int(wholeRect.height()));
			blendRect.right(min(blendRect.right(), rect.right - 1));
			Win32::drawColorBlend(dc, blendRect, lastKey, *it, alphaOnly);
			lastKey = *it;
		}
	}
}

namespace kdw{

class GradientBarImpl : public _WidgetWindow{
public:
	GradientBarImpl(GradientBar* owner);
	const char* className()const{ return "kdw.GradientBar"; }

	void onMessageLButtonDblClk(int x, int y);
	void onMessageLButtonDown(UINT button, int x, int y);
	void onMessageLButtonUp(UINT button, int x, int y);
	void onMessageRButtonDown(UINT button, int x, int y);
	void onMessageRButtonUp(UINT button, int x, int y);

	void onMessageMouseMove(UINT button, int x, int y);
	void onMessagePaint();
	BOOL onMessageEraseBkgnd(HDC dc);
	void redraw(HDC dc);
	void drawPegs(HDC dc, const RECT& rect, const KeysColor& gradient, int selected, bool alphaOnly = false);
	void update();

	void setSelection(int index);
	void moveSelection(float position);
	int getPegIndexAt(float pos, float precision);
	int getSelection() const;
	void setCurrentColor(const Color4f& color);
protected:
	GradientBar* owner_;

	int selection_;
	bool moving_;
	float lastMousePosition_;
	Vect2i lastMousePoint_;
	bool fixedPointsCount_;

	friend class GradientBar;
};

// ---------------------------------------------------------------------------


GradientBarImpl::GradientBarImpl(GradientBar* owner)
: _WidgetWindow(owner)
, owner_(owner)
, selection_(-1)
, lastMousePosition_(0.0f)
, lastMousePoint_(Vect2i::ZERO)
{
	create("", WS_CHILD, Recti(0, 0, 40, 40), *Win32::_globalDummyWindow, WS_EX_CLIENTEDGE);
}

void GradientBarImpl::onMessagePaint()
{
	PAINTSTRUCT paintStruct;
	HDC dc = BeginPaint(*this, &paintStruct);
	{
		Win32::MemoryDC memoryDC(dc);
		redraw(memoryDC);
	}
	EndPaint(*this, &paintStruct);
}

void GradientBarImpl::onMessageLButtonDblClk(int x, int y)
{
	KeysColor& gradient = owner_->gradient_;

	Win32::Rect rt;
	GetClientRect(*this, &rt);

	SetFocus(*this);

    float pos = float(x) / float(rt.width());
	float precision = 4.0f / float(rt.width());

	lastMousePosition_ = pos;
	int peg = getPegIndexAt(pos, precision);

	if(peg != -1){
		setSelection(peg);

		ColorChooserDialog dialog(owner_->mainWindow(), gradient[peg].Val(), owner_->hasColor_, owner_->hasAlpha_);
		if(dialog.showModal() == RESPONSE_OK){
			gradient[peg].Val() = dialog.get();
			if(owner_->cycled_ && gradient.size() > 1){
				if(peg == 0)
					gradient[gradient.size() - 1].Val() = gradient[peg].Val();
				if(peg == gradient.size() - 1)
					gradient[0].Val() = gradient[peg].Val();
			}
			RedrawWindow(*this, 0, 0, RDW_INVALIDATE);
		}
		owner_->signalChanged_.emit();
	}
	
}

void GradientBarImpl::onMessageLButtonDown(UINT button, int x, int y)
{
	KeysColor& gradient = owner_->gradient_;

	Win32::Rect rt;
	GetClientRect(*this, &rt);

	SetFocus(*this);

    float pos = float(x) / float(rt.width());
	float precision = 4.0f / float(rt.width());

	lastMousePosition_ = pos;
	int peg = getPegIndexAt(pos, precision);

	if(peg != -1){
		setSelection(peg);
		owner_->signalChanged_.emit();
	}
	else{
		if(owner_->allowAddPoints_ && !owner_->gradient_.empty()){
			KeyColor* newKey = &gradient.InsertKey(pos);
			int index = -1;
			for(int i = 0; i < gradient.size(); ++i){
				if(&gradient[i] == newKey){
					index = i;
					break;
				}
			}
			if(gradient.size() == 2){
				newKey->time = 1.0f;
				*static_cast<Color4f*>(newKey) = gradient [0];
			}
			else if(gradient.size() > 2){
				xassert(index > 0 && index < gradient.size() - 1);
				float scale = (newKey->time - gradient[index-1].time) / (gradient[index+1].time - gradient[index-1].time);
				static_cast<Color4f*>(newKey)->interpolate(gradient[index-1], gradient[index+1], scale);
			}
			else{
				index = -1;
			}
			setSelection(index);
		}
	}

	if(selection_ != 0 && selection_ != gradient.size () - 1){
		SetCapture(*this);
		moving_ = true;
	}
	else{
		moving_ = false;
	}
	RedrawWindow(*this, 0, 0, RDW_UPDATENOW | RDW_INVALIDATE);
	__super::onMessageLButtonDown(button, x, y);
}

void GradientBarImpl::onMessageLButtonUp(UINT button, int x, int y)
{
	if(moving_){
		moving_ = false;
		if(GetCapture() == *this)
			ReleaseCapture();
		owner_->signalChanged_.emit();
		UpdateWindow(*this);
	}
	__super::onMessageLButtonUp(button, x, y);
}

void GradientBarImpl::onMessageRButtonDown(UINT button, int x, int y)
{
	KeysColor& gradient = owner_->gradient_;

	Win32::Rect rt;
	GetClientRect(*this, &rt);

	SetFocus(*this);

    float pos = float(x) / float(rt.width());
	float precision = 4.0f / float(rt.width());

	lastMousePosition_ = pos;
	int peg = getPegIndexAt(pos, precision);

	if(peg != -1){
		setSelection(peg);
		owner_->signalChanged_.emit();
	}

    if(selection_ == -1 || selection_ == 0 || selection_ >= gradient.size() - 1)
        return;

	if(owner_->allowAddPoints_){
		if(selection_ > 0 && ((selection_ < gradient.size() - 1) ||
			(gradient.size () == 2 && selection_ == 1))){
			gradient.erase(gradient.begin () + selection_);
			setSelection(-1);
			owner_->signalChanged_.emit();
		}
	}

	lastMousePosition_ = pos;
	POINT screenPoint = { x, y };
	ClientToScreen(*this, &screenPoint);
	lastMousePoint_.set(screenPoint.x, screenPoint.y);
    moving_ = true;

    SetCapture(*this);

	RedrawWindow(*this, 0, 0, RDW_UPDATENOW | RDW_INVALIDATE);

	__super::onMessageRButtonDown(button, x, y);
}

void GradientBarImpl::onMessageRButtonUp(UINT button, int x, int y)
{
	if(moving_){
		moving_ = false;
		if(GetCapture() == *this){
			ReleaseCapture();
		}
	
		RedrawWindow(*this, 0, 0, RDW_UPDATENOW | RDW_INVALIDATE);
	}
	
	return __super::onMessageRButtonUp(button, x, y);
}

void GradientBarImpl::onMessageMouseMove(UINT button, int x, int y)
{
	KeysColor& gradient = owner_->gradient_;
	if(moving_ && selection_ != -1){
		xassert (selection_ > 0 && selection_ < gradient.size () - 1);
		Win32::Rect rt;
		GetClientRect(*this, &rt);
		float pos = float(x) / float(rt.width());
		float old_pos = gradient[selection_].time;

		float new_pos = old_pos;
		if((button & MK_SHIFT) && (button & MK_RBUTTON))
			new_pos += (pos - lastMousePosition_) / 10.0f;
		else
			new_pos += pos - lastMousePosition_;

		if(new_pos < 0.0f)
		   new_pos = 0.0f;
		if(new_pos > 1.0f)
		   new_pos = 1.0f;

		if(old_pos < new_pos){
			for(int i = 0; i < gradient.size() - 1; ++i){
				if(old_pos < gradient[i].time && gradient[i].time <= new_pos){
					std::swap(gradient[i], gradient[selection_]);
					selection_ = i;
				}
			}
		}
		else if(old_pos > new_pos){
			for(int i = gradient.size() - 1; i > 0; --i){
				if(old_pos > gradient[i].time && gradient[i].time >= new_pos){
					std::swap(gradient[i], gradient[selection_]);
					selection_ = i;
				}
			}
		}
		if(selection_ > gradient.size())
			selection_ = -1;
		//xassert(selection_ >= 0 && selection_ < gradient.size ());

		lastMousePosition_ = pos;
		gradient[selection_].time = new_pos;
		setSelection(selection_);
		RedrawWindow(*this, 0, 0, RDW_UPDATENOW | RDW_INVALIDATE);
	}
	__super::onMessageMouseMove(button, x, y);
}

BOOL GradientBarImpl::onMessageEraseBkgnd(HDC dc)
{
	return FALSE;
}

void GradientBarImpl::redraw(HDC dc)
{
	RECT rect;
	GetClientRect(*this, &rect);

	FillRect(dc, &rect, GetSysColorBrush(COLOR_APPWORKSPACE));

	bool alphaOnly = !owner_->hasColor_ && owner_->hasAlpha_;
	if(!owner_->gradient_.empty()){
		drawGradient(dc, rect, owner_->gradient_, alphaOnly);
		drawPegs(dc, rect, owner_->gradient_, selection_, alphaOnly);
	}
}

void GradientBarImpl::drawPegs(HDC dc, const RECT& rect, const KeysColor& gradient, int selected, bool alphaOnly)
{
	KeysColor::const_iterator it;
	int index = 0;
	FOR_EACH(gradient, it){
		const KeyColor& key = *it;
		float time = key.time;
		int x = round(time * (rect.right - rect.left));
		COLORREF color = Win32::toColorRef(alphaOnly ? Color4f(key.a, key.a, key.a, 1.0f) : key);
		FillRect(dc, &Win32::Rect(x - 4, rect.top + 1, x + 4, rect.bottom - 1), HBRUSH(GetStockObject(BLACK_BRUSH)));
		FillRect(dc, &Win32::Rect(x - 3, rect.top + 2, x + 3, rect.bottom - 2), HBRUSH(GetStockObject(WHITE_BRUSH)));
		Win32::Rect pegRect(x - 2, rect.top + 3, x + 2, rect.bottom - 3);
		HBRUSH brush = CreateSolidBrush(color);
		FillRect(dc, &pegRect, brush);
        DeleteObject(brush);		
		++index;
	}
}

void GradientBarImpl::update()
{
	InvalidateRect(*this, 0, FALSE);
}

void GradientBarImpl::setSelection(int selection)
{
	selection_ = selection;
}

int GradientBarImpl::getPegIndexAt(float pos, float precision)
{
	KeysColor& gradient = owner_->gradient_;

	for(int i = 1; i < gradient.size (); ++i)
		if(fabs(gradient[i].time - pos) <= precision)
			return i;
	if(!gradient.empty() && fabs(gradient[0].time - pos) <= precision)
		return 0;
	return -1;
}

// ---------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list
GradientBar::GradientBar(int border)
: _WidgetWithWindow(new GradientBarImpl(this), border)
, hasAlpha_(true)
, hasColor_(true)
, cycled_(false)
{
	_setMinimalSize(Vect2i(20, 20));
}
#pragma warning(pop)

GradientBar::~GradientBar()
{
}

GradientBarImpl& GradientBar::impl()
{
	return *static_cast<GradientBarImpl*>(_window());
}


void GradientBar::set(const KeysColor& gradient)
{
	gradient_ = gradient;
	impl().update();
}

}
