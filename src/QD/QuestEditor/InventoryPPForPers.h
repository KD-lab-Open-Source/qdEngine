#if !defined(AFX_INVENTORYPPFORPERS_H__AFA91C1A_1045_4087_88BA_324A3FB3788E__INCLUDED_)
#define AFX_INVENTORYPPFORPERS_H__AFA91C1A_1045_4087_88BA_324A3FB3788E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InventoryPPForPers.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// InventoryPPForPers dialog
#include "basepage.h"
/**
	Параметры инвентори для персонажа
 */
class InventoryPPForPers : public CBasePage
{
	DECLARE_DYNCREATE(InventoryPPForPers);

	/// Персонаж
	class qdGameObjectMoving* m_ptrObj;
	/// Диспетчер проекта
	class qdGameDispatcher* m_ptrGameDisp;
	/// True - если персонаж глобальный
	bool m_bGlobalObj;
	/// True - если надо запретить возможность выбора инвентори
	bool m_bDisableAll;
// Construction
public:
	InventoryPPForPers();
	~InventoryPPForPers();

	/// Назначает обрабатываемый персонаж и диспетчер проекта
	void set_data(qdGameDispatcher* ptrDisp, qdGameObjectMoving* ptrObj);
// Dialog Data
	//{{AFX_DATA(InventoryPPForPers)
	enum { IDD = IDD_PROPPAGE_INVENTORY_FOR_PERS };
	/// Список доступных инвентори
	CCJFlatComboBox	m_wndInventory;
	//}}AFX_DATA

	/// Устанавливает признак глобальности персонажа
	void set_global_obj(){
		m_bGlobalObj = true;
	}
	/// Запрещает выбор инвентори
	void set_disable_all(){
		m_bDisableAll = true;
	}

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(InventoryPPForPers)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(InventoryPPForPers)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboInventory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	/// Загружает список доступных инвентори
	VOID LoadInventoryList();
	/// имя инвентори из объекта в окно
	VOID obj2wnd();
	/// Имя выбранного в комбо инвентори в объект
	VOID wnd2obj();
	/// Дизейблит m_wndInventory
	VOID DisableAll();
	/// Назначает имя инвентори во все копии глобального персонажа
	VOID SetInventoryToCopies(const CString& strInvName);
	/// Назначает имя инвентори для копии глобального персонажа в конкретной сцене
	VOID SetInventoryToCopiesInScene(qdGameScene* pscene, const CString& strInvName);
private:
	//! Имя, назначенного объекту, инвентори
	CString m_strSettedInventory;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_INVENTORYPPFORPERS_H__AFA91C1A_1045_4087_88BA_324A3FB3788E__INCLUDED_)
