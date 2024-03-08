#include "stdafx.h"
#include "resource.h"

#include "ptree_consts.h"
#include ".\gamedisptreenode.h"
#include "Tree/LoadTreeAssist.h"
#include "ISceneViewFacade.h"

#include <Mmreg.h>

#include "qd_game_dispatcher.h"
#include "ds_snd_dispatcher.h"
#include "qd_trigger_chain.h"
#include "qd_game_object_mouse.h"
#include "qd_inventory.h"
#include "qd_counter.h"
#include "qd_font_info.h"
#include "qd_minigame.h"
#include "qd_video.h"
#include "qd_game_end.h"

#include <boost/rtl.hpp>

#include "GameSettingsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using load_tree_assist::InsertObject;

GameDispTreeNode::GameDispTreeNode(qdNamedObject* object)
: NamedObjectTreeNode(object)
{
}

GameDispTreeNode::~GameDispTreeNode(void)
{
}

bool GameDispTreeNode::load(ITreeNodeFactory* factory,
							TreeLogic& treeLogic, ITreeNode * parent, 
							qdNamedObjectBase const* owner,
							ITreeNode* insertAfter)
{
	if(sndDispatcher* p = sndDispatcher::get_dispatcher())
		//p -> disable();
		p -> stop_sounds();

	CString str;
	str.LoadString(IDS_GLOBAL_NODE);

	CTreeListItem* item = 
		insertToTree(treeLogic, str, 
					parent, II_GLOBAL_NODE,
					reinterpret_cast<DWORD_PTR>(getObject()), 
					TN_INSERT_SORT);
	if (!item)
		return false;

	loadInterface(factory, treeLogic);
	loadFonts(factory, treeLogic);
	loadVideo(factory, treeLogic);
	loadMinigames(factory, treeLogic);
	loadCounters(factory, treeLogic);
	loadInventory(factory, treeLogic);
	loadMouse(factory, treeLogic);
	loadTriggers(factory, treeLogic);
	loadGlobalGameObjects(factory, treeLogic);
	loadScenes(factory, treeLogic);

	treeLogic.expand(this, TreeLogic::EC_EXPAND);
	return true;
}

bool GameDispTreeNode::reload(ITreeNodeFactory* factory,
					TreeLogic& logic, 
					ITreeNode const* parent, 
					qdNamedObjectBase const* owner)
{
	ASSERT(getTreeListItem());
	reloadInterface(factory, logic);
	reloadFonts(factory, logic);
	reloadVideo(factory, logic);
	reloadMinigames(factory, logic);
	reloadCounters(factory, logic);
	reloadInventory(factory, logic);
	reloadMouse(factory, logic);
	reloadTriggers(factory, logic);
	reloadGlobalGameObjects(factory, logic);
	reloadScenes(factory, logic);
	return true;
}

bool GameDispTreeNode::loadScenes(ITreeNodeFactory* factory, TreeLogic& treeLogic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());

	using load_tree_assist::InsertObject;
	boost::rtl::rng::for_each(gameDisp->scene_list(), 
		InsertObject(factory, treeLogic, this, NULL, TN_INSERT_SORT));
	return true;

}

bool GameDispTreeNode::loadGlobalGameObjects(ITreeNodeFactory* factory, 
											TreeLogic& treeLogic) 
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());

	std::auto_ptr<ITreeNode> p = factory->createFolderNode(QD_NAMED_OBJECT_MOVING_OBJ);
	if (!p.get()||!p->load(factory, treeLogic, this, gameDisp, TN_INSERT_LAST))
		return false;
	ITreeNode* folder = p.release();
	boost::rtl::crange<qdGameObjectList const> r(gameDisp->global_object_list());
	for(; r; ++r)
	{
		std::auto_ptr<ITreeNode> p(factory->createGlobalGameObjectNode(*r));
		if (!p.get()||!p->load(factory, treeLogic, folder, gameDisp, TN_INSERT_SORT))
			return false;
		p.release();
	}
	return true;
}

bool GameDispTreeNode::loadTriggers(ITreeNodeFactory* factory, 
									TreeLogic& treeLogic) 
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	return load_tree_assist::loadSimpleCatalog( QD_NAMED_OBJECT_TRIGGER_CHAIN,
												factory, treeLogic, 
												this, 
												getObject(), 
												TN_INSERT_LAST, 
												gameDisp->trigger_chain_list());
}

bool GameDispTreeNode::loadMouse(ITreeNodeFactory* factory, TreeLogic& treeLogic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	std::auto_ptr<ITreeNode> p = factory->createGameObjectNode(gameDisp->mouse_object());
	if (!p.get() || !p->load(factory, treeLogic, this, NULL, TN_INSERT_LAST))
		return false;
	p.release();
	return true;
}

bool GameDispTreeNode::loadInventory(ITreeNodeFactory* factory, 
									 TreeLogic& treeLogic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	return load_tree_assist::loadSimpleCatalog(
		QD_NAMED_OBJECT_INVENTORY,
		factory, treeLogic, this, getObject(), TN_INSERT_LAST, 
		gameDisp->inventory_list());

}

bool GameDispTreeNode::loadCounters(ITreeNodeFactory* factory, 
									TreeLogic& treeLogic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	return load_tree_assist::loadSimpleCatalog(
		QD_NAMED_OBJECT_COUNTER,
		factory, treeLogic, this, getObject(), TN_INSERT_LAST, gameDisp->counter_list());
}

bool GameDispTreeNode::loadFonts(ITreeNodeFactory* factory, TreeLogic& treeLogic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	return load_tree_assist::loadSimpleCatalog(QD_NAMED_OBJECT_FONT_INFO,
												factory, treeLogic, 
												this, 
												getObject(), 
												TN_INSERT_LAST, 
												gameDisp->fonts_list());

}

bool GameDispTreeNode::loadVideo(ITreeNodeFactory* factory, TreeLogic& treeLogic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	return load_tree_assist::loadSimpleCatalog(QD_NAMED_OBJECT_VIDEO,
		factory, treeLogic, 
		this, 
		getObject(), 
		TN_INSERT_LAST, 
		gameDisp->video_list());
}

bool GameDispTreeNode::loadMinigames(ITreeNodeFactory* factory, 
									 TreeLogic& treeLogic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	return load_tree_assist::loadSimpleCatalog(QD_NAMED_OBJECT_MINIGAME,
		factory, treeLogic, 
		this, 
		getObject(), 
		TN_INSERT_LAST, 
		gameDisp->minigame_list());
}

bool GameDispTreeNode::loadInterface(ITreeNodeFactory* factory, 
									 TreeLogic& treeLogic)
{
	std::auto_ptr<ITreeNode> p = 
		factory->createInterfaceObjectNode(qdInterfaceDispatcher::get_dispatcher());
	if (!p.get()||!p->load(factory, treeLogic, this, NULL,TN_INSERT_LAST))
		return false;
	ITreeNode* interfaceNode = p.release();
	return loadGameEnds(factory, treeLogic, interfaceNode);
}

bool GameDispTreeNode::loadGameEnds(ITreeNodeFactory* factory, 
									TreeLogic& treeLogic,
									ITreeNode * parent)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	return load_tree_assist::loadSimpleCatalog(QD_NAMED_OBJECT_GAME_END,
		factory, treeLogic, 
		parent, 
		getObject(), 
		TN_INSERT_FIRST, 
		gameDisp->game_end_list());
}

//! Возвращает идентификатор ресурса меню для узла
DWORD GameDispTreeNode::getMenuResId() const{
	return IDR_GLOBAL_NODE;
}

bool GameDispTreeNode::onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState)
{
	if (!getObject())
		return false;
	if (newState&NS_FOCUSED) {
		logic.getSceneView()->SetViewState(VS_NONE);
		return true;
	}
	return false;
}

bool GameDispTreeNode::reloadScenes(ITreeNodeFactory* factory, TreeLogic& logic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());

	return load_tree_assist::reloadSimpleCatalog(factory, 
							logic, 
							this, 
							gameDisp,
							TN_INSERT_SORT, 
							boost::rtl::c_range(gameDisp->scene_list()));
}

bool GameDispTreeNode::reloadGlobalGameObjects(ITreeNodeFactory* factory, 
											   TreeLogic& logic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());

	ITreeNode* folder = logic.getSubfolder(this, QD_NAMED_OBJECT_MOVING_OBJ);
	ASSERT(folder);
	boost::rtl::crange<qdGameObjectList const> r(gameDisp->global_object_list());
	for(; r; ++r)
	{
		typedef boost::rtl::irange<TreeLogic::NodeIterator> Nodes;
		Nodes ir(logic.getObjectNodes(*r));
		if (ir)
			(*ir)->reload(factory, logic, this, gameDisp);
		else
		{
			std::auto_ptr<ITreeNode> p(factory->createGlobalGameObjectNode(*r));
			if (!p.get()
				||!p->load(factory, logic, folder, gameDisp, TN_INSERT_SORT))
				break;
			p.release();
		}
	}
	return true;
}

bool GameDispTreeNode::reloadTriggers(ITreeNodeFactory* factory, TreeLogic& logic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	ITreeNode* folder = logic.getSubfolder(this, QD_NAMED_OBJECT_TRIGGER_CHAIN);

	return load_tree_assist::reloadSimpleCatalog(factory, 
								logic, folder, 
								gameDisp,
								TN_INSERT_SORT, 
								boost::rtl::c_range(gameDisp->trigger_chain_list()));
}

bool GameDispTreeNode::reloadMouse(ITreeNodeFactory* factory, TreeLogic& logic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	ITreeNode* object = *logic.getObjectNodes(gameDisp->mouse_object());
	return object->reload(factory, logic, this, gameDisp);
}

bool GameDispTreeNode::reloadInventory(ITreeNodeFactory* factory, TreeLogic& logic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	ITreeNode* folder = logic.getSubfolder(this, QD_NAMED_OBJECT_INVENTORY);

	return load_tree_assist::reloadSimpleCatalog(factory, 
		logic, folder, 
		gameDisp,
		TN_INSERT_SORT, 
		boost::rtl::c_range(gameDisp->inventory_list()));
}

bool GameDispTreeNode::reloadCounters(ITreeNodeFactory* factory, TreeLogic& logic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	ITreeNode* folder = logic.getSubfolder(this, QD_NAMED_OBJECT_COUNTER);

	return load_tree_assist::reloadSimpleCatalog(factory, 
		logic, folder, 
		gameDisp,
		TN_INSERT_SORT, 
		boost::rtl::c_range(gameDisp->counter_list()));
}

bool GameDispTreeNode::reloadFonts(ITreeNodeFactory* factory, TreeLogic& logic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	ITreeNode* folder = logic.getSubfolder(this, QD_NAMED_OBJECT_FONT_INFO);

	return load_tree_assist::reloadSimpleCatalog(factory, 
		logic, folder, 
		gameDisp,
		TN_INSERT_SORT, 
		boost::rtl::c_range(gameDisp->fonts_list()));
}

bool GameDispTreeNode::reloadVideo(ITreeNodeFactory* factory, TreeLogic& logic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	ITreeNode* folder = logic.getSubfolder(this, QD_NAMED_OBJECT_VIDEO);

	return load_tree_assist::reloadSimpleCatalog(factory, 
		logic, folder, 
		gameDisp,
		TN_INSERT_SORT, 
		boost::rtl::c_range(gameDisp->video_list()));
}

bool GameDispTreeNode::reloadMinigames(ITreeNodeFactory* factory, TreeLogic& logic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	ITreeNode* folder = logic.getSubfolder(this, QD_NAMED_OBJECT_MINIGAME);

	return load_tree_assist::reloadSimpleCatalog(factory, 
		logic, folder, 
		gameDisp,
		TN_INSERT_SORT, 
		boost::rtl::c_range(gameDisp->minigame_list()));
}

bool GameDispTreeNode::reloadInterface(ITreeNodeFactory* factory, TreeLogic& logic)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	ITreeNode* object = *logic.getObjectNodes(qdInterfaceDispatcher::get_dispatcher());

	return object->reload(factory, logic, this, gameDisp);
}

bool GameDispTreeNode::reloadGameEnds(ITreeNodeFactory* factory, TreeLogic& logic,
									  ITreeNode * parent)
{
	qdGameDispatcher* gameDisp = static_cast<qdGameDispatcher*>(getObject());
	ITreeNode* folder = logic.getSubfolder(this, QD_NAMED_OBJECT_GAME_END);

	return load_tree_assist::reloadSimpleCatalog(factory, 
		logic, folder, 
		gameDisp,
		TN_INSERT_SORT, 
		boost::rtl::c_range(gameDisp->game_end_list()));
}

bool GameDispTreeNode::showProperties(TreeLogic& logic, int activePage)
{
	GameSettingsPage pg;
	CPropertyPage* pp[] = {&pg, 0};
	pg.set_data(logic.getGameDispatcher());

	return showPropDlg(pp, activePage, IDS_CAP_GAME_SETTINGS);
}

bool GameDispTreeNode::canRename(TreeLogic& logic){
	return false;
}
