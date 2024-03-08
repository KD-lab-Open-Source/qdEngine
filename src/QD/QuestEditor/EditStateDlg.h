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

//���� ���������
const int ST_NORMAL_STATE	= 0;///< ������� ���������
const int ST_CA_STATE		= 1;///< ��������� � ��

class EditStateDlg : public CDialog, public IRequestingStateEdit
{
	/// �������� ���������
	qdGameObjectAnimated* m_ptrObj;
	/// ��������� �������
	qdGameDispatcher* m_ptrDisp;
	/// �������� ���������
	qdGameObjectState* m_ptrState;
	/// ����� ��� ��������������
	std::auto_ptr<qdGameObjectState> m_ptrStateBak;

	/// ������ ��������
	std::auto_ptr<CStringSet_t> m_ptrAnimSet;
	/// ������ �������
	std::auto_ptr<CStringSet_t> m_ptrPersGaintsSet;
	/// ������ ������
	std::auto_ptr<CStringSet_t> m_ptrSoundSet;

	bool m_bInitDialog;
	bool m_bEditCoordAnim;///< ��������� ������������� ����� �� ������� ���������
	bool m_bUseCoordAnim;///< ��������� ������������ ������������ ��������
	bool m_bUseStacking;///< ��������� ������������ ��������
	bool m_bUseBound;///< ��������� �������������� ������
	HWND m_hOwnedWnd;///< ����, ������� ��������� ������
	CCJFlatComboBox m_wndCursors;
	/// ����������� �� ����
	BOOL m_bSoundLoop;
// Construction
public:
	EditStateDlg(HWND hOwnedWnd = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(EditStateDlg)
	enum { IDD = IDD_DLG_STATE_PROP };
	/// ������ �������� ��� ������� � ����������� �� ���� ���������
	CCJFlatComboBox	m_wndAnim;
	/// ��������� �� ��������
	BOOL	m_bLoop;
	/// ����������� �� ��������
	BOOL	m_bFlipVert;
	/// ����������� �� ��������
	BOOL	m_bFlipHorz;
	/// ����� ��������� � ����� ��������
	int		m_iAnimation;
	/// ����� ���������� � ����� �����
	int		m_iSound;
	/// ��-����� ��� ����� �� �����. ���-�� �� ����� ����� ��� �����
	BOOL	m_bSetPoints;
	/// ������� �������� �� ��
	int		m_iSpeed;
	/// QD_COORDS_ANM_OBJECT_START_FLAG
	BOOL	m_bGo2First;
	/// ��������� � ������������� ��������� ��� ���
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

	/// ��������� ��������� � ������ ��������
	bool set_data(qdGameDispatcher* ptrDisp, qdGameObjectAnimated* ptrObj);
	/// ��������� ������������� ���������
	bool set_state(qdGameObjectState* ptrState);

	/// ���������� ������������� ���������
	qdGameObjectState* GetState();
	/// ����������, ����� �������� ���������
	void EndStackState();
	/// ����������, ����� �������������� �� ���������
	void EndEditCoordAnimation();
	/// ����������, ����� �������������� �������� ���������
	void EndEditStateDims();


	/// ���������� ������������ �������� ���������
	qdCoordsAnimation& GetCoordsAnimation();

	/// ��������� ��������
	void suppress_stacking(){
		m_bUseStacking = false;
	}
	/// ��������� �������������� ��������
	void suppress_bound(){
		m_bUseBound = false;
	}

	/// ��������� ����� ������������� �� � �������� �, ���� ���� ������������
	void suppress_coord_anim(){
		m_bUseCoordAnim = false;
		m_bEditCoordAnim = false;
	}
	/// ��������� ������������� ����� �� ��.���������
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
	/// ��������� �������� � �������� �������
	void animation2wnd();
	/// ������ �� �������� �� ��������� ����������� � ���������
	void wnd2animation();

	/// ��������� ������� � �������� �������
	void gaint2wnd();
	/// ������ � ������� �� ��������� ����������� � ���������
	void wnd2gaint();

	/// ��������� ��������� � �������� �������
	void state2wnd();
	/// ������ �� �������� �� ��������� ����������� � ���������
	void wnd2state();

	/// ��������� ������ ������� � �������� �������
	void textOutputParams2wnd();
	/// ������ � ������ ������ �� ��������� ����������� � ���������
	void wnd2textOutputParams();

	/// ��������� ����� � �������� �������
	void sound2state();
	/// ������ � ����� �� ��������� ����������� � ���������
	void state2sound();

	/// ��������� ������ � �������� �������
	void text2wnd();
	/// ������ � ������ �� ��������� ����������� � ���������
	void wnd2text();

	/// ��������� ������� � �������� �������
	void cursor2wnd();
	/// ������ � ������� �� ��������� ����������� � ���������
	void wnd2cursor();

	/// � ����������� �� ���� ��������� ������ ���������� ��� ��� �������� � �������
	void SetupCtrls();
	/// ���������� ����������� �������� ��� �������. ��������� ������ �������
	void SetupCtrls4PersGaints();
	/// \brief ���������� ����������� �������� ��� ��������� � ��. 
	/// ��������� ������ ������� ��� ������ �������� � ����������� �� ���� ���������
	void SetupCtrls4CoordAnim();
	/// \brief ���������� ����������� �������� ��� ������������ ���������. 
	/// ��������� ������ ��������
	void SetupCtrls4Anim();
	/// ���������� ����������� �������� ��� �����
	void SetupCtrls4StateMask();

	/// ��������� m_wndAnim ��������� ��� ���������
	void SetData2AnimCombo(CStringSet_t* ptrSet);
	/// ��������� sounds ������
	void SetData2SoundCombo(CStringSet_t* ptrSet);
	/// ��������� m_wndCursors �������
	void SetData2CursorCombo();
	
	/// ������� ���������� ������� ��������, ������ � ������. ��� ������� 
	/// ������� �� ������� ����� � ���������� �������
	bool MakeSets();
	/// ��������� ������������ ������ "��������� ���������"
	void CheckAccess2StackStateBtn();

	/// ��������� ��� ������� ���������
	void LoadStateResources();
	/// ��������� ��� ������� ���������
	void FreeStateResources();

	/// ����� ������� ����� � box � �������� �� ���� ����������� � �������
	void setTextId2wnd(CCJFlatComboBox& box, LPCTSTR id);
	/// ���������� ��������� � box �����
	CString getIdText(CCJFlatComboBox& box);
	/// ����� �� ��������� �������
	bool hasCondition(qdGameObjectState const* state) const;

	/// ���������� ���� ������
	COLORREF getTextColor() const;
	/// ���������� hover color
	COLORREF getHiliteColor() const;
	/// ���������� ��� �����
	int getFontType() const;

	/// ��������� ������ � ����������
	void loadFonts();
	
	/// ���������� ��������� ����
	qdSound* getSound();
	/// ���������� ��������� ��������
	qdAnimation* getAnimation();

	/** ���������� � ������������ ��������, ���������� �� ������ � ����������� ��
	    ������ - ������������ ���������� ��������� ��� ��� */
	void toggleFormatSettings();
private:
	/// ��������� ������ ��������������� ����� � ��������� ����
	void LoadTextIds(CCJFlatComboBox& box);
	/// ��������� ������ � �����
	class insert_string;

	int autosaveMode();

private:
	/// ��������� �����
	int m_iSoundVolume;

	//! ������������� ��������� � ������ ��������
	BOOL m_bLoadToMemory;
	/// ������ ��������������� ��� ������ �������� �����
	CCJFlatComboBox m_wndShortPhraseIds;
	/// ������ ��������������� ��� ������ ������� �����
	CCJFlatComboBox m_wndLongPhraseIds;
	/// ����� �������� �����
	CString m_strShortText;
	/// ����� ������� �����
	CString m_strFullText;
	//! ������ ������ ������
	BOOL textArrangement_;
	/// ������������� �������� �����
	CString shortPhraseID_;
	/// ������������� ������� �����
	CString longPhraseID_;

	/// ������ ������
	CCJFlatComboBox sounds;
	/// ������� ������ ����� ������
	CCJColorPicker textColorWnd_;
	/// ������� ������ hover color
	CCJColorPicker hiliteColorWnd_;
	/// ������ ������
	CCJFlatComboBox fontsWnd_;

	CCJFlatComboBox autosaveMode_;
	int autosaveSlot_;

	BOOL isGlobal_;
	int fadeMode_;
	float fadeTime_;

	int shadowAlpha_;
	CCJColorPicker shadowColorWnd_;

	//! ������������ ������
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
