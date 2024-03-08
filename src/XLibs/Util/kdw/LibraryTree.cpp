#include "StdAfx.h"

#include "XTL\SafeCast.h"
#include "kdw/LibraryTree.h"
#include "kdw/LibraryTreeObject.h"

namespace kdw{

LibraryTree::LibraryTree()
{
	model()->setRoot(new LibraryTreeObject());
}

LibraryTreeObject* LibraryTree::focusedObject()
{
	return safe_cast<LibraryTreeObject*>(__super::focusedObject());
}

}