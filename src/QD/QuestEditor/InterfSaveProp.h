#pragma once

#include "basepage.h"
#include <boost/scoped_ptr.hpp>
#include "afxwin.h"
#include "qd_interface_element_state.h"

// InterfSaveProp dialog

class qdInterfaceSave;

/**
	�������� qdInterfaceSave
 */
class InterfSaveProp : public CBasePage
{
	DECLARE_DYNAMIC(InterfSaveProp)

public:
	InterfSaveProp();
	virtual ~InterfSaveProp();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_SAVE_PROP };

	/// ��������� ����, � ������� ����� ��������
	void set_data(qdInterfaceSave* psave);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedBtnSelSound();
	afx_msg void OnBnClickedBtnSelAnim();
	afx_msg void OnBnClickedCheckHorzInv();
	afx_msg void OnBnClickedCheckVertInv();
	afx_msg void OnBnClickedCheckLoop();
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboModes();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	/// ������ �� ��������� � ����
	void wnd2save();
	/// ������ �� ����� � �������� �������
	void save2wnd();

	/// ������ ������ � �������� �������
	void mode2wnd(qdInterfaceElementState::state_mode_t const md);
	/// ������ �� ��������� ������� � �����
	void wnd2mode(qdInterfaceElementState::state_mode_t const md);
	/// ��������� ��������� ������
	void LoadModes();
	/// ������������� �������� ���� � ���� �������� ��� �����
	void reset_files();
	/// ������� �� ��������� ������� ��� ��������
	void OnChangePosSize();

	/// ���������� ������� ����� ��� �����
	qdInterfaceElementState::state_mode_t GetCurMode();
	/// ���������� ����� �������� �� �������(����� �� 2�)
	qdInterfaceElementState::state_mode_t GetAnotherMode(
		qdInterfaceElementState::state_mode_t const m);
private:
	//! ���� ��������
	CString m_strAnimFile;
	//! �������� ����
	CString m_strSoundFile;
	/// �
	int m_iX;
	/// �
	int m_iY;
	/// �������
	int m_iD;
	/// ������ �� �����������
	int m_iSX;
	/// ������ �� ���������
	int m_iSY;
	//! �������� ������ ��� ����������� �����
	int m_iTextDx;
	int m_iTextDy;

	int isAutosaveSlot_;

	/// ����, � ������� ��������
	qdInterfaceSave* m_ptrSourceSave;
	/// ��������� �����
	boost::scoped_ptr<qdInterfaceSave> m_ptrBack;
	//! ������������� �����
	INT m_iSaveId;
	//! ��������� ��������������� ���� � ���������
	CCJFlatComboBox m_wndModes;
};
