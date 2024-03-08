#include "stdafx.h"
#include "insertrows.h"

#include "qd_trigger_chain.h"
#include "qd_trigger_element.h"
#include "te_helpers.h"
#include "TEWnd.h"

#include <limits>

#include "del_ins_col_row_helpers.h"
#include <boost/iterator/filter_iterator.hpp>

InsertRows::InsertRows(size_t nRowsCount, 
					   TEWnd* pwnd, 
					   qdTriggerChain* pchain, 
					   CPoint const& basePoint):
  m_nRowsCount(nRowsCount)
, m_pWnd(pwnd)
, m_pChain(pchain)
, m_basePoint(basePoint)
{
}

InsertRows::~InsertRows(void)
{
}

bool InsertRows::operator()()
{
	return run(m_nRowsCount, m_pWnd, m_pChain, m_basePoint);
}

/**
	Все элементы, лежащие снизу от места вставки строк, сдвигаются.
	Пересчитывается рект цепочки и её рабочая область
 */
bool InsertRows::run(size_t nRowsCount, 
						TEWnd* pwnd,
						qdTriggerChain* pchain,
						CPoint const& basePoint)
{
	int const w = pwnd->GetNetCellWidth() + pwnd->GetNetAmongCellW();
	int const h = pwnd->GetNetCellHeight() + pwnd->GetNetAmongCellH();
	int idx = te_h::get_cell_idx(basePoint.y, h);
	qdTriggerElementList const& lst = pchain->elements_list();

	typedef boost::filter_iterator<
		te_h::test_for_vert_offset&, 
		qdTriggerElementList::const_iterator> FilterIter;

	te_h::test_for_vert_offset predicate(idx);
	FilterIter first(predicate, lst.begin(), lst.end());
	FilterIter last(predicate, lst.end(), lst.end());
	te_h::offset_ele offseter(CSize(0, nRowsCount*h), w, h);
	std::for_each(first, last, offseter);

	if (predicate(pchain->root_element()))
		offseter(pchain->root_element());

	pwnd->RemakeChainRect(pchain);
	pwnd->UpdateChainWorkArea(pchain);
	return true;
}
