// AnimObjIdleCondObj.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "AnimObjIdleCondDlg.h"
#include "CondUsuals.h"
#include ".\animobjidleconddlg.h"


using namespace CondUsuals;

// AnimObjIdleCondObj dialog

// ќбъ€вление функции, чтобы линкер искал реализацию функции
void AdjustListWidth(CCJFlatComboBox* pmyComboBox);

IMPLEMENT_DYNAMIC(AnimObjIdleCondDlg, CDialog)
AnimObjIdleCondDlg::AnimObjIdleCondDlg(DWORD id, CWnd* pParent /*=NULL*/)
	: CondDlgBase(AnimObjIdleCondDlg::IDD, id, pParent)
	, m_iValue(0)
{
}

AnimObjIdleCondDlg::~AnimObjIdleCondDlg()
{
}

void AnimObjIdleCondDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SCENES, m_cbScenes);
	DDX_Control(pDX, IDC_COMBO_OBJECT, m_cbObjects);
	DDX_Text(pDX, IDC_EDIT, m_iValue);
	DDV_MinMaxInt(pDX, m_iValue, 0, INT_MAX);
	
	// My SaveAndValidation (заполн€ю m_qdCondition)
	if ((TRUE == pDX->m_bSaveAndValidate))
	{
		// Object
		if (NULL != GetSelPtr(m_cbObjects))
			m_qdCondition.put_object(0, 
			  reinterpret_cast<const qdNamedObject*>(GetSelPtr(m_cbObjects)));
		else
		{
			pDX->Fail();
			return;
		}
		// Idle time
		m_qdCondition.put_value(0, m_iValue);
	}
}


BEGIN_MESSAGE_MAP(AnimObjIdleCondDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT, OnCbnSelchangeComboObject)
	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_EDIT, OnEnChangeEdit)
END_MESSAGE_MAP()


// AnimObjIdleCondObj message handlers

BOOL AnimObjIdleCondDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_VALUE))->
				SetRange32(0, INT_MAX);

	CondDlgBase::LoadScenes(m_cbScenes, *GetDlgItem(IDC_STATIC_SCENES), SH_NONE);
	qdGameScene * ptrScene = m_ptrMakeCondDlg->get_scene();
	if (!ptrScene||(m_cbScenes.SelectString(-1, ptrScene->name()) == CB_ERR))
		m_cbScenes.SetCurSel(0);
	m_cbScenes.SetDroppedWidth(DROPPED_WIDTH_AUTO_FIT);
	AdjustListWidth(&m_cbScenes);

	if (NULL != GetSelPtr(m_cbScenes))
	{
		LoadAnimObjects(m_cbObjects, 
			reinterpret_cast<const qdGameScene*>(GetSelPtr(m_cbScenes)));
		m_cbObjects.SetCurSel(0);

		OnCbnSelchangeComboObject();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void AnimObjIdleCondDlg::OnCbnSelchangeComboScenes()
{
	if (NULL != GetSelPtr(m_cbScenes))
	{
		LoadAnimObjects(m_cbObjects, 
		  reinterpret_cast<const qdGameScene*>(GetSelPtr(m_cbScenes)));
		m_cbObjects.SetCurSel(0);
	}
	OnCbnSelchangeComboObject();
}

void AnimObjIdleCondDlg::OnCbnSelchangeComboObject()
{
	if (NULL != GetSelPtr(m_cbObjects))
		m_qdCondition.put_object(0, 
		  reinterpret_cast<const qdNamedObject*>(GetSelPtr(m_cbObjects)));
	else
		m_qdCondition.put_object(0, NULL); // ≈сли не удалось выбрать объект, то он NULL
	
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void AnimObjIdleCondDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CondDlgBase::OnShowWindow(bShow, nStatus);

	if (bShow)
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void AnimObjIdleCondDlg::OnEnChangeEdit()
{
	m_qdCondition.put_value(0, m_iValue);
}

bool AnimObjIdleCondDlg::IsCondDataComplete()
{
	if (NULL != m_qdCondition.get_object(0))
		return true;
	else return false;
}
