#if !defined(AFX_STATEBOUNDDLG_H__327B966C_5602_4C0B_80D6_64533914904A__INCLUDED_)
#define AFX_STATEBOUNDDLG_H__327B966C_5602_4C0B_80D6_64533914904A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StateBoundDlg.h : header file
//

#include <memory>

/////////////////////////////////////////////////////////////////////////////
// StateBoundDlg dialog
class qdGameObjectState;

/**
	Редакитруем границы состояния
 */
class StateBoundDlg : public CDialog
{
	/// Идет ли инициализация диалога
	bool m_bInitDialog;
	/// Копия состояния для восстановления
	std::auto_ptr<qdGameObjectState> m_ptrStateCopy;
	qdGameObjectState* m_pSrcState;///< исходное состояние
// Construction
public:
	StateBoundDlg(CWnd* pParent = NULL);   // standard constructor

	/// Константы описывают возможные пути прекращения редактирования
	enum eEndEditMode {EEM_DISCARD,///< Изменения отбрасываются
		EEM_SAVE///< Изменения сохраняются
	};
	/// Прекращает редактирование
	void EndEdit(eEndEditMode mode);
	/// Назначает редактрируемое состояние 
	void set_data(qdGameObjectState* pstate);
	/// Возвращает редактируемое состояние
	qdGameObjectState* getEditedState() const;
	/// Обновляет данные в контролах окна
	void UpdateStateData();
	// Dialog Data
	//{{AFX_DATA(StateBoundDlg)
	enum { IDD = IDD_DLG_STATE_BOUND };
	int		m_iX;///< размер по х
	int		m_iY;///< рамер по у
	int		m_iZ;///< размер по z
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(StateBoundDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(StateBoundDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	/// Данные из контролов в состояние
	void wnd2state();
	/// Данные из состояния в контролы
	void state2wnd();
};

/// Запускает диалог редактирования размеров состояния
bool EditStateBound(qdGameObjectState* pstate);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_STATEBOUNDDLG_H__327B966C_5602_4C0B_80D6_64533914904A__INCLUDED_)
