#ifndef __KDW_PLUG_DIALOG_H_INCLUDED__
#define __KDW_PLUG_DIALOG_H_INCLUDED__

#include "kdw/Dialog.h"

namespace kdw{

class PlugBase;

class KDW_API PlugDialog : public kdw::Dialog{
public:
	PlugDialog(Widget* parent, PlugBase* plug);
	PlugDialog(HWND parent, PlugBase* plug);
	~PlugDialog();

	PlugBase* plug();
protected:
	PlugBase* plug_;
};

}

#endif
