#include "stdafx.h"
#include "deletecolumns.h"
#include "qd_trigger_chain.h"
#include "qd_trigger_element.h"
#include "te_helpers.h"
#include "TEWnd.h"
#include <limits>
#include "del_ins_col_row_helpers.h"
#include <boost/iterator/filter_iterator.hpp>

DeleteColumns::DeleteColumns(size_t nColumnsCount, 
							 TEWnd* pwnd, 
							 qdTriggerChain* pchain, 
							 CPoint const& basePoint):
  m_nColumnsCount(nColumnsCount)
, m_pWnd(pwnd)
, m_pChain(pchain)
, m_basePoint(basePoint)
{
}

DeleteColumns::~DeleteColumns(void)
{
}

bool DeleteColumns::operator()()
{
	return run(m_nColumnsCount, m_pWnd, m_pChain, m_basePoint);
}

/**
	Берет элементы, лежащие в удаляемых столбцах, и проверяет можем ли мы их 
	сдвинуть. 
	Если можно, то все лежащие справа от удаляемых столбцов элементы сдвигаются.
	Пересчитывается bounding rect цепочки, а затем её рабочая область
 */
bool DeleteColumns::run(size_t nColumnsCount, 
						TEWnd* pwnd, 
						qdTriggerChain* pchain, 
						CPoint const& basePoint)
{
	int const w = pwnd->GetNetCellWidth() + pwnd->GetNetAmongCellW();
	int const h = pwnd->GetNetCellHeight() + pwnd->GetNetAmongCellH();
	int idx = te_h::get_cell_idx(basePoint.x, w);

	qdTriggerElementList const& lst = pchain->elements_list();
	qdTriggerElementList::const_iterator itr = lst.begin(), end = lst.end();
	typedef boost::filter_iterator<
		te_h::test_for_horz_offset&, 
		qdTriggerElementList::const_iterator> FilterIter;

	te_h::test_for_horz_offset predicate(idx, idx + 1);
	FilterIter first(predicate, lst.begin(), lst.end());
	FilterIter last(predicate, lst.end(), lst.end());

	//если не можем сдвинуть хотя бы один элемент, 
	//из тех, что находятся в удаляемом столбце
	//то операцию проводить не будем
	std::unary_negate<te_h::test_can_shift_element> shift_predicate(
		te_h::test_can_shift_element(CSize(nColumnsCount*-w, 0), pwnd));

	if (std::find_if(first, last, shift_predicate) != last
		||(predicate(pchain->root_element())//если нужно сдвигать корневой
		&&shift_predicate(pchain->root_element())))
		return false;

	//чтобы рассматривать все элементы находящиеся справа от удаляемого столбца
	predicate.setIdxMax(std::numeric_limits<int>::max());
	FilterIter first1(predicate, lst.begin(), lst.end());
	FilterIter last1(predicate, lst.end(), lst.end());

	te_h::offset_ele offseter(CSize(nColumnsCount*-w, 0), w, h);
	std::for_each(first1, last1, offseter);

	if (predicate(pchain->root_element()))
		offseter(pchain->root_element());

	pwnd->RemakeChainRect(pchain);
	pwnd->UpdateChainWorkArea(pchain);

	return true;
}