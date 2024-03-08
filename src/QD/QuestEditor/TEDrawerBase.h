/********************************************************************
	created:	2003/05/23
	created:	23:5:2003   13:19
	filename: 	d:\Projects\Quest\QuestEditor\TEDrawerBase.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TEDrawerBase
	file ext:	h
	Powerd by:	Илюха при идейном руководстве и моральной поддержке of Scmorr
	
	purpose:	базовая реализация интерфейса ITEDrawer
*********************************************************************/

#pragma once
#include "itedrawer.h"

/**
	Базовая реализация интерфейса ITEDrawer
	Функции ничего не делают
 */
class TEDrawerBase : public ITEDrawer
{
public:
	TEDrawerBase(void);
	~TEDrawerBase(void);
	virtual void draw(CDC* pdc, TEWnd* pwnd, CRect const& rcPaint) const;
	virtual void drawDraggedRect(HDC dc, CRect const&r, HPEN hpen) const;
};
