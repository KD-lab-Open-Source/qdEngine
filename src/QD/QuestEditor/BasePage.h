#if !defined(AFX_BASEPAGE_H__F1428263_8187_43AA_B4EE_C337DE891C70__INCLUDED_)
#define AFX_BASEPAGE_H__F1428263_8187_43AA_B4EE_C337DE891C70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BasePage.h : header file
//
/*!
  Константы, определяющие место положение страницы в списке страниц
*/

const int PAGE_PLACE_FIRST		= 0;///< Страница идёт первой
const int PAGE_PLACE_MIDLE		= 1;///< Страница где-то в середине
const int PAGE_PLACE_LAST		= 2;///< Самая последняя страница
const int PAGE_PLACE_FIRST_LAST	= 3;///< страница одна в мастере

/////////////////////////////////////////////////////////////////////////////
// CBasePage dialog
#include "PPNotification.h"
//нужен только для того, чтобы обработать правильно 
//и в одном месте измения проекта
class CBasePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBasePage);

	//! \brief на Ок можно нажать всегда, поэтому нам нужен 
	//!флаг, определящий были ли сделаны изменения.
	//! мы его ставим в true, в вызове SetModified
	BOOL m_bChage;
// Construction
public:
	CBasePage();
	CBasePage(UINT dlgID, UINT nIdCap = 0);
	~CBasePage();

	//! Выставляем/сбрасываем флаг, что данные изменены
	void SetModified(BOOL bChanged = TRUE);
	//! Возваращает флаг изменения данных
	BOOL GetModified(){
		return m_bChage;
	}

	//! Перерисовывает сцену в редакторе сцены.
	void redraw_scene(bool bRedrawScene);
	//! \brief Выравнивает размер битмапа в редакторе сцены по размеру сцены или окна,
	//! если окно шире
	void AdjustBitmapSize();
	// Dialog Data
	//{{AFX_DATA(CBasePage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBasePage)
	public:
	virtual BOOL OnApply();
	virtual void OnOK();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBasePage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_BASEPAGE_H__F1428263_8187_43AA_B4EE_C337DE891C70__INCLUDED_)
