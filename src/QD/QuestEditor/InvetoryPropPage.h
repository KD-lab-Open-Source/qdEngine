#pragma once

#include "basepage.h"
#include <memory>
#include "afxwin.h"
#include "custom controls\cjcolorpicker.h"

// InvetoryPropPage dialog
class qdInventory;
class qdGameDispatcher;

/**
	Страница свойств инвентори
 */
class InventoryPropPage : public CBasePage
{
	DECLARE_DYNAMIC(InventoryPropPage)

public:
	InventoryPropPage();
	virtual ~InventoryPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INVENTORY_PROPS };

	/// Назначает диспетчер игры и обрабатываемый инвентори
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
	/// Парамерты инвентори в контролы диалога
	void inv2wnd();
	/// Данные из контролов диалога в инвентори
	void wnd2inv();
	/// Загружает курсоры
	void LoadCursors();
private:
	/// Обрабатываемый инвентори
	qdInventory* m_pInv;
	/// Диспетчер проекта
	qdGameDispatcher *m_ptrGameDisp;

	//! чей курсор использовать
	int m_iCursorPolicy;
	/// Состояние флага qdInventory::INV_TAKE_TO_MOUSE
	BOOL m_bTakeToMouse;
	/// Состояние флага qdInventory::INV_DONT_OPEN_AFTER_TAKE
	BOOL m_bDontOpenAfterTake;
	/// Состояние флага qdInventory::INV_PUT_OBJECT_AFTER_CLICK
	BOOL m_bPutObjectAfterClick;
	/// Состояние флага qdInventory::INV_ENABLE_RIGHT_CLICK
	BOOL m_bPutToInventoryByRClick;
	/// Состояние флага qdInventory::INV_VISIBLE_WHEN_INACTIVE
	BOOL visibleWhenInactive_;
	/// Цвет тени
	CCJColorPicker shadowColor_;
	/// прозрачность
	int transparency_;

	/// True - если идет инициализация диалога
	bool m_bInitDialog;

	int additionalXCells_;
	int additionalYCells_;
};
