//#pragma once
#ifndef __COUNTER_LOADER_H__
#define __COUNTER_LOADER_H__

#include "qd_game_dispatcher.h"
#include "qd_counter.h"

/// Грузит в дерево счетчики. Подозреваю, что уже не используется
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
	/// \brief создает необходимый узел в дереве и загружает список объектов,
	/// вызвав loadList
	static bool load(CTreeCtrl& tree, HTREEITEM hParent, 
		qdCounterList const& lst, HTREEITEM hAfter);
	static bool loadList(CTreeCtrl& tree, HTREEITEM hParent, qdCounterList const&lst);
};

#endif//__COUNTER_LOADER_H__