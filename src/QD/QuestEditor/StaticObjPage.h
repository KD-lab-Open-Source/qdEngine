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
	Свойства статического объекта
 */
class StaticObjPage : public CBasePage
{
	DECLARE_DYNCREATE(StaticObjPage)

	/// true до первого показа окна
	bool m_bInitDialog;

	/// Диспетчер проекта
	qdGameDispatcher*	m_ptrGameDisp;
	/// Сцена, владеющая объектом
	qdGameScene*		m_ptrScene;
	/// Объект
	qdGameObjectStatic* m_ptrObj;

	bool	m_bRedraw;///<  ли перерисовывать сцену
	//если сцена активна, значит можно
	
	/// Применяет все параметры
	void apply_params();
	/// Применяет парметры относящиеся к поизиции объекта
	void apply_pos_params();
	/// Применяет новое изображение
	void apply_picture();
	// Construction

	/// для сохранения
	int		m_iScrXBak;
	int		m_iScrYBak;
	int		m_iXBak;
	int		m_iYBak;
	int		m_iZBak;
	int		m_iDepthBak;
	int		m_bImmovableBak;

	enum eConvertDir{CD_NONE, ///<  не надо конвертировать
		CD_WORLD_TO_SCREEN, ///<  в экранные
		CD_SCREEN_TO_WORLD, ///<  в мировые
	};
	//! сопособ конвертации изменненых значений
	eConvertDir m_eConvertDir;


	/// Сохраняет предыдущее имя файла изображения
	CString	m_strPicFileNameBak;
	
	/// Делает резервную копию исходных данных
	void real2bak();
	/// Восстанавливает данные из резервной копии
	void bak2real();

	/// Конвертирует координаты и назначает их объекту
	void screen2world();
	/// Конвертинует координаты
	Vect3f screen2world(int x, int y, int z);

	/// Конвертирует координаты и назначает их объекту
	void world2screen();
	/// Конвертинует координаты
	void world2screen(Vect3f pos, int& x, int& y, int &z);
	
	/// Дизейблит контролы для координат
	void DisablePositionCtrls();

	/// устанавливает интервалы и позицию спинов
	void AdjustSpins();

	/// из координат центра координаты левого верхнего угла
	Vect3f c2lt(Vect3f pos);
	/// из координат левого верхнего угла координаты центра
	Vect3f lt2c(Vect3f pos);

	/// Параметры объекта в контролы диалога
	void obj2scr();
public:
	StaticObjPage();
	~StaticObjPage();

	/// Устанавлвает диспетчер проекта и обрабатываемый объект
	bool set_data(qdGameDispatcher* ptrGD, qdGameObjectStatic* ptrObj);

	/// Открывает диалог для выбора файлов изображений
	/**
		Возвращает имя первого выбранного файла. Если ptrVect!=NULL, то складывает
		туда остальные
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
	/// Признак того, что объект не сдвигается
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
	/// Берет измененные экранные координаты и применяет их к объекту
	afx_msg void OnEnterInScrPos();
	/// Берет измененные мировые координаты и применяет их к объекту
	afx_msg void OnEnterInWorldPos();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	/// Выбирает и применяет изображение к объекту
	afx_msg void OnSelectBtn();
	afx_msg void OnUseLeftTop();
	afx_msg void OnCheckImmovable();
	/// Пересчитывает положение объекта
	afx_msg void OnEnterInNetProjScrD();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	/// Берет измененные экранные координаты и применяет их к объекту
	afx_msg void OnEnKillfocusInScrPos();
	/// Берет измененные мировые координаты и применяет их к объекту
	afx_msg void OnEnKillfocusInWorldPos();
	afx_msg void OnEnChangeInScrPos();
	afx_msg void OnEnChangeInWorldPos();
	/// Пересчитывает положение объекта
	afx_msg void OnDeltaposSpinNetProjScrD(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	/// Пытается правильно сдвинуть объект по изменению глубины его проекции на экран
	void changeNetProjScrD(int delta);
	/// Перерисовывает сцену в редакторе сцены
	bool redraw();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_STATICOBJPAGE_H__0740096A_85AB_469B_AAFA_DC2FB61C7AE4__INCLUDED_)
