#if !defined(AFX_STATECONDDLG_H__4D8E05C7_562A_4572_823F_6C419CD3654A__INCLUDED_)
#define AFX_STATECONDDLG_H__4D8E05C7_562A_4572_823F_6C419CD3654A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StateCondDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// StateCondDlg dialog

class qdGameDispatcher;
class qdGameObjectState;

#include "conddlgmgr.h"
#include "afxwin.h"
#include "cammoderep.h"
#include "CondsMgr.h"

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

/**
	Диалог условий для состояния
 */
class StateCondDlg : 
	private boost::base_from_member<CMyTree>///< нужно, чтобы дерево проинициализировалось раньше других
	, public CDialog
	, public MakeCondDlgBase
{
// Construction
public:
	StateCondDlg(CWnd* pParent = NULL);   // standard constructor

public:
// Dialog Data
	//{{AFX_DATA(StateCondDlg)
	enum { IDD = IDD_DLG_STATE_COND };
	CCJFlatComboBox	m_wndConds;
	//! Логика проверки условий, не находящихся в какой-либо группе
	int		m_iCondLog;

	//! Флаг восстановить предыдущее состояние
	BOOL	m_bRestorePrev;
	//! Флаг скрыть объект по окончании
	BOOL	m_bHideObj;
	//! Флаг подходить к первой точке координатной анимации
	BOOL	m_bCAGo2First;
	//! Задержка до старта
	float	m_fStartDelay;
	//! Время выполнения состояния
	float	m_fStateDuration;
	//}}AFX_DATA
public:
	// Функции класса MakeCondDlgBase
	//! Возвращает сцену, к которой принадлежит условие
	class qdGameScene* get_scene() const;
	//! возвращает указатель на диспетчер игры
	class qdGameDispatcher* get_disp() const{
		return m_ptrGameDisp;
	};
	void OnCondDataComplete(bool bComplete);

	//! были ли изменения
	bool changed() const {
		return m_bChanged||m_CondsMgr.was_changed();
	}
	const qdGameObjectState* get_state() const{
		ASSERT(m_qdState.get());
		return m_qdState.get();
	}

	bool set_data(qdGameDispatcher* ptrDisp, qdGameObjectState* ptrState);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(StateCondDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(StateCondDlg)
	virtual void OnOK();
	//! Стартовая функция диалога. Производит настройку всех контролов
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboCondition();
	afx_msg void OnButtonAddCond();
	afx_msg void OnDeltaposSpinStartDelay(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinStateDuration(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckInventory();
	afx_msg void OnBnClickedCheckMove2inventory();
	afx_msg void OnDeltaposSpinCamModeWorkTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboCameraMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	afx_msg void OnNMRclickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCreateOr();
	afx_msg void OnCreateAnd();
	afx_msg void OnChangeType();
	afx_msg void OnDelete();
	afx_msg void OnDeleteAll();
	afx_msg void OnRemoveFromGroup();
	afx_msg void OnBnClickedCheckSyncWithSound();
	afx_msg void OnCreateAndForAll();
	afx_msg void OnCreateOrForAll();
	afx_msg void OnDeltaposSpinCamModeScrollSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckInventory1();
	afx_msg void OnBnClickedCheckInventory2();
	afx_msg void OnBnClickedCheckInventory3();
	afx_msg void OnDeltaposSpinSoundDelay(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinTextDelay(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckRelativeCa();
	afx_msg void OnCbnSelchangeComboObjects();

	void OnCanDropReq(NMHDR* phdr, LRESULT* pResult);
	void OnCanDragReq(NMHDR* phdr, LRESULT* pResult);
	void OnDropReq(NMHDR* phdr, LRESULT* pResult);

	//! Загрузить группы
	bool LoadGroups();
	//! Условия объекта в дерево
	void ObjConditionsToTree();
	//! Дерево условий в объект
	void TreeToCO();
	//! Загружает условия
	void LoadConditions();
	
	//! Показать диалог соответствующий условию
	void ShowCondDlg(DWORD id);

	//! Загружает значение  для StateCondDlg::m_iCondLog
	void LoadCondMode();
	//! Загружает режимы камеры
	void LoadCameraModes();
	//! Определяет можно ли сделать кнопку "Добавить состояние" доступной
	void SetButtonsState();
	
	//! загружает в состояние данные, которые не относятся к условиям
	void wnd2state();
	//! загружает из состояний данные, которые не относятся к условиям
	void state2wnd();
	//! можно ли сделать доступной галочку "Инвентори"
	bool CanEnableInventory();
	//! Можно ли сделать доступной галочку "В инвентори по окончании"
	bool CanEnableMoveToInv();
	//! Управляет доступностью котролов, задающих параметры режима камеры
	void AdjustCamModeWorkTimeControlAccs();
	//! Запрещает доступ к куче контролов, если состояние - курсор мыши
	void SetupAccessToCtrls();
	//! Является ли состояние курсором мыши
	bool IsMouseCursor();
	//! Можно ли добавить условие
	bool CanAdd();
	//! Определяет досутпность контролов, управляещих продолжительностью работы состояния
	void SetAccessToStateDuration();

	//! назначает/убирает флаг в зависимости от тестого значения
	void testAndApplyFlag(qdNamedObject* ptrObj, BOOL bTestVal, int iFlag);
	//! Управляет доступностью контролов, выставляющих флаги связанные с инвентори
	void EnableInventoryOptions(BOOL bEnable);

	//! getter for m_bAddConditionOnClose
	bool getAddConditionOnClose() const{
		return m_bAddConditionOnClose;
	}
	//! setter for m_bAddConditionOnClose
	void setAddConditionOnClose(bool b){
		m_bAddConditionOnClose = b;
	}
	//! Загружает список объектов, которые можно использовать как относительные для КА
	void loadCARelativeObjects(CCJFlatComboBox& box, qdNamedObject const* exclude);
private:
	//! Менеджер диалогов условий
	CondDlgMgr m_CDMgr;
	//! Переносит условия из объекта в дерево и обратно
	CondsMgr m_CondsMgr;
	//! Копия условия, в которую складываются все изменения
	std::auto_ptr<qdGameObjectState> m_qdState;
	//! Исходное состояние
	qdGameObjectState* m_ptrSourceState;
	qdGameDispatcher*	m_ptrGameDisp;

	//! были ли сделаны изменения
	bool m_bChanged;

	//! Флаг состояние для инвентори
	BOOL m_bInventory;
	//! Флаг по окончании засунуть в инвентори
	BOOL m_bMoveToInventory;
	//! Флаг синхронизировать со звуком
	BOOL m_bSyncWithSound;
	//! QD_OBJ_STATE_FLAG_ENABLE_INTERRUPT флаг состояния
	BOOL m_bEnableInterrupt;
	//! Список режимов работы камеры
	CCJFlatComboBox m_wndCamModes;
	//! Время работы режима камеры
	float m_fCamModeWorkTime;

	//! Объект управляющий списком режимов работы камеры
	CamModeRep m_cam_mode_rep;
	//! Нельзя прервать подход
	BOOL m_bDisableInterruptWalk;
	//! Задаем скорость скроллирования
	float m_fCamScrollSpeed;
	//! Дистанция скроллирования
	int m_iCamScrollDistance;
	//! Мягкое преключение qdCameraMode::smooth_switch
	BOOL m_bCamSmoothSwitch;
	//! qdCameraMode::set_center_offset
	Vect2i m_vCamModeCameraOffset;

	//! Флаг: когда объект взят на мышь
	BOOL m_bMouseState;
	//! Флаг: когда мышь над объектом в инвентори
	BOOL m_bMouseHoverState;
	//! Флаг: Не вынимать из инвентори, когда взят на мышь
	BOOL m_bStayInInventory;
	
	//! Задержка звука при старте
	float m_fSoundDelay;

	//!Флаг:  Прерывать состояние по клику
	BOOL m_bInterruptStateByClick;

	//! Задержка перед появлением на экране текста
	float textDelay_;
	//! Флаг состояния QD_OBJ_STATE_FLAG_MOVE_TO_ZONE
	BOOL moveToZone_;
	
	//! Флаг состояния QD_OBJ_STATE_FLAG_MOVE_ON_OBJECT
	BOOL moveOnObject_;
	//! Флаг координатной анимации QD_COORDS_ANM_RELATIVE_FLAG
	BOOL relativeCA_;
	//! Список объектов, относительно которых может работать коородинатная анимация
	CCJFlatComboBox caRelativeObjects_;
	//!  флаг для активации персонажа по окончании работы 
	//! состояния (qdGameObjectState::QD_OBJ_STATE_FLAG_ACTIVATE_PERSONAGE)
	BOOL activatePersonage_;

	//! Нужно ли добавить выбранное условие при закрытии диалога
	bool m_bAddConditionOnClose;
};

//! Показыает диалог условия для состояния
bool ShowCondDlg(qdGameObjectState* ptrState, qdGameDispatcher* ptrDisp);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_STATECONDDLG_H__4D8E05C7_562A_4572_823F_6C419CD3654A__INCLUDED_)
