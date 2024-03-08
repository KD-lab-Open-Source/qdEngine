#pragma once

#include "WizardBasePage.h"

// SelectFolderMDSPP dialog

/**
	Выбирает каталого, в котором будет создаваться структура каталогов
 */
class SelectFolderMDSPP : public WizardBasePage
{
	DECLARE_DYNAMIC(SelectFolderMDSPP)

public:
	SelectFolderMDSPP();
	virtual ~SelectFolderMDSPP();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MDS_SELECT_FOLDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnSetActive();
	afx_msg void OnEnChangeEdit();
	afx_msg void OnBnClickedButton();
	/// @brief Дизейблит/енейблит Next в зависимости от того, 
	/// существует ли указанная папка
	void check_next_btn_access();
	/// Запускает диалог выбора папки на диске
	CString get_disk_folder();
private:
	/// Содержит выбранную папку
	CString m_strFolder;
public:
	/// кладет выбранную папку в данные визарда
	virtual BOOL OnKillActive();
};
