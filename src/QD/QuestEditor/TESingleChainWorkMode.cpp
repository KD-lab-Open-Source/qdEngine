#include "stdafx.h"
#include "resource.h"

#include "qd_trigger_element.h"
#include "qd_trigger_chain.h"

#include "tesinglechainworkmode.h"
#include "TEWnd.h"
#include "ptree_api.h"



void SetProjChange();
TESingleChainWorkMode::TESingleChainWorkMode(SelectionManager& selectionManager):
 m_ptrChain(NULL)
, selectionManager_(selectionManager)
{
}

TESingleChainWorkMode::~TESingleChainWorkMode(void)
{
}

void TESingleChainWorkMode::setBaseElement(qdTriggerElementPtr const& element){
	baseElement_ = element;
}
qdTriggerElementPtr const& TESingleChainWorkMode::getBaseElement() const{
	return baseElement_;
}

void TESingleChainWorkMode::setChain(qdTriggerChain* pchain){
	if (pchain != m_ptrChain)
	{
		getSelectionManager().deselectAll();
	}
	m_ptrChain = pchain;
}

SelectionManager const& TESingleChainWorkMode::getSelectionManager() const{
	return selectionManager_;
}
SelectionManager& TESingleChainWorkMode::getSelectionManager(){
	return selectionManager_;
}

BOOL TESingleChainWorkMode::OnCommand(TEWnd* pwnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam)) {
	case IDC_TE_SHOW_CONDITION:
		OnTeShowCondition(pwnd);
		break;
	case IDC_TE_PROPETIES:
		OnTeProperties(pwnd);
		break;
	default:
		return TEWorkModeBase::OnCommand(pwnd, wParam, lParam);
	}
	return TRUE;
}

void TESingleChainWorkMode::OnTeProperties(TEWnd* pwnd) 
{
	qdTriggerElementPtr const element = getSelectionManager().getLast();
	getSelectionManager().deselectAll();
	getSelectionManager().select(element);

	pwnd->InvalidateRect(NULL);

	if(ptree_api::ShowProperties(element->object()))
		SetProjChange();
	pwnd->InvalidateRect(NULL);
}

void TESingleChainWorkMode::OnTeShowCondition(TEWnd* pwnd) 
{
	qdTriggerElementPtr const element = getSelectionManager().getLast();
	getSelectionManager().deselectAll();
	getSelectionManager().select(element);

	pwnd->InvalidateRect(NULL);

	if(ptree_api::ShowCondDlg(element->object()))
		SetProjChange();

	pwnd->InvalidateRect(NULL);
}

void TESingleChainWorkMode::trackToolTip(TEWnd* pwnd, CPoint point)
{
	static qdTriggerElementConstPtr pred = NULL;

	CPoint scr_p(point);
	pwnd->scr2log(&point);
	qdTriggerElementConstPtr const pe = pwnd->find_te(point);

	if (pe != pred){ 
		if (pe)
		{
			static CString strText;
			strText = pe->title().c_str();
			if (pe->ID() == qdTriggerElement::ROOT_ID) 
			{
				strText += _T(" - ");
				//подписываем элементы, только тогда когда 
				//показывается конткретный триггер
				strText += getChain()->name();
			}
			pwnd->setToolTipText(strText);
		}
		else
			pwnd->setToolTipText(CString());
		pred = pe;
	}
	ClientToScreen(*pwnd, &scr_p);
	pwnd->trackToolTip(scr_p);
}

void TESingleChainWorkMode::OnShowWindow(TEWnd* pwnd, BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		CPoint scr_p;
		GetCursorPos(&scr_p);
		trackToolTip(pwnd, scr_p);	
	}
	else
		pwnd->setToolTipText(CString());
}
