#pragma once


#include "CondDlgBase.h"
#include "afxwin.h"
// ObjInStateCond dialog

/**
	������ � ���������
 */
class ObjInStateCond : public CondDlgBase
{

public:
	ObjInStateCond(DWORD id, CWnd* pParent = NULL);
	virtual ~ObjInStateCond();

// Dialog Data
	enum { IDD = IDD_DLG_COND_OBJ_IN_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	/// ������ ��� ������� � ������� � ������������� ������ ���������
	afx_msg virtual void OnCbnSelchangeComboObject();
	/// ������ ��� ��������� � �������
	afx_msg virtual void OnCbnSelchangeComboStates();
	/// ������������� ������ ��������
	afx_msg virtual void OnCbnSelchangeComboScenes();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();

	bool IsCondDataComplete();
	/// ��������� ������ �������� �� ��������� �����
	bool LoadObjects();
	/// ��������� ������ ��������� ���������� �������
	virtual bool LoadStates(const qdGameObjectAnimated* pa);
	/// ��������� ������ ����
	bool LoadScenes();
	/// ���������� ��������� �����
	class qdGameScene* get_scene();
	CCJFlatComboBox& getStatesCombo(){
		return m_wndStates;
	}
protected:
	/// ������ ��������
	CCJFlatComboBox m_wndObjs;
	/// ������ ���������
	CCJFlatComboBox m_wndStates;
	/// ������ ����
	CCJFlatComboBox m_wndScenes;
	/// ������� � ������ ����
	CStatic m_wndSceneStatic;
};
