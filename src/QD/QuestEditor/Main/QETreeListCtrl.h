#pragma once
#include <treelistctrl\treelistctrl.h>

class QETreeListCtrl :
	public CTreeListCtrl
{
public:
	QETreeListCtrl(void);
	~QETreeListCtrl(void);
	CTreeListItem* getSubItemByText(CTreeListItem const* parent, 
									LPCTSTR lpszText, int iSubItem) const;
	//! используется для скроллинга при drag & drop
	BOOL OnScroll(UINT nScrollCode);
	void setAutoFocus(bool set);

	bool isChildItem(CTreeListItem const* child, CTreeListItem const* parent) const;
private:
	bool autoFocus_;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
