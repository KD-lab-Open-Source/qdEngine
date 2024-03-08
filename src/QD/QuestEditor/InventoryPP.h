#if !defined(AFX_INVENTORYPP_H__3A68CB7C_5EB2_4D32_B341_5BF3F6C23C4C__INCLUDED_)
#define AFX_INVENTORYPP_H__3A68CB7C_5EB2_4D32_B341_5BF3F6C23C4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InventoryPP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// InventoryPP dialog
#include "basepage.h"
#include "SelectPicture//selectpicture.h"
/**
	Страница свойств анимированного объекта, на которой задаются парметры инвентори
 */
class InventoryPP : public CBasePage
{
	DECLARE_DYNCREATE(InventoryPP);
	
	/// Объект, для которого задаем инвентори
	class qdGameObjectAnimated* m_ptrObj;
	/// Диспетчер проекта
	class qdGameDispatcher* m_ptrGameDisp;
// Construction
public:
	InventoryPP();
	~InventoryPP();

	/// Назначает обрабатываемый объект и диспетчер
	VOID set_data(qdGameDispatcher* ptrDisp, qdGameObjectAnimated* ptrAO);
// Dialog Data
	//{{AFX_DATA(InventoryPP)
	enum { IDD = IDD_PROPPAGE_INVENTORY };
	/// Набор изображений для типов ячеек
	SelectPicture	m_wndPicSelector;
	/// Список инвентори
	CCJFlatComboBox	m_wndInventory;
	/// Отображает в статике тип ячеек инвентори
	CString m_strType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(InventoryPP)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(InventoryPP)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboInventory();
	afx_msg void OnCellImageChanged(NMHDR* phdr, LRESULT * pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	/// Загружает список доступных инвентори
	VOID LoadInventoryList();
	/// Загружает список достпуных типов ячеек
	VOID LoadCellTypes();
	/// Парметры инвентори в контролы диалога
	VOID obj2wnd();
	/// Данные из контролов диалога в объект
	VOID wnd2obj();
	/// Отбражает в диалоге указанный тип ячеек
	void setType(int type);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_INVENTORYPP_H__3A68CB7C_5EB2_4D32_B341_5BF3F6C23C4C__INCLUDED_)
