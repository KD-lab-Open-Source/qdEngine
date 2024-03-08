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
	Класс диалог. Выводит список сцен и позволяет выбрать отдельные из них
 */
class SceneList : public CDialog
{
	qdGameDispatcher* m_ptrGameDisp;
	/// Добавлять ли строчку "Новая сцена"
	bool m_bShowCrtNew;
	/// Позволять ли выбирать несколько сцен
	bool m_bMultiSel;
	/// Список выбранных сцен
	boost::scoped_array<qdGameScene*> m_ptrScenes;
	/// Количество выбранных сцен
	int m_iScenesCount;
// Construction
public:
	SceneList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SceneList)
	enum { IDD = IDD_DLG_SCENE_LIST };
	CListCtrl	m_list;
	//}}AFX_DATA

	/// Назначает диспетчер проекта
	bool set_data(qdGameDispatcher* ptrDisp);
	/// Setter for m_bShowCrtNew
	VOID set_show_crt_new(bool bShow){
		m_bShowCrtNew = bShow;

	}
	/// Setter for m_bMultiSel
	VOID set_multi_select(bool bms){
		m_bMultiSel = bms;
	}
	
	/// выдаёт первую выбранную сцену; 0 если выбрали создание новой
	qdGameScene* get_scene() const{
		return m_ptrScenes[0];
	}

	/// Возвращает указатель на массив с выбранным сценами
	qdGameScene** get_scenes() const {
		return m_ptrScenes.get();
	}
	/// Количество элементов в массиве выбранных сцен
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
	/// Вставляет необходимые столбци в контрол
	bool InsertColumns();
	/// Загружает список сцен
	bool LoadSceneList();
	/// Выравнивает ширину столбцов по ширине текста
	void AdjustColumnsWidth();
	/// Вставляет отдельную сцену в список
	int InsertScene(int nNum, qdGameScene* ptrScene);

	/// Выбирает из контрола выделенные сцены в массив
	bool GetSelected();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_SCENELIST_H__722E602B_016D_47B9_AFC8_1F34BB7253D4__INCLUDED_)
