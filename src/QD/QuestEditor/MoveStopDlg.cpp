// MoveStopDlg.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include <algorithm>
#include "chosename.h"
#include "gdi_GR_dispatcher.h"
#include "MovingDlg.h"
#include "qd_animation_set_preview.h"

#include "qd_game_object_moving.h"
#include "qd_game_object_state.h"

#include "name_op.h"
#include "ptree_api.h"
#include "ptree_hlprs.hpp"

#include "MoveStopDlg.h"

#ifdef USE_NEW_PROJ
#include "Main/ObjectTraits.h"
#endif // USE_NEW_PROJ

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MoveStopDlg dialog
void ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID = -1);
void ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID = -1);
bool AskUserForName(CString& strName);
#ifndef USE_NEW_PROJ
bool HasLastState(qdNamedObject* pno);
#endif // USE_NEW_PROJ

qdAnimationSet* CreateAnimationSet(qdGameDispatcherBase* ptrDispBase)
{
	CString strAnimSetName((LPTSTR)IDS_NEW_MOVING_NAME);

	if (!AskUserForName(strAnimSetName)) {
		return NULL;
	}
	strAnimSetName = get_unique_name(strAnimSetName, ptrDispBase->animation_set_list());
	
	qdAnimationSet* ptrRes = new qdAnimationSet;
	if(ptrRes)
	{
		ptrRes->set_name(strAnimSetName);
	}
	return ptrRes;
}

MoveStopDlg::MoveStopDlg(CWnd* pParent /*=NULL*/)
: CDialog(MoveStopDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(MoveStopDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bTreeChanged = false;
	m_bAsk4Del = false;
}

bool MoveStopDlg::set_data(qdGameDispatcher* ptrDisp, qdGameDispatcherBase* ptrGDB)
{
	m_ptrGameDisp = ptrDisp;
	m_ptrGameDispBase = ptrGDB;
	return true;
}

void MoveStopDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MoveStopDlg)
	DDX_Control(pDX, IDC_LIST_VARS, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MoveStopDlg, CDialog)
//{{AFX_MSG_MAP(MoveStopDlg)
ON_BN_CLICKED(IDC_BUTTON_VAR_EDIT2, OnButtonVarEdit2)
ON_BN_CLICKED(IDC_BUTTON_VAR_DEL, OnButtonVarDel)
ON_BN_CLICKED(IDC_BUTTON_VAR_ADD2, OnButtonVarAdd2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_VARS, OnDblclkListVars)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_VARS, OnKeydownListVars)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_LCE_ENDEDIT, IDC_LIST_VARS, OnEndEditList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MoveStopDlg message handlers

void MoveStopDlg::OnButtonVarEdit2() 
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos) {
		int iIndex = m_list.GetNextSelectedItem(pos);
		qdAnimationSet* ptrAS = (qdAnimationSet*)m_list.GetItemData(iIndex);
		m_bTreeChanged = EditAnimationSet(m_ptrGameDisp, ptrAS);
	}
	else
		ShowError(*this,IDS_NO_SELECTION_MSG, IDS_ERR_MSG_CAPTION);
}

void MoveStopDlg::OnButtonVarDel() 
{
	int iSelCount = m_list.GetSelectedCount();
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if(!pos)
		return;

	CString msg;
	CString cap;
	cap.LoadString(IDS_DEL_CAP);
	while (pos) {
		int iIndex = m_list.GetNextSelectedItem(pos);
		CString str = m_list.GetItemText(iIndex, 0);
		msg.Format(IDS_DEL_ANIMATION_MSG, (LPCTSTR)str);
		if(MessageBox(msg, cap, MB_OKCANCEL) == IDOK)
		{
			qdAnimationSet* pa = (qdAnimationSet*)m_list.GetItemData(iIndex);
			if(CanRemoveAS(pa->name()))
			{
				if(m_ptrGameDispBase->remove_animation_set(pa))
				{
					m_list.DeleteItem(iIndex);
					m_bTreeChanged = true;
					RemoveGaitFromPers();

					iIndex = min(iIndex, m_list.GetItemCount()-1);
					m_list.SetItemState(iIndex, LVIS_SELECTED|LVIS_FOCUSED,
						LVIS_SELECTED|LVIS_FOCUSED);
				}
			}
		}
	}
	AdjustColumnsWidth();
}

void MoveStopDlg::OnButtonVarAdd2() 
{
	qdAnimationSet* ptrAS = CreateAnimationSet(m_ptrGameDispBase);
	if(ptrAS)
	{
		m_ptrGameDispBase->add_animation_set(ptrAS);
		InsertAnimationSet(m_list.GetItemCount(), ptrAS);
		AdjustColumnsWidth();
		m_bTreeChanged = EditAnimationSet(m_ptrGameDisp, ptrAS);
//		if(m_bTreeChanged)
//			CheckScenes4Pers(ptrAS);
	}
}

BOOL MoveStopDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InsertColumns();
	LoadAnimationSets();
	AdjustColumnsWidth();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool MoveStopDlg::InsertColumns()
{
	CString str;
	str.LoadString(IDS_CLMN_MOVING_NAME);
	m_list.InsertColumn(0,str, LVCFMT_LEFT, 80);
	m_list.SetEditCtrlType(0, ELC_ITEM_EDIT_CTRL_EDIT);
	return true;
}

bool MoveStopDlg::LoadAnimationSets()
{
	const qdAnimationSetList& asl = m_ptrGameDispBase->animation_set_list();
	qdAnimationSetList::const_iterator _itr = asl.begin(), _end = asl.end();
	int i = 0;
	for(;_itr != _end; ++_itr)
	{
		InsertAnimationSet(i, *_itr);
	}
	return true;
}

void MoveStopDlg::AdjustColumnsWidth()
{
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}

bool MoveStopDlg::InsertAnimationSet(int iPos, qdAnimationSet* ptr)
{
	int iItem = m_list.InsertItem(iPos, ptr->name());
	if(iItem == -1)
		return false;
	return (bool)m_list.SetItemData(iItem, reinterpret_cast<DWORD_PTR>(ptr));
}

void MoveStopDlg::OnDblclkListVars(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos) {
		int iIndex = m_list.GetNextSelectedItem(pos);
		qdAnimationSet* ptrAS = (qdAnimationSet*)m_list.GetItemData(iIndex);
		m_bTreeChanged = EditAnimationSet(m_ptrGameDisp, ptrAS);
	}
	*pResult = 0;
}

bool MoveStopDlg::EditAnimationSet(qdGameDispatcher* ptrGameDisp, qdAnimationSet* ptrAS)
{
	MovingDlg dlg;
	dlg.set_data(ptrGameDisp, ptrAS);
	return (dlg.DoModal() == IDOK);
}

void MoveStopDlg::OnEndEditList(NMHDR * pNMHDR, LRESULT* pResult)
{
	NM_LCE_INFO * pInfo = (NM_LCE_INFO*)pNMHDR;
	if (pInfo->iEditCtrlType == ELC_ITEM_EDIT_CTRL_EDIT) {
		if(uniq_name(pInfo->item.pszText, m_ptrGameDispBase->animation_set_list()))
		{
			qdAnimationSet* pa = (qdAnimationSet*)m_list.GetItemData(pInfo->item.iItem);
			
			ReplaceGait(pa->name(), pInfo->item.pszText);
				
			pa->set_name(pInfo->item.pszText);
			
			qdGameDispatcherBase* owner = (qdGameDispatcherBase*)pa->owner();
			owner->remove_animation_set(pa);
			owner->add_animation_set(pa);
			
			*pResult = TRUE;
		}
		else
		{
			CString str;
			str.Format(IDS_NAME_REPEATED, pInfo->item.pszText);
			ShowError(*this, str);
			*pResult = FALSE;
		}
	}
	
	AdjustColumnsWidth();
	SetProjChange();
}

bool MoveStopDlg::CheckScenes4Pers(qdAnimationSet* ptrASet)
{
	const qdGameSceneList& gsl = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator _i = gsl.begin(), _E = gsl.end();
	for(;_i != _E; ++_i)
		CheckScene4Pers(*_i, ptrASet);
	
	return true;
}

/// Имеет ли перс походку
bool HasWalk(qdGameObjectMoving* ptrObj)
{
	const qdGameObjectStateVector& v = ptrObj->state_vector();
	qdGameObjectStateVector::const_iterator _itr = v.begin(), _end = v.end();

	for(; _itr != _end; ++_itr)
	{
		if ((*_itr)->state_type() == qdGameObjectState::STATE_WALK) {
			return true;
		}
	}
	return false;
}

bool MoveStopDlg::CheckScene4Pers(qdGameScene* ptrScene, 
													qdAnimationSet* ptrASet)
{
	const qdGameObjectList& gol = ptrScene->object_list();
	qdGameObjectList::const_iterator _itr = 
		std::find_if(gol.begin(), gol.end(), ptree_helpers::is_obj_moving);
	if(_itr == gol.end())
	{
		qdGameObjectMoving* ptrPers = AddPersonage(ptrScene);
		if(ptrPers)
		{
			if (ptree_api::AddEmptyMovementState(ptrPers)) {
				static_cast<qdGameObjectStateWalk*>(ptrPers->state_vector().front())
					->animation_set_info()->set_name(ptrASet->name());
				ptrPers->set_state(0);
				ptrPers->auto_bound();
				ptrPers->adjust_z();
				ptrPers->quant(0);
				ptree_api::ReloadStates(ptrPers);
			}
		}
	}
	else
	{
		//ищем персонажи, у которых отсутствуют походочные состояния
		//и вставляем им ...
		while ((_itr = std::find_if(++_itr, gol.end(), ptree_helpers::is_obj_moving)) 
							!= gol.end()) {
			qdGameObjectMoving* ptrPers = static_cast<qdGameObjectMoving*>(*_itr);
			if(!HasWalk(ptrPers))
			{
				if (qdGameObjectStateWalk* ptrState 
					= ptree_api::InsertEmptyMovementState(ptrPers, 0)) 
				{
					ptrState->animation_set_info()->set_name(ptrASet->name());
					ptrPers->set_default_state();
					ptrPers->auto_bound();
					ptrPers->adjust_z();
					ptrPers->quant(0);
				}
			}
		}
	}
	return true;
}

qdGameObjectMoving* MoveStopDlg::AddPersonage(qdGameScene* ptrScene)
{
	CString name((LPTSTR)IDS_NEW_MOVING_OBJ_NAME);

	return ptree_api::AddPersonage(ptrScene, name, false);
}

inline bool IsObjectEmpty(qdGameObjectMoving* ptrObj)
{
	if (ptrObj->state_vector().empty()) {
		return true;
	}
	return false;
}

void MoveStopDlg::RemoveGaitFromPers()
{
	CWaitCursor __wait__;
	qdAnimationSet* ptrNewSet = NULL;
	if(!m_ptrGameDisp->animation_set_list().empty())
		ptrNewSet = m_ptrGameDisp->animation_set_list().front();
	
	GameObjectStateList_t::iterator _itr = m_listStatesAffected.begin(), _end = m_listStatesAffected.end();
	
	for(; _itr != _end; ++_itr)
	{
		qdGameObjectMoving* ptrObj = static_cast<qdGameObjectMoving*>((*_itr)->owner());
		if(ptrNewSet)
			static_cast<qdGameObjectStateWalk*>(*_itr)->
			animation_set_info()->set_name(ptrNewSet->name());
		else
		{
			ptree_api::DeleteObject(*_itr,false);
		}
	}
}


CString MoveStopDlg::DetectLastState()
{
	CString res;
	GameObjectStateList_t::iterator _itr = m_listStatesAffected.begin(), 
		_end = m_listStatesAffected.end();
	for(; _itr != _end; ++_itr)
	{
		qdGameObjectState* ptrState = *_itr;

#ifdef USE_NEW_PROJ
		if (ObjectTraits::hasNoStates(static_cast<qdGameObject*>(ptrState->owner())))
#else
		if(HasLastState(ptrState->owner()))
#endif // USE_NEW_PROJ
		{
			res += ptrState->owner()->name();
			res += _T(':');
			res += ptrState->owner()->owner()->name();
			res += _T('\n');
		}
	}
	return res;
}

bool MoveStopDlg::CanRemoveAS(LPCTSTR lpszName)
{
//	const qdGameLocationList& gll = m_ptrGameDisp->location_list();
//	gait_user_finder gu = std::for_each(gll.begin(), gll.end(), 
//		gait_user_finder(lpszName, &m_listStatesAffected));
	gait_user_finder gu(lpszName, &m_listStatesAffected);
	gu(m_ptrGameDisp);
	if(gu.get_strUsers().IsEmpty())
		return true;
	CString strMsg;
	strMsg.Format(IDS_GAIT_USED, static_cast<LPCTSTR>(gu.get_strUsers()));
	if(AfxMessageBox(strMsg, MB_OKCANCEL|MB_ICONWARNING)== IDOK)
	{
		if(m_ptrGameDisp->animation_set_list().size() == 1)
		{
			CString str = DetectLastState();
			if(!str.IsEmpty())
			{
				strMsg.Format(IDS_GAIT_USED2, lpszName, static_cast<LPCTSTR>(str));
				if(AfxMessageBox(strMsg, MB_OKCANCEL|MB_ICONWARNING)== IDOK)
					return true;
				return false;
			}
		}
		return true;
	}
	m_listStatesAffected.clear();
	return false;
}

/**
	Переименовывает состояния
 */
struct rename_gait {
	LPCTSTR _lpszNewName;
	rename_gait(LPCTSTR lpszNewName):_lpszNewName(lpszNewName){}
	void operator()(qdGameObjectState* ptrState){
		static_cast<qdGameObjectStateWalk*>(ptrState)->animation_set_info()->set_name(_lpszNewName);
	}
};
void MoveStopDlg::ReplaceGait(LPCTSTR lpszOldName, LPCTSTR lpszNewName)
{
//	const qdGameLocationList& gll = m_ptrGameDisp->location_list();
//	gait_user_finder gu = std::for_each(gll.begin(), gll.end(), 
//		gait_user_finder(lpszOldName, &m_listStatesAffected));

	gait_user_finder gu(lpszOldName, &m_listStatesAffected);
	gu(m_ptrGameDisp);
	std::for_each(m_listStatesAffected.begin(), m_listStatesAffected.end(), rename_gait(lpszNewName));
}

void MoveStopDlg::OnKeydownListVars(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	if (pLVKeyDow->wVKey == VK_DELETE) {
		if(::IsWindowVisible(*GetDlgItem(IDC_BUTTON_VAR_DEL)))
			OnButtonVarDel();
		else
			Beep(1000, 100);
	}
	
	*pResult = 0;
}
