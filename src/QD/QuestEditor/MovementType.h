#pragma once
#include "qd_game_object_state.h"

/**
	��������� ��� �������� ���������
 */
class MovementType
{
	typedef qdGameObjectStateWalk::movement_type_t MovementType_t;
public:
	MovementType(MovementType_t type, UINT uDescriptionId);
	~MovementType(void);
	/// ������������� ���� �������� ���������
	MovementType_t movement_type() const {
		return m_type;
	}
	/// �������� ���� ��������
	CString description() const;
private:
	/// ������������� ����
	MovementType_t m_type;
	/// ������ �������� � ���������
	UINT m_uDescriptionId;
};
