#if !defined(AFX_MYTREE_H__16C2AE21_5D2D_11D2_9CFF_008048FB92E0__INCLUDED_)
#define AFX_MYTREE_H__16C2AE21_5D2D_11D2_9CFF_008048FB92E0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MyTree.h : header file
//

#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CMyTree window
// drag and drop action


//высылаем перед перетаскиванием
//узнаем можно или нет тащить
const unsigned NM_MT_CAN_DRAG_REQ = (NM_FIRST + 1U);
typedef struct tagNM_MT_CAN_DRAG
{
	NMHDR hdr;
	TVITEM *pItem;
	size_t item_count;
} NM_MT_CAN_DRAG;

const unsigned NM_MT_CAN_DROP_REQ = (NM_FIRST + 2U);
typedef struct tagNM_MT_CAN_DROP{
	NMHDR hdr;
	TVITEM *pItem;
	size_t item_count;
	TVITEM itemDest;
}NM_MT_CAN_DROP;

const unsigned NM_MT_DROP_REQ = (NM_FIRST + 3U);
typedef struct tagNM_MT_DROP{
	NMHDR hdr;
	TVITEM *pItem;
	size_t item_count;
	TVITEM itemDest;
	DWORD btn;//какую кнопку зарелизили
} NM_MT_DROP;

const unsigned NM_MT_GET_DRAG_CURSOR = (NM_FIRST + 4U);
enum eGDC_Action{GDC_NO, GDC_YES,};
typedef struct tagNM_MT_GET_DRAG_CUR{
	NMHDR hdr;
	eGDC_Action	eCurAction;
}NM_MT_GET_DRAG_CUR;


/**
	Попытка сделать контрол дерева, поддерживающий мультиселект и драг-энд-дроп

	К использованию не рекомендую. Лучше пользовать CTreeListCtrl
 */
class CMyTree : public CTreeCtrl
{
	/// Что делаем при перетаскивании
	enum eDragDropAction{DD_NONE,///< Ничего не делаем
		DD_COPY, ///< Копируем
		DD_MOVE///< Переносим
	};
	/// Куда будем скроллировать
	enum eScrollDirection{
		NO_SCROLL, ///< Никуда не будем скроллировать
		SCROLL_DOWN, ///< Вниз
		SCROLL_UP///< Вверх
	};
// Construction
public:
	typedef std::vector< HTREEITEM > ITEM_LIST;

	CMyTree();

// Attributes
public:

// Operations
public:

	/// Тоже самое что и CTreeCtrl::InsertItem
	HTREEITEM InsertItem(LPTVINSERTSTRUCT lpInsertStruct);
	/// Тоже самое что и CTreeCtrl::InsertItem
	HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage,
		int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam,
		HTREEITEM hParent, HTREEITEM hInsertAfter);
	/// Тоже самое что и CTreeCtrl::InsertItem
	HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT,
		HTREEITEM hInsertAfter = TVI_LAST);
	/// Тоже самое что и CTreeCtrl::InsertItem
	HTREEITEM InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage,
		HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	size_t GetSelectedCount() { 
		return item_list.size();
	}
	/// Массив выбранных элементов
	const HTREEITEM* GetSelected() const;

	/// Сбросить выбор со всех за исключением указанного
	void ClearSelection(HTREEITEM hitemNotEqual = NULL);
	/// Скопировать дочерние элементы под hitemDest
	void CopyChildItemTo(HTREEITEM hParent, HTREEITEM hitemDest);
	/// Перенести дочерние элементы под hitemDest
	void MoveChildItemTo(HTREEITEM hParent, HTREEITEM hitemDest);

	/// Переносит узел и все его подузлы под hitemDest
	HTREEITEM MoveItemTo(HTREEITEM hItem, HTREEITEM hitemDest);
	/// Копирует узел и все его подузлы под hitemDest
	HTREEITEM CopyItemTo(HTREEITEM hItem, HTREEITEM hitemDest);

	virtual ~CMyTree();

	/// Находит узел по тексту
	HTREEITEM FindItem(HTREEITEM hParentItem, const CString& strText);
	// Generated message map functions
protected:
	//{{AFX_MSG(CMyTree)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRenameItem();
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
protected:
	/// Можем ли бросить узел
	virtual BOOL CanDrop(HTREEITEM hItem);

private:
	/// На отмену драг&дроп
	void OnCancel();

	/// Переносит узел с подузлами под другой узел
	HTREEITEM TransferItem(HTREEITEM hitemDrag, HTREEITEM hitemDrop);
	/// Переносит подузламы узла под другой узел
	void TransferChildItem(HTREEITEM hPar, HTREEITEM hitemDest);


	/// рекурсивно назначает указанный риснук узлу и его подузлам
	BOOL SetItemsImage(UINT state, HTREEITEM hItem);
	/// Восстанавливает в дереве у узлов подсветку
	void RestoreSelection();

	/// Проверяет выбран ли родительский узел
	BOOL isParentSelected(HTREEITEM hitemChild);

	/// удаляет из item_list
	BOOL removefromil(HTREEITEM hItem);

	/// Возвращает самый верхний и самый нижний выбранные узлы
	BOOL GetSelectionBound(HTREEITEM *hUpperBound,HTREEITEM *hLowerBound);
	/// Снимает признак выбора со всех дочерних узлов
	void DeselectsubEntrys(HTREEITEM hRoot);
	/// Проверяет является ли узел дочерним для hitemSuspectedParent
	BOOL IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent);
	
	HTREEITEM GetLastVisible();

	void FillTVIBuffer(std::vector<TVITEM>& buff) const;
	void FillTVI(TVITEM& item, HTREEITEM hitem);
	void OnButtonUp(DWORD dwBtn);

	HCURSOR GetCursor(eGDC_Action act);
	LRESULT SendCustomNotify(NMHDR *hdr);
	eScrollDirection NeedScroll(CPoint const& point) const;
private:
	/// узел, с которого начинается выделение
	/// при использовании шифта
	HTREEITEM m_hShiftSelBegin;

	/// Список выбранных элементов
	ITEM_LIST item_list;

	/// Узел, который тянем
	HTREEITEM	m_hitemDrag;
	/// Узел, на который будем бросать
	HTREEITEM	m_hitemDrop;


	eDragDropAction m_dwAction;
	///< Идет ли процесс перетягивания узал
	BOOL m_bDragging;
	int m_ItemHeight;///< расчетов в onmousemove
	/// Можем ли кинуть
	bool m_bCanDrop;
	eScrollDirection m_eScrollDirection;
	/// Таймер для скроллирования при драг&дроп
	UINT_PTR m_uiScrollTimer;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MYTREE_H__16C2AE21_5D2D_11D2_9CFF_008048FB92E0__INCLUDED_)
