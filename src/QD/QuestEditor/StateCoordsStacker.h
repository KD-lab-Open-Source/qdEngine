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

const int MAX_ANIM_FRAMES = 1000;///< ������������ ���������� ������� � ��������

/// ������ ��������
enum STACK_MODE {
	SM_OBJ_COORDINATE, ///< ���������� �������
	SM_STATE_OFFSET, ///<  �������� ���������
	SM_STATE_BOUND, ///<  ������� ���������
	SM_STATE_SCALE,///< ������� ���������
	SM_BIG = 1000000
};

/// ��� ����� �������� ����������� � ������ ������� ��� ����� ������� � �����������
enum eWHAT_SHIFT{ WS_IMAGE, WS_CENTER};

/**
	������������ �������� ���������
 */
class StateCoordsStacker : public CDialog
{
// Construction
	/// ���� �� ������������� �������
	bool m_bInitDialog;

	/// ��������� ����� ��� ������� �������
	Vect3f m_vObjPosBak;
	/// ���������� ��������� ��������� �����
	int m_eOldViewState;
	/// ��������� ����� ���������
	std::auto_ptr<qdGameObjectState> m_ptrStateBak;

	/// �������� �����
	ISceneViewFacade* m_ptrView;
	///  ��������� ���������
	qdGameObjectState *m_ptrState;
	/// ���������, � ������� ������� - ��������(�������� �����)
	qdGameObjectState *m_ptrSelectedState;
	/// ��������� �������� ���������, �� �������� ���������� ��������(�������� �����)
	qdAnimationInfo* m_ptrBackAnimationInfo;

	/// ��������� �������� ���������� ���������(�� ��� �������� ������)
	qdAnimationInfo* m_ptrCurStateAI;

	///  ����������� ��� �������� �������� �� �������
	qdAnimationInfo const*const NO_EXCLUDE;

	/// ��������� �������� �������� �������� �������������� ��������� ���������� ��� ���
	qdGameObjectStateWalk::OffsetType editedStateAIType;
	/// ��������� �������� �������� �������� ���������-������� ���������� ��� ���
	qdGameObjectStateWalk::OffsetType backObjStateAIType;

public:
	StateCoordsStacker(CWnd* pParent = NULL);   // standard constructor
	enum { IDD = IDD_DLG_STACK_STATE_COORD };
	
	/// ���������� ��������� ���������
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
	//! \brief ��������� ������� ������ ��� ������� �������� � ����� ������������ 
	//! �������� ��� ���������-��������
	int		m_iObjStateAnimFrame;
	//! \brief ��������� ������� ������ ��� ������� �������� � ����� ������������ 
	//! �������� ��� �������������� ���������
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

	
	/// \brief ��������� ��������� �������, 
	/// �������� ��������� ���������, ���� ��� ���������
	void LoadObjectStates(qdGameObjectAnimated* ptrObj);

	/// ��������� ��������� ���� �������� �� �����
	void LoadObjectsStates();
	
	/// ��������� ��������, ������������ ����������
	/**
		��� ������� ��� ���������� �� ���� ������������. 
		��� ������������ ������ ���� ��������
	 */
	void LoadStateAnimation(CCJFlatComboBox& wnd, qdGameObjectState const* ptrState);
	/// ��������������� ��� LoadStateAnimation. ��������� �������� ������������ ���������
	void LoadStaticStateAnimation(CCJFlatComboBox& wnd, 
								qdGameObjectStateStatic const* ptrState);

	/// ��������������� ��� LoadStateAnimation. ��������� �������� �������
	void LoadWalkStateAnimation(CCJFlatComboBox& wnd, 
					qdGameObjectStateWalk const* ptrState, 
					qdAnimationInfo const* exclude);

	/// ��������� ��������� � ����
	/**
		��������� ����������, ������ �� ����, ��� �������
	 */
	void state2wnd();
	/// ������ �� ���� � ���������
	/**
		������ ����������, ������ �� ����, ��� �������
	 */
	void wnd2state();
	
	/// \brief �������� ������� ������� � �� ���������
	/// ��� ����, ����� ���� ����������� ������������
	void real2bak();
	/// ��������������� �� ��������� ����� �������� ��������
	void bak2real();
	
	/// ������������ �������� ��������� �� ������.
	void state2begin(qdGameObjectState* ptrState);

	///\brief ��������� �������� � ��������� ���������� ������� ������� ��������,
	/// �����, ������� ������
	void UpdateObjAnimControls();
	/// \brief ��������� �������� � m_wndCurStateAnim 
	/// � ���������� m_strStaticEditedStateCurFrame
	void UpdateEditedStateAnimControls();

	/// ��������� ��������� ����, � ������� �������� �����
	void UpdateSceneViewState();
	/// ��������� ������ ��� ��������� ������������ �������.
	/**
		�������� ������� ���������� ������, � ����������� �� ����, ��� �������
		(�������, ������� � �.�.) 
	 */
	void UpdatePosControls();
	/// \brief ��������� ��������� � ���������� ��������� 
	/// m_ptrBackAnimationInfo(��������-��������)
	void UpdateAnimation();
	/// \brief ��������� ��������� � ���������� ��������� 
	/// m_ptrCurStateAI(�� ��� ���������)
	void UpdateEditedStateAnimation();

	/// \brief ������ ����� �� �������� �������� �������� ��� ���� 
	/// m_ptrSelectedState � m_ptrState
	void UpdateModeCtrls();
	/// ������������ �������
	void scale_gait(qdGameObjectState* ptrState, float xs, float ys);
	
	/// ����������/�������� IDC_CHECK_TOGETHER, IDC_SPIN_Z, IDC_Z_EDIT, IDC_STATIC_Z
	void ShowSyncControls(UINT nCmd);

	/// \brief ��������� �������� � ���������, ���������������� ��� ������������� �� ������
	/// ��������, ����������� � ������������ ��������
	void UpdateCAAnimScroller(CSliderCtrl& wnd, CStatic& wnd_static,
								qdGameObjectState* pstate, 
								int *piCurFrame);
	/// �������� �� ��������� ��������� �������� ����
	bool IsMouseCursor();
	/// ��������� ������������ ������ ������������ ����� ������
	void WhatShiftEnable(BOOL bEnable);
	/// ��������� ����� �� ���� ������ � ������� ������������� ����� ������
	bool PermitAccess2WhatShift();
	/// \brief ��������� ������ � ��������� ��������� ��� ���������-��������
	void UpdateSelectedStateCtrls();
	/// ������ �� ������
	void UpdateSelectedStateCtrlsAccess();
public:
	/// ��������� �������� �����
	void SetView(ISceneViewFacade* ptrView);
	/// ��������� ��������� ���� ���������� ��������������
	enum eEndEditMode{
		EEM_DISCARD,///< �������� ���������
		EEM_SAVE///< ��������� ���������
	};
	/// ���������� ��������
	void EndEdit(eEndEditMode mode);
	/// ��������� ��������� ���������
	void set_state(qdGameObjectState* ptrState);

	/// ��������� � ��������� ���� � ��������� ���������
	void UpdateStateInfo();

	/// ���������� ���� �������� ���������� ���������
	float get_edited_state_anim_phase() const{
		return (static_cast<float>(m_iEditedStateAnimFrame)/MAX_ANIM_FRAMES);
	}
	/// ���������� ���� �������� ��������� - ��������
	float get_obj_state_anim_phase() const {
		return (static_cast<float>(m_iObjStateAnimFrame)/MAX_ANIM_FRAMES);
	}

	/// ���������� ���� �� �������� ��������� - ��������
	qdAnimationInfo* get_back_obj_state_ai() const{
		return m_ptrBackAnimationInfo;
	}
	/// ���������� ��������� - ��������
	qdGameObjectState* get_back_obj_state(){
		return m_ptrSelectedState;
	}

	/// ���������� ���� �� �������� ���������� ���������
	qdAnimationInfo* get_edited_state_ai() const{
		return m_ptrCurStateAI;
	}
	/// �������� �� m_ptrCurStateAI ����������
	qdGameObjectStateWalk::OffsetType edited_state_type() const{
		return editedStateAIType;
	}
	/// �������� �� m_ptrBackAnimationInfo ����������
	qdGameObjectStateWalk::OffsetType back_obj_state_type() const{
		return backObjStateAIType;
	}
	/// ����� ������������� �������� � ������� 
	int edited_state_ai_num() const;
	/// ����� � ������� �������� ���������-��������
	int back_obj_state_ai_num() const;

	/// ���������� ����� ��������
	STACK_MODE get_stack_mode() const {
		return static_cast<STACK_MODE>(m_iStackMode);
	}

	/// ���������� ������, �������� ����������� ��������� - �������
	qdGameObjectAnimated* get_back_obj() const;

	/// ���������� ����� ������ �����������
	eWHAT_SHIFT get_shift() const{
		return static_cast<eWHAT_SHIFT>(m_iWhatShift);
	}
	/// ��������� ������������ ��������� ������� ��� ���������� ���������
	void EnablePos(const bool b);
private:
	/**
		������ ���������� ��������� deley ���������.
		� ������������ ��������� � ����������� ���������������.
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
	//! ���� true, �� �������� ������ �� � � �� � ������������
	BOOL m_bSyncScale;
	//! ����������� �������� �������������� ���������
	CSliderCtrl m_wndEditedStateCAScroller;

	/// ����� �������� ����� ��������
	int m_iEditStateCurFrameNum;
	/// ����� �������� ����� ��������
	int m_iSelectedStateCurFrameNum;
	CStatic m_wndScrollStatic;
	//! ������� ��� ���������, ������� ������������� ����������� ��������
	CStatic m_wndScrollStatic2;
	//! �������, ������� ������������� ����� � ����������� ��
	CSliderCtrl m_wndSelStateCAScroller;
	//! ����-�������, ������������ �������� �
	CSpinButtonCtrl m_wndSpinX;
	//! ����-�������, ������������ �������� �
	CSpinButtonCtrl m_wndSpinY;

	activation_delay_guard m_adg;//! ��������� ��� �������������� ���������
	activation_delay_guard m_adg2;//! ��������� ��� ���������, � �������� �������
	//! ��� ���������(���� � ����������� ��� ����������� � ������)
	int m_iWhatShift;
	/// ������� ���������
	bool m_bMastabChanged;
	/// ������� �, ������, �������� �� � ��� ���������-��������
	int m_iXSelState;
	/// ������� Y, ������, �������� �� Y ��� ���������-��������
	int m_iYSelState;
	/// ������� Z, ������ ��� ���������-��������
	int m_iZSelState;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_STATECOORDSSTACKER_H__842FFD2A_D8CA_4EC2_B5FD_8C160739B8E8__INCLUDED_)
