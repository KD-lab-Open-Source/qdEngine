#pragma once

#include "BasePage.h"
// MinigamePropPage dialog

class qdMiniGame;
class qdGameDispatcher;

/**
	�������� ��������.
 */
class MinigamePropPage : public CBasePage
{
	DECLARE_DYNAMIC(MinigamePropPage)

public:
	MinigamePropPage();
	virtual ~MinigamePropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MINIGAME };

	/// ��������� �������������� �������� � ��������� �������
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
	/// ��������� �������
	qdGameDispatcher* gameDisp_;
	/// ��������
	qdMiniGame* miniGame_;
	/// ���� � �����������
	CString iniFileName_;
	/// ���� �� ��� ����� ��������.
	/**
		���� ���� �� ��������� ����� ��� ����� � ����������� ���������
	 */
	bool fileNameChanged_;
};
