#pragma once

///	��������� �� ���� ��������-������ ����������� � ��������� �� ������������� � ������
class TreeLogic;
class CopyOfGlobalUpdater
{
	CopyOfGlobalUpdater(void);
	~CopyOfGlobalUpdater(void);
public:
	static void update(TreeLogic& logic, qdGameObject const* globalObject);
};
