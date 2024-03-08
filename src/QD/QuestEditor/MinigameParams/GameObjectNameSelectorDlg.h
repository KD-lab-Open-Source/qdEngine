#pragma once
#include "afxwin.h"

class qdGameScene;
class qdGameDispatcher;

// GameObjectNameSelectorDlg dialog
/*!
	Класс-диалога позволяющего выбрать объект и сколько этот объект надо создавать.
	Вызов диалога осуществалятся функцией selectGameObjectName
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
	//! Возврщает имя объекта
	CString const getObjectName() const;
	//! Возвращает сколько раз надо его создать
	int getCount() const;
	void setCount(int count);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboScenes();
	afx_msg void OnLbnDblclkListObjects();

	DECLARE_MESSAGE_MAP();

	//! Загружает сцены в комбобокс scenes_
	void loadScenes();
	//! Загружает объекты сцены в листбокс objects_
	void loadObjects(qdGameScene const* scene);
	//! Возвращает выбранную сцену
	qdGameScene const* getScene();
	//! Достает имя объекта из листбокса
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
