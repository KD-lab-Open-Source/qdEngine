#ifndef SELECT_IFINTERSECT_H
#define SELECT_IFINTERSECT_H

#pragma once

#include "qd_trigger_element.h"

class SelectionManager;
/**
	функтор, который выделяет элементы триггера по заданному прямоугольнику
 */
class SelectIfIntersect
{
	/// Заданный прямоугольник выбора
	CRect const& rect_;
	/// Класс управляющий признаком выбранности для элементов триггера
	SelectionManager& selectionManager_;
public:
	SelectIfIntersect(CRect const& rect, SelectionManager& selectionManager);
	/// Если элемент попарадет в прямоугольник, то он выбирается
	void operator()(qdTriggerElementPtr const& ptr) const;
};

#endif