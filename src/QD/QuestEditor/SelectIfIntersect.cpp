#include "stdafx.h"
#include ".\selectifintersect.h"
#include "SelectionManager.h"

SelectIfIntersect::SelectIfIntersect(CRect const& rect, 
									 SelectionManager& selectionManager):
rect_(rect)
, selectionManager_(selectionManager)
{
}

void SelectIfIntersect::operator()(qdTriggerElementPtr const& ptr) const{
	CRect inters;
	inters.IntersectRect(rect_, &ptr->bounding_rect());
	if (!inters.IsRectEmpty())
		selectionManager_.select(ptr);
}
