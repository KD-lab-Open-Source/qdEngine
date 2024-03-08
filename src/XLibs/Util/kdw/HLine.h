#ifndef __KDW_HLINE_H_INCLUDED__
#define __KDW_HLINE_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"

class HLineImpl;

namespace kdw{

	class KDW_API HLine : public _WidgetWithWindow{
	public:
		HLine(int border = 0);
		bool _focusable() const{ return false; }
	protected:
		// внутренние функции
		HLineImpl* window() const{ return reinterpret_cast<HLineImpl*>(_window()); }
	};

}

#endif
