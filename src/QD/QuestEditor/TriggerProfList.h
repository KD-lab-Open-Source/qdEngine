#pragma once

#include "itriggerproflist.h"
#include <memory>

class CCoolDialogBar;
class TriggerDbgDlg;
interface ITriggerView;

/**
	Реализует интерфейс списка записей профайлера.
	Содержит все необходимое, чтобы показать список
 */
class TriggerProfList :
	public ITriggerProfList
{
public:
	TriggerProfList(void);
	~TriggerProfList(void);

	/// Создает окна диалога и панели. Увязывает их между собой
	bool Create(CFrameWnd* pParent, ITriggerView* ptrITView, 
								DWORD dwStyle = WS_CHILD |CBRS_FLYBY | 
								CBRS_RIGHT | CBRS_SIZE_DYNAMIC);

	virtual bool Show(eShowHide e)const;
	virtual bool IsVisible()const;

	virtual bool Load();
	virtual bool Next()const;
	virtual bool Prev()const;
	virtual bool Activate();
private:
	/// Диалог содержащий список с записями
	std::auto_ptr<TriggerDbgDlg> m_ptrTrigDbgDlg;
	/// Панель, позволяющая прицеплять диалог к краям окна
	std::auto_ptr<CCoolDialogBar> m_ptrDlgBar;
	/// Главное окно приложения
	CFrameWnd* m_pFrameWnd;
};
