#pragma once

class TEWnd;

// MiniMapWnd

class MiniMapWnd : public CWnd
{
	DECLARE_DYNCREATE(MiniMapWnd)

public:
	MiniMapWnd();
	virtual ~MiniMapWnd();
	
	/// Trigger Editor Window, � ������� �������� �������, ��������� � ���������
	const TEWnd* teWnd() const { return teWnd_; }
	void setTEWnd(TEWnd* wnd) { teWnd_ = wnd; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	void formatToMinimap(RECT& lp_rect);
	/// ������� ������� ��������� ������ ��� ����� �� ���������
	void moveViewport(CPoint mouse_click);

	TEWnd* teWnd_;
	float scaleCoef_;
	int xShift_;
	int yShift_;
	///	����� �� ������� ������� ��������� TE ��� �������� ������ �� ���������
	bool needMoveViewport_;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};