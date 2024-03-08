#include "stdafx.h"
#include ".\qetreelistctrl.h"

QETreeListCtrl::QETreeListCtrl(void)
: autoFocus_(false)
{
}

QETreeListCtrl::~QETreeListCtrl(void)
{
}

void QETreeListCtrl::setAutoFocus(bool set){
	autoFocus_ = set;
}

CTreeListItem* QETreeListCtrl::getSubItemByText(CTreeListItem const* parent, 
												LPCTSTR lpszText, int iSubItem) const
{
	CTreeListItem* child = GetChildItem(parent);
	if (!child)
		return NULL;
	
	do {
		if (!_tcscmp(child->GetText(iSubItem), lpszText))
			return child;
	} while(child = GetNextSiblingItem(child));
	return NULL;
}

BOOL QETreeListCtrl::OnScroll(UINT nScrollCode)
{
	int dx = 0;
	int dy = 0;
	switch(HIBYTE(nScrollCode)) {
	case SB_LINEUP:
		dy = -1;
		break;
	case SB_LINEDOWN:
		dy = 1;
		break;
	}
	switch(LOBYTE(nScrollCode)) {
	case SB_LINELEFT:
		dx = -1;
		break;
	case SB_LINERIGHT:
		dx = 1;
		break;
	}
	int sbmin, sbmax, sbpos;
	if (dx) {
		GetScrollRange(SB_HORZ,&sbmin, &sbmax);
		sbpos = GetScrollPos(SB_HORZ);
		if (sbmin < sbpos + dx&&sbpos+dx<sbmax) {
			SetScrollPos(SB_HORZ, sbpos + dx);
			SendMessage(WM_HSCROLL, 
				MAKEWPARAM(LOBYTE(nScrollCode), 0), NULL);
		}
	}
	if (dy) {
		GetScrollRange(SB_VERT,&sbmin, &sbmax);
		sbpos = GetScrollPos(SB_VERT);
		if (sbmin < sbpos + dy&&sbpos+dy<sbmax) {
			SetScrollPos(SB_VERT, sbpos + dy);
			SendMessage(WM_VSCROLL, 
				MAKEWPARAM(HIBYTE(nScrollCode), 0), NULL);
		}
	}
	return TRUE;
}
BEGIN_MESSAGE_MAP(QETreeListCtrl, CTreeListCtrl)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void QETreeListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (autoFocus_)
	{
		if (GetFocus() != this
			&&!GetState(TLS_MODIFY)
			&&GetForegroundWindow()==AfxGetMainWnd())
		{
			SetFocus();
		}
	}
	CTreeListCtrl::OnMouseMove(nFlags, point);
}

bool QETreeListCtrl::isChildItem(CTreeListItem const* child, 
								 CTreeListItem const* parent) const
{
	if (parent == TLI_ROOT)
		return true;
	ASSERT(child);

	do {
		if (child == parent)
			return true;
	} while(child = GetParentItem(child));
	return false;
}
