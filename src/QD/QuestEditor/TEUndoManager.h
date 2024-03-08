#pragma once

#include <list>
#include <utility>
#include <boost/function.hpp>

/**
	��������� �������� ��� ������/������� ��������
 */
class TEUndoManager
{
public:
	/// ��� ��������
	typedef boost::function0<bool> Action;
private:
	/// ���� ��������(��) ������/������
	typedef std::pair<Action, Action> DOREDOPair;
	/// ��������� ��
	typedef std::list<DOREDOPair> DOREDOContainer;
	/// �������� ��������
	typedef DOREDOContainer::reverse_iterator reverse_iterator;
	/// ������ ��������
	typedef DOREDOContainer::iterator iterator;
public:
	TEUndoManager(void);
	~TEUndoManager(void);

	TEUndoManager(TEUndoManager const&);
	/// ��������� �� �� ���������� ��������
	bool addAction(Action const& actionUNDO, Action const& actionREDO);
	/// �������� ��������� ������������ ��������. 
	/// ����������� ��������� �� ���������� ��������
	bool undo();
	/// ��������� ��������� ���������� ��������
	/// ����������� ��������� �� ��������� ��������
	bool redo();
	/// ����� �� ������
	bool canUNDO();
	/// ����� �� ���������
	bool canREDO();
public://types

	/**
		����� ��������, ������� ����� ���������(��������/���������) ������������

		��� �� ���� ����� �� ���� bool operator()() ���� ��������� ���������
	 */
	class Bunch
	{
		typedef std::list<Action> ActionContainer;
	public:
		/// ����������� �� ���� ��������� � ��������� ��
		bool operator()() const;
		/// ��������� �������� � �����
		bool addAction(Action const& action);
		/// ������ �� �����
		bool empty() const;
		/// \brief ������ ������� �� ��������
		/**
			��� undo-������� ����� ������ ������� �������� �� ��������,
			������ ��� undo ���� ������ � �������� �������
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

	/// �������� ��������� �� �������� undo
	void undoMoveIterators();
	/// �������� ��������� �� �������� redo
	void redoMoveIterators();

	/// ��������� undo-��������
	bool undo(DOREDOPair const& pair) const;
	/// ��������� redo-��������
	bool redo(DOREDOPair const& pair) const;
private:
	/// ������ ��������
	DOREDOContainer m_buffer;
	/// ������� undo �������
	reverse_iterator m_itrUNDOPointer; 
	/// ������� redu �������
	iterator m_itrREDOPointer;
};
