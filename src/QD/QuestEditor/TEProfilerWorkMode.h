#pragma once
#include "tesinglechainworkmode.h"
#include <memory>
interface ITEDrawer;
interface ITriggerProfList;

/**
	Работа в режиме отладки
 */
class TEProfilerWorkMode :
	public TESingleChainWorkMode
{
public:
	TEProfilerWorkMode(SelectionManager& selectionManger);
	~TEProfilerWorkMode(void);
	/// Назначает дровер
	void setDrawer(ITEDrawer* ptrDrawer);
	/// Назаначает список записей для отладки
	void setTriggerProfList(ITriggerProfList* ptrtpl);
	/// Возвращает список записей для отладки
	ITriggerProfList* getTrigerProfList() const{
		return m_ptrTriggerProfList;
	}
	virtual void OnLButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point);
	virtual void OnRButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(TEWnd* pwnd, UINT nFlags, CPoint point);
	virtual void OnKeyUp(TEWnd *pwnd, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnPaint(class TEWnd* pwnd, CDC* pdc, CRect const& rcPaint);
	virtual bool OnStart();
	virtual bool OnEnd();
	virtual void ModifyMenu(CMenu* pmenu, UINT menuResId, TEWnd* pwnd);
private:
	/// Дровер
	ITEDrawer *m_ptrDrawer;
	/// Класс, передающий в дровер данные для рисования
	std::auto_ptr<DataKeeper> m_ptrDrawDataKeeper;
	/// Список отладочных записей
	ITriggerProfList *m_ptrTriggerProfList;
};
