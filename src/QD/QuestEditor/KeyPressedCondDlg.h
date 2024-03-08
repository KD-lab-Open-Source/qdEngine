#pragma once

#include "CondDlgBase.h"
#include "afxcmn.h"
// KeyPressedCondDlg dialog

/**
	������ �� �������
 */
class KeyPressedCondDlg : public CondDlgBase
{
	DECLARE_DYNAMIC(KeyPressedCondDlg)

public:
	KeyPressedCondDlg(DWORD condId, CWnd* pParent = NULL);   // standard constructor
	virtual ~KeyPressedCondDlg();

// Dialog Data
	enum { IDD = IDD_DLG_COND_KEYPRESSED };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	/// ������������� ������� �������. ���������� ��� ��� ����. 
	///  ���������� ������� � �������
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

	bool IsCondDataComplete();
private:
};
