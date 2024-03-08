#ifndef __KDW_VIEWPORT_H_INCLUDED__
#define __KDW_VIEWPORT_H_INCLUDED__

#include "XMath/Colors.h"
#include "kdw/_WidgetWithWindow.h"
#include "sKey.h"

class cScene;
class cRenderWindow;
class cInterfaceRenderDevice;

namespace Win32{
	class Window;
}

namespace kdw{

class RenderWindow;
class ViewportImpl;
/// сигналов
class KDW_API Viewport : public _WidgetWithWindow{
public:
	explicit Viewport(cInterfaceRenderDevice* renderDevice, bool continuousUpdate = false, int border = 0);
	~Viewport();

	void redraw(bool updateNow = false);
	
	virtual void onRedraw() {}
	/// обычно нужно только для перещета Frustum-а камеры
	virtual void onInitialize() {}
	virtual void onResize(int width, int height) { size_.set(width, height); } 


	/// для получения координат используется mousePosition()
	virtual void onMouseMove();
	virtual void onMouseWheel(int delta){}
	virtual void onMouseButtonDown(MouseButton button) {}
	virtual void onMouseButtonUp(MouseButton button) {}

	bool continuousUpdate() const{ return continuousUpdate_; }
	void setContinuousUpdate(bool continuousUpdate);
	Vect2i size() const{ return size_; }
	Vect2i mousePosition() const{ return mousePosition_; }
	void captureMouse();
	void releaseMouse();

	void setFillColor(unsigned char red, unsigned char green, unsigned char blue);

	cRenderWindow* renderWindow();
	cInterfaceRenderDevice* renderDevice() { return renderDevice_; }

	void serialize(Archive& ar);
	
	ViewportImpl* impl();
protected:
	friend class ViewportImpl;
	friend class RenderWindow;

	Color3c fillColor_;
	bool continuousUpdate_;
	bool showFocusFrame_;
	Vect2i mousePosition_;
	Vect2i size_;
	cScene* scene_;
	cInterfaceRenderDevice* renderDevice_;
};

}

#endif
