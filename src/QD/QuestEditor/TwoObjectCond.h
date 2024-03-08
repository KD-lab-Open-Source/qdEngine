#if !defined(AFX_TwoObjectCond_H__9C4A941A_5ADC_49B7_A26D_147C73B2B93D__INCLUDED_)
#define AFX_TwoObjectCond_H__9C4A941A_5ADC_49B7_A26D_147C73B2B93D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TwoObjectCond.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TwoObjectCond dialog
#include "qd_condition.h"
#include "CondDlgBase.h"
#include "afxwin.h"

/**
	������� ����� ��� �������, ������� ���������� ������� ��� �������
 */
class TwoObjectCond : public CondDlgBase
{
	// Construction
public:
	TwoObjectCond(DWORD id, CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(TwoObjectCond)
	enum { IDD = IDD_DLG_COND_MOUSE_OBJ_CLICK };
	/// ������ ������ ��������
	CCJFlatComboBox	m_wndObj2;
	/// ������ ������ ��������. � ���� ������ ������ �������, �������� ����� �������
	CCJFlatComboBox	m_wndObj;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TwoObjectCond)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TwoObjectCond)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	/// ���������� ������ ������ � �������
	virtual afx_msg void OnSelchangeComboObject();
	/// ���������� ������ ������ � �������
	virtual afx_msg void OnSelchangeComboObject2();
	/// ��������� ������� � m_wndObj
	afx_msg void OnCbnSelchangeComboScenes();
	/// ��������� ������� � m_wndObj2
	afx_msg void OnCbnSelchangeComboScenes2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	bool IsCondDataComplete();
	//! ��������� �������, �������� ����� �������
	virtual bool LoadInventoryObjects();
	//! ��������� �������, �� ������� ����� �������
	virtual bool LoadObjects();
	/// ��������� ������ ���� � ���������
	bool LoadScenes(CCJFlatComboBox& box, CStatic& box_static);
	/// ���������� ����� �� ������� ������
	class qdGameScene* get_scene();
	/// ���������� ����� �� ������� ������
	class qdGameScene* get_scene2();
	/// ��������� ������� ��� ������� ���������� � ���������
	void setSecondObjectStaticText(CString const& str);
	/// ��������� ������� ��� ������� ���������� � ���������
	void setFirstObjectStaticText(CString const& str);
protected:
	/// ��������� � ������� �����
	CCJFlatComboBox m_scenes;
	/// ������ ����� ����
	CCJFlatComboBox m_scenes2;
	/// ������� ����� � ���������� ����
	CStatic m_scene_static;
	CStatic m_scene_static2;
	/// ������� ����� � ���������� ��������
	CString secondObjectStaticText_;
	CString firstObjectStaticText_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_TwoObjectCond_H__9C4A941A_5ADC_49B7_A26D_147C73B2B93D__INCLUDED_)
