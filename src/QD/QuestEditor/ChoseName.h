#if !defined(AFX_ChooseName_H__80408421_6F3E_46FA_BD1E_A93574B3480D__INCLUDED_)
#define AFX_ChooseName_H__80408421_6F3E_46FA_BD1E_A93574B3480D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseName.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ChooseName dialog

///	Класс диалога запрашивающего у пользователя имя объекта
/**
	Пустое имя водить не разрешается
 */
class ChooseName : public CDialog
{
// Construction
public:
	ChooseName(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ChooseName)
	enum { IDD = IDD_DLG_NAME };
	/// Содержит имя
	CString	m_strName;
	//}}AFX_DATA

	/// Позволяет задать начальное значение. То что пользователь увидит при открытии диалога
	void set_str(const CString &str){
		m_strName = str;
	}
	
	/// Возвращает имя
	const CString& get_str(){
		return m_strName;
	}
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ChooseName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ChooseName)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/// Запрашивает у пользователя строку. 
/**
	Возвращает то, что ввели, если нажали ОК. Либо дефаултное значение
*/
const CString AskUserForName(const CString& strDef);

/// Запрашивает у пользователя строку. 
/**
	Возвращает true , если нажали ОК. false - в противном случае
*/
bool AskUserForName(CString& strName);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_ChooseName_H__80408421_6F3E_46FA_BD1E_A93574B3480D__INCLUDED_)
