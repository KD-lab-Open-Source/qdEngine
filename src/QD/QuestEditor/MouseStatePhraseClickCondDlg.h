#pragma once

#include "ObjInStateCond.h"
// MouseStatePhraseClickCondDlg dialog

/**
	Клик по фразе
 */
class MouseStatePhraseClickCondDlg : public ObjInStateCond
{
	DECLARE_DYNAMIC(MouseStatePhraseClickCondDlg)

public:
	MouseStatePhraseClickCondDlg(DWORD id);   // standard constructor
	virtual ~MouseStatePhraseClickCondDlg();

protected:
	bool IsCondDataComplete();

	/// Загружает список состояний
	afx_msg virtual void OnCbnSelchangeComboObject();
	/// Кладет состояние в условие
	afx_msg virtual void OnCbnSelchangeComboStates();
	/// Перезагружает список объектов
	afx_msg virtual void OnCbnSelchangeComboScenes();

private:

};
