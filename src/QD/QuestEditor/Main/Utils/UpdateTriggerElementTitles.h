#pragma once

#include "qd_fwd.h"

class qdGameObjectAnimated;
/// Содержит набор функций для обновления заголовков элеметов цепочки триггеров
class UpdateTriggerElementTitles
{
	UpdateTriggerElementTitles(void);
	~UpdateTriggerElementTitles(void);
public:
	/*!
 		Пробегает по всем цепочкам и всем элементам в них. Если элемент содержит 
		переданный объект, то вызывает qdTriggerElement::update_object_reference()
		\param chainList - список цепочек триггеров
		\param pno	- Объект, для которого обновляем загловки элементов триггеров
	*/
	static void update(qdTriggerChainList const& chainList, qdNamedObject const* obj);

	/// Вызывает update для состояний объекта
	static void updateForAnimated(qdTriggerChainList const& chainList, 
		qdGameObjectAnimated const* obj);
	/// Вызывает update для состояний зоны
	static void updateForZone(qdTriggerChainList const& chainList, qdGridZone* zone);
};
