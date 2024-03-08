#pragma once


//#include "MovementTypeManager.h"
// MovementTypeSelectorDlg dialog
class qdGameObjectStateWalk;

/// Позволяет указать для походки movement_type
class MovementTypeSelectorDlg : public CDialog
{
	DECLARE_DYNAMIC(MovementTypeSelectorDlg)

public:
	MovementTypeSelectorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MovementTypeSelectorDlg();

	/// Назначает походку
	void set_data(qdGameObjectStateWalk* pstate);

// Dialog Data
	enum { IDD = IDD_DIALOG_MOVEMENT_TYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
private:
//	MovementTypeManager m_movementTypeManger;
	/// Для связи с котролами. Хранит выбор пользователя
	int m_iMovementType;
	/// Походка, для которой выбираем тип движения
	qdGameObjectStateWalk* m_ptrObj;
};
