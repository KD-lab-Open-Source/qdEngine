#pragma once

#include "qd_interface_text_window.h"
// InterfaceTextWindowPP dialog
#include "BasePage.h"
#include "afxwin.h"

#include "custom controls\CJColorPicker.h"

/*!
	������ ������ ��� ���� �������
*/
class InterfaceTextWindowPP : public CBasePage
{
	DECLARE_DYNAMIC(InterfaceTextWindowPP)

public:
	InterfaceTextWindowPP();
	virtual ~InterfaceTextWindowPP();

	//! ��������� ���� �������, ������� ����� �������������
	void set_data(qdInterfaceTextWindow* obj);
// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_TEXT_WINDOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG*);
	afx_msg void OnBnClickedButtonSelBackground();
	afx_msg void OnCbnSelchangeComboSliderNames();
	afx_msg void OnEnKillfocusEdit();
	afx_msg void OnDeltaposSpinX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinD(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLength(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinPlayer(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
protected:
	//! ������ �� ������� ������������ � ������
	void wnd2obj();
	//! ������ �� ������� ������������ � ������
	void obj2wnd();
	//! ������ ��������� ���������
	void loadSliderList();

	void InitFontBox(CCJFlatComboBox& box);
private:
	//! �������� ����, ���������� � set_data
	qdInterfaceTextWindow* sourceObj_;
	//! ������-����� ���������, ����� ����� ���� �������� ���������
	qdInterfaceTextWindow backUp_;
	//! ���������� �
	int x_;
	//! ���������� �
	int y_;
	//! ��������� Z
	int z_;
	//! ������
	int cx_;
	//! ������
	int cy_;
	//! ��� ����� � ��������� ��� ����
	CString backGround_;
	//! ��� ��������
	CString sliderName_;
	//! ������ ��������� ���������
	CCJFlatComboBox slidersCombo_;
	CCJColorPicker backColorWnd_;
	BOOL enableBackColor_;
	int backColorAlpha_;
	CCJFlatComboBox typesCombo_;
	int windowType_;

	/// ������������ � ���� �����
	CString text_;
	//! ������� ������ ����� ������
	CCJColorPicker textColorWnd_;
	//! ������� ������ hover color
	CCJColorPicker hiliteColorWnd_;
	//! ������ ������
	CCJFlatComboBox fontsWnd_;
	//! ������������ ������
	CCJFlatComboBox alignWnd_;
	/// ������������ ����� �������� ������.
	int stringLength_;

	//! ������������ �������� �� ���������
	CCJFlatComboBox dialogAlignWnd_;

	CCJFlatComboBox infoTypeWnd_;
	int playerID_;

public:
	afx_msg void OnBnClickedCheckFill();
};
