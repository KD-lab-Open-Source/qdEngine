#include "stdafx.h"
#include "resource.h"

#include "qd_trigger_chain.h"
#include <iterator>

#include "triggerjoiner.h"

TriggerJoiner::ele_copier::ele_copier(TriggerJoiner::EleTable& ele_table, 
									  qdTriggerChain* ptrTo)
: m_EleTable(ele_table)
, m_ptrTo(ptrTo)
{
}

void TriggerJoiner::ele_copier::operator()(qdTriggerElementPtr pele)
{
	if (qdTriggerElementPtr const pnew = m_ptrTo->add_element(pele->object()))
	{
		layout_copy(pele, pnew);
		m_EleTable.insert(EleTable::value_type(pele, pnew));
	}
}

void TriggerJoiner::ele_copier::layout_copy(qdTriggerElementConstPtr from, 
											qdTriggerElementPtr to) const
{
	to->set_bounding_rect(from->bounding_rect());
	to->set_cell_number(from->cell_number().x, from->cell_number().y);
	to->update_object_reference();
}

TriggerJoiner::links_copier::links_copier(TriggerJoiner::EleTable& ele_table,
										qdTriggerChain* pto)
: m_EleTable(ele_table)
, m_ptrTo(pto)
{}

void TriggerJoiner::links_copier::operator()(value_type& vt)
{
	qdTriggerLinkList const& lst = vt.first->children();
	std::for_each(lst.begin(), lst.end(), link_copier(m_EleTable, m_ptrTo, vt.second));
}

TriggerJoiner::links_copier::link_copier::link_copier(EleTable& ele_tbl,
														   qdTriggerChain* pto,
														   qdTriggerElementPtr from)
: m_ptrTo(pto)
, m_EleTable(ele_tbl)
, m_ptrFrom(from)
{
}

void TriggerJoiner::links_copier::link_copier::layout_copy(qdTriggerLink const& from, 
													qdTriggerElementConstPtr to) const
{
	qdTriggerLink* const pl = m_ptrFrom->find_child_link(to);
	pl->set_child_offset(from.get_child_offset());
	pl->set_owner_offset(from.get_owner_offset());
}

void TriggerJoiner::links_copier::link_copier::operator()(qdTriggerLink const& lnk) const
{
	qdTriggerElementPtr const pele = m_EleTable[lnk.element()];
	if (m_ptrTo->add_link(m_ptrFrom, pele, lnk.type()))
		layout_copy(lnk, pele);
}

TriggerJoiner::TriggerJoiner(void)
{
}

TriggerJoiner::~TriggerJoiner(void)
{
}

/**
	Сначала копирует все элементы из from в to. 
	Затем копирует линки элементов.
	После чего все добавленные элементы кладутся в список lst_ele
 */
bool TriggerJoiner::join(qdTriggerChain const* from, qdTriggerChain* to, 
						 qdTriggerElementList& lst_ele)
{
	qdTriggerElementList const& lst = from->elements_list();
	std::for_each(lst.begin(), lst.end(), ele_copier(m_EleTable, to));
	std::for_each(m_EleTable.begin(), m_EleTable.end(), links_copier(m_EleTable, to));

	__if_exists(qdTriggerElementList::reserve)
	{
		lst_ele.reserve(m_EleTable.size());
	}

	EleTable::iterator beg = m_EleTable.begin(), end = m_EleTable.end();
	for(; beg != end; ++beg)
		lst_ele.push_back(beg->second);
	return true;
}