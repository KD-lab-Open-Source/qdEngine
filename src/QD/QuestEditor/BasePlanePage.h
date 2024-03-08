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
  �������� ����� �� �����
*/

class BasePlanePage : public CBasePage
{
	DECLARE_DYNCREATE(BasePlanePage)
		
	//! ����� ��� ���������� ������ ������
	//! ����� ������� ������
	int		m_iCellSizeBak;
	//! ����� ������� ����� � ������� �� �
	int		m_iCellSXBak;
	//! ����� ������� ����� � ������� �� �
	int		m_iCellSYBak;
	//! ����� ������
	int		m_iFocusBak;
	//! ����� ���� ������� ������������ ��� �
	int		m_iRotateXBak;
	//! ����� ���� ������� ������������ ��� �
	int		m_iRotateYBak;
	//! ����� ���� ������� ������������ ��� Z
	int		m_iRotateZBak;
	//! ����� ���������� �� ������ ����� �� ������
	int		m_iZBak;
	
	typedef std::vector<Vect2s> coords_vector_t;
	typedef std::vector<qdGridZone> gz_vector_t;
	typedef std::map<qdGameObjectState*, Vect2i> StateScrLayout;
	typedef std::map<qdGridZone const*, coords_vector_t>CoordsMap_t;

	//! ������ �������, � ������� ��������� ����� ���� �����
	sGridCell* m_ptrBak;
	//! ������, � ������� ��������� ����� ���� ���
	gz_vector_t m_vecZoneBak;
	//! ������, �������� ���� ��������� - ��� �������� ����������
	StateScrLayout m_state_scr_layout;
	//! ������ ��� ��� ������� �������� ��������� ����� ����
	CoordsMap_t m_mapZoneScrCoords;

	//! ���������� � true �� ������� ������ ����
	bool	m_bInitDialog;

	//! \brief ����� �� �������������� �����,
	//! ���� ����� �������, ������ �����
	bool	m_bRedraw;

	//! ��������� �� ������ �����
	qdCamera* m_ptrCam;
	//! ���������� ����
	qdGameDispatcher* m_ptrGameDisp;
	//! �����, ����� ������� ������
	qdGameScene* m_ptrScene;
	
	//! �� ��������� ������ � ������, � ������� ���������� ���������
	void bak2real();
	//! �� ������, � ������� ���������� ���������, � ��������� ������
	void real2bak();
	
	//! ��������� � ������ ��������� ���������
	void apply_layout_param();
	//! ��������� � ������ ��������� �����
	void apply_grid_param();
	
	//! ��������� ��� ���������
	void apply_params();

	//! ��������� ���������� �� ������ ����� �� ������
	int calc_Z();

	//! ��������� � ������ ���������� �� ������ ����� �� ������
	void apply_Z();
	//! �������������� ����� � ��������� �����
	void redraw();

	
	//����� �����:
	//������:
	//��� �������� ������ ��������� ��������� �������� �� ������.
	//�������:
	//����� ���������� ���������� ������, �������� �������� 
	//���������� �������� � ��������� �� � ���� �� ��������
	//������ ��������� ������, � ������������� ���������� ��������
	//� �������.

	//! ������������ ���������� ���� �������� � ��������
	void World2Scr4ObjsCoords();
	
	//! ������������� �������� ���������� �������� � �������
	void Scr2World4ObjsCoords();
	
	//! ���������� *Bak ������ ���������� � �����������
	bool IsCopiesEqual();
	
	//! ��������� ����������� ����� �� � � �� �
	void UpdateCellsCount(int iNewCellSize);

	//! ��� ���� ��� �������� qdGridZone::divide_contour(...) � ��������� ������ �������
	void DivideZones(int iShift = 1);
	//! ��� ���� ��� �������� qdGridZone::mult_contour(...) � ��������� ������ �������
	void MultZones(int iShift = 1);
	//! �������� ���� �� ��������� ���������� ����� �� ����������� � �� ���������
	void ShiftZones(int iXShift, int iYShift);

	//! \brief ��� ���, ��� ������� qdGridZone::state() == true, 
	//! �������� qdGridZone::set_state(true)
	void ApplyZones();
	//! ��� ���� ��� �������� qdGridZone::select
	void SelectZones(bool bSelect);
	//! \brief ��������� ���� ������� � ��������, ������� ��������
	//! ��� ���������� ���������� �����. ���� "��", �� ����������� � ������������,
	//! ���������� �� ��������
	bool ZeroCheck();
	
	//! ��������� �������� ��������� �����(��������, �����, �������,���������� �� ������)
	bool applied_layout_params();
	//! ��� ��������� ��������� �������� ���������� ���� ��� ���������
	void SaveStateLayout(qdGameObjectMoving* pm);
	//! \brief ��������������� ��������� ��������� �� ������. 
	//! �� ��������� ��� ����� ��������� �����
	void RestoreStateLayout();
// Construction
public:
	BasePlanePage();
	~BasePlanePage();

	bool set_data(qdGameDispatcher* ptrGD, qdNamedObject* ptrGS);

	// Dialog Data
	//{{AFX_DATA(BasePlanePage)
	enum { IDD = IDD_PROPPAGE_SCENE_MAIN_PLANE };
	//! ������ ������ � ��������
	int		m_iCellSize;
	//! ���������� ����� �� �����������
	int		m_iCellSX;
	//! ���������� ����� �� ���������
	int		m_iCellSY;
	//! �����
	int		m_iFocus;
	//! ���� ������� ������������ ��� �
	int		m_iRotateX;
	//! ���� ������� ������������ ��� �
	int		m_iRotateY;
	//! ���� ������� ������������ ��� Z
	int		m_iRotateZ;
	//! ���������� �� ������ ����� �� ������
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

	//! ��� ���� ��� ����� ��������� �������� ���������� �����
	void CalcGridZonesNodesScrCoords();
	//! ��� ���� ��������� �������� ���������� ���� �� �����
	void CalcGridZoneNodesScrCoords(qdGridZone const* pzone);

	//! ��� ���� ��� �� �������� ����������� ����� ��������� ������� �����
	void RecalcGridZonesNodes();
	//! ��� ���� �� �������� ����������� ����� ��������� ������� �����
	void RecalcGridZoneNodes(qdGridZone* pzone, coords_vector_t const& vec);
private:
	//! ������� (qdCamera::scale_pow)
	float scalePow_;
	//! ����� �� Z ��� �������� (qdCamera::scale_z_offset)
	float zOffset_;
	//! ����� ����� �� Z ��� �������� (qdCamera::scale_z_offset)
	float zOffsetBak_;
	//! ����� ��������(qdCamera::scale_pow)
	float scalePowBak_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_BASEPLANEPAGE_H__AE46035E_A234_4B22_A12A_0A0CEFD3E377__INCLUDED_)
