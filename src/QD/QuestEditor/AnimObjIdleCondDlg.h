#pragma once

#include "afxwin.h"
#include "CondDlgBase.h"


// AnimObjIdleCondDlg dialog

/**
	������ �� ��������� ����� ������, ��� �������
 */
class AnimObjIdleCondDlg : public CondDlgBase
{
	DECLARE_DYNAMIC(AnimObjIdleCondDlg)

public:
	AnimObjIdleCondDlg(DWORD id, CWnd* pParent = NULL);   // standard constructor
	virtual ~AnimObjIdleCondDlg();
	/*virtual*/ bool IsCondDataComplete();

// Dialog Data
	enum { IDD = IDD_DLG_COND_ANIM_OBJ_IDLE_GRT_VAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	/// ������ ����
	CCJFlatComboBox m_cbScenes;
	/// ������ ������� ��� ��������� �����
	CCJFlatComboBox m_cbObjects;
	/// �������� ������� �������, ���������� ������� ����� � ������������ �������
	int m_iValue;
public:
	virtual BOOL OnInitDialog();
	/// �� ����� ����� ������������� �������
	afx_msg void OnCbnSelchangeComboScenes();
	/// ��������� � ������� ������
	afx_msg void OnCbnSelchangeComboObject();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEnChangeEdit();
};
