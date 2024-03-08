#ifndef __KDW_PROPERTY_TREE_COLUMN_DRAWER_H_INCLUDED__
#define __KDW_PROPERTY_TREE_COLUMN_DRAWER_H_INCLUDED__

#include "kdw/TreeView.h"
#include "kdw/TreeEnums.h"
#include "kdw/Win32/Types.h"

namespace kdw{
class Tree;
class TreeRow;
class PropertyRow;
class PropertyTreeColumnDrawer : public TreeColumnDrawer{
public:
	PropertyTreeColumnDrawer(Tree* tree = 0);
	void drawRow(TreeRow* row, int column, HDC dc, const RECT& rowRect, int indent, bool focused, bool hovered, bool activated) const;
	int rowHeight(TreeRow* row, int column) const;
	int rowIndent(TreeRow* row) const;
	bool rowHasIcon(PropertyRow* row) const;

	void getSubRectFloor(PropertyRow* row, const Recti& rowRect, Recti& outRect) const;
	void getSubRectIcon(PropertyRow* row, const Recti& rowRect, Recti& outRect) const;
	void getSubRectWidget(PropertyRow* row, const Recti& rowRect, Recti& outRect) const;
	void getSubRectText(TreeRow* row, const Recti& rowRect, Recti& outRect) const;
	void getSubRectPlus(TreeRow* row, const Recti& rowRect, Recti& outResult) const;
	
	TreeHitTest hitTest(TreeRow* node, Vect2i point, const RECT& rowRect);
	PropertyTree* tree(){ return static_cast<PropertyTree*>(__super::tree()); }
	const PropertyTree* tree() const{ return static_cast<const PropertyTree*>(__super::tree()); }
protected:
	PropertyRow::WidgetPosition overridenWidgetPosition(PropertyRow* row) const;

	std::string rowText(PropertyRow* row) const;
	HFONT rowFont(PropertyRow* row) const;
};

}
#endif
