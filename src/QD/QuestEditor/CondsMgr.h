#pragma once
/********************************************************************
	created:	2003/04/24
	created:	24:4:2003   14:54
	filename: 	d:\Projects\Quest\QuestEditor\CondsMgr.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	CondsMgr
	file ext:	h
	author:		Илюха
	
	purpose:	Переправляет условия в дерево и из дерева в объект.
*********************************************************************/

#include "mytree.h"
#include <map>
#include <boost/shared_ptr.hpp>

class qdConditionalObject;
class qdConditionGroup;
class qdCondition;

/**
	Переправляет условия в дерево и из дерева в объект.
	Так как эта функциональность общая для StateCondDlg и CO_ConditionDlg 
	она была вынесена в отдельный класс
 */
class CondsMgr
{
public:
	CondsMgr(CMyTree& tree);
	~CondsMgr();

	/// Были ли изменения
	bool was_changed() const{
		return m_bChanged;
	}
	/// Назначает редактируемый объект
	void set_obj(qdConditionalObject* pobj);

	/// Создает группу
	HTREEITEM CreateGroup(qdConditionGroup::conditions_mode_t mode);
	/// \brief Для каждого корневого условия создает группу с указанным режимом 
	/// и перемещает условие в эту группу
	void CreateGroups(qdConditionGroup::conditions_mode_t mode);

	/// Добавляет условие в список условий и в дерево
	/**
		Если выбрана группа, то добавляет в группу, иначе просто в корень 
	 */
	bool AddCondition(qdCondition const* pcond);
	/// удаляет выделенные условия из группы
	void RemoveFromGroup();

	/// меняет тип с И на ИЛИ и наоборот у выделенных узлов
	void InvertType();
	/// меняет тип с И на ИЛИ и наоборот для узла
	void InvertType(HTREEITEM hItem);

	/// Проверяет является ли узел узлом группы
	bool IsGroup(HTREEITEM hItem)const;

	//! Дерево в условия qdCoditionObject
	void TreeToCO();
	//! Условия в дерево
	void CondsToTree();
	//! Загрузка групп
	bool LoadGroups();

	/// Удаляет все выбранные условия
	void DeleteSelected();
	/// Удаляет все
	void DeleteAll();


	// Drag&Drop

	LRESULT OnCanDropReq(NM_MT_CAN_DROP* pcd);
	LRESULT OnCanDragReq(NM_MT_CAN_DRAG* pcd);
	LRESULT OnDropReq(NM_MT_DROP* pmd);

	LRESULT OnNMRclickTree(CWnd* from);
protected:
	/// меняет тип с И на ИЛИ и наоборот для группы
	void InvertType(qdConditionGroup& cg);

	/// Считывает из дерева содержимое групп
	void ReadGroupsFromTree();
	/// Считывает из дерева содержимое конкретной группы(hItem - узел группы)
	void ReadGroupFromTree(HTREEITEM hItem);

	/// Индекс условия в списке условий
	int GetCondIndex(const qdCondition* p);

	/// Удаляет группу из дерева и из индекса.
	/**
		Все условия переносятся в корень дерева
	 */
	void DeleteGroup(HTREEITEM hItem);
	/// Удаляет условие из дерева и из списка
	void DeleteCondition(HTREEITEM hItem);
private:
	/// Вставляет условие в дерево и в список условий m_conditions
	void ConditionToTree(const qdCondition* pcond, 
						HTREEITEM hParent,
						HTREEITEM hInsertAfter = TVI_LAST);
	/// Возвращает узел первой группы содержащей условие
	HTREEITEM FindCondGroup(int iCondNum);
	/// По режиму возвращает заголовок группы
	CString GetGroupCaption(qdConditionGroup::conditions_mode_t mode);
private:
	/// Контрол реализующий дерево
	CMyTree& m_tree;
	/// Редактируемый объект
	qdConditionalObject* m_ptrObj;
	/// Произошло ли изменение
	bool m_bChanged;

	typedef std::map<HTREEITEM, boost::shared_ptr<qdConditionGroup> > GroupItems_t;
	/// Индекс групп
	GroupItems_t m_group_items;

	typedef std::vector<boost::shared_ptr<qdCondition> > Conditions;
	/// Список условий
	Conditions m_conditions;
};
