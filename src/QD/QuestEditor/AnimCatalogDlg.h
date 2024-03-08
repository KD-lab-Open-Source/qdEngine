#if !defined(AFX_AnimCatalogDlg_H__DF5C6CF7_63E6_4C07_8E70_441EEFACFC72__INCLUDED_)
#define AFX_AnimCatalogDlg_H__DF5C6CF7_63E6_4C07_8E70_441EEFACFC72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimCatalogDlg.h : header file
//
#include <deque>
/////////////////////////////////////////////////////////////////////////////
// AnimCatalogDlg dialog

#include "user_finder.h"

enum AnimCatalogDlgAction{GOPA_SHOW, GOPA_EDIT, GOPA_CREATE, GOPA_ADD};

typedef std::deque<qdAnimationInfo*> qdAnimationInfoDEQue_t;

class AnimCatalogDlg : public CDialog
{
	DECLARE_DYNCREATE(AnimCatalogDlg)

	//глобальный диспетчер игры
	qdGameDispatcher		*m_ptrGameDisp;
	//диспетчер, для которого редактируется список анимаций
	qdGameDispatcherBase 	*m_ptrDisp;

	//ставим в true, если добавляли или удалили анимацию
	bool					m_bTreeChanged;
	//имя анимации, которую надо будет выделить в списке анимаций
	CString m_strActiveAnimName;
			
	AnimCatalogDlgAction m_eAction;

	//используется для того, чтобы необходимое действие 
	//запускалось только при первом показе диалога
	bool m_bFirstTimeShow;
	CString m_strCap;
	//показывать уникальный список каталога или все доступные
	bool m_bUniqueList;

	GameObjectStateList_t m_listStates4Affected;
	qdAnimationInfoDEQue_t m_dequeAI;

	struct ANIMATION_FOR_REMOVE {
		int iPos;
		class qdAnimation* anim;
		ANIMATION_FOR_REMOVE(int i, qdAnimation* pa):iPos(i),anim(pa){} 
	};
	typedef std::list<ANIMATION_FOR_REMOVE> AnimForRemoveList_t;
// Construction
public:
	AnimCatalogDlg();
	~AnimCatalogDlg();

	bool is_tree_changed(){
		return m_bTreeChanged;
	}
	bool set_data(qdGameDispatcher* ptrGD, qdGameDispatcherBase* ptrObj);
	bool set_active_anim_name(const CString& strName);
	void set_action(int eAction)
	{
		m_eAction = (AnimCatalogDlgAction)eAction;
	}
	void set_caption(const CString& str){
		m_strCap = str;
	}
	void set_unique(bool bUnique){
		m_bUniqueList = bUnique;
	}
// Dialog Data
	//{{AFX_DATA(AnimCatalogDlg)
	enum { IDD = IDD_ANIMATION_LIST };
	CListCtrl	m_list;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(AnimCatalogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(AnimCatalogDlg)
	afx_msg void OnButtonVarAdd2();
	afx_msg void OnButtonVarDel();
	afx_msg void OnButtonVarEdit2();
	virtual BOOL OnInitDialog();
	afx_msg void OnEndlabeleditListVars(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonVarCreat();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDblclkListVars(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListVars(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
private:
	void LoadAnimations();	
	void UpdateAnimationInfo(qdAnimation* pa, int iIdnex);
	void SelectAnim();

	int InsertAnimation2List(qdAnimation* pa);

	void AdjustColumnsWidth();

	bool CanRemoveAS(LPCTSTR lpszName);
	CString DetectLastState();
	void RemoveAnimationFromObj();
	void RemoveAnimationFromGaits();
	
	CString TestGaits(LPCTSTR lpszName);
	CString TestGlobObj(LPCTSTR lpszName);
	
	void RenameAnimation(LPCTSTR lpszOldName, LPCTSTR lpszNewName);
	CString TestGlobalObj(LPCTSTR lpszName);

	bool TestInObj(qdGameObject* po, LPCTSTR lpszName);
		
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_AnimCatalogDlg_H__DF5C6CF7_63E6_4C07_8E70_441EEFACFC72__INCLUDED_)
