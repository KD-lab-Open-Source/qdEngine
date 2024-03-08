#pragma once
#include "qd_game_object_moving.h"
class MovementTypeManager;

// PersonageControlPropPage dialog

#include "BasePage.h"
#include "afxwin.h"

/*!
 Класс-диалога страницы свойств, на которой редаткируются режимы движения персонажа
*/
class PersonageControlPropPage : public CBasePage
{
	DECLARE_DYNAMIC(PersonageControlPropPage)

public:
	PersonageControlPropPage();
	virtual ~PersonageControlPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MOVING_OBJ_CONTROL };

	void set_data(qdGameObjectMoving* ptrObj);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnApply();
	virtual void OnCancel();
	afx_msg void OnDeltaposSpinRadius(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckMouseControl();
	afx_msg void OnBnClickedCheckKeyboardControl();
	afx_msg void OnBnClickedCheckCollisionControl();
	afx_msg void OnDeltaposSpinCollisionPath(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAnyCheck();
	afx_msg void OnCbnSelchangeComboMovementMode();
	afx_msg void OnDeltaposSpinFollowRadiusMin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinFollowRadiusMax(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckFollowActivePersonage();
	afx_msg void OnBnClickedCheckControlAttachment();
	afx_msg void OnDeltaposSpinAttacherOffset(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboAttacher();
	afx_msg void OnBnClickedCheckControlAttachmentToActiveWithMoving();
	afx_msg void OnBnClickedCheckControlAttachmentWithoutDirRel();
	afx_msg void OnBnClickedCheckActiveClickReacting();
	afx_msg void OnBnClickedCheckAnimatedRotation();
	afx_msg void OnDeltaposSpinRotationSpeed(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

	//! Делает резервную копии данных из объекта
	void real2bak();
	//! Восстанавливает данные из резервной копии
	void bak2real();
	//! расскладывает данные из объекта по контролам
	void obj2wnd();
	//! собирает данные из контролов и кладет их в объект
	void wnd2obj();
	//! устанавливает/убирает соответствуюищий флаг
	void reset_control(BOOL bSetFlag, int iSetValue);
	//! Загружает типы движения. НЕ ИСПОЛЬЗУЕТСЯ
	void load_movement_types();
	qdGameObjectStateWalk::movement_type_t get_movement_type()const;

	//! Загружает список пероснажей, исключая тот который редактируем
	void loadPersonages();
	//! Управляет доступностью контролов,задающих за привязку персонажа
	void EnableAttacherControls(BOOL bEnable);
	//! Управляет доступностью контролов, задающих смещение
	void EnableOffsetCtrls(BOOL bEnable);
private:

	qdGameObjectMoving* m_ptrObj;

	//! Управление мышью
	BOOL m_bMouseControl;
	//! Управление клавиатурой
	BOOL m_bKeyboardControl;
	//! Управляется толчками другого персонажа
	BOOL m_bCollisionControl;
	//! Избегать столкновений
	BOOL m_bAvoidCollision;
	//! Автоматически двигать до препятствия
	BOOL m_bAutoMove;

	/// qdGameObjectMoving::collision_radius
	float m_fCollisionRadius;
	/// qdGameObjectMoving::collision_delay
	float m_fCollisionDelay;
	/// qdGameObjectMoving::collision_path
	float m_fCollisionPath;

	//! Для флага CONTROL_CLEAR_PATH
	BOOL clearPath_;
	//! CONTROL_FOLLOW_ACTIVE_PERSONAGE
	BOOL followActivePers_;
	//! CONTROL_REPEAT_ACTIVE_PERSONAGE_MOVEMENT
	BOOL repeatActivePersMovement_;
	/// qdGameObjectMoving::follow_min_radius
	float followRadiusMin_;
	/// qdGameObjectMoving::follow_max_radius
	float followRadiusMax_;

	/// qdGameObjectMoving::attach_shift().x
	int attacherOffsetX_;
	/// qdGameObjectMoving::attach_shift().y
	int attacherOffsetY_;

	//! Список персонажей, к которым можно привязать редактируемый
	CCJFlatComboBox attachers_;
	//! Для флага CONTROL_ATTACHMENT_WITH_DIR_REL
	BOOL controlAttach_;

	//! CONTROL_ATTACHMENT_WITHOUT_DIR_REL
	BOOL controlAttachWithoutDirRel_;
	//! CONTROL_ATTACHMENT_TO_ACTIVE_WITH_MOVING
	BOOL controlAttachToActiveWithMoving_;
	//! CONTROL_ACTIVE_CLICK_REACTING
	BOOL m_bActiveClickReacting;
	//! CONTROL_ANIMATED_ROTATION
	BOOL m_bAnimatedRotation;

	float m_fRotationSpeed;
	// копии. Используются тогда, когда надо откатить изменения

	BOOL m_bMouseControlBak;
	BOOL m_bKeyboardControlBak;
	BOOL m_bCollisionControlBak;
	BOOL m_bAvoidCollisionBak;
	BOOL m_bAutoMoveBak;
	float m_fCollisionRadiusBak;
	float m_fCollisionDelayBak;
	float m_fCollisionPathBak;

	BOOL clearPathBak_;
	BOOL followActivePersBak_;
	BOOL repeatActivePersMovementBak_;
	float followRadiusMinBak_;
	float followRadiusMaxBak_;

	BOOL controlAttachBak_;
	qdGameObjectMoving const* attacherBak_;
	int attacherOffsetXBak_;
	int attacherOffsetYBak_;

	BOOL controlAttachWithoutDirRelBak_;
	BOOL controlAttachToActiveWithMovingBak_;
	
	BOOL m_bActiveClickReactingBak;
	BOOL m_bAnimatedRotationBak;

	float m_fRotationSpeedBak;

	//! Режимы движения персонажа
	CCJFlatComboBox m_wndMovementModes;
	std::auto_ptr<MovementTypeManager> m_ptrMovementTypeManager;
};
