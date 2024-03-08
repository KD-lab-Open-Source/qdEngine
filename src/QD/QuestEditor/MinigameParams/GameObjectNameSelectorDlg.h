#pragma once
#include "afxwin.h"

class qdGameScene;
class qdGameDispatcher;

// GameObjectNameSelectorDlg dialog
/*!
	�����-������� ������������ ������� ������ � ������� ���� ������ ���� ���������.
	����� ������� �������������� �������� selectGameObjectName
*/
class GameObjectNameSelectorDlg : public CDialog
{
	DECLARE_DYNAMIC(GameObjectNameSelectorDlg)

public:
	GameObjectNameSelectorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~GameObjectNameSelectorDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SEL_OBJ_NAME };

	void setGameDispatcher(qdGameDispatcher const* gameDisp);
	//! ��������� ��� �������
	CString const getObjectName() const;
	//! ���������� ������� ��� ���� ��� �������
	int getCount() const;
	void setCount(int count);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboScenes();
	afx_msg void OnLbnDblclkListObjects();

	DECLARE_MESSAGE_MAP();

	//! ��������� ����� � ��������� scenes_
	void loadScenes();
	//! ��������� ������� ����� � �������� objects_
	void loadObjects(qdGameScene const* scene);
	//! ���������� ��������� �����
	qdGameScene const* getScene();
	//! ������� ��� ������� �� ���������
	void getObjectName();

private:
	qdGameDispatcher const* gameDisp_;

	CComboBox scenes_;
	CListBox objects_;
	CString objectName_;
	int count_;
protected:
	virtual void OnOK();
};
