// MyPropertySheet.h : header file
//
// This class defines custom modal property sheet 
// CMyPropertySheet.
 
#ifndef __MYPROPERTYSHEET_H__
#define __MYPROPERTYSHEET_H__
/////////////////////////////////////////////////////////////////////////////
// CMyPropertySheet
/**
	Ќужен здесь, чтобы реализовать обработку событи€ 
	WM_SET_COORD_ANIMATION_MODE. ¬се это нужно было, чтобы была возможность 
	редактировать координатную анимацию нажавк кнопку в диалоге свойств состо€ни€
 */
class CMyPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMyPropertySheet)

		// Construction
public:
	CMyPropertySheet(CPropertyPage** ptrPages, int iActive, 
					DWORD dwCaption, CWnd* pWndParent = NULL);

// Attributes
public:
// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyPropertySheet)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	LRESULT OnSetCoordAnimationMode(WPARAM wParam, LPARAM lParam);
	// Implementation
public:
	virtual ~CMyPropertySheet();

// Generated message map functions
protected:
	//{{AFX_MSG(CMyPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __MYPROPERTYSHEET_H__
