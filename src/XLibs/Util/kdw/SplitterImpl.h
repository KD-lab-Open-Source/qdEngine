#ifndef __KDW_SPLITTER_IMPL_H_INCLUDED__
#define __KDW_SPLITTER_IMPL_H_INCLUDED__



#include "kdw/Win32/Window.h"
#include "kdw/Win32/Handle.h"
namespace kdw{
class Splitter;
class SplitterImpl : public Win32::Window32{
public:
	const char* className() const{ return "kdw.SplitterImpl"; }
	SplitterImpl(kdw::Splitter* owner);
protected:
	BOOL	onMessageSetCursor(HWND window, USHORT hitTest, USHORT message);
	void	onMessageLButtonDown(UINT button, int x, int y);
	void	onMessageLButtonUp(UINT button, int x, int y);
	void	onMessageMouseMove(UINT button, int x, int y);

	void	onMessagePaint();
	BOOL	onMessageEraseBkgnd(HDC dc);

	int splitterByPoint(Vect2i point);
	Recti getSplitterRect(int splitterIndex);

	kdw::Splitter* owner_;
	Vect2i lastCursorPosition_;
	int grabbedSplitter_;
};
};

#endif
