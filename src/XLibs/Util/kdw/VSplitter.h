#ifndef __KDW_VSPLITTER_H_INCLUDED__
#define __KDW_VSPLITTER_H_INCLUDED__

#include "kdw/Splitter.h"

namespace kdw{

class SplitterImpl;
class KDW_API VSplitter : public Splitter{
public:
	VSplitter(int splitterSpacing = 1, int border = 0);
	~VSplitter();

protected:
	VSplitter(int splitterSpacing, int border, SplitterImpl* impl);
	bool vertical() { return true; }

	Widget* _nextWidget(Widget* last, FocusDirection focusDirection);
	int boxLength() const;
	Vect2i elementSize(Widget* widget) const;
	Recti rectByPosition(int start, int end);
	void setSplitterMinimalSize(const Vect2i& size);
	int positionFromPoint(const Vect2i point) const;
};

}

#endif
