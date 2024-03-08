#ifndef __UE_TREE_CTRL_H_INCLUDED__
#define __UE_TREE_CTRL_H_INCLUDED__

#include "kdw/ObjectsTree.h"

class EditorLibraryInterface;
class Serializer;

namespace kdw{

class PopupMenu;
class LibraryTreeObject;
class LibraryTabBase;

class LibraryTree: public ObjectsTree{
public:
	LibraryTree();
	LibraryTreeObject* focusedObject();

	void setTab(LibraryTabBase* tab){ tab_ = tab; }
	LibraryTabBase* tab(){ return tab_; }
protected:
	LibraryTabBase* tab_;
};

}

#endif
