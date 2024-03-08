/********************************************************************
	created:	2003/05/21
	created:	21:5:2003   15:40
	filename: 	d:\Projects\Quest\QuestEditor\TEWorkModeBase.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TEWorkModeBase
	file ext:	h
	Powerd by:	����� ��� ������� ����������� � ��������� ��������� of Scmorr
	
	purpose:	������� ����������. ����������� ������ �� ������, ����� �����
				����� ��� ���� �������
*********************************************************************/

#pragma once
#include "ITEWorkMode.h"


/**
	������� ����������. ����������� ������ �� ������, ����� �����
	����� ��� ���� �������
 */
class TEWorkModeBase : public ITEWorkMode
{
protected:

public:
	TEWorkModeBase(void);
	virtual ~TEWorkModeBase(void) = 0;

	/**
		��� �������������� �������� ������ ������
	 */
	void OnLButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point);
	/**
		��� �������������� �������� ��������������� ������
	 */
	void OnLButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point);
	void OnLButtonDblClk(TEWnd* pwnd, UINT nFlags, CPoint point);

	void OnRButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point);
	void OnRButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point);

	/**
		����������� ������� ���� ������� trackPopupMenu
	 */
	void OnMouseMove(TEWnd* pwnd, UINT nFlags, CPoint point);
	BOOL OnSetCursor(TEWnd* pwnd, CWnd* pWnd, UINT nHitTest, UINT message);

	/**
		������������ ������� � ������������� ������
	 */
	void OnKeyDown(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags);
	/**
		������������ ������� � ������������� ������
	 */
	void OnKeyUp(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags);

	void OnPaint(TEWnd* pwnd, CDC* pdc, CRect const& rcPaint);
	BOOL OnCommand(TEWnd* pwnd, WPARAM wParam, LPARAM lParam);

	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis);

	void OnShowWindow(TEWnd* pwnd, BOOL bShow, UINT nStatus);

	bool OnStart();
	bool OnEnd();

	BOOL OnDropData(TEWnd* pwnd, VOID* pData, const CPoint& point);
	bool CanDrop(TEWnd* pwnd, VOID* pData, const CPoint& point);

	virtual bool OnCommandUpdateUI(TEWnd *pwnd, CCmdUI* pCmdUI);
	virtual void OnSettingsChange();
	virtual void clearUndobuffer();
protected:
	/// �������� ����������� ���� � id
	BOOL trackPopupMenu(UINT id, TEWnd* pwnd);
	/// \brief ���������� trackPopupMenu ��� ��������� ����(������� ��������, 
	/// ���������� ������������ � �.�). ��� ���������������
	virtual void ModifyMenu(CMenu* pmenu, UINT menuResId, TEWnd* pwnd);
	/// ������ �� ������. ��� ���������� � �����������
	virtual void trackToolTip(TEWnd* pwnd, CPoint point);
protected:
	/// ��������� ������� �����
	void setBasePoint(CPoint const& point);
	/// ���������� ������� �����
	CPoint const& getBasePoint() const{
		return m_base_point;
	}
	/// Setter ��� m_bCanDragContent
	void setCanDragContent(bool bVal);
	/// Getter ��� m_bCanDragContent
	bool getCanDragContent() const{
		return m_bCanDragContent;
	}
	void setCursor(HCURSOR hCursor);
	HCURSOR getCursor() const{
		return m_hCursor;
	}

private:
	/// ����� ����, � ������� ��������� ���������� �������
	CPoint m_base_point;
	/// ����� �� ������������� ���������� ����
	bool m_bCanDragContent;
	/// ������ ��� ������
	HCURSOR m_hCursor;
};
