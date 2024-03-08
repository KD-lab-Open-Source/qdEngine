#if !defined(AFX_CONDDLGBASE_H__74EEDB2A_78C8_4119_8C5F_72644578EC75__INCLUDED_)
#define AFX_CONDDLGBASE_H__74EEDB2A_78C8_4119_8C5F_72644578EC75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CondDlgBase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CondDlgBase dialog

#include "qd_condition.h"
#include "MakeCondDlgBase.h"

/*!
 Базовый класс для класса диалога условия
*/
class CondDlgBase : public CDialog
{
	DECLARE_DYNAMIC(CondDlgBase)
protected:
	//! Указатель на класс-диалог, создающий условия(StateCondDlg, CO_ConditionDlg)
	MakeCondDlgBase* m_ptrMakeCondDlg;
	qdCondition m_qdCondition;
	// Construction
public:
	/*!
	 \param id - в младшем слове содержит идентификатор условия. В старшем содежит 
	 признак того, является ли состояние отрицанием
	*/
	CondDlgBase(UINT nIDTemplate, 
		DWORD id, 
		CWnd* pParentWnd = NULL);   // standard constructor

	//! Возвращет true, если данные условия полностью сформированы
	/**
		Таким вот вызовом m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
		происходит проверка. Проверкой заканчивается любая цепочка событий обрабатываемая
		в диалоге условия
	 */
	virtual bool IsCondDataComplete() = 0;
	virtual bool CanAdd(const class qdConditionalObject* pobj){
		return true;
	}
// Dialog Data
	//{{AFX_DATA(CondDlgBase)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void set_owner(MakeCondDlgBase* ptrCondDlg){
		m_ptrMakeCondDlg = ptrCondDlg;
	}
	
	const qdCondition& get_condtion(){
		return m_qdCondition;
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CondDlgBase)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_VIRTUAL

// Implementation
protected:
	enum eShowHide{
		SH_NONE, ///< Просто загрузить сцены 
		SH_SHOWHIDE///< \brief Загрузить сцены и, если сцена известна скрыть 
					///< комбобокс сцены.
	};
	//! Загружает сцены в комбобокс
	bool LoadScenes(CCJFlatComboBox& box, CWnd& sceneStatic, eShowHide eAct = SH_NONE);
	// Generated message map functions
	//{{AFX_MSG(CondDlgBase)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_CONDDLGBASE_H__74EEDB2A_78C8_4119_8C5F_72644578EC75__INCLUDED_)
