#pragma once

// Содержит общие функции для диалогов создания условий
#include "stdafx.h"
#include "qd_game_scene.h"

namespace CondUsuals
{	enum eShowHide{
		SH_NONE, ///< Просто загрузить сцены 
		SH_SHOWHIDE///< \brief Загрузить сцены и, если сцена известна скрыть 
					///< комбобокс сцены.
	};
	//! Загружает сцены в комбобокс
	bool LoadScenes(CCJFlatComboBox& box, CWnd& sceneStatic);

	//! Загружает объекты из заданной сцены в комбобокс
	bool LoadAnimObjects(CCJFlatComboBox& wndCombo, const qdGameScene* ptrScene);

	//! Возвращает указатель выбранного в комбобоксе элемента, при неудаче - NULL
	const void* GetSelPtr(const CCJFlatComboBox& wndCombo);
}