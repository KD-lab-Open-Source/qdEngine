#pragma once

#include "basepage.h"
#include "afxwin.h"

// InterfElePosPage dialog
class qdInterfaceDispatcher;
class qdInterfaceElement;

/**
	�������� ������ �������� ������������� ��� ��������� �� ������
 */
class InterfElePosPage : public CBasePage
{
	DECLARE_DYNAMIC(InterfElePosPage)

public:
	InterfElePosPage();
	InterfElePosPage(UINT dlgId);
	virtual ~InterfElePosPage();

	/// ��������� ��������� ���������� � �������, � ������� ����� ��������
	void set_data(qdInterfaceDispatcher* pd, qdInterfaceElement* ptrEle);
// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_ELE_POS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedCheckLeftTop();
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()

private:
	/// \brief ��������� ������ � ������� �� �������� � ����������
	/// ��������� � ���������� �������
	void ele2wnd();
	/// ��������� ������ �� ���������� ��������� � ���������� ������� � �������
	void wnd2ele();
	/// ������ ��������� ����� ������, ����� ����� ���� ������� ��� �����
	void real2bak();
	/// ��������������� ������ �� ��������� �����
	void bak2real();

	void OnChangePos();
private:
	/// ���������� �� ���������� ������ �������� ����
	BOOL m_bLeftTop;
	/// �
	int m_iX;
	/// y
	int m_iY;
	/// �������
	int m_iD;

	/// ��������� �
	int m_iXBak;
	/// ��������� �
	int m_iYBak;
	/// ��������� �������
	int m_iDBak;

	/// ��������� ����������
	qdInterfaceDispatcher* m_ptrIDisp;
	/// ������� � ��������, �������� ��������
	qdInterfaceElement* m_ptrEle;
};
