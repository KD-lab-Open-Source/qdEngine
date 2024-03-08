// MGInStateCond.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "MGInStateCond.h"
#include "MakeCondDlgBase.h"

#include "qd_minigame.h"
// MGInStateCond dialog

VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);

//IMPLEMENT_DYNAMIC(MGInStateCond, CondDlgBase)

MGInStateCond::MGInStateCond(DWORD id, CWnd* pParent /*=NULL*/)
: CondDlgBase(MGInStateCond::IDD, id, pParent)
{
}

MGInStateCond::~MGInStateCond()
{
}

void MGInStateCond::DoDataExchange(CDataExchange* pDX)
{
	CondDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MGS, m_wndMGs);
	DDX_Control(pDX, IDC_COMBO_STATES, m_wndStates);
}


BEGIN_MESSAGE_MAP(MGInStateCond, CondDlgBase)
	ON_CBN_SELCHANGE(IDC_COMBO_MGS, OnCbnSelchangeComboMgs)
	ON_CBN_SELCHANGE(IDC_COMBO_STATES, OnCbnSelchangeComboStates)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// MGInStateCond message handlers

BOOL MGInStateCond::OnInitDialog()
{
	CondDlgBase::OnInitDialog();

	LoadMiniGames();

	if (IsCondDataComplete()) 
		Data2Cond();

	AdjustListWidth(&m_wndMGs);
	AdjustListWidth(&m_wndStates);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void MGInStateCond::OnCbnSelchangeComboMgs()
{
	if (!m_wndMGs.GetCount()) 
		return;
	qdMiniGame* pmg = 
		static_cast<qdMiniGame*>(m_wndMGs.GetItemDataPtr(m_wndMGs.GetCurSel()));

	LoadMiniGameStates(pmg);
	Data2Cond();
}

void MGInStateCond::OnCbnSelchangeComboStates()
{
	Data2Cond();
}

VOID MGInStateCond::LoadMiniGames()
{
	qdGameDispatcher* ptrGD = m_ptrMakeCondDlg->get_disp();
	const qdMiniGameList& mgl = ptrGD->minigame_list();
	qdMiniGameList::const_iterator i = mgl.begin(), e = mgl.end();
	for(; i != e; ++i){
		qdMiniGame* pmg = *i;
		int res = m_wndMGs.AddString(pmg->name());
		if (res != CB_ERR)
			m_wndMGs.SetItemDataPtr(res, pmg);
	}
	if(m_wndMGs.GetCount())
	{
		m_wndMGs.SetCurSel(0);
		LoadMiniGameStates(mgl.front());
	}
	else{
		m_wndMGs.EnableWindow(FALSE);
		m_wndStates.EnableWindow(FALSE);
	}
}

VOID MGInStateCond::LoadMiniGameStates(qdMiniGame* pg)
{
	size_t nStatesCount = pg->states_count();
	for(int i = 0; i < nStatesCount; ++i){
		const qdMiniGameState* pmgs = pg->get_state(i);
		int res = m_wndStates.AddString(pmgs->name());
		if (res != CB_ERR)
			m_wndStates.SetItemDataPtr(res, const_cast<qdMiniGameState*>(pmgs));
	}
	m_wndStates.SetCurSel(0);
}

VOID MGInStateCond::Data2Cond(){
	m_qdCondition.put_object(qdCondition::MINIGAME_NAME, 
		reinterpret_cast<qdNamedObject*>(m_wndMGs.GetItemDataPtr(m_wndMGs.GetCurSel())));

	CString str;
	m_wndStates.GetLBText(m_wndStates.GetCurSel(), str);
	m_qdCondition.put_value(qdCondition::MINIGAME_STATE_NAME, str);

	m_ptrMakeCondDlg->OnCondDataComplete(true);
}

void MGInStateCond::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CondDlgBase::OnShowWindow(bShow, nStatus);
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

bool MGInStateCond::IsCondDataComplete()
{
	return static_cast<bool>(m_wndMGs.IsWindowEnabled());
}
