#pragma once
#include "MovementType.h"
#include <boost/shared_ptr.hpp>

/*!
 	��������� ���������� ������ �������� ���������
*/
class MovementTypeManager
{
	typedef boost::shared_ptr<MovementType> MovementTypePtr;
public:
	MovementTypeManager(void);
	~MovementTypeManager(void);
	/// ���������� �������
	size_t size() const;
	/// ����� �� �������
	MovementType* operator[](int idx) const;
	/// ����� �� ��������������
	MovementType* operator[](qdGameObjectStateWalk::movement_type_t type) const;
private:
	/// ������ �������
	std::vector<MovementTypePtr> m_types;
};
