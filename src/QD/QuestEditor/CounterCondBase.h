#pragma once
#include "conddlgbase.h"

class qdGameDispatcher;

/**
	Базовый класс для диалогов условия связанных со счетчиками
 */
class CounterCondBase :
	public CondDlgBase
{
	DECLARE_DYNAMIC(CounterCondBase);
protected:
	DECLARE_MESSAGE_MAP();
public:
	CounterCondBase(UINT nIDTemplate, DWORD condID, UINT counterComboId);
	~CounterCondBase(void);
protected:

	UINT getCounterComboId()const{
		return m_uiCounterComboId;
	}
	void setGameDispatcher(qdGameDispatcher* ptrGameDisp){
		m_ptrGameDisp = ptrGameDisp;
	}
	qdGameDispatcher* getGameDispatcher() const{
		return m_ptrGameDisp;
	}

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	/// Перекрыли, чтобы отловить событие CBN_SELCHANGE в m_wndCounters
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	/// загружаем список счетчиков
	virtual bool loadCounters(CCJFlatComboBox& box);
	/// Прописывает коунтер в условие
	virtual void onSelChangeInCounterCombo();

	/// Достает выбранный коунтер из комбобокса
	qdCounter* getCounterFromCombo(CCJFlatComboBox& box);
protected:
	/**
		Функтор, добавляющий счетчик в комбобокс
	 */
	class AddCounterToCombo
	{
		CCJFlatComboBox& m_wndCombo;
	public:
		AddCounterToCombo(CCJFlatComboBox& box);
		void operator()(qdCounter* pc);
	};
private:
	/// Идентификатор комбо в темплейте диалога. 
	/**
		Нужен, чтобы прицепить m_wndCounters к окну диалога
	 */
	UINT m_uiCounterComboId;
	CCJFlatComboBox m_wndCounters;
	qdGameDispatcher* m_ptrGameDisp;
};
