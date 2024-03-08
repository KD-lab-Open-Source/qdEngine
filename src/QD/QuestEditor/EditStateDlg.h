#if !defined(AFX_EDITSTATEDLG_H__D7F9FB45_4C99_4068_9477_2CAED45A4D46__INCLUDED_)
#define AFX_EDITSTATEDLG_H__D7F9FB45_4C99_4068_9477_2CAED45A4D46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditStateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// EditStateDlg dialog

class qdGameDispatcher;
class qdAnimationInfo;
class qdGameObjectState;
class qdGameObjectAnimated;
class qdCoordsAnimation;

interface IProjectFacade;

#include "RqstngStateEdit.h"
#include "DispsListsMerger.h"
#include "afxwin.h"
#include "custom controls\CJFlatComboBox.h"
#include "custom controls\CJColorPicker.h"
#include "custom controls\cjcolorpicker.h"
#include "custom controls\cjflatcombobox.h"

//Типы состояний
const int ST_NORMAL_STATE	= 0;///< Обычное состояния
const int ST_CA_STATE		= 1;///< Состояние с КА

class EditStateDlg : public CDialog, public IRequestingStateEdit
{
	/// Владелец состояния
	qdGameObjectAnimated* m_ptrObj;
	/// Диспетчер проекта
	qdGameDispatcher* m_ptrDisp;
	/// Исходное состояние
	qdGameObjectState* m_ptrState;
	/// Копия для восстановления
	std::auto_ptr<qdGameObjectState> m_ptrStateBak;

	/// Список анимаций
	std::auto_ptr<CStringSet_t> m_ptrAnimSet;
	/// Список походок
	std::auto_ptr<CStringSet_t> m_ptrPersGaintsSet;
	/// Список звуков
	std::auto_ptr<CStringSet_t> m_ptrSoundSet;

	bool m_bInitDialog;
	bool m_bEditCoordAnim;///< разрешает редактировать точик на опорной плоскости
	bool m_bUseCoordAnim;///< разрешает использовать координатную анимацию
	bool m_bUseStacking;///< разрешает использовать стыковку
	bool m_bUseBound;///< разрешает редактироавние границ
	HWND m_hOwnedWnd;///< окно, которое запустило диалог
	CCJFlatComboBox m_wndCursors;
	/// Зацикливать ли звук
	BOOL m_bSoundLoop;
// Construction
public:
	EditStateDlg(HWND hOwnedWnd = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(EditStateDlg)
	enum { IDD = IDD_DLG_STATE_PROP };
	/// Список анимаций или походок в зависимости от типа состояния
	CCJFlatComboBox	m_wndAnim;
	/// Зациклена ли анимация
	BOOL	m_bLoop;
	/// Перевернута ли анимация
	BOOL	m_bFlipVert;
	/// Перевернута ли анимация
	BOOL	m_bFlipHorz;
	/// номер выбранной в комбо анимации
	int		m_iAnimation;
	/// номер выбранного в комбо звука
	int		m_iSound;
	/// По-моему эта штука не нужна. Что-то не пойму зачем она здесь
	BOOL	m_bSetPoints;
	/// Скрость движения по КА
	int		m_iSpeed;
	/// QD_COORDS_ANM_OBJECT_START_FLAG
	BOOL	m_bGo2First;
	/// Состояние с коородинатной анимацией или нет
	int		m_iStateType;

	float angle_;
	float angle_speed_;

	float scale_x_;
	float scale_y_;

	float scale_x_speed_;
	float scale_y_speed_;

	/// qdGameObjectState::QD_OBJ_STATE_FLAG_DIALOG_PHRASE
	BOOL	m_IsDlgPhrase;
	//}}AFX_DATA

	/// Назначает диспетчер и объект владелец
	bool set_data(qdGameDispatcher* ptrDisp, qdGameObjectAnimated* ptrObj);
	/// Назначает редактируемое состояние
	bool set_state(qdGameObjectState* ptrState);

	/// Возвращает редактируемое состояние
	qdGameObjectState* GetState();
	/// Вызывается, когда стыковка завершена
	void EndStackState();
	/// Вызывается, когда редактирование КА завершено
	void EndEditCoordAnimation();
	/// Вызывается, когда редактирование размеров завершено
	void EndEditStateDims();


	/// возвращает координатную анимацию состояния
	qdCoordsAnimation& GetCoordsAnimation();

	/// Запрещает стыковку
	void suppress_stacking(){
		m_bUseStacking = false;
	}
	/// Запрещает редактирование размеров
	void suppress_bound(){
		m_bUseBound = false;
	}

	/// запрещаем любое использование КА и вычищаем её, если была использована
	void suppress_coord_anim(){
		m_bUseCoordAnim = false;
		m_bEditCoordAnim = false;
	}
	/// запрещаем редактировать точки на оп.плоскости
	void suppress_edit_coord_anim(){
		m_bEditCoordAnim = false;
	}
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EditStateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(EditStateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeComboAnimation();
	afx_msg void OnCheckSetPoints();
	afx_msg void OnChangeEditSpeed();
	afx_msg void OnBtnStackState();
	afx_msg void OnRadioStateType();
	afx_msg void OnRadioStateType2();
	afx_msg void OnButtonDims();
	afx_msg void OnCheckInventory();
	afx_msg void OnCheckMove2inventory();
	afx_msg void OnCbnSelchangeComboSound();
	afx_msg void OnBnClickedButtonAcceleration();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnCbnSelchangeComboShortPhraseId();
	afx_msg void OnCbnSelchangeComboFullPhraseId();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnSelShortPhraseId();
	afx_msg void OnBnClickedBtnSelLongPhraseId();
	afx_msg void OnBnClickedButtonPlaySound();
	afx_msg void OnBnClickedButtonEditAnimation();
	afx_msg void OnEnChangeEditTransparency();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
private:
	/// Параметры анимации в контролы диалога
	void animation2wnd();
	/// Данные об анимации из контролов переносятся в состояние
	void wnd2animation();

	/// Параметры походки в контролы диалога
	void gaint2wnd();
	/// Данные о походке из контролов переносятся в состояние
	void wnd2gaint();

	/// Параметры состояния в контролы диалога
	void state2wnd();
	/// Данные об анимации из контролов переносятся в состояние
	void wnd2state();

	/// Параметры вывода текстов в контролы диалога
	void textOutputParams2wnd();
	/// Данные о выводе тексте из контролов переносятся в состояние
	void wnd2textOutputParams();

	/// Параметры звука в контролы диалога
	void sound2state();
	/// Данные о звуке из контролов переносятся в состояние
	void state2sound();

	/// Параметры текста в контролы диалога
	void text2wnd();
	/// Данные о тексте из контролов переносятся в состояние
	void wnd2text();

	/// Параметры курсора в контролы диалога
	void cursor2wnd();
	/// Данные о курсоре из контролов переносятся в состояние
	void wnd2cursor();

	/// В зависимости от типа состояния делает доступными или нет контролы в диалоге
	void SetupCtrls();
	/// Регулирует доступность контрлов для походки. Заполняет список походок
	void SetupCtrls4PersGaints();
	/// \brief Регулирует доступность контрлов для состояния с КА. 
	/// Заполняет список походок или список анимации в зависимости от типа состояния
	void SetupCtrls4CoordAnim();
	/// \brief Регулирует доступность контрлов для статического состояния. 
	/// Заполняет список анимации
	void SetupCtrls4Anim();
	/// Регулирует доступность контрлов для маски
	void SetupCtrls4StateMask();

	/// Заполняет m_wndAnim походками или анимацией
	void SetData2AnimCombo(CStringSet_t* ptrSet);
	/// Заполняет sounds звуком
	void SetData2SoundCombo(CStringSet_t* ptrSet);
	/// Заполняет m_wndCursors звуками
	void SetData2CursorCombo();
	
	/// Создает уникальный списоки анимации, походк и звуков. Эти объекты 
	/// берутся из текущей сцены и диспетчера проекта
	bool MakeSets();
	/// Управляет доступностью кнопки "Стыковать состояние"
	void CheckAccess2StackStateBtn();

	/// Загружает все ресурсы состояния
	void LoadStateResources();
	/// Выгружает все ресурсы состояния
	void FreeStateResources();

	/// Менят текущий выбор в box и посылает об этом уведомление в диралог
	void setTextId2wnd(CCJFlatComboBox& box, LPCTSTR id);
	/// Возвращает выбранный в box текст
	CString getIdText(CCJFlatComboBox& box);
	/// Имеет ли состояние условия
	bool hasCondition(qdGameObjectState const* state) const;

	/// Возвращает цвет текста
	COLORREF getTextColor() const;
	/// Возвращает hover color
	COLORREF getHiliteColor() const;
	/// Возвращает тип фонта
	int getFontType() const;

	/// Загружает шрифты в компоненты
	void loadFonts();
	
	/// Возвращает выбранный звук
	qdSound* getSound();
	/// Возвращает выбранную анимацию
	qdAnimation* getAnimation();

	/** Активирует и даективирует элементы, отвечающие за формат в зависимости от
	    флажка - использовать глобальные параметры или нет */
	void toggleFormatSettings();
private:
	/// Загружает список идентификаторов строк в текстовой базе
	void LoadTextIds(CCJFlatComboBox& box);
	/// Добавляет строку в комбо
	class insert_string;

	int autosaveMode();

private:
	/// Громкость звука
	int m_iSoundVolume;

	//! Принудительно загружать в память анимацию
	BOOL m_bLoadToMemory;
	/// Список идентификаторов для выбора короткой фразы
	CCJFlatComboBox m_wndShortPhraseIds;
	/// Список идентификаторов для выбора длинной фразы
	CCJFlatComboBox m_wndLongPhraseIds;
	/// Текст короткой фразы
	CString m_strShortText;
	/// Текст длинной фразы
	CString m_strFullText;
	//! Способ вывода текста
	BOOL textArrangement_;
	/// Идентификатор короткой фразы
	CString shortPhraseID_;
	/// Идентификатор длинной фразы
	CString longPhraseID_;

	/// Список звуков
	CCJFlatComboBox sounds;
	/// Контрол выбора цвета текста
	CCJColorPicker textColorWnd_;
	/// Контрол выбора hover color
	CCJColorPicker hiliteColorWnd_;
	/// Список фонтов
	CCJFlatComboBox fontsWnd_;

	CCJFlatComboBox autosaveMode_;
	int autosaveSlot_;

	BOOL isGlobal_;
	int fadeMode_;
	float fadeTime_;

	int shadowAlpha_;
	CCJColorPicker shadowColorWnd_;

	//! Выравнивание текста
	CCJFlatComboBox alignWnd_;

public:
	afx_msg void OnBnClickedRadioOutputArrangement2();
	afx_msg void OnBnClickedCheckIsGlobal();
	afx_msg void OnBnClickedRadioOutputArrangement();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonHiliteColor();
	afx_msg void OnCbnSelchangeComboFonts();
	afx_msg void OnCbnSelchangeComboSaveMode();
	afx_msg void OnDeltaposSpinFade(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinAngle(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinAngleSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinScaleX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinScaleXSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinScaleY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinScaleYSpeed(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_EDITSTATEDLG_H__D7F9FB45_4C99_4068_9477_2CAED45A4D46__INCLUDED_)
