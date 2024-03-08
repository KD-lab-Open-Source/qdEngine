// SelectionManager.h: interface for the SelectionManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELECTIONMANAGER_H__01685F73_42E8_4E8A_85BF_8DA6BAF6F0F4__INCLUDED_)
#define AFX_SELECTIONMANAGER_H__01685F73_42E8_4E8A_85BF_8DA6BAF6F0F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "qd_trigger_element.h"
class qdTriggerChain;
class qdTriggerElement;

/**
	Управляет набором выбранных элементов цепочки триггеров
 */
class SelectionManager  
{

public:
	typedef std::vector<qdTriggerElementPtr>::const_iterator const_iterator;

	SelectionManager();
	~SelectionManager();

	/// Выбирает элемент.
	void select(qdTriggerElementPtr const& element);
	/// Убирает из списка выбнанных
	void deselect(qdTriggerElementPtr const& element);
	/// Сначала убирает из списка, потом заново добалвляет
	void reselect(qdTriggerElementPtr const& element);

	/// Убирает все выбранные
	void deselectAll();
	//! очищает внутрений буффер
	void clear();

	/// Возвращает последний выбранный
	qdTriggerElementPtr const getLast() const;

	/// Является ли элемент выбранным
	bool isSelected(qdTriggerElementPtr const& element) const;

	/// Начало списка
	const_iterator begin() const;
	/// Конец списка
	const_iterator end() const;
	/// количество выбранных элементов
	size_t getCount() const;
	/// Пустой ли список выбранных элементов
	bool empty() const;
protected:
	/// Если select==true, то вставляет элемент в список, иначе удалеят из списка
	/**
		Если элемент уже в списке, то он туда больше не кладется.
	 */
	void select(qdTriggerElementPtr const element, bool select);
//	qdTriggerElement& getTrigger(int index) const;
private:
	/// Список выбранных элемнтов
	std::vector<qdTriggerElementPtr> selected_;
};

#endif //!defined(AFX_SELECTIONMANAGER_H__01685F73_42E8_4E8A_85BF_8DA6BAF6F0F4__INCLUDED_)
