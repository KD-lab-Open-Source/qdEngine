#ifndef __KDW_COLOR_COMBO_BOX_H_INCLUDED__
#define __KDW_COLOR_COMBO_BOX_H_INCLUDED__

#include "kdw/ComboBox.h"
#include "XMath/Colors.h"
#include "XMath/ComboListColor.h"

namespace kdw{

class ColorComboBox: public ComboBox{
public:
	ColorComboBox(int border = 0);
	void add(Color4c color, const char* name = "");
	void set(const ComboListColor& value);
};

}

#endif
