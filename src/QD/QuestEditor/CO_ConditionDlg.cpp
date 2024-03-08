// CO_ConditionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "CO_ConditionDlg.h"

#include "conddlg_oper.h"

#include "qd_condition.h"
#include "condition.h"


#include "qd_conditional_object.h"

#include <boost/iterator_adaptors.hpp>
#include <boost/function_output_iterator.hpp>

//делает выпадающие список по ширине достаточным,
//чтобы уместить текст
VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);

bool CO_ShowCondDlg(qdGameDispatcher* ptrDisp, qdConditionalObject* pobj)
{
	if (!pobj)
		return false;

	CO_ConditionDlg dlg;
	dlg.set_data(ptrDisp, pobj);
	dlg.DoModal();
	return dlg.changed();
}

// CO_ConditionDlg dialog

IMPLEMENT_DYNAMIC(CO_ConditionDlg, CDialog)
CO_ConditionDlg::CO_ConditionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CO_ConditionDlg::IDD, pParent)
	, m_iCondLog(FALSE)
	, m_ptrGameDisp(NULL)
	, m_bChanged(false)
	, m_ptrSourceCO(NULL)
	, m_CondsMgr(member)
	, m_bAddConditionOnClose(false)
{
}

CO_ConditionDlg::~CO_ConditionDlg()
{
}

void CO_ConditionDlg::set_data(qdGameDispatcher* ptrDisp, qdConditionalObject* ptrObj){
	m_ptrGameDisp = ptrDisp;
	m_ptrSourceCO = ptrObj;
	ASSERT(m_ptrSourceCO);
	m_ptrSourceCO->backup_object();
	m_CondsMgr.set_obj(m_ptrSourceCO);
}

class qdGameScene* CO_ConditionDlg::get_scene() const{
	qdNamedObject* ptrNamed = dynamic_cast<qdNamedObject*>(m_ptrSourceCO);
	if (!ptrNamed)
		return NULL;
	qdNamedObject* owner = ptrNamed->owner();
	while (owner) {
		if (owner->named_object_type() == QD_NAMED_OBJECT_SCENE) 
			return static_cast<qdGameScene*>(owner);
		owner = owner->owner();
	}
	return NULL;
}

bool CO_ConditionDlg::CanAdd()
{
//	if (m_CDMgr.get_current()) 
//		return m_CDMgr.get_current()->CanAdd(m_ptrSourceCO);
	return true;
}

void CO_ConditionDlg::OnCondDataComplete(bool bComplete){
	bool const b = bComplete&&CanAdd()&&m_CDMgr.get_current();
	setAddConditionOnClose(b);
	GetDlgItem(IDC_BUTTON_ADD_COND)->EnableWindow(b);
}

void CO_ConditionDlg::ShowCondDlg(qdCondition::ConditionType type)
{
	if(m_CDMgr.get_current())
		m_CDMgr.get_current()->ShowWindow(SW_HIDE);

	m_CDMgr.set_current(type);
	if(m_CDMgr.get_current())
		m_CDMgr.get_current()->ShowWindow(SW_SHOW);
}

void CO_ConditionDlg::LoadConditions()
{
	/*
	for(int i = 0; i < g_iCondsSize; ++i)
	{
		int iPos = m_wndConds.AddString(g_arrConds[i].strName);
		if (iPos != -1) 
			m_wndConds.SetItemData(iPos, static_cast<DWORD>(g_arrConds[i].id));
	}*/
	fillConditionList(m_wndConds);
}

void CO_ConditionDlg::LoadCondMode()
{
	m_iCondLog = static_cast<int>(m_ptrSourceCO->conditions_mode());
	if(m_ptrSourceCO->conditions_mode() == qdConditionalObject::CONDITIONS_AND)
		m_iCondLog = 0;
	else if (m_ptrSourceCO->conditions_mode() == qdConditionalObject::CONDITIONS_OR) {
		m_iCondLog = 1;
	}
}


void CO_ConditionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CONDITION, m_wndConds);
	DDX_Radio(pDX, IDC_RADIO_LOG_CONN, m_iCondLog);
	DDX_Control(pDX, IDC_TREE, member);
}


BEGIN_MESSAGE_MAP(CO_ConditionDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_CONDITION, OnCbnSelchangeComboCondition)
	ON_BN_CLICKED(IDC_BUTTON_ADD_COND, OnBnClickedButtonAddCond)
	ON_LBN_SELCHANGE(IDC_LIST, OnLbnSelchangeList)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, OnNMRclickTree)
	ON_COMMAND(IDC_CREATE_OR, OnCreateOr)
	ON_COMMAND(IDC_CREATE_AND, OnCreateAnd)
	ON_COMMAND(IDC_CHANGE_TYPE, OnChangeType)
	ON_COMMAND(IDC_DELETE, OnDelete)
	ON_COMMAND(IDC_DELETE_ALL, OnDeleteAll)
	ON_NOTIFY(NM_MT_CAN_DROP_REQ, IDC_TREE, OnCanDropReq)
	ON_NOTIFY(NM_MT_DROP_REQ, IDC_TREE, OnDropReq)
	ON_NOTIFY(NM_MT_CAN_DRAG_REQ, IDC_TREE, OnCanDragReq)
	ON_COMMAND(IDC_REMOVE_FROM_GROUP, OnRemoveFromGroup)
	ON_COMMAND(IDC_CREATE_AND_FOR_ALL, OnCreateAndForAll)
	ON_COMMAND(IDC_CREATE_OR_FOR_ALL, OnCreateOrForAll)
END_MESSAGE_MAP()


// CO_ConditionDlg message handlers

BOOL CO_ConditionDlg::OnInitDialog()
{
	__super::OnInitDialog();

	CWnd* pWnd = GetDlgItem(IDC_STATIC_PLACE_HOLDER);
	CRect rcPlaceHolder;
	pWnd->GetWindowRect(rcPlaceHolder);
	ScreenToClient(rcPlaceHolder);
	m_CDMgr.init(this, this, rcPlaceHolder);

	SetButtonsState();
	LoadConditions();

	GetDlgItem(IDC_BUTTON_ADD_COND)->EnableWindow(FALSE);
	SetButtonsState();

	LoadGroups();
	ObjConditionsToTree();

	mg2wnd();

	UpdateData(FALSE);

	AdjustListWidth(&m_wndConds);

	CString cap;
	cap.Format(IDS_DLG_CAP_CO_COND, m_ptrSourceCO->name());
	SetWindowText(cap);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CO_ConditionDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_DELETE)	
	{
		if(pMsg->hwnd == member.m_hWnd)
			OnDelete();
	}

	return __super::PreTranslateMessage(pMsg);
}

void CO_ConditionDlg::OnOK()
{
	__super::OnOK();
	if(getAddConditionOnClose())
	{
		if (AfxMessageBox(IDS_ASK_ADD_CONDITION, MB_OKCANCEL) == IDOK) {
			OnBnClickedButtonAddCond();
		}
	}

	if (!changed()) 
		return;

	wnd2mg();
	TreeToCO();
}

void CO_ConditionDlg::OnCbnSelchangeComboCondition()
{
	int iCurSel = m_wndConds.GetCurSel();
	_ASSERT(iCurSel != -1);

	qdCondition::ConditionType type = 
		static_cast<qdCondition::ConditionType>(m_wndConds.GetItemData(iCurSel));
	ShowCondDlg(type);
}

void CO_ConditionDlg::OnBnClickedButtonAddCond()
{
	// Обновляем данные текущего выбранного диалога редактирования условия
	if (FALSE == m_CDMgr.get_current()->UpdateData(true))
		return;

	if (m_CondsMgr.AddCondition(&m_CDMgr.get_current()->get_condtion()))
		GetDlgItem(IDC_BUTTON_ADD_COND)->EnableWindow(CanAdd());
	setAddConditionOnClose(false);
}

//загружает в состояние данные, которые не относятся к условиям
VOID CO_ConditionDlg::wnd2mg()
{
	if (!changed()){
		if (m_iCondLog == 0) 
			m_bChanged = (qdConditionalObject::CONDITIONS_AND 
							!= m_ptrSourceCO->conditions_mode());
		else if (m_iCondLog == 1) 
			m_bChanged = (qdConditionalObject::CONDITIONS_OR 
							!= m_ptrSourceCO->conditions_mode());
	}
	if (m_iCondLog == 0) {
		m_ptrSourceCO->set_conditions_mode(qdConditionalObject::CONDITIONS_AND);
	}else if (m_iCondLog == 1) {
		m_ptrSourceCO->set_conditions_mode(qdConditionalObject::CONDITIONS_OR);
	}
}

//загружает из состояний данные, которые не относятся к условиям
VOID CO_ConditionDlg::mg2wnd()
{
	LoadCondMode();
}

void CO_ConditionDlg::SetButtonsState()
{
	if (m_CDMgr.get_current())
		OnCondDataComplete(m_CDMgr.get_current()->IsCondDataComplete());
}	

void CO_ConditionDlg::OnLbnSelchangeList()
{
	SetButtonsState();
}

void CO_ConditionDlg::OnCancel()
{
	if (changed()) 
		m_ptrSourceCO->restore_object();
	__super::OnCancel();
}

bool CO_ConditionDlg::LoadGroups()
{
	return m_CondsMgr.LoadGroups();
}

void CO_ConditionDlg::OnNMRclickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = m_CondsMgr.OnNMRclickTree(this);
}

void CO_ConditionDlg::OnCreateOr()
{
	m_CondsMgr.CreateGroup(qdConditionGroup::CONDITIONS_OR);
}

void CO_ConditionDlg::OnCreateAnd()
{
	m_CondsMgr.CreateGroup(qdConditionGroup::CONDITIONS_AND);
}

void CO_ConditionDlg::OnChangeType()
{
	m_CondsMgr.InvertType();
}

void CO_ConditionDlg::ObjConditionsToTree()
{
	m_CondsMgr.CondsToTree();
}

void CO_ConditionDlg::OnDelete()
{
	m_CondsMgr.DeleteSelected();
}

void CO_ConditionDlg::OnCanDropReq(NMHDR* phdr, LRESULT* pResult)
{
	*pResult = m_CondsMgr.OnCanDropReq(reinterpret_cast<NM_MT_CAN_DROP*>(phdr));
}

void CO_ConditionDlg::OnCanDragReq(NMHDR* phdr, LRESULT* pResult)
{
	*pResult = m_CondsMgr.OnCanDragReq(reinterpret_cast<NM_MT_CAN_DRAG*>(phdr));
}

void CO_ConditionDlg::OnDropReq(NMHDR* phdr, LRESULT* pResult)
{
	*pResult = m_CondsMgr.OnDropReq(reinterpret_cast<NM_MT_DROP*>(phdr));
}

void CO_ConditionDlg::TreeToCO()
{
	if (m_CondsMgr.was_changed())
		m_CondsMgr.TreeToCO();
}

void CO_ConditionDlg::OnRemoveFromGroup()
{
	m_CondsMgr.RemoveFromGroup();
}

void CO_ConditionDlg::OnDeleteAll(){
	m_CondsMgr.DeleteAll();
}

void CO_ConditionDlg::OnCreateAndForAll()
{
	m_CondsMgr.CreateGroups(qdConditionGroup::CONDITIONS_AND);
}

void CO_ConditionDlg::OnCreateOrForAll()
{
	m_CondsMgr.CreateGroups(qdConditionGroup::CONDITIONS_OR);
}
