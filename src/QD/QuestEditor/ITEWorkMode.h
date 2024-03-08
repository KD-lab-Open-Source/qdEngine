/********************************************************************
	created:	2003/05/21
	created:	21:5:2003   15:30
	filename: 	d:\Projects\Quest\QuestEditor\ITEWorkMode.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	ITEWorkMode
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	Интерфейс рабочего режима для редактора триггеров
*********************************************************************/
#pragma once

class TEWnd;

/**
	Рабочий режим редактора триггеров
 */
interface ITEWorkMode
{
	/**
		\name Обработчики стандартных сообщений
	 */
	//@{
	virtual void OnLButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point)				= 0;
	virtual void OnLButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point)				= 0;
	virtual void OnLButtonDblClk(TEWnd* pwnd, UINT nFlags, CPoint point)			= 0;

	virtual void OnRButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point)				= 0;
	virtual void OnRButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point)				= 0;

	virtual void OnMouseMove(TEWnd* pwnd, UINT nFlags, CPoint point)				= 0;
	virtual BOOL OnSetCursor(TEWnd* pwnd, CWnd* pWnd, UINT nHitTest, UINT message)  = 0;

	virtual void OnKeyDown(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags)		= 0;
	virtual void OnKeyUp(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags)		= 0;

	virtual void OnPaint(TEWnd* pwnd, CDC* pdc, CRect const& rcPaint)				= 0;
	virtual BOOL OnCommand(TEWnd* pwnd, WPARAM wParam, LPARAM lParam)				= 0;

	virtual void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis)						= 0;
	virtual void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis)				= 0;
	virtual void OnShowWindow(TEWnd* pwnd, BOOL bShow, UINT nStatus)				= 0;
	//@}
	/// начал работу
	virtual bool OnStart()															= 0;
	/// закончил
	virtual bool OnEnd()															= 0;

	/// Уведомление о брошенном каталоге
	virtual BOOL OnDropData(TEWnd* pwnd, VOID* pData, const CPoint& point)			= 0;
	/// Можем ли бросить перетягиваемый объект
	virtual bool CanDrop(TEWnd* pwnd, VOID* pData, const CPoint& point)				= 0;
	/// Регулирует состояние пунктов меню, кнопок на тулбаре и т.д.
	virtual bool OnCommandUpdateUI(TEWnd *pwnd, CCmdUI* pCmdUI)						= 0;
	/// На изменение настроек приложения
	virtual void OnSettingsChange()													= 0;
	/// Очистить undo-buffer РР
	virtual void clearUndobuffer()													= 0;
protected:
	//protected, чтобы нельзя было удалять напрямую указатель на этот интрефейс
	inline virtual ~ITEWorkMode() = 0;
};
ITEWorkMode::~ITEWorkMode(){}