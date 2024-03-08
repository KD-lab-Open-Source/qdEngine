#pragma once

#include <memory>
#include "qd_interface_element_state.h"
#include "editlistctrl.h"
#include <boost/shared_ptr.hpp>
#include "interf_events.h"

// InterfaceBtnStateProp dialog

class InterfaceBtnStateProp : public CDialog
{
	DECLARE_DYNAMIC(InterfaceBtnStateProp)

	typedef boost::shared_ptr<qdInterfaceEvent> qdInterfaceEventPtr;
	typedef std::vector<qdInterfaceEventPtr> EventsList_t;
public:
	InterfaceBtnStateProp(CWnd* pParent = NULL);   // standard constructor
	virtual ~InterfaceBtnStateProp();

// Dialog Data
	enum { IDD = IDD_INTRF_BTN_STATE };

	//! ������������� ���������, ������� ����� �������������
	void set_data(qdInterfaceElementState* pstate);
	//! ��������� �� �����������
	bool get_modified();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP();
	//! ��������� ������� � ������
	void AddListColumns();
	//! ��������� ������ �������
	void LoadEvents();
	qdInterfaceEvent* AddEvent(qdInterfaceEvent::event_t event);
	bool GetHasData(qdInterfaceEvent::event_t event);

	//! ���������� � ��������� ��� �������� ������
	void EventsToState();
	//! ��������� �� ��������� ������ � ������
	void LoadEventsToList();
	//! �� ������ ���������� ��� ��������
	LPCTSTR GetEventDescrip(qdInterfaceEvent const& event);
	//! ��������� � ������� ������ ��������������� ������
	void LoadCorrespondigData(CWnd* pwnd, qdInterfaceEvent::event_t type);
	//! ��������� �������� �� ���������
	LPCTSTR GetDefVal(qdInterfaceEvent::event_t type);
	//! ������� ����� �� ����������� ������ 
	bool RemoveEventFromList(qdInterfaceEvent* pevent);
	//! ����������� ������ �������� � ������
	void AdjustColumnsWidth();

	afx_msg void OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMEdtCtrlReq(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMEndEdit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetupEdit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMEditReq(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnAddEvent();
	afx_msg void OnRemoveEvent();

	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();

	//! ������������� ���� �����������
	void set_modified(bool flag);
private:
	//! ������ ��������
	CEditListCtrl m_wndActions;
	//! ����������� ������ ��������
	EventsList_t m_events_list;
	InterfEventRep m_EventsRep;
	//! ���������, � �������� ����������� ��� ���������
	std::auto_ptr<qdInterfaceElementState> m_ptrState;
	//! �������� ���������. ��������� ����������� � ���� ����� ������� Ok or Apply
	qdInterfaceElementState* m_ptrSourceState;
	bool modified_;
};
