// MinigameParams\GameObjectNameSelectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "MinigameParams\GameObjectNameSelectorDlg.h"
#include ".\gameobjectnameselectordlg.h"

#include "Main/ObjectTraits.h"

#include <boost/rtl.hpp>

#include "qd_game_object.h"

/*!
	Функция, которая используется для получения имени объекта и числа раз, которое
	необходимо создать этот объект
*/
std::pair<CString, int> selectGameObjectName(qdGameDispatcher* gameDisp, int count)
{
	GameObjectNameSelectorDlg dlg;
	dlg.setGameDispatcher(gameDisp);
	dlg.setCount(count);
	std::pair<CString, int> res;
	if (dlg.DoModal() == IDOK)
	{
		res.first = ((GameObjectNameSelectorDlg const&)dlg).getObjectName();
		res.second = dlg.getCount();
	}
	return res;
}

// GameObjectNameSelectorDlg dialog

IMPLEMENT_DYNAMIC(GameObjectNameSelectorDlg, CDialog)
GameObjectNameSelectorDlg::GameObjectNameSelectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GameObjectNameSelectorDlg::IDD, pParent)
	, gameDisp_(NULL)
	, count_(0)
{
}

GameObjectNameSelectorDlg::~GameObjectNameSelectorDlg()
{
}

void GameObjectNameSelectorDlg::setGameDispatcher(qdGameDispatcher const* gameDisp)
{
	gameDisp_ = gameDisp;
}

void GameObjectNameSelectorDlg::setCount(int count)
{
	count_ = count;
	if (!count_)
		count_ = 1;
}

CString const GameObjectNameSelectorDlg::getObjectName() const{
	return objectName_;
}

int GameObjectNameSelectorDlg::getCount() const{
	return count_;
}

void GameObjectNameSelectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SCENES, scenes_);
	DDX_Control(pDX, IDC_LIST_OBJECTS, objects_);
	DDX_Text(pDX, IDC_EDIT_COUNT, count_);
}


BEGIN_MESSAGE_MAP(GameObjectNameSelectorDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
	ON_LBN_DBLCLK(IDC_LIST_OBJECTS, OnLbnDblclkListObjects)
END_MESSAGE_MAP()


// GameObjectNameSelectorDlg message handlers

BOOL GameObjectNameSelectorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_COUNT))->SetRange(1, 1000);
	loadScenes();
	scenes_.SetCurSel(0);
	OnCbnSelchangeComboScenes();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void GameObjectNameSelectorDlg::OnCbnSelchangeComboScenes()
{
	qdGameScene const* scene = getScene();
	if (scene)
		loadObjects(scene);
	else
	{
		objects_.EnableWindow(FALSE);
	}
}

void GameObjectNameSelectorDlg::getObjectName()
{
	int curSel = objects_.GetCurSel();
	if (curSel == CB_ERR)
		return;
	objects_.GetText(curSel, objectName_);
}

void GameObjectNameSelectorDlg::OnLbnDblclkListObjects()
{
	getObjectName();
	UpdateData();
	EndDialog(IDOK);
}


void GameObjectNameSelectorDlg::loadScenes()
{
	boost::rtl::crange<qdGameSceneList const> r(gameDisp_->scene_list());
	for (; r; ++r)
	{
		int item = scenes_.AddString((*r)->name());
		if (item != CB_ERR)
			scenes_.SetItemDataPtr(item, *r);
	}
	if (!scenes_.GetCount())
	{
		CString str;
		str.LoadString(IDS_NOTHING);
		scenes_.AddString(str);
	}
}

void GameObjectNameSelectorDlg::loadObjects(qdGameScene const* scene)
{
	objects_.ResetContent();
	boost::rtl::filtered_range<
		qdGameObjectList const,
		bool (*)(qdNamedObject const*)
	> r(scene->object_list(), ObjectTraits::is_animated);

	for(; r; ++r)
	{
		int item = objects_.AddString((*r)->name());
		if (item != CB_ERR)
			objects_.SetItemDataPtr(item, *r);
	}
}

qdGameScene const* GameObjectNameSelectorDlg::getScene()
{
	int curSel = scenes_.GetCurSel();
	if (curSel == CB_ERR)
		return NULL;
	return static_cast<qdGameScene*>(scenes_.GetItemDataPtr(curSel));
}

void GameObjectNameSelectorDlg::OnOK()
{
	getObjectName();
	UpdateData();
	CDialog::OnOK();
}
