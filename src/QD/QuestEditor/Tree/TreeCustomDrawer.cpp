#include "stdafx.h"
#include ".\treecustomdrawer.h"

#include "SettingDefs.h"
#include "Tree/NodeTraits.h"
#include "Tree/ITreeNode.h"

#include "qd_game_object_state.h"

TreeCustomDrawer::TreeCustomDrawer()
: oldFont_(NULL)
{
}
TreeCustomDrawer::~TreeCustomDrawer(void)
{
}

void TreeCustomDrawer::selectStateItemBack(qdGameObjectState* pstate, 
										   ITEM_CUSTOM_DRAW_INFO* pcdi)
{
	if (pstate->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_HIDE_OBJECT)) 
		pcdi->clrTextBk = DEFAULT_VALUE(STATE_FLAG_HIDE_OBJECT_COLOR);
	else if (pstate->has_conditions()) 
	{
		const qdCondition* pc = pstate->get_condition();
		if (pc->is_inversed())
		{
			pcdi->clrTextBk = DEFAULT_VALUE(INVERSE_CONDITION_COLOR);
			return;
		}

		switch(pc->type()) {
			//! клик мышью по объекту
		case qdCondition::CONDITION_MOUSE_CLICK:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_MOUSE_CLICK_COLOR);
			break;
			//! клик объектом на мыши по объекту
		case qdCondition::CONDITION_MOUSE_OBJECT_CLICK:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_MOUSE_OBJECT_CLICK_COLOR);
			break;
			//! персонаж или объект в определенном месте сетки
		case qdCondition::CONDITION_OBJECT_IN_ZONE:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_OBJECT_IN_ZONE_COLOR);
			break;
			//! персонаж движется в определенном направлении
		case qdCondition::CONDITION_PERSONAGE_WALK_DIRECTION:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_PERSONAGE_WALK_DIRECTION_COLOR);
			break;
			//! персонаж стоит и ориентирован в определенном направлении
		case qdCondition::CONDITION_PERSONAGE_STATIC_DIRECTION:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_PERSONAGE_STATIC_DIRECTION_COLOR);
			break;
			//! условие выпоняется с некоторой периодичностью
		case qdCondition::CONDITION_TIMER:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_TIMER_COLOR);
			break;
			//! клик мышью по фразе в диалоге
		case qdCondition::CONDITION_MOUSE_DIALOG_CLICK:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_MOUSE_DIALOG_CLICK_COLOR);
			break;
			//! мини-игра находится в определенном состоянии
		case qdCondition::CONDITION_MINIGAME_STATE:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_MINIGAME_IN_STATE_COLOR);
			break;
			//! объект находится в определенном состоянии
		case qdCondition::CONDITION_OBJECT_STATE:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_OBJECT_STATE_COLOR);
			break;
			//! клик мышью по зоне на сетке сцены
		case qdCondition::CONDITION_MOUSE_ZONE_CLICK:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_MOUSE_ZONE_CLICK_COLOR);
			break;
		case qdCondition::CONDITION_MOUSE_OBJECT_ZONE_CLICK:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_MOUSE_OBJECT_ZONE_CLICK_COLOR);
			break;
		case qdCondition::CONDITION_OBJECT_STATE_WAS_ACTIVATED:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_OBJECT_STATE_WAS_ACTIVATED_COLOR);
			break;
		case qdCondition::CONDITION_OBJECTS_DISTANCE:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_OBJECTS_DISTANCE_COLOR);
			break;
		case qdCondition::CONDITION_PERSONAGE_ACTIVE:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_PERSONAGE_ACTIVE_COLOR);
			break;
		case qdCondition::CONDITION_OBJECT_STATE_WAITING:
			pcdi->clrTextBk = DEFAULT_VALUE(COND_OBJECT_STATE_WAITING_COLOR);
			break;
		}
	}

}

void TreeCustomDrawer::selectItemBack(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi)
{
	if (!(HFONT)font_)
	{
		CFont* font = source.GetFont();
		LOGFONT lf;
		font->GetLogFont(&lf);
		lf.lfWeight = FW_BOLD;
		font_.CreateFontIndirect(&lf);
	}
	ITreeNode* node = reinterpret_cast<ITreeNode*>(source.GetItemData(pcdi->item));

	if (!node->getContent().gameObject)
		return;
	pcdi->useBackgrndClr = true;
	if (TreeNodeTraits::isSceneNode(node)) {
		//		pcdi->clrText = RGB(255, 0, 0);
		pcdi->clrText = RGB(9, 115, 7);
	}
	else if (TreeNodeTraits::isInterfaceRoot(node))
		pcdi->clrText = RGB(255, 0, 0);
	else if (TreeNodeTraits::isContainerType(node, QD_NAMED_OBJECT_VIDEO)) 
		pcdi->clrText = RGB(255, 0, 0);
	else if (TreeNodeTraits::isContainerType(node, QD_NAMED_OBJECT_TRIGGER_CHAIN)) 
		pcdi->clrText = RGB(255, 0, 0);
	else if (TreeNodeTraits::isContainerType(node, QD_NAMED_OBJECT_INVENTORY)) 
		pcdi->clrText = RGB(255, 0, 0);
	else if (TreeNodeTraits::isContainerType(node, QD_NAMED_OBJECT_MINIGAME)) 
		pcdi->clrText = RGB(255, 0, 0);
	else if (TreeNodeTraits::isNodeType(node, QD_NAMED_OBJECT_MOUSE_OBJ)) 
		pcdi->clrText = RGB(255, 0, 0);
	else if (TreeNodeTraits::isNodeType(node, QD_NAMED_OBJECT_DISPATCHER)) 
		pcdi->clrText = RGB(255, 0, 0);
	else if (TreeNodeTraits::isContainerType(node, QD_NAMED_OBJECT_MOVING_OBJ)
		//только для узла глобальных персонажей
		&&!source.GetParentItem(source.GetParentItem(pcdi->item))) 
		pcdi->clrText = RGB(255, 0, 0);
	else if (TreeNodeTraits::isContainerType(node, QD_NAMED_OBJECT_MUSIC_TRACK)) 
		pcdi->clrText = RGB(0, 0, 255);
	else if (TreeNodeTraits::isNet(node)) 
		pcdi->clrText = RGB(0, 0, 255);
	else if (TreeNodeTraits::isNodeType(node, QD_NAMED_OBJECT_MOVING_OBJ)) 
		pcdi->clrText = RGB(0, 0, 255);
	else if (TreeNodeTraits::isStateNode(node)) 
	{
		qdGameObjectState* pstate = 
			static_cast<qdGameObjectState*>(node->getContent().gameObject);
		selectStateItemBack(pstate, pcdi);
	}
	if (TreeNodeTraits::isGameNode(node)
		&&node->getContent().gameObject->is_in_triggers())
	{
		oldFont_ = SelectObject(pcdi->hdc, font_);
	}
}



DWORD TreeCustomDrawer::onItemPrepaint(CTreeListCtrl& source, 
											   ITEM_CUSTOM_DRAW_INFO* pcdi)
{
	selectItemBack(source, pcdi);
	return CDRF_DODEFAULT;
}

DWORD TreeCustomDrawer::onItemPostpaint(CTreeListCtrl& source, 
										ITEM_CUSTOM_DRAW_INFO* pcdi)
{
	ITreeNode* node = reinterpret_cast<ITreeNode*>(source.GetItemData(pcdi->item));
	if (TreeNodeTraits::isGameNode(node)
		&&node->getContent().gameObject->is_in_triggers())
	{
		oldFont_ = SelectObject(pcdi->hdc, oldFont_);
		oldFont_ = NULL;
	}
	return CDRF_DODEFAULT;
}
