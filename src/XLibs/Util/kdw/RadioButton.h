#ifndef __KDW_RADIOBUTTON_H_INCLUDED__
#define __KDW_RADIOBUTTON_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"
#include <string>

namespace kdw{
	class RadioButtonImpl;
	class RadioButtonGroup;
	class KDW_API RadioButton : public _WidgetWithWindow{
	public:
		RadioButton(RadioButton* groupWith = 0, const char* text = "RadioButton", int border = 0);

		const char* text() const { return text_.c_str(); }
		void setText(const char* text);
		void setStatus(bool status);
		bool status() const { return status_; }

		virtual void onChanged();
		sigslot::signal0& signalChanged() { return signalChanged_; }

		void serialize(Archive& ar);
		RadioButtonGroup* group() const;

		bool _focusable() const;

	protected:
		friend class RadioButtonImpl;
		RadioButtonImpl* window() const;
		// внутренние функции
		sigslot::signal0 signalChanged_;
		std::string text_;
		bool status_;
	};

}

#endif
