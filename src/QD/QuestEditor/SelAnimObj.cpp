// SelAnimObj.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "qd_game_scene.h"
#include "qd_game_object.h"

#include "SelAnimObj.h"
#include "listctrl_insert_iterator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace listCtrlInserter{

	/**
		Основное назначение реализация функции insert для вставки qdGameObject
		в лист
	 */
	template<>
	struct data_traits<qdGameObject*>{
		inline static int insert(CListCtrl* ctrl, int pos, const qdGameObject* data){
			pos = ctrl->InsertItem(pos, data->name());

			if (pos != -1) {
				ctrl->SetItemData(pos, reinterpret_cast<DWORD_PTR>(data));
			}
			return pos;
		}
	};
}
/////////////////////////////////////////////////////////////////////////////
// SelAnimObj dialog

/**
	Показывает диалог для выбора объекта из сцены
	std::pair.first - true если нажали ОК, false в противном случае
	std::pair.second - выбранный объект. NULL если объект надо создать
 */
std::pair<bool, qdGameObject*> AskForObject(qdGameScene* ptrScene, 
											int type, bool bCanCrtNew)
{
	std::pair<bool, qdGameObject*> res(false, NULL);
	SelGameObj dlg;
	dlg.set_data(ptrScene);
	dlg.set_can_crt_new(bCanCrtNew);
	dlg.set_type(type);
	if(dlg.DoModal() == IDOK)
	{
		res.second = dlg.get_object();
		res.first = true;
	}
	return res;
}

SelGameObj::SelGameObj(CWnd* pParent /*=NULL*/)
	: CDialog(SelGameObj::IDD, pParent)
{
	//{{AFX_DATA_INIT(SelAnimObj)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_ptrObj = NULL;
	m_ptrScene = NULL;
	m_bCanCrtNew = true;
}

bool SelGameObj::set_data(qdGameScene* ptrScene)
{
	m_ptrScene = ptrScene;
	return (m_ptrScene != NULL);
}

void SelGameObj::set_type(int type)
{
	m_type = type;
}

void SelGameObj::set_can_crt_new(bool bf)
{
	m_bCanCrtNew = bf;
}

void SelGameObj::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelAnimObj)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelGameObj, CDialog)
//{{AFX_MSG_MAP(SelAnimObj)
ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelAnimObj message handlers

void SelGameObj::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if(pos)
	{
		int iItem = m_list.GetNextSelectedItem(pos);
		if(iItem != -1)
			m_ptrObj = reinterpret_cast<qdGameObject*>(m_list.GetItemData(iItem));
		EndDialog(IDOK);
	}
	*pResult = 0;
}

void SelGameObj::OnOK() 
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if(pos)
	{
		int iItem = m_list.GetNextSelectedItem(pos);
		if(iItem != -1)
			m_ptrObj = reinterpret_cast<qdGameObject*>(m_list.GetItemData(iItem));
	}
	
	CDialog::OnOK();
}

bool SelGameObj::LoadList()
{
	const qdGameObjectList& gol = m_ptrScene->object_list();
	qdGameObjectList::const_iterator _itr = gol.begin(), _end = gol.end();
	listCtrlInserter::listCtrl_insert_iterator Iter(m_list, 0);
	while ((_itr = std::find_if(_itr, _end, is_right_type(m_type))) != _end) {
		Iter = *_itr;
		++_itr;
		++Iter;
	}
	if (m_bCanCrtNew)
	{
		int iItem = m_list.InsertItem(0, CString((LPTSTR)IDS_ITEM_CRT_NEW_ANIM_OBJECT));
		if(iItem == -1)
			return false;
	}
	m_list.SetItemState(0, LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	return true;
}

BOOL SelGameObj::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_list.InsertColumn(0, "");
	LoadList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
