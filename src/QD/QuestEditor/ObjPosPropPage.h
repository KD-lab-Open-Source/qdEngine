#if !defined(AFX_ObjPosPropPage_H__D323D681_4EC2_4928_B295_A5C10AD4D661__INCLUDED_)
#define AFX_ObjPosPropPage_H__D323D681_4EC2_4928_B295_A5C10AD4D661__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjPosPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ObjPosPropPage dialog

class qdGameDispatcher;
class qdGameScene;
class qdGameCamera;
class qdGameObjectAnimated;

#include "basepage.h"
/**
	Страница свойств позволяющая редактировать позицию объкта
 */
class ObjPosPropPage : public CBasePage
{
	DECLARE_DYNCREATE(ObjPosPropPage)

	//! true до первого показа окна
	bool m_bInitDialog;
	
	
	qdGameDispatcher		*m_ptrGameDisp;
	qdGameObject			*m_ptrObj;
	qdGameScene				*m_ptrScene;
	//! Глубина проекции объекта на сетку
	int		m_iNetProjD;
	
	//! можно ли перерисовывать сцену, если сцена активна, значит можно
	bool	m_bRedraw;

	//! Применяет параметры
	void apply_params();
	//! Применяет параметы, определяющие положение объекта
	void apply_pos_params();
	// Construction

	// резервные копии
	//! Копия экранной Х координаты
	int		m_iScrXBak;
	//! Копия экранной Y координаты
	int		m_iScrYBak;
	//! Копия Х координаты
	int 	m_iXBak;
	//! Копия Y координаты
	int 	m_iYBak;
	//! Копия Z координаты
	int 	m_iZBak;
	//! Копия глубины
	int 	m_iDepthBak;
	

	//! \brief Функция сохраняет исходные значения парметров
	//! в резервных копиях
	void real2bak();
	//! восстанавливает параметры из резервных копий
	void bak2real();
	
	//! \brief переводит экранные коородинаты в координаты игрового мира.
	//! Учитывает необходимость перевода координат в левый верхний угол.
	void screen2world();
	//! переводит экранные коородинаты в координаты игрового мира
	Vect3f screen2world(int x, int y, int z);
	
	//! обратна к screen2world
	void world2screen();
	void world2screen(Vect3f pos, int& x, int& y, int &z);
	
	//! Делает недоступным контролы отвечающие за позицию объекта
	void DisablePositionCtrls();
	
	//! устанавливает интервалы и позицию спинов
	void AdjustSpins();
	
	//! из координат центра координаты левого верхнего угла
	Vect3f c2lt(Vect3f pos);
	//! из координат левого верхнего угла координаты центра
	Vect3f lt2c(Vect3f pos);
	
	//! рассовывает данные об объекте по диалогу
	void Obj2Wnd();

	//! по изменению глубины проектции на сетку вычисляет позицию объекта
	void changeNetProjScrD(int delta);
	// Construction
public:
	ObjPosPropPage();
	~ObjPosPropPage();

	bool set_data(qdGameDispatcher* ptrGD, qdGameObject* ptrObj);
	
// Dialog Data
	//{{AFX_DATA(ObjPosPropPage)
	enum { IDD = IDD_PROPPAGE_OBJECT_POS };
	int		m_iScrX;
	int		m_iScrY;
	int		m_iX;
	int		m_iY;
	int		m_iZ;
	int		m_iDepth;
	BOOL	m_bUseLeftTop;
	BOOL	m_bFixedScreenCoords;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(ObjPosPropPage)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(ObjPosPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnUseLeftTop();
	afx_msg void OnFixedScreenCoords();
	//}}AFX_MSG
	afx_msg LRESULT OnQuerySiblings(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnChangeWorldPos();
	afx_msg void OnEnChangeScrPos();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()
	
	//! перерысовыет окно редактора сцены
	bool redraw();
	//! Нажали ENTER в контролах, редактрирующих экранную позицию
	void OnEnterInScrPos();
	//! Нажали ENTER в контролах, редактрирующих мировую позицию
	void OnEnterInWorldPos();
	//! Нажали ENTER в контроле, редактрирующем глубину проекции на сетку
	void OnEnterInNetProjScrD();

	afx_msg void OnEnKillfocusInScrPos();
	afx_msg void OnEnKillfocusInWorldPos();
	afx_msg void OnDeltaposSpinNetProjScrD(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_ObjPosPropPage_H__D323D681_4EC2_4928_B295_A5C10AD4D661__INCLUDED_)
