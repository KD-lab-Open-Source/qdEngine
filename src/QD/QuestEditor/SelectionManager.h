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
	��������� ������� ��������� ��������� ������� ���������
 */
class SelectionManager  
{

public:
	typedef std::vector<qdTriggerElementPtr>::const_iterator const_iterator;

	SelectionManager();
	~SelectionManager();

	/// �������� �������.
	void select(qdTriggerElementPtr const& element);
	/// ������� �� ������ ���������
	void deselect(qdTriggerElementPtr const& element);
	/// ������� ������� �� ������, ����� ������ ����������
	void reselect(qdTriggerElementPtr const& element);

	/// ������� ��� ���������
	void deselectAll();
	//! ������� ��������� ������
	void clear();

	/// ���������� ��������� ���������
	qdTriggerElementPtr const getLast() const;

	/// �������� �� ������� ���������
	bool isSelected(qdTriggerElementPtr const& element) const;

	/// ������ ������
	const_iterator begin() const;
	/// ����� ������
	const_iterator end() const;
	/// ���������� ��������� ���������
	size_t getCount() const;
	/// ������ �� ������ ��������� ���������
	bool empty() const;
protected:
	/// ���� select==true, �� ��������� ������� � ������, ����� ������� �� ������
	/**
		���� ������� ��� � ������, �� �� ���� ������ �� ��������.
	 */
	void select(qdTriggerElementPtr const element, bool select);
//	qdTriggerElement& getTrigger(int index) const;
private:
	/// ������ ��������� ��������
	std::vector<qdTriggerElementPtr> selected_;
};

#endif //!defined(AFX_SELECTIONMANAGER_H__01685F73_42E8_4E8A_85BF_8DA6BAF6F0F4__INCLUDED_)
