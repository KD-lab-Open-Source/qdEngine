#include "stdafx.h"
#include "resource.h"
#include "qd_game_end.h"
#include "gameendloader.h"
#include "ptree_loading.h"
#include "ptree_consts.h"

GameEndLoader::insert2tree::insert2tree(CTreeCtrl& tree, HTREEITEM hPar)
: m_tree(tree)
, m_hParent(hPar)
{}
void GameEndLoader::insert2tree::operator()(qdGameEnd* const pobj)
{
	HTREEITEM hres = 
		ptree_loading::InsertObject(m_tree, m_hParent, pobj, II_GAME_END_NODE, TVI_LAST);
	if (hres)
		SetInTrigger(m_tree, hres, pobj->is_in_triggers(), II_NOT_ACTIVE_TRIGGER);
}
GameEndLoader::GameEndLoader()
{
}

GameEndLoader::~GameEndLoader()
{
}

bool GameEndLoader::load(CTreeCtrl& tree, HTREEITEM hParent, qdGameEndList const& lst, 
						 HTREEITEM hAfter)
{
	HTREEITEM hBrunchRoot = ptree_loading::CrtTypedNode(tree, hParent,
		IDS_GAME_ENDS_NODE_NAME, QD_NAMED_OBJECT_GAME_END, II_GAME_ENDS_NODE, hAfter);

	if (!hBrunchRoot)
		return false;

	
	if (loadList(tree, hBrunchRoot, lst))
	{
		tree.Expand(hBrunchRoot, TVE_EXPAND);
		return true;
	}
	return false;
}

bool GameEndLoader::loadList(CTreeCtrl& tree, HTREEITEM hParent, 
							 qdGameEndList const& lst)
{
	std::for_each(lst.begin(), lst.end(), insert2tree(tree, hParent));
	return true;
}
