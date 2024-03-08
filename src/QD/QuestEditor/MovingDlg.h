#if !defined(AFX_MOVINGDLG_H__F2D604FE_2B59_4B1A_838E_7F33AF01F94F__INCLUDED_)
#define AFX_MOVINGDLG_H__F2D604FE_2B59_4B1A_838E_7F33AF01F94F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MovingDlg.h : header file
//

#include "editlistctrl.h"
#include <set>

#include "DispsListsMerger.h"
#include "qd_animation_set.h"
#include "afxcmn.h"

/////////////////////////////////////////////////////////////////////////////
// MovingDlg dialog
class qdGameDispatcher;
class qdGameDispatcherBase;
class qdAnimationSet;
class qdAnimationInfo;
class qdAnimationSetPreview;

class GDI_grDispatcher;

class MovingDlg : public CDialog
{
	/// Редактируемая походка
	qdAnimationSet *m_ptrObj;
	/// Диспетчер каталога, которому принадлежит походка
	qdGameDispatcherBase* m_ptrGameDispBase;
	/// Диспетчер проекта
	qdGameDispatcher* m_ptrGameDisp;
	/// Уникальный список анимаций из каталогов
	std::auto_ptr<CStringSet_t> m_ptrAnimList;
	/// Прокручивается ли анимация
	bool *m_pbShowAnimation;
	/// Диспетчер для отображения походки
	std::auto_ptr<GDI_grDispatcher> m_ptrXGR;
	/// Отображает анимацию
	std::auto_ptr<qdAnimationSetPreview> m_ptrASP;

	CBrush m_brBackColor;
	CBrush m_brGridColor;
	/// Выбранная анимация
	qdAnimationInfo* m_ptrCurrAnimInfo;
	/// Идет ли инициализация
	bool m_bInitDialog;

	/// Резервная копия
	qdAnimationSet m_BackAnimSet;

	CSliderCtrl m_wndAnimPhase;
// Construction
public:
	MovingDlg(CWnd* pParent = NULL);   // standard constructor

	/// Назначает диспетчер проекта и походку
	bool set_data(qdGameDispatcher* ptrDips, qdAnimationSet* ptrObj);
// Dialog Data
	//{{AFX_DATA(MovingDlg)
	enum { IDD = IDD_DIALOG_MOVING };
	CEditListCtrl	m_list;
	/// список анимаций, для задания анимации поворота
	CCJFlatComboBox	animations_;
	/// номер выбранной в комбо анимации
	int		animation_;
	/// Количество направлений
	int		m_dirs;
	BOOL	m_bRunAnim;
	int		m_iCamPitch;
	int		m_iCamFocus;
	/// Размер сетки
	int		m_iGridSize;
	/// Высота персонажа
	int		m_iPersHeight;
	/// Инвертировать вертикально
	BOOL	m_bFlipVert;
	/// Зацикливать анимацию
	BOOL	m_bLoop;
	/// Отразить горизонтально
	BOOL	m_bFlipHorz;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MovingDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:

	BOOL PreTranslateMessage(MSG* pMsg);
	// Generated message map functions
	//{{AFX_MSG(MovingDlg)
	afx_msg void OnDirSet();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckShow();
	afx_msg void OnChangeEditPersHeight();
	afx_msg void OnChangeEditGridSize();
	afx_msg void OnChangeEditCamPitch();
	afx_msg void OnChangeEditCamFocus();
	afx_msg void OnBtnBackColor();
	afx_msg void OnBtnColor();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnCheckFlipVert();
	afx_msg void OnCheckLoop();
	afx_msg void OnCheckFlipHorz();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void OnEndEditList(NMHDR * pNMHDR, LRESULT* pResult);
	afx_msg void OnSetupCtrlList(NMHDR * pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedBtnToBeg();
	afx_msg void OnBnClickedBtnPrev();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnToEnd();
	afx_msg void OnDeltaposSpinStartAngle(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP();

	bool InsertColumns();

	/// создаем единый список имен анимаций без повторений
	bool MakeAnimationList();
	/// загружаем список анимаций походок
	bool LoadMovings();
	/// Выравнивает ширину столбцов, так чтобы не скрывался текст 
	void AdjustColumnsWidth();
	/// Вставляет пару походка/остановка в список 
	bool InsertAnimInfo(int iDir, qdAnimationInfo* walk_info, qdAnimationInfo* stand_info, qdAnimationInfo* start_info, qdAnimationInfo* stop_info);
	/// Запускает цикл прокрутки анимации
	bool ShowAnimation();

	/// Создает и настраивает графический диспетчер
	bool CreateXGraph();
	/// Создает и настраивает qdAnimationPreview
	bool CreateAnimPreview();
	/// Параметры превью в контролы диалога
	void AnimPreview2Wnd();
	/// Выравнивание промежутков изменения спин-контролов
	void AdjustSpints();

	/// \brief Управлеяет доступностью чек-боксов: IDC_CHECK_FLIP_VERT,
	/// IDC_CHECK_FLIP_HORZ, IDC_CHECK_LOOP
	void EnableCheckBoxes(BOOL bEnable);
	/// Выставляет состояния чек-боксов по инфе об анимации
	void SetChecksState(qdAnimationInfo* ptrAI);

	/// Возвращает фазу анимации
	float GetPhase() const;
	/// Устанавливает промежуток изменения фазы анимации
	void SetPhaseRange();
	/// Назначает фазу для отображения
	void SetASPPhase();
	/// Управляет доступностью кнопок навигации по анимации
	void AccessToAnimCtrls();
	/// Загружает битмапы для кнопок навигации по анимации
	void LoadBitmaps();
private:
	/// Начальный угол для походки
	float m_fStartAngle;
	/// ускорение
	float m_fAcceleration;
	/// Максимальная скорость
	float m_fMaxSpeed;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MOVINGDLG_H__F2D604FE_2B59_4B1A_838E_7F33AF01F94F__INCLUDED_)
