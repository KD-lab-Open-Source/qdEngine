#ifndef _MAKE_COND_DLG_BASE_
#define _MAKE_COND_DLG_BASE_

#pragma once

/*!
 Интерфейс для класса-диалога, с помощью которого создаётся условие.(StateCondDlg 
 и CO_ConditionDlg)
*/

class MakeCondDlgBase{
public:
	//! Вызывается из диалогов, редактирующих данные для условия
	virtual void OnCondDataComplete(bool bComplete) = 0;
	//! Возвращает сцену, в которой находится объект, условия которого редактируются
	virtual class qdGameScene* get_scene() const = 0;
	//! Возвращает диспетчер игры
	virtual class qdGameDispatcher* get_disp() const = 0;
};

#endif
