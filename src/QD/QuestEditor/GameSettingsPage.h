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
    �������� ������� ����
*/

class GameSettingsPage : public CBasePage
{
	DECLARE_DYNCREATE(GameSettingsPage);

	//! ��������� �� ���������� ���������
	qdGameDispatcher* m_ptrGameDisp;
	//! ����� �������� � ������ �������
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

	//! ��������� ��������� ����
	bool set_data(qdGameDispatcher* ptrDisp);
	//! ��������� ����� �������� � ������
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
	//! ��������� ������ ��������� ���������� ������
	bool LoadResolutions();
	//! ������ �� ������
	bool LoadCameraModes();
	//! ��������� ���������� (������������/�����������) ������ "�����" "������"
	bool UpdateWizardBtnsState();
	//! ��������� ������ ��������� ������������ �������
	bool LoadInterfaceScreens(CCJFlatComboBox& box);

	//! ��������� ������ ����
	bool LoadScenes();
	//! ��������� � ���������� ��������� �����
	void SetupStartupScene();
	//! ��������� ������������ �����, �� ������� ����������� �������� ����
	void SetupMainMenu();
	//! ��������� ������������� ������������ ����� � ���������
	void SetupIngameInvScr();//! � ���������
	//! ��������� ������������� ������������ ����� ��� ���������
	void SetupIngameScr();//! ��� ���������
	//! ��������� �����, ���������� ��� ������� �� ���������� ����������� ����
    void SetupSavePromptScr();
	void InitFontBox(CCJFlatComboBox& box);
	void LoadFonts();
//	void SetDefaultFont();
	//! ��������� ������ ������� �� ��������� ������������ � �������� �������
	void textOutputDefault2wnd();
	//! ������ � ������ ������ �� ��������� ����������� � ��������� �� ���������
	void wnd2textOutputDefault();

	//! ��������� ��������, ����������� ����� ������� �����, � ���������� � ��� ������������
	void toggleSaveTimeComponents(bool activate);
	void toggleSaveTitleComponents(bool activate);
private:
	/*!
	    �������. ��������� � ����� � ���������
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
	//! �������� �������������� ������ � �������� � �������
	int m_iScrollSpeed;
	//! �� ����� ���������� �� ������ ������� �� ���� ���� ���������� ��������������
	int m_iScroolDistance;

	//! ���������� �������� ��� ������, 0 - ���������, 9 - ������������
	int compression_;

	//! ������ ������� ��������, 0 ���� ����.
	int hallOfFameSize_;

	CCJFlatComboBox m_wndScenes;
	CCJFlatComboBox m_wndScreens;
	//! ����� � ���������
	CCJFlatComboBox m_wndScrn4Inv;
	//! ����� ��� ���������
	CCJFlatComboBox m_wndScrnWithoutInv;

	//! �����, ������������ ��� ������� ������������� ���������� �����
	CCJFlatComboBox m_wndSavePromptScrn;
	/// ����� ����� ����� �����
	CCJFlatComboBox saveTitleScreen;

	//! ����� �� �������� ��� ����������� �����
	BOOL m_boolNeedSaveScreenshot;
	//! ����� �� �������� ����� � ���� �����
	CButton m_wndNeedSaveTimeShow;
	CButton m_wndNeedSaveTitleShow;
	//! �����, ������� ��������� ����� ����� (� �.�. ����� � ���� �����)
	CCJFlatComboBox m_wndSaveFont;

	CString m_strGameName;
	CString m_strDBFileName;
	CString m_strSerialKey;
	//! qdInterfaceDispatcher::set_scene_redraw
	BOOL needSceneRedraw_;

	//! ������ ������ ������
	BOOL textHorizontal_;
	//! ������� ������ ����� ������
	CCJColorPicker textColorWnd_;
	//! ������� ������ hover color
	CCJColorPicker hiliteColorWnd_;
	//! ������ ������
	CCJFlatComboBox fontsWnd_;
	//! ������������ ������
	CCJFlatComboBox alignWnd_;

	//! ������ ������ ������
	BOOL topicTextHorizontal_;
	//! ������� ������ ����� ������
	CCJColorPicker topicTextColorWnd_;
	//! ������� ������ hover color
	CCJColorPicker topicHiliteColorWnd_;
	//! ������� ������ ������ �����
	CCJColorPicker saveFontColorWnd_;
	//! ������ ������
	CCJFlatComboBox topicFontsWnd_;
	//! ������������ ������
	CCJFlatComboBox topicAlignWnd_;
};



#endif // __WPGGLOBSET_H__
