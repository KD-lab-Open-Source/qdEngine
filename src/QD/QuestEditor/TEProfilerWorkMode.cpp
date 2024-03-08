#include "stdafx.h"
#include "resource.h"
#include "teprofilerworkmode.h"
#include "IDrawData.h"
#include "ITEDrawer.h"
#include "ITriggerProfList.h"
#include "qd_trigger_chain.h"
#include "ptree_api.h"
#include "menu_helpers.h"
#include "TEWnd.h"

TEProfilerWorkMode::TEProfilerWorkMode(SelectionManager& selectionManger)
:TESingleChainWorkMode(selectionManger)
, m_ptrDrawer(NULL)
, m_ptrDrawDataKeeper(new DataKeeper(NULL))
, m_ptrTriggerProfList(NULL)
{

}

TEProfilerWorkMode::~TEProfilerWorkMode(void)
{
}

void TEProfilerWorkMode::setDrawer(ITEDrawer* ptrDrawer){
	m_ptrDrawer = ptrDrawer;
}
void TEProfilerWorkMode::setTriggerProfList(ITriggerProfList* ptrtpl){
	m_ptrTriggerProfList = ptrtpl;
}

void TEProfilerWorkMode::OnPaint(class TEWnd* pwnd, CDC* pdc, CRect const& rcPaint)
{
	m_ptrDrawDataKeeper->setTriggerChain(getChain());
	m_ptrDrawer->draw(pdc, pwnd, m_ptrDrawDataKeeper.get(), rcPaint);
}

/// Показывает и активирует список отладочных записей
bool TEProfilerWorkMode::OnStart()
{
	ASSERT(m_ptrTriggerProfList);
	m_ptrTriggerProfList->Show(ITriggerProfList::TPL_SHOW);
	return m_ptrTriggerProfList->Activate();
}
/// Скрывает список отладочных записей
bool TEProfilerWorkMode::OnEnd()
{
	m_ptrTriggerProfList->Show(ITriggerProfList::TPL_HIDE);
	getChain()->init_debug_check();
	return true;
}

/**
	При нажатии на клавиши вверх/вниз перемещает текущую запись на предыдущую/следующую
 */
void TEProfilerWorkMode::OnKeyUp(TEWnd *pwnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_UP)
	{
		if (!m_ptrTriggerProfList->Prev())
			Beep(1000, 100);
	}
	else if (nChar == VK_DOWN)
	{
		if (!m_ptrTriggerProfList->Next())
			Beep(1000, 100);
	}

	TESingleChainWorkMode::OnKeyUp(pwnd, nChar, nRepCnt, nFlags);
}

/// Показывает диалог условий для объекта, содержащегося в элементе
void TEProfilerWorkMode::OnLButtonDblClk(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	pwnd->scr2log(&point);
	qdTriggerElementPtr const ptrEle = pwnd->find_te(point);	
	if (ptrEle&&ptrEle->object()
		&&ptrEle->object()->named_object_type()!= QD_NAMED_OBJECT_SCENE) 
	{
		if(ptree_api::ShowCondDlg(ptrEle->object()))
			pwnd->Invalidate();
	}
}

/**
	Если попали в элемент, показывает всплывающее меню
 */
void TEProfilerWorkMode::OnRButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	CPoint p(point);
	pwnd->scr2log(&p);
	getSelectionManager().deselectAll();
	qdTriggerElementPtr const element = pwnd->find_te(p);
	
	if (element)
	{
		getSelectionManager().select(element);
		pwnd->Invalidate();
		trackPopupMenu(IDR_TE_TRIGGER_ELE1, pwnd);
	}
	pwnd->Invalidate();
}

/**
	Управляет доступностью элеметов меню
 */
void TEProfilerWorkMode::ModifyMenu(CMenu* pmenu, UINT menuResId, TEWnd* pwnd)
{
	if (menuResId == IDR_TE_TRIGGER_ELE1)
	{
		if (qdTriggerElementPtr const element = getSelectionManager().getLast()) 
		{
			if(!element->object())
			{
				menu_h::disableMenuItem(pmenu, IDC_TE_SHOW_CONDITION);
				menu_h::disableMenuItem(pmenu, IDC_TE_PROPETIES);
			}
		}
	}
}

/**
	Либо скроллирует контент. Либо показывает в дереве объект содержащийся в элементе
 */
void TEProfilerWorkMode::OnLButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	if (getCanDragContent())
	{
		TESingleChainWorkMode::OnLButtonDown(pwnd, nFlags, point);
		return;
	}
	pwnd->scr2log(&point);
	qdTriggerElementPtr const ptrEle = pwnd->find_te(point);	
	if (ptrEle&&ptrEle->object()) 
	{
		if(ptree_api::ShowObjectInTree(ptrEle->object()))
			pwnd->Invalidate();
	}
}
