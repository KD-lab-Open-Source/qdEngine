#pragma once
#include "qd_game_object_state.h"

/**
	Описывает тип движения персонажа
 */
class MovementType
{
	typedef qdGameObjectStateWalk::movement_type_t MovementType_t;
public:
	MovementType(MovementType_t type, UINT uDescriptionId);
	~MovementType(void);
	/// Идентификатор типа двежения персонажа
	MovementType_t movement_type() const {
		return m_type;
	}
	/// Описание типа движения
	CString description() const;
private:
	/// Идентификатор типа
	MovementType_t m_type;
	/// Строка ресурсов с описанием
	UINT m_uDescriptionId;
};
