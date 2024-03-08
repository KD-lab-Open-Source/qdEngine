//#pragma once
#ifndef __COUNTER_LOADER_H__
#define __COUNTER_LOADER_H__

#include "qd_game_dispatcher.h"
#include "qd_counter.h"

/// ������ � ������ ��������. ����������, ��� ��� �� ������������
class CounterLoader
{
	class insert2tree
	{
	public:
		insert2tree(CTreeCtrl& tree, HTREEITEM hPar);
		void operator()(qdCounter* const pobj);
	private:
		CTreeCtrl& m_tree;
		HTREEITEM m_hParent;
	};
protected:
	CounterLoader(void);
public:
	~CounterLoader(void);
	/// \brief ������� ����������� ���� � ������ � ��������� ������ ��������,
	/// ������ loadList
	static bool load(CTreeCtrl& tree, HTREEITEM hParent, 
		qdCounterList const& lst, HTREEITEM hAfter);
	static bool loadList(CTreeCtrl& tree, HTREEITEM hParent, qdCounterList const&lst);
};

#endif//__COUNTER_LOADER_H__