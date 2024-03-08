#include "StdAfx.h"
#include "kdw/Navigator.h"
#include "kdw/TreeView.h"
#include "kdw/CommandManager.h"
#include "kdw/PopupMenu.h"
# include "kdw/Sandbox.h"
#include "kdw/Tool.h"
#include "Serialization/SerializationFactory.h"

namespace kdw{

Model* Model::instance_ = 0;

Model* Navigator::model()
{
	return Model::get();
}

Navigator::Navigator(int border)
: Tree(0, border)
, selectedNode_(0)
{
	TreeModel* treeModel = new StringTreeModel();
	
	ASSERT(model() && "Navigator::document is not set!");
	
	treeModel->setRoot(new NavigatorNode(model()->root()));
	model()->signalChanged().connect(this, &Navigator::onModelChanged);
	Tree::setModel(treeModel);

	setColumn(0, new StringTreeColumnDrawer(this), 0.0f);
}

void Navigator::onRowSelected(TreeRow* row)
{
	model()->freeze(0);
	__super::onRowSelected(row);
	model()->root()->setSelectedRecurse(false);
	TreeModel::Selection selection(Tree::model()->selection());
	TreeModel::Selection::iterator it;
	FOR_EACH(selection, it){
		TreeRow* row = Tree::model()->rowFromPath(*it);
		NavigatorNode* nav = safe_cast<NavigatorNode*>(row);
		nav->modelNode()->setSelected(true);
	}
	model()->unfreeze(0);
}

NavigatorNode* Navigator::nodeByPath(const char* path)
{
	NavigatorNode* root = safe_cast<NavigatorNode*>(model()->root());
	return root->nodeByPath(path);
}

void Navigator::onRowRMBDown(TreeRow* treeRow, const Recti& rowRect, Vect2i point)
{
	NavigatorNode* node = safe_cast<NavigatorNode*>(treeRow);

	PopupMenu popupMenu(100);
    node->modelNode()->onContextMenu(popupMenu.root(), this);

    popupMenu.spawn(this);
}

	/*
void Navigator::onSelectionChanged()
{
	if(selectedNode_)
		signal_disconnect(&selectedNode_->signalChanged());
	selectedNode_ = document()->selectedNode();
	if(selectedNode_){
		selectedNode_->signalChanged().connect(this, &Navigator::rebuild);
		if(NavigatorNode* nav = nodeByPath(document()->selectedNodePath())){
			Tree::onRowSelected(nav);
		}
	}
}
	*/

void Navigator::onModelChanged(ModelObserver* changer)
{
	if(changer != this)
		rebuild();
}


void Navigator::rebuild()
{
	if(model() && model()->root()){
		NavigatorNode* node = safe_cast<NavigatorNode*>(Tree::model()->root());
		xassert(node);
		node->build(Tree::model());
		update();
	}
	else{
		ASSERT(0 && "Navigator::setDocument was not called");
	}
}


// --------------------------------------------------------------------------- 

NavigatorNodeRoot::NavigatorNodeRoot(Model* document)
: NavigatorNode("root")
, document_(document)
{
}

// --------------------------------------------------------------------------- 

NavigatorNode::NavigatorNode(const char* path)
: node_(0)
{
	//setType(type);
}

NavigatorNode::NavigatorNode(ModelNode* node)
: node_(node)
, name_(node->name())
{
	//setType(type);
}

NavigatorNode* NavigatorNode::nodeByPath(const char* path)
{
	if(path[0] == '.' && path[1] == 0)
		return this;
	const char* end = path;
	while(*end && *end != '/')
		++end;
	std::string name(path, end);

	for(int i = 0; i < children_.size(); ++i){
		NavigatorNode* node = safe_cast<NavigatorNode*>(&*children_[i]);
		if(strcmp(name.c_str(), node->name()) == 0){
			if(*end == '/')
				return node->nodeByPath(end + 1);
			else
				return node;
		}
	}
	return 0;
}

Model* NavigatorNode::model()
{
	return Navigator::model();
}

void NavigatorNode::onFocus()
{
}

NavigatorNode* NavigatorNode::find(const char* name)
{
	iterator it;
	for(it = begin(); it != end(); ++it){
		NavigatorNode* node = safe_cast<NavigatorNode*>(&**it);
		if(strcmp(node->name(), name) == 0)
			return node;
	}
	return 0;
}

NavigatorNode* NavigatorNode::addNode(NavigatorNode* newNode, TreeModel* model)
{
	NavigatorNode* node = find(newNode->name());
	if(node != 0){
		model->selectRow(node, false);
		replaceAndPreserveState(node, newNode, true);
		newNode->setUpdated(true);
		model->selectRow(newNode, newNode->modelNode()->selected());
		return newNode;
	}
	else{
		newNode->setUpdated(true);
		model->selectRow(newNode, newNode->modelNode()->selected());
		return add(newNode);
	}
}

void NavigatorNode::build(TreeModel* model)
{
	iterator it;
	for(it = begin(); it != end(); ++it){
		NavigatorNode* node = safe_cast<NavigatorNode*>(&**it);
		node->setUpdated(false);		
	}

	ASSERT(node_);
	setLabel(node_->label());
	if(node_->browseable()){
		int count = node_->childrenCount();
		for(int i = 0; i < count; ++i){
			ModelNode* child = node_->child(i);

			// не используем TreeNode::add, т.к. нам нужно сохранять состояние элементов
			NavigatorNode* navNode = addNode(new NavigatorNode(child), model);
			if(navNode->modelNode()->hasChangedChildren())
				navNode->build(model);
		}
	}

	for(it = begin(); it != end(); ){
		NavigatorNode* node = safe_cast<NavigatorNode*>(&**it);
		if(!node->updated())
			it = erase(it);
		else
			++it;
	}

}

void NavigatorNode::setLabel(const char* text)
{
	value_ = text; // StringTreeRow
}

// --------------------------------------------------------------------------- 

ModelNode* ModelNode::findFirstSelectedNode()
{
	if(selected())
		return this;
	int count = children_.size();
	for(int i = 0; i < count; ++i){
		ModelNode* child = children_[i];
		if(child){
			ModelNode* node = child->findFirstSelectedNode();
			if(node)
				return node;
		}
	}
	return 0;
}

void ModelNode::setSelected(bool selected)
{
	bool changed = selected != selected_;
	selected_ = selected;
	if(changed){
		if(selected_)
			onSelect();
		else
			onDeselect();
		setChanged(true);
	}
}

void ModelNode::setSelectedRecurse(bool selected)
{
	setSelected(selected);
	ModelNodes::iterator it;
	FOR_EACH(children_, it){
		ModelNode* child = *it;
		child->setSelectedRecurse(selected);
	}
}

void ModelNode::setHasChangedChildren(bool hasChangedChildren)
{
	hasChangedChildren_ = true;
	propagateChanges();
}

ModelNode* ModelNode::add(ModelNode* node, ModelNode* beforeNode)
{
	node->parent_ = this;
	if(beforeNode == 0)
		children_.push_back(node);
	else{
		ModelNodes::iterator it = std::find(children_.begin(), children_.end(), beforeNode);
		xassert(it != children_.end());
		children_.insert(it, node);
    }
	node->onAdd();
	setHasChangedChildren(true);
	propagateChanges();
	return node;
}

void ModelNode::clear()
{
	ModelNodes::iterator it;
	FOR_EACH(children_, it){
		(*it)->onRemove();
	}
	children_.clear();
}

ModelNode* ModelNode::child(int index)
{
	ASSERT(index >= 0 && index < children_.size());
	return children_[index];
}

int ModelNode::childIndex(ModelNode* node)
{
	int count = children_.size();
	for(int i = 0; i < count; ++i){
		if(children_[i] == node)
			return i;
	}
    return -1;
}

ModelNode* ModelNode::child(const char* name)
{
	for(int i = 0; i < children_.size(); ++i){
		ASSERT(children_[i]);
		if(strcmp(children_[i]->name(), name) == 0){
			return children_[i];
		}
	}
	return 0;
}

void ModelNode::remove(ModelNode* child)
{
	ModelNodes::iterator it = std::find(children_.begin(), children_.end(), child);
	if(it != children_.end()){
		child->onRemove();
		children_.erase(it);
		setHasChangedChildren(true);
	}
	else
		xassert(0 && "No such children");
}

void ModelNode::setChanged(bool changed)
{
	if(changed && !changed_)
		propagateChanges();
	changed_ = changed;
}

void ModelNode::propagateChanges()
{
    ModelNode* parent = this->parent();
	while(parent && !parent->hasChangedChildren()){
		parent->setHasChangedChildren(true);
		parent = parent->parent();
	}
}

// --------------------------------------------------------------------------- 

Model::Model()
: sandbox_(new Sandbox())
, sandboxRenderer_(0)
, baseTool_(0)
, currentTool_(0)
, lastTool_(0)
, frozenCounter_(0)
{
}

void Model::freeze(ModelObserver* refrigerator)
{
    ++frozenCounter_;
}

void Model::unfreeze(ModelObserver* refrigerator)
{
	xassert(frozenCounter_ > 0);
	--frozenCounter_;

	if(frozenCounter_ == 0){
		onChanged(refrigerator);

		ModelNodes changedNodes;
		//root()->collectChangedNodes(changedNodes);

		//ModelNodes::
	}
}

void Model::setRoot(ModelNode* root)
{
	root_ = root;
}

void Model::onActiveToyChanged(Toy* toy)
{
	signalActiveToyChanged_.emit(toy);
}

std::string Model::nodePath(ModelNode* node)
{
	if(!node)
		return "";
	if(node == root())
		return ".";
	std::string result = node->name();
	ModelNode* n = node->parent();
	while(n->parent()){
		result = std::string(n->name()) + "/" + result;
		n = n->parent();
	}
	return result;
}

ModelNode* Model::findFirstSelectedNode()
{
	xassert(root_);
	return root_->findFirstSelectedNode();
}

void Model::toggleTool(Tool* tool, bool stickyToggle)
{
	if(tool == baseTool_){
		if(currentTool_){
			if(currentTool_ != tool && !stickyToggle)
				lastTool_ = tool;
			currentTool_->onDeselect();
			currentTool_ = 0;
			baseTool_->onSelect();
		}
	}
	else{
		if(currentTool_ != tool){
			if(currentTool_){
				currentTool_->onDeselect();
				lastTool_ = currentTool_;
			}
			currentTool_ = tool;
			currentTool_->onSelect();
		}
		else{
			currentTool_->onDeselect();
			currentTool_ = 0;
			if(stickyToggle && lastTool_ && lastTool_ != currentTool_){
				currentTool_ = lastTool_;
				currentTool_->onSelect();
			}
			else
				baseTool_->onSelect();
		}
	}
}

void Model::replaceTool(Tool* tool)
{
	if(tool == baseTool_){
		if(currentTool_){
			currentTool_->onDeselect();
			currentTool_ = 0;
			baseTool_->onSelect();
		}
	}
	else{
		if(currentTool_ != tool){
			if(currentTool_){
				currentTool_->onDeselect();
				lastTool_ = currentTool_;
			}
			currentTool_ = tool;
			currentTool_->onSelect();
		}
	}
}

void Model::setBaseTool(Tool* baseTool)
{
	baseTool_ = baseTool;
}

Tool* Model::currentTool()
{
	xassert(baseTool_);
	return currentTool_ ? currentTool_ : baseTool_;
}

// --------------------------------------------------------------------------- 

ModelTimed::ModelTimed()
: timeFraction_(0.0)
, lastTime_(double(xclock()))
, lastSignalTime_(0.0)
, signalPeriod_(0.05f)
, pause_(false)
, time_(0.0f)
, timeToSet_(0.0f)
, timeChanger_(0)
{
	lastSignalTime_ = lastTime_;
}

void ModelTimed::onIdle()
{
	int time = xclock();
	double dt = (double)time - (double)lastTime_;
	if(dt < 0 || dt > 1000)
		dt = 0;
	double fraction = timeFraction_ + dt * 1.0; // -1.0
	int intTime = round(fraction);

	rewinded_ = false;
	if(!pause_){
		time_ += float(dt) * 0.001f;
		if(duration_ > 0.0f){
			while(time_ > duration_){
				time_ -= duration_;
				rewinded_ = true;
			}
		}
		quant(float(intTime) * 0.001f);
		if(double(lastTime_) - lastSignalTime_ > signalPeriod_){
			signalTimeChanged_.emit(this);
			lastSignalTime_ = time;
		}
	}
	else{
		quant(0.0f);
	}

	lastTime_ = time;
	timeFraction_ = fraction - intTime;
}

void ModelTimed::setSignalPeriod(float period)
{
	signalPeriod_ = period;
}

void ModelTimed::setTime(float time, ModelObserver* who)
{
	time_ = time;
	timeToSet_ = time;
	signalTimeChanged_.emit(who);
}

void ModelTimed::setDuration(float duration)
{
	duration_ = duration;
}

void ModelTimed::setPause(bool pause, kdw::ModelObserver* changer)
{
	pause_ = pause;
	signalTimeChanged_.emit(changer);
}

// --------------------------------------------------------------------------- 

NavigatorSpace::NavigatorSpace()
: Space()
, navigator_(new Navigator)
{
	add(navigator_);
	navigator_->rebuild();

	commands().get("navigator");
	updateFilterMenu();
	setMenu("navigator");
}

void NavigatorSpace::updateFilterMenu()
{
	kdw::NavigatorNode::Factory& factory = kdw::NavigatorNode::Factory::instance();
	const ComboStrings& strings = factory.comboStrings();
	const ComboStrings& stringsAlt = factory.comboStringsAlt();
	int count = strings.size();
	for(int i = 0; i < count; ++i){
		std::string commandName = "navigator.filter.";
		commandName += strings[i];
		commands().get(commandName.c_str(), stringsAlt[i].c_str()).connect(this, NavigatorSpace::onMenuFilter, i);
	}
}

void NavigatorSpace::onMenuFilter(int index)
{

}

}
