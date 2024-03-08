/********************************************************************
	created:	2003/05/21
	created:	21:5:2003   15:40
	filename: 	d:\Projects\Quest\QuestEditor\TEWorkModeBase.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TEWorkModeBase
	file ext:	h
	Powerd by:	Илюха при идейном руководстве и моральной поддержке of Scmorr
	
	purpose:	Базовая реализация. Практически ничего не делает, кроме вещей
				общих для всех режимов
*********************************************************************/

#pragma once
#include "ITEWorkMode.h"


/**
	Базовая реализация. Практически ничего не делает, кроме вещей
	общих для всех режимов
 */
class TEWorkModeBase : public ITEWorkMode
{
protected:

public:
	TEWorkModeBase(void);
	virtual ~TEWorkModeBase(void) = 0;

	/**
		При скроллировании контента меняет курсор
	 */
	void OnLButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point);
	/**
		При скроллировании контента восстанавливает курсор
	 */
	void OnLButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point);
	void OnLButtonDblClk(TEWnd* pwnd, UINT nFlags, CPoint point);

	void OnRButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point);
	void OnRButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point);

	/**
		Скроллирует контент либо кликает trackPopupMenu
	 */
	void OnMouseMove(TEWnd* pwnd, UINT nFlags, CPoint point);
	BOOL OnSetCursor(TEWnd* pwnd, CWnd* pWnd, UINT nHitTest, UINT message);

	/**
		Обрабатывает нажатия и устанавливает курсор
	 */
	void OnKeyDown(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags);
	/**
		Обрабатывает нажатия и устанавливает курсор
	 */
	void OnKeyUp(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags);

	void OnPaint(TEWnd* pwnd, CDC* pdc, CRect const& rcPaint);
	BOOL OnCommand(TEWnd* pwnd, WPARAM wParam, LPARAM lParam);

	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis);

	void OnShowWindow(TEWnd* pwnd, BOOL bShow, UINT nStatus);

	bool OnStart();
	bool OnEnd();

	BOOL OnDropData(TEWnd* pwnd, VOID* pData, const CPoint& point);
	bool CanDrop(TEWnd* pwnd, VOID* pData, const CPoint& point);

	virtual bool OnCommandUpdateUI(TEWnd *pwnd, CCmdUI* pCmdUI);
	virtual void OnSettingsChange();
	virtual void clearUndobuffer();
protected:
	/// Показать всплывающее меню с id
	BOOL trackPopupMenu(UINT id, TEWnd* pwnd);
	/// \brief Вызывается trackPopupMenu для настройки меню(удления элеметов, 
	/// управления доступностью и т.д). Для переопределения
	virtual void ModifyMenu(CMenu* pmenu, UINT menuResId, TEWnd* pwnd);
	/// Ничего не делает. Для перекрытия в наследниках
	virtual void trackToolTip(TEWnd* pwnd, CPoint point);
protected:
	/// Назначает базовую точку
	void setBasePoint(CPoint const& point);
	/// Возвращает базовую точку
	CPoint const& getBasePoint() const{
		return m_base_point;
	}
	/// Setter для m_bCanDragContent
	void setCanDragContent(bool bVal);
	/// Getter для m_bCanDragContent
	bool getCanDragContent() const{
		return m_bCanDragContent;
	}
	void setCursor(HCURSOR hCursor);
	HCURSOR getCursor() const{
		return m_hCursor;
	}

private:
	/// точка окна, в которой произошло предудущее событие
	CPoint m_base_point;
	/// можно ли скроллировать содержание окна
	bool m_bCanDragContent;
	/// курсор для режима
	HCURSOR m_hCursor;
};
