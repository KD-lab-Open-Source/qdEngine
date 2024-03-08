#if !defined(AFX_MOVESTOPDLG_H__0F3F3D48_0EF7_47A1_8AA4_5E179DCD48C3__INCLUDED_)
#define AFX_MOVESTOPDLG_H__0F3F3D48_0EF7_47A1_8AA4_5E179DCD48C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MoveStopDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MoveStopDlg dialog
#include "editlistctrl.h"
#include "qd_game_dispatcher.h"
#include "user_finder.h"

/**
	Список походок. Объеденяет список походок из двух диспетчеров(если они разные)
 */
class MoveStopDlg : public CDialog
{
	qdGameDispatcher* m_ptrGameDisp;
	qdGameDispatcherBase* m_ptrGameDispBase;
	/// были ли сделаны измениния
	bool m_bTreeChanged;
	/// Не используется
	bool m_bAsk4Del;
	/// Список затронутых объектов
	GameObjectStateList_t m_listStatesAffected;
// Construction
public:
	MoveStopDlg(CWnd* pParent = NULL);   // standard constructor

	/// Назначает диспетчеры
	bool set_data(qdGameDispatcher* ptrGD, qdGameDispatcherBase* ptrGDB);

	bool has_change(){return m_bTreeChanged;}
// Dialog Data
	//{{AFX_DATA(MoveStopDlg)
	enum { IDD = IDD_MOVING_LIST };
	CEditListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MoveStopDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MoveStopDlg)
	afx_msg void OnButtonVarEdit2();
	afx_msg void OnButtonVarDel();
	afx_msg void OnButtonVarAdd2();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListVars(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListVars(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	void OnEndEditList(NMHDR * pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP();
private:
	/// Вставляет необходимые столбцы
	bool InsertColumns();
	/// Выравнивает ширину стобцов
	void AdjustColumnsWidth();
	/// Загружает список походок из m_ptrGameDispBase
	bool LoadAnimationSets();
	/// Вставляет конкретную походку в позицию
	bool InsertAnimationSet(int iPos, qdAnimationSet* ptr);
	/// Запускает диалог редактирования походки
	bool EditAnimationSet(qdGameDispatcher* ptrGameDisp, qdAnimationSet* ptrAS);

	/// \brief проверяет все сцены на наличеие персонажа
	/// если в сцене нет персонажа, то создаёт его с текущей походкой
	bool CheckScenes4Pers(qdAnimationSet* ptrASet);
	/// если в сцене нет персонажа, то создаёт его с текущей походкой
	bool CheckScene4Pers(qdGameScene* ptrScene, qdAnimationSet* ptrASet);
	
	/// Добавляет персонажа в сцену
	qdGameObjectMoving* AddPersonage(qdGameScene* ptrScene);

	/// Удаляет походку из персонажа
	void RemoveGaitFromPers();
	/// Отвечает на вопрос можно ли удалить походку.
	/**
		Проверяет используется ли походка с именем lpszName где-либо. 
		Если используется, то спрашиваем у пользователя можно ли удалить
	 */
	bool CanRemoveAS(LPCTSTR lpszName);

	/// \brief проверяет,какие состояния из списка удаления, являются последними
	/// возвращает имена соответствующих объектов
	CString DetectLastState();

	/// изменяет имя походки в объектах
	void ReplaceGait(LPCTSTR lpszOldName, LPCTSTR lpszNewName);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MOVESTOPDLG_H__0F3F3D48_0EF7_47A1_8AA4_5E179DCD48C3__INCLUDED_)
