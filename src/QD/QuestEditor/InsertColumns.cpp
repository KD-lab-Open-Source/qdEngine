#include "stdafx.h"
#include "insertcolumns.h"
#include "TEWnd.h"
#include "qd_trigger_chain.h"
#include "te_helpers.h"
#include <limits>
#include "del_ins_col_row_helpers.h"
#include <boost/iterator/filter_iterator.hpp>

InsertColumns::InsertColumns(size_t iColumnsCount, 
							 TEWnd* pwnd,
							 qdTriggerChain* pchain,
							 CPoint const& basePoint):
  m_iColumnsCount(iColumnsCount)
, m_pWnd(pwnd)
, m_pChain(pchain)
, m_basePoint(basePoint)
{
}

InsertColumns::~InsertColumns(void)
{
}

bool InsertColumns::operator()(){
	return run(m_iColumnsCount, m_pWnd, m_pChain, m_basePoint);
}

/**
	Все элементы лежащие справа от места вставки столбцов сдвигаются.
	Пересчитывается рект цепочки и её рабочая область
 */
bool InsertColumns::run(size_t nColumnsCount, 
						TEWnd* pwnd,
						qdTriggerChain* pchain,
						CPoint const& basePoint)
{
	int const w = pwnd->GetNetCellWidth() + pwnd->GetNetAmongCellW();
	int const h = pwnd->GetNetCellHeight() + pwnd->GetNetAmongCellH();
	int idx = te_h::get_cell_idx(basePoint.x, w);
	qdTriggerElementList const& lst = pchain->elements_list();

	typedef boost::filter_iterator<
		te_h::test_for_horz_offset&, 
		qdTriggerElementList::const_iterator> FilterIter;

	te_h::test_for_horz_offset predicate(idx);
	FilterIter first(predicate, lst.begin(), lst.end());
	FilterIter last(predicate, lst.end(), lst.end());
	te_h::offset_ele offseter(CSize(nColumnsCount*w, 0), w, h);
	std::for_each(first, last, offseter);

	if (predicate(pchain->root_element()))
		offseter(pchain->root_element());

	pwnd->RemakeChainRect(pchain);
	pwnd->UpdateChainWorkArea(pchain);
	return true;
}
