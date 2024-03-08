#include "stdafx.h"
#include ".\customrecttracker.h"

CustomRectTracker::CustomRectTracker(IRectChangeListener* rectChangeListener):
rectChangeListener_(rectChangeListener)
{
	m_nStyle = dottedLine;
}

CustomRectTracker::~CustomRectTracker(void)
{
}

void CustomRectTracker::OnChangedRect(const CRect& rectOld)
{
	if (rectChangeListener_)
		rectChangeListener_->onRectChange(*this, rectOld);
}

void CustomRectTracker::DrawTrackerRect(LPCRECT lpRect, 
										CWnd* pWndClipTo, 
										CDC* pDC, 
										CWnd* pWnd)
{
//	CRectTracker::DrawTrackerRect(lpRect, pWndClipTo, pDC, pWnd);
}
