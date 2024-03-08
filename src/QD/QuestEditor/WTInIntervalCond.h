#pragma once


// WTInIntervalCond dialog
#include "WTGreaterValue.h"
/**
	Рабочее время состояния в интервале
 */
class WTInIntervalCond : public WTGreaterValue
{
	DECLARE_DYNAMIC(WTInIntervalCond)

public:
	WTInIntervalCond(DWORD condId, CWnd* pParent = NULL);   // standard constructor
	virtual ~WTInIntervalCond();

// Dialog Data
	enum { IDD = IDD_DLG_COND_WORK_TIME_IN_INTERVAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	/// Кладется в условие верхняя грань
	afx_msg void OnDeltaposSpinValue2(NMHDR *pNMHDR, LRESULT *pResult);

	bool IsCondDataComplete();
private:
	/// Верхняя грань для рабочего времени. Нижняя берется из родителя
	float m_fMaxValue;
};
