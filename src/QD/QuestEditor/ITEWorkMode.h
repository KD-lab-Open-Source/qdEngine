/********************************************************************
	created:	2003/05/21
	created:	21:5:2003   15:30
	filename: 	d:\Projects\Quest\QuestEditor\ITEWorkMode.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	ITEWorkMode
	file ext:	h
	Powerd by:	�����
	
	purpose:	��������� �������� ������ ��� ��������� ���������
*********************************************************************/
#pragma once

class TEWnd;

/**
	������� ����� ��������� ���������
 */
interface ITEWorkMode
{
	/**
		\name ����������� ����������� ���������
	 */
	//@{
	virtual void OnLButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point)				= 0;
	virtual void OnLButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point)				= 0;
	virtual void OnLButtonDblClk(TEWnd* pwnd, UINT nFlags, CPoint point)			= 0;

	virtual void OnRButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point)				= 0;
	virtual void OnRButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point)				= 0;

	virtual void OnMouseMove(TEWnd* pwnd, UINT nFlags, CPoint point)				= 0;
	virtual BOOL OnSetCursor(TEWnd* pwnd, CWnd* pWnd, UINT nHitTest, UINT message)  = 0;

	virtual void OnKeyDown(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags)		= 0;
	virtual void OnKeyUp(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags)		= 0;

	virtual void OnPaint(TEWnd* pwnd, CDC* pdc, CRect const& rcPaint)				= 0;
	virtual BOOL OnCommand(TEWnd* pwnd, WPARAM wParam, LPARAM lParam)				= 0;

	virtual void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis)						= 0;
	virtual void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis)				= 0;
	virtual void OnShowWindow(TEWnd* pwnd, BOOL bShow, UINT nStatus)				= 0;
	//@}
	/// ����� ������
	virtual bool OnStart()															= 0;
	/// ��������
	virtual bool OnEnd()															= 0;

	/// ����������� � ��������� ��������
	virtual BOOL OnDropData(TEWnd* pwnd, VOID* pData, const CPoint& point)			= 0;
	/// ����� �� ������� �������������� ������
	virtual bool CanDrop(TEWnd* pwnd, VOID* pData, const CPoint& point)				= 0;
	/// ���������� ��������� ������� ����, ������ �� ������� � �.�.
	virtual bool OnCommandUpdateUI(TEWnd *pwnd, CCmdUI* pCmdUI)						= 0;
	/// �� ��������� �������� ����������
	virtual void OnSettingsChange()													= 0;
	/// �������� undo-buffer ��
	virtual void clearUndobuffer()													= 0;
protected:
	//protected, ����� ������ ���� ������� �������� ��������� �� ���� ���������
	inline virtual ~ITEWorkMode() = 0;
};
ITEWorkMode::~ITEWorkMode(){}