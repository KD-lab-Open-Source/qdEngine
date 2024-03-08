// AddAnimStateWPg.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"

#include <commctrl.h>
#include <Additional/itr_traits_spec.hpp>
#include <Additional\\qls.hpp>
#include <boost/static_assert.hpp>

#include "qd_sound.h"
#include "qd_animation_set.h"
#include "qd_game_object_animated.h"
#include "qd_game_object_moving.h"
#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"

#include "AddAnimStateWPg.h"

#include "file_enumerator.h"
#include "listCtrl_insert_iterator.h"

#include "name_op.h"
#include "ptree_api.h"

#include "curdirsafer.h"
#include "obj_type_detectors.h"


extern TCHAR __CURRENT_DIR__[];
extern TCHAR __PARENT_DIR__[];

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void SaveScrLayoutOnBndEdt(qdGameObjectState* pstate);

CurDirSafer g_DlgFilesDir;
//предназначен для того, чтобы не беспокоится о симметричности
//выполнения split/morge_global_object
template<class _Disp>
class split_guard
{
	_Disp* pd;
	qdGameObjectAnimated* pa;
public:
	split_guard(_Disp* pdisp, qdGameObjectAnimated* pa):pd(NULL), pa(NULL){
		if (pdisp&&pdisp->split_global_objects(pa)) {
			pd = pdisp;
			this->pa = pa;
		}
	}
	~split_guard()
	{
		if (pd) {
			pd->merge_global_objects(pa);
		}
	}
};

namespace listCtrlInserter{

	template<>
	struct data_traits<CString>{
		inline static int insert(CListCtrl* ctrl, int pos, const CString& data){
			return ctrl->InsertItem(pos, data);
		}
	};

	template<>
		struct data_traits<file_find_data>{
		static int insert(CListCtrl* ctrl, int pos, const file_find_data& data){
			pos =  ctrl->InsertItem(pos, data.name());
			if (pos != -1){
				LVITEM lvi;
				lvi.iItem = pos;
				lvi.iSubItem = 0;
				lvi.mask = LVIF_IMAGE;
				SHFILEINFO shi;
				SHGetFileInfo(data.name(),0, &shi, sizeof(shi), 
					SHGFI_SMALLICON|SHGFI_SYSICONINDEX);
				
				lvi.iImage = shi.iIcon;
				ctrl->SetItem(&lvi);
			}
			return pos;
		}
	};
}

/////////////////////////////////////////////////////////////////////////////
// AddAnimStateWPg property page
/************************************************************************/
/* Extern Declarations                                                  */
/************************************************************************/
VOID ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID = -1);
VOID ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID = -1);

namespace{
CString get_obj_uniq_name(const CString &src_name, qdGameDispatcherBase* ptrDisp)
{
	if (ptrDisp->named_object_type() == QD_NAMED_OBJECT_SCENE) {
		return get_unique_name(src_name, 
			static_cast<qdGameScene*>(ptrDisp)->object_list());
	}
	else if (ptrDisp->named_object_type() == QD_NAMED_OBJECT_DISPATCHER) {
		return get_unique_name(src_name, 
			static_cast<qdGameDispatcher*>(ptrDisp)->global_object_list());
	}
	ASSERT(0);
	return CString();
}

qdGameObjectAnimated* AddAnimated(qdGameDispatcherBase* ptrDisp,
								  CString &strObjName, bool bShow)
{
	if (ptrDisp->named_object_type() == QD_NAMED_OBJECT_SCENE) {
		return ptree_api::AddAnimated(
			static_cast<qdGameScene*>(ptrDisp), strObjName, bShow);
	}
	else if (ptrDisp->named_object_type() == QD_NAMED_OBJECT_DISPATCHER) {
//		return ptree_api::AddAnimated(static_cast<qdGameDispatcher*>(ptrDisp),
//								strObjName, bShow);
	}
	ASSERT(("AddAnimated",0));
	return NULL;
}

qdGameObjectMoving* AddPersonage(qdGameDispatcherBase* ptrDisp,
								  CString &strObjName, bool bShow)
{
	if (ptrDisp->named_object_type() == QD_NAMED_OBJECT_SCENE) {
		return ptree_api::AddPersonage(
			static_cast<qdGameScene*>(ptrDisp), strObjName, bShow);
	}
	else if (ptrDisp->named_object_type() == QD_NAMED_OBJECT_DISPATCHER) {
		return ptree_api::AddPersonage(static_cast<qdGameDispatcher*>(ptrDisp),
			strObjName, bShow);
	}
	ASSERT(("AddPersonage",0));
	return NULL;
}

}
void RestoreAnimationScrPlace(qdGameDispatcher* pdisp, qdGameObjectAnimated* ptrObj, 
							  qdGameObjectStateVector const&v)
{
	if (qdGameScene* pscene = pdisp->get_active_scene())
	{
		qdCamera* pcam = pscene->get_camera();
		ASSERT(pcam);
		Vect2s const v1(pcam->global2scr(Vect3f(0.f, 0.f, 0.f)));
		Vect2s const v2(pcam->global2scr(ptrObj->R()));
		std::for_each(v.begin(), v.end(),
			std::bind2nd(std::mem_fun(qdGameObjectState::set_center_offset), v1-v2));
	}
}
//////////////////////////////////////////////////////////////////////////

#ifdef _AddAnimState_USE_PROP_PAGE
IMPLEMENT_DYNCREATE(AddAnimStateWPg, CBasePage)
#else
IMPLEMENT_DYNCREATE(AddAnimStateWPg, CDialog)
#endif

#ifdef _AddAnimState_USE_PROP_PAGE
AddAnimStateWPg::AddAnimStateWPg() : CBasePage(AddAnimStateWPg::IDD)
#else
AddAnimStateWPg::AddAnimStateWPg() : CDialog(AddAnimStateWPg::IDD)
#endif
{
	//{{AFX_DATA_INIT(AddAnimStateWPg)
	m_iContentType = LIST_CONTENT_ANIMATION;
	//}}AFX_DATA_INIT
	m_b4AnimObj = true;
	m_bUseMovementState = false;

	m_bAddObjs = true;

	m_hStateNameEdit	= NULL;
	m_hObjNameEdit		= NULL;

	m_eLastOpStatus = OS_SUCC;

	m_hicoSucc = NULL; m_hicoWarn = NULL; m_hicoErr = NULL;

	m_ptrObj = NULL;
	m_bTunneling = false;
	m_bModified = false;
}

VOID AddAnimStateWPg::set_obj_type(qdNamedObjectType _type)
{
	_ASSERT(_type == QD_NAMED_OBJECT_ANIMATED_OBJ||
		_type == QD_NAMED_OBJECT_MOVING_OBJ||
		_type == QD_NAMED_OBJECT_MOUSE_OBJ);
	m_b4AnimObj = (_type == QD_NAMED_OBJECT_ANIMATED_OBJ
		||_type == QD_NAMED_OBJECT_MOUSE_OBJ);
}

bool AddAnimStateWPg::set_data(qdGameDispatcher* ptrGameDisp, 
							   qdGameDispatcherBase* ptrObjDisp)
{
	m_ptrGameDisp = ptrGameDisp;
	m_ptrObjDisp = ptrObjDisp;
	return true;
}

AddAnimStateWPg::~AddAnimStateWPg()
{
}

VOID AddAnimStateWPg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddAnimStateWPg)
	DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Control(pDX, IDC_LIST_FILES, m_list_files);
	DDX_Control(pDX, IDC_EDIT_OBJ_NAME, m_wndObjName);
	DDX_Control(pDX, IDC_COMBOBOXEX_LOG, m_wndLog);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Radio(pDX, IDC_RADIO_LIST_CONTENT, m_iContentType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddAnimStateWPg, CDialog)
//{{AFX_MSG_MAP(AddAnimStateWPg)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
ON_EN_SETFOCUS(IDC_EDIT_OBJ_NAME, OnSetFocusComboObjName)
ON_BN_CLICKED(IDC_BTN_ADD_STATE, OnBtnAddState)
ON_EN_KILLFOCUS(IDC_EDIT_OBJ_NAME, OnKillfocusComboObjName)
	ON_BN_CLICKED(IDC_RADIO_LIST_CONTENT, OnRadioListContent)
	ON_BN_CLICKED(IDC_RADIO_LIST_CONTENT2, OnRadioListContent2)
	ON_BN_CLICKED(IDC_RADIO_LIST_CONTENT3, OnRadioListContent3)
	ON_BN_CLICKED(IDC_RADIO_LIST_CONTENT4, OnRadioListContent4)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AddAnimStateWPg message handlers

VOID AddAnimStateWPg::set_obj(qdGameObjectAnimated* ptrObj)
{
	m_ptrObj = ptrObj;
	m_bAddObjs = (m_ptrObj == NULL);
	if (!m_bAddObjs) 
	{
		m_b4AnimObj = 
			(m_ptrObj->named_object_type() == QD_NAMED_OBJECT_ANIMATED_OBJ
			||m_ptrObj->named_object_type() == QD_NAMED_OBJECT_MOUSE_OBJ);
	}
	
}

BOOL AddAnimStateWPg::OnInitDialog() 
{
	__super::OnInitDialog();
	
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_CHECKBOXES);	
	
	LoadLists();
	if (m_b4AnimObj) {
		GetDlgItem(IDC_RADIO_LIST_CONTENT3)->EnableWindow(FALSE);
		if (m_ptrObj&&m_ptrObj->named_object_type() == QD_NAMED_OBJECT_MOUSE_OBJ)
		{
			GetDlgItem(IDC_RADIO_LIST_CONTENT2)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LIST_CONTENT4)->EnableWindow(FALSE);
		}
	}

	//загружаем доступные походки или анимацию
	if(m_bUseMovementState)
	{
		m_iContentType = LIST_CONTENT_GAIT;
		CString str((LPTSTR)IDS_ACCESSIBLE_GAITS);
		GetDlgItem(IDC_STATIC_ANIM)->SetWindowText(str);
		LoadGaits();
	}
	else
		LoadAnimations();
	
	if(!m_bAddObjs)
		m_wndObjName.EnableWindow(FALSE);

	m_hicoSucc = AfxGetApp()->LoadIcon(IDI_ICO_INFO);
	m_hicoWarn = AfxGetApp()->LoadIcon(IDI_ICO_WARN);
	m_hicoErr  = AfxGetApp()->LoadIcon(IDI_ICO_ERR);
	
	
	std::auto_ptr<CImageList> imlist(new CImageList);
	imlist->Create(16, 16, ILC_MASK, 3, 0);
	
	CBitmap bmp;
	bmp.LoadBitmap(IDB_INFO);
	imlist->Add(&bmp, (COLORREF)0xFFFFFF);
	bmp.DeleteObject();
	
	bmp.LoadBitmap(IDB_WARN);
	imlist->Add(&bmp, (COLORREF)0xFFFFFF);
	bmp.DeleteObject();
	
	bmp.LoadBitmap(IDB_STOP);
	imlist->Add(&bmp, (COLORREF)0xFFFFFF);
	bmp.DeleteObject();
	
	m_wndLog.SetImageList(imlist.release());
	
	ClearCheck();
	ClearStatus();

	SetupFileSelector();
#ifndef _AddAnimState_USE_PROP_PAGE
	SetWindowText(m_strCap);
#endif

	
	UpdateData(FALSE);
	return TRUE;  
}

bool AddAnimStateWPg::LoadLists()
{
#ifdef _AddAnimState_USE_PROP_PAGE
	GetParent()->SetupFileSelector();
#endif

	LoadObjects();

	m_ptrSList = merge_lists(m_ptrGameDisp, 
		m_ptrObjDisp, qdGameDispatcher::sound_list);
	if (!m_ptrSList.get()) {
		return false;
	}

	m_ptrAList = merge_lists(m_ptrGameDisp, 
							m_ptrObjDisp, qdGameDispatcher::animation_list);
	if (!m_ptrAList.get()) {
		return false;
	}

	m_ptrGaits = merge_lists(m_ptrGameDisp, m_ptrObjDisp,
		qdGameDispatcher::animation_set_list);
	if (!m_ptrGaits.get()) {
		return false;
	}
	m_list.SetFocus();
	m_list.SetItemState(0, 
		LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	return true;
}

bool AddAnimStateWPg::LoadGaits()
{
	m_list.DeleteAllItems();
	std::copy(m_ptrGaits->begin(), m_ptrGaits->end(), 
		listCtrlInserter::listCtrl_inserter(m_list, 0));
	return true;
}

bool AddAnimStateWPg::LoadAnimations()
{
	m_list.DeleteAllItems();
	std::copy(m_ptrAList->begin(), m_ptrAList->end(), 
		listCtrlInserter::listCtrl_inserter(m_list, 0));
	return true;
}

bool AddAnimStateWPg::LoadSounds()
{
	m_list.DeleteAllItems();
	std::copy(m_ptrSList->begin(), m_ptrSList->end(), 
		listCtrlInserter::listCtrl_inserter(m_list, 0));
	return true;
}

VOID AddAnimStateWPg::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if(pNMListView->uChanged&&LVIF_STATE)
	{
		if (((pNMListView->uNewState>>12)-1)==1) //поставли ли у узла галочку
		{
			m_vecChecked.push_back(pNMListView->iItem);
		}
		if (((pNMListView->uOldState>>12)-1)==1) //убрали ли галочку
		{
			IntVector_t::iterator _itr = 
				qls::find(m_vecChecked.begin(), 
									m_vecChecked.end(), pNMListView->iItem);
			if(_itr != m_vecChecked.end())
				m_vecChecked.erase(_itr);
		}
	}
	ClearStatus();
	*pResult = 0;
}

struct check_type {
	int type;
	explicit check_type(int t):type(t){}
	bool operator()(const qdGameObject* ptrObj) const{
		return (ptrObj->named_object_type() == type);
	}
};

bool AddAnimStateWPg::LoadObjects()
{
	if(m_bAddObjs)
		SetNewObjName();
	else
		m_wndObjName.SetWindowText(m_ptrObj->name());
	return true;
}

BOOL AddAnimStateWPg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN){
		if(pMsg->wParam == VK_RETURN)
		{
			if(pMsg->hwnd == m_hObjNameEdit)
			{
				OnBtnAddState();
				return TRUE;
			}
			else if(pMsg->hwnd == m_list.m_hWnd)
			{
				if(m_bAddObjs)
				{
					m_wndObjName.SetFocus();
					m_wndObjName.SetSel(0, -1);
				}
				else
					OnBtnAddState();
				return TRUE;
			}
		}
	}
	return __super::PreTranslateMessage(pMsg);
}


VOID AddAnimStateWPg::OnSetFocusComboObjName() 
{
	CWnd* pFocus = CWnd::GetFocus();
	m_hObjNameEdit = pFocus->m_hWnd;
	if(m_vecChecked.empty())
		WriteWrn(CString(m_bUseMovementState?
		(LPTSTR)IDS_GAIT_LIST_EMPTY:
		(LPTSTR)IDS_ANIM_LIST_EMPTY));
	else
		ClearStatus();
}

VOID AddAnimStateWPg::OnBtnAddState() 
{
	CWaitCursor __wait__;
	if (!CheckAddConds())
		return ;

	if(m_bAddObjs)
		AddObject();
	else
		AddState();
}

VOID AddAnimStateWPg::ClearCheck()
{
	while(!m_vecChecked.empty()){
		m_list.SetItemState(*m_vecChecked.begin(), 
				INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK);
	}
}

qdGameObjectAnimated* AddAnimStateWPg::InsertObj2Scene(CString& strObjName)
{
	qdGameObjectAnimated* res = NULL;
	if(m_b4AnimObj)
		res = AddAnimated(m_ptrObjDisp, strObjName, false);
	else
		res = AddPersonage(m_ptrObjDisp, strObjName, false);
	return res;
}

bool AddAnimStateWPg::AddStatesToObject(qdGameObjectAnimated* ptrObj)
{
	m_vecInsertedAnimStates.clear();
	if (m_iContentType != LIST_CONTENT_DLG_FILES) 
	{
		split_guard<qdGameDispatcher> __SG__(
			ptree_helpers::is_game_dispatcher(ptrObj->owner())?
			static_cast<qdGameDispatcher*>(ptrObj->owner()): NULL, ptrObj);

		IntVector_t::iterator	_itr = m_vecChecked.begin(), 
			_end = m_vecChecked.end();
		for(; _itr != _end; ++_itr)
		{
			qdGameObjectState* ptrState = NULL;
			CString strText = m_list.GetItemText(*_itr, 0);
			
			switch(m_iContentType) {
			case LIST_CONTENT_ANIMATION:
			case LIST_CONTENT_SOUND:
				{
					qdGameObjectStateStatic* __state = 
						ptree_api::AddEmptyStaticState(ptrObj);
					if(!__state)
						return false;
					
					if(m_iContentType == LIST_CONTENT_ANIMATION)
					{
						__state->animation_info()->set_animation_name(strText);
						m_vecInsertedAnimStates.push_back(__state);
					}
					else if (m_iContentType == LIST_CONTENT_SOUND) {
						__state->set_sound_name(strText);
					}
					ptrState = __state;
				}
				break;
			case LIST_CONTENT_GAIT:
				{
					qdGameObjectStateWalk* __state = 
						ptree_api::AddEmptyMovementState(
							static_cast<qdGameObjectMoving*>(ptrObj));
					if(!__state)
						return false;
					
					__state->animation_set_info()->set_name(strText);
					__state->coords_animation()->set_type(qdCoordsAnimation::CA_WALK);
					ptrState = __state;
				}
				break;
			}

			ptrState->set_name(get_unique_name(strText, ptrObj->state_vector()));
			
			SetModified();
		}
	}
	else {
		const CStringVect_t& vec = GetFiles();
		ASSERT(!vec.empty());

		split_guard<qdGameDispatcher> __SG__(
			(ptrObj->owner()->named_object_type() == QD_NAMED_OBJECT_DISPATCHER)?
			static_cast<qdGameDispatcher*>(ptrObj->owner()): NULL, ptrObj);

		CStringVect_t::const_iterator itr = vec.begin(), end = vec.end();
		for(; itr != end; ++itr){
			qdGameObjectStateStatic* __state = 
				ptree_api::AddEmptyStaticState(ptrObj);
			if(!__state)
				return false;
			
			CString strFileName(*itr);
			CString strText(ReadTextFromFile(strFileName));
			if (strText.IsEmpty()) 
			{
				ptrObj->remove_state(__state);
				delete __state;
			}
			else
			{
//#pragma message( "Insert text setting for state" __FILE__ )
//				__state->set_full_text(strText);
//				__state->set_short_text(strText);

				int pos = strFileName.ReverseFind(_T('.'));
				if (pos != -1) 
					strFileName = strFileName.Left(pos);

				__state->set_name(get_unique_name(strFileName, ptrObj->state_vector()));
				SetModified();
			}
		}
	}

	return true;
}

CString AddAnimStateWPg::ReadTextFromFile(const CString& strFileName)
{
	CFile f;
	
	if (f.Open(strFileName,CFile::modeRead|CFile::shareDenyNone)) 
	{
		CString str;
		char buff[256];
		DWORD dwReadBytes = 0;
		do {
			memset(&buff, 0, sizeof(buff));
			dwReadBytes = f.Read(&buff, 255);
			str += buff;
		} while(dwReadBytes);
		return str;
	}
	return CString();
}

VOID AddAnimStateWPg::SetNewObjName(){
	CString str;
	if(m_b4AnimObj)
		str.LoadString(IDS_NEW_ANIMATED_OBJ_NAME);
	else
		str.LoadString(IDS_NEW_MOVING_OBJ_NAME);

	str = get_obj_uniq_name(str, m_ptrObjDisp);
	m_wndObjName.SetWindowText(str);
}

bool WriteLog(int iImage, CComboBoxEx* ptrCC, const CString& strMsg){
	COMBOBOXEXITEM cbi;
	cbi.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
	cbi.pszText = const_cast<LPTSTR>((LPCTSTR)strMsg);
	cbi.iImage = iImage;
	cbi.iSelectedImage = iImage;
	cbi.iItem = 0;
	int iPos = ptrCC->InsertItem(&cbi);
	if(iPos != CB_ERR)
	{
		ptrCC->SetCurSel(iPos);
		CDC* pDC = ptrCC->GetDC();
		CSize sz = pDC->GetTextExtent(strMsg);
		ptrCC->SetDroppedWidth(sz.cx);
		ptrCC->ReleaseDC(pDC);
		return true;
	}
	return false;
}

bool AddAnimStateWPg::WriteSucc(const CString& strMsg){
	return WriteLog(0, &m_wndLog, strMsg);
}

bool AddAnimStateWPg::WriteWrn(const CString& strMsg){
	return WriteLog(1, &m_wndLog, strMsg);
}

bool AddAnimStateWPg::WriteErr(const CString& strMsg){
	return WriteLog(2, &m_wndLog, strMsg);
}

VOID AddAnimStateWPg::ClearStatus(){
	m_wndLog.SetCurSel(-1);
}

bool AddAnimStateWPg::CheckName(const CString& strName)
{
	CString strTest = get_obj_uniq_name(strName, m_ptrObjDisp);
	return (strTest == strName);
}

VOID AddAnimStateWPg::OnKillfocusComboObjName() 
{
	ClearStatus();
	CString strObjName;
	m_wndObjName.GetWindowText(strObjName);
	if(!CheckName(strObjName))
	{
		CString strMsg;
		strMsg.Format(IDS_NAME_REPEATED, strObjName);
		WriteWrn(strMsg);
	}
}

bool AddAnimStateWPg::CheckAddConds()
{
	switch (m_iContentType) 
	{
	case LIST_CONTENT_DLG_FILES:
		{
			const CStringVect_t& vecFiles = GetFiles();
			if (vecFiles.empty()){
				WriteErr(CString((LPTSTR)IDS_TXT_FILES_LIST_EMPTY));
				m_list_files.SetFocus();
				return false;
			}
		}
		break;
	case LIST_CONTENT_SOUND:
		if(m_vecChecked.empty())
		{
			WriteErr(CString((LPTSTR)IDS_SOUND_LIST_EMPTY));
			m_list.SetFocus();
			return false;
		}
		break;
	default:
		if(m_vecChecked.empty())
		{
			WriteErr(CString(m_bUseMovementState?(LPTSTR)IDS_GAIT_LIST_EMPTY:
									(LPTSTR)IDS_ANIM_LIST_EMPTY));
			m_list.SetFocus();
			return false;
		}
		break;
	}
	return true;
}
bool AddAnimStateWPg::AddObject()
{
	CString strObjName;
	m_wndObjName.GetWindowText(strObjName);
	if(strObjName.IsEmpty())
	{
		WriteErr(CString((LPTSTR)IDS_ERR_EMPTY_NAME));
		return false;
	}

	if(!CheckName(strObjName))
	{
		CString strMsg;
		strMsg.Format(IDS_NAME_REPEATED, strObjName);
		WriteErr(strMsg);

		return false;
	}
	else
		ClearStatus();

	qdGameObjectAnimated* ptrObj = InsertObj2Scene(strObjName);

	if (!ptrObj) {
		WriteErr(CString(m_b4AnimObj ? 
					(LPTSTR)IDS_CNNT_CRT_ANIMATED_OBJ : (LPTSTR)IDS_CNNT_CRT_MOVING_OBJ));
		return false;
	}
	else
		ClearStatus();

	if(!AddStatesToObject(ptrObj))
	{
		WriteErr(CString(m_b4AnimObj ? 
					(LPTSTR)IDS_CNNT_CRT_ANIMATED_OBJ : (LPTSTR)IDS_CNNT_CRT_MOVING_OBJ));
		return false;
	}
	else
		ClearStatus();

	ptrObj->set_default_state();
	if (ptrObj->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ) {
		ptrObj->auto_bound();
		static_cast<qdGameObjectMoving*>(ptrObj)->adjust_z();
	}
	RestoreAnimationScrPlace(m_ptrGameDisp, ptrObj, m_vecInsertedAnimStates);

	SetModified();

	SetNewObjName();

	m_list.SetFocus();
	ClearCheck();
	
	CString strMsg;
	strMsg.Format(IDS_CRT_OBJ_SUCC, ptrObj->name());
	WriteSucc(strMsg);

	return true;	
}

bool AddAnimStateWPg::AddState()
{
	if(!AddStatesToObject(m_ptrObj))
	{
		CString strMsg;
		strMsg.Format(IDS_ADD_STATES_ERR, m_ptrObj->name());
		WriteErr(strMsg);
		return false;
	}

	if (!ptree_helpers::is_obj_mouse(m_ptrObj))
		RestoreAnimationScrPlace(m_ptrGameDisp, m_ptrObj, m_vecInsertedAnimStates);
	
	m_list.SetFocus();
	ClearCheck();

	CString strMsg;
	strMsg.Format(IDS_ADD_STATES_SUCC, m_ptrObj->name());
	WriteSucc(strMsg);
	return true;
}

#ifdef _AddAnimState_USE_PROP_PAGE
BOOL AddAnimStateWPg::OnSetActive() 
{
	UpdateWizardBtnsState();
	LoadLists();	
	return __super::OnSetActive();
}
#endif

BOOL AddAnimStateWPg::DestroyWindow() 
{
	delete m_wndLog.GetImageList();
	return __super::DestroyWindow();
}

bool AddAnimStateWPg::UpdateWizardBtnsState()
{
#ifdef _AddAnimState_USE_PROP_PAGE
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	switch(m_iPagePlace) {
	case PAGE_PLACE_FIRST:
		psheet->SetWizardButtons( PSWIZB_NEXT);
		break;
	case PAGE_PLACE_MIDLE:
		psheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT);
		break;
	case PAGE_PLACE_LAST:
		psheet->SetWizardButtons(PSWIZB_BACK );
		break;
	}
#endif
	return true;
}

void AddAnimStateWPg::OnRadioListContent() 
{
	if (m_iContentType != LIST_CONTENT_ANIMATION) 
	{
		ShowFileCtrl(SW_HIDE);

		CString str((LPTSTR)IDS_ACCESSIBLE_ANIMATION);
		GetDlgItem(IDC_STATIC_ANIM)->SetWindowText(str);
		UpdateData();
		LoadAnimations();
		m_vecChecked.clear();
		m_list.ShowWindow(SW_SHOW);
	}
}

void AddAnimStateWPg::OnRadioListContent2() 
{
	if (m_iContentType != LIST_CONTENT_SOUND) 
	{
		ShowFileCtrl(SW_HIDE);

		CString str((LPTSTR)IDS_ACCESSIBLE_SOUNDS);
		GetDlgItem(IDC_STATIC_ANIM)->SetWindowText(str);
		UpdateData();
		LoadSounds();
		m_vecChecked.clear();
		m_list.ShowWindow(SW_SHOW);
	}
}

void AddAnimStateWPg::OnRadioListContent3() 
{
	if (m_iContentType != LIST_CONTENT_GAIT) 
	{
		ShowFileCtrl(SW_HIDE);

		CString str((LPTSTR)IDS_ACCESSIBLE_GAITS);
		GetDlgItem(IDC_STATIC_ANIM)->SetWindowText(str);
		UpdateData();
		LoadGaits();
		m_vecChecked.clear();

		m_list.ShowWindow(SW_SHOW);
	}
}

void AddAnimStateWPg::OnRadioListContent4() 
{
	//страхуемся от нескольких кликов по одной и той же кнопке
	if (LIST_CONTENT_DLG_FILES != m_iContentType)
	{
		m_list.ShowWindow(SW_HIDE);
		ClearStatus();

		CString str((LPTSTR)IDS_ACCESSIBLE_DLG_FILES);
		GetDlgItem(IDC_STATIC_ANIM)->SetWindowText(str);
		UpdateData();

		ShowFileCtrl(SW_SHOW);
	}
}

VOID AddAnimStateWPg::SetupFileSelector()
{
	CRect rcList;
	m_list.GetWindowRect(rcList);
	ScreenToClient(rcList);
	

	SetupFileCtrlsLayout(rcList);
	m_list_files.InsertColumn(0, static_cast<LPCTSTR>(NULL));
	EnableFileImages();
	SetFileExts(_T("*.txt"));
	TRACE("CurDirectory before tree populating: %s\n",
		LPCTSTR(CurDirSafer::GetCurrentDirectory()));
	m_tree.PopulateTree();
	TRACE("CurDirectory after tree populating: %s\n",
		LPCTSTR(CurDirSafer::GetCurrentDirectory()));

	SetPath(CurDirSafer::GetCurrentDirectory());
}

VOID AddAnimStateWPg::SetPath(const CString & strPath){
	m_bTunneling = true;
	TRACE(strPath);
	m_tree.TunnelTree(strPath);
	m_bTunneling = false;
	HTREEITEM hSelected = m_tree.GetSelectedItem();
	if (hSelected){
		m_tree.SelectItem(NULL);
		m_tree.SelectItem(hSelected);
	}
}

VOID AddAnimStateWPg::SetFileExts(LPCTSTR lpszExt){
	m_strFileExts = lpszExt;
}

VOID AddAnimStateWPg::EnableFileImages(){
	m_tree.EnableImages();
	m_list_files.SetImageList(m_tree.GetImageList(TVSIL_NORMAL), LVSIL_SMALL);
}

VOID AddAnimStateWPg::SetupFileCtrlsLayout(const CRect &rcBound){
	CRect rcWnd(rcBound);
	rcWnd.DeflateRect(1, 1, rcBound.Width()/3+1, 1);
	
	m_tree.MoveWindow(rcWnd);
	
	rcWnd.left = rcWnd.right + 1;
	rcWnd.right = rcBound.right - 1;
	
	m_list_files.MoveWindow(rcWnd);
}

bool AddAnimStateWPg::FillList(const CString & strRoot)
{
	m_list_files.DeleteAllItems();
	SetCurrentDirectory(strRoot);
	
	m_vecFiles.clear();
	
	//выбираем поочереди все поддиректории
	file_enumerator fe(m_strFileExts);
	if(!fe.good())
		return false;
	
	typedef std::vector<file_find_data> ffd_vec_t;
	ffd_vec_t vecFiles;
	std::copy(fe.begin(), fe.end(), std::back_inserter(vecFiles));
	
	if(vecFiles.empty())
		return true;
	
	std::sort(vecFiles.begin(), vecFiles.end());
	
	ffd_vec_t::const_iterator i = vecFiles.begin(), e = vecFiles.end();
	std::copy(i, e, listCtrlInserter::listCtrl_inserter(m_list_files, 0));
	
	m_list_files.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	return true;
}

const CStringVect_t& AddAnimStateWPg::GetFiles(){
	if (m_vecFiles.empty())
		MakeFilesList();
	return m_vecFiles;
}

VOID AddAnimStateWPg::MakeFilesList()
{
	POSITION pos = m_list_files.GetFirstSelectedItemPosition();
	while (pos) {
		int iIndex = m_list_files.GetNextSelectedItem(pos);
		m_vecFiles.push_back(m_list_files.GetItemText(iIndex, 0));
	}
}

VOID AddAnimStateWPg::ShowFileCtrl(int nCmdShow)
{
	m_tree.ShowWindow(nCmdShow);
	m_list_files.ShowWindow(nCmdShow);
}

void AddAnimStateWPg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if(m_tree.OnFolderSelected(pNMHDR, pResult, m_strRoot))
	{
		if(!m_bTunneling)
			FillList(m_strRoot);
	}
	
	*pResult = 0;
}

void AddAnimStateWPg::OnDestroy() 
{
	m_list_files.SetImageList(NULL, LVSIL_SMALL);
	__super::OnDestroy();
}
