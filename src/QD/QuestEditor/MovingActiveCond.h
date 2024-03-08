#pragma once


// MovingActiveCond dialog
#include "conddlgbase.h"
#include "afxwin.h"

/**
	������� ��������� ��������
 */
class MovingActiveCond : public CondDlgBase
{

public:
	MovingActiveCond(DWORD id, CWnd* pParent = NULL);   // standard constructor
	virtual ~MovingActiveCond();

// Dialog Data
	enum { IDD = IDD_DLG_COND_MOVING_ACITVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	/// ��������� � �������
	afx_msg void OnCbnSelchangeComboCharacter();
	/// ��������� ������ ����������
	afx_msg void OnCbnSelchangeComboScenes();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	bool IsCondDataComplete();

	/// ��������� ����������
	bool LoadPers();
	/// ��������� �����
	bool LoadScenes();
	/// �������� ��������� �����
	class qdGameScene* get_scene();
private:
	/// ����� � �����������
	CCJFlatComboBox m_wndPers;
	/// ������� � ����� ����
	CStatic m_scenes_static;
	/// ����� �� �������
	CCJFlatComboBox m_scenes;
};
