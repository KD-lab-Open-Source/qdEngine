#ifndef __KDW_DIALOG_H_INCLUDED__
#define __KDW_DIALOG_H_INCLUDED__

#include "kdw/Window.h"
#include "kdw/VBox.h"
#include "kdw/HBox.h"
#include "kdw/Button.h"
#include "kdw/HLine.h"
#include "kdw/Response.h"
#include "kdw/Win32/MessageLoop.h"

#include <vector>
#include <map>

#include "XTL/StaticMap.h"

namespace kdw{

	class Button;

	class KDW_API Dialog : public kdw::Window
	{
	public:
		Dialog(kdw::Widget* owner, int border = 12);
		Dialog(HWND owner, int border = 12);
		~Dialog();

		int showModal();
		virtual void onResponse(int response);

		void add(Widget* widget, bool placeInEnd = false, bool expand = true, bool fill = true);
		void addButton(const char* label, int response, bool atRight = true);
		void setDefaultResponse(int response);
		void setCancelResponse(int response);

		void onClose();
		int cancelResponse() const { return cancelResponse_; }
		int defaultResponse() const { return defaultResponse_; }

		void serialize(Archive& ar);

	protected:
		void interruptModalLoop();
		virtual void onKeyDefault();
		virtual void onKeyCancel();

		void init(int border);

		VBox* vbox_;
		HBox* hboxButtons_;

		
		int response_;
		int cancelResponse_;
		int defaultResponse_;

		typedef StaticMap<Response, Button*> Map;
		Map map_;
	private:
		HWND parentWnd_;
	};

}
#endif
