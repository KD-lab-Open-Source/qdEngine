#pragma once

///	Пробегает по всем объектам-копиям глобального и обновляет их представление в дереве
class TreeLogic;
class CopyOfGlobalUpdater
{
	CopyOfGlobalUpdater(void);
	~CopyOfGlobalUpdater(void);
public:
	static void update(TreeLogic& logic, qdGameObject const* globalObject);
};
