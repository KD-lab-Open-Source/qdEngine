#include "stdafx.h"
#include "resource.h"
#include ".\gamescenetreenode.h"
#include "Tree/TreeLogic.h"
#include "Tree/LoadTreeAssist.h"
#include "Main/Utils/UpdateTriggerElementTitles.h"

#include "ptree_consts.h"
#include "menu_helpers.h"
#include "name_op.h"
#include "Main/ObjectTraits.h"

#include "ScreenProp.h"

#include "qd_grid_zone.h"
#include "qd_music_track.h"
#include "qd_game_object.h"
#include "qd_game_scene.h"

#include "ISceneViewFacade.h"
#include "ITriggerView.h"
#include "ptree_te_filters.h"

#include "qd_game_object_animated.h"

#include <boost/rtl.hpp>

namespace
{
/// Перезагружает объект в дерево
bool reloadObject(ITreeNodeFactory* factory, 
				TreeLogic& logic, 
				ITreeNode* folder,
				qdNamedObject* object,
				qdNamedObject* owner)
{
	typedef boost::rtl::irange<TreeLogic::NodeIterator> Nodes;
	Nodes ir(logic.getObjectNodes(object));
	if (ir)
		(*ir)->reload(factory, logic, folder, owner);
	else
	{
		std::auto_ptr<ITreeNode> node = factory->createGameObjectNode(object);
		if (node.get()&&node->load(factory, 
						logic, 
						folder,
						owner))
		{
			node.release();
		}
		else
			return false;
	}
	return true;
}
}
using load_tree_assist::InsertGameObject;
using load_tree_assist::InsertObject;
GameSceneTreeNode::GameSceneTreeNode(qdNamedObject* object):
  ConditionalObjectTreeNode(object)
{
}

GameSceneTreeNode::~GameSceneTreeNode(void)
{
}

bool GameSceneTreeNode::load(ITreeNodeFactory* factory, 
							 TreeLogic& treeLogic, 
							 ITreeNode * parent, 
							 qdNamedObjectBase const* owner,
							 ITreeNode* insertAfter)
{
	QETreeListCtrl& tree = treeLogic.getTreeListCtrl();

	qdGameScene* pgs = static_cast<qdGameScene*>(getObject());

	TreeItemsIndex& sceneIndex = treeLogic.getSceneItemsIndex();
	CTreeListItem* insAfter = TLI_LAST;
	TreeItemsIndex::iterator itr = 
		sceneIndex.insert(TreeItemsIndex::value_type(pgs->name(), NULL)).first;

	if (itr != sceneIndex.begin()) {
		insAfter = boost::prior(itr)->second;
	}
	else
	{
		TreeItemsIndex::iterator next(boost::next(itr));
		if (next != sceneIndex.end()) {
			insAfter = tree.GetPrevSiblingItem(next->second);
			if (!insAfter)
				insAfter = TLI_FIRST;
		}
	}

	ASSERT (itr != sceneIndex.end());

	CTreeListItem* item = 
		insertToTree(treeLogic, pgs->name(), parent, II_SCENE_NODE, 
					reinterpret_cast<DWORD_PTR>(getObject()), itemToNode(insAfter));

	treeLogic.expand(this, TreeLogic::EC_EXPAND);

	itr->second = item;

	loadContent(factory, treeLogic);

	if (pgs->is_in_triggers())
		setINTriggerFlag(treeLogic, TIC_SET_NOT_ACTIVE);

	return true;
}

void GameSceneTreeNode::loadContent(ITreeNodeFactory* factory, 
									TreeLogic& treeLogic) 
{
	QETreeListCtrl& tree = treeLogic.getTreeListCtrl();

	if (!loadMusicTracks(factory, treeLogic))
		return;
	
	if (!loadZones(factory, treeLogic))
		return ;

	if (!loadGameObjects(factory, treeLogic))
		return;

}

bool GameSceneTreeNode::loadZones(ITreeNodeFactory* factory, 
									TreeLogic& treeLogic) 
{

	qdGameScene* scene = static_cast<qdGameScene*>(getObject());
	return load_tree_assist::loadSimpleCatalog(QD_NAMED_OBJECT_GRID_ZONE,
												factory, treeLogic, 
												this, 
												getObject(), TN_INSERT_LAST, 
												scene->grid_zone_list());
}

bool GameSceneTreeNode::loadMusicTracks(ITreeNodeFactory* factory, 
										TreeLogic& treeLogic) 
{
	qdGameScene* scene = static_cast<qdGameScene*>(getObject());
	return load_tree_assist::loadSimpleCatalog(QD_NAMED_OBJECT_MUSIC_TRACK,
		factory, treeLogic, 
		this, 
		getObject(), TN_INSERT_LAST, scene->music_track_list());
}

bool GameSceneTreeNode::loadGameObjects(ITreeNodeFactory* factory, 
										TreeLogic& treeLogic) 
{
	std::auto_ptr<ITreeNode> p(factory->createFolderNode(QD_NAMED_OBJECT_STATIC_OBJ));
	if (!p.get()||!p->load(factory, treeLogic, this, NULL))
		return false;
	ITreeNode* staticObjsNode = p.release();

	p = factory->createFolderNode(QD_NAMED_OBJECT_ANIMATED_OBJ);
	if (!p.get()||!p->load(factory, treeLogic, this, NULL))
		return false;
	ITreeNode* animObjsNode = p.release();

	p = factory->createFolderNode(QD_NAMED_OBJECT_MOVING_OBJ);
	if (!p.get()||!p->load(factory, treeLogic, this, NULL))
		return false;
	ITreeNode* movingObjsNode = p.release();

	qdGameScene const* scene = static_cast<qdGameScene const*>(getObject());
	boost::rtl::rng::for_each(scene->object_list(), 
		InsertGameObject(factory, treeLogic, 
						staticObjsNode,
						animObjsNode,
						movingObjsNode,
						NULL,
						TN_INSERT_SORT));

	return true;
}

	//! Возвращает идентификатор ресурса меню для узла
DWORD GameSceneTreeNode::getMenuResId() const{
	return IDR_SCENE_NODE;
}

void GameSceneTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
	if(!treeLogic.isSceneViewActive())
	{
		CMenu* psub = pm->GetSubMenu(0);
		if (psub){
			menu_h::disableMenuItem(psub, IDC_CRT_STATIC_OBJ);
			menu_h::disableMenuItem(psub, IDC_CRT_ANIMATED_OBJ);
			menu_h::disableMenuItem(psub, IDC_CRT_MOVING_OBJ);
		}
		menu_h::disableMenuItem(pm, IDC_SOUND_LIST);
	}

	if (static_cast<const qdGameScene*>(getObject())
		!=treeLogic.getGameDispatcher()->get_active_scene())
		menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES_FOR_ALL_OBJS);
}

bool GameSceneTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<ScreenProp> ptrSP (new ScreenProp);
	ptrPages[0] = ptrSP.get();
	ptrSP->set_data(logic.getGameDispatcher(), 
		static_cast<qdGameScene*>(getObject()));
	
	return showPropDlg(ptrPages, 0, IDS_PPCAP_SCENE_PROPERTIES);
}

bool GameSceneTreeNode::onSelectInTriggerView(TreeLogic& logic, bool bSelect)
{
	//это такая штука, чтобы темплайт инстанциировать правильно
	typedef bool (*f)(const qdNamedObject* po);

	if (bSelect) 
	{
		ITriggerView* triggerView = logic.getTriggerView();
		qdGameScene * const scene = static_cast<qdGameScene*>(getObject());
		switch(logic.getFilterScheme()) {
		case TreeLogic::FS_NONE:
			triggerView->SelectByFilter(tef_by_scene(scene), 
				ITriggerView::SA_SELECT,
				ITriggerView::SM_EXCLUSIVE);
			break;
		case TreeLogic::FS_ENTRY_FOR_DIALOGS:
			triggerView->SelectByFilter(tef_dlg_state(scene), 
				ITriggerView::SA_SELECT, 
				ITriggerView::SM_EXCLUSIVE);
			break;
		case TreeLogic::FS_ENTRY_FOR_DYN_OBJ:
			triggerView->SelectByFilter(tef_by_scenes_objs(scene,
				ObjectTraits::is_obj_animated), 
				ITriggerView::SA_SELECT, 
				ITriggerView::SM_EXCLUSIVE);
			break;
		case TreeLogic::FS_ENTRY_FOR_MOUSE:
			break;
		case TreeLogic::FS_ENTRY_FOR_PERS:
			triggerView->SelectByFilter(tef_by_scenes_objs(scene,
				ObjectTraits::is_obj_moving), 
				ITriggerView::SA_SELECT, 
				ITriggerView::SM_EXCLUSIVE);
			break;
		}
		if (logic.getShowTriggerElement()&&scene->is_in_triggers())
			triggerView->ShowTrigger(scene);
		else
			triggerView->ShowLTOfSelected(tef_by_scene(scene));

	}
	return true;
}

/*!
	На получение:
 		Переводит редактор сцены в состояния редактирования игровой области
		(VS_EDIT_GAME_AREA)
		Выравнивает размеры битмапа в редакторе сцены
	\param logic	- TreeLogic
	\param bSelect	- \a true - получил выделение.\a false - потерял
	\retval true	- если надо перерисовать активный редактор
	\retval false	- если не надо перерисовать активный редактор
*/
bool GameSceneTreeNode::onSelectInSceneView(TreeLogic& logic, bool bSelect)
{
	if(bSelect)
	{
		logic.getSceneView()->SetViewState(VS_EDIT_GAME_AREA);
		logic.getSceneView()->AdjustBitmapSize();
		return true;
	}
	return false;
}


bool GameSceneTreeNode::onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState)
{
	if (!getObject())
		return false;
	bool const bSelect = newState&NS_FOCUSED;
	if (logic.isSceneViewActive())
		return onSelectInSceneView(logic, bSelect);
	else
		return onSelectInTriggerView(logic, bSelect);
}

//! Добавляет недостающие части
bool GameSceneTreeNode::reload(ITreeNodeFactory* factory, 
								TreeLogic& logic,
								ITreeNode const* parent, 
								qdNamedObjectBase const* owner)
{
	if (!getTreeListItem())
	{
		ASSERT(parent);
		ASSERT(owner);
		return load(factory, logic, const_cast<ITreeNode*>(parent), owner, TN_INSERT_SORT);
	}
	return reloadContent(factory, logic);
}

bool GameSceneTreeNode::reloadContent(ITreeNodeFactory* factory, TreeLogic& treeLogic) 
{
	QETreeListCtrl& tree = treeLogic.getTreeListCtrl();

	if (!reloadMusicTracks(factory, treeLogic))
		return false;

	if (!reloadZones(factory, treeLogic))
		return false;

	if (!reloadGameObjects(factory, treeLogic))
		return false;
	return true;
}

bool GameSceneTreeNode::reloadMusicTracks(ITreeNodeFactory* factory, 
										  TreeLogic& logic) 
{
	ITreeNode* folder = logic.getSubfolder(this, QD_NAMED_OBJECT_MUSIC_TRACK);
//	if (!folder)
//		return loadMusicTracks(factory, logic);

	qdGameScene const& scene = static_cast<qdGameScene&>(*getObject());
	reloadSimpleCatalog(factory, 
						logic, 
						folder, 
						TN_INSERT_SORT,
						boost::rtl::c_range(scene.music_track_list()));
	return true;
}

bool GameSceneTreeNode::reloadZones(ITreeNodeFactory* factory, 
										  TreeLogic& logic) 
{
	ITreeNode* folder = logic.getSubfolder(this, QD_NAMED_OBJECT_GRID_ZONE);
//	if (!folder)
//		return loadZones(factory, logic);

	qdGameScene const& scene = static_cast<qdGameScene&>(*getObject());
	reloadSimpleCatalog(factory, 
						logic, 
						folder, 
						TN_INSERT_SORT,
						boost::rtl::c_range(scene.grid_zone_list()));
	return true;
}


template<typename Range>
bool GameSceneTreeNode::reloadSimpleCatalog(ITreeNodeFactory* factory, 
											TreeLogic& logic,
											ITreeNode * folder,
											ITreeNode* insertAfter,
											Range r)
{
	return load_tree_assist::reloadSimpleCatalog(factory, logic, 
		folder, getObject(),insertAfter, r);
}

bool GameSceneTreeNode::reloadGameObjects(ITreeNodeFactory* factory, TreeLogic& logic)
{
	ITreeNode* statics = logic.getSubfolder(this, QD_NAMED_OBJECT_STATIC_OBJ);
	ITreeNode* anims = logic.getSubfolder(this, QD_NAMED_OBJECT_ANIMATED_OBJ);
	ITreeNode* pers = logic.getSubfolder(this, QD_NAMED_OBJECT_MOVING_OBJ);
	ASSERT(statics&&anims&&pers);
	qdGameScene const& scene = static_cast<qdGameScene&>(*getObject());
	boost::rtl::crange<qdGameObjectList const> r(scene.object_list());

	for(; r; ++r)
	{
		ITreeNode* folder = NULL;
		switch((*r)->named_object_type()) {
		case QD_NAMED_OBJECT_STATIC_OBJ:
			folder = statics;
			break;
		case QD_NAMED_OBJECT_ANIMATED_OBJ:
			folder = anims;
			break;
		case QD_NAMED_OBJECT_MOVING_OBJ:
			folder = pers;
			break;
		}
		reloadObject(factory, logic, folder, *r, getObject());
	}
	return true;
}

bool GameSceneTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!preDelete(logic, ask, statDepends))
		return false;
	qdGameScene* pscene = static_cast<qdGameScene*>(getObject());
	pscene->set_active_personage(NULL);
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	if (gameDisp->get_active_scene() == pscene)
	{
		gameDisp->select_scene(static_cast<qdGameScene*>(NULL));
		logic.getSceneView()->reset();
	}
	deleteZones(logic);
	deleteObjects(logic);
	deleteMusicTracks(logic);
	if (gameDisp->remove_scene(static_cast<qdGameScene*>(getObject())))
	{
		ITreeNode* prevVisible = logic.getPrevVisibleNode(this);
		delete getObject();
//		setObject(NULL);
		logic.deleteNode(this);
		logic.setFocusedNode(prevVisible);
		return true;
	}
	return false;
}

void GameSceneTreeNode::deleteZones(TreeLogic& logic)
{
	qdGameScene* scene = static_cast<qdGameScene*>(getObject());

	qdGridZoneList const& lst = scene->grid_zone_list();
	for (; !lst.empty(); )
		deleteSubObject(logic, lst.front());
}

void GameSceneTreeNode::deleteObjects(TreeLogic& logic)
{
	qdGameScene* scene = static_cast<qdGameScene*>(getObject());

	qdGameObjectList const& lst = scene->object_list();
	for (; !lst.empty(); )
		deleteSubObject(logic, lst.front());
}

void GameSceneTreeNode::deleteMusicTracks(TreeLogic& logic)
{
	qdGameScene* scene = static_cast<qdGameScene*>(getObject());

	qdMusicTrackList const& lst = scene->music_track_list();
	for (; !lst.empty(); )
		deleteSubObject(logic, lst.front());
}

void GameSceneTreeNode::deleteSubObject(TreeLogic& logic, qdNamedObject const* object)
{
	TreeLogic::Nodes nodes = logic.getObjectNodes(object);
	if (!nodes)
		return;
	ITreeNode* node = *nodes;
	node->deleteAsSubObject(logic);
}

bool GameSceneTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	bool res = gameDisp->rename_scene(static_cast<qdGameScene*>(getObject()), newName);
	if (res)
	{
		UpdateTriggerElementTitles::update(gameDisp->trigger_chain_list(), getObject());
		updateTriggerElementTitlesForSceneObjects(gameDisp);
		return true;
	}
	return false;
}

bool GameSceneTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName)
{
	return uniq_name(newName, logic.getGameDispatcher()->scene_list());
}

void GameSceneTreeNode::updateTriggerElementTitlesForSceneObjects(
	qdGameDispatcher* gameDisp)
{
	qdGameScene* scene = static_cast<qdGameScene*>(getObject());
	boost::rtl::filtered_range<
		qdGameObjectList const,
		bool (*)(qdNamedObject const*)
	> r(scene->object_list(), ObjectTraits::is_animated);
	for(; r; ++r)
		UpdateTriggerElementTitles::updateForAnimated(
			gameDisp->trigger_chain_list(), static_cast<qdGameObjectAnimated*>(*r));


	boost::rtl::crange<qdGridZoneList const> zones(scene->grid_zone_list());
	for(; zones; ++zones)
		UpdateTriggerElementTitles::updateForZone(gameDisp->trigger_chain_list(), *zones);

	boost::rtl::crange<qdMusicTrackList const> mus(scene->music_track_list());
	for(; mus; ++mus)
		UpdateTriggerElementTitles::update(gameDisp->trigger_chain_list(), *mus);
}
