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

//устанавливает флаг измениния проекта в true
void SetProjChange();
#include <vector>
#include <map>

#include "IniManager.h"

typedef std::vector<CString> CStringVect_t;

class reg_setting;
class Setting;

/// Посылается, когда мы хотим из диалога свойств состояния редактировать КА
#define WM_SET_COORD_ANIMATION_MODE		(WM_APP+1)
/// Посылается, когда мы хотим из диалога свойств состояния стыковать
#define WM_STACK_STATE								(WM_APP+2)
/// Посылается, когда мы хотим из диалога свойств состояния менять размеры
#define WM_STATE_DIMS									(WM_APP+3)

/// Формирует строку параметра, чтобы писать было меньше
#define SETTING_NAME(_NAME) (IDS_SETTING_NAME_##_NAME)
/// Формирует строку параметра, чтобы писать было меньше
#define REG_VAL_NAME(_NAME) (IDS_REG_VAL_NAME_##_NAME)

interface IIdleProcessor;

class QEDocTemplate;
/// Класс приложения
class CQuestEditorApp : public CWinApp
{
	typedef std::map<CString, reg_setting> SettingsMap_t;
	/// Список параметров, которые хранятся в реестре
	SettingsMap_t m_Settings;

	/// Управляет загрузкой параметров из ини-файла
	IniManager   m_IniMngr;
	/// Указатель на собственный теплейт документа
	QEDocTemplate* m_pDocTemplate;
	/// обработчик состояния простоя
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

	/// По имени параметра вернуть целочисленное значение
	int GetIntSetting(const CString& strSettingName);
	/// По id ресурсной строки с именем параметра вернуть целочисленное значение
	int GetIntSetting(UINT uiSettingName);
	
	/// По имени параметра вернуть строку
	const CString GetStrSetting(const CString& strSettingName);
	/// По id ресурсной строки с именем параметра вернуть строку
	const CString GetStrSetting(UINT uiSettingName);

	/// Записать значение целочисленного параметра
	bool WriteIntSetting(const CString& strSettingName, int Val);
	/// Записать значение целочисленного параметра
	bool WriteIntSetting(UINT uiSettingName, int Val);

	/// Записать значение строкового параметра
	bool WriteStrSetting(const CString& strSettingName, CString Val);
	/// Записать значение строкового параметра
	bool WriteStrSetting(UINT uiSettingName, CString Val);

	/// Возвращает имя ини файла
	const CString GetProjIniFileName();

	interface IProjectSettings* GetProjectSettings() ;
	/// Переназначает имя ини файла в m_IniMngr
	bool ResetProjectSettingsMgr(const CString& strIniName);

	/// Назначает обработчк состояния простоя
	void setIdleProcessor(IIdleProcessor* project);

protected:
	/// Возвращает обработчк состояния простоя
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
	/// Находит параметр в списке параметров
	Setting& GetSetting(const CString& strSettingName);
	/// Находит параметр в списке параметров, загружая имя из ресурсов
	Setting& GetSetting(UINT uiSettingName);

	/// Собирает в единый список все параметры приложения
	void make_settings_list();

	/// Загружает из реестра все параметры приложения
	void load_settings_list();
	/// Записывает в реестр все параметры приложения
	void save_settings_list();

	/// Добавляет строковый параметр к списку настроек приложения
	bool add_string_setting(UINT uiSettingName, const CString& strSecName, UINT uiValueName, CString strDefVal = CString());
	/// Добавляет целочисленный параметр к списку настроек приложения
	bool add_int_setting(UINT uiSettingName, const CString& strSecName, UINT uiValueName, int iDefVal = 0);
};

CQuestEditorApp* GetMyApp();

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUESTEDITOR_H__6CF873A4_4A1A_4409_8CD9_F7C8A02D5DF7__INCLUDED_)
