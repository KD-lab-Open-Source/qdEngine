#pragma once
#include "MovementType.h"
#include <boost/shared_ptr.hpp>

/*!
 	Управляет возможными типами движения персонажа
*/
class MovementTypeManager
{
	typedef boost::shared_ptr<MovementType> MovementTypePtr;
public:
	MovementTypeManager(void);
	~MovementTypeManager(void);
	/// количество режимов
	size_t size() const;
	/// режим по индексу
	MovementType* operator[](int idx) const;
	/// режим по идентификатору
	MovementType* operator[](qdGameObjectStateWalk::movement_type_t type) const;
private:
	/// Список режимов
	std::vector<MovementTypePtr> m_types;
};
