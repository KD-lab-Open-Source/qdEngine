#pragma once
#include "afxwin.h"

#include "BasePage.h"

class qdGameDispatcher;
// FontsPropPage dialog

/*!
    Свойства каталога шрифтов
*/
class FontsPropPage : public CBasePage
{
	DECLARE_DYNAMIC(FontsPropPage)

public:
	FontsPropPage();
	virtual ~FontsPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_FONTS };

	//! Назначает диспетчер игры
	void set_data(qdGameDispatcher* gameDisp);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	afx_msg void OnCbnSelchangeComboDefaultFont();

	DECLARE_MESSAGE_MAP();
	//! Загружает список фонтов 
	void LoadFonts();
	//! Назначает выбранным в комбобоксе шрифт, установленный по умолчанию
	void SetDefaultFont();
private:
	CCJFlatComboBox defaultFont_;
	qdGameDispatcher* m_ptrGameDisp;
};
