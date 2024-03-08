#if !defined(AFX_STATICOBJPAGE_H__0740096A_85AB_469B_AAFA_DC2FB61C7AE4__INCLUDED_)
#define AFX_STATICOBJPAGE_H__0740096A_85AB_469B_AAFA_DC2FB61C7AE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticObjPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// StaticObjPage dialog

class qdGameDispatcher;
class qdGameScene;
class qdGameObjectStatic;
class qdNamedObject;

#include "basepage.h"
#include "CurDirSafer.h"

typedef std::vector<CString> CStringVect_t;

/**
	�������� ������������ �������
 */
class StaticObjPage : public CBasePage
{
	DECLARE_DYNCREATE(StaticObjPage)

	/// true �� ������� ������ ����
	bool m_bInitDialog;

	/// ��������� �������
	qdGameDispatcher*	m_ptrGameDisp;
	/// �����, ��������� ��������
	qdGameScene*		m_ptrScene;
	/// ������
	qdGameObjectStatic* m_ptrObj;

	bool	m_bRedraw;///<  �� �������������� �����
	//���� ����� �������, ������ �����
	
	/// ��������� ��� ���������
	void apply_params();
	/// ��������� �������� ����������� � �������� �������
	void apply_pos_params();
	/// ��������� ����� �����������
	void apply_picture();
	// Construction

	/// ��� ����������
	int		m_iScrXBak;
	int		m_iScrYBak;
	int		m_iXBak;
	int		m_iYBak;
	int		m_iZBak;
	int		m_iDepthBak;
	int		m_bImmovableBak;

	enum eConvertDir{CD_NONE, ///<  �� ���� ��������������
		CD_WORLD_TO_SCREEN, ///<  � ��������
		CD_SCREEN_TO_WORLD, ///<  � �������
	};
	//! ������� ����������� ���������� ��������
	eConvertDir m_eConvertDir;


	/// ��������� ���������� ��� ����� �����������
	CString	m_strPicFileNameBak;
	
	/// ������ ��������� ����� �������� ������
	void real2bak();
	/// ��������������� ������ �� ��������� �����
	void bak2real();

	/// ������������ ���������� � ��������� �� �������
	void screen2world();
	/// ������������ ����������
	Vect3f screen2world(int x, int y, int z);

	/// ������������ ���������� � ��������� �� �������
	void world2screen();
	/// ������������ ����������
	void world2screen(Vect3f pos, int& x, int& y, int &z);
	
	/// ��������� �������� ��� ���������
	void DisablePositionCtrls();

	/// ������������� ��������� � ������� ������
	void AdjustSpins();

	/// �� ��������� ������ ���������� ������ �������� ����
	Vect3f c2lt(Vect3f pos);
	/// �� ��������� ������ �������� ���� ���������� ������
	Vect3f lt2c(Vect3f pos);

	/// ��������� ������� � �������� �������
	void obj2scr();
public:
	StaticObjPage();
	~StaticObjPage();

	/// ������������ ��������� ������� � �������������� ������
	bool set_data(qdGameDispatcher* ptrGD, qdGameObjectStatic* ptrObj);

	/// ��������� ������ ��� ������ ������ �����������
	/**
		���������� ��� ������� ���������� �����. ���� ptrVect!=NULL, �� ����������
		���� ���������
	 */
	static CString SelectPicture(HWND m_hWndParent, 
		const CString &strTitle, CStringVect_t* ptrVect = NULL);
		
	// Dialog Data
	//{{AFX_DATA(StaticObjPage)
	enum { IDD = IDD_PROPPAGE_OBJECT_STAT };
	int		m_iScrX;
	int		m_iScrY;
	CString	m_strPicFileName;
	int		m_iX;
	int		m_iY;
	int		m_iZ;
	int		m_iDepth;
	BOOL	m_bUseLeftTop;
	/// ������� ����, ��� ������ �� ����������
	BOOL	m_bImmovable;
	//}}AFX_DATA
	int		m_iNetProjD;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(StaticObjPage)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(StaticObjPage)
	/// ����� ���������� �������� ���������� � ��������� �� � �������
	afx_msg void OnEnterInScrPos();
	/// ����� ���������� ������� ���������� � ��������� �� � �������
	afx_msg void OnEnterInWorldPos();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	/// �������� � ��������� ����������� � �������
	afx_msg void OnSelectBtn();
	afx_msg void OnUseLeftTop();
	afx_msg void OnCheckImmovable();
	/// ������������� ��������� �������
	afx_msg void OnEnterInNetProjScrD();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	/// ����� ���������� �������� ���������� � ��������� �� � �������
	afx_msg void OnEnKillfocusInScrPos();
	/// ����� ���������� ������� ���������� � ��������� �� � �������
	afx_msg void OnEnKillfocusInWorldPos();
	afx_msg void OnEnChangeInScrPos();
	afx_msg void OnEnChangeInWorldPos();
	/// ������������� ��������� �������
	afx_msg void OnDeltaposSpinNetProjScrD(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	/// �������� ��������� �������� ������ �� ��������� ������� ��� �������� �� �����
	void changeNetProjScrD(int delta);
	/// �������������� ����� � ��������� �����
	bool redraw();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_STATICOBJPAGE_H__0740096A_85AB_469B_AAFA_DC2FB61C7AE4__INCLUDED_)
