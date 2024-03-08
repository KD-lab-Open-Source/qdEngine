#pragma once

#include "AppSettingsBasePage.h"
#include "afxwin.h"
// SceneViewStngsPP dialog

/**
	��������� �������-����������, ������� ��������� ������� �����
	��������� ����������, �� ������� ������ ������� ������� ����������� 
	������� ����� � ��������������� �����������
 */
class SceneViewStngsPP : public AppSettingsBasePage
{
//	DECLARE_DYNAMIC(SceneViewStngsPP)

public:
	SceneViewStngsPP();
	virtual ~SceneViewStngsPP();

// Dialog Data
	enum { IDD = IDD_PO_PROPPAGE_SCENE_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	afx_msg void OnEnChangeEdit();

	DECLARE_MESSAGE_MAP();

private:

	/// �������� ������� ���� � ���� �����
	UINT m_iTopBottom;
	/// �������� ������� ����� � ������ �����
	UINT m_iLeftRight;
};
