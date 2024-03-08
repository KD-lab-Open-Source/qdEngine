#ifndef __KDW_PROGRESSBAR_H_INCLUDED__
#define __KDW_PROGRESSBAR_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"
#include <string>


namespace kdw{
	class ProgressBarImpl;
	class KDW_API ProgressBar : public _WidgetWithWindow{
	public:
		ProgressBar(int border = 0);
		
		void setPosition(float pos);
		/// [0..1]
		float position() const { return pos_; }
		
		void serialize(Archive& ar);
		bool _focusable() const{ return false; }
	protected:
		// внутренние функции
		ProgressBarImpl* window() const{ return reinterpret_cast<ProgressBarImpl*>(_window()); }

		float pos_;
	};

}

#endif
