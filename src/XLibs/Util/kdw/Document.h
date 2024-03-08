#ifndef __KDW_DOCUMENT_H_INCLUDED__
#define __KDW_DOCUMENT_H_INCLUDED__

#include "kdw/Application.h" // IdleListener
#include "XTL/sigslot.h"
#include "Serialization/Serializer.h"

namespace kdw{

class Tool;
class Toy;
class Widget;
class ModelNode;
class Sandbox;
class SandboxRenderer;
typedef std::vector<ShareHandle<ModelNode> > ModelNodes;

class PopupMenuItem;

class ModelObserver{
public:

};

class ModelNode : public ShareHandleBase, public ModelObserver, public sigslot::has_slots{
public:
	ModelNode(bool affectedByChildrenChanges = false)
	: parent_(0)
	, affectedByChildrenChanges_(affectedByChildrenChanges)
	, selected_(false)
	{}
	virtual ~ModelNode() {}
	const char* label() const{ return name_.c_str(); }
	const char* name() const{ return name_.c_str(); }
	void setName(const char* name){ name_ = name; }
	virtual ModelNode* add(ModelNode* node, ModelNode* beforeNode = 0);
	void clear();
	
	ModelNode* child(int index);
	int childIndex(ModelNode* node);
	ModelNode* child(const char* name);
	void remove(ModelNode* child);
	int childrenCount() const{ return children_.size(); }

	virtual bool browseable() const{ return true; }
	virtual void onContextMenu(PopupMenuItem& root, kdw::Widget* widget) {}
	void setSelected(bool selected);
	void setSelectedRecurse(bool selected);
	virtual void onSelect() { selected_ = true; }
	virtual void onDeselect() { selected_ = false; }
	bool selected() const{ return selected_; }
	virtual void onAdd() {}
	virtual void onRemove() {}
	virtual Serializer serializer() { return Serializer(); }

	ModelNode* parent(){ return parent_; }

	void setChanged(bool changed);
	bool changed() const{ return changed_; }
	void propagateChanges();

	void setHasChangedChildren(bool hasChangedChildren);
	bool hasChangedChildren() const{ return hasChangedChildren_; }

	typedef sigslot::signal1<ModelObserver*> SignalChanged;
	SignalChanged& signalChanged(){ return signalChanged_; }

	ModelNode* findFirstSelectedNode();
protected:
	SignalChanged signalChanged_;

	// вызывать ли signalChanged при изменении дочерних элементов
	bool affectedByChildrenChanges_;

	ModelNode* parent_;
	ModelNodes children_;
	std::string name_;
	bool selected_;

	bool changed_;
	bool hasChangedChildren_;
};

struct NavigatorNodeType{
	explicit NavigatorNodeType(int index = -1) : index_(index){}

	int index_;
};

class Model : public ModelObserver{
public:
	Model();
	virtual ~Model() {}

	typedef std::vector<std::string> NodeTypes;

	const NodeTypes& nodeTypes() { return nodeTypes_; }
	
	void setRoot(ModelNode* root);
	ModelNode* root(){ return root_; }

	Sandbox* sandbox(){ return sandbox_; }
	SandboxRenderer* sandboxRenderer(){ return sandboxRenderer_; }

	std::string nodePath(ModelNode* node);
	ModelNode* findFirstSelectedNode();

	void replaceTool(Tool* tool);
	void toggleTool(Tool* tool, bool stickyToggle = false);
	void setBaseTool(Tool* tool);
	Tool* currentTool();

	typedef std::vector<Tool*> Tools;

	static Model* get(){ return instance_; }
	static void set(Model* document){
		instance_ = document;
	}

	typedef sigslot::signal1<ModelObserver*> SignalChanged;
	SignalChanged& signalChanged(){ return signalChanged_; }
	virtual void onChanged(ModelObserver* observer){ signalChanged_.emit(observer); }

	typedef sigslot::signal1<Toy*> SignalActiveToyChanged;
	SignalActiveToyChanged& signalActiveToyChanged(){ return signalActiveToyChanged_; }
	virtual void onActiveToyChanged(Toy* toy);

	void freeze(ModelObserver* refrigerator);
	void unfreeze(ModelObserver* melter);
protected:
	SignalActiveToyChanged signalActiveToyChanged_;
	SignalChanged signalChanged_;

	int frozenCounter_;

	NodeTypes nodeTypes_;
	ShareHandle<ModelNode> root_;

	Sandbox* sandbox_;
	SandboxRenderer* sandboxRenderer_;

	Tool* baseTool_;
	Tool* lastTool_;
	Tool* currentTool_;

	static Model* instance_;
};

class ModelTimed : public Model, public sigslot::has_slots {
public:
	ModelTimed();

	virtual void quant(float deltaTime) {}
	virtual void setPause(bool pause, kdw::ModelObserver* changer);
	bool pause() const{ return pause_; }

	typedef sigslot::signal1<ModelObserver*> SignalTimeChanged;
	SignalTimeChanged& signalTimeChanged(){ return signalTimeChanged_; }

	void setSignalPeriod(float period);

	// на этом кадре произошел warp в начало
	bool rewinded() const{ return rewinded_; }
	void setTime(float time, ModelObserver* setter);
	float time() const{ return time_; }
	void setDuration(float time);
	float duration() const{ return duration_; }

	void onIdle();

protected:
	SignalTimeChanged signalTimeChanged_;
	float signalPeriod_;
	double lastSignalTime_;

	float timeToSet_;
	ModelObserver* timeChanger_;
	float time_;
	bool rewinded_;
	float duration_;
	bool pause_;

	///
	double lastTime_;
	double timeFraction_;
};

}
#endif
