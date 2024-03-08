#pragma once

#include "CondDlgBase.h"
#include "afxwin.h"
// StateAnimInPhaseDlg dialog

/**
	�������� ��������� � ����
 */
class StateAnimInPhaseDlg : public CondDlgBase
{

public:
	StateAnimInPhaseDlg(DWORD id, CWnd* pParent = NULL);   // standard constructor
	virtual ~StateAnimInPhaseDlg();

// Dialog Data
	enum { IDD = IDD_DLG_COND_ANIM_PHASE};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	/// ���������� ��������� � ������� 
	afx_msg void OnCbnSelchangeComboStates();
	/// ������������� ���������
	afx_msg void OnCbnSelchangeComboObject();
	/// ������������� �������
	afx_msg void OnCbnSelchangeComboScenes();

	/// ������ ������ ���������� � �������
	afx_msg void OnDeltaposSpinBegPhase(NMHDR *pNMHDR, LRESULT *pResult);
	/// ������ ����� ���������� � �������
	afx_msg void OnDeltaposSpinEndPhase(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	bool IsCondDataComplete();

	/// ������ �� ������
	void CondToDlg();
	/// ��������� �����
	bool LoadScenes();
	/// ��������� �������
	bool LoadObjects();
	/// ������� ����� �� ����
	class qdGameScene* get_scene();
	/// ��������� ��������� ��� �������
	bool LoadStates(qdGameObjectAnimated const* pobj);
private:
	/// ������ ���������� ��� ����
	int m_iFrom;
	/// ����� ���������� ��� ����
	int m_iTo;
	CCJFlatComboBox m_wndStates;
	CCJFlatComboBox m_wndObjs;
	CCJFlatComboBox m_wndScenes;
	CStatic m_wndSceneStatic;
};
