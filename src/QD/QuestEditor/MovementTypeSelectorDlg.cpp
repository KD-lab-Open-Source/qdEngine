// MovementTypeSelectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "MovementTypeSelectorDlg.h"
#include "qd_game_object_state.h"

/// «апускает дл€ походки диалог выбора movement_type.
/**
	\return true - если нажали OK
 */
bool SelectMovementType(qdGameObjectStateWalk* pstate)
{
	MovementTypeSelectorDlg dlg;
	dlg.set_data(pstate);
	return (dlg.DoModal() == IDOK);
}

// MovementTypeSelectorDlg dialog

IMPLEMENT_DYNAMIC(MovementTypeSelectorDlg, CDialog)
MovementTypeSelectorDlg::MovementTypeSelectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MovementTypeSelectorDlg::IDD, pParent)
	, m_iMovementType(-1)
	, m_ptrObj(NULL)
{
}

MovementTypeSelectorDlg::~MovementTypeSelectorDlg()
{
}

void MovementTypeSelectorDlg::set_data(qdGameObjectStateWalk* pstate)
{
	m_ptrObj = pstate;
}
void MovementTypeSelectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_MOVEMENT_LEFT, m_iMovementType);
}


BEGIN_MESSAGE_MAP(MovementTypeSelectorDlg, CDialog)
END_MESSAGE_MAP()


// MovementTypeSelectorDlg message handlers

BOOL MovementTypeSelectorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_iMovementType = static_cast<int>(m_ptrObj->movement_type());
	UpdateData(FALSE);
	return TRUE;
}

void MovementTypeSelectorDlg::OnOK()
{
	UpdateData();
//	m_ptrObj->set_movement_type(m_movementTypeManger[m_iMovementType]->movement_type());
	m_ptrObj->set_movement_type(
		static_cast<qdGameObjectStateWalk::movement_type_t>(m_iMovementType));
	CDialog::OnOK();
}