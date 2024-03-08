#pragma once

#include "BasePage.h"
#include <boost/scoped_ptr.hpp>
// InterfSldrStateProp dialog

/**
	��������� ��������� ��������
 */
enum eStateChoose{
	SC_BACK, ///< ��������� ����������
	SC_SLIDER///< �������� ���������
};
/**
	�������� ��������� ��������
 */
class InterfSldrStateProp : public CBasePage
{
	DECLARE_DYNAMIC(InterfSldrStateProp)

public:

	InterfSldrStateProp(eStateChoose stc, UINT idCap);
	virtual ~InterfSldrStateProp();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_SLIDER_STATE };

	/// ��������� �������, ��� ������
	void set_data(class qdInterfaceSlider* pslider);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedBtnSelAnim();
	afx_msg void OnBnClickedBtnSelSound();
	afx_msg void OnBnClickedCheck();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnDeltaposSpinOffsX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinOffsY(NMHDR *pNMHDR, LRESULT *pResult);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	/// ��������� � �������� ������� 
	void state2wnd();
	/// ������ �� ��������� ������� � ���������
	void wnd2state();

	/// ����������� ��������� ����
	void invert_flag(int flag);
	/// ������������� ���� ��������
	void reset_anim();
	/// ������������� �������� ����
	void reset_sound();
	/// ��������� ���������� ��������� � ��������
	void update_state(const class qdInterfaceElementState & pstate);
private:
	/// ���� ��������
	CString m_strAnimFile;
	/// �������� ����
	CString m_strSoundFile;
	/// � ����� ���������� �������� ��������
	eStateChoose const m_eStateChoose;

	/// �������� ��������
	Vect2i backgroundOffset_;
	/// ��������� ����� �������� ��������
	Vect2i backgroundOffsetBack_;

	/// ��������� �����
	boost::scoped_ptr<class qdInterfaceElementState> m_ptrBack;
	/// ���������, � ������� �������� ��� ���������
	/**
		��� ��������� �����, ������ ��� ������� �� ���� ������������ 
		������������� ���� ��������� ��������
	 */
	boost::scoped_ptr<class qdInterfaceElementState> m_ptrForChange;
	/// �������
	qdInterfaceSlider* m_ptrSlider;
};
