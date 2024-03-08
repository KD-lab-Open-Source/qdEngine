#ifndef RECT_CHANGE_LISTENER_H
#define RECT_CHANGE_LISTENER_H

#pragma once

class CustomRectTracker;
/**
	Класс реализующий этот интерфейс, может получать уведомления от
	CustomRectTracker о том, что прямоугольник изменился
	Используется совместно с CustomRectTracker
 */
interface IRectChangeListener
{
	/// Callback на измение размеров прямоугольника
	virtual void onRectChange(CustomRectTracker& rectTracker, CRect const& oldRect) = 0;
};

#endif//RECT_CHANGE_LISTENER_H