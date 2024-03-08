#include "StdAfx.h"
#include "kdw/HotkeyDialog.h"
#include "kdw/HotkeyButton.h"

#include "kdw/Win32/Window.h"

namespace kdw{

HotkeyDialog::HotkeyDialog(Widget* owner, sKey key)
: Dialog(owner, 0)
, button_(0)
{
	setMinimizeable(false);
	setResizeable(false);
	setShowTitleBar(false);
	button_ = new HotkeyButton(0);
	button_->signalChanged().connect(this, &HotkeyDialog::onChanged);
	add(button_);
}

HotkeyDialog::HotkeyDialog(HWND owner, sKey key)
: Dialog(owner, 0)
, button_(0)
{
	// COPY_PASTE
	setMinimizeable(false);
	setResizeable(false);
	setShowTitleBar(false);
	button_ = new HotkeyButton(0);
	button_->signalChanged().connect(this, &HotkeyDialog::onChanged);
	add(button_);
}

void HotkeyDialog::onChanged()
{
	key_ = button_->get();
	__super::onResponse(RESPONSE_OK);
}

void HotkeyDialog::onKeyDefault()
{
	// םו גûחûגאול __super::onKeyDefault
}

void HotkeyDialog::onKeyCancel()
{
	// םו גûחûגאול __super::onKeyCancel
}

}
