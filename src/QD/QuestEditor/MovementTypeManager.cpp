#include "stdafx.h"
#include "resource.h"
#include "movementtypemanager.h"

MovementTypeManager::MovementTypeManager(void)
{
	m_types.reserve(20);
	typedef MovementTypePtr Ptr;
	//! ����� ������ �����
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_LEFT,
		IDS_MOVEMENT_LEFT)));
		//! ����� ������ �����
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_UP,
		IDS_MOVEMENT_UP)));
		//! ����� ������ ������
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_RIGHT,
		IDS_MOVEMENT_RIGHT)));
		//! ����� ������ ����
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_DOWN,
		IDS_MOVEMENT_DOWN)));
		//! ����� ������ �� �����������
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_HORIZONTAL,
		IDS_MOVEMENT_HORIZONTAL)));
		//! ����� ������ �� ���������
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_VERTICAL,
		IDS_MOVEMENT_VERTICAL)));
		//! ����� �� ������� ������������
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_FOUR_DIRS,
		IDS_MOVEMENT_FOUR_DIRS)));
		//! ����� �� ������ ������������
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_EIGHT_DIRS,
		IDS_MOVEMENT_EIGHT_DIRS)));
		//! ����� �� ������ � ������ ������������ �� ������������
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_SMOOTH,
		IDS_MOVEMENT_SMOOTH)));

	//! ����� ������ �����-�����
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_UP_LEFT,
		IDS_MOVEMENT_UP_LEFT)));
		//! ����� ������ ������-�����
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_UP_RIGHT,
		IDS_MOVEMENT_UP_RIGHT)));
		//! ����� ������ ������-����
	m_types.push_back(Ptr(new MovementType(qdGameObjectStateWalk::MOVEMENT_DOWN_RIGHT,
		IDS_MOVEMENT_DOWN_RIGHT)));
		//! ����� ������ �����-����
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
