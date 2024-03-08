// FontsPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include ".\fontsproppage.h"

#include "qd_font_info.h"

#include <boost/rtl.hpp>

// FontsPropPage dialog

IMPLEMENT_DYNAMIC(FontsPropPage, CBasePage)
FontsPropPage::FontsPropPage()
	: CBasePage(FontsPropPage::IDD)
	, m_ptrGameDisp(NULL)
{
}

FontsPropPage::~FontsPropPage()
{
}

void FontsPropPage::set_data(qdGameDispatcher* gameDisp){
	m_ptrGameDisp = gameDisp;
}

void FontsPropPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEFAULT_FONT, defaultFont_);
}


BEGIN_MESSAGE_MAP(FontsPropPage, CBasePage)
	ON_CBN_SELCHANGE(IDC_COMBO_DEFAULT_FONT, OnCbnSelchangeComboDefaultFont)
END_MESSAGE_MAP()


// FontsPropPage message handlers

void FontsPropPage::OnCbnSelchangeComboDefaultFont()
{
	SetModified();
}

BOOL FontsPropPage::OnInitDialog()
{
	CBasePage::OnInitDialog();

	LoadFonts();
	SetDefaultFont();
	return TRUE;  
}

//! Загружает список фонтов 
void FontsPropPage::LoadFonts()
{
	boost::rtl::crange<qdFontInfoList const> r(m_ptrGameDisp->fonts_list());
	for(; r; ++r)
	{
		int const item = defaultFont_.AddString((*r)->name());
		if (item != CB_ERR)
			defaultFont_.SetItemDataPtr(item, *r);
	}

	CString str;
	str.LoadString(IDS_NOTHING);
	defaultFont_.InsertString(0, str);
}

//! Назначает выбранным в комбобоксе шрифт, установленный по умолчанию
void FontsPropPage::SetDefaultFont()
{
	for (int i = 1; i < defaultFont_.GetCount(); ++i)
	{
		qdFontInfo* fi = reinterpret_cast<qdFontInfo*>(defaultFont_.GetItemDataPtr(i));
		if (fi->type() == m_ptrGameDisp->default_font())
		{
			defaultFont_.SetCurSel(i);
			return;
		}
	}
	defaultFont_.SetCurSel(0);
}

BOOL FontsPropPage::OnApply()
{
	int font = defaultFont_.GetCurSel();
	if (font)
		font = reinterpret_cast<qdFontInfo*>(defaultFont_.GetItemDataPtr(font))->type();
	else
		font = -1;
	m_ptrGameDisp->set_default_font(font);
	return CBasePage::OnApply();
}
