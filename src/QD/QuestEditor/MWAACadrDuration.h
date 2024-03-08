#pragma once


// MWAACadrDuration dialog

#include "wizardbasepage.h"
/**
	��������, �� ������� �������� ����������������� �����
 */
class MWAACadrDuration : public WizardBasePage
{
	DECLARE_DYNAMIC(MWAACadrDuration)

public:
	MWAACadrDuration();
	virtual ~MWAACadrDuration();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_AA_CADR_PAUSE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardNext();

	afx_msg void OnDeltaposSpinDuration(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditPause();

	/// ��������� �� ���-����� ���������� �������� �����������������
	void ReadDurFromIni();
	/// ���������� � ���-���� �������� �����������������
	void WriteDurToIni();
private:
	/// �������� ��������� ����������������� �����
	float m_fDuration;
};
