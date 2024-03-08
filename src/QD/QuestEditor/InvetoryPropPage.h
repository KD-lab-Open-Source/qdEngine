#pragma once

#include "basepage.h"
#include <memory>
#include "afxwin.h"
#include "custom controls\cjcolorpicker.h"

// InvetoryPropPage dialog
class qdInventory;
class qdGameDispatcher;

/**
	�������� ������� ���������
 */
class InventoryPropPage : public CBasePage
{
	DECLARE_DYNAMIC(InventoryPropPage)

public:
	InventoryPropPage();
	virtual ~InventoryPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INVENTORY_PROPS };

	/// ��������� ��������� ���� � �������������� ���������
	void set_data(qdGameDispatcher* ptrDisp, qdInventory* ptrSource);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedCheck();
	afx_msg void OnDeltaposSpinTransparency(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditTransparency();
	afx_msg void OnEnChangeEditAdditX();
	afx_msg void OnEnChangeEditAdditY();
	LRESULT OnColorPicSelChange(WPARAM, LPARAM);
private:
	/// ��������� ��������� � �������� �������
	void inv2wnd();
	/// ������ �� ��������� ������� � ���������
	void wnd2inv();
	/// ��������� �������
	void LoadCursors();
private:
	/// �������������� ���������
	qdInventory* m_pInv;
	/// ��������� �������
	qdGameDispatcher *m_ptrGameDisp;

	//! ��� ������ ������������
	int m_iCursorPolicy;
	/// ��������� ����� qdInventory::INV_TAKE_TO_MOUSE
	BOOL m_bTakeToMouse;
	/// ��������� ����� qdInventory::INV_DONT_OPEN_AFTER_TAKE
	BOOL m_bDontOpenAfterTake;
	/// ��������� ����� qdInventory::INV_PUT_OBJECT_AFTER_CLICK
	BOOL m_bPutObjectAfterClick;
	/// ��������� ����� qdInventory::INV_ENABLE_RIGHT_CLICK
	BOOL m_bPutToInventoryByRClick;
	/// ��������� ����� qdInventory::INV_VISIBLE_WHEN_INACTIVE
	BOOL visibleWhenInactive_;
	/// ���� ����
	CCJColorPicker shadowColor_;
	/// ������������
	int transparency_;

	/// True - ���� ���� ������������� �������
	bool m_bInitDialog;

	int additionalXCells_;
	int additionalYCells_;
};
