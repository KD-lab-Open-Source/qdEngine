// MinigameParamsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "QuestEditor.h"
#include "MinigameParamsDlg.h"

#include "MiniGameParams/MGParamsChangerFactory.h"
#include "MiniGameParams/ParamChanger/MGParamChanger.h"

#include <boost/rtl.hpp>
#include ".\minigameparamsdlg.h"

/**
	«апускает диалог редактировани€ параметров миниигры
	\return true - нажали ќ  в диалоге
 */
bool editMinigameParams(qdGameDispatcher* gameDisp, qdMiniGame* miniGame)
{
	MinigameParamsDlg dlg;
	dlg.set_data(gameDisp, miniGame);
	return (dlg.DoModal() == IDOK);
}
// MinigameParamsDlg dialog

IMPLEMENT_DYNAMIC(MinigameParamsDlg, CDialog)
MinigameParamsDlg::MinigameParamsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MinigameParamsDlg::IDD, pParent)
	, gameDisp_(NULL)
	, minigame_(NULL)
{
}

MinigameParamsDlg::~MinigameParamsDlg()
{
	boost::rtl::crange<ParamChangers const> r(paramChangers_);
	for (; r; ++r)
		delete *r;
	paramChangers_.clear();
}

void MinigameParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM, treeList_);
}


BEGIN_MESSAGE_MAP(MinigameParamsDlg, CDialog)
END_MESSAGE_MAP()


// MinigameParamsDlg message handlers

void MinigameParamsDlg::set_data(qdGameDispatcher* gameDisp, qdMiniGame* miniGame)
{
	gameDisp_ = gameDisp;
	minigame_ = miniGame;
}

BOOL MinigameParamsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	initList();
	copyParamsToChangers();
	loadToList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void MinigameParamsDlg::copyParamsToChangers()
{
	boost::rtl::crange<qdMiniGame::config_container_t const> r(minigame_->config());
	for(; r; ++r)
	{
		std::auto_ptr<MGParamChanger> res = 
			MGParamsChangerFactory::instance().createParamChanger(*r, gameDisp_);
		if (res.get())
		{
			paramChangers_.push_back(res.get());
			res.release();
		}
	}
}

void MinigameParamsDlg::loadToList()
{
	boost::rtl::crange<ParamChangers const> r(paramChangers_);
	for (; r; ++r){
		MGParamChanger* changer = *r;
		int item = treeList_.InsertItem(0, changer->getParameterName());
		if (item != -1)
		{
			treeList_.SetItemData(item, reinterpret_cast<DWORD_PTR>(changer) );
			treeList_.SetItemText(item, 1, changer->getValueString());
			treeList_.SetItemText(item, 2, changer->getCommentString());
		}
	}
	treeList_.SetColumnWidth(0, TLSCW_AUTOSIZE_USEHEADER);
	treeList_.SetColumnWidth(1, TLSCW_AUTOSIZE_USEHEADER);
	treeList_.SetColumnWidth(2, TLSCW_AUTOSIZE_USEHEADER);
}

void MinigameParamsDlg::initList()
{
	treeList_.SetStyle
		( 0
		//		| TLC_BKGNDIMAGE							// image background
		//		| TLC_BKGNDCOLOR							// colored background ( not client area )
		//		| TLC_DOUBLECOLOR							// double color background

//		| TLC_MULTIPLESELECT						// single or multiple select
		| TLC_SHOWSELACTIVE							// show active column of selected item
		| TLC_SHOWSELALWAYS							// show selected item always
		| TLC_SHOWSELFULLROWS						// show selected item in fullrow mode

		| TLC_HEADER								// show header
		| TLC_HGRID									// show horizonal grid lines
		| TLC_VGRID									// show vertical grid lines
		| TLC_TGRID									// show tree horizonal grid lines ( when HGRID & VGRID )
		| TLC_HGRID_EXT								// show extention horizonal grid lines
		| TLC_VGRID_EXT								// show extention vertical grid lines
		| TLC_HGRID_FULL							// show full horizonal grid lines
//		| TLC_READONLY								// read only

//		| TLC_IMAGE									// show image
		| TLC_HOTTRACK								// show hover text 

//		| TLC_DROP									// drop support
//		| TLC_DRAG									// drop support
//		| TLC_DROPTHIS								// drop self support
//		| TLC_DROPROOT								// drop on root support

//		| TLC_HEADDRAGDROP							// head drag drop
		| TLC_NOAUTOCHECK							// do NOT auto set checkbox of parent & child
		| TLC_NOAUTOLOCK							// do NOT auto set lockbox of parent & child
		);

	CString caption;
	caption.LoadString(IDS_PARAM_NAME_COLUMN);
	treeList_.InsertColumn(caption,TLF_DEFAULT_LEFT, 100);

	caption.LoadString(IDS_PARAM_VALUE_COLUMN);
	treeList_.InsertColumn(caption,TLF_DEFAULT_LEFT, 100);

	caption.LoadString(IDS_PARAM_COMMENT_COLUMN);
	treeList_.InsertColumn(caption,TLF_DEFAULT_LEFT, 100);

	treeList_.SetColumnModify(1, TLM_REQUEST);
	treeList_.setItemChangeNotifyListener(this);
	treeList_.setGeneralNotifyListener(this);

}

//! ѕосле изменени€ состо€ни€
void MinigameParamsDlg::onItemStateChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci)
{
	if (pci->uNewState&TLIS_FOCUSED)
	{
		MGParamChanger* changer = 
			reinterpret_cast<MGParamChanger*>(tree.GetItemData(pci->item));

		if (changer)
			treeList_.setChangeItemLabelNotifyListener(changer);
	}
}

void MinigameParamsDlg::OnOK()
{
	copyChangersToParams();	
	__super::OnOK();
}

void MinigameParamsDlg::copyChangersToParams()
{
	qdMiniGame::config_container_t params;
	boost::rtl::crange<ParamChangers const> r(paramChangers_);
	for (; r; ++r){
		MGParamChanger const* changer = *r;
		params.push_back(changer->getParameter());
	}
	minigame_->set_config(params);
}

/*!
	¬озрващаемое значение имеет смысл только дл€ дерева
	\retval true - дл€ того, чтобы позволить обработку по умолчанию
	\retval false - дл€ того, чтобы запретить обработку по умолчанию
*/
bool MinigameParamsDlg::onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info)
{
	if (info->iSubItem == 1)
	{
		treeList_.EditLabel(info->item, info->iSubItem);
	}
	return true;
}
