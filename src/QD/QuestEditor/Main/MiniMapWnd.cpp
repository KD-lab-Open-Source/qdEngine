// Main\MiniMapWnd.cpp : implementation file
//

#include "stdafx.h"
#include "MiniMapWnd.h"
#include "..\Core\qdCore\qd_trigger_chain.h"
#include "TEWnd.h"
#include ".\minimapwnd.h"

const int MINI_MAP_BORDER_X = 10; // Отступ слева и справа при выводе миникарты
const int MINI_MAP_BORDER_Y = 10; // Отступ сверху и снизу

// MiniMapWnd

IMPLEMENT_DYNCREATE(MiniMapWnd, CWnd)
MiniMapWnd::MiniMapWnd() :
	teWnd_(NULL),
	scaleCoef_(1.0f),
	needMoveViewport_(false)
{
}

MiniMapWnd::~MiniMapWnd()
{
}


BEGIN_MESSAGE_MAP(MiniMapWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void MiniMapWnd::formatToMinimap(RECT& rect)
{
	rect.left -= xShift_, rect.top -= yShift_;
	rect.right -= xShift_, rect.bottom -= yShift_;
	rect.left *= scaleCoef_, rect.top *= scaleCoef_;
	rect.right *= scaleCoef_, rect.bottom *= scaleCoef_;
	// adjust
	if (rect.right <= rect.left) rect.right = rect.left+1;
	if (rect.bottom <= rect.top) rect.bottom = rect.top+1;
	rect.left += MINI_MAP_BORDER_X, rect.top += MINI_MAP_BORDER_Y;
	rect.right += MINI_MAP_BORDER_X, rect.bottom += MINI_MAP_BORDER_Y;
}

void MiniMapWnd::moveViewport(CPoint mouse_click)
{
	if (teWnd_ && scaleCoef_ > 1e-5)
	{
		POINT pnt = mouse_click;
		pnt.x = (pnt.x - MINI_MAP_BORDER_X)/scaleCoef_ + xShift_;
		pnt.y = (pnt.y - MINI_MAP_BORDER_Y)/scaleCoef_ + yShift_;
		RECT client_rect;
		teWnd_->GetClientRect(&client_rect);
		pnt.x = -(teWnd_->scale_x(pnt.x) - client_rect.right/2);
		pnt.y = -(teWnd_->scale_y(pnt.y) - client_rect.bottom/2);
		teWnd_->setViewportOrg(pnt);
		teWnd_->UpdateScrollers();
		teWnd_->Redraw();
	}
}

// MiniMapWnd message handlers


void MiniMapWnd::OnPaint()
{// Вывод миникарты
	CPaintDC dc(this); // device context for painting
	
	RECT r;
	GetClientRect(&r);
	dc.FillSolidRect(&r, 0x00FFFFFF); // Заполняем белым карту
	
	const qdTriggerChain* chain = teWnd_->ConstShowedTrigger();
	if (!chain) return;
	if (!teWnd_) return;
	float coef;
	if (teWnd_->GetNetCellWidth() > teWnd_->GetNetCellHeight())
		coef = teWnd_->GetNetCellHeight();
	else
		coef = teWnd_->GetNetCellWidth();

	if (r.bottom <= 2*MINI_MAP_BORDER_X || r.right <= 2*MINI_MAP_BORDER_Y) return;
	RECT el_rect = chain->bounding_rect();
	float coefX = (el_rect.right-el_rect.left)/(r.right-2*MINI_MAP_BORDER_X);
	float coefY = (el_rect.bottom-el_rect.top)/(r.bottom-2*MINI_MAP_BORDER_Y);
	if (coef < coefX) coef = coefX;
	if (coef < coefY) coef = coefY;
	if (coef < 1.0f) return;
	scaleCoef_ = 1/coef;
	xShift_ = el_rect.left;
	yShift_ = el_rect.top;

	if (!chain) return;
	const qdTriggerElementList& triggers = chain->elements_list();
	for (qdTriggerElementList::const_iterator it = triggers.begin(); it != triggers.end(); ++it)
	{
		el_rect = (*it)->bounding_rect();
		formatToMinimap(el_rect);
		dc.FillSolidRect(&el_rect, 0x0000C000);
	}
	// Выводим корень цепочки
	el_rect = chain->root_element().get()->bounding_rect();;
	formatToMinimap(el_rect);
	dc.FillSolidRect(&el_rect, 0x000000FF);

	// Определеяем логические координаты рамки
	::GetClientRect(teWnd_->m_hWnd, &el_rect);
	teWnd_->scr2log(&el_rect);	
	formatToMinimap(el_rect);
	// Выводим прямоугольник
	COLORREF brd_clr = 0x00000000;
	dc.FillSolidRect(el_rect.left, el_rect.top, el_rect.right-el_rect.left, 1, brd_clr);
	dc.FillSolidRect(el_rect.left, el_rect.bottom, el_rect.right-el_rect.left, 1, brd_clr);
	dc.FillSolidRect(el_rect.left, el_rect.top, 1, el_rect.bottom-el_rect.top, brd_clr);
	dc.FillSolidRect(el_rect.right, el_rect.top, 1, el_rect.bottom-el_rect.top, brd_clr);
}

int MiniMapWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void MiniMapWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}

void MiniMapWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	moveViewport(point);
	needMoveViewport_ = true;

	CWnd::OnLButtonDown(nFlags, point);
}

void MiniMapWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	SHORT state;
	// Если в системе левую и правую кнопку мыши поменяли местами, то смотрим состояние правой
	if (!::GetSystemMetrics(SM_SWAPBUTTON)) state = ::GetAsyncKeyState(VK_LBUTTON);
	else state = ::GetAsyncKeyState(VK_RBUTTON);
	// Если левая кнопка не зажата, то двигать область видимости не нужно
	if (!(state && 0x8000)) needMoveViewport_ = false;
	if (needMoveViewport_)
		moveViewport(point);

	CWnd::OnMouseMove(nFlags, point);
}

void MiniMapWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	needMoveViewport_ = false;	

	CWnd::OnLButtonUp(nFlags, point);
}
