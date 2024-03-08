#pragma	once
/********************************************************************
	created:	2003/02/08
	created:	8:2:2003   16:37
	filename: 	d:\projects\quest\questeditor\isceneviewmode.h
	file path:	d:\projects\quest\questeditor
	file base:	isceneviewmode
	file ext:	h
	author:		Илюха
	
	purpose:	интерфейс смены поведния для окна редактирования сцены
*********************************************************************/

class CChildView;
interface ISceneViewMode
{
	virtual bool Init(CChildView* pview, void *data) = 0;
	virtual bool Deinit() = 0;

	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) = 0;
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) = 0;
	virtual void OnLButtonDown(UINT nFlags, const CPoint& point) = 0;
	virtual void OnLButtonUp(UINT nFlags, const CPoint& point) = 0;
	virtual void OnLButtonDblClk(UINT nFlags, const CPoint& point) = 0;
	virtual void OnRButtonDown(UINT nFlags, const CPoint& point) = 0;
	virtual void OnRButtonUp(UINT nFlags, const CPoint& point) = 0;
	virtual void OnMouseMove(UINT nFlags, const CPoint &point) = 0;
	virtual bool OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) = 0;
	virtual void Draw(HDC dc) = 0;
	virtual ~ISceneViewMode() = 0;
};

inline ISceneViewMode::~ISceneViewMode()
{}
inline void ISceneViewMode::Draw(HDC dc){};
