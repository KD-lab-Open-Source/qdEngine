// AnimCatalogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"

#include "qd_animation_set.h"
#include "qd_game_object_state.h"
#include "qd_game_object_animated.h"
#include "AnimCatalogDlg.h"

#include "relativepath.h"

#include "name_op.h"
#include "ChoseName.h"

#include "curdirsafer.h"
#include "WizardSheet.h"
#include "AddAnimFirstWPg.h"
#include "DispsListsMerger.h"
#include "addanimmultiwizard.h"

#include "disk_op.h"
#include "ptree_api.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static TCHAR THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AnimCatalogDlg property page

namespace{
	const TCHAR STR_SEPARATOR[] = _T("\\");
	inline CurDirSafer& GetCurDirSafer(){
		static CurDirSafer cds;
		return cds;
	}
}

void ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID = -1);
void ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID = -1);

bool HasLastEmptyState(qdNamedObject* pno);


IMPLEMENT_DYNCREATE(AnimCatalogDlg, CDialog)


void WaitEndProc(PROCESS_INFORMATION* ppi);
bool EditAnimation(HWND hParent, LPCTSTR lpszFileName);
qdAnimation* AddNewAnimation(qdGameDispatcherBase* ptrDisp, 
								const CString& strFileName, bool bAskName);

bool GetAnimationFileName(HWND hWnd, bool bNewFile, CStringVect_t* ptrFiles);
qdAnimation* CreateNewAnimation(HWND hPar, qdGameDispatcherBase* ptrDisp);

void ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID)
{
	CString cap;
	if(dwCapResID == -1)
		cap.LoadString(IDS_ERR_MSG_CAPTION);
	else
		cap.LoadString(dwCapResID);

	CString msg((LPTSTR)dwMsgResID);

	::MessageBox(hPar, msg, cap, MB_OK|MB_ICONHAND);
}

void ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID)
{
	CString cap;
	if(dwCapResID == -1)
		cap.LoadString(IDS_ERR_MSG_CAPTION);
	else
		cap.LoadString(dwCapResID);
	
	::MessageBox(hPar, strMsg, cap, MB_OK|MB_ICONHAND);
}


void WaitEndProc(PROCESS_INFORMATION* ppi, HWND hPar)
{
	while(true)
	{
		DWORD __res = MsgWaitForMultipleObjects(1, 
				&ppi->hProcess, FALSE, INFINITE,QS_ALLINPUT);
		if(__res == WAIT_OBJECT_0)
		{
			return;
		}
		else if(__res == WAIT_OBJECT_0 + 1)
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				if(msg.hwnd == hPar)
				{
					if(msg.message == WM_PAINT||
						msg.message == WM_NCPAINT)
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

		}
	}
}

//запускает редактор анимации
bool EditAnimation(HWND hParent, LPCTSTR lpszFileName)
{
	CString str;
	str.LoadString(IDS_ANIMATION_EDITOR_FILE_NAME);
	
	CString app_path = disk_op::GetModulePath();		
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_splitpath( app_path, drive, dir, fname, ext );
	
	app_path = drive;
	app_path += dir;
	app_path += str;
	
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	
	app_path;
	CString strCommandLine = _T("\"");
	strCommandLine += app_path;
	strCommandLine += _T("\"");

	strCommandLine += _T(" \"");
	strCommandLine += lpszFileName;
	strCommandLine += _T("\"");
	if(CreateProcess(app_path,
		const_cast<LPTSTR>((LPCTSTR)strCommandLine), NULL, NULL, FALSE,
		0, NULL, NULL, &si, &pi))
	{
		WaitEndProc(&pi, hParent);
		
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		
		return true;
	}
	else
		ShowError(hParent, IDS_CNT_LOAD_ANIMATION_EDITOR);
	
	return false;
}


qdAnimation* AddNewAnimation(qdGameDispatcherBase* ptrDisp, 
								const CString& strFileName, bool bAskName)
{
	CString str;
	str = disk_op::GetFileName(strFileName);;

	if(bAskName)
	{
		ChooseName dlg;
		dlg.set_str(str);
		dlg.DoModal();
		str = dlg.get_str();
	}
	str = get_unique_name(str, ptrDisp->animation_list());
	
	//создаём и настраиваем анимацию
	qdAnimation * pa = new qdAnimation;
	if(!pa)
		return NULL;
	
	pa->set_name(str);
	pa->qda_set_file(strFileName);
	ptrDisp->add_animation(pa);
	
	return pa;
}

bool GetAnimationFileName(HWND hWnd, bool bNewFile, CStringVect_t* ptrFiles)
{
	CString str;
	str.LoadString(IDS_OPEN_ANIMATION_EXT);
	CString str2;
	str2.LoadString(IDS_OPEN_ANIMATION_FILTER);
	CString strTitle((LPTSTR)IDS_OSD_CAP_SEL_ANIM);
	
	CString strNewFileName;
	DWORD dwFlags = 0;
	if(bNewFile)
		strNewFileName.LoadString(IDS_NEW_ANIMATION_FILE_NAME);
	else
		dwFlags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
	return disk_op::AskUser4FileNames(hWnd, bNewFile, ptrFiles, 
									str, str2, strTitle, 
									strNewFileName, GetCurDirSafer(), dwFlags);
}

qdAnimation* CreateNewAnimation(HWND hPar, qdGameDispatcherBase* ptrDisp)
{
	CStringVect_t vecFileNames;
	if(GetAnimationFileName(hPar, true, &vecFileNames))
	{
		if (vecFileNames.empty())
			return NULL;
		qdAnimation* pa = AddNewAnimation(ptrDisp, vecFileNames.front(), true);
		if(!pa)
		{
			ShowError(hPar, IDS_EM_CDNT_CRT_ANIMATION);
			return NULL;
		}
		pa->qda_save(vecFileNames.front());
		EditAnimation(hPar, pa->qda_file());
		return pa;
	}
	return NULL;
}	

AnimCatalogDlg::AnimCatalogDlg() : CDialog(AnimCatalogDlg::IDD)
{
	//{{AFX_DATA_INIT(AnimCatalogDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_ptrGameDisp = NULL;
	m_ptrDisp = NULL;
	m_bTreeChanged = false;

	m_eAction = GOPA_SHOW;
	m_bFirstTimeShow = true;
}

AnimCatalogDlg::~AnimCatalogDlg()
{
}

bool AnimCatalogDlg::set_data(qdGameDispatcher* ptrGD, 
							  qdGameDispatcherBase* ptrObj)
{
	m_ptrGameDisp = ptrGD;
	m_ptrDisp = ptrObj;

	return true;
}

bool AnimCatalogDlg::set_active_anim_name(const CString& strName)
{
	m_strActiveAnimName = strName;
	return true;
}

void AnimCatalogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AnimCatalogDlg)
	DDX_Control(pDX, IDC_LIST_VARS, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AnimCatalogDlg, CDialog)
//{{AFX_MSG_MAP(AnimCatalogDlg)
ON_BN_CLICKED(IDC_BUTTON_VAR_ADD2, OnButtonVarAdd2)
ON_BN_CLICKED(IDC_BUTTON_VAR_DEL, OnButtonVarDel)
ON_BN_CLICKED(IDC_BUTTON_VAR_EDIT2, OnButtonVarEdit2)
ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_VARS, OnEndlabeleditListVars)
ON_BN_CLICKED(IDC_BUTTON_VAR_CREAT, OnButtonVarCreat)
ON_WM_SHOWWINDOW()
ON_NOTIFY(NM_DBLCLK, IDC_LIST_VARS, OnDblclkListVars)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_VARS, OnKeydownListVars)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AnimCatalogDlg message handlers


int AnimCatalogDlg::InsertAnimation2List(qdAnimation* pa)
{
	int iIdx = m_list.InsertItem(m_list.GetItemCount(), pa->name());
	UpdateAnimationInfo(pa, iIdx);
	return iIdx;
}

void AnimCatalogDlg::OnButtonVarAdd2() 
{
/*
CStringVect_t vecFileNames;
if (GetAnimationFileName(*this, false, &vecFileNames)) 
{
CStringVect_t::const_iterator _itr = vecFileNames.begin(), 
_end = vecFileNames.end();

		bool bAskName = vecFileNames.size() == 1;
		for(; _itr != _end; ++_itr)
		{
		qdAnimation *pa = AddNewAnimation(m_ptrDisp, *_itr, bAskName);
		if(!pa)
		{
		ShowError(*this, IDS_EM_CDNT_CRT_ANIMATION);
		return;
		}			
		//вставляем инфу о ней в список
		InsertAnimation2List(pa);
		}
		m_bTreeChanged = true;
		}
	*/
/*
	AddAnimFirstWPg apg;
	apg.set_place(PAGE_PLACE_FIRST_LAST);
	apg.set_data(m_ptrGameDisp, m_ptrDisp);
	
	CPropertyPage* pPage = &apg;
	WizardSheet ws(&pPage, 1, 0);

	ws.DoModal();
	if(apg.GetModified())
	{
		m_bTreeChanged = true;
		LoadAnimations();
	}
	
	AdjustColumnsWidth();
*/
	AddAnimMultiWizard wizard;
	if (wizard.show(this, m_ptrGameDisp, m_ptrDisp, IDS_WIZARD_CAP_ANIMATION_ADD)){
		SetProjChange();
		m_bTreeChanged = true;
		LoadAnimations();
	}
	AdjustColumnsWidth();
}

void AnimCatalogDlg::RemoveAnimationFromGaits()
{
	qdAnimationInfoDEQue_t::iterator _itr = m_dequeAI.begin(), 
									_end = m_dequeAI.end();
	for(; _itr != _end; ++_itr)
	{
		if(m_ptrGameDisp != m_ptrDisp)//удаляем анимацию из сцены
		{
			if(!m_ptrGameDisp->get_animation((*_itr)->animation_name()))
				(*_itr)->set_animation_name(NULL);
		}
		else
			(*_itr)->set_animation_name(NULL);
	}
}

void AnimCatalogDlg::RemoveAnimationFromObj()
{
	CWaitCursor __wait__;
	qdAnimation* ptrNew = NULL;

	GameObjectStateList_t::iterator _itr = m_listStates4Affected.begin(), 
									_end = m_listStates4Affected.end();
	
	for(; _itr != _end; ++_itr)
	{
		qdGameObjectState* pstate = *_itr;
		if(m_ptrGameDisp != m_ptrDisp)
			ptrNew = m_ptrGameDisp->get_animation(
			static_cast<qdGameObjectStateStatic*>(pstate)->
					animation_info()->animation_name());

		qdGameObjectAnimated* ptrObj = 
			static_cast<qdGameObjectAnimated*>(pstate->ref_owner());
		if(!ptrNew&&pstate->is_state_empty())
			ptree_api::DeleteObject(pstate, false);
	}
}

void AnimCatalogDlg::OnButtonVarDel() 
{
	AnimForRemoveList_t list_for_remove;
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	while (pos) {
		int iIndex = m_list.GetNextSelectedItem(pos);
		qdAnimation* pa = (qdAnimation*)m_list.GetItemData(iIndex);
		list_for_remove.push_back(ANIMATION_FOR_REMOVE(iIndex, pa));
	}
	int iDeletedCount = 0;//сколько уже было удалено
	AnimForRemoveList_t::iterator b = list_for_remove.begin(), 
		e = list_for_remove.end();
	CString cap,msg;
	cap.LoadString(IDS_DEL_CAP);

	m_ptrGameDisp->split_global_objects();
	while (b != e) {
		msg.Format(IDS_DEL_ANIMATION_MSG, (*b).anim->name());
		int res = MessageBox(msg, cap, MB_YESNOCANCEL|MB_ICONSTOP);
		if (res == IDCANCEL)
			break;
		if (res == IDYES){
			if(CanRemoveAS((*b).anim->name()))
			{
				if(m_ptrDisp->remove_animation((*b).anim))
				{
					int iIndex = (*b).iPos - iDeletedCount;
					++iDeletedCount;
					m_list.DeleteItem(iIndex);
					m_bTreeChanged = true;
					
					RemoveAnimationFromObj();
					RemoveAnimationFromGaits();
					
					iIndex = (std::min)(iIndex, m_list.GetItemCount()-1);
				}
			}
		}
		++b;
	}
	m_ptrGameDisp->merge_global_objects();
	AdjustColumnsWidth();
}

void AnimCatalogDlg::OnButtonVarEdit2() 
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if(!pos)
	{
		ShowError(*this,IDS_NO_SELECTION_MSG, IDS_ERR_MSG_CAPTION);
		return;
	}
	int iIndex = m_list.GetNextSelectedItem(pos);
	qdAnimation *pa = (qdAnimation *)m_list.GetItemData(iIndex);
	if(EditAnimation(*this, pa->qda_file()))
	{
		UpdateAnimationInfo(pa, iIndex);
		AdjustColumnsWidth();
	}
}

void AnimCatalogDlg::UpdateAnimationInfo(qdAnimation* pa, int iIndex)
{
	if(!pa->load_resource_header())
		return;
	CString str;
	str.Format("%d", pa->num_frames());
	m_list.SetItemText(iIndex, 1, str);
	//в принципе эта строчка не всегда нужна
	//оставлю её здесь для сохранения единообразия
	m_list.SetItemData(iIndex, reinterpret_cast<DWORD_PTR>(pa));
}

BOOL AnimCatalogDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	CString str;
	str.LoadString(IDS_ANIMATION_COLOUMN_NAME);
	m_list.InsertColumn(0, str,LVCFMT_LEFT,100, 1);
	
	str.LoadString(IDS_FRAME_COUNT_COLOUMN_NAME);
	m_list.InsertColumn(1, str,LVCFMT_LEFT,50);
	
	
	LoadAnimations();	
	//делаем эту установку тольк один раз при показе
	SelectAnim();

	if(!m_strCap.IsEmpty())
		SetWindowText(m_strCap);

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

inline int Add2List(CListCtrl& lst, int idx, qdAnimation* pAnim)
{
	int iItem = lst.InsertItem(idx, pAnim->name());
	ASSERT(iItem != -1);
	lst.SetItemData(iItem, reinterpret_cast<DWORD_PTR>(pAnim));
	TCHAR strBuff[32];
	
	pAnim->load_resource_header();
	lst.SetItemText(iItem, 1, _itoa(pAnim->num_frames(), strBuff, 10));
	return iItem;
}

bool AnimList2ListCtrl(CListCtrl& lst, 
					   const qdAnimationList& alist, CStringSet_t* ptrSet)
{
	qdAnimationList::const_iterator _itr = alist.begin(),
		_end = alist.end();
	int iLastIdx = lst.GetItemCount();
	for(; _itr != _end; ++_itr){
		if(ptrSet->insert((*_itr)->name()).second)
			iLastIdx = Add2List(lst, iLastIdx, *_itr);
	}
	return true;
}

void AnimCatalogDlg::LoadAnimations()
{
	m_list.DeleteAllItems();

	CStringSet_t strIndex;
	AnimList2ListCtrl(m_list, m_ptrDisp->animation_list(), &strIndex);
	if(!m_bUniqueList&&m_ptrDisp != m_ptrGameDisp)
		AnimList2ListCtrl(m_list, m_ptrGameDisp->animation_list(), &strIndex);
	
	AdjustColumnsWidth();
}

void AnimCatalogDlg::OnEndlabeleditListVars(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if(!pDispInfo->item.pszText)
	{
		*pResult = 0;
		return;
	}

	if(uniq_name(pDispInfo->item.pszText, m_ptrDisp->animation_list()))
	{
		qdAnimation* pa = 
			reinterpret_cast<qdAnimation*>
					(m_list.GetItemData(pDispInfo->item.iItem));

		CString strOldName(pa->name());

		pa->set_name(pDispInfo->item.pszText);
		qdGameDispatcherBase* owner = 
			static_cast<qdGameDispatcherBase*>(pa->owner());

		owner->remove_animation(pa);
		owner->add_animation(pa);

		RenameAnimation(strOldName, pDispInfo->item.pszText);

		m_bTreeChanged = true;
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

BOOL AnimCatalogDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	BOOL res = CDialog::PreCreateWindow(cs);
	cs.style |= DS_NOIDLEMSG;
	return res;
}

void AnimCatalogDlg::SelectAnim()
{
	if(m_strActiveAnimName.IsEmpty())
		return;

	LVFINDINFO lfi;
	memset(&lfi, 0, sizeof(LVFINDINFO));
	lfi.psz = m_strActiveAnimName;
	lfi.flags = LVFI_STRING;

	int i = m_list.FindItem(&lfi);
	if(i != -1)
	{
		m_list.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		m_list.EnsureVisible(i, TRUE);
	}
}

void AnimCatalogDlg::OnButtonVarCreat() 
{
	qdAnimation* pa = CreateNewAnimation(this->m_hWnd, m_ptrDisp);
	if(pa)
	{
		InsertAnimation2List(pa);
		AdjustColumnsWidth();
		m_bTreeChanged = true;
	}
}


void AnimCatalogDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	if(bShow&&m_bFirstTimeShow)
	{
		m_bFirstTimeShow = false;
		::ShowWindow(*this, SW_SHOW);
		switch(m_eAction){
			case GOPA_CREATE:
				OnButtonVarCreat();
				break;
		}
	}
}

void AnimCatalogDlg::OnDblclkListVars(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnButtonVarEdit2();	
	*pResult = 0;
}

void AnimCatalogDlg::AdjustColumnsWidth()
{
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_list.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
}

CString AnimCatalogDlg::DetectLastState()
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

CString AnimCatalogDlg::TestGaits(LPCTSTR lpszName)
{
	CString res;
	const qdAnimationSetList &al = m_ptrGameDisp->animation_set_list();
	qdAnimationSetList::const_iterator _itr = al.begin(), _end = al.end();

	m_dequeAI.clear();
	
	for(;_itr != _end; ++_itr)
	{
		qdAnimationSet* ptrAS = *_itr;
		int size = ptrAS->size();
		for(int i = 0; i < size; ++i)
		{
			bool bAdded = false;
			qdAnimationInfo* pAI = ptrAS->get_animation_info(i);
			if(pAI->animation_name()
				&&(strcmp(pAI->animation_name(), lpszName)==0))
			{
				m_dequeAI.push_back(pAI);
				if(!res.IsEmpty())
					res += _T(",\n");
				
				res += ptrAS->name();
				bAdded = true;
			}
			pAI = ptrAS->get_static_animation_info(i);
			if(pAI->animation_name()
				&&(strcmp(pAI->animation_name(), lpszName)==0))
			{
				m_dequeAI.push_back(pAI);
				
				if (!bAdded) {
					if(!res.IsEmpty())
						res += _T(",\n");
					res += ptrAS->name();
				}
			}
		}
	}
	return res;
}

bool AnimCatalogDlg::TestInObj(qdGameObject* po, LPCTSTR lpszName){
	qdGameObjectAnimated* pa = static_cast<qdGameObjectAnimated*>(po);

	typedef qdGameObjectStateVector::const_iterator ci_t;

	bool bres = false;
	const qdGameObjectStateVector& v = pa->state_vector();
	ci_t i = v.begin(), e = v.end();
	for(; i != e; ++i){
		if ((*i)->state_type() == qdGameObjectState::STATE_STATIC){
			qdAnimationInfo* pai = 
				static_cast<qdGameObjectStateStatic*>(*i)->animation_info();
			if (pai&&pai->animation_name()&&
				!_tcsicmp(pai->animation_name(), lpszName))
			{
				m_listStates4Affected.push_back(*i);
				bres = true;
			}
		}
	}

	return bres;
}

CString AnimCatalogDlg::TestGlobalObj(LPCTSTR lpszName){
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

bool AnimCatalogDlg::CanRemoveAS(LPCTSTR lpszName)
{
	CString strMsg;
	//проверяем походки на использование этой анимации
	CString strGaits = TestGaits(lpszName);
	
	m_listStates4Affected.clear();
	
	if(m_ptrGameDisp != m_ptrDisp)//если редактируем сцену
	{
		find_users_in_scene<anim_user_finder::is_animated,
			anim_user_finder::animation_names_eqal> 
					fu(lpszName, &m_listStates4Affected);
		
		//ищем только в этой сцене
		bool bInScene = fu(static_cast<qdGameScene*>(m_ptrDisp));
		if(!bInScene&&strGaits.IsEmpty())
			return true;
		
		if (strGaits.IsEmpty()&&bInScene) {
			strMsg.LoadString(IDS_ANIMATION_USED_IN_SCENE2);
		}else if (!strGaits.IsEmpty()&&bInScene){
			strMsg.Format(IDS_ANIMATION_USED_IN_SCENE_AND_GAIT2, 
				static_cast<LPCTSTR>(strGaits));
		}else if (!strGaits.IsEmpty()&&!bInScene){
			strMsg.Format(IDS_ANIMATION_USED_IN_GAIT, 
				static_cast<LPCTSTR>(strGaits));
		}
	}
	else
	{//глобальный каталог
		//ищем во всех сценах
//		anim_user_finder gu = std::for_each(gll.begin(), gll.end(), 
//			anim_user_finder(lpszName, &m_listStates4Affected));
		anim_user_finder gu(lpszName, &m_listStates4Affected);
		gu(m_ptrGameDisp);
		if(gu.get_strUsers().IsEmpty()&&strGaits.IsEmpty())
			return true;

		CString strGlobaObj = TestGlobalObj(lpszName);

		if(!strGaits.IsEmpty()&&!gu.get_strUsers().IsEmpty())
			if (strGlobaObj.IsEmpty()) 
				strMsg.Format(IDS_ANIMATION_USED_IN_SCENE_AND_GAIT, 
					static_cast<LPCTSTR>(gu.get_strUsers()), 
					static_cast<LPCTSTR>(strGaits));
			else
				strMsg.Format(IDS_ANIMATION_USED_IN_SCENE_GAIT_AND_GLOBAL_OBJ, 
					static_cast<LPCTSTR>(gu.get_strUsers()), 
					static_cast<LPCTSTR>(strGaits),
					static_cast<LPCTSTR>(strGlobaObj));
			
		else if (!strGaits.IsEmpty()&&gu.get_strUsers().IsEmpty()) {
			if (strGlobaObj.IsEmpty()) 
				strMsg.Format(IDS_ANIMATION_USED_IN_GAIT, 
					static_cast<LPCTSTR>(strGaits));
			else
				strMsg.Format(IDS_ANIMATION_USED_IN_GAIT_AND_GLOBAL_OBJ, 
					static_cast<LPCTSTR>(strGaits),
					static_cast<LPCTSTR>(strGlobaObj));
		} else if (strGaits.IsEmpty()&&!gu.get_strUsers().IsEmpty()) {
			if (strGlobaObj.IsEmpty()) 
				strMsg.LoadString(IDS_ANIMATION_USED_IN_SCENE2);
			else
				strMsg.Format(IDS_ANIMATION_USED_IN_SCENE2_AND_GLOBAL_OBJ,
					static_cast<LPCTSTR>(gu.get_strUsers()), 
					static_cast<LPCTSTR>(strGlobaObj));
		}
		else if (strGlobaObj.IsEmpty())
			strMsg.Format(IDS_ANIMATION_USED_IN_GLOBAL_OBJ,
								static_cast<LPCTSTR>(strGlobaObj));
	}
	
	if(AfxMessageBox(strMsg, MB_OKCANCEL|MB_ICONWARNING)== IDOK)
	{
		if((!m_ptrGameDisp->get_animation(lpszName)))
		{
			CString str = DetectLastState();
			if(!str.IsEmpty())
			{
				strMsg.Format(IDS_ANIMATION_USED2, 
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

struct rename_in_obj{
	LPCTSTR _lpszNewName;
	LPCTSTR _lpszOldName;
	//является ли переименовываемая анимация глобальной
	bool _isGlobal;
	rename_in_obj(LPCTSTR lpszNewName, LPCTSTR lpszOldName, bool isGlobal):
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
			qdAnimation* ptr = ptrScene->get_animation(_lpszOldName);
			if (ptr) {
				return;
			}
		}
		qdGameObjectStateStatic* ptrSS = 
			static_cast<qdGameObjectStateStatic*>(ptrState);
		ptrSS->animation_info()->set_animation_name(_lpszNewName);
	}
};

void AnimCatalogDlg::RenameAnimation(LPCTSTR lpszOldName, LPCTSTR lpszNewName)
{
	//обрабатываем объекты
	m_listStates4Affected.clear();
	if (m_ptrGameDisp != m_ptrDisp) {//редактируем сцену
		find_users_in_scene<anim_user_finder::is_animated,
			anim_user_finder::animation_names_eqal> 
									fu(lpszOldName, &m_listStates4Affected);
		
		//ищем только в этой сцене
		fu(static_cast<qdGameScene*>(m_ptrDisp));
	}
	else//редактируем глобальный каталог
	{
//		const qdGameLocationList& gll = m_ptrGameDisp->location_list();
//		//ищем во всех сценах
//		anim_user_finder gu = std::for_each(gll.begin(), gll.end(),
//			anim_user_finder(lpszOldName, &m_listStates4Affected));
		anim_user_finder(lpszOldName, &m_listStates4Affected)(m_ptrGameDisp);
	}
	
	std::for_each(m_listStates4Affected.begin(), m_listStates4Affected.end(), 
		rename_in_obj(lpszNewName, lpszOldName, m_ptrGameDisp != m_ptrDisp));
	m_listStates4Affected.clear();

	//обрабатываем походки
	TestGaits(lpszOldName);
	qdAnimationInfoDEQue_t::iterator _itr = m_dequeAI.begin(), 
									 _end = m_dequeAI.end();
	for(; _itr != _end; ++_itr)
	{
		if (m_ptrDisp != m_ptrGameDisp) {
			if(!m_ptrGameDisp->get_animation(lpszOldName))
			//если нет такой в глобальном каталоге
			//то заменяем имя
				(*_itr)->set_animation_name(lpszNewName);
		}
		else
		{
			(*_itr)->set_animation_name(lpszNewName);
		}
	}
}

void AnimCatalogDlg::OnKeydownListVars(NMHDR* pNMHDR, LRESULT* pResult) 
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
