#pragma once
#include "basepage.h"

#include "custom controls\CJFlatComboBox.h"
#include "custom controls\CJColorPicker.h"

// InterfCounterPP dialog
class qdInterfaceCounter;

/*!
	����� ������� ������� ��� ������������� ��������
*/
class InterfCounterPP : public CBasePage
{
	DECLARE_DYNAMIC(InterfCounterPP)

public:
	InterfCounterPP();
	virtual ~InterfCounterPP();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_COUNTER_PROP };

	bool set_data(qdInterfaceCounter* ptrBack);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()

private:

	//! ������ �� ������� � ������
	void ele2wnd();
	//! ������ �� ������� � ������
	void wnd2ele();

	bool loadCounters(CCJFlatComboBox& box);
	bool loadFonts(CCJFlatComboBox& box);

	void OnChangePos();

private:
	//! �
	int m_iX;
	//! �
	int m_iY;
	//! �������
	int m_iD;

	/// ���������� �������� ��� �����������
	int digits_;

	//! ������� ������ ����� ������
	CCJColorPicker textColorWnd_;
	//! ������ ������
	CCJFlatComboBox fontsWnd_;

	/// ������� ��� ������ ��������
	CCJFlatComboBox counters_;

	//! �������� ������
	qdInterfaceCounter* m_ptrCounter;
	//! ����� ��� ����, ����� ���� ����������� ��������
	std::auto_ptr<qdInterfaceCounter> m_ptrCopy;
};
