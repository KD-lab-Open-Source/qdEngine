#if !defined(AFX_BASEPLANEPAGE_H__AE46035E_A234_4B22_A12A_0A0CEFD3E377__INCLUDED_)
#define AFX_BASEPLANEPAGE_H__AE46035E_A234_4B22_A12A_0A0CEFD3E377__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BasePlanePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// BasePlanePage dialog
#include <vector>
#include <map>

class qdCamera;
class qdGameDispatcher;
class qdGameScene;
class qdGameObjectMoving;
class qdGameObjectState;

#include "basepage.H"
#include "qd_grid_zone.h"
/*!
  Свойства сетки на сцене
*/

class BasePlanePage : public CBasePage
{
	DECLARE_DYNCREATE(BasePlanePage)
		
	//! набор для сохранения старых данных
	//! Копия размера ячейки
	int		m_iCellSizeBak;
	//! Копия размера сетки в ячейках по Х
	int		m_iCellSXBak;
	//! Копия размера сетки в ячейках по У
	int		m_iCellSYBak;
	//! Копия фокуса
	int		m_iFocusBak;
	//! Копия угла наклона относительно оси Х
	int		m_iRotateXBak;
	//! Копия угла наклона относительно оси У
	int		m_iRotateYBak;
	//! Копия угла наклона относительно оси Z
	int		m_iRotateZBak;
	//! Копия расстояния от центра сетки до камеры
	int		m_iZBak;
	
	typedef std::vector<Vect2s> coords_vector_t;
	typedef std::vector<qdGridZone> gz_vector_t;
	typedef std::map<qdGameObjectState*, Vect2i> StateScrLayout;
	typedef std::map<qdGridZone const*, coords_vector_t>CoordsMap_t;

	//! Буффер памаяти, в котором храниться копия всех ячеек
	sGridCell* m_ptrBak;
	//! Буффер, в котором храняться копии всех зон
	gz_vector_t m_vecZoneBak;
	//! Индекс, хранящий пары состояние - его экранные координаты
	StateScrLayout m_state_scr_layout;
	//! Хранит для зон массивы экранных координат узлов зоны
	CoordsMap_t m_mapZoneScrCoords;

	//! установлен в true до первого показа окна
	bool	m_bInitDialog;

	//! \brief можно ли перерисовывать сцену,
	//! если сцена активна, значит можно
	bool	m_bRedraw;

	//! Указатель на камеру сцены
	qdCamera* m_ptrCam;
	//! Диспетчекр игры
	qdGameDispatcher* m_ptrGameDisp;
	//! Сцена, сетку которой правим
	qdGameScene* m_ptrScene;
	
	//! из резервной памяти в память, в которой производим изменения
	void bak2real();
	//! из памяти, в которой производим изменения, в резервную память
	void real2bak();
	
	//! применяет к камере параметры положения
	void apply_layout_param();
	//! применяет к камере параметры сетки
	void apply_grid_param();
	
	//! применяет все параметры
	void apply_params();

	//! вычисляет расстояние от центра сетки до камеры
	int calc_Z();

	//! применяем к камере расстояние от центра сетки до камеры
	void apply_Z();
	//! Перерисовывает сцену в редакторе сцены
	void redraw();

	
	//смысл такой:
	//Задача:
	//при повороте камеры сохранить положение объектов на экране.
	//Решение:
	//перед изменением параметров камеры, получаем экранные 
	//координаты объектов и сохраняем их в этих же объектах
	//меняем параметры камеры, и пересчитываем координаты объектов
	//в мировые.

	//! перещитывает координаты всех объектов в экранные
	void World2Scr4ObjsCoords();
	
	//! пересчитывает экранные координаты объектов в мировые
	void Scr2World4ObjsCoords();
	
	//! сравнивает *Bak версии параметров с изменяемыми
	bool IsCopiesEqual();
	
	//! вычисляем колличество ячеел по х и по у
	void UpdateCellsCount(int iNewCellSize);

	//! Для всех зон вызывает qdGridZone::divide_contour(...) и вычисляет заново начинку
	void DivideZones(int iShift = 1);
	//! Для всех зон вызывает qdGridZone::mult_contour(...) и вычисляет заново начинку
	void MultZones(int iShift = 1);
	//! Сдвигает зону на указанное количество ячеек по горизонтали и по вертикали
	void ShiftZones(int iXShift, int iYShift);

	//! \brief Для зон, для которых qdGridZone::state() == true, 
	//! вызывает qdGridZone::set_state(true)
	void ApplyZones();
	//! Для всех зон вызывает qdGridZone::select
	void SelectZones(bool bSelect);
	//! \brief Проверяет если отрезки в контурах, которые сольются
	//! при дальнейшем разрежении сетки. Если "да", то справшиваем у пользователя,
	//! продолжать ли операцию
	bool ZeroCheck();
	
	//! Применяет парметры положения сетки(вращение, фокус, масштаб,расстояние до камеры)
	bool applied_layout_params();
	//! Для персонажа сохраняет экранные координаты всех его состояний
	void SaveStateLayout(qdGameObjectMoving* pm);
	//! \brief Восстанавливает положение состояний на экране. 
	//! Он сбивается при смене положения сетки
	void RestoreStateLayout();
// Construction
public:
	BasePlanePage();
	~BasePlanePage();

	bool set_data(qdGameDispatcher* ptrGD, qdNamedObject* ptrGS);

	// Dialog Data
	//{{AFX_DATA(BasePlanePage)
	enum { IDD = IDD_PROPPAGE_SCENE_MAIN_PLANE };
	//! размер ячейки в пикселях
	int		m_iCellSize;
	//! количество ячеек по горизонтали
	int		m_iCellSX;
	//! количество ячеек по вертикали
	int		m_iCellSY;
	//! фокус
	int		m_iFocus;
	//! Угол наклона относительно оси Х
	int		m_iRotateX;
	//! Угол наклона относительно оси У
	int		m_iRotateY;
	//! Угол наклона относительно оси Z
	int		m_iRotateZ;
	//! Расстояние от центра сетки до камеры
	int		m_iZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(BasePlanePage)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(BasePlanePage)
	afx_msg void OnButtonUndo();
	afx_msg void OnGridLayoutParamsChange();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnGridCellParamsChange();
	afx_msg void OnBtnLoadDefs();
	afx_msg void OnBtnSaveDefs();
	afx_msg void OnDeltaposSpin8(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinScalePow(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinZOffset(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//! Для всех зон сцены вычисляет экранные координаты узлов
	void CalcGridZonesNodesScrCoords();
	//! Для зоны вычисляет экранные координаты всех ее узлов
	void CalcGridZoneNodesScrCoords(qdGridZone const* pzone);

	//! Для всех зон по экранным координатам узлов вычисляет индексы ячеек
	void RecalcGridZonesNodes();
	//! Для зоны по экранным координатам узлов вычисляет индексы ячеек
	void RecalcGridZoneNodes(qdGridZone* pzone, coords_vector_t const& vec);
private:
	//! Масштаб (qdCamera::scale_pow)
	float scalePow_;
	//! Сдвиг по Z для масштаба (qdCamera::scale_z_offset)
	float zOffset_;
	//! Копия сдвиг по Z для масштаба (qdCamera::scale_z_offset)
	float zOffsetBak_;
	//! Копия масштаба(qdCamera::scale_pow)
	float scalePowBak_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_BASEPLANEPAGE_H__AE46035E_A234_4B22_A12A_0A0CEFD3E377__INCLUDED_)
