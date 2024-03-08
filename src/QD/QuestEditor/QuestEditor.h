// QuestEditor.h : main header file for the QUESTEDITOR application
//

#if !defined(AFX_QUESTEDITOR_H__6CF873A4_4A1A_4409_8CD9_F7C8A02D5DF7__INCLUDED_)
#define AFX_QUESTEDITOR_H__6CF873A4_4A1A_4409_8CD9_F7C8A02D5DF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CQuestEditorApp:
// See QuestEditor.cpp for the implementation of this class
//

//������������� ���� ��������� ������� � true
void SetProjChange();
#include <vector>
#include <map>

#include "IniManager.h"

typedef std::vector<CString> CStringVect_t;

class reg_setting;
class Setting;

/// ����������, ����� �� ����� �� ������� ������� ��������� ������������� ��
#define WM_SET_COORD_ANIMATION_MODE		(WM_APP+1)
/// ����������, ����� �� ����� �� ������� ������� ��������� ���������
#define WM_STACK_STATE								(WM_APP+2)
/// ����������, ����� �� ����� �� ������� ������� ��������� ������ �������
#define WM_STATE_DIMS									(WM_APP+3)

/// ��������� ������ ���������, ����� ������ ���� ������
#define SETTING_NAME(_NAME) (IDS_SETTING_NAME_##_NAME)
/// ��������� ������ ���������, ����� ������ ���� ������
#define REG_VAL_NAME(_NAME) (IDS_REG_VAL_NAME_##_NAME)

interface IIdleProcessor;

class QEDocTemplate;
/// ����� ����������
class CQuestEditorApp : public CWinApp
{
	typedef std::map<CString, reg_setting> SettingsMap_t;
	/// ������ ����������, ������� �������� � �������
	SettingsMap_t m_Settings;

	/// ��������� ��������� ���������� �� ���-�����
	IniManager   m_IniMngr;
	/// ��������� �� ����������� ������� ���������
	QEDocTemplate* m_pDocTemplate;
	/// ���������� ��������� �������
	IIdleProcessor* idleProcessor_;
public:
	CQuestEditorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuestEditorApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	//}}AFX_VIRTUAL

// Implementation

	/// �� ����� ��������� ������� ������������� ��������
	int GetIntSetting(const CString& strSettingName);
	/// �� id ��������� ������ � ������ ��������� ������� ������������� ��������
	int GetIntSetting(UINT uiSettingName);
	
	/// �� ����� ��������� ������� ������
	const CString GetStrSetting(const CString& strSettingName);
	/// �� id ��������� ������ � ������ ��������� ������� ������
	const CString GetStrSetting(UINT uiSettingName);

	/// �������� �������� �������������� ���������
	bool WriteIntSetting(const CString& strSettingName, int Val);
	/// �������� �������� �������������� ���������
	bool WriteIntSetting(UINT uiSettingName, int Val);

	/// �������� �������� ���������� ���������
	bool WriteStrSetting(const CString& strSettingName, CString Val);
	/// �������� �������� ���������� ���������
	bool WriteStrSetting(UINT uiSettingName, CString Val);

	/// ���������� ��� ��� �����
	const CString GetProjIniFileName();

	interface IProjectSettings* GetProjectSettings() ;
	/// ������������� ��� ��� ����� � m_IniMngr
	bool ResetProjectSettingsMgr(const CString& strIniName);

	/// ��������� ��������� ��������� �������
	void setIdleProcessor(IIdleProcessor* project);

protected:
	/// ���������� ��������� ��������� �������
	IIdleProcessor* getIdleProcessor();
protected:
	//{{AFX_MSG(CQuestEditorApp)
	afx_msg void OnAppAbout();
	afx_msg void OnCrtProj();
	afx_msg void OnMakeDirStruct();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, 
		void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnIdle(LONG lCount);

private:
	/// ������� �������� � ������ ����������
	Setting& GetSetting(const CString& strSettingName);
	/// ������� �������� � ������ ����������, �������� ��� �� ��������
	Setting& GetSetting(UINT uiSettingName);

	/// �������� � ������ ������ ��� ��������� ����������
	void make_settings_list();

	/// ��������� �� ������� ��� ��������� ����������
	void load_settings_list();
	/// ���������� � ������ ��� ��������� ����������
	void save_settings_list();

	/// ��������� ��������� �������� � ������ �������� ����������
	bool add_string_setting(UINT uiSettingName, const CString& strSecName, UINT uiValueName, CString strDefVal = CString());
	/// ��������� ������������� �������� � ������ �������� ����������
	bool add_int_setting(UINT uiSettingName, const CString& strSecName, UINT uiValueName, int iDefVal = 0);
};

CQuestEditorApp* GetMyApp();

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUESTEDITOR_H__6CF873A4_4A1A_4409_8CD9_F7C8A02D5DF7__INCLUDED_)
