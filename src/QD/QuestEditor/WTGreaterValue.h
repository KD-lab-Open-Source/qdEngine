#pragma once


// WTGreaterValue dialog

#include "WTCondDlgBase.h"

/**
	Время выполнения больше заданного значения
 */
class WTGreaterValue : public WTCondDlgBase
{
	DECLARE_DYNAMIC(WTGreaterValue)
public:
	WTGreaterValue(DWORD condId, CWnd* pParent = NULL);   // standard constructor
	virtual ~WTGreaterValue();

// Dialog Data
	enum { IDD = IDD_DLG_COND_WORK_TIME_GREATER_VALUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	/// Устанавливает m_fValue в условие
	afx_msg void OnDeltaposSpinValue(NMHDR *pNMHDR, LRESULT *pResult);
	bool IsCondDataComplete();
	/// Возвращает значение нижней грани
	float getValue() const{
		return m_fValue;
	}
private:
	/// Нижняя грань для "времени выполнения"
	float m_fValue;
};
