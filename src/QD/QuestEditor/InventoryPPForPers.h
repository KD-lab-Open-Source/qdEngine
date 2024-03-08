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
	��������� ��������� ��� ���������
 */
class InventoryPPForPers : public CBasePage
{
	DECLARE_DYNCREATE(InventoryPPForPers);

	/// ��������
	class qdGameObjectMoving* m_ptrObj;
	/// ��������� �������
	class qdGameDispatcher* m_ptrGameDisp;
	/// True - ���� �������� ����������
	bool m_bGlobalObj;
	/// True - ���� ���� ��������� ����������� ������ ���������
	bool m_bDisableAll;
// Construction
public:
	InventoryPPForPers();
	~InventoryPPForPers();

	/// ��������� �������������� �������� � ��������� �������
	void set_data(qdGameDispatcher* ptrDisp, qdGameObjectMoving* ptrObj);
// Dialog Data
	//{{AFX_DATA(InventoryPPForPers)
	enum { IDD = IDD_PROPPAGE_INVENTORY_FOR_PERS };
	/// ������ ��������� ���������
	CCJFlatComboBox	m_wndInventory;
	//}}AFX_DATA

	/// ������������� ������� ������������ ���������
	void set_global_obj(){
		m_bGlobalObj = true;
	}
	/// ��������� ����� ���������
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

	/// ��������� ������ ��������� ���������
	VOID LoadInventoryList();
	/// ��� ��������� �� ������� � ����
	VOID obj2wnd();
	/// ��� ���������� � ����� ��������� � ������
	VOID wnd2obj();
	/// ��������� m_wndInventory
	VOID DisableAll();
	/// ��������� ��� ��������� �� ��� ����� ����������� ���������
	VOID SetInventoryToCopies(const CString& strInvName);
	/// ��������� ��� ��������� ��� ����� ����������� ��������� � ���������� �����
	VOID SetInventoryToCopiesInScene(qdGameScene* pscene, const CString& strInvName);
private:
	//! ���, ������������ �������, ���������
	CString m_strSettedInventory;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_INVENTORYPPFORPERS_H__AFA91C1A_1045_4087_88BA_324A3FB3788E__INCLUDED_)
