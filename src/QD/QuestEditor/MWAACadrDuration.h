#pragma once


// MWAACadrDuration dialog

#include "wizardbasepage.h"
/**
	Страница, на которой задается продолжительность кадра
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

	/// Считывает из ини-файла предыдущее значение продолжительности
	void ReadDurFromIni();
	/// Записывает в ини-файл значение продолжительности
	void WriteDurToIni();
private:
	/// Содрежит указанную продолжительность кадра
	float m_fDuration;
};
