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
	�������� ������� �������������� �������, �� ������� �������� �������� ���������
 */
class InventoryPP : public CBasePage
{
	DECLARE_DYNCREATE(InventoryPP);
	
	/// ������, ��� �������� ������ ���������
	class qdGameObjectAnimated* m_ptrObj;
	/// ��������� �������
	class qdGameDispatcher* m_ptrGameDisp;
// Construction
public:
	InventoryPP();
	~InventoryPP();

	/// ��������� �������������� ������ � ���������
	VOID set_data(qdGameDispatcher* ptrDisp, qdGameObjectAnimated* ptrAO);
// Dialog Data
	//{{AFX_DATA(InventoryPP)
	enum { IDD = IDD_PROPPAGE_INVENTORY };
	/// ����� ����������� ��� ����� �����
	SelectPicture	m_wndPicSelector;
	/// ������ ���������
	CCJFlatComboBox	m_wndInventory;
	/// ���������� � ������� ��� ����� ���������
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
	/// ��������� ������ ��������� ���������
	VOID LoadInventoryList();
	/// ��������� ������ ��������� ����� �����
	VOID LoadCellTypes();
	/// �������� ��������� � �������� �������
	VOID obj2wnd();
	/// ������ �� ��������� ������� � ������
	VOID wnd2obj();
	/// ��������� � ������� ��������� ��� �����
	void setType(int type);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_INVENTORYPP_H__3A68CB7C_5EB2_4D32_B341_5BF3F6C23C4C__INCLUDED_)
