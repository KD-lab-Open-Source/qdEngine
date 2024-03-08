#pragma once

interface ITreeNode;
interface ITreeNodeFactory;

class qdNamedObject;
class qdInterfaceObjectBase;

class TreeLogic;

#include "Main/QETreeListCtrl.h"

enum ContentClassificator{
	CC_FOLDER,///< Узел, не содержащий ни какой объект
	CC_NAMED_OBJECT, ///< Узел, созданный на основе потомка qdNamedObject
	CC_INTERFACE_OBJECT///< Узел, созданный на основе потомка qdInterfaceObjectBase
};

enum eNodeState{
	NS_SELECTED = 0x01,
	NS_FOCUSED	= 0x02,
};
/// Что содержит ужел в своих данных. 
/**
	Поля используются в зависимости от классификатора ContentClassificator
 */
typedef union tagNodeContent{
	DWORD folderId;
	qdNamedObject* gameObject;
	qdInterfaceObjectBase* interfaceObject;
}NodeContent;

//! Вставлять узел первым
ITreeNode* const TN_INSERT_FIRST = (ITreeNode*)0xFFFFFFFF;
//! Вставлять узел последним
ITreeNode* const TN_INSERT_LAST	= (ITreeNode*)0xFFFFFFFE;
//! Вставлять узел сортированно
ITreeNode* const TN_INSERT_SORT	= (ITreeNode*)0xFFFFFFFC;
//! Корневой узел
ITreeNode* const TN_ROOT = (ITreeNode*)0xFFFFFFFB;

/// Базовый интерфейс для узлов дерева
struct ITreeNode
{
	virtual ~ITreeNode() = 0;
	virtual CTreeListItem* getTreeListItem() const = 0;

	//! Что храниться в узле
	virtual ContentClassificator getContentClassificator() const = 0;
	//! Возвращает то, что храниться в узле
	virtual NodeContent getContent() const = 0;
	//! Загрузка узла в дерево
	/*!
		Должна возвратить \a false,если не удалось вставить в дерево узел. 
		Если не удалось загрузить под узлы, возвращается true
	*/
	virtual bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, 
					ITreeNode* parent, qdNamedObjectBase const* owner, 
					ITreeNode* insertAfter = TN_INSERT_LAST) = 0;
	//! Добавляет недостающие части
	/*!
		parent и owner допускаются равными NULL, если node уже был загружен.
	*/
	virtual bool reload(ITreeNodeFactory* factory, TreeLogic& logic,
							ITreeNode const* parent, 
							qdNamedObjectBase const* owner) = 0;

	//! Вызывается на удаление узла из дерева
	virtual void onDeleteItem(TreeLogic& treeLogic) = 0;

	/// Обрабатывает событие "Клик правой кнопокой мыши"
	virtual void onRClick(TreeLogic& treeLogic) = 0;

	virtual bool showProperties(TreeLogic& treeLogic, int activePage) = 0;
	virtual bool showConditions(TreeLogic& logic) = 0;

	//! Обновляет изображение рядом с узлом в дереве
	virtual void updateItemImage(TreeLogic& treeLogic) = 0;

	//! На изменения состояния узла в дереве
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState) = 0;
	//! На изменение состояния чекбокса в дереве на против узла
	virtual bool onCheckNode(TreeLogic& logic, bool bCheck) = 0;
	//! Удаляет объект движка, к которому привязан узел. ask спрашивать ли 
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends) = 0;
	//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
	/// при его удаление(сотояния, когда удаляется анимированный объект. 
	/// Все объекты, когда удаляется сцена)
	virtual bool deleteAsSubObject(TreeLogic& logic) = 0;

	//! Можно ли перетягивать узел
	virtual bool canDrag(TreeLogic& logic) = 0;
	//! Можно ли кинуть узел на тот, над которым тянем
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode) = 0;
	//! Узел бросили на dropTarget 
	virtual DROPEFFECT onDropData(TreeLogic& logic, 
							ITreeNode* dropTarget, 
							DROPEFFECT dropEffect) = 0;
	//! Возвращает true, если удалось переимновать, false в противном случае
	virtual bool rename(TreeLogic& logic, LPCTSTR newName) = 0;
	//! Можно ли сменить имя узлу
	virtual bool canRename(TreeLogic& logic) = 0;
};


