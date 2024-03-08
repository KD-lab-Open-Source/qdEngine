#ifndef __KDW_VIEWPORT_2D_H_INCLUDED__
#define __KDW_VIEWPORT_2D_H_INCLUDED__

#include "kdw/Win32/Types.h"
#include "kdw/_WidgetWithWindow.h"
#include "kdw/Win32/Handle.h"
#include "XMath/Colors.h"
#include "sKey.h"

namespace kdw{
class Viewport2DImpl;

class KDW_API Viewport2D : public _WidgetWithWindow 
{
public:
	enum TextAlign {
		ALIGN_LEFT,
		ALIGN_CENTER,
		ALIGN_RIGHT
	};

	explicit Viewport2D(int border, int fontHeight);
	~Viewport2D();

	// updateNow влияет на то, положится ли сообщение перерисовки в очередь сообщений
	// (на потом) или вызовется непосредственно сейчас
	void redraw(bool updateNow = false);

	// для получения координат используется mousePosition()
	virtual void onMouseMove(const Vect2i& delta);
	virtual void onMouseButtonDown(MouseButton button);
	virtual void onMouseButtonUp(MouseButton button);

	virtual void onKeyDown(const sKey& key) {}
	virtual void onKeyUp(const sKey& key) {}

	virtual void onRedraw(HDC dc);
	virtual void onResize(int width, int height);

	void captureMouse();
	void releaseMouse();

	Vect2i size() const{ return size_; }
	Vect2i mousePosition() const{ return mousePosition_; }
	
	void setBackgroundColor(Color3c backgroundColor){
		backgroundColor_ = backgroundColor;
		redraw();
	}
	Color3c backgroundColor() const{ return backgroundColor_; }

	void drawPixel(HDC dc, const Vect2f& pos, const Color4c& color);
	void drawCircle(HDC dc, const Vect2f& pos, float radius, const Color4c& color, int outline_width);
	void drawLine(HDC dc, const Vect2f& start, const Vect2f& end, const Color4c& color, int style = PS_SOLID, int width = 1);
	void drawRectangle(HDC dc, const Rectf& rect, const Color4c& color);
	void fillRectangle(HDC dc, const Rectf& rect, const Color4c& color);
	void drawText(HDC dc, const Rectf& rect, const char* text, TextAlign align = ALIGN_CENTER, bool endEllipsis = false);
	void drawText(HDC dc, const Vect2f& pos, const char* text, const Color4c& textColor, const Color4c& backColor = Color4c::ZERO, TextAlign align = ALIGN_CENTER);

	float viewScale() const { return viewScale_; }
	float pixelWidth() const { return 1.f/viewScale_; }
	float pixelHeight() const { return 1.f/viewScale_; }
	Vect2f coordsToScreen(const Vect2f& pos) const;
	Vect2f coordsFromScreen(const Vect2f& pos) const;
	void centerOn(const Vect2f& point) { viewOffset_ = -point; }
	Rectf visibleArea() const;
	void setVisibleArea(const Rectf& rect);

	bool lmbPressed() const { return lmbPressed_; }
	bool rmbPressed() const { return rmbPressed_; }
	bool scrolling() const { return scrolling_; }

	Vect2f clickPoint() const { return coordsFromScreen(mousePosition()); }

protected:
	int fontHeight_;
	Win32::Handle<HFONT> positionFont_;

	Vect2i viewSize_;
	Vect2f viewOffset_;
	float viewScale_;
	int zoomIndex_;

	Color3c backgroundColor_;

	bool lmbPressed_;
	bool rmbPressed_;
	bool scrolling_;

	Vect2i mousePosition_;
	Vect2i size_;

	void createFont();

	Viewport2DImpl* impl();
	friend class Viewport2DImpl;
};

}

#endif
