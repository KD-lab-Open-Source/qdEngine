#ifndef __KDW_TREE_H_INCLUDED__
#define __KDW_TREE_H_INCLUDED__

#include <vector>

#include "kdw/_WidgetWithWindow.h"
#include "kdw/TreeEnums.h"
#include "kdw/TreePath.h"
#include "sKey.h"

namespace kdw{

class TreeRow;
class TreeImpl;
class TreeColumnDrawer;
class TreeModel;
class TreeColumns;
class ImageStore;

class KDW_API Tree : public _ContainerWithWindow{
public:
	Tree(TreeModel* model = 0, int border = 0);
	~Tree();

	int childIndent() const{ return childIndent_; }

	void setColumn(int column, TreeColumnDrawer* drawer, float position);
	void setModel(TreeModel* model);

	void setFlat(bool flat);
	bool flat() const{ return flat_; }

	void setCompact(bool compact);
	bool compact() const{ return compact_; }

	void setRootIndent(int indent);
	int rootIndent() const{ return rootIndent_; }

	void setSelectFocused(bool selectFocused);
	bool selectFocused() const { return selectFocused_; }

	void setEmphasizeFolders(bool emphasize);
	bool emphasizeFolders() const{ return emphasizeFolders_; }

	void setTrackHovered(bool track) { trackHovered_ = track; }
	bool trackHovered() const{ return trackHovered_; }

	/// нужно вызыывать после изменения модели
	void update();

	void setImageStore(ImageStore* store);
	ImageStore* imageStore() const { return imageStore_; }

	TreeModel* model() const{ return model_; }

	const TreeColumns& columns() const{ return *columns_; }

	TreeState state();
	void setState(const TreeState& state);
	void serialize(Archive& ar);

	virtual void onModelUpdated();

	void ensureVisible(TreeRow* row, bool update = true);
	void redraw();

	virtual bool rowVisible(TreeRow* row) const;
	virtual bool canBeToggled(TreeRow* row) const;
	bool _focusable() const{ return true; }
	TreeImpl* impl() const;
	virtual bool isFocused() const;
	void _setFocus();
	bool getRowRect(TreeRow* row, Recti& outRect, bool onlyVisible = true);
	void expandParents(TreeRow* row);
	void expandRow(TreeRow* row, bool expanded = true);
	void expandAll(TreeRow* root = 0);
protected:
	friend class TreeImpl;

	virtual void onRowSelected(TreeRow* row);
	virtual void onRowDeselected(TreeRow* row);

	virtual bool onRowLMBDoubleClick(TreeRow* row, const Recti& rowRect, Vect2i point);
	virtual bool onRowLMBDown(TreeRow* row, const Recti& rowRect, Vect2i point);
	virtual void onRowLMBUp(TreeRow* row, const Recti& rowRect, Vect2i point);
	virtual void onRowRMBDown(TreeRow* row, const Recti& rowRect, Vect2i point);
	virtual void onRowRMBUp(TreeRow* row, const Recti& rowRect, Vect2i point);
	virtual bool onRowKeyDown(TreeRow* row, sKey key);
	virtual void onRowMouseMove(TreeRow* row, const Recti& rowRect, Vect2i point) {}

    void interruptDrag();
	void updateHeights();

	virtual bool hitActivationRect(TreeRow* row, Vect2i point, const Recti& rowRect, int column);

	TreeColumns& columns(){ return *columns_; }
	TreeColumnDrawer* autoCreateColumnDrawer(const char* typeName);
	
	bool flat_;
	bool compact_;
	bool emphasizeFolders_;
	bool selectFocused_;
	bool trackHovered_;

	int childIndent_;
	int rootIndent_;

	PolymorphicHandle<TreeModel> model_;

	TreeColumns* columns_;
	ImageStore* imageStore_;

};


}

#endif
