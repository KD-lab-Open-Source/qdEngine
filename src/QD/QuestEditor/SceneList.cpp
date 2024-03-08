// SceneList.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"

#include "SceneList.h"
#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SceneList dialog


SceneList::SceneList(CWnd* pParent /*=NULL*/)
	: CDialog(SceneList::IDD, pParent)
{
	//{{AFX_DATA_INIT(SceneList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bShowCrtNew = true;
	m_bMultiSel = false;
}

bool SceneList::set_data(qdGameDispatcher* ptrDisp)
{
	m_ptrGameDisp = ptrDisp;
	return true;
}
void SceneList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SceneList)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SceneList, CDialog)
	//{{AFX_MSG_MAP(SceneList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SceneList message handlers

BOOL SceneList::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_bMultiSel) {
		ModifyStyle(LVS_SINGLESEL, 0);
	}
	
	InsertColumns();
	if(!LoadSceneList())
		return FALSE;
	AdjustColumnsWidth();
	m_list.SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool SceneList::InsertColumns()
{
	CString str((LPTSTR)IDS_CLMN_SCENE_NAME);
	m_list.InsertColumn(0, str);
	return true;
}

bool SceneList::LoadSceneList()
{
	int iSceneNum = 0;
	const qdGameSceneList& gsl = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator _beg = gsl.begin(), _end = gsl.end();
	for(; _beg != _end; ++_beg)
	{
		InsertScene(iSceneNum, *_beg);
		++iSceneNum;
	}
	if (m_bShowCrtNew) {
		//вставляем строку "Новая сцена"
		int iItem = m_list.InsertItem(0, CString((LPTSTR)IDS_ITEM_CRT_NEW_SCENE));
		if(iItem == -1)
			return false;
		m_list.SetItemState(0, LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
	}
	return true;
}

void SceneList::AdjustColumnsWidth()
{
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}

int SceneList::InsertScene(int nNum, qdGameScene* ptrScene)
{
	int iItem = m_list.InsertItem(nNum, ptrScene->name());
	if(iItem != -1)
		m_list.SetItemData(nNum, reinterpret_cast<DWORD>(ptrScene));
	
	return iItem;
}

bool SceneList::GetSelected()
{
	m_iScenesCount = m_list.GetSelectedCount();
	m_ptrScenes.reset(new qdGameScene*[m_iScenesCount]);
	memset(m_ptrScenes.get(), 0, sizeof(qdGameScene*)*m_iScenesCount);
	
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (!pos) {
		return false;
	}
	int i = 0;
	while(pos)
	{
		int iItem = m_list.GetNextSelectedItem(pos);
		if(iItem != -1)
		{
			m_ptrScenes[i] = reinterpret_cast<qdGameScene*>(
												m_list.GetItemData(iItem));
			++i;
		}
	}
	m_iScenesCount = i;
	return true;
}

void SceneList::OnOK() 
{
	GetSelected();
	CDialog::OnOK();
}

void SceneList::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (GetSelected()) {
		EndDialog(IDOK);
	}
	*pResult = 0;
}
