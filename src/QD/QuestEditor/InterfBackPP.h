#pragma once
#include "basepage.h"

// InterfBackPP dialog
class qdInterfaceBackground;

/*!
	Класс диалога свойств для интерфейсного фона
*/
class InterfBackPP : public CBasePage
{
	DECLARE_DYNAMIC(InterfBackPP)

public:
	InterfBackPP();
	virtual ~InterfBackPP();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_BACK_PROP };

	//! Назначает фон, котороый будем изменять
	bool set_data(qdInterfaceBackground* ptrBack);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedButton1();

	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnBnClickedCheck();

	DECLARE_MESSAGE_MAP()
private:
	//! Данные из объекта в диалог
	void ele2wnd();
	//! данные из диалога в объект
	void wnd2ele();

	void OnChangePos();
	//! Перезагружает файл анимации
	void reload_file();
private:
	//! Image file
	CString m_strFileName;
	//! Х
	int m_iX;
	//! У
	int m_iY;
	//! Глубина
	int m_iD;

	//! Исходный объект
	qdInterfaceBackground* m_ptrBack;
	//! Копия для того, чтобы была возможность октатить
	std::auto_ptr<qdInterfaceBackground> m_ptrCopy;
	//! Зацикливать ли анимацию
	BOOL m_bLoop;
	//! Перевернуть ли по горизонтали
	BOOL m_bFlipH;
	//! Перевернуть ли по вертикали
	BOOL m_bFlipV;
};
