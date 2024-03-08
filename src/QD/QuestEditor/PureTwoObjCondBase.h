/*
	����.
	�����
*/
#pragma once

#include "afxwin.h"
#include "CondDlgBase.h"


// PureTwoObjCondBase dialog

/**
	����������� ����� ��� �������, ������� ����� �������� ��� �������
 */
class PureTwoObjCondBase : public CondDlgBase
{
	DECLARE_DYNAMIC(PureTwoObjCondBase)

public:
	PureTwoObjCondBase(DWORD id, CWnd* pParent = NULL);   // standard constructor
	virtual ~PureTwoObjCondBase();

	/*virtual*/ bool IsCondDataComplete();

	//! ������� �������� �������� � ����������
	virtual bool LoadToObjCombo1(const qdGameScene* scene) = 0;
	virtual bool LoadToObjCombo2(const qdGameScene* scene) = 0;
	//! �������� ������������ ��������� ������������� ��������
	virtual bool ValidObjects(const qdGameObject* obj1, const qdGameObject* obj2) const = 0;

// Dialog Data
	enum { IDD = IDD_DLG_COND_TWO_OBJ_BASE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CCJFlatComboBox m_cbScenes1;
	CCJFlatComboBox m_cbScenes2;
	CCJFlatComboBox m_cbObjects1;
	CCJFlatComboBox m_cbObjects2;
	CStatic m_sStaticObject1;
	CStatic m_sStaticObject2;
public:
	/// ������������� ������� � ������ �����
	afx_msg void OnCbnSelchangeComboScenes1();
	/// ������������� ������� �� ������ �����
	afx_msg void OnCbnSelchangeComboScenes2();

	afx_msg void OnCbnSelchangeComboObjects1();
	afx_msg void OnCbnSelchangeComboObjects2();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
