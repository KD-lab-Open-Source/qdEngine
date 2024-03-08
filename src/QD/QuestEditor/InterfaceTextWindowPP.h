#pragma once

#include "qd_interface_text_window.h"
// InterfaceTextWindowPP dialog
#include "BasePage.h"
#include "afxwin.h"

#include "custom controls\CJColorPicker.h"

/*!
	Диалог свойст для окна текстов
*/
class InterfaceTextWindowPP : public CBasePage
{
	DECLARE_DYNAMIC(InterfaceTextWindowPP)

public:
	InterfaceTextWindowPP();
	virtual ~InterfaceTextWindowPP();

	//! Назначает окно текстов, которое будем редактировать
	void set_data(qdInterfaceTextWindow* obj);
// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_TEXT_WINDOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG*);
	afx_msg void OnBnClickedButtonSelBackground();
	afx_msg void OnCbnSelchangeComboSliderNames();
	afx_msg void OnEnKillfocusEdit();
	afx_msg void OnDeltaposSpinX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinD(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinCY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinLength(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinPlayer(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
protected:
	//! данные из диалога отправляются в объект
	void wnd2obj();
	//! данные из объекта отправляются в диалог
	void obj2wnd();
	//! список доступных слайдеров
	void loadSliderList();

	void InitFontBox(CCJFlatComboBox& box);
private:
	//! Исходное окно, переданное в set_data
	qdInterfaceTextWindow* sourceObj_;
	//! Объект-копия исходного, чтобы можно было откатить изменения
	qdInterfaceTextWindow backUp_;
	//! Координата Х
	int x_;
	//! Координата У
	int y_;
	//! Координта Z
	int z_;
	//! Ширина
	int cx_;
	//! Высота
	int cy_;
	//! Имя файла с картинкой для фона
	CString backGround_;
	//! Имя слайдера
	CString sliderName_;
	//! Список доступных слайдеров
	CCJFlatComboBox slidersCombo_;
	CCJColorPicker backColorWnd_;
	BOOL enableBackColor_;
	int backColorAlpha_;
	CCJFlatComboBox typesCombo_;
	int windowType_;

	/// отображаемый в окне текст
	CString text_;
	//! Контрол выбора цвета текста
	CCJColorPicker textColorWnd_;
	//! Контрол выбора hover color
	CCJColorPicker hiliteColorWnd_;
	//! Список фонтов
	CCJFlatComboBox fontsWnd_;
	//! Выравнивание текста
	CCJFlatComboBox alignWnd_;
	/// Максимальная длина вводимой строки.
	int stringLength_;

	//! Выравнивание диалогов по вертикали
	CCJFlatComboBox dialogAlignWnd_;

	CCJFlatComboBox infoTypeWnd_;
	int playerID_;

public:
	afx_msg void OnBnClickedCheckFill();
};
