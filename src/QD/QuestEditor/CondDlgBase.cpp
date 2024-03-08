// CondDlgBase.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "CondDlgBase.h"
#include "condition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CondDlgBase dialog

IMPLEMENT_DYNAMIC(CondDlgBase, CDialog);
CondDlgBase::CondDlgBase(UINT nIDTemplate, DWORD id, CWnd* pParentWnd /* = NULL */)
	: CDialog(nIDTemplate, pParentWnd), m_qdCondition(get_type(id))
{
	//{{AFX_DATA_INIT(CondDlgBase)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_qdCondition.inverse(get_invers(id));
}


void CondDlgBase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CondDlgBase)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CondDlgBase, CDialog)
	//{{AFX_MSG_MAP(CondDlgBase)
	//}}AFX_MSG_MAP
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CondDlgBase message handlers


BOOL CondDlgBase::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

bool CondDlgBase::LoadScenes(CCJFlatComboBox& box, CWnd& sceneStatic,
							 eShowHide eAct/* = SH_NONE*/){
	qdGameDispatcher* ptrGD = qd_get_game_dispatcher();
	ASSERT(ptrGD);
	const qdGameSceneList& gsl = ptrGD->scene_list();
	if(gsl.empty())
	{
		box.SetCurSel(-1);
		box.EnableWindow(FALSE);
		return false;
	}

	qdGameSceneList::const_iterator itr = gsl.begin(), end = gsl.end();
	for(;itr != end; ++itr){
		int ires = box.AddString((*itr)->name());
		if (ires != CB_ERR)
			box.SetItemDataPtr(ires, *itr);
	}
	if (eAct == SH_SHOWHIDE)
	{
		if (qdGameScene* pscene = m_ptrMakeCondDlg->get_scene())
		{// —цена известна
			box.SelectString(-1, pscene->name());
//			box.ShowWindow(SW_HIDE);
//			sceneStatic.ShowWindow(SW_HIDE);
			sceneStatic.ShowWindow(SW_SHOW);
			box.ShowWindow(SW_SHOW);
		}
		else{
			sceneStatic.ShowWindow(SW_SHOW);
			box.ShowWindow(SW_SHOW);
			box.SetCurSel(0);
		}
	}
	return true;
}

void CondDlgBase::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	if (bShow)
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}
