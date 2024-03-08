#pragma once
#include <afxext.h>

#include "IRectChangeListener.h"

/// Отрисовывает тянущийся прямоугольник
/**
	Что умеет см в MSDN CRectTracker. Сделан только для того, чтобы была возможность
	подписаться на событие об изменении размера
 */
class CustomRectTracker : public CRectTracker
{
public:
	CustomRectTracker(IRectChangeListener* rectChangeListener);
	~CustomRectTracker(void);

	/// Отсылает уведомление
	virtual void OnChangedRect(const CRect& rectOld);
	/// ничего не делает
	virtual void DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd);
private:
	/// Подписчик на событие "Измениние размера прямоугольника"
	IRectChangeListener* rectChangeListener_;
};
