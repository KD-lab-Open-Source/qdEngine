#ifndef __KDW_TREE_VIEW_H_INCLUDED__
#define __KDW_TREE_VIEW_H_INCLUDED__

#include <string>
#include "kdw/Win32/Types.h"

#include "kdw/TreeEnums.h"
#include "XMath/XMath.h"
#include "XTL/Rect.h"


namespace kdw{

class Tree;
class TreeModel;
class TreeRow;

class TreeColumnDrawer : public ShareHandleBase{
public:
	TreeColumnDrawer(Tree* tree = 0)
	: tree_(tree)
	{}
	
	void setTree(Tree* tree){ tree_ = tree; }
	Tree* tree() { return tree_; }
	const Tree* tree() const{ return tree_; }

	TreeModel* model();
	const TreeModel* model() const;

	virtual TreeHitTest hitTest(TreeRow* row, Vect2i point, const RECT& rowRect) = 0;
	virtual int rowHeight(TreeRow* row, int column) const = 0;
	virtual int rowIndent(TreeRow* row) const { return 8; }
	virtual void drawRow(TreeRow* row, int column, HDC dc, const RECT& rowRect, int level, bool focused, bool hovered, bool activated) const = 0;

	virtual void getSubRectPlus(TreeRow* row, const Recti& rowRect, Recti& outResult) const;

	virtual void serialize(Archive& ar);
protected:
	void drawPlus(HDC dc, const Recti& rect, bool expanded, bool selected, bool grayed) const;

	Tree* tree_;
};



class TreeColumn{
public:
	TreeColumn()
	: rightCornerPosition_(0.5f)
	{
	}
	float position() const{ return rightCornerPosition_; }
	TreeColumnDrawer* drawer() { return drawer_; }
	const TreeColumnDrawer* drawer() const{ return drawer_; }
	void setDrawer(TreeColumnDrawer* drawer) { drawer_ = drawer; }

	void setCaption(const char* caption){ caption_ = caption; }
	const char* caption() const{ return caption_.c_str(); }

	void serialize(Archive& ar);
protected:
	ShareHandle<TreeColumnDrawer> drawer_;
	float rightCornerPosition_;
	std::string caption_;
};


class TreeColumns : public std::vector<TreeColumn>{
public:
	bool serialize(Archive& ar, const char* name, const char* nameAlt);
};


class StringTreeModel;
class StringTreeColumnDrawer : public TreeColumnDrawer{
public:
	typedef StringTreeModel Model;
	StringTreeColumnDrawer(Tree* tree = 0);
	int rowHeight(TreeRow* row, int column) const;
	TreeHitTest hitTest(TreeRow* row, Vect2i point, const RECT& rowRect);
	void getTestRect(TreeRow* row, TreeHitTest testArea, const RECT& rowRect, RECT& outResult) const;
	void getSubRectImage(TreeRow* row, const Recti& rowRect, Recti& imageRect) const;
	void drawRow(TreeRow* row, int column, HDC dc, const RECT& rowRect, int indent, bool focused, bool hovered, bool activated) const;
};

}

#endif
