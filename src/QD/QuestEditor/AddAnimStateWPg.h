#if !defined(AFX_ADDANIMSTATEWPG_H__73D629B8_01FE_4D91_8AB9_7424FE015720__INCLUDED_)
#define AFX_ADDANIMSTATEWPG_H__73D629B8_01FE_4D91_8AB9_7424FE015720__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddAnimStateWPg.h : header file
//

#include "dispslistsmerger.h"
#include "ielikecombobox.h"

#ifdef _AddAnimState_USE_PROP_PAGE
#include "basepage.h"
#endif

#include "shelltree\shelltree.h"
/////////////////////////////////////////////////////////////////////////////
// AddAnimStateWPg dialog

class qdGameScene;
class qdGameDispatcher;
class qdGameObjectState;
typedef std::vector<qdGameObjectState*> qdGameObjectStateVector;

typedef std::vector<int> IntVector_t;

enum OPERATION_STATUS{OS_SUCC, OS_WARN, OS_ERR};

const int LIST_CONTENT_ANIMATION	= 0;
const int LIST_CONTENT_GAIT			= 2;
const int LIST_CONTENT_SOUND		= 1;
const int LIST_CONTENT_DLG_FILES	= 3;

#ifdef _AddAnimState_USE_PROP_PAGE
class AddAnimStateWPg : public CBasePage
#else
class AddAnimStateWPg : public CDialog
#endif
{
	DECLARE_DYNCREATE(AddAnimStateWPg);

	//обрабатываются динамические объекты или нет
	bool m_b4AnimObj;

	bool m_bTunneling;
	CString m_strFileExts;
	CString m_strRoot;//выбранный в текущий момент каталог
	CStringVect_t m_vecFiles;

#ifndef _AddAnimState_USE_PROP_PAGE
	bool m_bModified;
#endif

	//для песонажей. Обрабатывются состояния движения или нет
	bool m_bUseMovementState;
	//добавляем мы объекты или 
	//состояния к существующим объектам
	bool m_bAddObjs;
	
	//диспетчер игры
	qdGameDispatcher *m_ptrGameDisp;
	//сцена
	qdGameDispatcherBase* m_ptrObjDisp;

	CStringSet_t m_setObjs;
	CStringSet_t m_setObjStates;

	qdGameObjectStateVector m_vecInsertedAnimStates;
	HWND m_hStateNameEdit;
	HWND m_hObjNameEdit;

	IntVector_t m_vecChecked;

	OPERATION_STATUS m_eLastOpStatus;
	HICON m_hicoSucc, m_hicoWarn, m_hicoErr;

	//объект, для которого добавляем состояния
	qdGameObjectAnimated* m_ptrObj;

	int m_iPagePlace;
	
	CString m_strCap;

	std::auto_ptr<CStringSet_t> m_ptrAList;
	std::auto_ptr<CStringSet_t> m_ptrSList;
	std::auto_ptr<CStringSet_t> m_ptrGaits;

	// Construction
public:
	AddAnimStateWPg();
	~AddAnimStateWPg();

	VOID set_obj_type(qdNamedObjectType _type);
	VOID set_use_movement(){
		m_bUseMovementState = true;
	}

	bool set_data(qdGameDispatcher* ptrDisp, qdGameDispatcherBase* ptrObjDisp);

	VOID set_obj(qdGameObjectAnimated* ptrObj);

	VOID set_place(int nPlace){
		m_iPagePlace = nPlace;
	}

	VOID set_caption(const CString& str){
		m_strCap = str;
	}

#ifndef _AddAnimState_USE_PROP_PAGE
	void SetModified(){
		m_bModified = true;
	}
	bool GetModified(){
		return m_bModified;
	}
#endif
// Dialog Data
	//{{AFX_DATA(AddAnimStateWPg)
	enum { IDD = IDD_PROPPAGE_MAKE_STATE };
	CShellTree	m_tree;
	CListCtrl	m_list_files;
	CEdit	m_wndObjName;
	CComboBoxEx	m_wndLog;
	CListCtrl	m_list;
	int		m_iContentType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(AddAnimStateWPg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
#ifdef _AddAnimState_USE_PROP_PAGE
	virtual BOOL OnSetActive();
#endif
	virtual BOOL DestroyWindow();
	protected:
	virtual VOID DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(AddAnimStateWPg)
	virtual BOOL OnInitDialog();
	afx_msg VOID OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg VOID OnSetFocusComboObjName();
	afx_msg VOID OnBtnAddState();
	afx_msg VOID OnKillfocusComboObjName();
	afx_msg void OnRadioListContent();
	afx_msg void OnRadioListContent2();
	afx_msg void OnRadioListContent3();
	afx_msg void OnRadioListContent4();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	bool LoadGaits();
	bool LoadAnimations();
	bool LoadSounds();
	bool LoadObjects();
	bool FillStateCombo();
	bool SelectStateVector(VOID *pItemData);

	VOID ClearCheck();

	qdGameObjectAnimated* InsertObj2Scene(CString& strObjName);
	bool AddStatesToObject(qdGameObjectAnimated* ptrObj);
	//проверяем условия создания объекта
	bool CheckAddConds();

	VOID SetNewObjName();

	bool WriteSucc(const CString& strMsg);
	bool WriteWrn(const CString & strMsg);
	bool WriteErr(const CString& strMsg);

	VOID ClearStatus();

	bool CheckName(const CString& strName);

	bool AddObject();
	bool AddState();

	bool LoadLists();

	bool UpdateWizardBtnsState();
	VOID SetupFileSelector();

	CString ReadTextFromFile(const CString& strFileName);

	VOID SetPath(const CString& strPath);
	VOID SetFileExts(LPCTSTR lpszExt);
	VOID SetupFileCtrlsLayout(const CRect &rcBound);
	bool FillList(const CString & strRoot);
	const CStringVect_t& GetFiles();
	VOID MakeFilesList();
	VOID ShowFileCtrl(int nCmdShow);
	VOID EnableFileImages();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_ADDANIMSTATEWPG_H__73D629B8_01FE_4D91_8AB9_7424FE015720__INCLUDED_)
