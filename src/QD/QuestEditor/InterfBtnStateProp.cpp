// InterfBtnStateProp.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "InterfBtnStateProp.h"

#include "qd_interface_element_state.h"
#include "qd_interface_button.h"
#include "qd_interface_screen.h"

#include "qd_game_object.h"
#include "qd_video.h"

#include <ShLwApi.h>

#include "custom_ddx.h"
#include "sv_api.h"
#include "interf_events.h"
#include "interf_disk_op.h"
#include "file_tests.h"
#include "ConvertTGAtoQDA.h"
#include "EdgeRemover.h"

// InterfBtnStateProp dialog

bool EditInterfBtnState(qdInterfaceElementState* pstate,
						qdInterfaceElementState::state_mode_t state_mode)
{
	InterfBtnStateProp dlg;
	dlg.set_data(pstate);
	dlg.set_start_mode(state_mode);
	dlg.DoModal();

	return dlg.get_modified();
}


IMPLEMENT_DYNAMIC(InterfBtnStateProp, CDialog)

InterfBtnStateProp::InterfBtnStateProp()
	: CDialog(InterfBtnStateProp::IDD)
	, m_strAnimFileName(_T(""))
	, m_strSoundFile(_T(""))
	, m_bModeModified(false)
	, m_bModified(false)
	, start_mode_(qdInterfaceElementState::state_mode_t::DEFAULT_MODE)
{
}

InterfBtnStateProp::~InterfBtnStateProp()
{
}

void InterfBtnStateProp::set_start_mode(qdInterfaceElementState::state_mode_t const m){
	start_mode_ = m;
}

void InterfBtnStateProp::set_data(qdInterfaceElementState* pstate)
{
	m_ptrState.reset(new qdInterfaceElementState(*pstate));
	m_ptrSourceState = pstate;
}


void InterfBtnStateProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MODES, m_wndModes);
	custom_ddx::DDX_Text(pDX, IDC_EDIT_ANIM_FILE, m_strAnimFileName, 
		file_tests::strict_file_test());
	custom_ddx::DDX_Text(pDX, IDC_EDIT_SOUND_FILE, m_strSoundFile, 
		file_tests::non_strict_file_test());
	DDX_Control(pDX, IDC_LIST, m_wndActions);
}


BEGIN_MESSAGE_MAP(InterfBtnStateProp, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_MODES, OnCbnSelchangeComboModes)
	ON_BN_CLICKED(IDC_BTN_SEL_ANIM, OnBnClickedBtnSelAnim)
	ON_BN_CLICKED(IDC_BTN_SEL_SOUND, OnBnClickedBtnSelSound)
	ON_BN_CLICKED(IDC_CHECK_HORZ_INV, OnBnClickedCheckHorzInv)
	ON_BN_CLICKED(IDC_CHECK_VERT_INV, OnBnClickedCheckVertInv)
	ON_BN_CLICKED(IDC_CHECK_LOOP, OnBnClickedCheckLoop)
	ON_BN_CLICKED(IDC_CHECK_SOUND_LOOP, OnBnClickedCheckSoundLoop)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST, OnLvnKeydownList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnNMRclickList)
	ON_NOTIFY(NM_LCE_EDITCONTROL_REQUEST, IDC_LIST, OnNMEdtCtrlReq)
	ON_NOTIFY(NM_LCE_EDITREQUEST, IDC_LIST,			OnNMEditReq)
	ON_NOTIFY(NM_LCE_SETUP_CTRL, IDC_LIST,			OnNMSetupEdit)
	ON_NOTIFY(NM_LCE_ENDEDIT, IDC_LIST,				OnNMEndEdit)
	ON_COMMAND(IDC_ADD_EVENT, OnAddEvent)
	ON_COMMAND(IDC_REMOVE_EVENT, OnRemoveEvent)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnNMDblclkList)
END_MESSAGE_MAP()


qdInterfaceElementState::state_mode_t InterfBtnStateProp::GetCurMode()
{
	ASSERT(m_wndModes.GetCurSel() != CB_ERR);
	return static_cast<qdInterfaceElementState::state_mode_t>
				(m_wndModes.GetItemData(m_wndModes.GetCurSel()));
}

bool InterfBtnStateProp::SetMode(int mode)
{
	int const cnt = m_wndModes.GetCount();
	for(int i = 0; i < cnt; ++i)
	{
		DWORD_PTR data = m_wndModes.GetItemData(i);
		if (data == mode)
		{
			m_wndModes.SetCurSel(i);
			return true;
		}
	}
	return false;
}

void InterfBtnStateProp::state_mode2wnd(qdInterfaceElementState::state_mode_t const m)
{
	m_strAnimFileName = m_ptrSourceState->animation_file(m);
	m_strSoundFile = m_ptrSourceState->sound_file(m);

	CButton* pb = static_cast<CButton*>(GetDlgItem(IDC_CHECK_HORZ_INV));

	ASSERT(BST_UNCHECKED == static_cast<int>(false));
	ASSERT(BST_CHECKED == static_cast<int>(true));

	pb->SetCheck(
		m_ptrSourceState->check_animation_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL, m)); 

	pb = static_cast<CButton*>(GetDlgItem(IDC_CHECK_VERT_INV));
	pb->SetCheck(
		m_ptrSourceState->check_animation_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL, m)); 

	pb = static_cast<CButton*>(GetDlgItem(IDC_CHECK_LOOP));
	pb->SetCheck(
		m_ptrSourceState->check_animation_flag(QD_ANIMATION_FLAG_LOOP, m)); 

}

void InterfBtnStateProp::wnd2state_mode(qdInterfaceElementState::state_mode_t const md)
{
	m_strAnimFileName.Trim();
	if (m_strAnimFileName.IsEmpty())
		m_ptrSourceState->set_animation_file(NULL, md);
	else
		m_ptrSourceState->set_animation_file(m_strAnimFileName, md);

	m_strSoundFile.Trim();
	if (m_strSoundFile.IsEmpty())
		m_ptrSourceState->set_sound_file(NULL, md);
	else
		m_ptrSourceState->set_sound_file(m_strSoundFile, md);
}

// InterfBtnStateProp message handlers
//! ¬ зависимости от режима назначает заголовок окна
void InterfBtnStateProp::setDlgCaption(qdInterfaceElementState::state_mode_t const md)
{
	switch(md)
	{
		case qdInterfaceElementState::DEFAULT_MODE:
			SetWindowText(CString((LPTSTR)IDS_INTRF_ELE_STATE_MODE_DEFAULT));
			break;
		case qdInterfaceElementState::MOUSE_HOVER_MODE:
			SetWindowText(CString((LPTSTR)IDS_INTRF_ELE_STATE_MODE_MOUSE_OVER));
			break;
		case qdInterfaceElementState::EVENT_MODE:
			SetWindowText(CString((LPTSTR)IDS_INTRF_ELE_STATE_MODE_EVENT));
			break;
	}
}

void InterfBtnStateProp::LoadModes()
{
	CString str((LPTSTR)IDS_INTRF_ELE_STATE_MODE_DEFAULT);
	int iItem = m_wndModes.AddString(str);
	if (iItem != CB_ERR)
		m_wndModes.SetItemData(iItem, 
						static_cast<DWORD_PTR>(qdInterfaceElementState::DEFAULT_MODE));
	str.LoadString(IDS_INTRF_ELE_STATE_MODE_MOUSE_OVER);
	iItem = m_wndModes.AddString(str);
	if (iItem != CB_ERR)
		m_wndModes.SetItemData(iItem, 
			static_cast<DWORD_PTR>(qdInterfaceElementState::MOUSE_HOVER_MODE));

	str.LoadString(IDS_INTRF_ELE_STATE_MODE_EVENT);
	iItem = m_wndModes.AddString(str);
	if (iItem != CB_ERR)
		m_wndModes.SetItemData(iItem, 
			static_cast<DWORD_PTR>(qdInterfaceElementState::EVENT_MODE));
	m_wndModes.SetCurSel(0);
}

void InterfBtnStateProp::AdjustColumnsWidth()
{
	m_wndActions.SetColumnWidth(0,  LVSCW_AUTOSIZE_USEHEADER);
	m_wndActions.SetColumnWidth(1,  LVSCW_AUTOSIZE_USEHEADER);
	m_wndActions.SetColumnWidth(2,  LVSCW_AUTOSIZE_USEHEADER);
}

BOOL InterfBtnStateProp::OnInitDialog()
{
	CDialog::OnInitDialog();

	LoadModes();
	m_ptrSourceState->set_state_mode(start_mode_);
	if (SetMode(start_mode_))
		state_mode2wnd(start_mode_);
	setDlgCaption(start_mode_);
	m_wndActions.SetExtendedStyle(
		m_wndActions.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	AddListColumns();
	LoadEvents();

	AdjustColumnsWidth();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void InterfBtnStateProp::OnOK()
{
	__super::OnOK();
	wnd2state_mode(m_ptrSourceState->state_mode());
	m_ptrSourceState->set_state_mode(qdInterfaceElementState::DEFAULT_MODE);
	static_cast<qdInterfaceButton*>(m_ptrSourceState->owner())->init();
	EventsToState();
}

void InterfBtnStateProp::OnCancel()
{
	*m_ptrSourceState = *m_ptrState.get();
	m_ptrSourceState->set_state_mode(qdInterfaceElementState::DEFAULT_MODE);
	static_cast<qdInterfaceButton*>(m_ptrSourceState->owner())->init();
	CDialog::OnCancel();
}

void InterfBtnStateProp::OnCbnSelchangeComboModes()
{
	const int iCurSel = m_wndModes.GetCurSel();
	if (iCurSel == CB_ERR)
		return;

	UpdateData();
	wnd2state_mode(m_ptrSourceState->state_mode());
	qdInterfaceElementState::state_mode_t const md = GetCurMode();
	m_ptrSourceState->set_state_mode(md);
	state_mode2wnd(md);
	UpdateData(FALSE);
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}

void InterfBtnStateProp::OnBnClickedBtnSelAnim()
{
	CString res = GetIntrfAnimFileName();
	if (res.IsEmpty())
		return;
	ConvertTGAtoQDA convertor;
	res = convertor.convert(res);
	if (res.IsEmpty())
		return;
	EdgeRemover::remove_edges(res);
	m_strAnimFileName = res;
	wnd2state_mode(GetCurMode());
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	UpdateData(FALSE);
}

void InterfBtnStateProp::OnBnClickedBtnSelSound()
{
	CString const res = GetIntrfSoundFileName();
	if (res.IsEmpty())
		return;
	m_strSoundFile = res;
	wnd2state_mode(GetCurMode());
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	UpdateData(FALSE);
}

void InterfBtnStateProp::OnBnClickedCheckHorzInv()
{
	qdInterfaceElementState::state_mode_t const md = GetCurMode();

	if (m_ptrSourceState->check_animation_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL, md)) 
		m_ptrSourceState->drop_animation_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL, md);
	else
		m_ptrSourceState->set_animation_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL, md);

	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);

	SetModified();
}

void InterfBtnStateProp::OnBnClickedCheckVertInv()
{
	qdInterfaceElementState::state_mode_t const md = GetCurMode();

	if (m_ptrSourceState->check_animation_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL, md)) 
		m_ptrSourceState->drop_animation_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL, md);
	else
		m_ptrSourceState->set_animation_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL, md);

	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);

	SetModified();
}

void InterfBtnStateProp::OnBnClickedCheckLoop()
{
	qdInterfaceElementState::state_mode_t const md = GetCurMode();

	if (m_ptrSourceState->check_animation_flag(QD_ANIMATION_FLAG_LOOP, md)) 
		m_ptrSourceState->drop_animation_flag(QD_ANIMATION_FLAG_LOOP, md);
	else
		m_ptrSourceState->set_animation_flag(QD_ANIMATION_FLAG_LOOP, md);

	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);

	SetModified();
}

void InterfBtnStateProp::OnBnClickedCheckSoundLoop()
{
}

void InterfBtnStateProp::SetModified()
{
	m_bModified = true;
}

bool InterfBtnStateProp::get_modified()
{
	return m_bModified;
}

void InterfBtnStateProp::OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if (pLVKeyDow->wVKey == VK_DELETE)
		OnRemoveEvent();
	*pResult = 0;
}

void InterfBtnStateProp::OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu menu;
	menu.LoadMenu(IDR_INTERF_EVENTS);
	CPoint pt;
	GetCursorPos(&pt);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTBUTTON, pt.x, pt.y, this);
	*pResult = 0;
}

void InterfBtnStateProp::AddListColumns()
{
	m_wndActions.InsertColumn(0, CString((LPTSTR)IDS_CLMN_ACTION), LVCFMT_LEFT, 100);
	m_wndActions.InsertColumn(1, CString((LPTSTR)IDS_CLMN_EXECUTE), LVCFMT_LEFT, 100);
	m_wndActions.SetEditCtrlType(1, ELC_ITEM_EDIT_CTRL_COMBO);
	m_wndActions.InsertColumn(2, CString((LPTSTR)IDS_CLMN_PARAMETER), LVCFMT_LEFT, 100);
	m_wndActions.SetEditCtrlType(2, ELC_ITEM_EDIT_CTRL_COMBO);
}

void InterfBtnStateProp::LoadEventsToList()
{
	EventsList_t::const_iterator ix = m_events_list.begin(),
								 ex = m_events_list.end();
	CString str((LPTSTR)IDS_AFTER_ANIMATION);
	CString str1((LPTSTR)IDS_BEFORE_ANIMATION);

	int i = 0;
	for(;ix != ex; ++ix)
	{
		qdInterfaceEventPtr pevent = *ix;
		int item = m_wndActions.InsertItem(i++,GetEventDescrip(*pevent.get()));
		if (item != -1)
		{
			if (pevent->is_before_animation())
				m_wndActions.SetItemText(item, 1, str1);
			else
				m_wndActions.SetItemText(item, 1, str);
			if (pevent->has_data())
				m_wndActions.SetItemText(item, 2, pevent->event_data());
			m_wndActions.SetItemData(item, reinterpret_cast<DWORD_PTR>(pevent.get()));
		}
	}
}

void InterfBtnStateProp::LoadEvents()
{
	const qdInterfaceElementState::events_container_t& evt = m_ptrSourceState->events();
	qdInterfaceElementState::events_container_t::const_iterator b = evt.begin(),
																e = evt.end();
	for(;b != e; ++b)
		m_events_list.push_back(qdInterfaceEventPtr(new qdInterfaceEvent(*b)));
	LoadEventsToList();
}

void InterfBtnStateProp::OnNMEdtCtrlReq(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LCE_INFO *pli = reinterpret_cast<NM_LCE_INFO*>(pNMHDR);
	if (pli->item.iSubItem == 2)
	{
		pli->iEditCtrlType = ELC_ITEM_EDIT_CTRL_COMBO;
		*pResult = TRUE;
	}
}

void InterfBtnStateProp::OnNMEndEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LCE_INFO *pli = reinterpret_cast<NM_LCE_INFO*>(pNMHDR);
	CCJFlatComboBox* pcombo = static_cast<CCJFlatComboBox*>(pli->pCtrl);
	if (pli->item.iSubItem == 0)
	{
		int iSel = pcombo->GetCurSel();
		if (iSel == CB_ERR||!*pli->item.pszText)
			m_wndActions.DeleteItem(pli->item.iItem);
		else
		{
			qdInterfaceEvent::event_t const e_id = 
				static_cast<qdInterfaceEvent::event_t>(pcombo->GetItemData(iSel));

			qdInterfaceEvent* pevnt = AddEvent(e_id);
			m_wndActions.SetItemData(pli->item.iItem, 
				reinterpret_cast<DWORD_PTR>(pevnt));
			if (GetHasData(pevnt->event()))
			{
				LPCTSTR data = GetDefVal(pevnt->event());
				if (data)
				{
					m_wndActions.SetItemText(pli->item.iItem, 2, data);
					pevnt->set_event_data(data);
				}
			}

			*pResult = 1;
			m_wndActions.SetColumnWidth(0,  LVSCW_AUTOSIZE_USEHEADER);
		}
	}
	if (pli->item.iSubItem == 1)
	{
		if (*pli->item.pszText)
		{
			qdInterfaceEvent* const pevent =
				reinterpret_cast<qdInterfaceEvent*>
				(m_wndActions.GetItemData(pli->item.iItem));
			pevent->set_before_animation(pcombo->GetItemData(pcombo->GetCurSel()));
			*pResult = TRUE;
		}
	}
	if (pli->item.iSubItem == 2)//редактировали данные дл€ эвента
	{
		if (*pli->item.pszText)
		{
			qdInterfaceEvent* const pevent =reinterpret_cast<qdInterfaceEvent*>
				(m_wndActions.GetItemData(pli->item.iItem));
			CString str;
			pcombo->GetLBText(pcombo->GetCurSel(), str);
			pevent->set_event_data(str);
			m_wndActions.SetColumnWidth(2,  LVSCW_AUTOSIZE_USEHEADER);
			*pResult = TRUE;
		}
	}
}

void InterfBtnStateProp::OnNMSetupEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LCE_INFO *pli = reinterpret_cast<NM_LCE_INFO*>(pNMHDR);
	if (pli->item.iSubItem == 1)
	{
		CCJFlatComboBox* pcombo = static_cast<CCJFlatComboBox*>(pli->pCtrl);
		int iitem2 = pcombo->AddString(CString((LPTSTR)IDS_AFTER_ANIMATION));
		int iitem  = pcombo->AddString(CString((LPTSTR)IDS_BEFORE_ANIMATION));
		if (iitem != CB_ERR)
			pcombo->SetItemData(iitem, TRUE);

		qdInterfaceEvent* const pevent =
			reinterpret_cast<qdInterfaceEvent*>
						(m_wndActions.GetItemData(pli->item.iItem));
		if (pevent->is_before_animation())
			pcombo->SetCurSel(iitem);
		else
			pcombo->SetCurSel(iitem2);
	}
	if (!pli->item.iSubItem)
	{
		CCJFlatComboBox* pcombo = static_cast<CCJFlatComboBox*>(pli->pCtrl);

		size_t count = m_EventsRep.events_count();
		for(int i = 0; i < count; ++i)
		{
			int item = pcombo->AddString(m_EventsRep[i]->description());
			if (item != CB_ERR)
				pcombo->SetItemData(item, m_EventsRep[i]->eventId());
		}
	}
	if (pli->item.iSubItem == 2)
	{
		qdInterfaceEvent* pev = 
			reinterpret_cast<qdInterfaceEvent*>
											(m_wndActions.GetItemData(pli->item.iItem));
		LoadCorrespondigData(pli->pCtrl, pev->event());
		CCJFlatComboBox* combo = static_cast<CCJFlatComboBox*>(pli->pCtrl);
		if (pev->event_data())
			combo->SelectString(-1, pev->event_data());
		if (combo->GetCurSel())
			combo->SetCurSel(0);
	}
}

void InterfBtnStateProp::OnNMEditReq(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LCE_INFO *pli = reinterpret_cast<NM_LCE_INFO*>(pNMHDR);
	if (pli->item.iSubItem == 2)
	{
		qdInterfaceEvent* const pevent =
			reinterpret_cast<qdInterfaceEvent*>
				(m_wndActions.GetItemData(pli->item.iItem));

		*pResult = !GetHasData(pevent->event());
	}
}

void InterfBtnStateProp::OnAddEvent()
{
	int item = m_wndActions.InsertItem(m_wndActions.GetItemCount(),_T(""));
	if (item != -1)
	{
		m_wndActions.SetItemText(item, 1, CString((LPTSTR)IDS_AFTER_ANIMATION));
		m_wndActions.EditSubItem(item, 0, ELC_ITEM_EDIT_CTRL_COMBO);
	}
}

void InterfBtnStateProp::OnRemoveEvent()
{
	POSITION pos = m_wndActions.GetFirstSelectedItemPosition();
	while (pos) {
		int nItem = m_wndActions.GetNextSelectedItem(pos);
		qdInterfaceEvent* const pevent =
			reinterpret_cast<qdInterfaceEvent*>(m_wndActions.GetItemData(nItem));
		if (RemoveEventFromList(pevent))
			m_wndActions.DeleteItem(nItem);

		pos = m_wndActions.GetFirstSelectedItemPosition();
	}
	
}				 

bool InterfBtnStateProp::GetHasData(qdInterfaceEvent::event_t event)
{
	return m_EventsRep[event]->hasData();
}

qdInterfaceEvent* InterfBtnStateProp::AddEvent(qdInterfaceEvent::event_t event)
{
	m_events_list.push_back(
		qdInterfaceEventPtr(new qdInterfaceEvent(event, _T(""), false)));

	return m_events_list.back().get();
}

void InterfBtnStateProp::EventsToState()
{
	m_ptrSourceState->erase_all_events();
	EventsList_t::const_iterator ix = m_events_list.begin(), ex = m_events_list.end();
	for(;ix != ex; ++ix)
		m_ptrSourceState->add_event(*(ix->get()));
}

LPCTSTR InterfBtnStateProp::GetEventDescrip(qdInterfaceEvent const& event)
{
	return m_EventsRep[event.event()]->description();
}

LPCTSTR InterfBtnStateProp::GetDefVal(qdInterfaceEvent::event_t type){
	return m_EventsRep[type]->getDefaultValue();
}

void InterfBtnStateProp::LoadCorrespondigData(CWnd* pwnd, qdInterfaceEvent::event_t type)
{
	m_EventsRep[type]->loadData(pwnd);
}


bool InterfBtnStateProp::RemoveEventFromList(qdInterfaceEvent* pevent)
{
	EventsList_t::iterator ix = m_events_list.begin(),
						  ex = m_events_list.end();
	for(; ix != ex; ++ix)
	{
		if (ix->get() == pevent)
		{
			m_events_list.erase(ix);
			return true;
		}
	}
	return false;
}

void InterfBtnStateProp::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LCE_INFO *pli = reinterpret_cast<NM_LCE_INFO*>(pNMHDR);
	CPoint pt;
	GetCursorPos(&pt);
	m_wndActions.ScreenToClient(&pt);
	LVHITTESTINFO lvhti;
	lvhti.pt = pt;
	m_wndActions.SubItemHitTest(&lvhti);
	if (lvhti.flags&LVHT_ONITEM)
	{
		if (lvhti.iSubItem)
			m_wndActions.EditSubItem(lvhti.iItem, lvhti.iSubItem);
	}
	if (lvhti.flags&LVHT_NOWHERE)
		OnAddEvent();
	*pResult = 0;
}

BOOL InterfBtnStateProp::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
	{
		switch(::GetDlgCtrlID(pMsg->hwnd)) {
		case IDC_EDIT_ANIM_FILE:
		case IDC_EDIT_SOUND_FILE:
			if (UpdateData())
				SetModified();
			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}
