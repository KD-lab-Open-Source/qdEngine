#pragma once

#include <list>
#include <utility>
#include <boost/function.hpp>

/**
	Управляет записями для отмены/повтора действий
 */
class TEUndoManager
{
public:
	/// Тип действия
	typedef boost::function0<bool> Action;
private:
	/// Пара действий(ПД) отмена/повтор
	typedef std::pair<Action, Action> DOREDOPair;
	/// Контейнер ПД
	typedef std::list<DOREDOPair> DOREDOContainer;
	/// обратный итератор
	typedef DOREDOContainer::reverse_iterator reverse_iterator;
	/// прямой итератор
	typedef DOREDOContainer::iterator iterator;
public:
	TEUndoManager(void);
	~TEUndoManager(void);

	TEUndoManager(TEUndoManager const&);
	/// Добавляет ПД из переданных действий
	bool addAction(Action const& actionUNDO, Action const& actionREDO);
	/// Отменяет последнее неотменненое действие. 
	/// Передвигает указатель на предыдущее действие
	bool undo();
	/// Повторяет последнее отменненое действие
	/// Передвигает указатель на следующее действие
	bool redo();
	/// можем ли ундить
	bool canUNDO();
	/// можем ли повторить
	bool canREDO();
public://types

	/**
		Набор действий, которые нужно выполнить(отменить/повторить) одновременно

		Сам по себе набор за счет bool operator()() тоже явяляется действием
	 */
	class Bunch
	{
		typedef std::list<Action> ActionContainer;
	public:
		/// Пробегается по всем действиям и выполняет их
		bool operator()() const;
		/// Добавляет действие в пакет
		bool addAction(Action const& action);
		/// Пустой ли пакет
		bool empty() const;
		/// \brief меняем порядок на обратный
		/**
			Для undo-пакетов нужно менять порядок действий на обратный,
			потому как undo надо делать в обратном порядке
		 */
		void reverse();
	private:
		ActionContainer m_buffer;
	};
protected:

	void setUNDOPointer(reverse_iterator const& itr);
	reverse_iterator const& getUNDOPointer() const;
	void setREDOPointer(iterator const& itr);
	iterator const& getREDOPointer() const;

	/// сдвигаем итераторы на операцию undo
	void undoMoveIterators();
	/// сдвигаем итераторы на операцию redo
	void redoMoveIterators();

	/// Выполняет undo-действие
	bool undo(DOREDOPair const& pair) const;
	/// Выполняет redo-действие
	bool redo(DOREDOPair const& pair) const;
private:
	/// Список действий
	DOREDOContainer m_buffer;
	/// Текущая undo позиция
	reverse_iterator m_itrUNDOPointer; 
	/// Текущая redu позиция
	iterator m_itrREDOPointer;
};
