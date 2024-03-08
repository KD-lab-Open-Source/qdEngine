#include "afxwin.h"
#include "afxcmn.h"
#if !defined(AFX_STATECOORDSSTACKER_H__842FFD2A_D8CA_4EC2_B5FD_8C160739B8E8__INCLUDED_)
#define AFX_STATECOORDSSTACKER_H__842FFD2A_D8CA_4EC2_B5FD_8C160739B8E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StateCoordsStacker.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// StateCoordsStacker dialog

#include "qd_game_object_state.h"

interface ISceneViewFacade;
class qdGameObjectState;
class qdGameObjectAnimated;
class qdGameObjectStateStatic;
class qdGameObjectStateWalk;
class qdAnimationInfo;
class SceneTreeView;

const int MAX_ANIM_FRAMES = 1000;///< Максимальное количество фреймов в анимации

/// Режимы стыковки
enum STACK_MODE {
	SM_OBJ_COORDINATE, ///< Координаты объекта
	SM_STATE_OFFSET, ///<  Смещение состояния
	SM_STATE_BOUND, ///<  Границы состояния
	SM_STATE_SCALE,///< Масштаб состояния
	SM_BIG = 1000000
};

/// Что будем сдвигать изображение к центру объекта или центр объекта к изображению
enum eWHAT_SHIFT{ WS_IMAGE, WS_CENTER};

/**
	Всевозможные стыковки состояния
 */
class StateCoordsStacker : public CDialog
{
// Construction
	/// Идет ли инициализация диалога
	bool m_bInitDialog;

	/// Резервная копия для позиции объекта
	Vect3f m_vObjPosBak;
	/// Предыдущее состояние редактора сцены
	int m_eOldViewState;
	/// Резервная копия состояния
	std::auto_ptr<qdGameObjectState> m_ptrStateBak;

	/// Редактор сцены
	ISceneViewFacade* m_ptrView;
	///  стыкуемое состояние
	qdGameObjectState *m_ptrState;
	/// состояние, с которым стыкуем - подложка(рисуется снизу)
	qdGameObjectState *m_ptrSelectedState;
	/// выбранная анимация состояния, по которому происходит стыковка(рисуется снизу)
	qdAnimationInfo* m_ptrBackAnimationInfo;

	/// выбранная анимация стыкуемого состояния(то что рисуется сверху)
	qdAnimationInfo* m_ptrCurStateAI;

	///  применяется при загрузке анимации из походки
	qdAnimationInfo const*const NO_EXCLUDE;

	/// указывает является рисуемая анимация редактируемого состояния остановкой или нет
	qdGameObjectStateWalk::OffsetType editedStateAIType;
	/// указывает является рисуемая анимация состояния-образца остановкой или нет
	qdGameObjectStateWalk::OffsetType backObjStateAIType;

public:
	StateCoordsStacker(CWnd* pParent = NULL);   // standard constructor
	enum { IDD = IDD_DLG_STACK_STATE_COORD };
	
	/// Возвращает стыкуемое состояние
	qdGameObjectState* getStackedState() const;
private:
// Dialog Data
	//{{AFX_DATA(StateCoordsStacker)
	CCJFlatComboBox	m_wndCurStateAnim;
	CButton	m_wndCurStateCap;
	CSliderCtrl	m_wndEditedStateAnimFrames;
	CSliderCtrl	m_wndObjStateAnimFrames;
	CCJFlatComboBox	m_wndAnim;
	CCJFlatComboBox	m_wndStates;
	int		m_iX;
	int		m_iY;
	int		m_iZ;
	CString	m_strStaticObjCurFrame;
	CString	m_strStaticEditedStateCurFrame;
	//! \brief Управляет текущим кадром для обычной анимации и фазой координатной 
	//! анимации для состояния-подложки
	int		m_iObjStateAnimFrame;
	//! \brief Управляет текущим кадром для обычной анимации и фазой координатной 
	//! анимации для редактируемого состояния
	int		m_iEditedStateAnimFrame;
	int		m_iStackMode;
	CStatic m_wndStaticZ;
	//}}AFX_DATA
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(StateCoordsStacker)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(StateCoordsStacker)
	afx_msg void OnSelchangeComboStates();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDeltaposSpinX(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinY(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboAnimation();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangeComboCurStateAnim();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnBnClickedCheckTogether();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedRadioToCenter();
	afx_msg void OnBnClickedRadioToImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	
	/// \brief загружает состояния объекта, 
	/// исключая стыкуемое состояние, если это непоходка
	void LoadObjectStates(qdGameObjectAnimated* ptrObj);

	/// загружает состояния всех объектов из сцены
	void LoadObjectsStates();
	
	/// Загружает анимацию, используемую состоянием
	/**
		Для походки это анимамация по всем направлениям. 
		Для статического только одна анимация
	 */
	void LoadStateAnimation(CCJFlatComboBox& wnd, qdGameObjectState const* ptrState);
	/// Вспомогательная для LoadStateAnimation. Загружает анимацию статического состояния
	void LoadStaticStateAnimation(CCJFlatComboBox& wnd, 
								qdGameObjectStateStatic const* ptrState);

	/// Вспомогательная для LoadStateAnimation. Загружает анимацию походки
	void LoadWalkStateAnimation(CCJFlatComboBox& wnd, 
					qdGameObjectStateWalk const* ptrState, 
					qdAnimationInfo const* exclude);

	/// Парамерты состояния в окно
	/**
		Параметры выбираются, исходя из того, что стыкуем
	 */
	void state2wnd();
	/// Данные из окна в состояние
	/**
		Данные выбираются, исходя из того, что стыкуем
	 */
	void wnd2state();
	
	/// \brief Копируем позицию объекта и всё состояние
	/// для того, чтобы была возможность восстановить
	void real2bak();
	/// Восстанавливаем из резервной копии изменные значения
	void bak2real();
	
	/// Перематывает анимацию состояния на начало.
	void state2begin(qdGameObjectState* ptrState);

	///\brief управляем доступом к контролам управлящим выбором текущей анимации,
	/// фазой, текущим кадром
	void UpdateObjAnimControls();
	/// \brief управляет доступом к m_wndCurStateAnim 
	/// и содержимым m_strStaticEditedStateCurFrame
	void UpdateEditedStateAnimControls();

	/// обновляет состояние окна, в котором отражена сцена
	void UpdateSceneViewState();
	/// Обновляет данные для контролов отображающих позицию.
	/**
		Контролы позиции показывают данные, в зависимости от того, что стыкуем
		(размеры, масштаб и т.д.) 
	 */
	void UpdatePosControls();
	/// \brief Обновляет выбранной в комбобоксе анимацией 
	/// m_ptrBackAnimationInfo(анимация-подложка)
	void UpdateAnimation();
	/// \brief Обновляет выбранной в комбобоксе анимацией 
	/// m_ptrCurStateAI(та что стыкуется)
	void UpdateEditedStateAnimation();

	/// \brief Решает какие из способов стыковки доступны для пары 
	/// m_ptrSelectedState и m_ptrState
	void UpdateModeCtrls();
	/// Масштабирует походку
	void scale_gait(qdGameObjectState* ptrState, float xs, float ys);
	
	/// Показывает/скрывает IDC_CHECK_TOGETHER, IDC_SPIN_Z, IDC_Z_EDIT, IDC_STATIC_Z
	void ShowSyncControls(UINT nCmd);

	/// \brief управляет доступом к контролам, предназначенными для скролирования по кадрам
	/// анимации, привязанной к координатной анимации
	void UpdateCAAnimScroller(CSliderCtrl& wnd, CStatic& wnd_static,
								qdGameObjectState* pstate, 
								int *piCurFrame);
	/// Является ли стыкуемое состояния курсором мыши
	bool IsMouseCursor();
	/// Управляет достопностью кнопок преключающих метод сдвига
	void WhatShiftEnable(BOOL bEnable);
	/// Проверяет можно ли дать доступ к кнопкам переключающим метод сдвига
	bool PermitAccess2WhatShift();
	/// \brief Обновляет данные в контролах координат для состояния-подложки
	void UpdateSelectedStateCtrls();
	/// Ничего не делает
	void UpdateSelectedStateCtrlsAccess();
public:
	/// Назначает редактор сцены
	void SetView(ISceneViewFacade* ptrView);
	/// Описывает возможные пути завершения редактирования
	enum eEndEditMode{
		EEM_DISCARD,///< Отменить изменения
		EEM_SAVE///< Сохранить изменения
	};
	/// Прекращает стыковку
	void EndEdit(eEndEditMode mode);
	/// Назначает стыкуемое состояние
	void set_state(qdGameObjectState* ptrState);

	/// Обновляет в контролах инфу о стыкуемом состоянии
	void UpdateStateInfo();

	/// Возвращает фазу анимации стыкуемого состояния
	float get_edited_state_anim_phase() const{
		return (static_cast<float>(m_iEditedStateAnimFrame)/MAX_ANIM_FRAMES);
	}
	/// Возвращает фазу анимации состояния - подложки
	float get_obj_state_anim_phase() const {
		return (static_cast<float>(m_iObjStateAnimFrame)/MAX_ANIM_FRAMES);
	}

	/// Возвращает инфу об анимации состояния - подложки
	qdAnimationInfo* get_back_obj_state_ai() const{
		return m_ptrBackAnimationInfo;
	}
	/// Возвращает состояние - подложку
	qdGameObjectState* get_back_obj_state(){
		return m_ptrSelectedState;
	}

	/// Возвращает инфу об анимации стыкуемого состояния
	qdAnimationInfo* get_edited_state_ai() const{
		return m_ptrCurStateAI;
	}
	/// Является ли m_ptrCurStateAI остановкой
	qdGameObjectStateWalk::OffsetType edited_state_type() const{
		return editedStateAIType;
	}
	/// Является ли m_ptrBackAnimationInfo остановкой
	qdGameObjectStateWalk::OffsetType back_obj_state_type() const{
		return backObjStateAIType;
	}
	/// Номер редактируемой анимации в походке 
	int edited_state_ai_num() const;
	/// Номер в походке анимации состояния-подложки
	int back_obj_state_ai_num() const;

	/// Возвращает режим стыковки
	STACK_MODE get_stack_mode() const {
		return static_cast<STACK_MODE>(m_iStackMode);
	}

	/// Возвращает объект, которому пренадлежит состояние - положка
	qdGameObjectAnimated* get_back_obj() const;

	/// Возвращает метод сдвига изображения
	eWHAT_SHIFT get_shift() const{
		return static_cast<eWHAT_SHIFT>(m_iWhatShift);
	}
	/// Управляет достпуностью контролов позиции для стыкуемого состояния
	void EnablePos(const bool b);
private:
	/**
		Объект призванный сохранять deley состояния.
		В конструкторе сохраняет в деструкторе восстанавливает.
	 */
	class activation_delay_guard
	{
		qdGameObjectState* pState_;
		float delay_;

		inline void release();
		inline void set(qdGameObjectState* pState);
	public:
		activation_delay_guard();
		activation_delay_guard(qdGameObjectState* pState);
		~activation_delay_guard();
		void reset(qdGameObjectState* pState);
	};
private:
	//! Если true, то изменяем размер по х и по у одновременно
	BOOL m_bSyncScale;
	//! скроллирует анимацию редактируемого состояния
	CSliderCtrl m_wndEditedStateCAScroller;

	/// Номер текущего кадра анимации
	int m_iEditStateCurFrameNum;
	/// Номер текущего кадра анимации
	int m_iSelectedStateCurFrameNum;
	CStatic m_wndScrollStatic;
	//! Подпись над слайдером, которым скроллируется неподвижная анимация
	CStatic m_wndScrollStatic2;
	//! Слайдер, которым скроллируется ролик в неподвижной КА
	CSliderCtrl m_wndSelStateCAScroller;
	//! Спин-контрол, регулирующий значение Х
	CSpinButtonCtrl m_wndSpinX;
	//! Спин-контрол, регулирующий значение У
	CSpinButtonCtrl m_wndSpinY;

	activation_delay_guard m_adg;//! сохраняет для редактируемого состояния
	activation_delay_guard m_adg2;//! сохраняет для состояний, с которыми стыкуем
	//! Что смещается(цент к изображению или изображение к центру)
	int m_iWhatShift;
	/// Масштаб изменился
	bool m_bMastabChanged;
	/// контрол Х, длинны, масштаба по Х для состояния-подложки
	int m_iXSelState;
	/// контрол Y, ширины, масштаба по Y для состояния-подложки
	int m_iYSelState;
	/// контрол Z, высоты для состояния-подложки
	int m_iZSelState;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_STATECOORDSSTACKER_H__842FFD2A_D8CA_4EC2_B5FD_8C160739B8E8__INCLUDED_)
