#pragma once
#include "tesinglechainworkmode.h"
#include <memory>
interface ITEDrawer;
interface ITriggerProfList;

/**
	������ � ������ �������
 */
class TEProfilerWorkMode :
	public TESingleChainWorkMode
{
public:
	TEProfilerWorkMode(SelectionManager& selectionManger);
	~TEProfilerWorkMode(void);
	/// ��������� ������
	void setDrawer(ITEDrawer* ptrDrawer);
	/// ���������� ������ ������� ��� �������
	void setTriggerProfList(ITriggerProfList* ptrtpl);
	/// ���������� ������ ������� ��� �������
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
	/// ������
	ITEDrawer *m_ptrDrawer;
	/// �����, ���������� � ������ ������ ��� ���������
	std::auto_ptr<DataKeeper> m_ptrDrawDataKeeper;
	/// ������ ���������� �������
	ITriggerProfList *m_ptrTriggerProfList;
};
