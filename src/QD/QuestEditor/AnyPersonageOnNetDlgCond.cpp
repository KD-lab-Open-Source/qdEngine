// AnyPersonageOnNetDlgCond.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "AnyPersonageOnNetDlgCond.h"
#include "qd_grid_zone.h"


void AdjustListWidth(CCJFlatComboBox* ptrCombo);

IMPLEMENT_DYNAMIC(AnyPersonageOnNetDlgCond, CondDlgBase)
AnyPersonageOnNetDlgCond::AnyPersonageOnNetDlgCond(DWORD condId, CWnd* pParent /*=NULL*/)
	: CondDlgBase(AnyPersonageOnNetDlgCond::IDD, condId, pParent)
{
}

AnyPersonageOnNetDlgCond::~AnyPersonageOnNetDlgCond()
{
}

void AnyPersonageOnNetDlgCond::DoDataExchange(CDataExchange* pDX)
{
	CondDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ZONE, m_wndZones);
	DDX_Control(pDX, IDC_COMBO_SCENES, m_wndScenes);
}


BEGIN_MESSAGE_MAP(AnyPersonageOnNetDlgCond, CondDlgBase)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
	ON_CBN_SELCHANGE(IDC_COMBO_ZONE, OnCbnSelchangeComboZone)
END_MESSAGE_MAP()

BOOL AnyPersonageOnNetDlgCond::OnInitDialog()
{
	BOOL res = CondDlgBase::OnInitDialog();
	if (LoadScenes(m_wndScenes, *GetDlgItem(IDC_STATIC_SCENE)))
	{
		AdjustListWidth(&m_wndScenes);
		if (qdGameScene* const pscene = get_scene()) 
			m_wndScenes.SelectString(-1, pscene->name());
		else
			m_wndScenes.SetCurSel(0);
		OnCbnSelchangeComboScenes();
	}
	else
		m_wndZones.EnableWindow(FALSE);
	return TRUE;
}
// AnyPersonageOnNetDlgCond message handlers

void AnyPersonageOnNetDlgCond::OnCbnSelchangeComboScenes()
{
	if (LoadZones(get_scene()))
	{
		m_wndZones.SetCurSel(0);
		OnCbnSelchangeComboZone();
	}
}

void AnyPersonageOnNetDlgCond::OnCbnSelchangeComboZone()
{
	int const iCurSel = m_wndZones.GetCurSel();
	if (iCurSel != CB_ERR)
	{
		qdGridZone* const pzone = 
			reinterpret_cast<qdGridZone*>(m_wndZones.GetItemDataPtr(iCurSel));
		m_qdCondition.put_object(0, pzone);
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
	}
	else
		m_qdCondition.put_object(0, NULL);
}

bool AnyPersonageOnNetDlgCond::LoadZones(qdGameScene* pscene)
{
	if (!pscene)
		return false;
	qdGridZoneList const& lst = pscene->grid_zone_list();
	m_wndZones.ResetContent();
	m_wndZones.EnableWindow(!lst.empty());
	if (lst.empty())
	{
		CString st;
		st.LoadString(IDS_NOTHING);
		m_wndZones.AddString(st);
	}
	else{
		qdGridZoneList::const_iterator i = lst.begin(), e = lst.end();
		for(; i != e; ++i)
		{
			int const iItem = m_wndZones.AddString((*i)->name());
			if (iItem != CB_ERR)
				m_wndZones.SetItemDataPtr(iItem, *i);
		}
		AdjustListWidth(&m_wndZones);
	}
	return true;
}

bool AnyPersonageOnNetDlgCond::IsCondDataComplete()
{
	return (m_qdCondition.get_object(0) != NULL); 
}

qdGameScene* AnyPersonageOnNetDlgCond::get_scene(){
	int const iCurSel = m_wndScenes.GetCurSel();
	if (iCurSel == CB_ERR)
		return NULL;
	return reinterpret_cast<qdGameScene*>(m_wndScenes.GetItemDataPtr(iCurSel));
}