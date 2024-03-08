#if !defined(AFX_MULTIWIZARD_H__4063091A_02BC_4770_8EEF_585D77F084F3__INCLUDED_)
#define AFX_MULTIWIZARD_H__4063091A_02BC_4770_8EEF_585D77F084F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MultiWizard.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MultiWizard
#include <list>

class MWBasePage;
class MWWizard;

/**
	Класс реализует концепцию мульти-визарда. Хотелось избежать клепания похожих
	визардов по нескольку раз. Поэтому попытался сделать так, чтобы один 
	и тот же визард можно было запускать как отдельно, так и в совместно с другими.
	Например, визард добавления анимации может быть вызван как отдельно так и при 
	создании нового проекта вместе с визардом параметров сцены.

	Класс вставляет дерево слева. В этом дереве каждый визард записывает свое имя.
	Между визардами можно переключаться туда сюда
 */
class MultiWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(MultiWizard);

	friend class MWBasePage;
	/// Тип списка визардов
	typedef std::list<MWWizard*> WizardList_t;
// Construction
public:
	MultiWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	MultiWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Operations
public:

	bool AddWizard(MWWizard* pw);

	VOID RemoveWizard(MWWizard* pw);
	VOID SuppresWizardChange(BOOL bSupp = TRUE);

	//актирвизирует указанный и возвращает предыдущий
	bool ActivateWizard(MWWizard* pw);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MultiWizard)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	afx_msg VOID OnTreeItemChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg VOID OnTreeItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg VOID OnNextWizard();
	afx_msg VOID OnPrevWizard();
// Implementation
public:
	virtual ~MultiWizard();

	// Generated message map functions
protected:
	
	//{{AFX_MSG(MultiWizard)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	VOID AdjustPageLayout(MWBasePage* page);
	VOID OnEndWizard();

	CRect GetFirstPageRect();
	VOID MoveStandartButtons();

	VOID AddNavigationButtons();
	VOID FillTree();

	VOID CheckAccessToNWB();
	VOID CheckAccessToPWB();

	bool ShowWizard(MWWizard* pw);
	bool SuspendWizard(MWWizard* pw);

	void add_pages(MWWizard* pw);
private:
	/// Дерево с именами визардов
	CTreeCtrl m_wndTree;
	/// Кнопка перхода к следующему визарду
	CButton m_wndNextWizard;
	/// Кнопка перхода к предыдущему визарду
	CButton m_wndPrevWizard;
	/// Список добавленных визардов
	WizardList_t m_wizards;
	/// Текущий визард
	WizardList_t::iterator m_cur_wizard;
	/// Идентификатор ресурсной строки для заголовка
	UINT m_uiCapID;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MULTIWIZARD_H__4063091A_02BC_4770_8EEF_585D77F084F3__INCLUDED_)
