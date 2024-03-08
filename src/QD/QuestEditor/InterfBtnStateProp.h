#pragma once
#include "afxwin.h"
#include <memory>
#include "qd_interface_element_state.h"
#include "editlistctrl.h"
#include <boost/shared_ptr.hpp>
#include "interf_events.h"

// InterfBtnStateProp dialog

class InterfBtnStateProp : public CDialog
{
	DECLARE_DYNAMIC(InterfBtnStateProp)

	typedef boost::shared_ptr<qdInterfaceEvent> qdInterfaceEventPtr;
	typedef std::vector<qdInterfaceEventPtr> EventsList_t;
public:
	InterfBtnStateProp();
	virtual ~InterfBtnStateProp();

	void set_data(qdInterfaceElementState* pstate);
	void set_start_mode(qdInterfaceElementState::state_mode_t const m);
	bool get_modified();

// Dialog Data
	enum { IDD = IDD_INTRF_ELE_STATE_PROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnCbnSelchangeComboModes();
	afx_msg void OnBnClickedBtnSelAnim();
	afx_msg void OnBnClickedBtnSelSound();

	afx_msg void OnBnClickedCheckHorzInv();
	afx_msg void OnBnClickedCheckVertInv();
	afx_msg void OnBnClickedCheckLoop();
	afx_msg void OnBnClickedCheckSoundLoop();
	afx_msg void OnEnKillfocusEditSoundFile();
	afx_msg void OnEnKillfocusEditAnimFile();
	afx_msg void OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMEdtCtrlReq(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMEndEdit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetupEdit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMEditReq(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnAddEvent();
	afx_msg void OnRemoveEvent();

	void state_mode2wnd(qdInterfaceElementState::state_mode_t const m);
	void wnd2state_mode(qdInterfaceElementState::state_mode_t const m);
	void LoadModes();
	void SetModified();
	bool SetMode(int mode);
	qdInterfaceElementState::state_mode_t GetCurMode();

	void AddListColumns();
	void LoadEvents();
	qdInterfaceEvent* AddEvent(qdInterfaceEvent::event_t event);
	bool GetHasData(qdInterfaceEvent::event_t event);

	void EventsToState();
	void LoadEventsToList();
	LPCTSTR GetEventDescrip(qdInterfaceEvent const& event);
	void LoadCorrespondigData(CWnd* pwnd, qdInterfaceEvent::event_t type);

	LPCTSTR GetDefVal(qdInterfaceEvent::event_t type);
	bool RemoveEventFromList(qdInterfaceEvent* pevent);
	void AdjustColumnsWidth();
	//! В зависимости от режима назначает заголовок окна
	void setDlgCaption(qdInterfaceElementState::state_mode_t const md);
private:
	class NamedObjToCombo
	{
	public:
		NamedObjToCombo(CCJFlatComboBox& box):m_box(box){};
		void operator()(qdNamedObjectBase const* pobj){
			m_box.AddString(pobj->name());
		}
	private:
		CCJFlatComboBox& m_box;
	};
private:
	//! Режимы состояния
	CCJFlatComboBox m_wndModes;

	//! состояние, к которому применяются все изменения
	std::auto_ptr<qdInterfaceElementState> m_ptrState;
	//! исходное состояния. измениния переносятся в него после нажатия Ok or Apply
	qdInterfaceElementState* m_ptrSourceState;
	//! Файл анимации
	CString m_strAnimFileName;
	//! Звуквой файл
	CString m_strSoundFile;
	//! Была ли модификация
	bool m_bModified;
	//! Был ли изменен режим
	bool m_bModeModified;
	//! список действий
	CEditListCtrl m_wndActions;
	//! собственный список действий
	EventsList_t m_events_list;
	InterfEventRep m_EventsRep;

	//! Мод, который будет отображаться при запуске диалога
	qdInterfaceElementState::state_mode_t start_mode_;
};
