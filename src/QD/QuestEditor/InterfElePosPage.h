#pragma once

#include "basepage.h"
#include "afxwin.h"

// InterfElePosPage dialog
class qdInterfaceDispatcher;
class qdInterfaceElement;

/**
	Страница свойст элемента редактирующая его положение на экране
 */
class InterfElePosPage : public CBasePage
{
	DECLARE_DYNAMIC(InterfElePosPage)

public:
	InterfElePosPage();
	InterfElePosPage(UINT dlgId);
	virtual ~InterfElePosPage();

	/// Назначает диспетчер интерфейса и элемент, с которым будем работать
	void set_data(qdInterfaceDispatcher* pd, qdInterfaceElement* ptrEle);
// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_ELE_POS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedCheckLeftTop();
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()

private:
	/// \brief Переносит данные о позиции из элемента в переменные
	/// связанные с контролами диалога
	void ele2wnd();
	/// Переносит данные из переменных связанных с контролами диалога в элемент
	void wnd2ele();
	/// делает резервную копию данных, чтобы можно было вернуть все назад
	void real2bak();
	/// восстанавливает данные из резервной копии
	void bak2real();

	void OnChangePos();
private:
	/// Показывать ли координаты левого верхнего угла
	BOOL m_bLeftTop;
	/// х
	int m_iX;
	/// y
	int m_iY;
	/// Глубина
	int m_iD;

	/// резервный х
	int m_iXBak;
	/// резервный у
	int m_iYBak;
	/// резервная глубина
	int m_iDBak;

	/// Диспетчер интерфейса
	qdInterfaceDispatcher* m_ptrIDisp;
	/// Элемент с позицией, которого работаем
	qdInterfaceElement* m_ptrEle;
};
