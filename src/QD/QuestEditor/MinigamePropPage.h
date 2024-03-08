#pragma once

#include "BasePage.h"
// MinigamePropPage dialog

class qdMiniGame;
class qdGameDispatcher;

/**
	Свойства миниигры.
 */
class MinigamePropPage : public CBasePage
{
	DECLARE_DYNAMIC(MinigamePropPage)

public:
	MinigamePropPage();
	virtual ~MinigamePropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MINIGAME };

	/// Назначает обрабатываемую миниигры и диспетчер проекта
	void set_data(qdMiniGame* miniGame, qdGameDispatcher* gameDisp);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	afx_msg void OnBnClickedBtnSelectFile();
	afx_msg void OnEnKillfocusEditIniFile();
	afx_msg void OnBnClickedButtonParams();

	DECLARE_MESSAGE_MAP();
private:
	/// Диспетчер проекта
	qdGameDispatcher* gameDisp_;
	/// Миниигра
	qdMiniGame* miniGame_;
	/// Файл с параметрами
	CString iniFileName_;
	/// было ли имя файла изменено.
	/**
		Если было то назначаем новое имя файла и перегружаем параметры
	 */
	bool fileNameChanged_;
};
