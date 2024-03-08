#ifndef __NAVIGATOR_H_INCLUDED__
#define __NAVIGATOR_H_INCLUDED__

#include "kdw/Tree.h"
#include "kdw/TreeModel.h"
#include "kdw/Space.h"
#include "kdw/Document.h"
#include "Serialization/Serializer.h"

template<class BaseType, class FactoryArg>
class SerializationFactory;

namespace kdw{

class NavigatorNode : public StringTreeRow{
public:
	typedef SerializationFactory<NavigatorNode, FactoryArg0<NavigatorNode> > Factory;
	explicit NavigatorNode(const char* path);
	explicit NavigatorNode(ModelNode* node = 0);
	void build(TreeModel* model);
	virtual void onFocus();

    Model* model();
	ModelNode* modelNode(){ return node_; }
	void setLabel(const char* text);
	const char* name() const{ return name_.c_str(); }

	NavigatorNode* nodeByPath(const char* path);

	NavigatorNode* parent() { return static_cast<NavigatorNode*>(TreeRow::parent()); }
	const NavigatorNode* parent() const{ return static_cast<const NavigatorNode*>(TreeRow::parent()); }
protected:

	NavigatorNode* find(const char* name);
	NavigatorNode* addNode(NavigatorNode* newNode, TreeModel* model);

	bool updated()const{ return updated_; }
	void setUpdated(bool updated){ updated_ = updated; }

	ModelNode* node_;
	NavigatorNodeType type_;
	std::string path_;
	std::string name_;
	bool updated_;
};

class NavigatorNodeRoot : public NavigatorNode{
public:
	NavigatorNodeRoot(Model* document);
	Model* document(){ return document_; }
protected:
	Model* document_;
};

class Navigator : public Tree, public ModelObserver{
public:
	Navigator(int border = 0);
	void rebuild();

	void onRowSelected(TreeRow* row);
	NavigatorNode* nodeByPath(const char* path);

	void onRowRMBDown(TreeRow* row, const Recti& rowRect, Vect2i point);
	
	void onModelChanged(ModelObserver* changer);
	
	static Model* model();
protected:
	ModelNode* selectedNode_;
};

class NavigatorSpace : public Space{
public:
	NavigatorSpace();

	void updateFilterMenu();

	void onMenuFilter(int index);
protected:
	Navigator* navigator_;
};

}

#endif
