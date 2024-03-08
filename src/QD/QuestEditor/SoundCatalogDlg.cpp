// SoundCatalogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"

#include "qd_sound.h"
#include "qd_game_object_state.h"
#include "qd_game_object_animated.h"

#include "SoundCatalogDlg.h"
#include "DispsListsMerger.h"

#include <algorithm>

#include "ListCtrl_insert_iterator.h"
#include "CurDirSafer.h"
#include "name_op.h"
#include "relativepath.h"
#include "disk_op.h"

#include "ptree_api.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace listCtrlInserter{
	
	template<>
		struct data_traits<CString>{
		inline static int insert(CListCtrl* ctrl, int pos, const CString& data){
			return ctrl->InsertItem(pos, data);
		}
	};
}

/////////////////////////////////////////////////////////////////////////////
// SoundCatalogDlg dialog
/*
 *	внешние
 */
void ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID = -1);
void ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID = -1);
const CString AskUserForName(const CString& strDef);
bool HasLastEmptyState(qdNamedObject* pno);
void SetProjChange();

//////////////////////////////////////////////////////////////////////////
/*
 *	глобальные
 */
namespace{
	CurDirSafer g__cur_dir_safer__;
}
qdSound* AddNewSound(qdGameDispatcherBase* ptrDisp, 
					 const CString& strFileName, bool bAskName)
{
	CString str;
	str = disk_op::GetFileName(strFileName);

	if(bAskName)
		str = AskUserForName(str);

	str = get_unique_name(str, ptrDisp->animation_list());
	
	//создаём и настраиваем анимацию
	std::auto_ptr<qdSound> sound ( new qdSound);
	if(!sound.get())
		return NULL;
	
	sound->set_name(str);
	sound->set_file_name(strFileName);
	if (ptrDisp->add_sound(sound.get())) {
		SetProjChange();
		return sound.release();
	}
	
	return NULL;
}

bool GetSoundFileNames(HWND hWnd, CStringVect_t* ptrFiles)
{
	CString str;
	str.LoadString(IDS_OPEN_SOUND_EXT);
	CString str2;
	str2.LoadString(IDS_OPEN_SOUND_FILTER);
	CString strTitle((LPTSTR)IDS_OSD_CAP_SEL_SOUND);
	
	CString strNewFileName;
	DWORD dwFlags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST 
					| OFN_ALLOWMULTISELECT;

	return disk_op::AskUser4FileNames(hWnd, false, ptrFiles, 
		str, str2, strTitle, CString(), 
		g__cur_dir_safer__,dwFlags);
}

SoundCatalogDlg::SoundCatalogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SoundCatalogDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SoundCatalogDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bProjChanged = false;
}

bool SoundCatalogDlg::set_data(qdGameDispatcher* ptrGameDisp, 
							   qdGameDispatcherBase* ptrDisp)
{
	m_ptrGameDisp = ptrGameDisp;
	m_ptrDisp = ptrDisp;
	return true;
}

void SoundCatalogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SoundCatalogDlg)
	DDX_Control(pDX, IDC_LIST_VARS, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SoundCatalogDlg, CDialog)
//{{AFX_MSG_MAP(SoundCatalogDlg)
ON_BN_CLICKED(IDC_BUTTON_VAR_ADD2, OnButtonVarAdd2)
ON_BN_CLICKED(IDC_BUTTON_VAR_DEL, OnButtonVarDel)
ON_NOTIFY(NM_DBLCLK, IDC_LIST_VARS, OnDblclkListVars)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_VARS, OnEndlabeleditListVars)
	ON_BN_CLICKED(IDC_BUTTON_VAR_EDIT2, OnButtonVarEdit2)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_VARS, OnKeydownListVars)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SoundCatalogDlg message handlers

void SoundCatalogDlg::OnButtonVarAdd2() 
{
	CStringVect_t vecStrs;
	while(GetSoundFileNames(*this, &vecStrs))
	{
		if (vecStrs.size() == 1) {
			AddNewSound(m_ptrDisp, vecStrs.front(), true);
		}
		else
		{
			CStringVect_t::const_iterator _itr = vecStrs.begin(), 
											_end = vecStrs.end();
			for(; _itr != _end; ++_itr)
				AddNewSound(m_ptrDisp, *_itr, false);
		}
		vecStrs.clear();
	}
	LoadSounds();
}

void SoundCatalogDlg::OnButtonVarDel() 
{
	SoundForRemoveList_t list_for_remove;
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	while (pos) {
		int iIndex = m_list.GetNextSelectedItem(pos);
		qdSound* p = (qdSound*)m_list.GetItemData(iIndex);
		list_for_remove.push_back(SOUND_FOR_REMOVE(iIndex, p));
	}
	int iDeletedCount = 0;//сколько уже было удалено
	SoundForRemoveList_t::iterator b = list_for_remove.begin(), 
		e = list_for_remove.end();
	CString cap,msg;
	cap.LoadString(IDS_DEL_CAP);
	while (b != e) {
		msg.Format(IDS_DEL_SOUND_MSG, (*b).sound->name());
		int res = MessageBox(msg, cap, MB_YESNOCANCEL|MB_ICONSTOP);
		if (res == IDCANCEL)
			break;
		if (res == IDYES){
			if(CanRemoveSound((*b).sound->name()))
			{
				if(m_ptrDisp->remove_sound((*b).sound))
				{
					int iIndex = (*b).iPos - iDeletedCount;
					++iDeletedCount;
					m_list.DeleteItem(iIndex);
					m_bProjChanged = true;
					RemoveSoundFromObj();
				}
			}
		}
		++b;
	}		
	AdjustColumnsWidth();
}

BOOL SoundCatalogDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	
	CString str;
	str.LoadString(IDS_SOUND_COLOUMN_NAME);
	m_list.InsertColumn(0, str,LVCFMT_LEFT,100, 1);
	
	if (!m_strCap.IsEmpty()) {
		SetWindowText(m_strCap);	
	}

	LoadResources();
	LoadSounds();
	//если показыавем все доступную в этом каталоге анимацию,
	//то убираем кнопки "Создать" и "Добавить".
	if(!m_bUniqueList)
	{
		GetDlgItem(IDC_BUTTON_VAR_CREAT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_VAR_ADD2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_VAR_DEL)->ShowWindow(SW_HIDE);
		m_list.ModifyStyle(LVS_EDITLABELS, 0);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SoundCatalogDlg::AdjustColumnsWidth()
{
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}


void SoundCatalogDlg::OnDblclkListVars(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	*pResult = 0;
}

int Add2List(CListCtrl& lst, int idx, qdSound* pSound)
{
	int iItem = lst.InsertItem(idx, pSound->name());
	ASSERT(iItem != -1);
	lst.SetItemData(iItem, reinterpret_cast<DWORD_PTR>(pSound));

	return iItem;
}

bool SoundList2ListCtrl(CListCtrl& lst, const qdSoundList& slist, CStringSet_t* ptrSet)
{
	qdSoundList::const_iterator _itr = slist.begin(), _end = slist.end();
	int iLastIdx = lst.GetItemCount();
	for(; _itr != _end; ++_itr){
		if(ptrSet->insert((*_itr)->name()).second)
			iLastIdx = Add2List(lst, iLastIdx, *_itr);
	}
	return true;
}

bool SoundCatalogDlg::LoadSounds()
{
	CWaitCursor cur;
	m_list.DeleteAllItems();
/*	
	std::auto_ptr<CStringSet_t> strIndex = merge_lists(
		(m_bUniqueList ? m_ptrDisp: m_ptrGameDisp),
		m_ptrDisp, qdGameDispatcherBase::sound_list);
	
	std::copy(strIndex->begin(), strIndex->end(), 
		listCtrlInserter::listCtrl_inserter(m_list, 0));
*/
	CStringSet_t strIndex;
	SoundList2ListCtrl(m_list, m_ptrDisp->sound_list(), &strIndex);
	if(!m_bUniqueList&&m_ptrDisp != m_ptrGameDisp)
		SoundList2ListCtrl(m_list, m_ptrGameDisp->sound_list(), &strIndex);

	AdjustColumnsWidth();
	return true;
}

void SoundCatalogDlg::OnEndlabeleditListVars(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if(!pDispInfo->item.pszText)
	{
		*pResult = 0;
		return;
	}
	
	if(uniq_name(pDispInfo->item.pszText, m_ptrDisp->sound_list()))
	{
		CString strOldName(m_list.GetItemText(pDispInfo->item.iItem, 0));
		
		qdSound* sound = m_ptrDisp->get_sound(strOldName);
		
		
		sound->set_name(pDispInfo->item.pszText);
		qdGameDispatcherBase* owner = 
					static_cast<qdGameDispatcherBase*>(sound->owner());
		
		owner->remove_sound(sound);
		owner->add_sound(sound);
		
		RenameSound(strOldName, pDispInfo->item.pszText);
		
		m_bProjChanged = true;
		AdjustColumnsWidth();
		*pResult = TRUE;
	}
	else
	{
		CString str;
		str.Format(IDS_NAME_REPEATED, pDispInfo->item.pszText);
		ShowError(*this, str);
		*pResult = 0;
	}
	
}

struct rename_sound_in_obj{
	LPCTSTR _lpszNewName;
	LPCTSTR _lpszOldName;
	//является ли переименовываемая анимация глобальной
	bool _isGlobal;
	rename_sound_in_obj(LPCTSTR lpszNewName,
							LPCTSTR lpszOldName, bool isGlobal):
						_lpszNewName(lpszNewName),
						_lpszOldName(lpszOldName),
						_isGlobal(isGlobal){}

	void operator()(qdGameObjectState* ptrState){
		if (_isGlobal) {
			qdGameScene* ptrScene = 
				static_cast<qdGameScene*>(ptrState->owner()->owner());
			//проверяем, если анимация с таким именем в сцене.
			//если есть, значит она перекрывала глобальную и 
			//переименование нельзя делать.
			qdSound* ptr = ptrScene->get_sound(_lpszOldName);
			if (ptr) {
				return;
			}
		}
		ptrState->set_sound_name(_lpszNewName);
	}
};

void SoundCatalogDlg::RenameSound(LPCTSTR lpszOldName, LPCTSTR lpszNewName)
{
	//обрабатываем объекты
	m_listStates4Affected.clear();
	if (m_ptrGameDisp != m_ptrDisp) {//редактируем сцену
		find_users_in_scene<sound_user_finder::is_animated,
			sound_user_finder::sound_names_eqal> 
				fu(lpszOldName, &m_listStates4Affected);
		
		//ищем только в этой сцене
		fu(static_cast<qdGameScene*>(m_ptrDisp));
	}
	else//редактируем глобальный каталог
	{
//		const qdGameLocationList& gll = m_ptrGameDisp->location_list();
//		//ищем во всех сценах
//		sound_user_finder gu = std::for_each(gll.begin(), gll.end(),
//			sound_user_finder(lpszOldName, &m_listStates4Affected));
		sound_user_finder(lpszOldName, &m_listStates4Affected)(m_ptrGameDisp);
	}
	
	std::for_each(m_listStates4Affected.begin(), m_listStates4Affected.end(), 
		rename_sound_in_obj(lpszNewName, lpszOldName, 
							m_ptrGameDisp != m_ptrDisp));
	m_listStates4Affected.clear();
}

bool SoundCatalogDlg::TestInObj(qdGameObject* po, LPCTSTR lpszName){
	qdGameObjectAnimated* pa = static_cast<qdGameObjectAnimated*>(po);
	
	typedef qdGameObjectStateVector::const_iterator ci_t;
	
	bool bres = false;
	const qdGameObjectStateVector& v = pa->state_vector();
	ci_t i = v.begin(), e = v.end();
	for(; i != e; ++i){
		qdGameObjectState* ps = *i;
		LPCTSTR lpszSoundName = ps->sound_name();
		if (lpszSoundName&&
			!_tcsicmp(lpszSoundName, lpszName))
		{
			m_listStates4Affected.push_back(*i);
			bres = true;
		}
	}
	
	return bres;
}

CString SoundCatalogDlg::TestGlobalObj(LPCTSTR lpszName){
	CString res;
	const qdGameObjectList& l = m_ptrGameDisp->global_object_list();
	qdGameObjectList::const_iterator i = l.begin(), e = l.end();
	for(; i != e; ++i){
		qdGameObject* po = *i;
		if (po->named_object_type() != QD_NAMED_OBJECT_STATIC_OBJ){
			if (TestInObj(po, lpszName)){
				if (!res.IsEmpty()){
					res += _T(",\n");
				}
				res += po->name();
			}
		}
	}
	return res;
}


bool SoundCatalogDlg::CanRemoveSound(LPCTSTR lpszName)
{
	CString strMsg;
	
	m_listStates4Affected.clear();
	
	if(m_ptrGameDisp != m_ptrDisp)//если редактируем сцену
	{
		find_users_in_scene<sound_user_finder::is_animated,
			sound_user_finder::sound_names_eqal> 
							fu(lpszName, &m_listStates4Affected);
		
		//ищем только в этой сцене
		bool bInScene = fu(static_cast<qdGameScene*>(m_ptrDisp));
		if(!bInScene)
			return true;
		
		strMsg.Format(IDS_SOUND_USED_IN_SCENE, m_ptrDisp->name());
	}
	else
	{//глобальный каталог
//		const qdGameLocationList& gll = m_ptrGameDisp->location_list();
		
		CString strGlobalObj = TestGlobalObj(lpszName);
		//ищем во всех сценах
//		sound_user_finder gu = std::for_each(gll.begin(), gll.end(), 
//			sound_user_finder(lpszName, &m_listStates4Affected));
		sound_user_finder gu(lpszName, &m_listStates4Affected);
		gu(m_ptrGameDisp);
		if(gu.get_strUsers().IsEmpty()&&strGlobalObj.IsEmpty())
			return true;
		
		if (!gu.get_strUsers().IsEmpty()&&strGlobalObj.IsEmpty()) {
			strMsg.Format(IDS_SOUND_USED_IN_SCENE,
				static_cast<LPCTSTR>(gu.get_strUsers()));
		}
		else if (gu.get_strUsers().IsEmpty()&&!strGlobalObj.IsEmpty()) {
			strMsg.Format(IDS_SOUND_USED_IN_GLOBAL_OBJS,
				static_cast<LPCTSTR>(strGlobalObj));
		}
		else if (!gu.get_strUsers().IsEmpty()&&!strGlobalObj.IsEmpty()) {
			strMsg.Format(IDS_SOUND_USED_IN_SCENE_AND_GLOBAL_OBJS,
				static_cast<LPCTSTR>(strGlobalObj));
		}

	}
	
	if(AfxMessageBox(strMsg, MB_OKCANCEL|MB_ICONWARNING)== IDOK)
	{
		if((!m_ptrGameDisp->get_sound(lpszName)))
		{
			CString str = DetectLastState();
			if(!str.IsEmpty())
			{
				strMsg.Format(IDS_SOUND_USED, 
					lpszName, static_cast<LPCTSTR>(str));
				if(AfxMessageBox(strMsg, MB_OKCANCEL|MB_ICONWARNING)== IDOK)
					return true;
				return false;
			}
		}
		return true;
	}
	m_listStates4Affected.clear();
	return false;
}

CString SoundCatalogDlg::DetectLastState()
{
	CString res;
	GameObjectStateList_t::iterator _itr = m_listStates4Affected.begin(), 
		_end = m_listStates4Affected.end();
	for(; _itr != _end; ++_itr)
	{
		qdGameObjectState* ptrState = *_itr;
		
		if(HasLastEmptyState(ptrState->owner()))
		{
			res += ptrState->owner()->name();
			res += _T(':');
			res += ptrState->owner()->owner()->name();
			res += _T('\n');
		}
	}
	return res;
}

void SoundCatalogDlg::RemoveSoundFromObj()
{
	CWaitCursor __wait__;
	qdSound* ptrNew = NULL;
	
	GameObjectStateList_t::iterator _itr = m_listStates4Affected.begin(), 
									_end = m_listStates4Affected.end();
	
	for(; _itr != _end; ++_itr)
	{
		if(m_ptrGameDisp != m_ptrDisp)
			ptrNew = m_ptrGameDisp->get_sound((*_itr)->sound_name());
		
		qdGameObjectAnimated* ptrObj = 
			static_cast<qdGameObjectAnimated*>((*_itr)->owner());
		if(!ptrNew&&(*_itr)->is_state_empty())
		{
			ptree_api::DeleteObject(*_itr,false);
		}
	}
}

void SoundCatalogDlg::OnButtonVarEdit2() 
{
	int iSelCount = m_list.GetSelectedCount();
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos) {
		int iIndex = m_list.GetNextSelectedItem(pos);
		CString str = m_list.GetItemText(iIndex, 0);
		
		qdSound *ptrSound = getSound(str);
		if (!ptrSound)
		{
			ASSERT(0);
			return;
		}

		if (!ptrSound->is_resource_loaded()) {
			CWaitCursor __wait__;
			ptrSound->load_resource();
		}
		ptrSound->play();
	}
}

void SoundCatalogDlg::OnKeydownListVars(NMHDR* pNMHDR, LRESULT* pResult) 
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

void SoundCatalogDlg::LoadResources()
{
	if (!m_ptrDisp||m_ptrDisp == m_ptrGameDisp)
		return;
//	m_ptrDisp->load_resources();
}
void SoundCatalogDlg::FreeResources()
{
	if (!m_ptrDisp||m_ptrDisp == m_ptrGameDisp)
		return;
//	m_ptrDisp->free_resources();
}

void SoundCatalogDlg::OnDestroy()
{
	CDialog::OnDestroy();
	FreeResources();
}

qdSound * SoundCatalogDlg::getSound(CString const& name)
{
	qdSound * const ptrSound = m_ptrGameDisp->get_sound(name);
	if (ptrSound)
		return ptrSound;
	return m_ptrDisp->get_sound(name);
}
