#pragma once
//#include "gameobjecttreenode.h"
#include "animatedobjecttreenode.h"

/// Класс узла, принадлежащего персонажу
class MovingObjectTreeNode : public AnimatedObjectTreeNode
{
public:
	MovingObjectTreeNode(qdNamedObject* object);
	~MovingObjectTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	bool reload(ITreeNodeFactory* factory, TreeLogic& logic, 
				ITreeNode const* parent, qdNamedObjectBase const* owner);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;
	bool showProperties(TreeLogic& logic, int iActive);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
	// при его удаление(сотояния, когда удаляется анимированный объект. 
	//Все объекты, когда удаляется сцена)
	virtual bool deleteAsSubObject(TreeLogic& logic);

	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);
	virtual DROPEFFECT onDropData(TreeLogic& logic, ITreeNode* dropTarget, 
		DROPEFFECT dropEffect);
	virtual void updateItemImage(TreeLogic& treeLogic);
protected:
	class InsertState;
	/// \brief Создает папки состояний и вызывает 
	/// loadStates(ITreeNodeFactory* factory, TreeLogic& treeLogic,
	/// ITreeNode* staticStates, ITreeNode* walkStates) для загрузки состояний
	bool loadStates(ITreeNodeFactory* factory, TreeLogic& treeLogic) ;
	/// Загружает состояния, расскладывая их по соответствующим папкам
	bool loadStates(ITreeNodeFactory* factory, TreeLogic& treeLogic,
					ITreeNode* staticStates, ITreeNode* walkStates) ;
	/// Переименовывает масштаб для объекта
	static void renameScale(qdNamedObject const* obj, LPCTSTR oldName, LPCTSTR newName);
	/// Переменовывает объек и его масштаб
	static bool doRename(TreeLogic& logic, qdNamedObject* object, LPCTSTR newName);
};
