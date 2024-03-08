#if !defined(AFX_PARALAXPAGE_H__61DB12DC_E99E_4812_882F_9A90C9FB7E08__INCLUDED_)
#define AFX_PARALAXPAGE_H__61DB12DC_E99E_4812_882F_9A90C9FB7E08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParalaxPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ParalaxPage dialog
#include "basepage.h"

class qdGameObject;
class qdGameDispatcher;

/**
	Страница свойст с параметрами паралакса
 */
class ParalaxPage : public CBasePage
{
	DECLARE_DYNCREATE(ParalaxPage);

	/// Идиет ли инициализация диалога
	bool m_bInitDialog;
	/// Резервная копия отступа по х
	int		m_iXBak;
	/// Резервная копия отступа по у
	int		m_iYBak;

	/// Объект, для котрого редактируем паралакс
	qdGameObject* m_ptrObj;
// Construction
public:
	ParalaxPage();
	~ParalaxPage();

// Dialog Data
	//{{AFX_DATA(ParalaxPage)
	enum { IDD = IDD_PROPPAGE_OBJECT_PARALAX };
	int		m_iX;
	int		m_iY;
	//}}AFX_DATA


	bool set_data(qdGameObject* ptrObj);
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(ParalaxPage)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(ParalaxPage)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnChangeParalax();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	
	/// Сохраняет копию для последующего восстановления
	void real2bak();
	/// Восстанавливает паралакс из резервной копии
	void bak2real();

	/// Применяет значения паралакса к объекту
	void apply_params();

	/// Копирует значения паралакса в переменные связанные с контролами диалога
	void Obj2Dlg();
	/// Выставляет для регуляторов максимальные и минимальные значения
	void AdjustSpinRanges();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_PARALAXPAGE_H__61DB12DC_E99E_4812_882F_9A90C9FB7E08__INCLUDED_)
