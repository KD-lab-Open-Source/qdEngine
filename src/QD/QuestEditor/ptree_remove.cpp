/********************************************************************
	created:	2002/09/11
	created:	11:9:2002   11:57
	filename: 	D:\Projects\Quest\QuestEditor\ptree_remove.cpp
	file path:	D:\Projects\Quest\QuestEditor
	file base:	ptree_remove
	file ext:	cpp
	Powerd by:	�����
	
	purpose:	�������� �� ������ � �������, � ��� �� ���������������
				�������
*********************************************************************/
#include "stdafx.h"
#include "questeditor.h"

#include "qd_music_track.h"
#include "qd_game_object_static.h"
#include "qd_game_object_animated.h"
#include "qd_game_object_moving.h"
#include "qd_game_object_mouse.h"
#include "qd_grid_zone.h"
#include "qd_minigame.h"
#include "qd_video.h"

#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"
#include "qd_interface_button.h"
#include "qd_interface_background.h"
#include "qd_interface_slider.h"
#include "qd_interface_save.h"
#include "qd_game_end.h"

#include "qd_counter.h"
#include "qd_inventory.h"
#include "qd_trigger_chain.h"
#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"

#include "snd_dispatcher.h"

#include "SceneTreeView.h"
#include "mainfrm.h"
#include "childview.h"

#include "ptree_hlprs.hpp"
#include "ptree_loading.h"
#include "te_helpers.h"
#include "ITriggerView.h"

#include "Stat Dependencies/StatObjectDependencies.h"
#include "Stat Dependencies/filterobjectdependencies.h"

#include "CursorIdUpdater.h"

#include <boost/rtl.hpp>
#include <utility>
#include "sv_api.h"

/************************************************************************/
/* ��������������� ���������� �����                                     */
/************************************************************************/
//! ���� <������� ���������, ��������� �� �������>
typedef	std::pair<qdTriggerChain*, qdTriggerElementPtr> TC_TE_PAIR_t;
typedef std::vector<TC_TE_PAIR_t> TC_TE_PAIR_VECT_t;
/************************************************************************/
/* ��������������� ���������� �������                                   */
/************************************************************************/
bool CanRemoveState(qdGameDispatcher* pdisp,
								   qdGameObjectState* ptrState,
								   TC_TE_PAIR_VECT_t* pl,
								   NamedObjectList* namedObjectList);
/************************************************************************/
/* ���������� �������                                                   */
/************************************************************************/

/*!
	��������� �� ���� ��������� � �������� ��, ����� ���������� �� ��������� ������ 
	�� ���������
*/
void ReinitCounters(qdGameDispatcher const* gameDisp)
{
	qdCounterList const& lst = gameDisp->counter_list();
	std::for_each(lst.begin(), lst.end(), std::mem_fun(qdCounter::init));
}

//! � ������� ���� ������ ���� ���������?
bool HasLastState(qdNamedObject* pno)
{
	ASSERT(ptree_helpers::is_obj_animated(pno)
		||ptree_helpers::is_obj_moving(pno)
		||ptree_helpers::is_obj_mouse(pno));
	
	qdGameObjectAnimated* ptrObj = static_cast<qdGameObjectAnimated*>(pno);
	return (ptrObj->state_vector().size() == 1);
}

//! � ������� ���� ������ ���� ������ ���������?
bool HasLastEmptyState(qdNamedObject* pno)
{
	if (HasLastState(pno)) {
		qdGameObjectState* ptrState = 
			static_cast<qdGameObjectAnimated*>(pno)->state_vector().front();
		return ptrState->is_state_empty();		
	}
	return false;
}

//! � ������ �������� ���������?
BOOL HasNoStates(qdGameObject* pno)
{
	ASSERT(ptree_helpers::is_obj_animated(pno)
		||ptree_helpers::is_obj_moving(pno));
	
	return (static_cast<qdGameObjectAnimated*>(pno)->state_vector().empty());
}

//////////////////////////////////////////////////////////////////////////

/*!
 	���������� �� �������� ��������� � �������� ����� � ��������� ������� ��������,
	������� �������� ��������� \a ptrState, � ���������� �� � ������. ���� ������ ����,
	�� ��������� �������. ���� �� ���� � \a bUserAsk ����������� � ������������: �����
	�� �������. 
 	\param bUserAsk		- ���������� �� �������������
	\param pdisp		- ��������� �� ��������� �������
	\param ptrState		- ���������, ������� �������� �������
	\param pl			- ��������� �� ������-������, ���� ����� ������� ��� �������,
							���������� \a ptrState
*/
bool CanRemoveState(bool bUserAsk, qdGameDispatcher* pdisp,
								   qdGameObjectState* ptrState,
								   TC_TE_PAIR_VECT_t* pl,
								   NamedObjectList* namedObjectList)
{
	const qdTriggerChainList& gcl = pdisp->trigger_chain_list();
	qdTriggerChainList::const_iterator _itr = gcl.begin(), _end = gcl.end();

	CString strUsers;
	for(;_itr != _end; ++_itr)
	{
		qdTriggerChain* pchain = (*_itr);
		const qdTriggerElementList& tel = pchain->elements_list();
		qdTriggerElementPtr const ptrEle = pchain->search_element(ptrState);
		if (ptrEle)
		{
			if(!strUsers.IsEmpty())
				strUsers += _T(",\n");
			strUsers += pchain->name();

			pl->push_back(std::make_pair(pchain, ptrEle));
		}
	}

	if (!bUserAsk) //���� ��� �� ���� ���������� ������������,
					//�� ���������� �������� �� �����
		return true;
	
	if(strUsers.IsEmpty())
	{
		if(HasLastEmptyState(ptrState->owner()))
		{
			CString strMsg;
			strMsg.Format(IDS_LAST_STATE_IN_OBJ, ptrState->owner()->name());
			return (AfxMessageBox(strMsg, MB_ICONWARNING|MB_OKCANCEL) == IDOK);
		}
		return true;
	}

	CString strMsg;
	strMsg.Format(IDS_STATE_USED, static_cast<LPCTSTR>(strUsers));
	if(AfxMessageBox(strMsg, MB_ICONWARNING|MB_OKCANCEL) == IDOK)
	{
		if(HasLastEmptyState(ptrState->owner()))
		{
			strMsg.Format(IDS_LAST_STATE_IN_OBJ, ptrState->owner()->name());
			return (AfxMessageBox(strMsg, MB_ICONWARNING|MB_OKCANCEL) == IDOK);
		}
		return true;
	}
	return false;
}

/*!
 	������� ������� �� ������� ���������
*/
void RemoveTgrEle(TC_TE_PAIR_t& pr)
{
	te_h::RemoveLinks(pr.first, pr.second);
	pr.first->remove_element(pr.second, true);
}

/*!
 	��������� �� ������ � ��� ������� �������� �������� RemoveTgrEle()
*/
void RemoveTriggerChainEles(TC_TE_PAIR_VECT_t& lst4remove){
	std::for_each(lst4remove.begin(), lst4remove.end(), RemoveTgrEle);
}

/*!
 	��������� �� ���� ��������. ���� ������� �������, ������� �������� \a pno, �������
	���
*/
void RemoveObjectFromTriggers(qdNamedObject const* pno, qdTriggerChainList const& lst)
{
	qdTriggerChainList::const_iterator itr = lst.begin(), end = lst.end();
	for(; itr != end; ++itr)
	{
		qdTriggerChain* const pchain = *itr;
		while (qdTriggerElementPtr ptrEle = pchain->search_element(pno)) {
			te_h::RemoveLinks(pchain, ptrEle);
			pchain->remove_element(ptrEle);
		}
	}
}

/*!
	������� ��������� ���� �� ���������
*/
void RemoveGridZoneFromTriggers(qdGridZone *zone, qdTriggerChainList const& lst)
{
	qdGridZoneState const* state = zone->get_state(qdGridZoneState::ZONE_STATE_OFF_NAME);
	if (state->is_in_triggers())
		RemoveObjectFromTriggers(state, lst);
	state = zone->get_state(qdGridZoneState::ZONE_STATE_ON_NAME);
	if (state->is_in_triggers())
		RemoveObjectFromTriggers(state, lst);
}
/*!
 	����, ��� ������������ ���������(SceneTreeView::CanRemoveInventory()). � 
	��������, ��� ������������, ������������. ��������� �� ������ ���������
	(qdGameDispatcher::remove_inventory())
	\param pInv			- ��������� ���������
	\param bAsk			- ������������� ��� ��� ������������
	\return	\a true - ���� �������.
	\remarks ������������ �������� ������� �� ����������
*/
bool SceneTreeView::RemoveInventory(qdInventory* pInv, bool bAsk)
{
	qdGameObjectList lst_obj;
	if (CanRemoveInventory(bAsk, pInv,&lst_obj)){
		qdGameObjectList::iterator i = lst_obj.begin(), e = lst_obj.end();
		for(; i != e; ++i)
			static_cast<qdGameObjectAnimated*>(*i)->set_inventory_name(NULL);
        return m_ptrGameDisp->remove_inventory(pInv);
	}
	return false;
}

/*!
 	��������� �� ���� �������� �� ���� ������ � ��������� ������������ �� � ���
	���������. ���� ��, �� ������ �������� � ������. ���� ������ � ����� ������
	�� ���� � bAsk = true, �� ������������� ������������, ��� ��������� ������������.
	\param bAsk			- ������������� ��� ��� ������������
	\param pInv			- ��������� ���������
	\param plist4Remove	- ������ ��������, ��� ��������� ������������
	\return \a true - ���� ����� �������
*/
bool SceneTreeView::CanRemoveInventory(bool bAsk, qdInventory* pInv, 
									   qdGameObjectList* plist4Remove)
{
	const qdGameSceneList& l = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator i = l.begin(), e = l.end();
	for(; i != e; ++i){
		qdGameScene* const ptrScene = *i;
		qdGameObjectList const& ol = ptrScene->object_list();
		qdGameObjectList::const_iterator itr = ol.begin(), end = ol.end();
		for(; itr != end; ++itr){
			qdGameObject* const po = *itr;
			if (po->named_object_type() != QD_NAMED_OBJECT_STATIC_OBJ){
				qdGameObjectAnimated* const pao = static_cast<qdGameObjectAnimated*>(po);
				if (pao->has_inventory_name()
					&&!_tcscmp(pInv->name(), pao->inventory_name()))
					plist4Remove->push_back(pao);
			}
		}
	}
	if (plist4Remove->empty())
		return true;
	CString msg;
	msg.Format(IDS_DEL_INVENTORY_USED_REMOVE, pInv->name());
	if (!bAsk||MessageBox(msg, CString((LPTSTR)IDS_DEL_CAP), MB_OKCANCEL) == IDOK) 
		return true;
	return false;
}

template<class ObjectContainer>
void SceneTreeView::cleanupObjects(ObjectContainer& container)
{
	while (container.begin() != container.end()) {
		typename ObjectContainer::value_type object = container.front();
		DelCorrespondingObj(object, false);
		delete object;
	}
}
/*!
 	�������� ��� ������� ��������� ���������� ��������. ��� ����� ��� ����, ����� 
	������� ��������� �� ���������.
	\param pobj	- ������ ��� �������
*/
//void SceneTreeView::CleanupAnimatedObjectStates(qdGameObjectAnimated* pobj)
//{
//	const qdGameObjectStateVector& v = pobj->state_vector();
//	while (v.begin() != v.end()) {
//		qdGameObjectState* pst = *v.begin();
//		DelCorrespondingObj(pst, false);
//		delete pst;
//	}
//}
/*!
 	���������� ��� ������� ����������. 
	���� ���� - ������ ������� ������. � ���� ������
			���������, ���� ������ ��� �������� ������ ��������, 
			�� ������������� ������ ��� ���� ������ � ������ �� ���������. 
			����� ������� ������ �� ������ ��������.
	���� ������ - ������ ������� ��������� �������. ��������� ������������ �� ���������
			� ���������. ���� ������������, �� ���������� � ������������� ����� �� 
			�������(SceneTreeView::CanRemoveState(qdGameDispatcher* pdisp, 
			qdGameObjectState* ptrState, TC_TE_PAIR_VECT_t* pl)). ���� �����, �� 
			������� �� ���������(SceneTreeView::RemoveTriggerChainEles (
			TC_TE_PAIR_VECT_t& lst4remove)). ����� ������� ��������� �� ������ ���������
			�������. ���� ��������� ����������, �� ����� �������� ���������, ����������
			qdGameDispatcher::split_global_objects(). ����� �������� 
			qdGameDispatcher::merge_global_objects(). ��������� ��������� �� ���������
*/
bool SceneTreeView::RemoveState(qdGameObjectState* ptrObj, bool bAsk )
{
	qdGameObjectAnimated* poa = 
		static_cast<qdGameObjectAnimated*>(ptrObj->owner());

	if (ptree_helpers::is_obj_mouse(poa))
	{
		qdGameObjectMouse* const pm = static_cast<qdGameObjectMouse*>(poa);
		const int idx = ptree_helpers::get_state_index(pm->state_vector(), ptrObj);
		qdGameObjectState* pCur4DynObj = NULL;
		if (pm->default_cursor(qdGameObjectMouse::OBJECT_CURSOR) == idx)
			pm->set_default_cursor(qdGameObjectMouse::OBJECT_CURSOR, 0);
		else
			pCur4DynObj = pm->get_state(
								pm->default_cursor(qdGameObjectMouse::OBJECT_CURSOR));

		qdGameObjectState* pCur4Zones = NULL;
		if (pm->default_cursor(qdGameObjectMouse::ZONE_CURSOR) == idx)
			pm->set_default_cursor(qdGameObjectMouse::ZONE_CURSOR, 0);
		else
			pCur4Zones = pm->get_state(pm->default_cursor(qdGameObjectMouse::ZONE_CURSOR));

		if (pm->remove_state(ptrObj))
		{
			if (pCur4DynObj)
				pm->set_default_cursor(qdGameObjectMouse::OBJECT_CURSOR, 
						ptree_helpers::get_state_index(pm->state_vector(), pCur4DynObj));
			if (pCur4Zones)
				pm->set_default_cursor(qdGameObjectMouse::ZONE_CURSOR, 
					ptree_helpers::get_state_index(pm->state_vector(), pCur4Zones));
			CursorIdUpdater::run(*m_ptrGameDisp);
			return true;
		}
	}
	else
	{
		TC_TE_PAIR_VECT_t lst4remove;
		NamedObjectList namedObjectList;
		if (CanRemoveState(bAsk, m_ptrGameDisp, ptrObj,&lst4remove, &namedObjectList))
		{
			bool bHasGlobFlag = false;
			if (ptree_helpers::HasGlobalFlag(ptrObj)) {
				qdGameObject* pGlobObj = 
					m_ptrGameDisp->get_global_object(ptrObj->owner()->name());

				//��� ����� ��������� ���������, �������� ������ 
				//���� ������������
				m_ptrGameDisp->split_global_objects(pGlobObj);
				
				poa = static_cast<qdGameObjectAnimated*>(pGlobObj);
				bHasGlobFlag = true;
			}

			RemoveTriggerChainEles(lst4remove);

			bool res = false;
			if(poa->remove_state(ptrObj))
			{
				poa->set_default_state();
				res = true;
			}
			if (bHasGlobFlag)//����� ��������������
				m_ptrGameDisp->merge_global_objects(poa);
			ReinitCounters(m_ptrGameDisp);
			return res;
		}
		else{
			if (!namedObjectList.empty()&&bAsk)
			{
				CString msg;
				msg.LoadString(IDS_STATE_IN_CONDITIONS);
				msg += _T("\n");
				msg += CString(reinterpret_cast<LPTSTR>(IDS_ASK_SHOW_OBJECTS));
				if (AfxMessageBox(msg, MB_ICONEXCLAMATION |MB_YESNO) == IDYES)
				{
					objectList_->clearList();
					boost::rtl::crange<NamedObjectList const> r(namedObjectList);
					while (r) {
						qdNamedObjectReference ref(*r);
						objectList_->addObjectToList(&ref);
						++r;
					}
					objectList_->show();
				}
			}
		}
	}
	return false;
}


/*!
 	���������� ��� ������� � � ����������� �� ���� ��������� ����� �������� 
	����������� ��� ����������� �������� ������� �� �������. ��������� ������ �� 
	������������
	\param pnob - ��������� �����.
	\param bAsk - �������� ��� �� �������� ������������ ������ ����� ����������
	\return \a true - ���� �������� ��������� �������
*/
bool SceneTreeView::DelCorrespondingObj(qdNamedObjectBase* pnob, bool bAsk)
{
	CWaitCursor __wait__;
	if (qdNamedObject* ptrObj = dynamic_cast<qdNamedObject*>(pnob))
	{
		ASSERT(ptrObjectsDependencies_);

		if (!ptree_helpers::is_obj_state(ptrObj)&&ptrObj->is_in_triggers())
			RemoveObjectFromTriggers(ptrObj, m_ptrGameDisp->trigger_chain_list());


		qdNamedObject* owner = ptrObj->owner();
		switch(ptrObj->named_object_type()) {
		case QD_NAMED_OBJECT_STATIC_OBJ:
			{
				static_cast<qdGameScene*>(owner)->remove_object((qdGameObject*)ptrObj);
				return true;
				
			}
		case QD_NAMED_OBJECT_ANIMATED_OBJ:
			{
				qdGameObjectAnimated* const pobj = 
					static_cast<qdGameObjectAnimated*>(ptrObj);
				cleanupObjects(pobj->state_vector());
				static_cast<qdGameScene*>(owner)->remove_object(pobj);
				return true;
				
			}
		case QD_NAMED_OBJECT_MOVING_OBJ:
			{
				qdGameObjectAnimated* pgo = static_cast<qdGameObjectAnimated*>(ptrObj);

				if (ptree_helpers::IsGlobalObj(ptrObj)) {

					m_ptrGameDisp->split_global_objects(pgo);
					RemoveEmptyCopyOfGlobal(pgo);

					cleanupObjects(pgo->state_vector());
					UpdateCatalogsInScenes(QD_NAMED_OBJECT_MOVING_OBJ);
					m_ptrGameDisp->remove_global_object(pgo);
				}
				else
				{
					qdGameScene *pscene = static_cast<qdGameScene*>(owner);
					qdGameObject* pGlobalObj = ptree_helpers::
						IsCopyOfGlobal(m_ptrGameDisp,ptrObj->name());
					if (pGlobalObj) 
						m_ptrGameDisp->split_global_objects(pGlobalObj);

					cleanupObjects(pgo->state_vector());

					pscene->set_active_personage(NULL);
					SelectObjectInSceneView(NULL);
					pscene->remove_object((qdGameObject*)ptrObj);
					if (pGlobalObj)
						m_ptrGameDisp->merge_global_objects(pGlobalObj);
				}
				return true;
			}
		case QD_NAMED_OBJECT_SCENE:
			{
				qdGameScene* pscene = 
					static_cast<qdGameScene*>(ptrObj);
				pscene->set_active_personage(NULL);

				cleanupObjects(pscene->object_list());
				cleanupObjects(pscene->grid_zone_list());
				cleanupObjects(pscene->music_track_list());

				if(m_ptrGameDisp->get_active_scene() == ptrObj)
				{
					m_ptrGameDisp->select_scene((qdGameScene*)NULL);
					m_ptrMainFrame->ResetSceneView();
				}
				return m_ptrGameDisp->remove_scene(static_cast<qdGameScene*>(ptrObj));
			}
		case QD_NAMED_OBJECT_OBJ_STATE:
			{
				return RemoveState(static_cast<qdGameObjectState*>(ptrObj), bAsk);
			}
		case QD_NAMED_OBJECT_GRID_ZONE:
			{
				qdGridZone* const zone = static_cast<qdGridZone*>(ptrObj);
				zone->set_state(false);
				RemoveGridZoneFromTriggers(zone, m_ptrGameDisp->trigger_chain_list());
				return static_cast<qdGameScene*>(owner)->remove_grid_zone(zone);
			}
		case QD_NAMED_OBJECT_TRIGGER_CHAIN:
				return m_ptrGameDisp->remove_trigger_chain(
											static_cast<qdTriggerChain*>(ptrObj));
		case QD_NAMED_OBJECT_INVENTORY:
				return RemoveInventory(static_cast<qdInventory*>(ptrObj), bAsk);
		case QD_NAMED_OBJECT_MINIGAME:
				return m_ptrGameDisp->remove_minigame(
											static_cast<qdMiniGame*>(ptrObj));
		case QD_NAMED_OBJECT_MUSIC_TRACK:
			return static_cast<qdGameScene*>(owner)->
				remove_music_track(static_cast<qdMusicTrack*>(ptrObj));
		case QD_NAMED_OBJECT_GAME_END:
			return m_ptrGameDisp->remove_game_end(static_cast<qdGameEnd*>(ptrObj));
		case QD_NAMED_OBJECT_COUNTER:
			return m_ptrGameDisp->remove_counter(static_cast<qdCounter*>(ptrObj));
		case QD_NAMED_OBJECT_VIDEO:
			{
				if(m_ptrGameDisp->is_video_playing(static_cast<qdVideo*>(ptrObj)))
				{
					m_ptrGameDisp->stop_video();
					m_ptrGameDisp->close_video();
				}
				return m_ptrGameDisp->remove_video(static_cast<qdVideo*>(ptrObj));
			}
		case QD_NAMED_OBJECT_FONT_INFO:
			return m_ptrGameDisp->remove_font_info(static_cast<qdFontInfo*>(ptrObj));
		}
	}
	else if (qdInterfaceObjectBase* piob = dynamic_cast<qdInterfaceObjectBase*>(pnob))
	{
		qdInterfaceObjectBase* owner = piob->owner();
		const type_info& ti = typeid(*piob);
		if (typeid(qdInterfaceButton) == ti
			||typeid(qdInterfaceSlider) == ti
			||typeid(qdInterfaceBackground) == ti
			||typeid(qdInterfaceSave)==ti
			||typeid(qdInterfaceTextWindow) == ti)
		{
			return static_cast<qdInterfaceScreen*>(owner)->
					remove_element(static_cast<qdInterfaceElement*>(piob));
		}
		else if (typeid(qdInterfaceScreen) == ti)
		{
			qdInterfaceDispatcher* const pdisp = 
				static_cast<qdInterfaceDispatcher*>(owner);
			qdInterfaceScreen* const pscr = static_cast<qdInterfaceScreen*>(piob);

			if (pdisp->is_screen_active(pscr))
				pdisp->select_screen(NULL);
			return pdisp->remove_screen(pscr);
		}
		else if (typeid(qdInterfaceElementState) == ti)
		{
			qdInterfaceButton* const pbtn = 
				static_cast<qdInterfaceButton*>(piob->owner());
			int const num_states = pbtn->num_states();
			for(int i = 0; i < num_states; ++i)
				if (piob == static_cast<qdInterfaceObjectBase*>(pbtn->get_state(i)))
					return (pbtn->erase_state(i));
		}
	}
	return false;
}

/*!
	������� ������� ������. ������ ������� ����������
	��������� � �������� � �����,
	� ������� ������� �����.
	\param hItem - ���� - �������� �� ��������
	\return \a true - ���� ������� �����
*/
bool SceneTreeView::CanRemoveItem(HTREEITEM hItem)
{
	CTreeCtrl& tree = GetTreeCtrl();
	if(!hItem||ptree_helpers::IsContainerObj(tree, hItem))
		return false;

	if (ptree_helpers::is_state_node(tree, hItem)) 
	{
		qdGameObjectState* pstate = 
			reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hItem));

		return !IsGlobalState(pstate, hItem);
	}
	return true;
}

/*!
 	���� bAsk = true ���������� � ������������� ����� �� ������� ������.
	������� ������ (SceneTreeView::DelCorrespondingObj(qdNamedObjectBase* ptrObj, 
	bool bAsk)). ���� �������� ������ �������, �� ����������� �������������� ������
	�� �������� ����
	\param hItem - ����, ������� ����� ������
	\param bAsk - ���������� �� � ������������
*/
void SceneTreeView::RemoveItem(HTREEITEM hItem, bool bAsk)
{
	CWaitCursor wait;
	CTreeCtrl& tree = GetTreeCtrl();
	qdNamedObjectBase* pnob = 
		reinterpret_cast<qdNamedObject*>(tree.GetItemData(hItem));
	CString strCap;
	CString strMsg;
	
	strCap.LoadString(IDS_DEL_CAP);
	strMsg.Format(IDS_DEL_OBJECT, pnob->name());
	if(!bAsk || MessageBox(strMsg, strCap, MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
	{
		objectList_->clearList();
			
		if (dynamic_cast<qdNamedObject*>(pnob))
		{
			if (statObjectDependencies(static_cast<qdNamedObject *>(pnob)))
				return;
		}
		class SoundDisabler{
			bool soundEnabled_;
		public:
			SoundDisabler(){
				if(sndDispatcher* p = sndDispatcher::get_dispatcher())
				{
					soundEnabled_ = p->is_enabled();
					p -> disable();
				}
			}
			~SoundDisabler(){
				if(sndDispatcher* p = sndDispatcher::get_dispatcher())
					if (soundEnabled_) p ->enable();
			}
		}__soundDisabler__;

		if(DelCorrespondingObj(pnob, bAsk))
		{
			if (qdNamedObject* pno = dynamic_cast<qdNamedObject*>(pnob))
			{
				switch(pno->named_object_type()) {
				case QD_NAMED_OBJECT_OBJ_STATE:
					if (riStateSuppl(pno, hItem)) 
						hItem = NULL;
					break;
				case QD_NAMED_OBJECT_TRIGGER_CHAIN:
					SetInTgrIcon(static_cast<qdTriggerChain*>(pno), FALSE, -1);
					break;
				}
			}
			else if (qdInterfaceObjectBase* piob = 
				dynamic_cast<qdInterfaceObjectBase*>(pnob))
			{
				std::type_info const& ti = typeid(*piob);
				if (typeid(qdInterfaceElementState) == ti)
				{
					if (riIBtnStateSuppl(piob, hItem))
						hItem = NULL;
				}
			}
			if(hItem)
			{
				if (tree.GetSelectedItem() == hItem) 
					tree.SelectItem(tree.GetPrevVisibleItem(hItem));
				tree.DeleteItem(hItem);
			}
			
			delete pnob;
			
			Invalidate();
			SetProjChange();
			if (m_IsSceneViewActive) 
				sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
			else
				m_ptrITView->Redraw();
		}
	}
}

/*!
 	���� � ������ ������ �� �������� ��������� - ��� ���������.
	\param piob		- ��������� ������
	\param hItem	- ���� ���������
	\return \a true	- ���� ������ ���� �������(hItem - ���� ����������������)
*/
bool SceneTreeView::riIBtnStateSuppl(qdInterfaceObjectBase *piob, HTREEITEM hItem)
{
	qdInterfaceButton* const pbtn = static_cast<qdInterfaceButton*>(piob->owner());
	if (pbtn->num_states())
		return false;
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hBtn = tree.GetParentItem(hItem);
	if (DelCorrespondingObj(pbtn, false))
	{
		if (tree.GetSelectedItem() == hItem) 
			tree.SelectItem(tree.GetPrevVisibleItem(hBtn));
		tree.DeleteItem(hBtn);
		piob->set_owner(NULL);
		delete pbtn;
		return true;
	}
	return false;
}

/*!
 	�������������� �������� ���������, ���� �� �������.
	���������, ���� � ������� ������ �� �������� ���������, �� ������� � ������
	\param pno		- ��������� ���������
	\param hItem	- ���� ���������
	\return \a true - ���� hItem ���� ����������������. ��� ����� ��������� ��-�� 
					�������� ������������� ����, ���� ��-�� ������������ ������ ���������
*/
bool SceneTreeView::riStateSuppl(qdNamedObject *pno, HTREEITEM hItem)
{
	qdGameObjectAnimated* owner = 
		static_cast<qdGameObjectAnimated*>(pno->owner());
			
	if (!m_IsSceneViewActive) 
		m_ptrITView->Redraw();

	CTreeCtrl& tree = GetTreeCtrl();

	if (owner->named_object_type() == QD_NAMED_OBJECT_MOUSE_OBJ) 
		return false;
	else
	{
		if(HasNoStates(owner))
		{
			bool bIsGlobal = ptree_helpers::IsGlobalObj(owner);
			HTREEITEM hObjItem = NULL;
			if (bIsGlobal) 
				hObjItem = FindObjectItem(owner);
			else
			{
				hObjItem = tree.GetParentItem(hItem);
				if(owner->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ)
					hObjItem = tree.GetParentItem(hObjItem);
			}

			//�������� ���������� ����� ��������� ��������
			//������ ��� �� ����� ���� ������� � ������ ������
			thread_suspender suspend(this);

			std::map<qdNamedObject const*, NamedObjectList> statistic;
			ptrObjectsDependencies_ = &statistic;
			objectList_->clearList();
			StatObjectDependencies::run(m_ptrGameDisp, owner, ptrObjectsDependencies_);
			if (!statistic.empty())
			{
				if (filterDepedenciesBound(owner) == ACROSS_OBJECT)
				{
					fillObjectList();
					objectList_->showObjectInList(&qdNamedObjectReference(owner));
					objectList_->show();
					return false;
				}
			}

			if(DelCorrespondingObj(owner, false))
			{
				if (tree.GetSelectedItem() == hItem||tree.GetSelectedItem() == hObjItem) 
					tree.SelectItem(tree.GetPrevVisibleItem(hObjItem));
				
				//���� ��������� �������� �� ���� ������������
				//������ ����, �� �������� ���������� � �����
				if(owner == m_ptrPlayedObject)
					SuspendAndClear();//������������� ��������� ���������
				
				if (bIsGlobal) 
					UpdateCatalogsInScenes(
							static_cast<qdNamedObjectType>(owner->named_object_type()));

				tree.DeleteItem(hObjItem);
				
				delete owner;

				return true;			
			}
		}
		else
		{
			if (ptree_helpers::IsGlobalObj(owner)) {
				
				if (tree.GetSelectedItem() == hItem) 
					tree.SelectItem(tree.GetPrevVisibleItem(hItem));
				
				ReloadStates(tree, FindObjectItem(owner), owner);
				SyncGlobalStatesCatalog(owner);
				return true;
			}
		}
	}
	return false;
}

/*!
 	�������� ���������� ptree_helpers::DeleteSubItems()
*/
void SceneTreeView::DeleteChildern(HTREEITEM hPar)
{
	CTreeCtrl& tree = GetTreeCtrl();
	ptree_helpers::DeleteSubItems(tree, hPar);
}

/*!
 	������� ���� ��������(SceneTreeView::FindObjectItem()). � ������� ���(
	SceneTreeView::RemoveItem());
	\param ptrObj	- ������, ������� ����� �������
	\param bAsk		- ���������� �� ������������ �� ��������
	\return \a true - ���� ��������� ����������� ��������� �������
*/
bool SceneTreeView::DeleteObject(qdNamedObjectBase* ptrObj, bool bAsk)
{
	if(ptrObj)
	{
		HTREEITEM hItem = FindObjectItem(ptrObj);
		if(hItem)
		{
			RemoveItem(hItem, bAsk);
			return true;
		}
	}
	return false;
}

/*!
 	��� ����������� �������� ������ �� ��������. ��� ������������� �������� � ����������
	������� ���������� SceneTreeView::aoRemoveEmptyCopyOfGlobal(qdGameObjectAnimated* pobj)
	\param pobj - ���������� ������, ����� �������� ����� ������ � �������
*/
void SceneTreeView::RemoveEmptyCopyOfGlobal(qdGameObject* pobj)
{
	if (pobj->named_object_type() != QD_NAMED_OBJECT_STATIC_OBJ) {
		aoRemoveEmptyCopyOfGlobal(static_cast<qdGameObjectAnimated*>(pobj));
	}
}

/*!
	�������� ������ �������(��� ���������) ����� \a pobj � ������
	(SceneTreeView::GetCopyOfGlobalList(LPCTSTR name, 
			CHECK_FUN f, qdGameObjectList* pres)
	������� ��� ��� ������� (SceneTreeView::DelCorrespondingObj(
	qdNamedObjectBase* ptrObj, bool bAsk)
 	\remarks ����� ��������� ������, ���� � ��� ��� ���������.
	\param pobj -  - ���������� ������, ����� �������� ����� ������ � �������
*/
void SceneTreeView::aoRemoveEmptyCopyOfGlobal(qdGameObjectAnimated* pobj)
{
	qdGameObjectList list;
	GetCopyOfGlobalList(pobj->name(), HasNoStates, &list);
	if (!list.empty()) {
		qdGameObjectList::iterator i = list.begin(), e = list.end();
		for(; i != e; ++i)
			DelCorrespondingObj(*i, false);
	}
}

/*!
 	��������� ������������ �� ��������� ��� ����� � ��������. ���� ������������,
	�� ������������� ������������. ���� �� ������������ � ��������, ��� ���� 
	������������ ����������, �� ������� ����� �� ���������(
	qdInventory::remove_cell_set()). ��������� ������������ �� ��� ����� �� ����� ������
	��� � ������ ���������(SceneTreeView::IsInventoryTypeUsed()). ���� ��� ��, �������
	��� ����� �� �������(qdGameDispatcher::remove_inventory_cell_type())
	\param pinv		- ���������, �� �������� ����� ������� �����
	\param icellset - �� ������
	\return \a true -  ���� ����� ��� ������
*/
bool SceneTreeView::DeleteInventoryCellset(qdInventory* pinv, int icellset)
{
	if (icellset == -1)
		return false;

	CWaitCursor wait;

	ASSERT(!pinv->get_cell_set(icellset).cells().empty());
	int type = pinv->get_cell_set(icellset).cells().front().type();

	bool bUsedInObj = IsInventoryTypeUsedInObjs(type);
	if (!bUsedInObj||
		MessageBox(CString((LPTSTR)IDS_CELLSET_CELL_TYPE_USED_IN_OBJ),
									NULL, MB_OKCANCEL) == IDOK)
		pinv->remove_cell_set(icellset);
	else
		return false;

	if (!IsInventoryTypeUsed(type)&&!bUsedInObj)
		m_ptrGameDisp->remove_inventory_cell_type(type);
	return true;
}

//! ��������� ������� �� ����������� �������� ����� �� ������� ���� �����
SceneTreeView::eDependenciesBound SceneTreeView::filterDepedenciesBound(
													qdNamedObject const* object)
{
	return FilterObjectDependencies::run(object, ptrObjectsDependencies_)?
										ACROSS_OBJECT : IN_OBJECT;
}

bool SceneTreeView::statObjectDependencies(qdNamedObject const* object)
{
	std::map<qdNamedObject const*, NamedObjectList> statistic;
	ptrObjectsDependencies_ = &statistic;
	StatObjectDependencies::run(m_ptrGameDisp, object, ptrObjectsDependencies_);
	if (!statistic.empty())
	{
		if (filterDepedenciesBound(object) == ACROSS_OBJECT)
		{
			fillObjectList();
			objectList_->showObjectInList(&qdNamedObjectReference(object));
			objectList_->show();
			return true;
		}
	}
	return false;
}