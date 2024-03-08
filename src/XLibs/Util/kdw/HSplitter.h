#ifndef __KDW_HSPLITTER_H_INCLUDED__
#define __KDW_HSPLITTER_H_INCLUDED__

#include "kdw/Splitter.h"

namespace kdw{
class SplitterImpl;
class KDW_API HSplitter : public Splitter{
public:
	HSplitter(int splitterSpacing = 1, int border = 0);
	~HSplitter();
protected:
	HSplitter(int splitterSpacing, int border, SplitterImpl* impl);

	Widget* _nextWidget(Widget* last, FocusDirection focusDirection);
	bool vertical() { return false; }
	int boxLength() const;
	Vect2i elementSize(Widget* widget) const;
	Recti rectByPosition(int start, int end);
	void setSplitterMinimalSize(const Vect2i& size);
	int positionFromPoint(const Vect2i point) const;
};

}

#endif
