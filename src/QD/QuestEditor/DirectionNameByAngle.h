#pragma once

/// Класс утилита, козвращет по имени угла имя направления
class DirectionNameByAngle
{
	DirectionNameByAngle(void);
	~DirectionNameByAngle(void);
public:
	static CString get(float fAngle);
};
