#ifndef __KDW_CLIPBOARD_INCLUDED__
#define __KDW_CLIPBOARD_INCLUDED__

#include "kdw/ConstStringList.h"

class Serializer;

namespace kdw{
class Widget;
class PropertyRow;
class PropertyTreeModel;
class Clipboard{
public:
	Clipboard(Widget* owner, ConstStringList* constStrings = 0, PropertyTreeModel* model = 0);
	~Clipboard();

	bool empty();
	bool copy(PropertyRow* row);
	bool copy(Serializer& se);
	bool paste(PropertyRow* row, bool onlyCheck = false);
	bool paste(Serializer& se);
	bool canBePastedOn(const char* destinationType);
	int smartPaste(Serializer& se);
protected:
	bool pasteRaw(Serializer& se);

	Widget* widget_;
	PropertyTreeModel* model_;
	ConstStringList* constStrings_;
	ConstStringList ownConstStrings_;
	unsigned int clipboardFormat_;
};

};

#endif
