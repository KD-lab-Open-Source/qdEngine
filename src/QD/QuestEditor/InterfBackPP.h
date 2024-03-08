#pragma once
#include "basepage.h"

// InterfBackPP dialog
class qdInterfaceBackground;

/*!
	����� ������� ������� ��� ������������� ����
*/
class InterfBackPP : public CBasePage
{
	DECLARE_DYNAMIC(InterfBackPP)

public:
	InterfBackPP();
	virtual ~InterfBackPP();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_BACK_PROP };

	//! ��������� ���, �������� ����� ��������
	bool set_data(qdInterfaceBackground* ptrBack);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedButton1();

	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnBnClickedCheck();

	DECLARE_MESSAGE_MAP()
private:
	//! ������ �� ������� � ������
	void ele2wnd();
	//! ������ �� ������� � ������
	void wnd2ele();

	void OnChangePos();
	//! ������������� ���� ��������
	void reload_file();
private:
	//! Image file
	CString m_strFileName;
	//! �
	int m_iX;
	//! �
	int m_iY;
	//! �������
	int m_iD;

	//! �������� ������
	qdInterfaceBackground* m_ptrBack;
	//! ����� ��� ����, ����� ���� ����������� ��������
	std::auto_ptr<qdInterfaceBackground> m_ptrCopy;
	//! ����������� �� ��������
	BOOL m_bLoop;
	//! ����������� �� �� �����������
	BOOL m_bFlipH;
	//! ����������� �� �� ���������
	BOOL m_bFlipV;
};
