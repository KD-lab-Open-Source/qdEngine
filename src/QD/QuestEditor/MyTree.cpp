// MyTree.cpp : implementation file
//

#include "stdafx.h"
#include <algorithm>
#include "MyTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyTree

namespace{
	bool insert_uniq(CMyTree::ITEM_LIST& il, CMyTree::ITEM_LIST::value_type val)
	{
		CMyTree::ITEM_LIST::iterator i = std::find(il.begin(), il.end(), val);
		if(i == il.end())
		{
			il.push_back(val);
			return true;
		}
		return false;
	}

	inline BOOL IsKeyPressed(int keyID)
	{
		return GetAsyncKeyState(keyID)>>15;
	}
}
UINT_PTR const SCROLL_TIMER = 1;
UINT_PTR const SCROLL_TIMER_PERIOD = 300;
/////////////////////////////////////////////////////////////////////////////

CMyTree::CMyTree()
{
	m_dwAction = DD_NONE;
	m_bDragging = FALSE;
	m_hShiftSelBegin = NULL;
	m_eScrollDirection = NO_SCROLL;
	m_uiScrollTimer = static_cast<UINT_PTR>(-1);
}

CMyTree::~CMyTree()
{
}


BEGIN_MESSAGE_MAP(CMyTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CMyTree)
	ON_WM_RBUTTONUP()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(TVN_BEGINRDRAG, OnBegindrag)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteitem)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyTree message handlers

void CMyTree::OnCancel() 
{
	if (m_bDragging)
	{
		ReleaseCapture();
		m_bDragging = FALSE;
		SelectDropTarget(NULL);
		SetItemState(m_hitemDrag,0,TVIS_DROPHILITED);
		m_hitemDrag = NULL;
	}
}

void CMyTree::OnButtonUp(DWORD dwBtn)
{
	if (m_bDragging)
	{
		if (m_uiScrollTimer != static_cast<UINT_PTR>(-1))
		{
			KillTimer(m_uiScrollTimer);
			m_uiScrollTimer = static_cast<UINT_PTR>(-1);
		}

		if (m_bCanDrop)
		{
			std::vector<TVITEM> buff;
			buff.reserve(GetSelectedCount());

			FillTVIBuffer(buff);

			NM_MT_DROP cd;
			memset(&cd, 0, sizeof(NM_MT_DROP));
			cd.item_count = buff.size();
			cd.pItem = &*buff.begin();
			cd.hdr.code = NM_MT_DROP_REQ;
			cd.hdr.hwndFrom = m_hWnd;
			cd.hdr.idFrom = GetDlgCtrlID();
			cd.btn = dwBtn;

			FillTVI(cd.itemDest, m_hitemDrop);
			
			SendCustomNotify(reinterpret_cast<NMHDR*>(&cd));
		}
		ReleaseCapture();
		m_bDragging = FALSE;

		SelectDropTarget(NULL);

		if(HTREEITEM hSelected = GetSelectedItem())
		{
			SetItemState(hSelected, TVIS_DROPHILITED, TVIS_DROPHILITED);
			insert_uniq(item_list, hSelected);
			m_hShiftSelBegin = hSelected;
		}
		else if (m_hShiftSelBegin)
			SelectItem(m_hShiftSelBegin);
		else
			m_hShiftSelBegin = NULL;
		m_hitemDrag = NULL;
	}
}

void CMyTree::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ASSERT(!m_bDragging);
	m_hitemDrop = NULL;


	//восстанавливаем, потому что с узла, за который тянем 
	//выделение снимается
	SetItemState(m_hitemDrag, TVIS_DROPHILITED, TVIS_DROPHILITED);


	//запрашиваем можно ли тянуть
	NM_MT_CAN_DRAG cd;
	cd.hdr.code		= NM_MT_CAN_DRAG_REQ;
	cd.hdr.hwndFrom = m_hWnd;
	cd.hdr.idFrom	= GetDlgCtrlID();
	std::vector<TVITEM> buff;
	buff.reserve(GetSelectedCount());
	FillTVIBuffer(buff);
	cd.item_count = buff.size();
	cd.pItem = &*buff.begin();

	if (SendCustomNotify(reinterpret_cast<NMHDR*>(&cd)) == TRUE)
	{
		*pResult = 1;
		return;
	}

	HTREEITEM hItem = GetChildItem(TVI_ROOT);
	CRect r;
	GetItemRect(hItem,&r,FALSE);
	m_ItemHeight = r.Height();//используется в Onmousemove

	m_bDragging = TRUE;
	SetCapture();
}

void CMyTree::OnRButtonUp(UINT nFlags, CPoint point) 
{
	OnButtonUp(MK_RBUTTON);
	CTreeCtrl::OnRButtonUp(nFlags, point);
}

void CMyTree::FillTVI(TVITEM& item, HTREEITEM hitem)
{
	memset(&item, 0, sizeof(TVITEM));
	item.mask = TVIF_TEXT|TVIF_HANDLE|TVIF_PARAM;
	item.hItem = hitem;
	GetItem(&item);
}

void CMyTree::FillTVIBuffer(std::vector<TVITEM>& buff) const
{
	TVITEM tvi;
	memset(&tvi, 0, sizeof(TVITEM));
	tvi.mask = TVIF_TEXT|TVIF_HANDLE|TVIF_PARAM;
	ITEM_LIST::const_iterator i = item_list.begin(), e = item_list.end();
	for(; i != e; ++i){
		tvi.hItem = *i;
		if (GetItem(&tvi))
			buff.push_back(tvi);
	}
}

CMyTree::eScrollDirection CMyTree::NeedScroll(CPoint const& point) const{
	if(point.y/m_ItemHeight < 3)
		return SCROLL_UP;
	else if((GetVisibleCount() - point.y/m_ItemHeight) < 3)
		return SCROLL_DOWN;
	return NO_SCROLL;
}

void CMyTree::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bDragging)
	{
		switch(NeedScroll(point)) {
		case NO_SCROLL:
			{
				KillTimer(m_uiScrollTimer);
				m_uiScrollTimer = static_cast<UINT_PTR>(-1);
			}
			break;
		case SCROLL_UP:
		case SCROLL_DOWN:
			m_uiScrollTimer = SetTimer(SCROLL_TIMER, SCROLL_TIMER_PERIOD, NULL);
			break;
		}
/*
		HTREEITEM hFirstVisible = GetFirstVisibleItem();
		HTREEITEM hTarget = GetPrevVisibleItem(hFirstVisible);

		if(EnsureVisible(hTarget))
			Invalidate(FALSE);
		HTREEITEM hLast = GetLastVisible();
		HTREEITEM hTarget = GetNextVisibleItem(hLast);

		if(EnsureVisible(hTarget))
			Invalidate(FALSE);
*/
		m_bCanDrop = false;
		UINT flags = 0;
		HTREEITEM hitem = HitTest(point, &flags);

		std::vector<TVITEM> buff;
		buff.reserve(GetSelectedCount());
		FillTVIBuffer(buff);

		NM_MT_CAN_DROP cd;
		memset(&cd, 0, sizeof(NM_MT_CAN_DROP));
		cd.item_count = buff.size();
		cd.pItem = &*buff.begin();
		cd.hdr.code = NM_MT_CAN_DROP_REQ;
		cd.hdr.hwndFrom = m_hWnd;
		cd.hdr.idFrom = GetDlgCtrlID();

		FillTVI(cd.itemDest, hitem);

		LRESULT const res = SendCustomNotify(reinterpret_cast<NMHDR*>(&cd));

		if (res||!CanDrop(hitem))//запрещено кидать
		{
			SetCursor(GetCursor(GDC_NO));
			m_hitemDrop = NULL;
		}
		else
		{
			m_hitemDrop = hitem;
			SetCursor(GetCursor(GDC_YES));
			m_bCanDrop = true;
		}
		SelectDropTarget(m_hitemDrop);
	}
	
	CTreeCtrl::OnMouseMove(nFlags, point);
}

BOOL CMyTree::IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent)
{
	do
	{
		if (hitemChild == hitemSuspectedParent)
			break;
	}
	while ((hitemChild = GetParentItem(hitemChild)) != NULL);

	return (hitemChild != NULL);
}

HTREEITEM CMyTree::TransferItem(HTREEITEM hitemDrag, HTREEITEM hitemDrop)
{
	TV_INSERTSTRUCT		tvstruct;
	TCHAR				sztBuffer[128];
	HTREEITEM			hNewItem, hFirstChild;

// avoid an infinite recursion situation
	tvstruct.item.hItem = hitemDrag;
	tvstruct.item.cchTextMax = 127;
	tvstruct.item.pszText = sztBuffer;
	tvstruct.item.mask = TVIF_HANDLE | TVIF_CHILDREN | TVIF_IMAGE | 
						TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM | TVIF_STATE;

	GetItem(&tvstruct.item);  // get information of the dragged element

	tvstruct.hParent = hitemDrop;
	tvstruct.hInsertAfter = TVI_LAST;
	tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM | TVIF_STATE;

	hNewItem = CTreeCtrl::InsertItem(&tvstruct);

	SetItemState(hNewItem,0,TVIS_SELECTED|TVIS_DROPHILITED);

	if(GetItemState(hitemDrag, TVIS_EXPANDED)&TVIS_EXPANDED)
		Expand(hNewItem,TVE_COLLAPSE);

	if ((hFirstChild = GetChildItem(hitemDrag)) != NULL)
	{
		HTREEITEM hSibling = GetNextItem(hFirstChild,TVGN_NEXT);

		if(hSibling)
			do
			{
				TransferItem(hSibling, hNewItem);  // recursively transfer all the items

				if(m_dwAction == DD_MOVE)
				{
					DeleteItem(hSibling);

					hSibling = hFirstChild;
				}
			}
			while((hSibling = GetNextItem(hSibling,TVGN_NEXT)) != NULL);
			
		TransferItem(hFirstChild, hNewItem);  // recursively transfer all the items
		if(m_dwAction == DD_MOVE)
			DeleteItem(hFirstChild);
	}

	return hNewItem;
}


void CMyTree::OnLButtonUp(UINT nFlags, CPoint point) 
{
	OnButtonUp(MK_LBUTTON);

	CTreeCtrl::OnLButtonUp(nFlags, point);
}

HTREEITEM CMyTree::GetLastVisible()
{
	RECT r;
	GetClientRect(&r);

	POINT p = {20,r.bottom - (m_ItemHeight>>1)};
	HTREEITEM hItem = HitTest(p,NULL);

	return hItem;
}

void CMyTree::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bDragging)
		OnCancel();

	TV_HITTESTINFO htinfo;

	htinfo.pt = point;

	HTREEITEM hItem = HitTest(&htinfo);
	if(htinfo.flags&~(TVHT_ONITEM | TVHT_ONITEMBUTTON | TVHT_ONITEMINDENT))
	{
		ClearSelection();
		SelectItem(NULL);
		SetItemState(hItem, 0, TVIS_DROPHILITED);
		CTreeCtrl::OnRButtonDown( nFlags, point);
	}
	else
	{
		if((GetItemState(hItem,TVIS_DROPHILITED)&TVIS_DROPHILITED) != TVIS_DROPHILITED)
		{
			ClearSelection();
		//сохраняем item в списке выделенных 
			insert_uniq(item_list, hItem);
			SelectItem(hItem);
			SetItemState(hItem, TVIS_DROPHILITED, TVIS_DROPHILITED);
			m_hShiftSelBegin = hItem;
		}
		else
		{
			HTREEITEM hSel = GetSelectedItem();

			SelectItem(hItem);
			SetItemState(hItem, TVIS_DROPHILITED, TVIS_DROPHILITED);

			if(item_list.empty())
				item_list.push_back(hItem);

			SetItemState(hSel,TVIS_DROPHILITED,TVIS_DROPHILITED);
		}
		m_hitemDrag = hItem;
		CTreeCtrl::OnRButtonDown( nFlags, point);
		if(!m_bDragging)
		{
			m_hitemDrag = NULL;
			PostMessage(WM_RBUTTONUP,WPARAM(nFlags),MAKELPARAM(point.x,point.y));
		}
	}
}

void CMyTree::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//если кликнули мимо drag and drop  меню

	if(m_bDragging)
		OnCancel();

	UINT fl = 0;
	HTREEITEM hItem = HitTest(point,&fl);
	CEdit* ped = GetEditControl();
	if(ped)
		ped->DestroyWindow();

	if(fl&~(TVHT_ONITEM | TVHT_ONITEMBUTTON))
	{
		LONG styles = GetWindowLong(m_hWnd, GWL_STYLE);

		SetItemState(hItem, 0,TVIS_DROPHILITED); 

		if(styles&TVS_SHOWSELALWAYS)
		{
			m_hShiftSelBegin = GetSelectedItem();
		}
		else
			m_hShiftSelBegin = NULL;

		SelectItem(m_hShiftSelBegin);	
		ClearSelection(m_hShiftSelBegin);
		CTreeCtrl::OnLButtonDown( nFlags, point);
		return;
	}

	if(fl&TVHT_ONITEM)
	{
		UINT nState = GetItemState(hItem,TVIS_DROPHILITED|TVIS_SELECTED);
		if(nFlags & MK_SHIFT)
		{
			bool bSameParent = GetParentItem(m_hShiftSelBegin) ==  GetParentItem(hItem);
			if(!bSameParent) 
				m_hShiftSelBegin = hItem;

			ClearSelection();

			if(bSameParent&&hItem != m_hShiftSelBegin)
			{
				RECT r1,r2;
				GetItemRect(m_hShiftSelBegin,&r1, FALSE);
				GetItemRect(hItem,&r2, FALSE);
				
				UINT dir = TVGN_NEXT;
				if(r2.top < r1.top)
					dir = TVGN_PREVIOUS;
				HTREEITEM i = m_hShiftSelBegin;
				do {
					item_list.push_back(i);
					SetItemState(i, TVIS_DROPHILITED, TVIS_DROPHILITED);
				} while((i = GetNextItem(i,dir))&&i!=hItem);
			}
			item_list.push_back(hItem);
			SetItemState(hItem, TVIS_DROPHILITED, TVIS_DROPHILITED);
			SelectItem(hItem);
		}
		else if(nFlags & MK_CONTROL)
		{
			//если не было раньше выбрано
			//выбрали и вышли
			if(!m_hShiftSelBegin)
			{
				item_list.push_back(hItem);
				SetItemState(hItem, TVIS_DROPHILITED, TVIS_DROPHILITED);
				SelectItem(hItem);
				m_hShiftSelBegin = hItem;
				return;
			}

			//узлы можно выбирать только под одним и тем же родителем
			if(GetParentItem(hItem) != GetParentItem(m_hShiftSelBegin))
			{
				MessageBeep(0);
				return;
			}
			if(nState&TVIS_DROPHILITED)
			{
				//если элемент был подсвечен
				//снимаем подсветку
				SetItemState(hItem,0,TVIS_DROPHILITED);
				removefromil(hItem);
				if(nState&TVIS_SELECTED)//если был выбран
				{
					if(item_list.empty())//выбрать вместо него нечего
					{
						SelectItem(NULL);
						m_hShiftSelBegin = NULL;
					}
					else
					{
						//выбираем предыдущий подсвеченный
						SelectItem(item_list.back());
					}
				}
			}
			else
			{
				//если не подсвечен, то выбираем и подсвечиваем
				//item_list.push_back(hItem);
				insert_uniq(item_list, hItem);
				SetItemState(hItem,TVIS_DROPHILITED,TVIS_DROPHILITED);
				SelectItem(hItem);
			}
		}
		else
		{
			ASSERT(fl & TVHT_ONITEM);
			ASSERT(hItem);

			//если hItem подсвечен,
			//то сбрасываем выбор с этого элемента, чтобы не включилось 
			//редактирование
			if((GetItemState(hItem,TVIS_DROPHILITED)&TVIS_DROPHILITED))
			{
				if(hItem != m_hShiftSelBegin)
					SelectItem(NULL);
			}
			else
			{
				SelectItem(NULL);
				//очищаем и выделяем
				ClearSelection();
				SetItemState(hItem, TVIS_DROPHILITED|TVIS_SELECTED, 
					TVIS_DROPHILITED|TVIS_SELECTED);
				item_list.push_back(hItem);
			}
			m_hitemDrag = hItem;
			//вызываем, чтобы завести драг и дроп, когда есть подходящий стиль
			CTreeCtrl::OnLButtonDown( nFlags, point);

			//если не тащим, значит очистить выделение
			if(!m_bDragging) 
			{
				m_hitemDrag = NULL;
				ClearSelection(hItem);
				SetItemState(hItem, TVIS_DROPHILITED, TVIS_DROPHILITED);
				SelectItem(hItem);
			}

			m_hShiftSelBegin = hItem;
		}
		return;
	}

	CTreeCtrl::OnLButtonDown( nFlags, point);
}

void CMyTree::DeselectsubEntrys(HTREEITEM hRoot)
{
	HTREEITEM hItem = GetChildItem(hRoot);
	if(hItem)
		do
		{
			UINT State = GetItemState(hItem,TVIS_EXPANDED);
			if(State&TVIS_DROPHILITED)
			{
				SetItemState(hItem,0,TVIS_DROPHILITED);
				removefromil(hItem);
			}
			if(State&TVIS_EXPANDED)
				DeselectsubEntrys(hItem);
		}
		while(hItem = GetNextSiblingItem(hItem));
}

void CMyTree::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pnmtree = (NM_TREEVIEW*)pNMHDR;
	if(pnmtree->action == TVE_COLLAPSE)
		DeselectsubEntrys(pnmtree->itemNew.hItem);
	
	*pResult = 0;
}

void CMyTree::ClearSelection(HTREEITEM hitemNotEqual)
{
	ITEM_LIST::iterator i = item_list.begin();
	ITEM_LIST::iterator end = item_list.end();
	for(;i!=item_list.end();++i)
		if(*i != hitemNotEqual)
			SetItemState(*i,0, TVIS_SELECTED|TVIS_DROPHILITED);

	item_list.clear();
	if(hitemNotEqual)
		item_list.push_back(hitemNotEqual);
#ifdef _DEBUG
	if(hitemNotEqual)
		_ASSERT(item_list.size()==1);
	else
		_ASSERT(item_list.size()==0);
#endif
}

inline BOOL CMyTree::removefromil(HTREEITEM hItem)
{
	ITEM_LIST::iterator _itr = std::find(item_list.begin(), item_list.end(), hItem);
	if(_itr != item_list.end())
	{
		item_list.erase(_itr);
		return TRUE;
	}
	return FALSE;
}

BOOL CMyTree::isParentSelected(HTREEITEM hitemChild)
{
	do
	{
		UINT uState = GetItemState(hitemChild,TVIS_DROPHILITED);
		if((uState&TVIS_DROPHILITED) == TVIS_DROPHILITED) break;
	}
	while((hitemChild = GetParentItem(hitemChild)) != NULL);

	return (hitemChild != NULL);
}

void CMyTree::RestoreSelection()
{
	ITEM_LIST::iterator i = item_list.begin();
	ITEM_LIST::iterator end = item_list.end();

	for(;i != end;++i)
		SetItemState(*i,TVIS_DROPHILITED,TVIS_DROPHILITED);
}

BOOL CMyTree::SetItemsImage(UINT image,HTREEITEM hItem)
{
	SetItemState(hItem,image,TVIS_STATEIMAGEMASK);

	HTREEITEM hChild = GetChildItem(hItem);
	if(hChild)
		do
		{
			SetItemsImage(image, hChild);
		}
		while(hChild = GetNextSiblingItem(hChild));
	return FALSE;
}

void CMyTree::TransferChildItem(HTREEITEM hPar, HTREEITEM hitemDest)
{
	HTREEITEM hFirstChild = GetChildItem(hPar);
	if(hFirstChild)
	{
		HTREEITEM hSibling = GetNextSiblingItem(hFirstChild);
		if(hSibling)
			do
			{
				TransferItem(hSibling,hitemDest);
				if(m_dwAction == DD_MOVE)
				{
					DeleteItem(hSibling);
					hSibling = hFirstChild;
				}
			}
			while(hSibling = GetNextSiblingItem(hSibling));

		TransferItem(hFirstChild,hitemDest);
		if(m_dwAction == DD_MOVE)
			DeleteItem(hFirstChild);
	}
}

BOOL CMyTree::CanDrop(HTREEITEM hItem)
{
	if(!hItem) return TRUE;

	ITEM_LIST::iterator i = item_list.begin();
	ITEM_LIST::iterator end = item_list.end();

	for(;i != end;++i)
	{
		HTREEITEM hi = *i;
		if(IsChildNodeOf(hItem, hi) || GetParentItem(hi) == hItem)
			return FALSE;
	}

	return TRUE;
}

HTREEITEM CMyTree::MoveItemTo(HTREEITEM hItem, HTREEITEM hitemDest)
{
	eDragDropAction save = m_dwAction;
	m_dwAction = DD_MOVE;
	HTREEITEM const res = TransferItem(hItem, hitemDest);
	DeleteItem(hItem);
	m_dwAction = save;
	return res;
}

HTREEITEM CMyTree::CopyItemTo(HTREEITEM hItem, HTREEITEM hitemDest)
{
	eDragDropAction save = m_dwAction;
	m_dwAction = DD_COPY;
	HTREEITEM const res = TransferItem(hItem, hitemDest);
	m_dwAction = save;
	return res;
}

void CMyTree::MoveChildItemTo(HTREEITEM hParent, HTREEITEM hitemDest)
{
	eDragDropAction save = m_dwAction;
	m_dwAction = DD_MOVE;
	TransferChildItem(hParent, hitemDest);
	m_dwAction = save;
}

void CMyTree::CopyChildItemTo(HTREEITEM hParent, HTREEITEM hitemDest)
{
	eDragDropAction save = m_dwAction;
	m_dwAction = DD_COPY;
	TransferChildItem(hParent, hitemDest);
	m_dwAction = save;
}


void CMyTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iItemCount = GetCount();
	NM_TREEVIEW* pTreeView = (NM_TREEVIEW*)pNMHDR;
	if(pTreeView->itemNew.hItem && 
		pTreeView->itemNew.state&TVIS_SELECTED)
	{
		if (item_list.empty()) {
			m_hShiftSelBegin = pTreeView->itemNew.hItem;
			SetItemState(pTreeView->itemNew.hItem, TVIS_DROPHILITED, TVIS_DROPHILITED);
		}
		insert_uniq(item_list, pTreeView->itemNew.hItem);
	}
	
	*pResult = 0;
}

void CMyTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar == VK_F5)
	{
		CWaitCursor wc;
		return;
	}

	HTREEITEM hprev = GetSelectedItem();
	if(nChar == VK_LEFT)
	{
		if(GetItemState(hprev,TVIS_EXPANDED)&TVIS_EXPANDED)
			;
		else
		{
			if(GetParentItem(hprev))
				ClearSelection();
		}
	}
	else if(nChar == VK_RIGHT)
	{
		if(GetItemState(hprev,TVIS_EXPANDED)&TVIS_EXPANDED)
			ClearSelection();
	}

	BOOL bShiftPressed = IsKeyPressed(VK_SHIFT);

///////////////////////////
	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

///////////////////////////
//после обработчика событий выделенный элемент меняется
//по этому hcurItem и hprev имеют разные значения.

	HTREEITEM hcurItem = GetSelectedItem();
	if(nChar == VK_UP || nChar == VK_DOWN )
	{
		if(bShiftPressed)
		{
			HTREEITEM hparent = GetParentItem(*item_list.begin());
#ifdef _DEBUG
			CString str = GetItemText(hparent);
			CString str1 = GetItemText(hcurItem);
			str = GetItemText(hprev);
#endif
//если паренты разные, то очищаем выбор и начинаем копить заново 
//может быть выбран несколько элементов только одного и того же родителя
			if(hparent != GetParentItem(hcurItem))
			{
				ClearSelection();
				item_list.push_back(hcurItem);
				SetItemState(hcurItem, TVIS_DROPHILITED, TVIS_DROPHILITED);
				m_hShiftSelBegin = hcurItem;
			}
			else
			{
				if(GetItemState(hcurItem, TVIS_DROPHILITED)& TVIS_DROPHILITED)
				{
					//мы убираем подсветку элементов
					removefromil(hprev);
					SetItemState(hprev, 0, TVIS_DROPHILITED);
				}
				else
				{
					SetItemState(hcurItem, TVIS_DROPHILITED, TVIS_DROPHILITED);
					//item_list.push_back(hcurItem);
					insert_uniq(item_list, hcurItem);
					
				}
			}
		}
		else
		{
			ClearSelection();
			item_list.push_back(hcurItem);
			SetItemState( hcurItem, TVIS_DROPHILITED, TVIS_DROPHILITED);

			m_hShiftSelBegin = hcurItem;
		}

	}
	else if(nChar == VK_END || nChar == VK_HOME
	   || nChar == VK_NEXT || nChar == VK_PRIOR)
	{
		if(bShiftPressed)
		{
			UINT nState = GetItemState(hcurItem,TVIS_DROPHILITED);

			bool bSameParent = GetParentItem(m_hShiftSelBegin) ==  GetParentItem(hcurItem);
			if(!bSameParent) 
				m_hShiftSelBegin = hcurItem;

			ClearSelection();
			SelectItem(NULL);

			if(bSameParent)
			{
				RECT r1,r2;
				GetItemRect(m_hShiftSelBegin,&r1, FALSE);
				GetItemRect(hcurItem,&r2, FALSE);
				
				UINT dir = TVGN_NEXT;
				if(r2.top < r1.top)
					dir = TVGN_PREVIOUS;

				HTREEITEM i = m_hShiftSelBegin;
				do
				{
					item_list.push_back(i);
					SetItemState(i,TVIS_DROPHILITED,TVIS_DROPHILITED);
				}
				while((i = GetNextItem(i,dir)) && (i != hcurItem));
			}

			insert_uniq(item_list, hcurItem);
			SelectItem(hcurItem);
			SetItemState(hcurItem, TVIS_DROPHILITED,TVIS_DROPHILITED);
		}
		else
		{
			ClearSelection();
			item_list.push_back(hcurItem);
			SetItemState( hcurItem, TVIS_DROPHILITED, TVIS_DROPHILITED);

			m_hShiftSelBegin = hcurItem;
		}
	}
	if(nChar == VK_RETURN)
	{
		CEdit* pe = GetEditControl();
		if(pe)
			pe->DestroyWindow();
	}
}

HTREEITEM CMyTree::InsertItem(LPTVINSERTSTRUCT lpInsertStruct)
{
	return CTreeCtrl::InsertItem(lpInsertStruct);
}
HTREEITEM CMyTree::InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage,
					 int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam,
					 HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	return CTreeCtrl::InsertItem(nMask, lpszItem, nImage, 
							nSelectedImage, nState, nStateMask,
							lParam, hParent, hInsertAfter);
}
HTREEITEM CMyTree::InsertItem(LPCTSTR lpszItem, HTREEITEM hParent,
					 HTREEITEM hInsertAfter)
{
	return CTreeCtrl::InsertItem(lpszItem, hParent, hInsertAfter);
}
HTREEITEM CMyTree::InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage,
					 HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	return CTreeCtrl::InsertItem(lpszItem, nImage, nSelectedImage, hParent, hInsertAfter);
}

const HTREEITEM* CMyTree::GetSelected() const
{
	return item_list.size() ? &*item_list.begin() : NULL;
}

void CMyTree::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pTreeView = (NM_TREEVIEW*)pNMHDR;
	removefromil(pTreeView->itemOld.hItem);
	*pResult = 0;
}

HTREEITEM CMyTree::FindItem(HTREEITEM hParentItem, const CString& strText)
{
	HTREEITEM hRes = GetChildItem(hParentItem);
	if (hRes) {
		do {
			if(GetItemText(hRes) == strText)
				return hRes;
		} while((hRes = GetNextSiblingItem(hRes)));
	}
	return NULL;
}

HCURSOR CMyTree::GetCursor(eGDC_Action act)
{
	NM_MT_GET_DRAG_CUR dc;
	dc.hdr.code = NM_MT_GET_DRAG_CURSOR;
	dc.hdr.hwndFrom = m_hWnd;
	dc.hdr.idFrom = GetDlgCtrlID();
	dc.eCurAction = act;
	HCURSOR res = 
		reinterpret_cast<HCURSOR>(SendCustomNotify(reinterpret_cast<NMHDR*>(&dc)));
	if (res)
		return res;
	if (act == GDC_NO)
		return LoadCursor(NULL, MAKEINTRESOURCE(IDC_NO));
	else
		return LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
}

LRESULT CMyTree::SendCustomNotify(NMHDR *hdr)
{
	return GetParent()->SendMessage(WM_NOTIFY, hdr->code, 
				reinterpret_cast<LPARAM>(hdr));
}

void CMyTree::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == SCROLL_TIMER)
	{
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		eScrollDirection dir = NeedScroll(point);
		if (m_bDragging&&dir != NO_SCROLL)
		{
			switch(dir) {
			case SCROLL_UP:
				{
					HTREEITEM hFirstVisible = GetFirstVisibleItem();
					HTREEITEM hTarget = GetPrevVisibleItem(hFirstVisible);

					if(EnsureVisible(hTarget))
						Invalidate(FALSE);
				}
				break;
			case SCROLL_DOWN:
				HTREEITEM hLast = GetLastVisible();
				HTREEITEM hTarget = GetNextVisibleItem(hLast);

				if(EnsureVisible(hTarget))
					Invalidate(FALSE);
				break;
			}
		}
		else
		{
			KillTimer(m_uiScrollTimer);
			m_uiScrollTimer = static_cast<UINT_PTR>(-1);
		}
	}
	CTreeCtrl::OnTimer(nIDEvent);
}
