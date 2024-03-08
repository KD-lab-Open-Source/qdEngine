#if !defined(AFX_EDITLISTCTRL_H__AB089162_8B4E_44BE_9BEA_B7C299C8694C__INCLUDED_)
#define AFX_EDITLISTCTRL_H__AB089162_8B4E_44BE_9BEA_B7C299C8694C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditListCtrl.h : header file
//

#pragma warning(disable:4666)
#include <vector>


///определюят тип контрола, с помощью которого
///производится редактирование
const int ELC_ITEM_EDIT_CTRL_NONE = 0;
const int ELC_ITEM_EDIT_CTRL_EDIT = 1;
const int ELC_ITEM_EDIT_CTRL_COMBO = 2;

///коды нотификации
///конец радактирования. Вне зависимости от контрола
const unsigned NM_LCE_ENDEDIT = (NM_FIRST + 1U);

///посылается для того, чтобы определить можно ли редактировать
///коктретный айтем
const unsigned NM_LCE_EDITREQUEST	= (NM_FIRST + 2U);

/// посылается для того, чтобы  заполнить, если надо контрол 
///подходящими значениями. выровнять его, поменять стиль и все такое
///Например, заполнить комбобокс
const unsigned NM_LCE_SETUP_CTRL	= (NM_FIRST + 3U);

///приходят когда изменился активный столбец
const unsigned NM_LCE_SUBITEM_CHANGING = (NM_FIRST + 4U);
const unsigned NM_LCE_SUBITEM_CHANGED = (NM_FIRST + 5U);

///запрос типа контрола редактирования
const unsigned NM_LCE_EDITCONTROL_REQUEST = (NM_FIRST + 6U);

typedef struct tagNM_LCE_INFO
{
	NMHDR hdr;
	LVITEM item;
	//контрол, которым осуществлялось редактирование
	CWnd* pCtrl;
	int iEditCtrlType; 
} NM_LCE_INFO;

/////////////////////////////////////////////////////////////////////////////
// CXEditCtrl window


/////////////////////////////////////////////////////////////////////////////
// CEditListCtrl window

enum
{
	ELC_SIMPLEEDIT,
	ELC_MULTIEDIT,
	ELC_FLAGS,
	ELC_FILESELECT
};

/**
	Дописанный ListCtrl  с тем, чтобы можно было редактировать узлы в любом столбце
*/
class CEditListCtrl : public CListCtrl
{
	friend class CXEditCtrl;
	friend class CXComboCtrl;
	typedef std::vector<int> IntVect_t;

	CWnd*        m_pEdit;
	CPoint       m_cellEdit;

	/// Создает edit-контрол 
	CWnd* CreateEdit(CRect& rc, LPCTSTR lpszText, BOOL bMulti);
	/// Создает комбобокс-контрол 
	CWnd* CreateCombo(CRect& rc, LPCTSTR lpszText);

	/// показать контрол в нужной позиции
	void ShowEditControl(int row, int col, int iEditType);
	/// Вычисляет позицию для редактирующего контрола
	BOOL CalculateEditPosition(int row, int col, CRect& r);
	/// Закрывает редактирующий контрол и передает текст в родительский диалог
	void DestroyAndUpdateEdit(bool bSave);

	/// можено ли редактировать данный столбец
	bool is_editable_subitem(int iCol);

	/// список типов редактирования для стобцов
	IntVect_t m_vecColumnEditType;

	/// Номер выбранного столбца
	int m_iSelectedSubItem;
	/// Редактируем ли элемент
	bool m_bEditItem;
public:

	CEditListCtrl();

	/// Устанавливает тип контрола, которым будут редактироваться элементы в столбце
	bool SetEditCtrlType(int nCol, int iType);
	/// Возвращает тип контрола, которым редактируются элементы в стобце
	int GetEditCtrlType(int nCol) const{
		return m_vecColumnEditType[nCol];
	}
	/// возвращает номер выбранного столбца
	int GetSelectedSubitem() const{
		return m_iSelectedSubItem;
	}
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual VOID PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditListCtrl();

	/// Вставить столбец. Смысл такой же как и CListCtrl::InsertColumn
	/**
		Дополнительно делает отметку о типе редактирующего контрола
	 */
	int InsertColumn( int nCol, const LVCOLUMN* pColumn );
	/// Вставить столбец. Смысл такой же как и CListCtrl::InsertColumn
	/**
		Дополнительно делает отметку о типе редактирующего контрола
	 */
	int InsertColumn( int nCol, LPCTSTR lpszColumnHeading, 
		int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1 );

	/// Запускает редактирование элемента
	CWnd* EditSubItem(int iItem, int iSubItem);
	/// Запускает редактирование элемента и позовляет выбрать тип редактирующего контрола
	CWnd* EditSubItem(int iItem, int iSubItem, int iEditType);
	
	VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	// Generated message map functions
protected:
	//{{AFX_MSG(CEditListCtrl)
	afx_msg VOID OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg VOID OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	VOID DrawSubItem(LPDRAWITEMSTRUCT lpdis, int iSubItem, CRect& rc);
	LRESULT SendCustomMsg(unsigned id, NM_LCE_INFO* pnli);
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_EDITLISTCTRL_H__AB089162_8B4E_44BE_9BEA_B7C299C8694C__INCLUDED_)
