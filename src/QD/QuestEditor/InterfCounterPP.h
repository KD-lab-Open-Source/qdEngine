#pragma once
#include "basepage.h"

#include "custom controls\CJFlatComboBox.h"
#include "custom controls\CJColorPicker.h"

// InterfCounterPP dialog
class qdInterfaceCounter;

/*!
	Класс диалога свойств для интерфейсного счётчика
*/
class InterfCounterPP : public CBasePage
{
	DECLARE_DYNAMIC(InterfCounterPP)

public:
	InterfCounterPP();
	virtual ~InterfCounterPP();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_COUNTER_PROP };

	bool set_data(qdInterfaceCounter* ptrBack);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()

private:

	//! Данные из объекта в диалог
	void ele2wnd();
	//! данные из диалога в объект
	void wnd2ele();

	bool loadCounters(CCJFlatComboBox& box);
	bool loadFonts(CCJFlatComboBox& box);

	void OnChangePos();

private:
	//! Х
	int m_iX;
	//! У
	int m_iY;
	//! Глубина
	int m_iD;

	/// количество разрядов для отображения
	int digits_;

	//! Контрол выбора цвета текста
	CCJColorPicker textColorWnd_;
	//! Список фонтов
	CCJFlatComboBox fontsWnd_;

	/// Контрол для выбора счетчика
	CCJFlatComboBox counters_;

	//! Исходный объект
	qdInterfaceCounter* m_ptrCounter;
	//! Копия для того, чтобы была возможность октатить
	std::auto_ptr<qdInterfaceCounter> m_ptrCopy;
};
