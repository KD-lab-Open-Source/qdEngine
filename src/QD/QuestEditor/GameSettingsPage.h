// WPgGlobSet.h : header file
//

#ifndef __WPGGLOBSET_H__
#define __WPGGLOBSET_H__

/////////////////////////////////////////////////////////////////////////////
// WPgGlobSet dialog
#include "basepage.h"
#include "afxwin.h"
class qdGameDispatcher;
#include "cammoderep.h"

#include "custom controls\CJFlatComboBox.h"
#include "custom controls\CJColorPicker.h"

/*!
    Страница свойств игры
*/

class GameSettingsPage : public CBasePage
{
	DECLARE_DYNCREATE(GameSettingsPage);

	//! указатель на глобальный диспетчер
	qdGameDispatcher* m_ptrGameDisp;
	//! Номер страницы в наборе страниц
	int m_iPagePlace;
	// Construction
public:
	GameSettingsPage();
	~GameSettingsPage();

// Dialog Data
	//{{AFX_DATA(WPgGlobSet)
	enum { IDD = IDD_PROPPAGE_GAME_SETTINGS };
	CCJFlatComboBox	m_combo;
	//}}AFX_DATA

	//! Назначает диспетчер игры
	bool set_data(qdGameDispatcher* ptrDisp);
	//! Назначает место страницы в наборе
	void set_place(int iPlace){
		m_iPagePlace = iPlace;
	}

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(WPgGlobSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnWazardFinish();
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	afx_msg void OnCbnSelchangeComboCameraMode();
	afx_msg void OnBnClickedCheckNeedSaveTimeShow();
	afx_msg void OnBnClickedCheckNeedSaveTitleShow();
	//}}AFX_VIRTUAL

	BOOL OnInitDialog();
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(WPgGlobSet)
	afx_msg void OnSelchangeResolution();
	afx_msg void OnCbnSelchangeComboScenes();
	afx_msg void OnEnChangeEditScroll();
	afx_msg void OnCbnSelchangeCombo();
	afx_msg void OnBnClickedButtonSelectTdb();
	afx_msg void OnEnChangeEdit();
	afx_msg void OnBnClickedCheckNeedSceneRedraw();
	afx_msg void OnCbnSelchangeComboDefaultFont();
	afx_msg void OnDeltaposSpinHallOfFame(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	virtual BOOL PreTranslateMessage(MSG* );
	//! Загружает список доступных разрешений экрана
	bool LoadResolutions();
	//! ничего не делает
	bool LoadCameraModes();
	//! Управляет состоянием (доступностны/недосутупны) кнопок "Назад" "Дальше"
	bool UpdateWizardBtnsState();
	//! Загружает список доступных интерфейсных экранов
	bool LoadInterfaceScreens(CCJFlatComboBox& box);

	//! Загружает список сцен
	bool LoadScenes();
	//! Указывает в диспетчере стартовую сцену
	void SetupStartupScene();
	//! Указывает интерфейсный экран, на котором расположено основное меню
	void SetupMainMenu();
	//! Назначает внутриигровой интерфейсный экран с инвентори
	void SetupIngameInvScr();//! с инвентори
	//! Назначает внутриигровой интерфейсный экран без инвентори
	void SetupIngameScr();//! без инвентори
	//! Назначает экран, вызываемый при запросе на перезапись сохраненной игры
    void SetupSavePromptScr();
	void InitFontBox(CCJFlatComboBox& box);
	void LoadFonts();
//	void SetDefaultFont();
	//! Параметры вывода текстов по умолчанию переводяться в контролы диалога
	void textOutputDefault2wnd();
	//! Данные о выводе тексте из контролов переносятся в параметры по умолчанию
	void wnd2textOutputDefault();

	//! Управляет галочкой, разрешающей вывод времени сэйва, и связанными с ней компонентами
	void toggleSaveTimeComponents(bool activate);
	void toggleSaveTitleComponents(bool activate);
private:
	/*!
	    Функтор. Загружает в сцену в комбобокс
	*/
	class insert_scenes
	{
	public:
		insert_scenes(CCJFlatComboBox& box):combo_(box){}

		void operator()(qdGameScene* pscene) const{
			int pos = combo_.AddString(pscene->name());
			if (pos != CB_ERR)
				combo_.SetItemDataPtr(pos, pscene);
		}
	private:
		CCJFlatComboBox& combo_;
	};
private:
	//! Скорость скроллирования камере в пикселях в секутду
	int m_iScrollSpeed;
	//! На каком расстоянии от центра объекта до края окна начинается скроллирование
	int m_iScroolDistance;

	//! Компрессия ресурсов при сборке, 0 - отключена, 9 - максимальная
	int compression_;

	//! Размер таблицы рекордов, 0 если нету.
	int hallOfFameSize_;

	CCJFlatComboBox m_wndScenes;
	CCJFlatComboBox m_wndScreens;
	//! Экран с инвентори
	CCJFlatComboBox m_wndScrn4Inv;
	//! Экран без инвентори
	CCJFlatComboBox m_wndScrnWithoutInv;

	//! Экран, вызывающийся при запросе подтверждения перезаписи сэйва
	CCJFlatComboBox m_wndSavePromptScrn;
	/// Экран ввода имени сэйва
	CCJFlatComboBox saveTitleScreen;

	//! Нужен ли скриншот при отображении сэйва
	BOOL m_boolNeedSaveScreenshot;
	//! Нужно ли выводить время и дату сэйва
	CButton m_wndNeedSaveTimeShow;
	CButton m_wndNeedSaveTitleShow;
	//! Шрифт, которым выводится текст сэйва (в т.ч. время и дата сэйва)
	CCJFlatComboBox m_wndSaveFont;

	CString m_strGameName;
	CString m_strDBFileName;
	CString m_strSerialKey;
	//! qdInterfaceDispatcher::set_scene_redraw
	BOOL needSceneRedraw_;

	//! Способ вывода текста
	BOOL textHorizontal_;
	//! Контрол выбора цвета текста
	CCJColorPicker textColorWnd_;
	//! Контрол выбора hover color
	CCJColorPicker hiliteColorWnd_;
	//! Список фонтов
	CCJFlatComboBox fontsWnd_;
	//! Выравнивание текста
	CCJFlatComboBox alignWnd_;

	//! Способ вывода текста
	BOOL topicTextHorizontal_;
	//! Контрол выбора цвета текста
	CCJColorPicker topicTextColorWnd_;
	//! Контрол выбора hover color
	CCJColorPicker topicHiliteColorWnd_;
	//! Контрол выбора шрифта сэйва
	CCJColorPicker saveFontColorWnd_;
	//! Список фонтов
	CCJFlatComboBox topicFontsWnd_;
	//! Выравнивание текста
	CCJFlatComboBox topicAlignWnd_;
};



#endif // __WPGGLOBSET_H__
