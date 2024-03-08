#include "stdafx.h"
#include "resource.h"
#include "condsmgr.h"
#include "qd_conditional_object.h"
#include "qd_condition.h"
#include "condition.h"

#include "conddlg_oper.h"

#include <boost/bind.hpp>
#include <boost/function_output_iterator.hpp>

CondsMgr::CondsMgr(CMyTree& tree):m_tree(tree)
{
	m_bChanged = false;
}
void CondsMgr::set_obj(qdConditionalObject* pobj){
	m_ptrObj = pobj;
}

CondsMgr::~CondsMgr()
{
}

bool CondsMgr::IsGroup(HTREEITEM hItem) const
{
	return !m_tree.GetItemData(hItem);
}

void CondsMgr::CreateGroups(qdConditionGroup::conditions_mode_t mode)
{
	HTREEITEM hItem = m_tree.GetRootItem();
	HTREEITEM hPrev = NULL;
	while (hItem) {
		if (!IsGroup(hItem))
		{
			HTREEITEM hGroup = CreateGroup(mode);
			m_tree.MoveItemTo(hItem, hGroup);
			m_tree.Expand(hGroup, TVE_EXPAND);
			if (!hPrev)
				hPrev = m_tree.GetRootItem();
		}
		else
			hPrev = hItem;
		hItem = m_tree.GetNextSiblingItem(hPrev);
	}
}

CString CondsMgr::GetGroupCaption(qdConditionGroup::conditions_mode_t mode)
{
	if (mode == qdConditionGroup::CONDITIONS_OR)
		return CString(reinterpret_cast<LPTSTR>(IDS_OR_GROUP_NAME));
	else if (mode == qdConditionGroup::CONDITIONS_AND)
		return CString(reinterpret_cast<LPTSTR>(IDS_AND_GROUP_NAME));
	ASSERT(0);
	return CString();
}

HTREEITEM CondsMgr::CreateGroup(qdConditionGroup::conditions_mode_t mode)
{
	boost::shared_ptr<qdConditionGroup> p(new qdConditionGroup(mode));
	HTREEITEM h = m_tree.InsertItem(GetGroupCaption(mode), TVI_ROOT, TVI_FIRST);
	m_tree.Expand(h, TVE_EXPAND);
	m_group_items.insert(GroupItems_t::value_type(h, p));

	m_bChanged = true;
	return h;
}

void CondsMgr::InvertType(qdConditionGroup& cg)
{
	if (cg.conditions_mode() == qdConditionGroup::CONDITIONS_AND)
		cg.set_conditions_mode(qdConditionGroup::CONDITIONS_OR);
	else if (cg.conditions_mode() == qdConditionGroup::CONDITIONS_OR)
		cg.set_conditions_mode(qdConditionGroup::CONDITIONS_AND);
}
void CondsMgr::InvertType(HTREEITEM hItem)
{
	GroupItems_t::iterator res = m_group_items.find(hItem);
	if (res != m_group_items.end())
	{
		qdConditionGroup& cg = *res->second;
		InvertType(cg);
		m_tree.SetItemText(res->first, GetGroupCaption(cg.conditions_mode()));
	}
	m_bChanged = true;
}

void CondsMgr::InvertType()
{
	const HTREEITEM * psel = m_tree.GetSelected();
	if (!psel)
		return;
	const HTREEITEM* end = psel + m_tree.GetSelectedCount();

	typedef void (CondsMgr::*Inv_type)(HTREEITEM);
	std::for_each(psel, end, 
		boost::bind(static_cast<Inv_type>(CondsMgr::InvertType), this, _1));
}

LRESULT CondsMgr::OnCanDropReq(NM_MT_CAN_DROP* pcd)
{
	if (!pcd->itemDest.hItem||IsGroup(pcd->itemDest.hItem))
	{
		TVITEM* pti = pcd->pItem;
		for(; pti != pcd->pItem + pcd->item_count; ++pti)
		{
			if (IsGroup(pti->hItem))
				return 1;
		}
		return 0;
	}
	return 1;
}

LRESULT CondsMgr::OnCanDragReq(NM_MT_CAN_DRAG* pcd)
{
	TVITEM* pti = pcd->pItem;
	for(; pti != pcd->pItem + pcd->item_count; ++pti)
	{
		if (IsGroup(pti->hItem))
			return 1;
	}
	return 0;
}

LRESULT CondsMgr::OnDropReq(NM_MT_DROP* pmd)
{
	TVITEM * items = pmd->pItem;

	if (pmd->btn == MK_RBUTTON)
		return 0;

	for(;items != pmd->pItem + pmd->item_count;++items)
		m_tree.MoveItemTo(items->hItem, pmd->itemDest.hItem);
	if (pmd->itemDest.hItem)
		m_tree.Expand(pmd->itemDest.hItem, TVE_EXPAND);

	m_bChanged = true;
	return 0;
}

void CondsMgr::ReadGroupsFromTree()
{
	HTREEITEM hItem = m_tree.GetRootItem();
	if (!hItem)
		return;
	do {
		if (IsGroup(hItem))
			ReadGroupFromTree(hItem);
	} while(hItem = m_tree.GetNextSiblingItem(hItem));
}

void CondsMgr::ReadGroupFromTree(HTREEITEM hItem)
{
	GroupItems_t::iterator itr = m_group_items.find(hItem);
	ASSERT(itr != m_group_items.end());
	boost::shared_ptr<qdConditionGroup> pcg = itr->second;
	pcg->remove_all();
	HTREEITEM hChild = m_tree.GetChildItem(hItem);
	if (hChild)
		do {
			qdCondition* const p = 
				reinterpret_cast<qdCondition*>(m_tree.GetItemData(hChild));
			pcg->add_condition(GetCondIndex(p));
		} while(hChild = m_tree.GetNextSiblingItem(hChild));
}

int CondsMgr::GetCondIndex(const qdCondition* p)
{
	Conditions::iterator i = m_conditions.begin(), 
						 e = m_conditions.end();
	for(; i != e; ++i)
		if (i->get()==p)
			break;
	ASSERT(i != e);
	return std::distance(m_conditions.begin(), i);
}

void CondsMgr::TreeToCO()
{
	ReadGroupsFromTree();
	m_ptrObj->remove_all_conditions();
	m_ptrObj->remove_all_groups();
	
	std::copy(m_conditions.begin(), m_conditions.end(),
		boost::make_function_output_iterator(add_cond(m_ptrObj)));

	std::copy(m_group_items.begin(), m_group_items.end(),
		boost::make_function_output_iterator(add_group(m_ptrObj)));
}

void CondsMgr::RemoveFromGroup()
{
	int const cnt = m_tree.GetSelectedCount();
	const HTREEITEM *pSel = m_tree.GetSelected();
	if (!pSel)
		return;
	//делаем копию, потому что при вызове некоторых методов
	//внутренний список выбранных элементов(на который и возвращен указатель), 
	//изменяется
	std::vector<HTREEITEM> tmp(pSel, pSel + cnt);
//	std::vector<HTREEITEM>::iterator i = tmp.begin(), e = tmp.end();
//	for(; i != e; ++i)
//		m_tree.MoveItemTo(*i, TVI_ROOT);
	std::for_each(tmp.begin(), tmp.end(), 
		boost::bind(CMyTree::MoveItemTo,&m_tree, _1, TVI_ROOT));
	if (!tmp.empty())
		m_bChanged = true;
}

LRESULT CondsMgr::OnNMRclickTree(CWnd* from)
{
	POINT p;
	GetCursorPos(&p);
	CMenu menu;
	menu.LoadMenu(IDR_CO_DLG_MENU);
	const HTREEITEM * pSel = m_tree.GetSelected();
	if (!pSel||!m_tree.GetParentItem(*pSel))
	{
		menu.GetSubMenu(0)->DeleteMenu(IDC_REMOVE_FROM_GROUP, MF_BYCOMMAND);	
		menu.GetSubMenu(0)->DeleteMenu(
			menu.GetSubMenu(0)->GetMenuItemCount()-1, MF_BYPOSITION);	
	}
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTBUTTON, p.x, p.y, from);
	return 0;
}

void CondsMgr::ConditionToTree(const qdCondition* pcond, 
							   HTREEITEM hParent,
							   HTREEITEM hInsertAfter)
{
	m_conditions.push_back(boost::shared_ptr<qdCondition>((new qdCondition(*pcond))));
	qdCondition* const p = m_conditions.back().get();
	HTREEITEM hres = m_tree.InsertItem(GetConditionText(p), hParent, hInsertAfter);
	if (hres)
		m_tree.SetItemData(hres, reinterpret_cast<DWORD_PTR>(p));
}

bool CondsMgr::LoadGroups()
{
	CString strAndName(GetGroupCaption(qdConditionGroup::CONDITIONS_AND));
	CString strOrName(GetGroupCaption(qdConditionGroup::CONDITIONS_OR));
	int const count = m_ptrObj->condition_groups_count();
	for(int i = 0; i < count; ++i)
	{
		boost::shared_ptr<qdConditionGroup> p(
			new qdConditionGroup(*m_ptrObj->get_condition_group(i)));
		if (!p)
			return false;

		HTREEITEM hRes = NULL;
		if (p->conditions_mode() == qdConditionGroup::CONDITIONS_AND)
			hRes = m_tree.InsertItem(strAndName, TVI_ROOT, TVI_FIRST);
		else if (p->conditions_mode() == qdConditionGroup::CONDITIONS_OR)
			hRes = m_tree.InsertItem(strOrName, TVI_ROOT, TVI_FIRST);
		ASSERT(hRes);
		m_tree.Expand(hRes, TVE_EXPAND);
		m_group_items.insert(GroupItems_t::value_type(hRes, p));
	}

	return true;
}

void CondsMgr::CondsToTree()
{
	int const count = m_ptrObj->conditions_count();
	m_conditions.reserve(count);
	for(int i = 0; i < count; ++i)
	{
		HTREEITEM hRoot = TVI_ROOT;
		if (m_ptrObj->is_condition_in_group(i))
			hRoot = FindCondGroup(i);
		ConditionToTree(m_ptrObj->get_condition(i), hRoot);
		if (hRoot != TVI_ROOT)
			m_tree.Expand(hRoot, TVE_EXPAND);
	}
}

HTREEITEM CondsMgr::FindCondGroup(int iCondNum)
{
	GroupItems_t::const_iterator i = m_group_items.begin(),
								 e = m_group_items.end();
	for(; i != e; ++i)
	{
		const qdConditionGroup& cg = *i->second;
		qdConditionGroup::conditions_iterator_t c = 
			std::find(cg.conditions_begin(), cg.conditions_end(), iCondNum);
		if (c != cg.conditions_end())
			return i->first;
	}
	ASSERT(0);
	return NULL;
}

void CondsMgr::DeleteSelected()
{
	size_t sel_count = m_tree.GetSelectedCount();
	const HTREEITEM *pSel = m_tree.GetSelected();
	if (!pSel)
		return;
	if (MessageBox(m_tree.m_hWnd, 
		CString((LPTSTR)IDS_DEL_CONDITION), CString((LPTSTR)IDS_DEL_CAP),
		MB_OKCANCEL) == IDCANCEL)
		return;
	std::vector<HTREEITEM> tmp(pSel, pSel + sel_count);
	std::vector<HTREEITEM>::const_iterator i = tmp.begin(), e = tmp.end();
	for(; i != e;++i)
	{
		if (IsGroup(*i))
			DeleteGroup(*i);
		else
			DeleteCondition(*i);
		m_tree.DeleteItem(*i);
	}
	m_bChanged = true;
}

void CondsMgr::DeleteGroup(HTREEITEM hItem)
{
	GroupItems_t::iterator i = m_group_items.find(hItem);
	ASSERT(i != m_group_items.end());
	m_tree.MoveChildItemTo(hItem, TVI_ROOT);
	m_group_items.erase(i);
	m_bChanged = true;
}

void CondsMgr::DeleteCondition(HTREEITEM hItem)
{
	qdCondition* const p = 
		reinterpret_cast<qdCondition*>(m_tree.GetItemData(hItem));

	Conditions::iterator i = m_conditions.begin(), e = m_conditions.end();
	for(; i != e; ++i)
		if (i->get()==p)
		{
			m_conditions.erase(i);
			break;
		}
		m_bChanged = true;
}

void CondsMgr::DeleteAll()
{
	if (MessageBox(m_tree.m_hWnd,
		CString(reinterpret_cast<LPTSTR>(IDS_ASK_REMOVE_ALL)), 
		CString(reinterpret_cast<LPTSTR>(IDS_DEL_CAP)), 
		MB_YESNO) == IDYES)
	{
		m_bChanged = m_bChanged||m_tree.GetCount() != 0;
		m_tree.DeleteAllItems();
		m_conditions.clear();
		m_group_items.clear();
	}
}

bool CondsMgr::AddCondition( qdCondition const* pcond)
{
	if (!pcond)
		return false;
	int const sel_count = m_tree.GetSelectedCount();
	const HTREEITEM *hSel = m_tree.GetSelected();
	HTREEITEM hRoot = TVI_ROOT;
	if (hSel&&IsGroup(*hSel))
		hRoot = *hSel;
	ConditionToTree(pcond, hRoot, TVI_FIRST);
	if (hRoot != TVI_ROOT)
		m_tree.Expand(hRoot, TVE_EXPAND);
	m_tree.Invalidate();
	m_bChanged = true;
	return true;
}