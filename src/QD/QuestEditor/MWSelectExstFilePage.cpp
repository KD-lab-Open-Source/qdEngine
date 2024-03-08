// MWSelectExstFilePage.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "MWSelectExstFilePage.h"
#include "addanimwizdata.h"
#include "AddAnimMWWizard.h"
#include "selectdirdlg.h"

#include "listCtrl_insert_iterator.h"
#include <algorithm>
#include <iterator>

#include "SelectDirs.h"
#include "curdirsafer.h"
#include "disk_op.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace listCtrlInserter{
	template<>
struct data_traits<CString>{
	static int insert(CListCtrl* ctrl, int pos, const CString& data){
		return ctrl->InsertItem(pos, data);
	}
};
}
/////////////////////////////////////////////////////////////////////////////
// MWSelectExstFilePage property page
namespace{
	/// Хранитель для операции выбора набора каталогов
	CurDirSafer __select_folders_safer;
	/// Хранитель для операции выбора каталога
	CurDirSafer g_select_dir_safer;

	/// Хранит текущий каталог для операции выбора существующих файлов
	CurDirSafer& GetQDATGADirSafer(){
		static CurDirSafer safer;
		return safer;
	}
	/// Возвращает набор сущестующих файлов для сборки анимации
	bool AskQDATGAFileNames(HWND hwnd, CStringVect_t* ptrFiles)
	{
		CString strFilter;
		strFilter.LoadString(IDS_OPEN_QDA_TGA_FILTER);
		CString strTitle;
		strTitle.LoadString(IDS_OSD_CAP_QDA_TGA_SELECT);
		DWORD dwFlags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
		return disk_op::AskUser4FileNames(hwnd, false, ptrFiles, CString(), 
						strFilter, strTitle,
							CString(), GetQDATGADirSafer(), dwFlags);
	}
}
IMPLEMENT_DYNCREATE(MWSelectExstFilePage, WizardBasePage)

MWSelectExstFilePage::MWSelectExstFilePage() : WizardBasePage(MWSelectExstFilePage::IDD)
{
	//{{AFX_DATA_INIT(MWSelectExstFilePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

MWSelectExstFilePage::~MWSelectExstFilePage()
{
}

void MWSelectExstFilePage::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MWSelectExstFilePage)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MWSelectExstFilePage, WizardBasePage)
	//{{AFX_MSG_MAP(MWSelectExstFilePage)
	ON_BN_CLICKED(IDC_BTN_SEL_FILES, OnBntSelFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MWSelectExstFilePage message handlers

void MWSelectExstFilePage::OnBntSelFiles() 
{
	AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());	
	add_anim_wiz_data* data = pw->data();

	if (data->add_anim_way_ == AAW_SELECT_EXISTING)
		SelectFiles();
	else{
		switch(data->convert_way_) {
		case CW_FOLDERS:
			SelectCatalogs();
			break;
		case CW_SCENE:
		case CW_MOVE_TO_SCENES:
		case CW_STRICT_FOLDER_STRUCTURE:
			SelectCatalog();
			break;
		}
	}
	CheckNextBTNAccess();
}

BOOL MWSelectExstFilePage::OnSetActive() 
{
	UpdateFileControlsCaption();
	m_list.DeleteAllItems();
	Files2List();
	if (m_list.GetItemCount() == 0) 
		GetDlgItem(IDC_BTN_SEL_FILES)->EnableWindow(TRUE);

	CheckNextBTNAccess();

	return WizardBasePage::OnSetActive();
}

LRESULT MWSelectExstFilePage::OnWizardNext() 
{
	return 0;
}

LRESULT MWSelectExstFilePage::OnWizardBack() 
{
	GET_PSHEET()->SetActivePage(m_ptrPrevPage);
	return 1;
}

BOOL MWSelectExstFilePage::OnInitDialog() 
{
	WizardBasePage::OnInitDialog();

	m_list.InsertColumn(0, _T(""));


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

VOID MWSelectExstFilePage::UpdateFileControlsCaption(){
	CString strText;
	UINT idListClmnCap = 0;
	AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());
	add_anim_wiz_data* data = pw->data();
	if (data->add_anim_way_ == AAW_SELECT_EXISTING)
	{
		strText.LoadString(IDS_CAP_BROWSE_BUTTON1);
		idListClmnCap = IDS_CLMN_SELECTED_FILES;
	}
	else if (data->add_anim_way_ == AAW_CONVERT){
		switch(data->convert_way_) {
		case CW_FOLDERS:
			strText.LoadString(IDS_CAP_BROWSE_BUTTON2);
			idListClmnCap = IDS_CLMN_SELECTED_FOLDERS;
			break;
		case CW_SCENE:
		case CW_MOVE_TO_SCENES:
		case CW_STRICT_FOLDER_STRUCTURE:
			strText.LoadString(IDS_CAP_BROWSE_BUTTON3);
			idListClmnCap = IDS_CLMN_SELECTED_FOLDER;
			break;
		}
	}
	GetDlgItem(IDC_BTN_SEL_FILES)->SetWindowText(strText);

	CHeaderCtrl* ph = m_list.GetHeaderCtrl();

	strText.LoadString(idListClmnCap);

	HDITEM hdi;
	hdi.mask = HDI_TEXT;
	hdi.pszText = strText.GetBuffer(0);
	hdi.cchTextMax = 0;
	
	ph->SetItem(0, &hdi);
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}

bool MWSelectExstFilePage::SelectFiles()
{
	CStringVect_t vecFiles;
	if(AskQDATGAFileNames(*this, &vecFiles))
	{
		std::copy(vecFiles.begin(), vecFiles.end(), 
			listCtrlInserter::listCtrl_inserter(m_list, m_list.GetItemCount()));
		m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

		AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());
		std::copy(vecFiles.begin(), vecFiles.end(), 
			std::back_insert_iterator<CStringVect_t>(pw->data()->files_));

		return true;
	}
	return false;
}

bool MWSelectExstFilePage::SelectCatalog()
{
	CurDirGuard cdg(&g_select_dir_safer);
	SelectDirDlg dlg;
	if (dlg.DoModal()==IDOK)
	{
		m_list.InsertItem(0, dlg.GetPath());
		GetDlgItem(IDC_BTN_SEL_FILES)->EnableWindow(FALSE);
		AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());
		pw->data()->files_.push_back(dlg.GetPath());
	}
	return true;
}

bool  MWSelectExstFilePage::SelectCatalogs()
{
	CurDirGuard __cdg(&__select_folders_safer);

	CStringVect_t vecFiles;
	SelectDirs sd(&vecFiles);
	if(sd.DoModal()==IDOK)
	{
		listCtrlInserter::listCtrl_insert_iterator list_itr = 
			listCtrlInserter::listCtrl_inserter(m_list, m_list.GetItemCount());
		CString strPath = *vecFiles.begin();
		strPath += _T("\\");
		CStringVect_t::iterator itr = vecFiles.begin()+1, end = vecFiles.end();

		AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());
		add_anim_wiz_data* data = pw->data();
		std::back_insert_iterator<CStringVect_t> biitr(data->files_);

		for(;itr != end; ++itr){
			CString str (strPath + *itr);

			*list_itr = str;
			*biitr = str;

			++biitr;
			++list_itr;
		}

		m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

		__select_folders_safer.save_current();

		return true;
	}
	return false;
}

VOID MWSelectExstFilePage::Files2List(){
	AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());
	add_anim_wiz_data* data = pw->data();
	std::copy(data->files_.begin(), data->files_.end(), 
		listCtrlInserter::listCtrl_inserter(m_list, 0));
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}

void MWSelectExstFilePage::CheckNextBTNAccess()
{
	if (CanClickNext()) 
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	else
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK);
}

bool MWSelectExstFilePage::CanClickNext(){
	return m_list.GetItemCount();
}
