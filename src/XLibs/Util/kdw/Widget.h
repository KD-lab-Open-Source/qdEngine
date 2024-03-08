#ifndef __KDW_WIDGET_H_INCLUDED__
#define __KDW_WIDGET_H_INCLUDED__

#include "kdw/API.h"

#include "XTL/Rect.h"
#include "xMath.h"

#include "XTL/sigslot.h"
#include "Handle.h"
#include "Serialization/Dictionary.h"

namespace Win32{
	class Window32;
};	

namespace kdw{

enum{
	SERIALIZE_DESIGN = 1 << 0,
	SERIALIZE_STATE  = 1 << 1
};

enum MouseButton{
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_WHEEL_UP,
	MOUSE_BUTTON_WHEEL_DOWN,
	MOUSE_BUTTON_LEFT_DOUBLE,
};

class HotkeyContext;
class Window;
class Container;
class Tooltip;

enum FocusDirection{
	FOCUS_FIRST,
	FOCUS_LAST,

	FOCUS_PREVIOUS,
	FOCUS_NEXT,

	FOCUS_UP,
	FOCUS_DOWN,
	FOCUS_LEFT,
	FOCUS_RIGHT
};

class Widget;

struct WidgetVisitor{
	virtual void operator()(Widget& widget) = 0;
};

template<class Type>
class WidgetHandle : public ShareHandle<Type>
{
public:
	~WidgetHandle();
};

class KDW_API Widget : public sigslot::has_slots, public ShareHandleBase{
public:
	Widget();
	virtual ~Widget();
	virtual bool isVisible() const = 0;

	void setVisibility(bool visible);
	virtual void show();
	virtual void showAll();
	virtual void hide();

	virtual void visitChildren(WidgetVisitor& visitor) const{};

	/// �� �������: enabled/active
	virtual void setSensitive(bool sensitive){ sensitive_ = sensitive; }
	/// �� �������: enabled/active
	bool sensitive() const{ return sensitive_; }

	/// ������� ������ ��������, � ��������
	virtual void setBorder(int border);
	int border() const{ return border_; }
	/// ���������������� ����������� ������ � ��������
	virtual void setRequestSize(const Vect2i size);
	Vect2i requestSize() const{ return requestSize_; }

	virtual void passFocus(FocusDirection direction);
	void setFocus();

	virtual HotkeyContext* hotkeyContext(){ return 0; }
		
	/// ������ � ������������� ����������
	Container* parent() const{ return parent_; }

	sigslot::signal0& signalDelete() { return signalDelete_; };

	virtual void serialize(Archive& ar);

	// �����: ��� ������, ������������ � underscore('_') �� ������ ����������
	// �������������� ���������� � � ������ ��������������� � ����������� �� 
	// Widget � Container ������� ������ ���������� � ������ protected ��� private

	/// ����������� ������� ������������ ���������
    virtual void _setParent(Container* container);

	const Recti& _position() const{ return position_; }
	virtual void _setPosition(const Recti& position);
	void _relayoutParents(bool propagate);
	virtual void _relayoutParents();
	virtual void _queueRelayout();

	const Vect2i _minimalSize() const{ return Vect2i(max(minimalSize_.x, requestSize_.x), max(minimalSize_.y, requestSize_.y)); }
	virtual void _setMinimalSize(const Vect2i& size);

	virtual Win32::Window32* _window() const{ return 0; }

	virtual Widget* _nextWidget(Widget* last, FocusDirection direction);
	Widget* _nextFocusableWidget(Widget* last, FocusDirection direction);
	virtual void _setFocus();

	static void _setFocusedWidget(Widget* widget);
	static Widget* _focusedWidget();

	virtual bool _focusable() const{ return sensitive() && _visible() && _visibleInLayout(); }

	virtual bool canBeDefault() const { return false; }
	virtual void setDefaultFrame(bool enable) {}
	Window* mainWindow();

	virtual void _updateVisibility();
	virtual void _setVisibleInLayout(bool visibleInLayout);
	bool _visibleInLayout() const{ return visibleInLayout_; }
	bool _visible() const{ return visible_; }
	
	void setToolTip(Tooltip* toolTip) { toolTip_ = toolTip; }
	Tooltip* toolTip() const { return toolTip_; }

protected:
	/// ���������� �� ����������
	sigslot::signal0 signalDelete_;

	/// ��������� �������� � ������������ ����������, � �������� (�� ������ � ������������ � ����)
	Recti position_;
	/// ������������ ���������, ��������� ���������
    Container* parent_;
	/// ����������-��������� ������
	Vect2i minimalSize_;
	/// ����������� ������ ������������� �������������
	Vect2i requestSize_;
	/// ����� ������ ��������, � ��������
	int border_;

	// * ��������� � Container?
	/// ����� ������ ��������, � ��������
	int focusedChildIndex_;
	// * ^^^

	/// ����� �� ����������� ��������� ����� ������������
	bool relayoutQueued_;
	/// �-�� enabled
	bool sensitive_;
	/// ����� �� ������� (� ������ ���� ����� ��� ������������ �������)
	bool visible_;
	/// ��������� ���������� ����� ������� �������� (�������� Splitter �����������)
	bool visibleInLayout_;

	Tooltip* toolTip_;

	static Widget* focusedWidget_;
};

HotkeyContext* findHotkeyContext(Widget* widget);
Win32::Window32* _findWindow(const Widget* widget);

};

#endif
