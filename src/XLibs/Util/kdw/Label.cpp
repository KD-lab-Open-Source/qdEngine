#include "StdAfx.h"
#include "kdw/Label.h"
#include "kdw/Container.h"

#include "kdw/_WidgetWindow.h"
#include <windowsx.h>

#include "kdw/Serialization.h"
#include "Serialization/SerializationFactory.h"

namespace kdw{

REGISTER_CLASS(Widget, Label, "Метка")
class LabelImpl: public _WidgetWindow{
public:
	LabelImpl(Label* owner, bool emphasis);
	~LabelImpl();

	const char* className() const{ return "kdw.LabelImpl"; }
	int onMessageDestroy();
	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);
	void recreateFont(bool emphasis);
	HFONT font() { return owner_->emphasis_ ? Win32::defaultBoldFont() : Win32::defaultFont(); }

	void setText(const char* text);
protected:
	kdw::Label* owner_;
};

LabelImpl::LabelImpl(Label* owner, bool emphasis)
: _WidgetWindow(owner)
, owner_(owner)
{
	VERIFY(create("", WS_CHILD , Recti(0, 0, 42, 42), *Win32::_globalDummyWindow));
}

LabelImpl::~LabelImpl()
{

}

void LabelImpl::setText(const char* text)
{
	Vect2i textSize = Win32::calculateTextSize(*this, font(), text);
	if(owner_->expandByContent_)
		owner_->_setMinimalSize(textSize + Vect2i(2 + owner_->border() * 2, 2 + owner_->border() * 2));
	else
		owner_->_setMinimalSize(Vect2i(0, textSize.y) + Vect2i(2 + owner_->border() * 2, 2 + owner_->border() * 2));
	owner_->_queueRelayout();
}

int LabelImpl::onMessageDestroy()
{
	return __super::onMessageDestroy();
}

LRESULT LabelImpl::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message){
	case WM_ERASEBKGND:
		{
			HDC dc = (HDC)(wparam);
			return FALSE;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC dc = ::BeginPaint(*this, &ps); 
			ASSERT(dc != 0);
			const char* text = owner_->text_.c_str();
			
			UINT flags = DT_SINGLELINE;
			if(!owner_->expandByContent_)
				flags |= DT_PATH_ELLIPSIS;
			
			switch(owner_->alignHorizontal_){
			case kdw::ALIGN_LEFT:
				flags |= DT_LEFT;
				break;
			case kdw::ALIGN_CENTER:
				flags |= DT_CENTER;
				break;
			case kdw::ALIGN_RIGHT:
				flags |= DT_RIGHT;
				break;
			};
			
			switch(owner_->alignVertical_){
			case kdw::ALIGN_TOP:
				flags |= DT_TOP;
				break;
			case kdw::ALIGN_MIDDLE:
				flags |= DT_VCENTER;
				break;
			case kdw::ALIGN_BOTTOM:
				flags |= DT_BOTTOM;
				break;
			};

			RECT rect;
			VERIFY(GetClientRect(*this, &rect));
			HFONT oldFont = HFONT(::SelectObject(dc, (HGDIOBJ)font()));
			HBRUSH oldBrush = HBRUSH(::SelectObject(dc, (HGDIOBJ)(::GetStockObject(GRAY_BRUSH))));
			int oldBackMode = ::SetBkMode(dc, OPAQUE);
			COLORREF oldBkColor = ::SetBkColor(dc, ::GetSysColor(COLOR_BTNFACE));
			::FillRect(dc, &rect, ::GetSysColorBrush(COLOR_BTNFACE));
			DrawText(dc, text, int(strlen(text)), &rect, flags);
			::SelectObject(dc, oldFont);
			::SelectObject(dc, oldBrush);
			::SetBkMode(dc, oldBackMode);
			::SetBkColor(dc, oldBkColor);

			::EndPaint(*this, &ps);
			return TRUE;
		}
	}
	return __super::onMessage(message, wparam, lparam);
}


#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list

Label::Label(const char* text, bool emphasis, int border)
: _WidgetWithWindow(new LabelImpl(this, emphasis), border)
, mnemonicWidget_(0)
, alignHorizontal_(ALIGN_LEFT)
, alignVertical_(ALIGN_MIDDLE)
, emphasis_(emphasis)
, expandByContent_(true)
{
    _setMinimalSize(Vect2i(42, 42));
	setText(text);
}
#pragma warning(pop)

void Label::setText(const char* text)
{
	impl()->setText(text);
	text_ = text;
}

void Label::setAlignment(kdw::TextAlignHorizontal alignh, kdw::TextAlignVertical alignv)
{
	alignHorizontal_ = alignh;
	alignVertical_ =  alignv;

	::RedrawWindow(*impl(), 0, 0, RDW_INVALIDATE);
}

void Label::setExpandByContent(bool expand)
{
	expandByContent_ = expand;
	setText(text_.c_str());
}

void Label::serialize(Archive& ar)
{
	if(ar.filter(SERIALIZE_DESIGN)){
		ar.serialize(text_, "text", "&Текст");
		ar.serialize(emphasis_, "emphasis", "Выделить");
		ar.serialize(expandByContent_, "expandByContent", "Растягивать под содержимое");
	}
	Widget::serialize(ar);
}

/*
void Label::setMnemonicWidget(Widget* mnemonicWidget)
{

}
*/

};
