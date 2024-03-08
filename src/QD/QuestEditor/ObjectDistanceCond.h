#pragma once


// ObjectDistanceCond dialog
#include "CondDlgBase.h"
#include "afxwin.h"
/**
	���������� ����� ����� ��������
 */
class ObjectDistanceCond : public CondDlgBase
{
public:
	ObjectDistanceCond(DWORD id, CWnd* pParent = NULL);   // standard constructor
	virtual ~ObjectDistanceCond();

// Dialog Data
	enum { IDD = IDD_DLG_COND_OBJ_DISTANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	/// ��������� ������� � m_wndObjs2 � ������ ������ ������ � �������
	afx_msg void OnCbnSelchangeComboObject();
	/// ��������� ������� � m_wndObjs1
	afx_msg void OnCbnSelchangeComboScenes();
	/// ������ ��������� � �������
	afx_msg void OnDeltaposSpinDistance(NMHDR *pNMHDR, LRESULT *pResult);
	/// ������ ������ ������ � �������
	afx_msg void OnCbnSelchangeComboObject2();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
	/// ��������� ������ ����
	bool LoadScenes();
	/// ��������� ������� � m_wndObjs1
	bool LoadObjects();
	bool IsCondDataComplete();
	/// ������ ��������� �����
	class qdGameScene* get_scene();
	/// ������ ��������� � ���������� ������
	qdGameObject* get_object(CCJFlatComboBox& cb);
private:
	CCJFlatComboBox m_wndObjs1;
	CCJFlatComboBox m_wndObjs2;
	CCJFlatComboBox m_scenes;
	CStatic m_scene_static;
	//! ���������� ����� ��������� �� �����
	float m_fDist;
};
