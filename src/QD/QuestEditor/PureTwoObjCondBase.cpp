// PureTwoObjCondBase.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "PureTwoObjCondBase.h"
#include "CondUsuals.h"
#include ".\puretwoobjcondbase.h"


// PureTwoObjCondBase dialog

// Объявление функции, чтобы линкер искал реализацию функции
void AdjustListWidth(CCJFlatComboBox* pmyComboBox);

IMPLEMENT_DYNAMIC(PureTwoObjCondBase, CDialog)
PureTwoObjCondBase::PureTwoObjCondBase(DWORD id, CWnd* pParent /*=NULL*/)
	: CondDlgBase(PureTwoObjCondBase::IDD, id, pParent)
{
}

PureTwoObjCondBase::~PureTwoObjCondBase()
{
}

bool PureTwoObjCondBase::IsCondDataComplete()
{
	const qdGameObject* obj1 = reinterpret_cast<const qdGameObject*>(
		m_qdCondition.get_object(0));
	const qdGameObject* obj2 = reinterpret_cast<const qdGameObject*>(
		m_qdCondition.get_object(1));
	if ((NULL == obj1) || (NULL == obj2) || !ValidObjects(obj1, obj2))
		return false;
	else 
		return true;
}

void PureTwoObjCondBase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SCENES1, m_cbScenes1);
	DDX_Control(pDX, IDC_COMBO_SCENES2, m_cbScenes2);
	DDX_Control(pDX, IDC_COMBO_OBJECTS1, m_cbObjects1);
	DDX_Control(pDX, IDC_COMBO_OBJECTS2, m_cbObjects2);
	DDX_Control(pDX, IDC_STATIC_OBJECTS1, m_sStaticObject1);
	DDX_Control(pDX, IDC_STATIC_OBJECTS2, m_sStaticObject2);

	// Make m_qdCondition
	if (TRUE == pDX->m_bSaveAndValidate)
	{
		const qdGameObject* obj1 = reinterpret_cast<const qdGameObject*>(
			CondUsuals::GetSelPtr(m_cbObjects1));
		const qdGameObject* obj2 = reinterpret_cast<const qdGameObject*>(
			CondUsuals::GetSelPtr(m_cbObjects2));

		if ((NULL != obj1) && (NULL != obj2) && ValidObjects(obj1, obj2))
		{
			m_qdCondition.put_object(0, reinterpret_cast<const qdNamedObject*>(obj1));
			m_qdCondition.put_object(1, reinterpret_cast<const qdNamedObject*>(obj2));
			m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
		}
		else
		{
			m_ptrMakeCondDlg->OnCondDataComplete(false);
			pDX->Fail();
		}
	}
}


BEGIN_MESSAGE_MAP(PureTwoObjCondBase, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES1, OnCbnSelchangeComboScenes1)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES2, OnCbnSelchangeComboScenes2)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECTS1, OnCbnSelchangeComboObjects1)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECTS2, OnCbnSelchangeComboObjects2)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// PureTwoObjCondBase message handlers

void PureTwoObjCondBase::OnCbnSelchangeComboScenes1()
{
	if (NULL != CondUsuals::GetSelPtr(m_cbScenes1))
	{
		LoadToObjCombo1(reinterpret_cast<const qdGameScene*>(CondUsuals::GetSelPtr(m_cbScenes1)));
		m_cbObjects1.SetCurSel(0);
	}

	UpdateData(true);
}

void PureTwoObjCondBase::OnCbnSelchangeComboScenes2()
{
	if (NULL != CondUsuals::GetSelPtr(m_cbScenes2))
	{
		LoadToObjCombo2(reinterpret_cast<const qdGameScene*>(CondUsuals::GetSelPtr(m_cbScenes2)));
		m_cbObjects2.SetCurSel(0);
	}

	UpdateData(true);
}

void PureTwoObjCondBase::OnCbnSelchangeComboObjects1()
{
	UpdateData(true);
}

void PureTwoObjCondBase::OnCbnSelchangeComboObjects2()
{
	UpdateData(true);
}

BOOL PureTwoObjCondBase::OnInitDialog()
{
	CDialog::OnInitDialog();

	qdGameScene * ptrScene = m_ptrMakeCondDlg->get_scene();

	// Первый листбокс сцен
	CondDlgBase::LoadScenes(m_cbScenes1, *GetDlgItem(IDC_STATIC_SCENES1));		
	if (!ptrScene||(m_cbScenes1.SelectString(-1, ptrScene->name()) == CB_ERR))
		m_cbScenes1.SetCurSel(0);
	m_cbScenes1.SetDroppedWidth(DROPPED_WIDTH_AUTO_FIT);
	AdjustListWidth(&m_cbScenes1);

	// Второй листбокс сцен
	CondDlgBase::LoadScenes(m_cbScenes2, *GetDlgItem(IDC_STATIC_SCENES2));
	if (!ptrScene||(m_cbScenes2.SelectString(-1, ptrScene->name()) == CB_ERR))
		m_cbScenes2.SetCurSel(0);
	m_cbScenes1.SetDroppedWidth(DROPPED_WIDTH_AUTO_FIT);
	AdjustListWidth(&m_cbScenes2);

	// Сцены загружены в оба комбобокса, грузим объекты
	OnCbnSelchangeComboScenes1();
	OnCbnSelchangeComboScenes2();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void PureTwoObjCondBase::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CondDlgBase::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		UpdateData(true); // Грузим данные из элементов управления диалога
		m_ptrMakeCondDlg->OnCondDataComplete(false);		
	}
}
