/********************************************************************
	created:	2003/05/21
	created:	21:5:2003   13:46
	filename: 	d:\Projects\Quest\QuestEditor\ITEDrawer.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	ITEDrawer
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	Базовый интерфейс для отрисовщика
*********************************************************************/

#pragma once

interface IDrawData;

class TEWnd;
/**
	Базовый интерфейс дровера для РР
 */
interface ITEDrawer
{
	/// Отрисовывает данные переданные через \a pdd
	/**
		@param pdc	- dc окна, в котором будем рисовать
		@param pwnd - окно редактора триггеров
		@param pdd	- Данные для отрисовки
		@param rcPaint - прямоугольник, который надо отрисовать
	 */
	virtual void draw(CDC* pdc, TEWnd* pwnd, 
						IDrawData* pdd, CRect const& rcPaint) const = 0;
	/**
		Предназначена для отрисовки перетаскиваемого прямоугольника
		@param dc	- dc окна, в котором будем рисовать
		@param r - прямоугольник, который надо отрисовать
		@param pen - карандаш для отрисовки
	 */
	virtual void drawDraggedRect(HDC dc, CRect const&r, HPEN pen) const = 0;
protected:
	inline ~ITEDrawer();
};
inline ITEDrawer::~ITEDrawer()
{}
