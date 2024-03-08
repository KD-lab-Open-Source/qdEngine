#pragma once
#include "qd_game_object_moving.h"
class MovementTypeManager;

// PersonageControlPropPage dialog

#include "BasePage.h"
#include "afxwin.h"

/*!
 �����-������� �������� �������, �� ������� ������������� ������ �������� ���������
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

	//! ������ ��������� ����� ������ �� �������
	void real2bak();
	//! ��������������� ������ �� ��������� �����
	void bak2real();
	//! ������������� ������ �� ������� �� ���������
	void obj2wnd();
	//! �������� ������ �� ��������� � ������ �� � ������
	void wnd2obj();
	//! �������������/������� ���������������� ����
	void reset_control(BOOL bSetFlag, int iSetValue);
	//! ��������� ���� ��������. �� ������������
	void load_movement_types();
	qdGameObjectStateWalk::movement_type_t get_movement_type()const;

	//! ��������� ������ ����������, �������� ��� ������� �����������
	void loadPersonages();
	//! ��������� ������������ ���������,�������� �� �������� ���������
	void EnableAttacherControls(BOOL bEnable);
	//! ��������� ������������ ���������, �������� ��������
	void EnableOffsetCtrls(BOOL bEnable);
private:

	qdGameObjectMoving* m_ptrObj;

	//! ���������� �����
	BOOL m_bMouseControl;
	//! ���������� �����������
	BOOL m_bKeyboardControl;
	//! ����������� �������� ������� ���������
	BOOL m_bCollisionControl;
	//! �������� ������������
	BOOL m_bAvoidCollision;
	//! ������������� ������� �� �����������
	BOOL m_bAutoMove;

	/// qdGameObjectMoving::collision_radius
	float m_fCollisionRadius;
	/// qdGameObjectMoving::collision_delay
	float m_fCollisionDelay;
	/// qdGameObjectMoving::collision_path
	float m_fCollisionPath;

	//! ��� ����� CONTROL_CLEAR_PATH
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

	//! ������ ����������, � ������� ����� ��������� �������������
	CCJFlatComboBox attachers_;
	//! ��� ����� CONTROL_ATTACHMENT_WITH_DIR_REL
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
	// �����. ������������ �����, ����� ���� �������� ���������

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

	//! ������ �������� ���������
	CCJFlatComboBox m_wndMovementModes;
	std::auto_ptr<MovementTypeManager> m_ptrMovementTypeManager;
};
