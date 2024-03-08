#include "stdafx.h"
#include "resource.h"
#include "movementtype.h"

MovementType::MovementType(MovementType_t type, UINT uDescriptionId):
  m_type(type)
, m_uDescriptionId(uDescriptionId)
{
}

MovementType::~MovementType(void)
{
}

CString MovementType::description() const
{
	return CString(reinterpret_cast<LPTSTR>(m_uDescriptionId));
}
