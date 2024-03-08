#include "stdafx.h"
#include "resource.h"
#include "movementtypemanager.h"

MovementTypeManager::MovementTypeManager(void)
{
	m_types.reserve(20);
	typedef MovementTypePtr Ptr;
	//! ходит только влево
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_LEFT,
		IDS_MOVEMENT_LEFT)));
		//! ходит только вверх
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_UP,
		IDS_MOVEMENT_UP)));
		//! ходит только вправо
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_RIGHT,
		IDS_MOVEMENT_RIGHT)));
		//! ходит только вниз
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_DOWN,
		IDS_MOVEMENT_DOWN)));
		//! ходит только по горизонтали
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_HORIZONTAL,
		IDS_MOVEMENT_HORIZONTAL)));
		//! ходит только по вертикали
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_VERTICAL,
		IDS_MOVEMENT_VERTICAL)));
		//! ходит по четырем направлениям
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_FOUR_DIRS,
		IDS_MOVEMENT_FOUR_DIRS)));
		//! ходит по восьми направлениям
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_EIGHT_DIRS,
		IDS_MOVEMENT_EIGHT_DIRS)));
		//! ходит по восьми и больше направлениям со сглаживанием
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_SMOOTH,
		IDS_MOVEMENT_SMOOTH)));

	//! ходит только влево-вверх
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_UP_LEFT,
		IDS_MOVEMENT_UP_LEFT)));
		//! ходит только вправо-вверх
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_UP_RIGHT,
		IDS_MOVEMENT_UP_RIGHT)));
		//! ходит только вправо-вниз
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_DOWN_RIGHT,
		IDS_MOVEMENT_DOWN_RIGHT)));
		//! ходит только влево-вниз
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_DOWN_LEFT,
		IDS_MOVEMENT_DOWN_LEFT)));
}

MovementTypeManager::~MovementTypeManager(void)
{
}

MovementType* MovementTypeManager::operator[](int idx) const{
	return m_types[idx].get();
}

MovementType* MovementTypeManager::operator[](
	qdGameObjectStateWalk::movement_type_t type) const
{
	return m_types[static_cast<int>(type - qdGameObjectStateWalk::MOVEMENT_LEFT)].get();
}

size_t MovementTypeManager::size() const{
	return m_types.size();
}
