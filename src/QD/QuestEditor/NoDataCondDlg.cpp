// NoDataCondDlg.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "NoDataCondDlg.h"
#include "qd_game_object_state.h"
#include "condition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NoDataCondDlg dialog


NoDataCondDlg::NoDataCondDlg(DWORD id, CWnd* pParent /*=NULL*/)
:CondDlgBase(NoDataCondDlg::IDD, id, pParent)
{
	//{{AFX_DATA_INIT(NoDataCondDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void NoDataCondDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NoDataCondDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NoDataCondDlg, CDialog)
	//{{AFX_MSG_MAP(NoDataCondDlg)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NoDataCondDlg message handlers
bool NoDataCondDlg::IsCondDataComplete()
{
	return true;
}

void NoDataCondDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	if (bShow)
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

bool NoDataCondDlg::CanAdd(const qdConditionalObject* pobj)
{
	size_t count = pobj->conditions_count();
	for(int i = 0; i < count; ++i)
	{
		const qdCondition* pc = pobj->get_condition(i);
		if (pc->type() == get_condtion().type()
			&&pc->is_inversed() == get_condtion().is_inversed())
			return false;
	}
	return true;
}
