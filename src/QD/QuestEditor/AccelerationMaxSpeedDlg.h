#pragma once

class qdGameObjectStateWalk;
// AccelerationMaxSpeedDlg dialog

///	Диалог задающий для походки ускорение и максимальную скорость
class AccelerationMaxSpeedDlg : public CDialog
{
	DECLARE_DYNAMIC(AccelerationMaxSpeedDlg)

public:
	AccelerationMaxSpeedDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~AccelerationMaxSpeedDlg();

// Dialog Data
	enum { IDD = IDD_DLG_ACCELERATION_MAX_SPEED };

	/// Устанавливает походку, для которой будем выставлять параметры
	void set_data(qdGameObjectStateWalk* pstate);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pmsg);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDeltaposSpinAcceleration(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinMaxSpeed(NMHDR *pNMHDR, LRESULT *pResult);


	DECLARE_MESSAGE_MAP()
private:
	float m_fAcceleration;
	float m_fMaxSpeed;
	qdGameObjectStateWalk* m_ptrState;
};
