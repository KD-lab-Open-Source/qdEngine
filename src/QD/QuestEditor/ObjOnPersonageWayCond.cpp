// ObjOnPersonageWayCond.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "ObjOnPersonageWayCond.h"
#include <boost/function_output_iterator.hpp>
#include "obj_type_detectors.h"
#include "qd_game_object.h"

// ObjOnPersonageWayCond dialog

template<class Filter>
ObjOnPersonageWayCond::ObjTypeToCombo<Filter>::ObjTypeToCombo(CCJFlatComboBox& box, Filter const& f)
:m_box(box)
, m_filter(f)
{

}
template<class Filter>
void ObjOnPersonageWayCond::ObjTypeToCombo<Filter>::operator()(qdNamedObject* pobj)
{
	if (m_filter(pobj))
	{
		int const iItem = m_box.AddString(pobj->name());
		if (iItem != CB_ERR)
			m_box.SetItemDataPtr(iItem, pobj);
	}
}

IMPLEMENT_DYNAMIC(ObjOnPersonageWayCond, CondDlgBase)
ObjOnPersonageWayCond::ObjOnPersonageWayCond(DWORD condId, CWnd* pParent /*=NULL*/)
	: CondDlgBase(ObjOnPersonageWayCond::IDD, condId, pParent)
	, m_pPrevPers(NULL)
	, m_fMaxDist(0.f)
{
}

ObjOnPersonageWayCond::~ObjOnPersonageWayCond()
{
}

void ObjOnPersonageWayCond::DoDataExchange(CDataExchange* pDX)
{
	CondDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SCENES, m_wndScenes);
	DDX_Control(pDX, IDC_COMBO_OBJECT, m_wndPers);
	DDX_Control(pDX, IDC_COMBO_OBJECT2, m_wndObjs);
	DDX_Text(pDX, IDC_EDIT_MAX_DIST, m_fMaxDist);
}


BEGIN_MESSAGE_MAP(ObjOnPersonageWayCond, CondDlgBase)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT, OnCbnSelchangeComboObject)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT2, OnCbnSelchangeComboObject2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAX_DIST, OnDeltaposSpinMaxDist)
END_MESSAGE_MAP()


template<class ObjType>
ObjType* ObjOnPersonageWayCond::getObjFromCombo(CCJFlatComboBox& box)
{
	int const iCurSel = box.GetCurSel();
	if (iCurSel == CB_ERR)
		return NULL;
	return reinterpret_cast<ObjType*>(box.GetItemDataPtr(iCurSel));
}
// ObjOnPersonageWayCond message handlers
qdGameScene* ObjOnPersonageWayCond::getScene()
{
	return getObjFromCombo<qdGameScene>(m_wndScenes);
}

qdNamedObject* ObjOnPersonageWayCond::getPersonage()
{
	return getObjFromCombo<qdNamedObject>(m_wndPers);
}

qdNamedObject* ObjOnPersonageWayCond::getObject()
{
	return getObjFromCombo<qdNamedObject>(m_wndObjs);
}

void ObjOnPersonageWayCond::setPrevPersonage(qdNamedObject* p){
	m_pPrevPers = p;
}

void ObjOnPersonageWayCond::OnCbnSelchangeComboScenes()
{
	m_wndPers.ResetContent();
	//зачищаем здесь, потому что при смене персонажа не провдится
	//полная перегрузка объектов
	m_wndObjs.ResetContent();

	setPrevPersonage(NULL);
	LoadPersonages();
	LoadObjects();
	m_wndPers.SetCurSel(0);
	OnCbnSelchangeComboObject();
	m_wndObjs.SetCurSel(0);
	OnCbnSelchangeComboObject2();
}

void ObjOnPersonageWayCond::OnCbnSelchangeComboObject()
{
	qdNamedObject * const ppers = getPersonage();
	m_qdCondition.put_object(0, ppers);
	m_wndObjs.EnableWindow(ppers != NULL);
	if (!ppers)
	{
		m_wndObjs.ResetContent();
		m_qdCondition.put_object(0, NULL);
		setPrevPersonage(ppers);
		return;
	}

	int iItem = m_wndObjs.FindString(-1, ppers->name());
	if (iItem != CB_ERR)
		m_wndObjs.DeleteString(iItem);
	if (m_pPrevPers)
	{
		int const iItem = m_wndObjs.AddString(m_pPrevPers->name());
		if (iItem != CB_ERR)
			m_wndObjs.SetItemDataPtr(iItem, m_pPrevPers);
	}
	OnCbnSelchangeComboObject2();
	setPrevPersonage(ppers);
}

void ObjOnPersonageWayCond::OnCbnSelchangeComboObject2()
{
	m_qdCondition.put_object(1, getObject());
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void ObjOnPersonageWayCond::LoadObjects()
{
	qdGameScene* const pscene = getScene();
	ASSERT(pscene);
	qdGameObjectList const& lst = pscene->object_list();
	typedef bool (*Fun)(qdNamedObject const*);
	std::copy(lst.begin(), lst.end(), 
		boost::make_function_output_iterator(
		ObjTypeToCombo<Fun>(m_wndObjs,
		&ptree_helpers::is_animated)));
}

void ObjOnPersonageWayCond::LoadPersonages()
{
	qdGameScene* const pscene = getScene();
	ASSERT(pscene);
	qdGameObjectList const& lst = pscene->object_list();
/*	std::for_each(
		boost::make_filter_iterator(
			pscene->object_list().begin(),
			pscene->object_list().end(),
			std::ptr_fun(ptree_helpers::is_obj_moving)
			),
			boost::make_filter_iterator(
			pscene->object_list().end(),
			pscene->object_list().end(),
			std::ptr_fun(ptree_helpers::is_obj_moving)
			),
			std::bind1st(std::mem_fun(ObjOnPersonageWayCond::PersToCombo), this)
			);
*/
	typedef bool (*Fun)(qdNamedObject const*);
	std::copy(lst.begin(), lst.end(), 
		boost::make_function_output_iterator(
			ObjTypeToCombo<Fun>(m_wndPers,
			&ptree_helpers::is_obj_moving)));
}

void ObjOnPersonageWayCond::LoadScenes()
{
	CondDlgBase::LoadScenes(m_wndScenes,*GetDlgItem(IDC_STATIC_SCENE));
	m_wndScenes.SetCurSel(0);
}

bool ObjOnPersonageWayCond::IsCondDataComplete()
{
	if (!m_qdCondition.get_object(0)||!m_qdCondition.get_object(1))
		return false;
	float fMaxDist = 0.f;
	m_qdCondition.get_value(0,fMaxDist, 0);
	if (fMaxDist == 0.f)
		return false;
	return true;
}

void ObjOnPersonageWayCond::OnDeltaposSpinMaxDist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		return;
	}
	m_fMaxDist += pNMUpDown->iDelta / 100.f;
	if (m_fMaxDist < 0.f)
		m_fMaxDist = 0.f;
	m_qdCondition.put_value(0, m_fMaxDist, 0);
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
	UpdateData(FALSE);
	*pResult = 0;
}

BOOL ObjOnPersonageWayCond::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (GetFocus() == GetDlgItem(IDC_EDIT_MAX_DIST))
		{
			UpdateData();
			m_qdCondition.put_value(0, m_fMaxDist, 0);
			m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
			return TRUE;
		}
	}
	return CondDlgBase::PreTranslateMessage(pMsg);
}

BOOL ObjOnPersonageWayCond::OnInitDialog()
{
	CondDlgBase::OnInitDialog();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_MAX_DIST))->SetRange(1, 100);
	LoadScenes();
	m_wndScenes.SetCurSel(0);
	OnCbnSelchangeComboScenes();
	return TRUE;
}
