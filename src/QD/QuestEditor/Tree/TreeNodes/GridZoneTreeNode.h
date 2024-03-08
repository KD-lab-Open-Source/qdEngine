#pragma once
#include "namedobjecttreenode.h"

#include <boost/scoped_ptr.hpp>

#include "IWorkOwner.h"

/// Класс узла, принадлежащий зоне
class GridZoneTreeNode : public NamedObjectTreeNode
	, public IWorkOwner
{
public:
	GridZoneTreeNode(qdNamedObject* zone);
	~GridZoneTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	virtual void updateItemImage(TreeLogic& treeLogic);
	virtual bool showProperties(TreeLogic& logic, int iActive);
	virtual bool onCheckNode(TreeLogic& logic, bool bCheck);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
	/// при его удаление(сотояния, когда удаляется анимированный объект. 
	/// Все объекты, когда удаляется сцена)
	virtual bool deleteAsSubObject(TreeLogic& logic);
	virtual bool canDrag(TreeLogic& logic);
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);
	virtual DROPEFFECT onDropData(TreeLogic& logic, ITreeNode* dropTarget, 
		DROPEFFECT dropEffect);

	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	static int selectPicture(qdNamedObject const* pgz);
	bool loadStates(ITreeNodeFactory* factory, TreeLogic& treeLogic);
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;

	///	Удаляет состояния зоны из триггеров
	void removeGridZoneFromTriggers(qdGridZone *zone, qdGameDispatcher* gameDisp);

	/// Сохраняет изменения сделанные в редакторе сцены
	virtual bool onSave(bool bSave);
	/// Возвращает true, если newName уникально
	bool nameTest(TreeLogic& logic, LPCTSTR newName);
private:
	//! Хранит копию выделенной зоны. Этот объект не редактируется. 
	static boost::scoped_ptr<qdGridZone> zoneForEdit_;
};
