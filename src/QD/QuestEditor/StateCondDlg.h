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
	������ ������� ��� ���������
 */
class StateCondDlg : 
	private boost::base_from_member<CMyTree>///< �����, ����� ������ ��������������������� ������ ������
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
	//! ������ �������� �������, �� ����������� � �����-���� ������
	int		m_iCondLog;

	//! ���� ������������ ���������� ���������
	BOOL	m_bRestorePrev;
	//! ���� ������ ������ �� ���������
	BOOL	m_bHideObj;
	//! ���� ��������� � ������ ����� ������������ ��������
	BOOL	m_bCAGo2First;
	//! �������� �� ������
	float	m_fStartDelay;
	//! ����� ���������� ���������
	float	m_fStateDuration;
	//}}AFX_DATA
public:
	// ������� ������ MakeCondDlgBase
	//! ���������� �����, � ������� ����������� �������
	class qdGameScene* get_scene() const;
	//! ���������� ��������� �� ��������� ����
	class qdGameDispatcher* get_disp() const{
		return m_ptrGameDisp;
	};
	void OnCondDataComplete(bool bComplete);

	//! ���� �� ���������
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
	//! ��������� ������� �������. ���������� ��������� ���� ���������
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

	//! ��������� ������
	bool LoadGroups();
	//! ������� ������� � ������
	void ObjConditionsToTree();
	//! ������ ������� � ������
	void TreeToCO();
	//! ��������� �������
	void LoadConditions();
	
	//! �������� ������ ��������������� �������
	void ShowCondDlg(DWORD id);

	//! ��������� ��������  ��� StateCondDlg::m_iCondLog
	void LoadCondMode();
	//! ��������� ������ ������
	void LoadCameraModes();
	//! ���������� ����� �� ������� ������ "�������� ���������" ���������
	void SetButtonsState();
	
	//! ��������� � ��������� ������, ������� �� ��������� � ��������
	void wnd2state();
	//! ��������� �� ��������� ������, ������� �� ��������� � ��������
	void state2wnd();
	//! ����� �� ������� ��������� ������� "���������"
	bool CanEnableInventory();
	//! ����� �� ������� ��������� ������� "� ��������� �� ���������"
	bool CanEnableMoveToInv();
	//! ��������� ������������ ��������, �������� ��������� ������ ������
	void AdjustCamModeWorkTimeControlAccs();
	//! ��������� ������ � ���� ���������, ���� ��������� - ������ ����
	void SetupAccessToCtrls();
	//! �������� �� ��������� �������� ����
	bool IsMouseCursor();
	//! ����� �� �������� �������
	bool CanAdd();
	//! ���������� ����������� ���������, ����������� ������������������ ������ ���������
	void SetAccessToStateDuration();

	//! ���������/������� ���� � ����������� �� ������� ��������
	void testAndApplyFlag(qdNamedObject* ptrObj, BOOL bTestVal, int iFlag);
	//! ��������� ������������ ���������, ������������ ����� ��������� � ���������
	void EnableInventoryOptions(BOOL bEnable);

	//! getter for m_bAddConditionOnClose
	bool getAddConditionOnClose() const{
		return m_bAddConditionOnClose;
	}
	//! setter for m_bAddConditionOnClose
	void setAddConditionOnClose(bool b){
		m_bAddConditionOnClose = b;
	}
	//! ��������� ������ ��������, ������� ����� ������������ ��� ������������� ��� ��
	void loadCARelativeObjects(CCJFlatComboBox& box, qdNamedObject const* exclude);
private:
	//! �������� �������� �������
	CondDlgMgr m_CDMgr;
	//! ��������� ������� �� ������� � ������ � �������
	CondsMgr m_CondsMgr;
	//! ����� �������, � ������� ������������ ��� ���������
	std::auto_ptr<qdGameObjectState> m_qdState;
	//! �������� ���������
	qdGameObjectState* m_ptrSourceState;
	qdGameDispatcher*	m_ptrGameDisp;

	//! ���� �� ������� ���������
	bool m_bChanged;

	//! ���� ��������� ��� ���������
	BOOL m_bInventory;
	//! ���� �� ��������� �������� � ���������
	BOOL m_bMoveToInventory;
	//! ���� ���������������� �� ������
	BOOL m_bSyncWithSound;
	//! QD_OBJ_STATE_FLAG_ENABLE_INTERRUPT ���� ���������
	BOOL m_bEnableInterrupt;
	//! ������ ������� ������ ������
	CCJFlatComboBox m_wndCamModes;
	//! ����� ������ ������ ������
	float m_fCamModeWorkTime;

	//! ������ ����������� ������� ������� ������ ������
	CamModeRep m_cam_mode_rep;
	//! ������ �������� ������
	BOOL m_bDisableInterruptWalk;
	//! ������ �������� ��������������
	float m_fCamScrollSpeed;
	//! ��������� ��������������
	int m_iCamScrollDistance;
	//! ������ ����������� qdCameraMode::smooth_switch
	BOOL m_bCamSmoothSwitch;
	//! qdCameraMode::set_center_offset
	Vect2i m_vCamModeCameraOffset;

	//! ����: ����� ������ ���� �� ����
	BOOL m_bMouseState;
	//! ����: ����� ���� ��� �������� � ���������
	BOOL m_bMouseHoverState;
	//! ����: �� �������� �� ���������, ����� ���� �� ����
	BOOL m_bStayInInventory;
	
	//! �������� ����� ��� ������
	float m_fSoundDelay;

	//!����:  ��������� ��������� �� �����
	BOOL m_bInterruptStateByClick;

	//! �������� ����� ���������� �� ������ ������
	float textDelay_;
	//! ���� ��������� QD_OBJ_STATE_FLAG_MOVE_TO_ZONE
	BOOL moveToZone_;
	
	//! ���� ��������� QD_OBJ_STATE_FLAG_MOVE_ON_OBJECT
	BOOL moveOnObject_;
	//! ���� ������������ �������� QD_COORDS_ANM_RELATIVE_FLAG
	BOOL relativeCA_;
	//! ������ ��������, ������������ ������� ����� �������� ������������� ��������
	CCJFlatComboBox caRelativeObjects_;
	//!  ���� ��� ��������� ��������� �� ��������� ������ 
	//! ��������� (qdGameObjectState::QD_OBJ_STATE_FLAG_ACTIVATE_PERSONAGE)
	BOOL activatePersonage_;

	//! ����� �� �������� ��������� ������� ��� �������� �������
	bool m_bAddConditionOnClose;
};

//! ��������� ������ ������� ��� ���������
bool ShowCondDlg(qdGameObjectState* ptrState, qdGameDispatcher* ptrDisp);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_STATECONDDLG_H__4D8E05C7_562A_4572_823F_6C419CD3654A__INCLUDED_)
