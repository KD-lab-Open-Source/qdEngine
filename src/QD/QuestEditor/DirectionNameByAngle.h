#pragma once

/// ����� �������, ��������� �� ����� ���� ��� �����������
class DirectionNameByAngle
{
	DirectionNameByAngle(void);
	~DirectionNameByAngle(void);
public:
	static CString get(float fAngle);
};
