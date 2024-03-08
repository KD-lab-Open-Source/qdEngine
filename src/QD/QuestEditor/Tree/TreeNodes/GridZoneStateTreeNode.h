#pragma once
#include ".\conditionalobjecttreenode.h"
/// Класс узла, принадлежащего состоянию сеточной зоны
class GridZoneStateTreeNode :
	public ConditionalObjectTreeNode
{
public:
	GridZoneStateTreeNode(LPCTSTR name, qdNamedObject* object);
	~GridZoneStateTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, 
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
protected:
	/// Возвращает имя состояния
	LPCTSTR getName() const;
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;
private:
	/// Имя состояния
	CString name_;
};
