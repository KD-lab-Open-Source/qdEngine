#pragma once

#include "WizardBasePage.h"
// SelectNameMWPage dialog

interface INameSetter;
interface INameTester;

/**
	Страничка выбора имени для элемента интерфейса
 */
class SelectNameMWPage : public WizardBasePage
{
	DECLARE_DYNAMIC(SelectNameMWPage)

public:
	SelectNameMWPage();
	virtual ~SelectNameMWPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_SELECT_NAME };

	/// Назначает m_pStringSetter
	void SetNameSetter(INameSetter* psetter);
	/// Назначает m_pNameTester
	void SetNameTester(INameTester* pTester);
	/// Назначает заголовок окна
	void SetCaption(const CString& cap);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	/**
		Если имя пустое, то кнопка Next дизейблится.
		Иначе генерируем на основе введенного имени уникальное 
		и даем доступ к Next
	 */
	afx_msg void OnEnChangeEditName();
private:
	/// Введенное имя
	CString m_strName;
	/// Интерфейс класса, назначающего имя объекту
	INameSetter* m_pStringSetter;
	/// Интерфейс класс, тестирующего уникальность имени
	INameTester* m_pNameTester;
	/// Заголовок станицы
	CString m_strCaption;
};
