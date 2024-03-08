#pragma once

#include "BasePage.h"
#include "afxwin.h"
#include "interfsliderpurposes.h"
// InterfSldrProp dialog

/**
	�������� ������������� ��������
 */
class InterfSldrProp : public CBasePage
{
	DECLARE_DYNAMIC(InterfSldrProp)

public:
	InterfSldrProp();
	virtual ~InterfSldrProp();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_SLIDER };

	/// �������, � ������� ����� ��������
	void set_data(class qdInterfaceSlider* pslider);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedOriHorz();
	afx_msg void OnBnClickedOriVert();

	afx_msg void OnDeltaposSpinX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinD(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboTarget();

	afx_msg void OnDeltaposSpinArSx(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinArSy(NMHDR *pNMHDR, LRESULT *pResult);

	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	/// ��������� ������ ��������� ���������� ��������
	void LoadPurposes();

	DECLARE_MESSAGE_MAP()

	/// ��������������� ������ �� ��������� �����
	void bak2real();
	/// ������ ��������� ����� ��� ������������ ��������������
	void real2bak();

	/// ������ �� ��������� ������� � �������
	void wnd2slider();
	/// ��������� ������� �������� �� ������
	void update_pos();
	//! ��������� �������� �������������
	void update_ar();

	/// ������ �� �������� � ���������� ��������� � ���������� �������
	void slider2wnd();

	/// ���������� �������� ��� �������� ���������� � ���������� ����������
	void purpose2wnd();
	/// ��������� � �������� ��������� � ���������� ����������
	void wnd2purpose();
private:
	/// �
	int m_iX;
	/// �
	int m_iY;
	/// �������
	int m_iD;
	/// ������ ��������� ��������������
	int m_iARSX;
	/// ������ ��������� ��������������
	int m_iARSY;

	//! �������������� (0), ������������ (1) ���������� ��������
	BOOL m_iOrient;
	BOOL inverseDirection_;
	//! ��� ���������� �������
	CCJFlatComboBox m_wndTarget;

	//�� �� ����� ������ ����������� ����� �������.
	//������ ��� �� ��� ������������ �� ������ ����������

	/// ��������� �
	int m_iXBak;
	/// ��������� �
	int m_iYBak;
	/// ��������� �������
	int m_iDBak;
	/// ��������� ����������
	BOOL m_iOrientBak;
	BOOL inverseDirectionBak_;
	/// ��������� ������ ��������� ��������������
	int m_iARSXBak;
	/// ��������� ������ ��������� ��������������
	int m_iARSYBak;
	/// ����� ����� ������������� ��� ����, ����� ����� ���� �������� ���������
	qdInterfaceElement::option_ID_t m_eOptionIdBak;

	/// �������
	class qdInterfaceSlider* m_pSlider;
	/// ������ ����������
	InterfSliderPurposes m_Purposes;
};
