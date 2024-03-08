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

	//! Устанавливает состояние, которое будем редактировать
	void set_data(qdInterfaceElementState* pstate);
	//! Произошла ли модификация
	bool get_modified();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP();
	//! Добавляет столбцы к список
	void AddListColumns();
	//! Загружает список эвентов
	void LoadEvents();
	qdInterfaceEvent* AddEvent(qdInterfaceEvent::event_t event);
	bool GetHasData(qdInterfaceEvent::event_t event);

	//! Записывает в состояние все заданные эвенты
	void EventsToState();
	//! Загружает из состояния эвенты в список
	void LoadEventsToList();
	//! По эвенту возвращает его описание
	LPCTSTR GetEventDescrip(qdInterfaceEvent const& event);
	//! Загружает в контрол данные соответствующие эвенту
	void LoadCorrespondigData(CWnd* pwnd, qdInterfaceEvent::event_t type);
	//! Возврщает значение по умолчанию
	LPCTSTR GetDefVal(qdInterfaceEvent::event_t type);
	//! Удаляет эвент из собтвенного списка 
	bool RemoveEventFromList(qdInterfaceEvent* pevent);
	//! Выравнивает ширину столбцов в списке
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

	//! Устанавливает флаг модификации
	void set_modified(bool flag);
private:
	//! список действий
	CEditListCtrl m_wndActions;
	//! собственный список действий
	EventsList_t m_events_list;
	InterfEventRep m_EventsRep;
	//! состояние, к которому применяются все изменения
	std::auto_ptr<qdInterfaceElementState> m_ptrState;
	//! исходное состояния. измениния переносятся в него после нажатия Ok or Apply
	qdInterfaceElementState* m_ptrSourceState;
	bool modified_;
};
