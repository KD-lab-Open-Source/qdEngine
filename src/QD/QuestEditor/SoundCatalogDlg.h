#if !defined(AFX_SOUNDCATALOGDLG_H__0C0F0BDE_3A54_44DC_A0BE_B0144B4443DC__INCLUDED_)
#define AFX_SOUNDCATALOGDLG_H__0C0F0BDE_3A54_44DC_A0BE_B0144B4443DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SoundCatalogDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SoundCatalogDlg dialog
class qdGameDispatcher;
class qdGameDispatcherBase;

#include "user_finder.h"

class SoundCatalogDlg : public CDialog
{
// Construction
	CString m_strCap;
	bool m_bUniqueList;//заполняем диалог только содержимым текущего каталога или 
					//объединенным содержанием текущего и глобального.

	qdGameDispatcherBase*	m_ptrDisp;
	qdGameDispatcher*		m_ptrGameDisp;
	bool					m_bProjChanged;
	GameObjectStateList_t m_listStates4Affected;

	struct SOUND_FOR_REMOVE {
		int iPos;
		class qdSound* sound;
		SOUND_FOR_REMOVE(int i, qdSound* ps):iPos(i),sound(ps){} 
	};
	typedef std::list<SOUND_FOR_REMOVE> SoundForRemoveList_t;
	
public:
	SoundCatalogDlg(CWnd* pParent = NULL);   // standard constructor

	void set_caption(const CString &strCap){
		m_strCap = strCap;
	}

	void set_unique(bool bUniq){
		m_bUniqueList = bUniq;
	}

	bool is_proj_changed() const{
		return m_bProjChanged;
	}

	bool set_data(qdGameDispatcher* ptrGameDisp, qdGameDispatcherBase* ptrDisp);
// Dialog Data
	//{{AFX_DATA(SoundCatalogDlg)
	enum { IDD = IDD_SOUND_LIST };
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SoundCatalogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SoundCatalogDlg)
	afx_msg void OnButtonVarAdd2();
	afx_msg void OnButtonVarDel();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListVars(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditListVars(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonVarEdit2();
	afx_msg void OnKeydownListVars(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	void AdjustColumnsWidth();
	bool LoadSounds();

	void RenameSound(LPCTSTR strOldName, LPCTSTR strNewName);
	CString DetectLastState();
	void RemoveSoundFromObj();
	bool CanRemoveSound(LPCTSTR lpszName);

	CString TestGlobalObj(LPCTSTR lpszName);
	bool TestInObj(qdGameObject* po, LPCTSTR lpszName);
	void LoadResources();
	void FreeResources();
	qdSound * getSound(CString const& name);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_SOUNDCATALOGDLG_H__0C0F0BDE_3A54_44DC_A0BE_B0144B4443DC__INCLUDED_)
