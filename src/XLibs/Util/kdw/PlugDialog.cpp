#include "StdAfx.h"
#include "Plug.h"
#include "PlugDialog.h"

namespace kdw{

PlugDialog::PlugDialog(Widget* widget, PlugBase* plug)
: Dialog(widget, 8)
, plug_(plug)
{
	xassert(plug_);
	xassert(plug_->asWidget());

	setTitle(TRANSLATE("Редактор"));
	setDefaultSize(Vect2i(1024, 768));
	addButton(TRANSLATE("OK"), kdw::RESPONSE_OK);
	addButton(TRANSLATE("Отмена"), kdw::RESPONSE_CANCEL);
	setResizeable(true);

	add(plug_->asWidget());
}

PlugDialog::PlugDialog(HWND widget, PlugBase* plug)
: Dialog(widget, 8)
, plug_(plug)
{
	xassert(plug_);
	xassert(plug_->asWidget());

	setTitle(TRANSLATE("Редактор"));
	setDefaultSize(Vect2i(1024, 768));
	addButton(TRANSLATE("OK"), kdw::RESPONSE_OK);
	addButton(TRANSLATE("Отмена"), kdw::RESPONSE_CANCEL);
	setResizeable(true);

	add(plug_->asWidget());
}

PlugDialog::~PlugDialog()
{
	plug_ = 0;
}

PlugBase* PlugDialog::plug()
{
	return plug_;
}

}
