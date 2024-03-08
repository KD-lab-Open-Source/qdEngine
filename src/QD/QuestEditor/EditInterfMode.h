#pragma once
#include "isceneviewmode.h"
#include "editinterfmodedata.h"

class qdInterfaceElement;
class qdInterfaceScreen;
class qdInterfaceSlider;
class qdInterfaceSave;
class qdInterfaceTextWindow;

class EditInterfMode :
	public ISceneViewMode
{
public:
	EditInterfMode(void);
	~EditInterfMode(void);

	bool Init(CChildView* pview, void *data);
	bool Deinit();
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnLButtonDown(UINT nFlags, const CPoint& point);
	void OnLButtonUp(UINT nFlags, const CPoint& point);
	void OnLButtonDblClk(UINT nFlags, const CPoint& point);
	void OnRButtonDown(UINT nFlags, const CPoint& point);
	void OnRButtonUp(UINT nFlags, const CPoint& point);
	void OnMouseMove(UINT nFlags, const CPoint &point);
	bool OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	void Draw(HDC dc);

protected:
	void CustomDraw(qdInterfaceElement const* pele) const;
	void CustomDrawSlider(qdInterfaceSlider const* psldr) const;
	void CustomDrawSave(qdInterfaceSave const* psldr) const;
	void CustomDrawTextWindow(qdInterfaceTextWindow const* textWindow) const;

	void DrawSelectedEle(const qdInterfaceElement* pele) const;
	qdInterfaceScreen* selected_screen() const;
	//! ведущий элемент в группе выбранных
	qdInterfaceElement* get_leading_selected() const;
	void DispatchOnLBtnDown(UINT nFlags, const CPoint& point);
	inline void DispatchOnMM(UINT nFlags, CPoint const& point);

	void OnLBtnDownBckgrnd(UINT nFlags, const CPoint& point);
	void OnLBtnDownSave(UINT nFlags, const CPoint& point);
	void OnLBtnDownSlider(UINT nFlags, const CPoint& point);
	void OnLBtnDownBtn(UINT nFlags, const CPoint& point);
	void OnLBtnDownTextWindow(UINT nFlags, const CPoint& point);

	void OnMMBckgrnd(UINT nFlags,	const CPoint& point);
	void OnMMSave(UINT nFlags,		const CPoint& point);
	void OnMMSlider(UINT nFlags,	const CPoint& point);
	void OnMMBtn(UINT nFlags,		const CPoint& point);
	void OnMMTextWindow(UINT nFlags,const CPoint& point);
private:
	CChildView* m_ptrView;
	CPoint m_base_point;
	bool m_bCanDrag;
	bool m_bWasMouseMove;
	int m_iActiveAnchor;
};
