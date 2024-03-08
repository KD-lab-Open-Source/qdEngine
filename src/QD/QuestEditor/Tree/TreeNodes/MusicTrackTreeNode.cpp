#include "stdafx.h"
#include "resource.h"
#include "ptree_consts.h"
#include ".\musictracktreenode.h"

#include "MusicTrackPropPage.h"
#include "ISceneViewFacade.h"
#include "ITriggerView.h"
#include "qd_music_track.h"

#include "ptree_te_filters.h"

#include "name_op.h"
#include "Main/Utils/UpdateTriggerElementTitles.h"

#include "menu_helpers.h"

MusicTrackTreeNode::MusicTrackTreeNode(qdNamedObject* object)
: ConditionalObjectTreeNode(object)
{
}

MusicTrackTreeNode::~MusicTrackTreeNode(void)
{
}

bool MusicTrackTreeNode::load(ITreeNodeFactory* factory, 
							  TreeLogic& treeLogic, 
							  ITreeNode * parent, 
							  qdNamedObjectBase const* owner,
							  ITreeNode* insertAfter)
{
	CTreeListItem* const item = insertToTree(treeLogic, getObject()->name(),
											parent, II_MUSIC_TRACK_NODE, 
											reinterpret_cast<DWORD_PTR>(getObject()), 
											insertAfter);
	if (!item)
		return false;
	if (getObject()->is_in_triggers())
		setINTriggerFlag(treeLogic, TIC_SET_NOT_ACTIVE);

	return true;
}

//! Возвращает идентификатор ресурса меню для узла
DWORD MusicTrackTreeNode::getMenuResId() const{
	return IDR_MUSIC_TRACK_NODE;
}

void MusicTrackTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const{
	qdMusicTrack const* pmt = static_cast<qdMusicTrack const*>(getObject());
	menu_h::checkMenuItem(pm, IDC_MUSIC_TRACK_CYCLED, pmt->is_cycled());
}

bool MusicTrackTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<MusicTrackPropPage> ptrPage(new MusicTrackPropPage);
	ptrPage->set_data(static_cast<qdMusicTrack*>(getObject()));
	ptrPages[0] = ptrPage.get();
	return showPropDlg(ptrPages, iActive, IDS_PPCAP_MUSIC_TRACK_PROPERTIES);
}

bool MusicTrackTreeNode::onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState)
{
	if (!getObject())
		return false;
	if (newState&NS_FOCUSED)
	{
		logic.getSceneView()->SetViewState(VS_EDIT_SCENE);
		logic.getSceneView()->SetSubTools(SSTE_NONE);
		logic.getGameDispatcher()->play_music_track(
			static_cast<qdMusicTrack*>(getObject()));
		if (newState&NS_FOCUSED
			&&logic.getShowTriggerElement()
			&&getObject()->is_in_triggers())
		{
			logic.getTriggerView()->ShowTrigger(getObject());
			if (logic.getFilterScheme() == TreeLogic::FS_NONE)
			{
				logic.getTriggerView()->SelectByFilter(tef_by_obj(getObject()), 
					ITriggerView::SA_SELECT,
					ITriggerView::SM_EXCLUSIVE);
			}
		}

	}
	else if (oldState&NS_FOCUSED)
		logic.getGameDispatcher()->stop_music();

	return false;
}

bool MusicTrackTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!preDelete(logic, ask, statDepends))
		return false;
	if (deleteAsSubObject(logic))
	{
		ITreeNode* prevVisible = logic.getPrevVisibleNode(this);
		logic.deleteNode(this);
		logic.setFocusedNode(prevVisible);
		return true;
	}
	return false;
}

//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
// при его удаление(сотояния, когда удаляется анимированный объект. 
//Все объекты, когда удаляется сцена)
bool MusicTrackTreeNode::deleteAsSubObject(TreeLogic& logic)
{
	qdGameScene* scene = static_cast<qdGameScene*>(getObject()->owner());
	if (scene->remove_music_track(static_cast<qdMusicTrack*>(getObject())))
	{
		delete getObject();
//		setObject(NULL);
		return true;
	}
	return false;
}

bool MusicTrackTreeNode::canRename(TreeLogic& logic)
{
	return true;
}
bool MusicTrackTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;
	qdGameScene* scene = static_cast<qdGameScene*>(getObject()->owner());
	if (scene->rename_music_track(static_cast<qdMusicTrack*>(getObject()), newName))
	{
		UpdateTriggerElementTitles::update(
			logic.getGameDispatcher()->trigger_chain_list(), getObject());
		return true;
	}
	return false;
}

bool MusicTrackTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName)
{
	qdGameScene* scene = static_cast<qdGameScene*>(getObject()->owner());
	return uniq_name(newName, scene->music_track_list());
}
