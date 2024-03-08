// ObjectIsCloserCondDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "ObjectIsCloserCondDlg.h"
#include ".\objectiscloserconddlg.h"

#include "qd_game_object.h"
#include "ptree_hlprs.hpp"
#include <boost/rtl.hpp>

// ObjectIsCloserCondDlg dialog

IMPLEMENT_DYNAMIC(ObjectIsCloserCondDlg, CondDlgBase)
ObjectIsCloserCondDlg::ObjectIsCloserCondDlg(DWORD id)
	: CondDlgBase(ObjectIsCloserCondDlg::IDD, id, NULL)
{
}

ObjectIsCloserCondDlg::~ObjectIsCloserCondDlg()
{
}

void ObjectIsCloserCondDlg::DoDataExchange(CDataExchange* pDX)
{
	CondDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SCENES, scenes_);
	DDX_Control(pDX, IDC_COMBO_OBJECT, firstObjects_);
	DDX_Control(pDX, IDC_COMBO_OBJECT2, candidateObjects1_);
	DDX_Control(pDX, IDC_COMBO_OBJECT3, candidateObjects2_);
}


BEGIN_MESSAGE_MAP(ObjectIsCloserCondDlg, CondDlgBase)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT, OnCbnSelchangeComboObject)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT2, OnCbnSelchangeComboObject2)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT3, OnCbnSelchangeComboObject3)
END_MESSAGE_MAP()

bool ObjectIsCloserCondDlg::IsCondDataComplete()
{
	return (m_qdCondition.get_object(qdCondition::OBJECT_NAME)
		&&m_qdCondition.get_object(qdCondition::OBJECT2_NAME)
		&&m_qdCondition.get_object(2));
}

// ObjectIsCloserCondDlg message handlers
void ObjectIsCloserCondDlg::OnCbnSelchangeComboScenes()
{
	loadObjects(firstObjects_, NULL, NULL);
	loadObjects(candidateObjects1_, NULL, NULL);
	loadObjects(candidateObjects2_, NULL, NULL);
	OnCbnSelchangeComboObject();
	OnCbnSelchangeComboObject2();
	OnCbnSelchangeComboObject3();
}

void ObjectIsCloserCondDlg::OnCbnSelchangeComboObject()
{
	int iSel = firstObjects_.GetCurSel();
	if(iSel == -1)
		return;
	void* data = firstObjects_.GetItemDataPtr(iSel);
	if(data)
	{
		qdNamedObject* const p = reinterpret_cast<qdNamedObject*>(data);
		m_qdCondition.put_object(qdCondition::OBJECT_NAME, p);
	}
	else
		m_qdCondition.put_object(qdCondition::OBJECT_NAME, NULL);

	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void ObjectIsCloserCondDlg::OnCbnSelchangeComboObject2()
{
	int iSel = candidateObjects1_.GetCurSel();
	if(iSel == -1)
		return;
	void* data = candidateObjects2_.GetItemDataPtr(iSel);
	if(data)
	{
		qdNamedObject* const p = reinterpret_cast<qdNamedObject*>(data);
		m_qdCondition.put_object(qdCondition::OBJECT2_NAME, p);
	}
	else
		m_qdCondition.put_object(qdCondition::OBJECT2_NAME, NULL);

	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void ObjectIsCloserCondDlg::OnCbnSelchangeComboObject3()
{
	int iSel = candidateObjects2_.GetCurSel();
	if(iSel == -1)
		return;
	void* data = candidateObjects1_.GetItemDataPtr(iSel);
	if(data)
	{
		qdNamedObject* const p = reinterpret_cast<qdNamedObject*>(data);
		m_qdCondition.put_object(2, p);
	}
	else
		m_qdCondition.put_object(2, NULL);

	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

BOOL ObjectIsCloserCondDlg::OnInitDialog()
{
	CondDlgBase::OnInitDialog();

	LoadScenes(scenes_, *GetDlgItem(IDC_STATIC_SCENE), SH_SHOWHIDE);

	loadObjects(firstObjects_, NULL, NULL);
	loadObjects(candidateObjects1_, NULL, NULL);
	loadObjects(candidateObjects2_, NULL, NULL);

	OnCbnSelchangeComboObject();
	OnCbnSelchangeComboObject2();
	OnCbnSelchangeComboObject3();

	return TRUE;  
}

class qdGameScene* ObjectIsCloserCondDlg::get_scene(){
	if (scenes_.GetCurSel() == CB_ERR)
		return NULL;
	return static_cast<qdGameScene*>(scenes_.GetItemDataPtr(scenes_.GetCurSel()));
}

class qdNamedObject* ObjectIsCloserCondDlg::get_obj(CCJFlatComboBox& combo){
	if (combo.GetCurSel() == CB_ERR)
		return NULL;
	return static_cast<qdGameScene*>(combo.GetItemDataPtr(combo.GetCurSel()));
}

void ObjectIsCloserCondDlg::loadObjects(CCJFlatComboBox& combo, 
				 qdNamedObject const* excl, 
				 qdNamedObject const* excl2)
{
	combo.ResetContent();
	qdGameScene* scene = get_scene();
	boost::rtl::filtered_range<
		qdGameObjectList const,
		bool (*)(qdNamedObject const*)
	> r(scene->object_list(), ptree_helpers::is_animated);

	for(; r; ++r)
	{
		qdNamedObject* o = *r;
		if (o != excl&&o != excl2)
		{
			int iPos = combo.AddString(o->name());
			if(iPos != -1)
				combo.SetItemDataPtr(iPos, o);
		}
	}
	if (!combo.GetCount())
	{
		CString str;
		str.LoadString(IDS_NOTHING);
		combo.AddString(str);
	}
	combo.SetCurSel(0);
}
