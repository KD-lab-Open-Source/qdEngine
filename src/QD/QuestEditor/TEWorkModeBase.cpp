#include "stdafx.h"
#include "resource.h"
#include "teworkmodebase.h"
#include "TEWnd.h"

namespace{
	inline bool IsKeyPressed(int keyID)
	{
		return static_cast<bool>(GetAsyncKeyState(keyID)>>15);
	}
}
TEWorkModeBase::TEWorkModeBase(void)
: m_bCanDragContent(false)
, m_base_point(0, 0)
, m_hCursor(NULL)
{
	m_hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}

TEWorkModeBase::~TEWorkModeBase(void)
{
}

void TEWorkModeBase::setBasePoint(CPoint const& point){
	m_base_point = point;
}

void TEWorkModeBase::setCanDragContent(bool bVal){
	m_bCanDragContent = bVal;
}

void TEWorkModeBase::setCursor(HCURSOR hCursor)
{
	m_hCursor = hCursor;
	SetCursor(m_hCursor);
}

void TEWorkModeBase::OnLButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	if (getCanDragContent()&&nFlags&MK_LBUTTON){
		setBasePoint(point);
		setCursor(AfxGetApp()->LoadCursor(IDC_CUR_DRAG_HAND));
		pwnd->SetCapture();
	}
}
void TEWorkModeBase::OnLButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	if(getCanDragContent())
	{
		ReleaseCapture();
		setCursor(AfxGetApp()->LoadCursor(IDC_CUR_HAND));
		pwnd->UpdateScrollers();
		pwnd->Invalidate();
		return;
	}
}

void TEWorkModeBase::OnLButtonDblClk(TEWnd* pwnd, UINT nFlags, CPoint point)
{

}

void TEWorkModeBase::OnRButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point)
{

}

void TEWorkModeBase::OnRButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point)
{

}

void TEWorkModeBase::OnMouseMove(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	if(getCanDragContent()&&(nFlags&MK_LBUTTON))
	{
		CPoint diff(point - getBasePoint());
		pwnd->ScrollView(diff.x, diff.y);
		pwnd->ExtendWorkArea();
		pwnd->UpdateScrollers();
		setBasePoint(point);
	}
	else
		trackToolTip(pwnd, point);
}

void TEWorkModeBase::OnKeyDown(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!getCanDragContent()&&nChar == VK_CONTROL)
	{
		if(!IsKeyPressed(VK_SHIFT)&&!IsKeyPressed(VK_LBUTTON)&&!IsKeyPressed(VK_RBUTTON))
		{
			setCanDragContent(true);
			setCursor(AfxGetApp()->LoadCursor(IDC_CUR_HAND));
		}
	}
	else if(m_bCanDragContent&&nChar == VK_SHIFT)
	{
		setCanDragContent(false);
		setCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		ReleaseCapture();
	}
}

void TEWorkModeBase::OnKeyUp(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(getCanDragContent()&&nChar == VK_CONTROL)
	{
		setCanDragContent(false);
		setCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		ReleaseCapture();
	}
	else if(!getCanDragContent()&&nChar == VK_SHIFT)
	{
		if(IsKeyPressed(VK_CONTROL)&&!IsKeyPressed(VK_LBUTTON)&&!IsKeyPressed(VK_RBUTTON))
		{
			setCanDragContent(true);
			setCursor(AfxGetApp()->LoadCursor(IDC_CUR_HAND));
		}
	}
}

void TEWorkModeBase::OnPaint(TEWnd* pwnd, CDC* pdc, CRect const& rcPaint)
{

}

BOOL TEWorkModeBase::OnSetCursor(TEWnd* pwnd, CWnd* pWnd, UINT nHitTest, UINT message)
{
	SetCursor(getCursor());
	return TRUE;
}

BOOL TEWorkModeBase::OnCommand(TEWnd* pwnd, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

void TEWorkModeBase::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis)
{

}

void TEWorkModeBase::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis)
{

}

void TEWorkModeBase::OnShowWindow(TEWnd* pwnd, BOOL bShow, UINT nStatus)
{
}

void TEWorkModeBase::ModifyMenu(CMenu* pmenu, UINT menuResId, TEWnd* pwnd)
{

}

BOOL TEWorkModeBase::trackPopupMenu(UINT id, TEWnd* pwnd)
{
	CMenu menu;
	menu.LoadMenu(id);
	CPoint point;
	GetCursorPos(&point);
	BOOL res = FALSE;
	ModifyMenu(menu.GetSubMenu(0), id, pwnd);

	if((res = menu.GetSubMenu(0)->TrackPopupMenu(
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,point.x, point.y, pwnd)))
	{
		//если указать TPM_RETURNCMD, то эта дура не шлет нотификацию
		//но че нам кабанам, сами пошлем:)
		pwnd->SendMessage(WM_COMMAND, (WPARAM)res, NULL);
	}
	return res;
}

void TEWorkModeBase::trackToolTip(TEWnd* pwnd, CPoint point)
{

}

bool TEWorkModeBase::OnStart()
{
	return true;
}
bool TEWorkModeBase::OnEnd()
{
	return true;
}

BOOL TEWorkModeBase::OnDropData(TEWnd* pwnd, VOID* pData, const CPoint& point)
{
	return FALSE;
}

bool TEWorkModeBase::CanDrop(TEWnd* pwnd, VOID* pData, const CPoint& point)
{
	return false;
}

bool TEWorkModeBase::OnCommandUpdateUI(TEWnd *pwnd, CCmdUI* pCmdUI)
{
	return false;
}

void TEWorkModeBase::OnSettingsChange()
{

}

void TEWorkModeBase::clearUndobuffer()
{

}
