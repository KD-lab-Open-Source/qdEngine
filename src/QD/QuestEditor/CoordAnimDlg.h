#if !defined(AFX_COORDANIMDLG_H__FFBE07DF_C40D_4490_8499_155AA2DA79C7__INCLUDED_)
#define AFX_COORDANIMDLG_H__FFBE07DF_C40D_4490_8499_155AA2DA79C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CoordAnimDlg.h : header file
//

interface ISceneViewFacade;
class qdCoordsAnimation;
class qdGameObjectState;

/////////////////////////////////////////////////////////////////////////////
// CoordAnimDlg dialog

/// Класс диалога редактирования КА.
class CoordAnimDlg : public CDialog
{
// Construction
	/// Указатель на редактор сцены
	ISceneViewFacade* m_ptrView;
	/// Редактируемая копия исходной КА
	qdCoordsAnimation *m_ptrEditedCA;
	/// Исходная КА
	qdCoordsAnimation *m_ptrSourceCA;

	/// Находимся ли мы в пределах OnInitDialog
	bool m_bInitDialog;
public:
	CoordAnimDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CoordAnimDlg)
	enum { IDD = IDD_DLG_COORD_ANIM };
	int		m_iZ;
	int		m_iCurPoint;
	int		m_iPhase;
	BOOL	m_bUseDir;
	int		m_iDir;
	//}}AFX_DATA

	/// Устанавливает редактор сцены
	void SetView(ISceneViewFacade* ptrView);
	/// Набор констант используемых при окончании редактирования
	enum eEndEditMode{
		EEM_DISCARD, ///< Закончить и отбросить изменения
		EEM_SAVE///< Закончить и сохранить изменения
	};
	/// Прекращает редактирование, сохраняя или отбрасывая изменения
	VOID EndEdit(eEndEditMode mode);
	
	/// Назначает исходную КА
	void set_source_ca(qdCoordsAnimation* pca){
		m_ptrSourceCA = pca;
	}
	/// Возвращает исходную КА
	qdCoordsAnimation* get_source_ca() const{
		return m_ptrSourceCA;
	}
	/// вызывается в ответ на изменение из вне текущей точки
	void OnChangeCurrPoint();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CoordAnimDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CoordAnimDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdtCurPoint();
	afx_msg void OnChangeEdtPhase();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnCancel();
	afx_msg void OnChangeEditZ();
	afx_msg void OnCheckUseOrientation();
	afx_msg void OnChangeEditDirection();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	/// Параметры текущей точки переносит в контролы диалога
	void CurPointToWnd();
	VOID CollapseWindow(BOOL bCollapse);

	int GetCurrAnimFrame();		
	VOID CheckZAccess();
	VOID SetDirAccess();
	VOID CheckPointsDirection();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_COORDANIMDLG_H__FFBE07DF_C40D_4490_8499_155AA2DA79C7__INCLUDED_)
