// FontPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "FontPropPage.h"
#include ".\fontproppage.h"

#include "qd_font_info.h"

#include "custom_ddx.h"
#include "file_tests.h"

#include "CurDirSafer.h"

#ifdef USE_NEW_PROJ
const CString getFontFileName();
#else
namespace ptree_add{
	const CString GetFontFileName();
}
#endif // USE_NEW_PROJ

namespace {

/// Хранит текущую директорию для операции выбора текстуры фонта
CurDirSafer& GetCurDirSafer()
{
	static CurDirSafer dirSafer;
	return dirSafer;
}

}
// FontPropPage dialog

IMPLEMENT_DYNAMIC(FontPropPage, CBasePage)
FontPropPage::FontPropPage()
	: CBasePage(FontPropPage::IDD)
	, fontFileName_(_T(""))
	, gameDisp_(NULL)
	, fontInfo_(NULL)
{
}

FontPropPage::~FontPropPage()
{
}

void FontPropPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	custom_ddx::DDX_Text(pDX, IDC_EDIT_FONT_FILE, fontFileName_, 
		file_tests::strict_file_test());
}


BEGIN_MESSAGE_MAP(FontPropPage, CBasePage)
	ON_BN_CLICKED(IDC_BTN_SELECT_FONT_FILE, OnBnClickedBtnSelectFontFile)
	ON_EN_KILLFOCUS(IDC_EDIT_FONT_FILE, OnEnKillfocusEditFontFile)
END_MESSAGE_MAP()


// FontPropPage message handlers

void FontPropPage::OnBnClickedBtnSelectFontFile()
{
#ifdef USE_NEW_PROJ
	CString res = getFontFileName();
#else
	CString res = ptree_add::GetFontFileName();
#endif // USE_NEW_PROJ
	if (res.IsEmpty())
		return;

	SetModified(fontFileName_ != res);

	fontFileName_ = res;
	UpdateData(FALSE);
}

void FontPropPage::set_data(qdGameDispatcher const* gameDisp, qdFontInfo* fontInfo)
{
	assert(fontInfo);
	assert(gameDisp);

	gameDisp_ = gameDisp;
	fontInfo_ = fontInfo;
	fontInfoBackup_.reset(new qdFontInfo(*fontInfo));
}

BOOL FontPropPage::OnInitDialog()
{
	BOOL res = CBasePage::OnInitDialog();
	if (!res)
		return FALSE;
	fontFileName_ = fontInfo_->font_file_name();
	fontFileName_.Replace(_T(".tga"), _T(".idx"));
	UpdateData(FALSE);

	return TRUE;
}

BOOL FontPropPage::OnApply()
{
	if (!UpdateData())
		return FALSE;
	CString temp(fontFileName_);
	temp.Replace(_T(".idx"), _T(".tga"));
	fontInfo_->set_font_file_name(temp);

	if (!fontInfo_->load_font())
	{
		CString msg;
		msg.Format(IDS_ERROR_CANT_LOAD_FONT, fontInfo_->font_file_name());
		AfxMessageBox(msg, MB_OK|MB_ICONERROR);
		return FALSE;
	}

	return CBasePage::OnApply();
}
void FontPropPage::OnCancel()
{
	*fontInfo_ = *fontInfoBackup_;
	CBasePage::OnCancel();
}

void FontPropPage::OnEnKillfocusEditFontFile()
{
	if (!UpdateData())
		return;

	SetModified(fontFileName_ != fontInfo_->font_file_name());
}
