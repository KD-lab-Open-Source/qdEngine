#pragma once

#include "CondDlgBase.h"
#include "afxcmn.h"
// KeyPressedCondDlg dialog

/**
	Нажата ли клавиша
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
	/// Перехватывает нажатие клавиши. Определяет что это было. 
	///  Записывает клавишу в условие
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

	bool IsCondDataComplete();
private:
};
