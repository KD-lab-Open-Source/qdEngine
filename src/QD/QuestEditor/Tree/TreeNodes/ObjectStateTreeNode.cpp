#include "stdafx.h"
#include "resource.h"
#include "ptree_consts.h"
#include "obj_type_detectors.h"
#include ".\objectstatetreenode.h"
#include "menu_helpers.h"

#include "Object List/IObjectList.h"
#include "ISceneViewFacade.h"
#include "ITriggerView.h"
#include "ptree_te_filters.h"

#include "Main/StateTraits.h"
#include "Main/ObjectTraits.h"

#include "EditStateDlg.h"
#include "CursorPropDlg.h"

#include "MovementTypeManager.h"

#include "qd_game_object_state.h"
#include "qd_game_object_mouse.h"
#include "qd_game_object_moving.h"
#include "qd_counter.h"
#include "qd_trigger_chain.h"

#include "CursorIdUpdater.h"

#include "Main/Utils/CopyOfGlobalUpdater.h"
#include "Main/Utils/UpdateTriggerElementTitles.h"

#include "ptree_hlprs.hpp"
#include "te_helpers.h"
#include "Main/StateTraits.h"

#include "name_op.h"

#include "Tree/LoadTreeAssist.h"

#include <boost/pool/pool_alloc.hpp>

#include "snd_dispatcher.h"


bool ShowCondDlg(qdGameObjectState* ptrState, class qdGameDispatcher* ptrDisp);
//! У объекта есть только одно пустое состояние?
bool HasLastEmptyState(qdNamedObject* pno);

namespace{

	//! Пара <цепочка триггеров, указатель на элемент>
	typedef	std::pair<qdTriggerChain*, qdTriggerElementPtr> TC_TE_PAIR_t;
	typedef std::vector<TC_TE_PAIR_t> TC_TE_PAIR_VECT_t;

	//! Дизейблит пункты меню связанные с анимацией в меню состояния
	void disableAnimationMenuItems(CMenu* pm)
	{
		menu_h::disableMenuItem(pm, IDC_STATE_DIMS);
		menu_h::disableMenuItem(pm, IDC_STATE_HAS_BOUND);
		menu_h::disableMenuItem(pm, IDC_BTN_AUTO_BND);
		menu_h::disableMenuItem(pm, IDC_STACK_STATE);
		menu_h::disableMenuItem(pm, IDC_COORDS_ANIMATION);
		//	menu_h::disableMenuItem(pm, IDC_MAKE_BOUND_AS_FIRST);
	}
	/*!
	Модифицирует заголовок пункта меню IDC_SELECT_MOVEMENT_TYPE меню состояния, 
	вставляя в него описание текущего типа движения.
	*/
	void modifySelectMovementCaption(CMenu *pm, qdGameObjectStateWalk const* pstate)
	{
		CString str;
		pm->GetMenuString(IDC_SELECT_MOVEMENT_TYPE, str, MF_BYCOMMAND);
		MovementTypeManager mgr;
		str += _T(" - ");
		str += mgr[pstate->movement_type()]->description();
		MENUITEMINFO mi;
		mi.cbSize = sizeof (MENUITEMINFO); // must fill up this field
		mi.fMask = MIIM_STATE;             // get the state of the menu item
		pm->GetMenuItemInfo(IDC_SELECT_MOVEMENT_TYPE, &mi);

		pm->ModifyMenu(IDC_SELECT_MOVEMENT_TYPE, MF_BYCOMMAND|MF_STRING,
			IDC_SELECT_MOVEMENT_TYPE, str);
		pm->EnableMenuItem(IDC_SELECT_MOVEMENT_TYPE,  mi.fState|MF_BYCOMMAND);
	}

	/*!
	Пробегаем по всем счетчикам и реинитим их, чтобы избавиться от подвисших ссылок 
	на состояния
	*/
	void ReinitCounters(qdGameDispatcher const* gameDisp)
	{
		qdCounterList const& lst = gameDisp->counter_list();
		std::for_each(lst.begin(), lst.end(), std::mem_fun(qdCounter::init));
	}

	//! У объекта есть только одно состояние?
	bool HasLastState(qdNamedObject* pno)
	{
		ASSERT(ptree_helpers::is_obj_animated(pno)
			||ptree_helpers::is_obj_moving(pno)
			||ptree_helpers::is_obj_mouse(pno));

		qdGameObjectAnimated* ptrObj = static_cast<qdGameObjectAnimated*>(pno);
		return (ptrObj->state_vector().size() == 1);
	}


	//! У объект остались состояния?
	BOOL HasNoStates(qdGameObject* pno)
	{
		ASSERT(ptree_helpers::is_obj_animated(pno)
			||ptree_helpers::is_obj_moving(pno));

		return (static_cast<qdGameObjectAnimated*>(pno)->state_vector().empty());
	}

	//////////////////////////////////////////////////////////////////////////

	/*!
	Проебегает по цепочкам триггеров и пытается найти в элементах цепочек элементы,
	которые содержат состояние \a ptrState, и складывает их в список. Если список пуст,
	то разрешаем удалить. Если не пуст и \a bUserAsk запрашиваем у пользователя: можно
	ли удалить. 
	\param bUserAsk		- Спрашивать ли упользователя
	\param pdisp		- Указатель на диспетчер проекта
	\param ptrState		- Состояние, которое пытаются удалить
	\param pl			- указатель на объект-список, куда будут сложены все элемент,
	содержащие \a ptrState
	*/
	bool canRemoveState(bool bUserAsk, qdGameDispatcher* pdisp,
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

		if (!bUserAsk) //если нам не надо спрашивать пользователя,
			//то дальнейшее значение не имеет
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
	Удаляет элемент из цепочки триггеров
	*/
	void RemoveTgrEle(TC_TE_PAIR_t& pr)
	{
		te_h::RemoveLinks(pr.first, pr.second);
		pr.first->remove_element(pr.second, true);
	}

	/*!
	Пробегает по списку и для каждого элемента вызывает RemoveTgrEle()
	*/
	void RemoveTriggerChainEles(TC_TE_PAIR_VECT_t& lst4remove){
		std::for_each(lst4remove.begin(), lst4remove.end(), RemoveTgrEle);
	}

	/*!
 		Находит первое состояние, удовлетворяющее фильтру
	*/
	template<class _Filter>
	qdGameObjectState* findFirstState(qdGameObjectAnimated* pobj, _Filter _f)
	{
		boost::rtl::crange<qdGameObjectStateVector const> res = 
			boost::rtl::rng::find_if(pobj->state_vector(), _f);
		if (res)
			return *res;
		return NULL;
	}

	/*!
 		Находит первое собственное(неглобальное) походочное состояние в объекте
	*/
	qdGameObjectState* findFirstOwnWalk(qdGameObjectAnimated* pobj)
	{
		return findFirstState(pobj, load_tree_assist::own_walk(pobj));
	}

	/*!
 		Находит первое собственное(неглобальное) непоходочное состояние в объекте
	*/
	qdGameObjectState* findFirstOwnNotWalk(qdGameObjectAnimated* pobj)
	{
		return findFirstState(pobj, load_tree_assist::own_not_walk(pobj));
	}

}

//! У объекта есть только одно пустое состояние?
bool HasLastEmptyState(qdNamedObject* pno)
{
	if (HasLastState(pno)) {
		qdGameObjectState* ptrState = 
			static_cast<qdGameObjectAnimated*>(pno)->state_vector().front();
		return ptrState->is_state_empty();		
	}
	return false;
}


ObjectStateTreeNode::ObjectStateTreeNode(qdNamedObject* object)
: ConditionalObjectTreeNode(object)
{
}

ObjectStateTreeNode::~ObjectStateTreeNode(void)
{
}

bool ObjectStateTreeNode::load(ITreeNodeFactory* factory, 
							   TreeLogic& treeLogic, 
							   ITreeNode * parent, 
							   qdNamedObjectBase const* owner,
							   ITreeNode* insertAfter)
{
	qdNamedObject const* ownr = static_cast<qdNamedObject const*>(owner);
	CTreeListItem* const item = 
		insertToTree(treeLogic, 
					getObject()->name(), 
					parent, 
					selectPicture(static_cast<qdGameObjectState const&>(*getObject()), 
								*ownr),
					reinterpret_cast<DWORD_PTR>(getObject()), 
					insertAfter);
	if (!item)
		return false;

	resetDefaultStateIcon(treeLogic);
	if (getObject()->is_in_triggers()
		&&!(StateTraits::isGlobalForObj(*getObject(), *ownr)
			&&!ptree_helpers::is_obj_mouse(ownr)))
		setINTriggerFlag(treeLogic, TIC_SET_NOT_ACTIVE);

	return true;
}

int ObjectStateTreeNode::selectPicture(qdGameObjectState const& state, 
									   qdNamedObject const& owner)
{
	if (ptree_helpers::is_obj_mouse(&owner))
		return selectMouseStatePicture(
			static_cast<qdGameObjectMouse const&>(owner), state);
	else
	{
		if (!state.coords_animation()->is_empty())
			return II_CA_STATE_NODE;
		else if (StateTraits::isGlobalForObj(state, owner)) 
			return II_GLOBAL_STATE_NODE;
		else if (ObjectTraits::isGlobal(owner)) 
			return II_GLOBAL_OBJ_STATE_NODE;
		else if (state.check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_INVENTORY))
			return II_INVENTORY_STATE_NODE;
		else if (state.has_text())
			return II_DIALOG_STATE_NODE;
		else if (state.check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_MOVE_TO_INVENTORY))
			return II_STATE4INVENTORY_NODE;
	}
	return II_STATE_NODE;
}

int ObjectStateTreeNode::selectMouseStatePicture(
						qdGameObjectMouse const& mouse, 
						qdNamedObject const& state)
{
	int const id = mouse.default_cursor(qdGameObjectMouse::OBJECT_CURSOR);
	int const id2 = mouse.default_cursor(qdGameObjectMouse::ZONE_CURSOR);
	int const id3 = mouse.default_cursor(qdGameObjectMouse::MAIN_MENU_CURSOR);
	int const id4 = mouse.default_cursor(qdGameObjectMouse::INGAME_INTERFACE_CURSOR);
	int const id5 = mouse.default_cursor(qdGameObjectMouse::INVENTORY_OBJECT_CURSOR);
	int const state_id = StateTraits::getStateIndex(mouse.state_vector(), &state);

	if (!state_id)
		return II_CURSOR_FOR_ALL_NODE;

	if (id&&id == id2&&state_id == id) 
		return II_CURSOR_FOR_DYN_OBJ_AND_ZONES;
	else
	{
		if (id&&id == state_id)
			return II_CURSOR_FOR_DYN_OBJ;

		if (id2&&id2 == state_id)
			return II_CURSOR_FOR_ZONES;
		if (id3&&id3 == state_id)
			return II_CURSOR_FOR_MAIN_MENU;
		if (id4&&id4 == state_id)
			return II_CURSOR_FOR_INGAME_INTERF;
		if (id5&&id5 == state_id)
			return II_CURSOR_FOR_OBJ_IN_INVENTORY;
	}
	return II_CURSOR_NODE;
}

//! Возвращает идентификатор ресурса меню для узла
DWORD ObjectStateTreeNode::getMenuResId() const{
	return IDR_STATE_NODE;
}

void ObjectStateTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const{
	qdGameObjectState const* pstate = static_cast<qdGameObjectState const*>(getObject());
	qdGameDispatcher const* const gameDisp = treeLogic.getGameDispatcher();
	if (pstate->owner()->named_object_type() == QD_NAMED_OBJECT_MOUSE_OBJ)
	{
		pm->RemoveMenu(IDC_COORDS_ANIMATION, MF_BYCOMMAND);
		pm->RemoveMenu(IDC_BIND_OBJ_CENTER, MF_BYCOMMAND);
		pm->RemoveMenu(IDC_BTN_AUTO_BND, MF_BYCOMMAND);
		pm->RemoveMenu(IDC_STATE_HAS_BOUND, MF_BYCOMMAND);
		pm->RemoveMenu(IDC_STATE_DIMS, MF_BYCOMMAND);
		pm->RemoveMenu(IDC_MAKE_BOUND_AS_FIRST, MF_BYCOMMAND);
		pm->RemoveMenu(IDC_SELECT_MOVEMENT_TYPE, MF_BYCOMMAND);
		//удаляем сепаратор
		pm->RemoveMenu(5, MF_BYPOSITION);
		pm->RemoveMenu(5, MF_BYPOSITION);
		if (!treeLogic.isSceneViewActive()||!gameDisp->get_active_scene()) 
			menu_h::disableMenuItem(pm, IDC_STACK_STATE);
		pm->RemoveMenu(IDC_STATE_CONDITION, MF_BYCOMMAND);
	}
	else
	{
		bool bDisableAll = false;
		if (StateTraits::hasGlobalFlag(pstate)) {
			//вычисляем конкретное местополжение клика. 
			//Если кликнули в глобальное состояние под глобальным же персожаем,
			//то позволяем делать все.
			//Если в под сценой(т.е. под копией глобального персонажа в сцене),
			//то модифицируем меню.

			if (treeLogic.isNodeUnderScene(this)) 
				bDisableAll = true;
			else
				menu_h::disableMenuItem(pm, IDC_COORDS_ANIMATION);
		}

		if (bDisableAll) 
			menu_h::disableAllMenuItems(pm);
		else{
			//локальные состояния
			qdGameScene const* const pActiveScene = gameDisp->get_active_scene();
			//если состояние не принадлежит активной сцене, то производим доп. запрещения
			if (pActiveScene != static_cast<qdGameScene*>(pstate->ref_owner()->owner())) 
			{
				if (!pActiveScene||!treeLogic.isSceneViewActive()) {
					menu_h::disableMenuItem(pm, IDC_STATE_DIMS);
					menu_h::disableMenuItem(pm, IDC_STATE_HAS_BOUND);
					menu_h::disableMenuItem(pm, IDC_BTN_AUTO_BND);
					menu_h::disableMenuItem(pm, IDC_MAKE_BOUND_AS_FIRST);
					menu_h::disableMenuItem(pm, IDC_STACK_STATE);
				}
				menu_h::disableMenuItem(pm, IDC_COORDS_ANIMATION);
				menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES);
				menu_h::disableMenuItem(pm, IDC_BIND_OBJ_CENTER);
			}
			else{
				if (StateTraits::isMaskState(pstate)) 
				{
					disableAnimationMenuItems(pm);
					menu_h::disableMenuItem(pm, IDC_MAKE_BOUND_AS_FIRST);
					menu_h::disableMenuItem(pm, IDC_CANCEL_REMOVING_EDGES);
				}
				else
				{
					if (!treeLogic.isSceneViewActive()){
						disableAnimationMenuItems(pm);
					}
					if (StateTraits::isStaticState(pstate))
					{
						qdGameObjectStateStatic const* p = 
							static_cast<qdGameObjectStateStatic const*>(pstate);
						if (!p->animation())
						{
							disableAnimationMenuItems(pm);
							menu_h::disableMenuItem(pm, IDC_BIND_OBJ_CENTER);
							menu_h::disableMenuItem(pm, IDC_EDIT_STATE_ANIMATION);
							menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES);
						}
					}
					qdGameObjectAnimated* const owner = 
						static_cast<qdGameObjectAnimated*>(pstate->owner());
					if(qdGameObjectState* const pdef_state = owner->get_default_state()){
						if (!pdef_state->has_bound())
							menu_h::disableMenuItem(pm, IDC_MAKE_BOUND_AS_FIRST);
					}
				}
			}

			if (!StateTraits::isStaticState(pstate))
			{
				menu_h::disableMenuItem(pm, IDC_EDIT_STATE_ANIMATION);
				menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES);
				menu_h::disableMenuItem(pm, IDC_BIND_OBJ_CENTER);
			}
		}
		if (StateTraits::isStaticState(pstate))
			menu_h::checkMenuItem(pm, IDC_STATE_HAS_BOUND, pstate->has_bound());
		if (StateTraits::isWalkState(pstate))
		{
			modifySelectMovementCaption(pm, 
				static_cast<qdGameObjectStateWalk const*>(pstate));
		}
		else
			pm->RemoveMenu(IDC_SELECT_MOVEMENT_TYPE, MF_BYCOMMAND);
	}
}

bool ObjectStateTreeNode::showProperties(TreeLogic& logic, int iActive)
{
//	sndDispatcher::SoundDisabler disableSound;
	sndDispatcher::get_dispatcher()->stop_sounds();

	qdGameObjectState* pstate = static_cast<qdGameObjectState*>(getObject());

	if (ptree_helpers::is_obj_mouse(pstate->owner()))
	{
		CursorPropDlg dlg(NULL);
		dlg.set_data(logic.getGameDispatcher(), 
			static_cast<qdGameObjectStateStatic*>(pstate));
		return (dlg.DoModal() == IDOK);
	}
	else{
		EditStateDlg dlg(NULL);
		dlg.set_data(logic.getGameDispatcher(), 
				static_cast<qdGameObjectAnimated*>(pstate->owner()));
		if (ObjectTraits::isGlobal(*pstate->ref_owner())) {
			dlg.suppress_coord_anim();
			if (!logic.getGameDispatcher()->get_active_scene()){
				dlg.suppress_bound();
				dlg.suppress_stacking();
			}
		}
		else if (static_cast<qdGameScene*>(pstate->owner()->owner()) 
			!= logic.getGameDispatcher()->get_active_scene()) {
				dlg.suppress_bound();
				dlg.suppress_edit_coord_anim();
				dlg.suppress_stacking();
			}
		else if (StateTraits::isMaskState(pstate))
		{
			dlg.suppress_coord_anim();
			dlg.suppress_stacking();
		}
		dlg.set_state(pstate);

		if (dlg.DoModal() == IDOK)
		{
			qdGameObjectState* state = static_cast<qdGameObjectState*>(getObject());
			qdGameObjectAnimated* obj = 
				static_cast<qdGameObjectAnimated*>(state->owner());
			obj->set_state(state);
			return true;
		}
	}
	return false;
}

void ObjectStateTreeNode::updateItemImage(TreeLogic& treeLogic)
{
	qdGameObjectState* const state = static_cast<qdGameObjectState*>(getObject());

	treeLogic.getTreeListCtrl().SetItemImage(
		getTreeListItem(), selectPicture(*state, *state->owner()));
	resetDefaultStateIcon(treeLogic);
}

bool ObjectStateTreeNode::showConditions(TreeLogic& logic)
{
	if (ObjectTraits::is_obj_mouse(getObject()->owner()))
		return false;

	sndDispatcher::get_dispatcher()->stop_sounds();
	
	if (ObjectTraits::isGlobal(*getObject()->ref_owner())
		&&logic.isNodeUnderScene(this))
		return false;

	if (ShowCondDlg(
				static_cast<qdGameObjectState*>(getObject()), 
				logic.getGameDispatcher()))
	{
		return true;
	}

	return false;
}

/*!
На получение:
выбирает объект-владелец состояния в редакторе сцены. 
Если состояние маска, то редактор переводится в состояние редактирования маски
и вызывается метод OnEditStateMask()
Если состояние другого типа редактор переводится в состояние редактирования 
сцены(VS_EDIT_SCENE). Если владелец состояния мышь, то устанавливает её в
центр экрана. Проигрывает анимацию


На потерю:
Если состояние маска, то останавливаем редактирование маски.
Если обычное, то прикращаем прокручивать анимацию или редактировать размеры, 
если они редактировались
\param pno		- Объект, узел которого получил/потерял выделение
\param bIsObj	- Является ли \a ptrObj узателем на объект
\param bSelect	- \a true - получил выделение.\a false - потерял
\retval true	- если надо перерисовать активный редактор
\retval false	- если не надо перерисовать активный редактор
*/
bool ObjectStateTreeNode::onSelectInSceneView(TreeLogic& logic, bool bSelect)
{
	ISceneViewFacade* sceneFacade = logic.getSceneView();
	if (bSelect) {
		qdGameObjectState* ptrState = static_cast<qdGameObjectState*>(getObject());
		sceneFacade->SelectObj(static_cast<qdGameObject*>(ptrState->owner()));
		sceneFacade->SetActiveState(ptrState);

		if (StateTraits::isMaskState(ptrState)) {

			static_cast<qdGameObjectAnimated*>(ptrState->owner())->set_state(ptrState);
			sceneFacade->SetViewState(VS_EDIT_MASK);
			sceneFacade->SetSubTools(SSTE_EDIT_MASK);
			//состояния выставляются внутри
			sceneFacade->OnEditStateMask(static_cast<qdGameObjectStateMask*>(ptrState));
			return true;
		}
		else
		{
			sceneFacade->SetViewState(VS_EDIT_SCENE);
			
			if (qdGameScene* scene = logic.getGameDispatcher()->get_active_scene()) {
				if (ptree_helpers::is_obj_mouse(ptrState->owner()))
				{
					qdGameObjectMouse* const p = 
						static_cast<qdGameObjectMouse*>(ptrState->owner());
					qdCamera* const pcam = scene->get_camera();
					p->set_pos(Vect3f(pcam->get_scr_center_x(),
						pcam->get_scr_center_y(), 0.f));
					p->set_default_pos(Vect3f(pcam->get_scr_center_x(),
						pcam->get_scr_center_y(), 0.f));

				}
				logic.showAnimation(ptrState);
//				ShowStateAnimation(ptrState);
			}

//			return false;
			return true;
		}
		sceneFacade->SetViewState(VS_EDIT_SCENE);
		return true;
	}
	else
	{
		sceneFacade->SetActiveState(NULL);
		qdGameObjectState *ptrState = static_cast<qdGameObjectState*>(getObject());
		if (StateTraits::isMaskState(ptrState)) 
			sceneFacade->OnAbortStateMaskEdit(true);
		else
		{
			if (sceneFacade->GetViewState() == VS_EDIT_STATE) 
				logic.endEditStateDims(true);

//			if (m_ptrPlayedState == ptrState)	
//				StopAnimation(ptrState);
			logic.stopAnimation(ptrState);
		}
	}

	return false;
}

/*!
Всегда, если есть активная сцена, выделяет объект в соответствии с установленным 
типом фильтра
На получение:
Выделяет узел, соответствующий состоянию
\param bSelect	- \a true - получил выделение.\a false - потерял
\retval true	- если надо перерисовать активный редактор
\retval false	- если не надо перерисовать активный редактор

*/
bool ObjectStateTreeNode::onSelectInTriggerView(TreeLogic& logic, bool bSelect)
{
	//это такая штука, чтобы темплэйт инстанциировать правильно
	typedef bool (*f)(const qdNamedObject* po);

	ITriggerView* triggerView = logic.getTriggerView();
	qdGameScene * const ptrScene = logic.getGameDispatcher()->get_active_scene();
	if (bSelect&&ptrScene)
	{
		switch(logic.getFilterScheme()) {
		case TreeLogic::FS_ENTRY_FOR_DIALOGS:
			triggerView->SelectByFilter(
				tef_dlg_state(ptrScene), 
				ITriggerView::SA_SELECT, 
				ITriggerView::SM_EXCLUSIVE);
			break;
		case TreeLogic::FS_ENTRY_FOR_DYN_OBJ:
			triggerView->SelectByFilter(
				tef_by_scenes_objs(ptrScene,
				ptree_helpers::is_obj_animated), 
				ITriggerView::SA_SELECT, 
				ITriggerView::SM_EXCLUSIVE);
			break;
		case TreeLogic::FS_ENTRY_FOR_PERS:
			triggerView->SelectByFilter(
				tef_by_scenes_objs(ptrScene,
				ptree_helpers::is_obj_moving), 
				ITriggerView::SA_SELECT, 
				ITriggerView::SM_EXCLUSIVE);
			break;
		}
	}
	if (bSelect) 
	{
		triggerView->SelectByFilter(
			tef_by_obj(getObject()), 
			ITriggerView::SA_SELECT,
			(TreeLogic::FS_NONE == logic.getFilterScheme())?
									ITriggerView::SM_EXCLUSIVE:
									ITriggerView::SM_INTERSECT);
	}
	if (bSelect
		&&logic.getShowTriggerElement()
		&&getObject()->is_in_triggers())
		triggerView->ShowTrigger(getObject());
	return true;
}

bool ObjectStateTreeNode::onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState)
{
	if (!getObject())
		return false;
	if (logic.isSceneViewActive())
	{
		if (newState&NS_FOCUSED)
			return onSelectInSceneView(logic, true);
		else if (oldState&NS_FOCUSED)
			return onSelectInSceneView(logic, false);
	}
	else
	{
		if (newState&NS_FOCUSED)
			return onSelectInTriggerView(logic, true);
		else if (oldState&NS_FOCUSED)
			return onSelectInTriggerView(logic, false);
	}
	return false;
}


bool ObjectStateTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!getObject()) 
		return true;

	if (!preDelete(logic, ask, statDepends))
		return false;
	if (ObjectTraits::is_obj_mouse(getObject()->owner()))
	{
		if (deleteMouseState(logic))
		{
			ITreeNode* prevVisible = logic.getPrevVisibleNode(this);
			delete getObject();
//			setObject(NULL);
			logic.deleteNode(this);
			logic.setFocusedNode(prevVisible);
			return true;
		}
	}
	else
	{
		if (deleteAnimatedObjectState(logic, ask))
		{
			ITreeNode* prevVisible = logic.getPrevVisibleNode(this);
			qdNamedObject* obj = getObject();
			if (!postDeleteNotMouseState(logic))
			{
				setObject(0);
				logic.deleteNode(this);
				logic.setFocusedNode(prevVisible);
			}
			delete obj;
			return true;
		}
	}
	return false;
}

bool ObjectStateTreeNode::deleteMouseState(TreeLogic& logic)
{
	qdGameObjectMouse* const pm = static_cast<qdGameObjectMouse*>(getObject()->owner());
	qdGameObjectState* state = static_cast<qdGameObjectState*>(getObject());
	const int idx = StateTraits::getStateIndex(pm->state_vector(), state);
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

	if (pm->remove_state(state))
	{
		if (pCur4DynObj)
			pm->set_default_cursor(qdGameObjectMouse::OBJECT_CURSOR, 
			StateTraits::getStateIndex(pm->state_vector(), pCur4DynObj));
		if (pCur4Zones)
			pm->set_default_cursor(qdGameObjectMouse::ZONE_CURSOR, 
				StateTraits::getStateIndex(pm->state_vector(), pCur4Zones));
		CursorIdUpdater::run(*logic.getGameDispatcher());
		return true;
	}

	return false;
}

bool ObjectStateTreeNode::deleteAnimatedObjectState(TreeLogic& logic, 
													bool ask)
{
	qdGameObjectAnimated* poa = static_cast<qdGameObjectAnimated*>(getObject()->owner());

	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	qdGameObjectState* state = static_cast<qdGameObjectState*>(getObject());
	TC_TE_PAIR_VECT_t lst4remove;
	NamedObjectList namedObjectList;
	if (canRemoveState(ask, 
				gameDisp, 
				state,
				&lst4remove, 
				&namedObjectList))
	{
		bool bHasGlobFlag = 
			StateTraits::hasGlobalFlag(static_cast<qdGameObjectState*>(getObject()));

		qdGameObject* pGlobObj = ObjectTraits::isCopyOfGlobal(gameDisp, poa);
		if (bHasGlobFlag) {

			//вот здесь удаляемое состояние, навсегда теряет 
			//флаг глобальности
			gameDisp->split_global_objects(pGlobObj);

			poa = static_cast<qdGameObjectAnimated*>(pGlobObj);
			bHasGlobFlag = true;
		}

		if (!lst4remove.empty()) 
			logic.getTriggerView()->clearUndobuffer();

		RemoveTriggerChainEles(lst4remove);

		bool res = false;
		if(poa->remove_state(state))
		{
			poa->set_default_state();
			res = true;
		}
		if (bHasGlobFlag)//нужна симметричность
		{
			gameDisp->merge_global_objects(pGlobObj);
			CopyOfGlobalUpdater::update(logic, pGlobObj);
		}
		ReinitCounters(gameDisp);
		return res;
	}
	else{
		if (!namedObjectList.empty()&&ask)
		{
			CString msg;
			msg.LoadString(IDS_STATE_IN_CONDITIONS);
			msg += _T("\n");
			msg += CString(reinterpret_cast<LPTSTR>(IDS_ASK_SHOW_OBJECTS));
			if (AfxMessageBox(msg, MB_ICONEXCLAMATION |MB_YESNO) == IDYES)
			{
				logic.getObjectList()->clearList();
				boost::rtl::crange<NamedObjectList const> r(namedObjectList);
				while (r) {
					qdNamedObjectReference ref(*r);
					logic.getObjectList()->addObjectToList(&ref);
					++r;
				}
				logic.getObjectList()->show();
			}
		}
	}
	return false;
}

bool ObjectStateTreeNode::preDelete(TreeLogic& logic, bool ask, bool statDepends)
{
	return __super::preDelete(logic, ask, statDepends);
}

/*!
 	Перерисовываем редактор триггеров, если он активен.
	Проверяем, если у объекта больше не осталось состояний, то удаляем и объект
	\param pno		- удаляемое состояние
	\param hItem	- узел состояния
	\return \a true - если hItem стал недействительным. Это могло случиться из-за 
					удаления родительского узла, либо из-за перезагрузки списка состояний
*/
bool ObjectStateTreeNode::postDeleteNotMouseState(TreeLogic& logic)
{
	resetDefaultStateIcon(logic);

	qdGameObjectAnimated* owner = 
		static_cast<qdGameObjectAnimated*>(getObject()->owner());
	
	if (!logic.isSceneViewActive()) 
		logic.getTriggerView()->Redraw();

	if(ObjectTraits::hasNoStates(owner))
	{
		bool bIsGlobal = ObjectTraits::isGlobal(*owner);
		ITreeNode* ownerNode = *logic.getObjectNodes(owner);

		//пытаемся остановить поток прокрутки анимации
		//потому что он может быть запущен в данном случае
		logic.getSceneView()->StopAnimation(static_cast<qdGameObjectState*>(getObject()));

		ITreeNode* prevVisible = logic.getPrevVisibleNode(ownerNode);
		if(ownerNode->deleteObject(logic, false, true))
		{
			logic.setFocusedNode(prevVisible);
			return true;			
		}
	}
	return false;
}


//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
// при его удаление(сотояния, когда удаляется анимированный объект. 
//Все объекты, когда удаляется сцена)
bool ObjectStateTreeNode::deleteAsSubObject(TreeLogic& logic)
{
	if (!getObject())
		return true;

	if (ObjectTraits::is_obj_mouse(getObject()->owner()))
	{
		if (deleteMouseState(logic))
		{
			delete getObject();
//			setObject(0);
			return true;
		}
	}
	else
	{
		if (deleteAnimatedObjectState(logic, false))
		{
			qdNamedObject* obj = getObject();
			delete obj;
			setObject(NULL);
			return true;
		}
	}
	return false;
}

//! Можно ли кинуть узел на тот, над которым тянем
DROPEFFECT ObjectStateTreeNode::canDrop(TreeLogic& logic, ITreeNode* overNode)
{
	qdGameObjectAnimated* parent = 
		static_cast<qdGameObjectAnimated*>(
			logic.getDraggedParentNode()->getContent().gameObject);
	if (StateTraits::isGlobalForObj(*getObject(), *parent))
		return cdGlobalState(logic, overNode);
	else
		return cdOwnState(logic, overNode);
	return DROPEFFECT_NONE;
}

/*!
	Позволяет только копировать состояние. Если не нажата VK_CONTROL, то выходим, иначе
	передаём управление ObjectStateTreeNode::cdOwnState()
	\param logic	- TreeLogic
	\param overNode	- Узел, на который хотим бросить
	\retval DROPEFFECT_COPY - можем скопировать
	\retval DROPEFFECT_NONE - нельзя скопировать
*/
DROPEFFECT ObjectStateTreeNode::cdGlobalState(TreeLogic& logic, ITreeNode* overNode)
{
	if (!ptree_helpers::IsKeyPressed(VK_CONTROL)) {
		logic.selectDropTarget(NULL);
		return DROPEFFECT_NONE;
	}

	return cdOwnState(logic, overNode);
}

/*!
	Так как у персонажа 2 каталога состояний, то приходится сравнивать тип состояния
	(походочное или нет) с типом каталога. Они должны совпадать
	\param logic	- TreeLogic
	\param overNode	- Узел, на который хотим бросить
	\retval DROPEFFECT_COPY - можем скопировать
	\retval DROPEFFECT_MOVE - можем перенести
	\retval DROPEFFECT_NONE - нельзя скопировать
*/
DROPEFFECT ObjectStateTreeNode::cdOwnStateUnderMoving(TreeLogic& logic,
													  ITreeNode* overNode,
												ITreeNode* animatedNode)
{
	if(!canDropUnderMoving(logic, animatedNode))
	{
		logic.selectDropTarget(NULL);
		return DROPEFFECT_NONE;
	}
	qdGameObjectAnimated* pobj = 
		static_cast<qdGameObjectAnimated*>(animatedNode->getContent().gameObject);

	ITreeNode* draggedNodeParent = logic.getDraggedParentNode();
	if (animatedNode != draggedNodeParent&&getObject()->ref_owner() == pobj) {
		logic.selectDropTarget(NULL);
		return DROPEFFECT_NONE;
	}

	bool bIsWalkCatalog = false;
	if (ITreeNode*stateCatalog = logic.isNodeUnderStateCatalog(overNode, &bIsWalkCatalog)) 
	{
		if (StateTraits::isWalkState(static_cast<qdGameObjectState*>(getObject())))
		{
			if (bIsWalkCatalog) {
				if (overNode == stateCatalog) {
					logic.selectDropTarget(overNode);
				}
				else {
					//значит у нас есть реальный объект состояние
					qdGameObjectState* pstate = 
						static_cast<qdGameObjectState*>(overNode->getContent().gameObject);

					//не можем кидать на глобальные состония
					if (StateTraits::isGlobalForObj(*pstate, *pobj)) {
						logic.selectDropTarget(stateCatalog);
					}
					else
						logic.selectDropTarget(overNode);
				}
				if (ptree_helpers::IsKeyPressed(VK_CONTROL)) {
					return DROPEFFECT_COPY;
				}
				return DROPEFFECT_MOVE;
			}
		}
		else if (!bIsWalkCatalog)
		{
			if (overNode == stateCatalog) 
			{
				logic.selectDropTarget(overNode);
			}
			else 
			{
				qdGameObjectState* pstate = 
					static_cast<qdGameObjectState*>(overNode->getContent().gameObject);

				//не можем кидать на глобальные состония
				if (StateTraits::isGlobalForObj(*pstate, *pobj)) {
					logic.selectDropTarget(stateCatalog);
				}
				else
					logic.selectDropTarget(overNode);
			}
			if (ptree_helpers::IsKeyPressed(VK_CONTROL)) {
				return DROPEFFECT_COPY;
			}
			return DROPEFFECT_MOVE;
		}
	}
	logic.selectDropTarget(NULL);
	return DROPEFFECT_NONE;
}

/*!
	Бросить можем только под динамический объект или персонаж. Нельзя перетащить 
	курсор мыши под объект сцены
	\param logic	- TreeLogic
	\param overNode	- Узел, на который хотим бросить
	\retval DROPEFFECT_COPY - можем скопировать
	\retval DROPEFFECT_MOVE - можем перенести
	\retval DROPEFFECT_NONE - нельзя скопировать
*/
DROPEFFECT ObjectStateTreeNode::cdOwnState(TreeLogic& logic, ITreeNode* overNode)
{
	if (overNode != this) {
		bool IsMovingObj = false;
		//кинуть можем только под анимированный объект
		//(Это в том числе и персонаж)
		ITreeNode *animated = logic.isNodeUnderAnimated(overNode, &IsMovingObj);
		if (animated) {

			if ((!IsMovingObj&&canDropUnderAnim(logic, animated))) {

				logic.selectDropTarget(overNode);

				if (ptree_helpers::IsKeyPressed(VK_CONTROL)) {
					return DROPEFFECT_COPY;
				}
				return DROPEFFECT_MOVE;
			}
			else if(IsMovingObj)
			{
				return cdOwnStateUnderMoving(logic, overNode, animated);
			}
		}
	}
	logic.selectDropTarget(NULL);
	return DROPEFFECT_NONE;
}

/*!
 	Нельзя кинуть только курсор.
	\param movingNode - Объект, под который пытаются бросить
	\retval true - если можно
*/
bool ObjectStateTreeNode::canDropUnderMoving(TreeLogic& logic, ITreeNode* movingNode)
{
	ITreeNode* draggedNodeParent = logic.getDraggedParentNode();
	if (movingNode != draggedNodeParent) 
	{
		qdGameObjectMoving* pa = 
			static_cast<qdGameObjectMoving*>(draggedNodeParent->getContent().gameObject);
		assert (!ObjectTraits::is_obj_mouse(pa));
	}
	return true;
}

/*!
 	Нельзя кинуть курсор и походочное состояние
	\param node - узел анимированного объекта
	\retval true - если можно
*/
bool ObjectStateTreeNode::canDropUnderAnim(TreeLogic& logic, ITreeNode* node)
{
	if (StateTraits::isWalkState(static_cast<qdGameObjectState*>(getObject())))
		return false;

	ITreeNode* draggedParentNode = logic.getDraggedParentNode();
	if (node != draggedParentNode) 
	{
		if (ObjectTraits::is_obj_mouse(draggedParentNode->getContent().gameObject)
			||ObjectTraits::is_obj_mouse(node->getContent().gameObject)) 
			return false;
	}
	return true;
}

/*!
 	Определяет под кого бросли и вызывает соответствующую функцию обработчик
*/

DROPEFFECT ObjectStateTreeNode::onDropData(TreeLogic& logic, 
									 ITreeNode* dropTarget, DROPEFFECT dropEffect)
{
	bool bIsMoving = false;

	ITreeNode* animated = logic.isNodeUnderAnimated(dropTarget, &bIsMoving);
	ASSERT(animated);

	BOOL res = DROPEFFECT_NONE;
	if (bIsMoving) 
		res = oddOwnStateMovingObj(logic, animated, dropTarget, dropEffect);
	else 
	{
		qdGameObjectAnimated* ptrAnimObj = 
			static_cast<qdGameObjectAnimated*>(animated->getContent().gameObject);
		if (ObjectTraits::is_obj_mouse(ptrAnimObj)) 
			res = oddOwnStateMouseObj(logic, animated, dropTarget, dropEffect);
		else
			res = oddOwnStateAnimObj(logic, animated, dropTarget, dropEffect);
	}
	return res;
}

/*!
	Меняет порядок состояний, переносит в другой объект или копирует состояния брошенные
	под мышь
	\param animated			- Узел объекта, под который бросли
	\param dropTarget		- Узел, на который бросили
	\param dropEffect		- Переностить или копировать
	\retval TRUE			- если операция закончилась успешно
*/
DROPEFFECT ObjectStateTreeNode::oddOwnStateMouseObj(TreeLogic& logic, 
											  ITreeNode* animated,
											  ITreeNode* dropTarget, 
											  DROPEFFECT dropEffect)
{
	qdGameObjectMouse* ptrMouse = 
		static_cast<qdGameObjectMouse*>(animated->getContent().gameObject);

	int iCurState = ptrMouse->cur_state();
	//если iCurState != 0, тогда ptrRestoreState будет восстановлено у объекта
	//иначе переустановим нулевое, каким бы оно не было
	qdGameObjectState* ptrRestoreState = ptrMouse->get_state(iCurState);

	ITreeNode* draggedNodeParent = logic.getDraggedParentNode();
	//вставляем в новое
	//в объект
	std::auto_ptr<qdGameObjectState> ptrInsState;
	//сдвигаем ли мы состояние. Т.е. меняем его местоположение внутри
	//того же объекта
	bool bShift = (dropEffect == DROPEFFECT_MOVE &&animated==draggedNodeParent);

	qdGameObjectState* dragged_state = static_cast<qdGameObjectState*>(getObject());

	//вставляем первым, если цель объект
	ITreeNode* insertAfter = TN_INSERT_FIRST;
	int iInsBefore = 0;

	//иначе вставляем после цель-состояния
	if (dropTarget != animated) {
		insertAfter = dropTarget;

		qdGameObjectState* ptrInsAfterState = 
			static_cast<qdGameObjectState*>(insertAfter->getContent().gameObject);
		iInsBefore = StateTraits::getStateIndex(ptrMouse->state_vector(), 
												ptrInsAfterState) + 1;
		if (bShift) {
			//если вставляем состояние в позицию после его текущей,
			//то надо уменьшить iInsBefore, потому что после удаления
			//все состояния, находящиеся после dragged_state, сдвинуться
			int state_pos = 
				StateTraits::getStateIndex(ptrMouse->state_vector(), dragged_state);
			if (iInsBefore - 1 > state_pos) 
				--iInsBefore;
		}
	}

	DROPEFFECT res = DROPEFFECT_MOVE;
	qdGameObjectState* insertedState = NULL;
	if (bShift) 
	{
		setObject(NULL);
		insertedState = logic.moveState(ptrMouse, ptrMouse, dragged_state, 
						animated, insertAfter, iInsBefore);
		logic.removeTreeNodeIndexEntry(reinterpret_cast<DWORD_PTR>(dragged_state), this);
	}
	else
	{
		res = DROPEFFECT_COPY;

		insertedState = logic.copyState(ptrMouse, ptrMouse, dragged_state, 
					animated, insertAfter, iInsBefore);
		insertedState->remove_all_conditions();
		if (dropEffect == DROPEFFECT_MOVE)
		{
			if (deleteAsSubObject(logic))
				res = DROPEFFECT_MOVE;
		}
	}

	if (iCurState) 
		ptrMouse->set_state(ptrRestoreState);
	else
		ptrMouse->set_default_state();


	CursorIdUpdater::run(*logic.getGameDispatcher());
	return res;
}


/*!
	Меняет порядок состояний, переносит в другой объект или копирует состояния брошенные
	под динамический объект(в дереве)
	\param animated		- Узел объекта, под который бросли
	\param dropTraget		- Узел, на который бросили
	\param dropEffect		- Переностить или копировать
	\retval TRUE			- если операция закончилась успешно
*/

DROPEFFECT ObjectStateTreeNode::oddOwnStateAnimObj(TreeLogic& logic, 
											  ITreeNode* animated,
											  ITreeNode* dropTarget, 
											  DROPEFFECT dropEffect)
{
	qdGameObjectAnimated* animObj = 
		static_cast<qdGameObjectAnimated*>(animated->getContent().gameObject);

	qdGameObjectAnimated* pDraggedStateOwner =
		static_cast<qdGameObjectAnimated*>(getObject()->ref_owner());

	int iCurState = animObj->cur_state();
	//если iCurState != 0, тогда ptrRestoreState будет восстановлено у объекта
	//иначе переустановим нулевое, каким бы оно не было
	qdGameObjectState* ptrRestoreState = animObj->get_state(iCurState);

	ITreeNode* draggedNodeParent = logic.getDraggedParentNode();
	//вставляем в новое
	//в объект
	std::auto_ptr<qdGameObjectState> ptrInsState;
	//сдвигаем ли мы состояние. Т.е. меняем его местоположение внутри
	//того же объекта
	bool bShift = (dropEffect == DROPEFFECT_MOVE &&animated==draggedNodeParent);

	qdGameObjectState* dragged_state = static_cast<qdGameObjectState*>(getObject());

	//вставляем первым, если цель объект
	ITreeNode* insertAfter = TN_INSERT_FIRST;
	int iInsBefore = 0;

	//иначе вставляем после цель-состояния
	if (dropTarget != animated) {
		insertAfter = dropTarget;

		qdGameObjectState* ptrInsAfterState = 
			static_cast<qdGameObjectState*>(insertAfter->getContent().gameObject);
		iInsBefore = StateTraits::getStateIndex(animObj->state_vector(), 
			ptrInsAfterState) + 1;
		if (bShift) {
			//если вставляем состояние в позицию после его текущей,
			//то надо уменьшить iInsBefore, потому что после удаления
			//все состояния, находящиеся после dragged_state, сдвинуться
			int state_pos = 
				StateTraits::getStateIndex(animObj->state_vector(), dragged_state);
			if (iInsBefore - 1 > state_pos) 
				--iInsBefore;
		}
	}

	DROPEFFECT res = DROPEFFECT_MOVE;
	qdGameObjectState* insertedState = NULL;
	if (bShift) 
	{
		setObject(NULL);
		insertedState = logic.moveState(pDraggedStateOwner, animObj, dragged_state, 
			animated, insertAfter, iInsBefore);
		logic.removeTreeNodeIndexEntry(reinterpret_cast<DWORD_PTR>(dragged_state), this);
	}
	else
	{
		res = DROPEFFECT_COPY;
		insertedState = logic.copyState(pDraggedStateOwner, animObj, dragged_state, 
			animated, insertAfter, iInsBefore);
		insertedState->remove_all_conditions();
		if (dropEffect == DROPEFFECT_MOVE)
		{
			if (!statObjectDependencies(logic, getObject()))
			{
				if (HasLastState(getObject()->owner()))
				{
					ITreeNode* ownerNode = *logic.getObjectNodes(getObject()->owner());
					assert(ownerNode);
					logic.setFocusedNode(ownerNode);
					if (ownerNode->deleteObject(logic, false, true))
					{
						return DROPEFFECT_COPY;
					}
				}
				else 
				{
					logic.setFocusedNode(logic.getPrevVisibleNode(this));
					if (deleteAsSubObject(logic))
						res = DROPEFFECT_MOVE;
				}
			}
		}
	}

	if (iCurState) 
		animObj->set_state(ptrRestoreState);
	else
		animObj->set_default_state();


	return res;
}
/*!
	Меняет порядок состояний, переносит в другой объект или копирует состояния брошенные
	под персонажа(в дереве)
	\param animated			- Узел объекта, под который бросли
	\param dropTrarget		- Узел, на который бросили
	\param dropEffect		- Переностить или копировать
	\retval TRUE			- если операция закончилась успешно
	\ramarks Непоходочные состояния могут свободно переноситься(копироваться) между 
	динамическими объектами и персонажами. Походочными состояниями могут обмениваться
	только персонажи
*/
DROPEFFECT ObjectStateTreeNode::oddOwnStateMovingObj(TreeLogic& logic, 
											   ITreeNode* animated,
											   ITreeNode* dropTarget, 
											   DROPEFFECT dropEffect)
{

	qdGameObjectMoving* movingObj = 
		static_cast<qdGameObjectMoving*>(animated->getContent().gameObject);

	qdGameObjectAnimated* pDraggedStateOwner =
		static_cast<qdGameObjectAnimated*>(getObject()->ref_owner());

	bool bIsWalkCatalog = false;
	ITreeNode* stateCatalog = logic.isNodeUnderStateCatalog(dropTarget, &bIsWalkCatalog);

	int iCurState = movingObj->cur_state();
	//если iCurState != 0, тогда ptrRestoreState будет восстановлено у объекта
	//иначе переустановим нулевое, каким бы оно не было
	qdGameObjectState* ptrRestoreState = movingObj->get_state(iCurState);

	ITreeNode* draggedNodeParent = logic.getDraggedParentNode();
	qdGameObjectState* dragged_state = static_cast<qdGameObjectState*>(getObject());

	//сдвигаем ли мы состояние. Т.е. меняем его местоположение внутри
	//того же объекта
	//сдвигаем ли мы состояние. Т.е. меняем его местоположение внутри
	//того же объекта
	bool bShift = (dropEffect == DROPEFFECT_MOVE&&
		animated == draggedNodeParent//внутри одного и того же объкта
		&&!StateTraits::isGlobalForObj(*dragged_state, *movingObj));


	//вставляем первым, если цель объект
	ITreeNode* insertAfter = TN_INSERT_FIRST;
	int iInsBefore = 0;

	//иначе вставляем после цель-состояния
	if (dropTarget != stateCatalog) {
		insertAfter = dropTarget;

		qdGameObjectState* ptrInsAfterState = 
			static_cast<qdGameObjectState*>(insertAfter->getContent().gameObject);

		iInsBefore = 
			StateTraits::getStateIndex(movingObj->state_vector(), ptrInsAfterState) + 1;
		if (bShift) {
			//если вставляем состояние в позицию после его текущей,
			//то надо уменьшить iInsBefore, потому что после удаления
			//все состояния, находящиеся после dragged_state, сдвинуться
			int state_pos = 
				StateTraits::getStateIndex(movingObj->state_vector(), dragged_state);
			if (iInsBefore - 1 > state_pos) 
				--iInsBefore;
		}
	}
	else if (!ObjectTraits::isGlobal(*movingObj)
		&&ObjectTraits::isCopyOfGlobal(logic.getGameDispatcher(), movingObj))
	{//раз копия значит у него есть глобальные состояния
		//находим первое неглобальное состоняние(походку или непоходку)
		qdGameObjectState* pFirstOwnState = NULL;
		if (bIsWalkCatalog) 
			pFirstOwnState = findFirstOwnWalk(movingObj);

		if (!pFirstOwnState) {
			//находим первое непоходочное и вставляем перед ним
			pFirstOwnState = findFirstOwnNotWalk(movingObj);
		}

		if (!pFirstOwnState) {
			//если нет своих или непоходчных состояний, 
			//тогда вставлем последним
			iInsBefore = movingObj->state_vector().size();
			insertAfter = TN_INSERT_LAST;
		}
		else
		{
			iInsBefore = 
				StateTraits::getStateIndex(movingObj->state_vector(), pFirstOwnState);

			ITreeNode* node = *logic.getObjectNodes(pFirstOwnState);
			insertAfter = 
				logic.getPrevSiblingNode(node);
			if (!insertAfter) {
				//значит не было у персонажа глобальных состояний
				//такого же типа как и здесь вставляемое.
				insertAfter = TN_INSERT_FIRST;
			}
		}
	}

	if (ObjectTraits::isGlobal(*movingObj)) 
		logic.getGameDispatcher()->split_global_objects(movingObj);

	qdGameObjectState* insertedState = NULL;
	DROPEFFECT res = DROPEFFECT_MOVE;
	if (bShift) 
	{
		setObject(NULL);
		insertedState = logic.moveState(pDraggedStateOwner, movingObj, dragged_state, 
			stateCatalog, insertAfter, iInsBefore);
		logic.removeTreeNodeIndexEntry(reinterpret_cast<DWORD_PTR>(dragged_state), this);
	}
	else
	{
		insertedState = logic.copyState(pDraggedStateOwner, movingObj, dragged_state, 
			stateCatalog, insertAfter, iInsBefore);
		insertedState->remove_all_conditions();

		res = DROPEFFECT_COPY;

		if (dropEffect == DROPEFFECT_MOVE)
		{
			if (!statObjectDependencies(logic, getObject()))
			{
				if (HasLastState(getObject()->ref_owner())
					&&!ObjectTraits::isGlobal(*getObject()->owner()))
				{
					ITreeNode* ownerNode = *logic.getObjectNodes(getObject()->owner());
					assert(ownerNode);
					logic.setFocusedNode(ownerNode);
					if (ownerNode->deleteObject(logic, false, true))
					{
						return DROPEFFECT_COPY;
					}
				}
				else
				{
					logic.setFocusedNode(logic.getPrevVisibleNode(this));
					if (deleteAsSubObject(logic))
					{
						res = DROPEFFECT_MOVE;
					}
				}
			}
		}
	}

	if (ObjectTraits::isGlobal(*movingObj)) 
		logic.getGameDispatcher()->merge_global_objects(movingObj);

	if (iCurState) 
		movingObj->set_state(ptrRestoreState);
	else
		movingObj->set_default_state();

	if (ObjectTraits::isGlobal(*movingObj)) 
		CopyOfGlobalUpdater::update(logic, movingObj);

	return res;

}

bool ObjectStateTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName)
{
	return uniq_name(newName,
		static_cast<qdGameObjectAnimated*>(getObject()->owner())->state_vector());
}
//! Возвращает true, если удалось переимновать, false в противном случае
bool ObjectStateTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;
	getObject()->set_name(newName);
	if (getObject()->is_in_triggers()){
		UpdateTriggerElementTitles::update(
			logic.getGameDispatcher()->trigger_chain_list(),getObject());
		logic.getTriggerView()->Redraw();
	}
	TreeLogic::Nodes nodes = logic.getObjectNodes(getObject());
	for (;nodes; ++nodes)
	{
		logic.setNodeText(*nodes, newName);
	}
	return true;
}
//! Можно ли сменить имя узлу
bool ObjectStateTreeNode::canRename(TreeLogic& logic)
{
	if (StateTraits::hasGlobalFlag(static_cast<qdGameObjectState*>(getObject()))
		&&logic.isNodeUnderScene(this))
	{
		return false;
	}
	return true;
}

void* ObjectStateTreeNode::operator new(size_t size)
{
	return boost::fast_pool_allocator<ObjectStateTreeNode>::allocate(1);
}
void ObjectStateTreeNode::operator delete(void* p)
{
	boost::fast_pool_allocator<ObjectStateTreeNode>::deallocate((ObjectStateTreeNode*)p);
}

void* ObjectStateTreeNode::operator new(size_t size, LPCTSTR file, unsigned line)
{
	return ObjectStateTreeNode::operator new(size);
}
void ObjectStateTreeNode::operator delete(void* p, LPCTSTR file, unsigned line)
{
	return ObjectStateTreeNode::operator delete(p);

}

//! Функция для рекурсивного обхода при смене иконки состояния по-умолчанию
void resetDefaultStateIconRecur(TreeLogic& logic, ITreeNode* child)
{
	while (child)
	{
		if (CC_NAMED_OBJECT == child->getContentClassificator())
		{
			qdGameObjectState* state = dynamic_cast<qdGameObjectState*>(child->getContent().gameObject);
			// Постулируется, что дефолтный объект не в триггерах. Если ситуация изменится, 
			// то мы уже не сможем снимать или замещать значки - на объекты в триггерах
			// вешаются свои значки, не допускающие бесконтрольного вмешательства.
			if (state && !state->is_in_triggers())
			{
				if (state->is_default())
					logic.getTreeListCtrl().SetItemStateImage(child->getTreeListItem(), II_DEFAULT_STATE);
				else
					// Убираем картинку (0)
					logic.getTreeListCtrl().SetItemStateImage(child->getTreeListItem(), 0);
			}

		}
		resetDefaultStateIconRecur(logic, logic.getChildNode(child));
		child = logic.getNextSiblingNode(child);
	}
}

//! Найти и установить новую дефолтную иконку, учитывая все соседние состояния
void ObjectStateTreeNode::resetDefaultStateIcon(TreeLogic& logic)
{
	ITreeNode* cur_parent = logic.getParentNode(this);
	// Ищем объект, которому принадлежит состояние
	while (cur_parent)
	{
		// -> Если текущий родитель - объект, значит нашли того, состояния которого будем править
		if (CC_NAMED_OBJECT == cur_parent->getContentClassificator())
			break;
		ITreeNode* up_parent = logic.getParentNode(cur_parent);
		// -> Если родитель текущего не существует, то нужным нам родителем считаем текущего
		if (!up_parent) 
			break;
		cur_parent = up_parent;
	}
	// Рекурсивно обходим всех детей найденного узла, если родитель найден
	if (cur_parent)
		resetDefaultStateIconRecur(logic, logic.getChildNode(cur_parent));
}
