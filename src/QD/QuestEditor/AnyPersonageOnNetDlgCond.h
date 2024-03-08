#pragma once

#include "CondDlgBase.h"
#include "afxwin.h"

// AnyPersonageOnNetDlgCond dialog
/**
	����� �������� � ���� �����
 */
class AnyPersonageOnNetDlgCond : public CondDlgBase
{
	DECLARE_DYNAMIC(AnyPersonageOnNetDlgCond)

public:
	AnyPersonageOnNetDlgCond(DWORD condId, CWnd* pParent = NULL); // standard constructor
	virtual ~AnyPersonageOnNetDlgCond();

// Dialog Data
	enum { IDD = IDD_DLG_COND_ANY_MOVING_ON_NET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	/// ��������� ������ ����
	afx_msg void OnCbnSelchangeComboScenes();
	/// ��������� ���� � �������
	afx_msg void OnCbnSelchangeComboZone();

	DECLARE_MESSAGE_MAP()
	virtual bool IsCondDataComplete();
	virtual BOOL OnInitDialog();

	/// ��������� ���� ��� �����
	bool LoadZones(qdGameScene* pscene);

	/// ���������� ��������� ����
	qdGameScene* get_scene();
private:
	/// ������ ���, ������������� ��������� �����
	CCJFlatComboBox m_wndZones;
	/// ������ ����
	CCJFlatComboBox m_wndScenes;
};
