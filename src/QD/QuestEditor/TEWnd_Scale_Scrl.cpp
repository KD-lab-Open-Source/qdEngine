#include "stdafx.h"
#include "questeditor.h"
#include "qd_trigger_chain.h"

#include "TEWnd.h"

#include <float.h>
#include <algorithm>
/************************************************************************/
/* Constants                                                            */
/************************************************************************/
//const float SCALE_STEP = .85f;
//////////////////////////////////////////////////////////////////////////

BOOL TEWnd::OnScroll(UINT nScrollCode)
{
	int dx = 0;
	int dy = 0;
	switch(HIBYTE(nScrollCode)) {
	case SB_LINEUP:
		dy = -1;
		break;
	case SB_LINEDOWN:
		dy = 1;
		break;
	}
	switch(LOBYTE(nScrollCode)) {
	case SB_LINELEFT:
		dx = -1;
		break;
	case SB_LINERIGHT:
		dx = 1;
		break;
	}
	int sbmin, sbmax, sbpos;
	if (dx) {
		GetScrollRange(SB_HORZ,&sbmin, &sbmax);
		sbpos = GetScrollPos(SB_HORZ);
		if (sbmin < sbpos + dx&&sbpos+dx<sbmax) {
			SetScrollPos(SB_HORZ, sbpos + dx);
			SendMessage(WM_HSCROLL, 
				MAKEWPARAM(LOBYTE(nScrollCode), 0), NULL);
		}
	}
	if (dy) {
		GetScrollRange(SB_VERT,&sbmin, &sbmax);
		sbpos = GetScrollPos(SB_VERT);
		if (sbmin < sbpos + dy&&sbpos+dy<sbmax) {
			SetScrollPos(SB_VERT, sbpos + dy);
			SendMessage(WM_VSCROLL, 
				MAKEWPARAM(HIBYTE(nScrollCode), 0), NULL);
		}
	}
	return TRUE;
}

VOID TEWnd::SetScale(float fOldScale, float fNewScale)
{
	if (fNewScale > m_fMaxScale)
		fNewScale = m_fMaxScale;
	else if (fNewScale < m_fMinScale)
		fNewScale = m_fMinScale;
//	if (fOldScale == fNewScale)
//		return;
	//левый верхний угол окна до изменения масштаба
	CPoint ptViewportOrg(m_ptViewportOrg);//в логических координатах
	descale_point(&ptViewportOrg);//перевели в логические без учета сдвига
	
	//получили реальную точку,
	//которая соответсвует углу
	ptViewportOrg = -ptViewportOrg;

	CRect rectClient;
	GetClientRect(rectClient);
	
	//не подвижная точка это центр окна
	CPoint ptConst(rectClient.CenterPoint());
	scr2log(&ptConst,1);//центр в лог. координатах

	float fScaledDim = rectClient.Width()/fNewScale;
	if(m_rcWorkArea.Width() <= fScaledDim)
	{
		//если изображение полностью влазит в окно, 
		//то просто ставим его в центер
		ptViewportOrg.x = (m_rcWorkArea.Width() - 
			static_cast<int>(fScaledDim))>>1;
	}
	else
	{
		//иначе стягиваем(расстягиваем) к(от) неподвижной точки
		ptViewportOrg.x = ptConst.x - 
			static_cast<int>((ptConst.x - ptViewportOrg.x)*fOldScale/fNewScale);
		
		if (ptViewportOrg.x > m_rcWorkArea.Width() - fScaledDim) 
			ptViewportOrg.x = static_cast<int>(m_rcWorkArea.Width() - fScaledDim);

		if(ptViewportOrg.x < 0)
			ptViewportOrg.x = 0;
	}
	fScaledDim = rectClient.Height()/fNewScale;
	if(m_rcWorkArea.Height() <= fScaledDim)
	{
		ptViewportOrg.y = (m_rcWorkArea.Height()
			- static_cast<int>(fScaledDim))>>1;
	}
	else
	{
		ptViewportOrg.y = ptConst.y - 
			static_cast<int>((ptConst.y - ptViewportOrg.y)*fOldScale/fNewScale);
		
		if (ptViewportOrg.y > m_rcWorkArea.Height() - fScaledDim) 
			ptViewportOrg.y = static_cast<int>(m_rcWorkArea.Height() - fScaledDim);

		if(ptViewportOrg.y < 0)
			ptViewportOrg.y = 0;
	}

	m_fScale = fNewScale;

	scale_point(&ptViewportOrg);
	m_ptViewportOrg = -ptViewportOrg;

	//пересозадем фонт с нужной нам высотой
	CrtFont();
	SetScrollRanges();
	UpdateScrollers();
	UpdateScaleInfo();
}

VOID TEWnd::SetScale(const SCALE_ACTION action)
{
	float const fOldScale = m_fScale;
	float fNewScale = fOldScale;
	switch(action) {
	case SA_PLUS:
		fNewScale = fNewScale/SCALE_STEP;
		break;
	case SA_MINUS:
		fNewScale = fNewScale*SCALE_STEP;
		break;
	case SA_RESET:
		break;
	}
	SetScale(fOldScale, fNewScale);
}

VOID TEWnd::SetScaleRanges()
{
	CRect rcClient;
	GetClientRect(rcClient);


	float fH = static_cast<float>(rcClient.Height())/
		static_cast<float>(m_rcWorkArea.Height());
	float fW = static_cast<float>(rcClient.Width())/
		static_cast<float>(m_rcWorkArea.Width());

//	m_fMinScale = (std::min)(fH, fW);
	m_fMinScale = 0.25f;
	m_fMaxScale = 5.f;

	if(!_finite(m_fMinScale))
		m_fMinScale = 0;
	
	if(m_fMaxScale < m_fMinScale)
		m_fMinScale = m_fMaxScale;
}

VOID TEWnd::SetScrollRanges()
{
	CRect rectClient;
	GetClientRect(rectClient);

	CRect scaledTR(m_rcWorkArea);
	log2scr(reinterpret_cast<POINT*>(&scaledTR), 2);
	
	if (rectClient.Width() < scaledTR.Width()) {
		m_iMaxScrollLimX = 0;
		m_iMinScrollLimX = rectClient.Width() - scaledTR.Width();
	}
	else
		m_iMinScrollLimX = 
			m_iMaxScrollLimX = (rectClient.Width() - scaledTR.Width())>>1;
	
	if (rectClient.Height() < scaledTR.Height()) {
		m_iMaxScrollLimY = 0;
		m_iMinScrollLimY = rectClient.Height() - scaledTR.Height();
	}
	else
		m_iMinScrollLimY = m_iMaxScrollLimY = 
						(rectClient.Height() - scaledTR.Height())>>1;

	if (m_ptViewportOrg.x > m_iMaxScrollLimX) {
		m_ptViewportOrg.x = m_iMaxScrollLimX;
	}
	if (m_ptViewportOrg.y > m_iMaxScrollLimY) {
		m_ptViewportOrg.y = m_iMaxScrollLimY;
	}
	if (m_ptViewportOrg.x < m_iMinScrollLimX) {
		m_ptViewportOrg.x = m_iMinScrollLimX;
	}
	if (m_ptViewportOrg.y < m_iMinScrollLimY) {
		m_ptViewportOrg.y = m_iMinScrollLimY;
	}
}

VOID TEWnd::UpdateScrollers()
{
	SetHorzScroller();
	SetVertScroller();
}

bool TEWnd::SetHorzScroller()
{
	CPoint p(m_ptViewportOrg);
	CRect rc;
	GetClientRect(rc);
	
	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nPage = 0;
	si.nMax = m_iMaxScrollLimX - m_iMinScrollLimX;
	si.nPos = m_iMaxScrollLimX - p.x;
	if(si.nMax)
	{
		si.nMin = 0;
		si.nPage = 800;//rc.Width() / scale_x(m_iNetCellWidth + m_iNetAmongCellW);
	}
	SetScrollInfo(SB_HORZ, &si);
	
	return true;
}

bool TEWnd::SetVertScroller()
{
	CPoint p(m_ptViewportOrg);
	CRect rc;
	GetClientRect(rc);
	
	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nPage = 0;
	si.nMax = m_iMaxScrollLimY - m_iMinScrollLimY;
	si.nPos = m_iMaxScrollLimY - p.y;
	if(si.nMax)
	{
		si.nMin = 0;
		si.nPage = 800;//rc.Height() / scale_x(m_iNetCellHeight + m_iNetAmongCellH);
	}
	SetScrollInfo(SB_VERT, &si);
	
	return true;
}

void TEWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO si;
	if (GetScrollInfo(SB_HORZ, &si, SIF_ALL)) {
		int dx = 0;
		
		switch(nSBCode) {
		case SB_LINELEFT:
			dx = 1;
			si.nPos -= 1;
			break;
		case SB_LINERIGHT:
			dx = -1;
			si.nPos += 1;
			break;
		case SB_PAGERIGHT:
			dx = si.nPage;
			dx = -dx;
			si.nPos += si.nPage;
			break;
		case SB_PAGELEFT:
			dx = si.nPage;
			si.nPos -= si.nPage;
			break;
		case SB_THUMBTRACK:
			dx = si.nPos - nPos;
			si.nPos = nPos;
			break;
		default:
			return;
		}
		SetScrollInfo(SB_HORZ, &si);
		ScrollView(dx, 0);
//		ExtendWorkArea();
	}
	
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void TEWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO si;
	if (GetScrollInfo(SB_VERT, &si, SIF_ALL)) {
		int dx = 0;
		
		switch(nSBCode) {
		case SB_LINEUP:
			dx = 1;
			si.nPos -= 1;
			break;
		case SB_LINEDOWN:
			dx = -1;
			si.nPos += 1;
			break;
		case SB_PAGEDOWN:
			dx = si.nPage;
			dx = -dx;
			si.nPos += si.nPage;
			break;
		case SB_PAGEUP:
			dx = si.nPage;
			si.nPos -= si.nPage;
			break;
		case SB_THUMBTRACK:
			dx = si.nPos - nPos;
			si.nPos = nPos;
			break;
		default:
			return;
		}
		SetScrollInfo(SB_VERT, &si);
		ScrollView(0, dx);
//		ExtendWorkArea();
	}
	
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

VOID TEWnd::ScrollView(int dx, int dy)
{
	CPoint oldBmpOrg(m_ptViewportOrg);
	
	OffsetViewportOrg(dx, dy);
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

VOID TEWnd::OffsetViewportOrg(int dx, int dy)
{
	CPoint &org = m_ptViewportOrg;
	
	org.Offset(dx, dy);
	
	if (org.x > m_iMaxScrollLimX) {
		org.x = m_iMaxScrollLimX;
	}
	if (org.y > m_iMaxScrollLimY) {
		org.y = m_iMaxScrollLimY;
	}
	if (org.x < m_iMinScrollLimX) {
		org.x = m_iMinScrollLimX;
	}
	if (org.y < m_iMinScrollLimY) {
		org.y = m_iMinScrollLimY;
	}
}

VOID TEWnd::EnsureVisibleRect(const CRect& r){
	CRect rcClient;
	GetClientRect(rcClient);

	float kW = static_cast<float>(rcClient.Width()) /r.Width();
	float kH = static_cast<float>(rcClient.Height())/r.Height();

	SetScale(m_fScale, (std::min)(kW, kH));
	CRect scaled_r(r);
	log2scr(scaled_r);
	m_ptViewportOrg.x = -(scaled_r.left - m_ptViewportOrg.x);
	m_ptViewportOrg.y = -(scaled_r.top - m_ptViewportOrg.y);
	while (ExtendWorkArea());
	
	//перевыставляем скроллеры, потому что изменили
	//m_ptViewportOrg
	UpdateScrollers();
}

void TEWnd::ExtendWAToLeft(int ex)
{
	const int offset = ex ? ex : get_wa_x_add();
	m_rcWorkArea.right += offset;
	m_ptViewportOrg.x -= scale_x(offset);

	CSize sz(offset, 0);
	if (m_bShowAll) 
		OffsetChainsGL(sz);
	else
	{
		OffsetChain(m_vecDrwList.front(),sz);
		UpdateChainWorkArea(m_vecDrwList.front());
		m_vecDrwList.front()->set_work_area(m_rcWorkArea);
	}

	SetScrollRanges();
	UpdateScrollers();
}
void TEWnd::ExtendWAToRight(int ex)
{
	m_rcWorkArea.right += ex ? ex : get_wa_x_add();
	if (!m_bShowAll) 
	{
		UpdateChainWorkArea(m_vecDrwList.front());
		m_vecDrwList.front()->set_work_area(m_rcWorkArea);
	}
	SetScrollRanges();
	UpdateScrollers();
}

void TEWnd::ExtendWAToTop(int ex)
{
	const int offset = ex ? ex : get_wa_y_add();
	m_rcWorkArea.bottom += offset;
	m_ptViewportOrg.y -= scale_y(offset);

	CSize sz(0, offset);
	if (m_bShowAll) 
		OffsetChainsGL(sz);
	else
	{
		OffsetChain(m_vecDrwList.front(),sz);
		UpdateChainWorkArea(m_vecDrwList.front());
		m_vecDrwList.front()->set_work_area(m_rcWorkArea);
	}
	SetScrollRanges();
	UpdateScrollers();
}

void TEWnd::ExtendWAToBottom(int ex)
{
	m_rcWorkArea.bottom += ex ? ex : get_wa_y_add();

	if (!m_bShowAll) 
	{
		UpdateChainWorkArea(m_vecDrwList.front());
		m_vecDrwList.front()->set_work_area(m_rcWorkArea);
	}

	SetScrollRanges();
	UpdateScrollers();
}

bool TEWnd::ExtendWorkArea()
{
	bool res = false;
	if (m_ptViewportOrg.x >= m_iMaxScrollLimX) {
		ExtendWAToLeft(descale_x(m_ptViewportOrg.x - m_iMaxScrollLimX));
		res = true;
	}
	if (m_ptViewportOrg.y >= m_iMaxScrollLimY) {
		ExtendWAToTop(descale_y(m_ptViewportOrg.y - m_iMaxScrollLimY));
		res = true;
	}
	if (m_ptViewportOrg.x <=  m_iMinScrollLimX) {
		ExtendWAToRight(descale_x(m_iMinScrollLimX - m_ptViewportOrg.x));
		res = true;
	}
	if (m_ptViewportOrg.y <=  m_iMinScrollLimY) {
		ExtendWAToBottom(descale_y(m_iMinScrollLimY - m_ptViewportOrg.y));
		res = true;
	}
	return res;
}