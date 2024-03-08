#ifndef __TREE_SELECTOR_H_INCLUDED__
#define __TREE_SELECTOR_H_INCLUDED__

#include "XTL/SafeCast.h"
#include "XTL/Handle.h"
#include "kdw/Dialog.h"
#include "kdw/Entry.h"
#include "kdw/Tree.h"
#include "kdw/TreeModel.h"

namespace kdw{

class TreeObjectFilteredBase : public StringTreeRow{
public:
	TreeObjectFilteredBase(const char* text = "");
	bool matching() const { return matchFilter_; }
	TreeObjectFilteredBase* addFiltered(TreeObjectFilteredBase* object, bool sort = false);

	template<class T>
	TreeObjectFilteredBase* add(const T& object, const char* name);

	virtual bool onDoubleClick(Tree* tree);
	bool matchFilter(const char* filter);
	bool buildFiltered();
	const char* name() const{ return text(); }
protected:
	bool matchFilter_;
	bool removeIfEmpty_;
	typedef std::vector< ShareHandle <TreeObjectFilteredBase> > Children;
	Children potentialChildren_;
};

class TreeBuilder{
public:
	class Object : public TreeObjectFilteredBase{
	public:
		template<class T>
		Object* add(T& object, const char* name){
			return static_cast<Object*>(TreeObjectFilteredBase::add(object, name));
		}

		Object* add(Object* object, bool sort = false){
			return static_cast<Object*>(addFiltered(object, sort));
		}
		Object(const char* label, const char* key = 0, bool removeIfEmpty = false){
			setText(label);
			removeIfEmpty_ = removeIfEmpty;
			if(key){
				key_ = key;
				selectable_ = true;
			}
			else{
				selectable_ = false;
			}
		}
		const char* key() const{ return key_.c_str(); }
		bool selectable() const{ return selectable_; }
	protected:
		std::string key_;
		bool selectable_;
	};

	virtual Object* buildTree(Object* root) = 0;
	virtual bool select(Object* object) = 0;
private:
};

template<class T>
class TreeObjectFiltered;

template<class T>
class TreeObjectFiltered : public TreeObjectFilteredBase{
public:
	TreeObjectFiltered(const T& t, const char* name)
	: TreeObjectFilteredBase(name)
	, object_(t)
	{
	}

protected:
	T& object_;
};


class BuildableTree : public Tree{
public:
	BuildableTree();
	TreeBuilder::Object* root();

	bool onRowLMBDoubleClick(TreeRow* row, const Recti& rowRect, Vect2i point);
};

class TreeSelectorDialog : public Dialog
{
public:
	TreeSelectorDialog(Widget* parent);
	TreeSelectorDialog(HWND parent);
	virtual ~TreeSelectorDialog() {}

	void setBuilder(TreeBuilder* builder);

	BuildableTree& tree(){ return *tree_; }
	void show();

	void onResponse(int response);
protected:
	void init();
	void onFilterChanged();

	BuildableTree* tree_;
	Entry* filterEntry_;
	TreeBuilder* builder_;
};

template<class T>
TreeObjectFilteredBase* TreeObjectFilteredBase::add(const T& object, const char* name)
{
	return addFiltered(new TreeObjectFiltered<T>(const_cast<T&>(object), name));
}

}

#endif
