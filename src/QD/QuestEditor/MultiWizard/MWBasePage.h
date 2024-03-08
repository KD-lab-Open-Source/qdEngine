#if !defined(AFX_MWBASEPAGE_H__368FB32B_EEF0_49A7_B9FF_5CF38F9E0863__INCLUDED_)
#define AFX_MWBASEPAGE_H__368FB32B_EEF0_49A7_B9FF_5CF38F9E0863__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MWBasePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MWBasePage dialog
class MWWizard;
#define GET_PSHEET() static_cast<CPropertySheet*>(GetParent())

/**
	Базовый класс для страницы мульти-визарда.
	Её задача вызывать при своей активации 
	MultiWizard::AdjustPageLayout, чтобы выравнять положение в окне.
	MultiWizard::OnEndWizard в OnWizardFinish, чтобы была возможность 
	переключиться на следующий визард.
 */
class MWBasePage : public CPropertyPage
{
	DECLARE_DYNCREATE(MWBasePage)

// Construction
public:
	MWBasePage(){};
	MWBasePage(UINT dlgIDD);
	~MWBasePage();

	/// Назначает владельца
	void set_wizard(MWWizard* pw){
		m_ptrMWWizard = pw;
	}
	/// Возвращает владельца
	MWWizard* wizard() const{
		return m_ptrMWWizard;
	}
// Dialog Data
	//{{AFX_DATA(MWBasePage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MWBasePage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(MWBasePage)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	/// визарад, к которому принадлежит данная старничка
	MWWizard* m_ptrMWWizard;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MWBASEPAGE_H__368FB32B_EEF0_49A7_B9FF_5CF38F9E0863__INCLUDED_)
