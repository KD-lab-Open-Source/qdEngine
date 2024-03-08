#pragma once

#include "BasePage.h"

class qdFontInfo;
class qdGameDispatcher;
// FontPropPage dialog

/// Диалог свойст фонта
class FontPropPage : public CBasePage
{
	DECLARE_DYNAMIC(FontPropPage)

public:
	FontPropPage();
	virtual ~FontPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_FONT };

	/// Устанавливает диспетчер и фонт, парамерты которого будем изменять
	void set_data(qdGameDispatcher const* gameDisp, qdFontInfo* fontInfo);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnSelectFontFile();
	afx_msg void OnEnKillfocusEditFontFile();
private:
	/// Файл с текстурой фонта
	CString fontFileName_;
	//! исходный объект
	qdFontInfo* fontInfo_;
	//! копия, для сохранения настроек
	std::auto_ptr<qdFontInfo> fontInfoBackup_;
	qdGameDispatcher const* gameDisp_;
};
