#ifndef __TREE_MODEL_H_INCLUDED__
#define __TREE_MODEL_H_INCLUDED__

#include <string>
#include <typeinfo>
#include <vector>
#include <list>

#include "kdw/API.h"
#include "XTL/sigslot.h"
#include "kdw/Serialization.h"
#include "kdw/TreePath.h"
#include "xMath/xMath.h"

namespace kdw{

class Tree;
class TreeRow;
class TreeModel;
class DragManager;
struct TreeRowVisitor{
	virtual bool operator()(TreeRow* row) = 0;
};

class KDW_API TreeRow : public ShareHandleBase{
public:
	TreeRow(TreeRow* parent = 0)
	: expanded_(false)
	, height_(1)
	, parent_(parent)
	, selected_(false)
	, level_(0)
	, previousDeadRow_(0)
	, imageIndex_(-1)
	, hidden_(false)
	, indent_(0)
	{
	}
	virtual ~TreeRow() {}
	bool selected() const{ return selected_; }
	void setSelected(bool selected) { selected_ = selected; }
    bool expanded() const{ return expanded_; }
	// используйте Tree::expandRow
	void _setExpanded(bool expanded){ expanded_ = expanded; }
	void expandAll(Tree* tree);
	int height() const{ return height_; }
	int baseHeight() const{ return height_; }
	void setHeight(int height) { height_ = height; }
	virtual void adjust(int height, const Tree* tree) { height_ = height; }

	bool hidden() const{ return hidden_; }

	void setImage(int index) { imageIndex_ = index; }
	int imageIndex() const { return imageIndex_; }

	int level() const{ return level_; }
	void setLevel(int level) { level_ = level; }

	int indent() const{ return indent_; }
	void setIndent(int indent) { indent_ = indent; }
	virtual int textIndent() const{ return 0; }
	TreeRow* parent() { return parent_; }
	const TreeRow* parent() const{ return parent_; }
	void setParent(TreeRow* row) { parent_ = row; }
	void inheritIndent(int indent, int rootIndent){
		setIndent(parent()->parent() ? (parent()->indent() + indent) : rootIndent);
	}

	typedef std::vector< ShareHandle<TreeRow> > Rows;
	typedef Rows::iterator iterator;
	typedef Rows::const_iterator const_iterator;

	template<class DerivedTreeRow>
	DerivedTreeRow* add(DerivedTreeRow* row, TreeRow* after = 0){
		if(after == 0 || after == this)
			children_.push_back(row);
		else{
			iterator it = std::find(children_.begin(), children_.end(), after);
			if(it != children_.end()){
				++it;
				children_.insert(it, row);
			}
			else{
				children_.push_back(row);
			}
		}
		row->setParent(this);
		return row;
	}
	
	bool visitChildren(TreeRowVisitor& visitor){
		Rows::iterator it;
		for(it = children_.begin(); it != children_.end(); ++it){
			if(!visitor(*it))
				return false;
		}
		return true;
	}
	int index() const;
	TreeRow* childrenByIndex(int index);
	const TreeRow* childrenByIndex(int index) const;
	int childrenIndex(TreeRow* row);
	bool isChildOf(const TreeRow* row) const;
	bool empty() const{ return children_.empty(); }
	iterator find(TreeRow* row) { return std::find(children_.begin(), children_.end(), row); }
	iterator begin() { return children_.begin(); }
	iterator end() { return children_.end(); }
	const_iterator begin() const{ return children_.begin(); }
	const_iterator end() const{ return children_.end(); }
	std::size_t size() const{ return children_.size(); }
	TreeRow* front() { return children_.front(); }
	TreeRow* back() { return children_.back(); }
	void clear(){ children_.clear(); }
	void assignRowProperties(TreeRow* row){
		expanded_ = row->expanded_;
		level_ = row->level_;
		indent_ = row->indent_;
		selected_ = row->selected_;
		height_ = row->height_;
		parent_ = row->parent_;
		previousDeadRow_ = row;
	}
	void replaceAndPreserveState(TreeRow* oldRow, TreeRow* newRow, bool preserveChildren = true){
		Rows::iterator it = std::find(children_.begin(), children_.end(), oldRow);
		xassert(it != children_.end());
		if(it != children_.end()){
			newRow->assignRowProperties(*it);
			if(preserveChildren)
				(*it)->swapChildren(newRow);
			*it = newRow;
		}
	}
	iterator erase(iterator it){
		return children_.erase(it);
	}
	void erase(TreeRow* node){
		node->setParent(0);
		Rows::iterator it = std::find(children_.begin(), children_.end(), node);
		xassert(it != children_.end());
		if(it != children_.end())
			children_.erase(it);
	}
	void swapChildren(TreeRow* row)
	{
		children_.swap(row->children_);
		iterator it;
		FOR_EACH(children_, it)
			(**it).setParent(this);
		FOR_EACH(row->children_, it)
			(**it).setParent(row);
	}
	TreeRow* previousDeadRow() const{ return previousDeadRow_; }

	virtual bool canBeToggled() const{ return true; }
	virtual bool canBeDragged() const{ return false; }

	// admix: не слишком ли много функций для drop-а?
	virtual bool canBeDroppedOn(const TreeRow* row, const TreeRow* beforeChild, const Tree* tree, bool direct) const{ return false; }
	
	virtual void dropInto(TreeRow* destination, TreeRow* beforeChild, Tree* tree);
	virtual void receiveDrop(TreeRow* row, TreeRow* beforeChild, TreeModel* model){}
	virtual void onDrag(DragManager& dragManager){}

protected:
	Rows children_;
	TreeRow* previousDeadRow_;
	TreeRow* parent_;
    int height_;
	int level_;
	int indent_;

	int imageIndex_;
	bool hidden_;
    bool expanded_;
	bool selected_;
};



class KDW_API TreeModel : public PolymorphicBase, public sigslot::has_slots{
public:
	class LockedUpdate : public ShareHandleBase{
	public:
		LockedUpdate(TreeModel* model)
		: model_(model)
		, updateRequested_(false)
		{}
		void requestUpdate(){ updateRequested_ = true; }
		void dismissUpdate(){ updateRequested_ = false; }
		~LockedUpdate(){
			model_->updateLock_ = 0;
			if(updateRequested_)
				model_->signalUpdated().emit();
		}
	protected:
		TreeModel* model_;
		bool updateRequested_;
	};
	typedef ShareHandle<LockedUpdate> UpdateLock;

	typedef TreeSelection Selection;

	virtual ~TreeModel() {}

	TreePath pathFromRow(TreeRow* node);
	TreeRow* rowFromPath(const TreePath& path);
	void setFocusedRow(TreeRow* node);
	TreeRow* focusedRow();

	virtual const char* columnTypeName(int column) { return ""; }
	virtual int columnsCount() const { return 1; }

	void clear();
	const Selection& selection() const{ return selection_; }
	void setSelection(const Selection& selection);

	void setRoot(TreeRow* root);
	TreeRow* root() { return root_; }
	const TreeRow* root() const{ return root_; }

	virtual void serialize(Archive& ar);

	UpdateLock lockUpdate();
	void requestUpdate();
	void dismissUpdate();
	
	virtual void onUpdated() { signalUpdated().emit(); }
	sigslot::signal0& signalUpdated() { return signalUpdated_; };

	virtual bool activateRow(Tree* tree, TreeRow* row, int column) { return false; };
	virtual void activateRowContext(Tree* tree, TreeRow* row, int column) {};
	virtual void activateRowAlternative(Tree* tree, TreeRow* row, int column) {};
	virtual void selectRow(TreeRow* row, bool selected);
	virtual void deselectAll(bool visit = false);

protected:
	TreePath focusedRow_;
	Selection selection_;
	sigslot::signal0 signalUpdated_;

	ShareHandle<TreeRow> root_;
	UpdateLock updateLock_;

	friend class TreeImpl;
};

class StringTreeRow : public TreeRow{
public:
	explicit StringTreeRow(const char* text = "")
	: TreeRow()
	, value_(text) 
	{
	}
	void adjust(int height, const Tree*) { height_ = height; }
	const char* value() const{ return value_.c_str(); }
	StringTreeRow* parent() { return safe_cast<StringTreeRow*>(TreeRow::parent()); }
	const StringTreeRow* parent() const{ return safe_cast<const StringTreeRow*>(TreeRow::parent()); }
	const char* text() const{ return value_.c_str(); }
	void setText(const char* text){ value_ = text; }
protected:
	std::string value_;
};

class StringTreeModel : public TreeModel{
public:
	int columnsCount() const{ return 1; }
	StringTreeRow* root() { return safe_cast<StringTreeRow*>(TreeModel::root()); }
};

// ----------------------------------------------------------------------
}

#endif
