#include "stdafx.h"
#include "resource.h"
#include "CounterLoader.h"
#include "ptree_consts.h"
#include "ptree_loading.h"

CounterLoader::insert2tree::insert2tree(CTreeCtrl& tree, HTREEITEM hPar)
: m_tree(tree)
, m_hParent(hPar)
{}
void CounterLoader::insert2tree::operator()(qdCounter* const pobj)
{
	HTREEITEM hres = 
		ptree_loading::InsertObject(m_tree, m_hParent, pobj, II_COUNTER_NODE, TVI_LAST);
}

CounterLoader::CounterLoader(void)
{
}

CounterLoader::~CounterLoader(void)
{
}

bool CounterLoader::load(CTreeCtrl& tree, HTREEITEM hParent, qdCounterList const& lst, 
						 HTREEITEM hAfter)
{
	HTREEITEM hBrunchRoot = ptree_loading::CrtTypedNode(tree, hParent,
		IDS_COUNTERS_NODE_NAME, QD_NAMED_OBJECT_COUNTER, II_COUNTERS_NODE, hAfter);

	if (!hBrunchRoot)
		return false;


	if (loadList(tree, hBrunchRoot, lst))
	{
		tree.Expand(hBrunchRoot, TVE_EXPAND);
		return true;
	}
	return false;
}

bool CounterLoader::loadList(CTreeCtrl& tree, HTREEITEM hParent, qdCounterList const&lst)
{
	std::for_each(lst.begin(), lst.end(), insert2tree(tree, hParent));
	return true;
}