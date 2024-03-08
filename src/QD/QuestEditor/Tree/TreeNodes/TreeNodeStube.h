#pragma once
#include "tree\itreenode.h"
#include "Tree/ITreeNodeFactory.h"

#include "Tree/TreeLogic.h"

/// @brief Реализация методов ITreeNode по умолчанию. Содержит функциональность 
/// необходимую всем узалм дерева
class TreeNodeStube :
	public ITreeNode
{
public:
	TreeNodeStube(void);
	virtual ~TreeNodeStube(void);
	//! Загрузка узла в дерево
	virtual bool load(ITreeNodeFactory* factory,
		TreeLogic& treeLogic, CTreeListItem* parent, qdNamedObjectBase const* owner);
	//! Перезагружает содержимое 
	virtual bool reload(ITreeNodeFactory* factory, TreeLogic& logic, 
						ITreeNode const* parent, 
						qdNamedObjectBase const* owner);

	CTreeListItem* getTreeListItem() const;

	virtual void onRClick(TreeLogic& treeLogic);

	virtual bool showProperties(TreeLogic& treeLogic, int activePage);
	virtual void updateItemImage(TreeLogic& treeLogic);
	virtual bool showConditions(TreeLogic& logic);

	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool onCheckNode(TreeLogic& logic, bool bCheck);

	//! Удаляет объект движка, к которому привязан узел. ask спрашивать ли 
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
	/// при его удаление(сотояния, когда удаляется анимированный объект. 
	/// Все объекты, когда удаляется сцена)
	virtual bool deleteAsSubObject(TreeLogic& logic);
	//! Можно ли перетягивать узел
	virtual bool canDrag(TreeLogic& logic);
	//! Можно ли кинуть узел на тот, над которым тянем
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);

	virtual DROPEFFECT onDropData(TreeLogic& logic, 
							ITreeNode* dropTarget, 
							DROPEFFECT dropEffect);
	//! Возвращает true, если удалось переимновать, false в противном случае
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
	//! Можно ли сменить имя узлу
	virtual bool canRename(TreeLogic& logic);

	void* operator new(size_t size);
	void operator delete(void* p);

	void* operator new(size_t size, LPCTSTR file, unsigned line);
	void operator delete(void* p, LPCTSTR file, unsigned line);

protected:
	void setTreeListItem(CTreeListItem* item);
	/// Вставляет узел в дерево и делает запись в индекс узлов дерева в TreeLogic
	CTreeListItem* insertToTree(TreeLogic& treeLogic, 
										LPCTSTR title, 
										ITreeNode* parent, 
										UINT uiImage, 
										DWORD_PTR indexKey,
										ITreeNode* insertAfter);
	/// Удаляет запись из индекса узлов дерева
	void removeIndexEntry(TreeLogic& logic, DWORD_PTR indexKey);

	//! Возвращает идентификатор ресурса меню для узла
	virtual DWORD getMenuResId() const = 0;
	//! Модифицирует меню, так чтобы оно соответствовало содержимому узла
	/**
		@param treeLogic - логика дерева
		@param pm - меню соответствующее узлу
	 */
	virtual void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;

	static bool showPropDlg(CPropertyPage** ptrPages, int iActive, UINT uiCaption);

	/// Показывает чек-бокс у узла дерева
	void showCheckBox(TreeLogic& logic, bool bShow) const;
	/// Ставит/убирает галочку в чекбоксе
	void setCheck(TreeLogic& logic, bool bCheck) const;
	/// Спрашивает у пользователя разрешения удалить объект с именем objectName
	static bool askUserForDelete(LPCTSTR objectName);

	/// Конвертирует ITreeNode в CTreeListItem
	static CTreeListItem* nodeToItem(ITreeNode const *node);
	/// Конвертирует CTreeListItem в ITreeNode
	static ITreeNode* itemToNode(CTreeListItem* item);

private:
	/// Узел дерева, соответствующий этому объекту
	CTreeListItem* item_;
};
