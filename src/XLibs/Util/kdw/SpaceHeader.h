#ifndef __SPACE_HEADER_H_INCLUDED__
#define __SPACE_HEADER_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"

namespace kdw{

class Space;
class SpaceHeader : public _WidgetWithWindow{
public:
	SpaceHeader(Space* space, int border = 0);

	void serialize(Archive& ar);
	
	const char* label() const{ return label_.c_str(); }
	void setLabel(const char* label);

	Space* space() { return space_; }

	void onMenuReplaceSpace(int index);
	void onMenuSplit();
protected:
	friend class SpaceHeaderImpl;
	bool _focusable() const{ return false; }

	void updateMinimalSize();

	std::string label_;
	Space* space_;
};

}

#endif
