#pragma once
#include "OneObjectCond.h"

/**
	Мышь над объектом
 */
class MouseHoverCond :
	public OneObjectCond
{
public:
	MouseHoverCond(DWORD id, CWnd* pParent = NULL);
	~MouseHoverCond(void);
protected:
	/// Записывает объект в условие
	virtual void OnSelchangeComboObject();
	/// Еще добавляет строчку "над самим собой"
	virtual void LoadObjects();
};
