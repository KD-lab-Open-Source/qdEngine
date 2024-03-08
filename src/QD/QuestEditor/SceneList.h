#if !defined(AFX_SCENELIST_H__722E602B_016D_47B9_AFC8_1F34BB7253D4__INCLUDED_)
#define AFX_SCENELIST_H__722E602B_016D_47B9_AFC8_1F34BB7253D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SceneList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SceneList dialog
class qdGameDispatcher;
class qdGameScene;

#include <boost/scoped_array.hpp>

/**
	����� ������. ������� ������ ���� � ��������� ������� ��������� �� ���
 */
class SceneList : public CDialog
{
	qdGameDispatcher* m_ptrGameDisp;
	/// ��������� �� ������� "����� �����"
	bool m_bShowCrtNew;
	/// ��������� �� �������� ��������� ����
	bool m_bMultiSel;
	/// ������ ��������� ����
	boost::scoped_array<qdGameScene*> m_ptrScenes;
	/// ���������� ��������� ����
	int m_iScenesCount;
// Construction
public:
	SceneList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SceneList)
	enum { IDD = IDD_DLG_SCENE_LIST };
	CListCtrl	m_list;
	//}}AFX_DATA

	/// ��������� ��������� �������
	bool set_data(qdGameDispatcher* ptrDisp);
	/// Setter for m_bShowCrtNew
	VOID set_show_crt_new(bool bShow){
		m_bShowCrtNew = bShow;

	}
	/// Setter for m_bMultiSel
	VOID set_multi_select(bool bms){
		m_bMultiSel = bms;
	}
	
	/// ����� ������ ��������� �����; 0 ���� ������� �������� �����
	qdGameScene* get_scene() const{
		return m_ptrScenes[0];
	}

	/// ���������� ��������� �� ������ � ��������� �������
	qdGameScene** get_scenes() const {
		return m_ptrScenes.get();
	}
	/// ���������� ��������� � ������� ��������� ����
	int count() const{
		return m_iScenesCount;
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SceneList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SceneList)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	/// ��������� ����������� ������� � �������
	bool InsertColumns();
	/// ��������� ������ ����
	bool LoadSceneList();
	/// ����������� ������ �������� �� ������ ������
	void AdjustColumnsWidth();
	/// ��������� ��������� ����� � ������
	int InsertScene(int nNum, qdGameScene* ptrScene);

	/// �������� �� �������� ���������� ����� � ������
	bool GetSelected();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_SCENELIST_H__722E602B_016D_47B9_AFC8_1F34BB7253D4__INCLUDED_)
