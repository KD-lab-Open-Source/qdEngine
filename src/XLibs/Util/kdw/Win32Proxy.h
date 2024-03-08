#ifndef __KDW_WIN32_PROXY_H_INCLUDED_
#define __KDW_WIN32_PROXY_H_INCLUDED_

#include "kdw/_WidgetWithWindow.h"
#include "kdw/Win32/Types.h"

namespace kdw{

class Win32ProxyImpl;
class KDW_API Win32Proxy : public _ContainerWithWindow{
public:
	Win32Proxy(HWND parent, int border = 0);

	void add(Widget* widget);
	void visitChildren(WidgetVisitor& visitor) const;
protected:
	void _arrangeChildren();

	Win32ProxyImpl& impl();
	ShareHandle<Widget> child_;
};

}

#endif
