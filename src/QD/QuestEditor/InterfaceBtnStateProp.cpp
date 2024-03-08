// InterfaceBtnStateProp.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "InterfaceBtnStateProp.h"
#include ".\interfacebtnstateprop.h"

//! Запускает диалог редактирования
bool EditInterfaceBtnState(qdInterfaceElementState* pstate)
{
	InterfaceBtnStateProp dlg;
	dlg.set_data(pstate);
//	dlg.set_start_mode(state_mode);
	dlg.DoModal();

	return dlg.get_modified();
}

// InterfaceBtnStateProp dialog

IMPLEMENT_DYNAMIC(InterfaceBtnStateProp, CDialog)
InterfaceBtnStateProp::InterfaceBtnStateProp(CWnd* pParent /*=NULL*/)
	: CDialog(InterfaceBtnStateProp::IDD, pParent)
	, modified_(false)
{
}

InterfaceBtnStateProp::~InterfaceBtnStateProp()
{
}

void InterfaceBtnStateProp::set_data(qdInterfaceElementState* pstate)
{
	m_ptrState.reset(new qdInterfaceElementState(*pstate));
	m_ptrSourceState = pstate;
}

void InterfaceBtnStateProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_wndActions);
}


BEGIN_MESSAGE_MAP(InterfaceBtnStateProp, CDialog)
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


// InterfaceBtnStateProp message handlers
bool InterfaceBtnStateProp::get_modified(){
	return modified_;
}
void InterfaceBtnStateProp::set_modified(bool flag){
	modified_ = flag;
}

BOOL InterfaceBtnStateProp::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndActions.SetExtendedStyle(
		m_wndActions.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	AddListColumns();
	LoadEvents();

	AdjustColumnsWidth();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void InterfaceBtnStateProp::OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if (pLVKeyDow->wVKey == VK_DELETE)
		OnRemoveEvent();
	*pResult = 0;
}

void InterfaceBtnStateProp::OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu menu;
	menu.LoadMenu(IDR_INTERF_EVENTS);
	CPoint pt;
	GetCursorPos(&pt);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTBUTTON, pt.x, pt.y, this);
	*pResult = 0;
}

void InterfaceBtnStateProp::AddListColumns()
{
	m_wndActions.InsertColumn(0, CString((LPTSTR)IDS_CLMN_ACTION), LVCFMT_LEFT, 100);
	m_wndActions.InsertColumn(1, CString((LPTSTR)IDS_CLMN_EXECUTE), LVCFMT_LEFT, 100);
	m_wndActions.SetEditCtrlType(1, ELC_ITEM_EDIT_CTRL_COMBO);
	m_wndActions.InsertColumn(2, CString((LPTSTR)IDS_CLMN_EXECUTE_TYPE), LVCFMT_LEFT, 100);
	m_wndActions.SetEditCtrlType(2, ELC_ITEM_EDIT_CTRL_COMBO);
	m_wndActions.InsertColumn(3, CString((LPTSTR)IDS_CLMN_PARAMETER), LVCFMT_LEFT, 200);
	m_wndActions.SetEditCtrlType(3, ELC_ITEM_EDIT_CTRL_COMBO);
}

void InterfaceBtnStateProp::LoadEventsToList()
{
	EventsList_t::const_iterator ix = m_events_list.begin(),
		ex = m_events_list.end();
	CString str((LPTSTR)IDS_AFTER_ANIMATION);
	CString str1((LPTSTR)IDS_BEFORE_ANIMATION);

	CString str2((LPTSTR)IDS_EXECUTE_CLICK);
	CString str3((LPTSTR)IDS_EXECUTE_HOVER);

	int i = 0;
	for(;ix != ex; ++ix)
	{
		qdInterfaceEventPtr pevent = *ix;
		int item = m_wndActions.InsertItem(i++,GetEventDescrip(*pevent.get()));
		if (item != -1)
		{
			if(pevent->is_before_animation())
				m_wndActions.SetItemText(item, 1, str1);
			else
				m_wndActions.SetItemText(item, 1, str);

			if(pevent->activation() == qdInterfaceEvent::EVENT_ACTIVATION_HOVER)
				m_wndActions.SetItemText(item, 2, str3);
			else
				m_wndActions.SetItemText(item, 2, str2);

			if(pevent->has_data())
				m_wndActions.SetItemText(item, 3, pevent->event_data());
			m_wndActions.SetItemData(item, reinterpret_cast<DWORD_PTR>(pevent.get()));
		}
	}
}

void InterfaceBtnStateProp::LoadEvents()
{
	const qdInterfaceElementState::events_container_t& evt = 
		m_ptrSourceState->events();
	qdInterfaceElementState::events_container_t::const_iterator b = evt.begin(),
		e = evt.end();
	for(;b != e; ++b)
		m_events_list.push_back(qdInterfaceEventPtr(new qdInterfaceEvent(*b)));
	LoadEventsToList();
}

void InterfaceBtnStateProp::OnNMEdtCtrlReq(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LCE_INFO *pli = reinterpret_cast<NM_LCE_INFO*>(pNMHDR);
	if (pli->item.iSubItem == 2)
	{
		pli->iEditCtrlType = ELC_ITEM_EDIT_CTRL_COMBO;
		*pResult = TRUE;
	}
}

void InterfaceBtnStateProp::OnNMEndEdit(NMHDR *pNMHDR, LRESULT *pResult)
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
					m_wndActions.SetItemText(pli->item.iItem, 3, data);
					pevnt->set_event_data(data);
				}
			}
			set_modified(true);

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
			m_wndActions.SetColumnWidth(1,  LVSCW_AUTOSIZE_USEHEADER);
		}
	}
	if (pli->item.iSubItem == 2)
	{
		if (*pli->item.pszText)
		{
			qdInterfaceEvent* const pevent =
				reinterpret_cast<qdInterfaceEvent*>
				(m_wndActions.GetItemData(pli->item.iItem));
			pevent->set_activation(qdInterfaceEvent::activation_t(pcombo->GetItemData(pcombo->GetCurSel())));
			*pResult = TRUE;
			m_wndActions.SetColumnWidth(2,  LVSCW_AUTOSIZE_USEHEADER);
		}
	}
	if (pli->item.iSubItem == 3)//редактировали данные для эвента
	{
		if (*pli->item.pszText)
		{
			qdInterfaceEvent* const pevent =reinterpret_cast<qdInterfaceEvent*>
				(m_wndActions.GetItemData(pli->item.iItem));
			CString str;
			pcombo->GetLBText(pcombo->GetCurSel(), str);
			pevent->set_event_data(str);
			m_wndActions.SetColumnWidth(3,  LVSCW_AUTOSIZE_USEHEADER);
			*pResult = TRUE;
		}
	}
}

void InterfaceBtnStateProp::OnNMSetupEdit(NMHDR *pNMHDR, LRESULT *pResult)
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
	if (pli->item.iSubItem == 2)
	{
		CCJFlatComboBox* pcombo = static_cast<CCJFlatComboBox*>(pli->pCtrl);
		int iitem = pcombo->AddString(CString((LPTSTR)IDS_EXECUTE_CLICK));
		int iitem2  = pcombo->AddString(CString((LPTSTR)IDS_EXECUTE_HOVER));
		if (iitem != CB_ERR)
			pcombo->SetItemData(iitem, qdInterfaceEvent::EVENT_ACTIVATION_CLICK);
		if (iitem2 != CB_ERR)
			pcombo->SetItemData(iitem2, qdInterfaceEvent::EVENT_ACTIVATION_HOVER);

		qdInterfaceEvent* const pevent =
			reinterpret_cast<qdInterfaceEvent*>
			(m_wndActions.GetItemData(pli->item.iItem));
		if (pevent->activation() == qdInterfaceEvent::EVENT_ACTIVATION_HOVER)
			pcombo->SetCurSel(iitem2);
		else
			pcombo->SetCurSel(iitem);
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
	if (pli->item.iSubItem == 3)
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

void InterfaceBtnStateProp::OnNMEditReq(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LCE_INFO *pli = reinterpret_cast<NM_LCE_INFO*>(pNMHDR);
	if (pli->item.iSubItem == 3)
	{
		qdInterfaceEvent* const pevent =
			reinterpret_cast<qdInterfaceEvent*>
			(m_wndActions.GetItemData(pli->item.iItem));

		*pResult = !GetHasData(pevent->event());
	}
}

void InterfaceBtnStateProp::OnAddEvent()
{
	int item = m_wndActions.InsertItem(m_wndActions.GetItemCount(),_T(""));
	if (item != -1)
	{
		m_wndActions.SetItemText(item, 1, CString((LPTSTR)IDS_AFTER_ANIMATION));
		m_wndActions.SetItemText(item, 2, CString((LPTSTR)IDS_EXECUTE_CLICK));
		m_wndActions.EditSubItem(item, 0, ELC_ITEM_EDIT_CTRL_COMBO);
	}
}

void InterfaceBtnStateProp::OnRemoveEvent()
{
	POSITION pos = m_wndActions.GetFirstSelectedItemPosition();
	while (pos) {
		int nItem = m_wndActions.GetNextSelectedItem(pos);
		qdInterfaceEvent* const pevent =
			reinterpret_cast<qdInterfaceEvent*>(m_wndActions.GetItemData(nItem));
		if (RemoveEventFromList(pevent))
		{
			m_wndActions.DeleteItem(nItem);
			set_modified(true);
		}

		pos = m_wndActions.GetFirstSelectedItemPosition();
	}

}				 

bool InterfaceBtnStateProp::GetHasData(qdInterfaceEvent::event_t event)
{
	return m_EventsRep[event]->hasData();
}

qdInterfaceEvent* InterfaceBtnStateProp::AddEvent(qdInterfaceEvent::event_t event)
{
	m_events_list.push_back(
		qdInterfaceEventPtr(new qdInterfaceEvent(event, _T(""), false)));

	return m_events_list.back().get();
}

void InterfaceBtnStateProp::EventsToState()
{
	m_ptrSourceState->erase_all_events();
	EventsList_t::const_iterator ix = m_events_list.begin(), ex = m_events_list.end();
	for(;ix != ex; ++ix)
		m_ptrSourceState->add_event(*(ix->get()));
}

LPCTSTR InterfaceBtnStateProp::GetEventDescrip(qdInterfaceEvent const& event)
{
	return m_EventsRep[event.event()]->description();
}

LPCTSTR InterfaceBtnStateProp::GetDefVal(qdInterfaceEvent::event_t type){
	return m_EventsRep[type]->getDefaultValue();
}

void InterfaceBtnStateProp::LoadCorrespondigData(CWnd* pwnd, 
												 qdInterfaceEvent::event_t type)
{
	m_EventsRep[type]->loadData(pwnd);
}


bool InterfaceBtnStateProp::RemoveEventFromList(qdInterfaceEvent* pevent)
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

void InterfaceBtnStateProp::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
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

void InterfaceBtnStateProp::OnOK()
{
	CDialog::OnOK();
	EventsToState();
}

void InterfaceBtnStateProp::AdjustColumnsWidth()
{
	m_wndActions.SetColumnWidth(0,  LVSCW_AUTOSIZE_USEHEADER);
	m_wndActions.SetColumnWidth(1,  LVSCW_AUTOSIZE_USEHEADER);
	m_wndActions.SetColumnWidth(2,  LVSCW_AUTOSIZE_USEHEADER);
}


void InterfaceBtnStateProp::OnCancel()
{
	*m_ptrSourceState = *m_ptrState.get();
	set_modified(false);
	CDialog::OnCancel();
}
