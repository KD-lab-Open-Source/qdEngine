#pragma once

#include "qd_fwd.h"
class qdGameObjectAnimated;

/// Производит во всех состояниях обновление информации о курсоре.
/**
	Просто пробегается по всем возможным состояниям и вызывает 
	qdGameObjectState::update_cursor_id
 */
class CursorIdUpdater
{
	CursorIdUpdater(void);
	~CursorIdUpdater(void);
protected:
	/// Обрабатывает список глобальных объектов
	static void processGlobalObjects(qdGameDispatcher& gameDispatcher);
	/// Обрабатывает список сцен
	static void processScenes(qdGameDispatcher& gameDispatcher);
	/// Обрабатывает все объекты из сцены
	static void processScene(qdGameScene* scene, qdGameDispatcher& gameDispatcher);
	/// Обрабатывает все состояния у объекта
	static void processObject(qdGameObjectAnimated& object, 
								qdGameDispatcher& gameDispatcher);
	/// Пробегает по списку состояний и обрабатывает их
	static void processStates(qdGameObjectStateVector const& states, 
								qdGameDispatcher& gameDispatcher);
public:
	/// Входная функция
	static void run(qdGameDispatcher& gameDispatcher);
};
