#include "stdafx.h"
#include "TEWnd.h"
#include "qd_trigger_chain.h"
#include "qd_trigger_element.h"
#include "BaseTEDrawer.h"
#include "te_consts.h"
#include "te_helpers.h"

BaseTEDrawer::BaseTEDrawer(void):
  m_szTriggerOffset(0, 0)
, m_NetPen(PS_DOT, 1, NET_COLOR)
, m_penAutoRestart(PS_SOLID, 5, AUTO_RESTART_PEN_COLOR)
, m_brNormalEleBrush(TE_NORMAL_ELE_BACK)
, m_brSelEleBrush(TE_SELECTED_ELE_BACK)
, m_ptrCurrentDrawedChain(NULL)
, m_ptrDrawedLinkOwner(NULL)
, paintRect_(0, 0, 0, 0)
{
}

BaseTEDrawer::~BaseTEDrawer(void)
{
}

void BaseTEDrawer::setPaintRect(CRect const& rc) const{
	paintRect_ = rc;
}

CRect const& BaseTEDrawer::getPaintRect() const{
	return paintRect_;
}
void BaseTEDrawer::setTriggerOffset(CSize const& sz) const{
	m_szTriggerOffset = sz;
}

BOOL BaseTEDrawer::setNetPen(int nPenStyle, int nWidth, COLORREF crColor){
	return m_NetPen.CreatePen(nPenStyle, nWidth, crColor);
}
BOOL BaseTEDrawer::setNetPen(HPEN hPen){
	return m_NetPen.Attach(hPen);
}
BOOL BaseTEDrawer::setAutoRestartPen(HPEN hPen){
	return m_penAutoRestart.Attach(hPen);
}
BOOL BaseTEDrawer::setAutoRestartPen(int nPenStyle, int nWidth, COLORREF crColor){
	return m_penAutoRestart.CreatePen(nPenStyle, nWidth, crColor);
}
void BaseTEDrawer::setCurrentDrawedChain(qdTriggerChain const* pchain) const{
	m_ptrCurrentDrawedChain = pchain;
}

BOOL BaseTEDrawer::setNormalEleBrush(HBRUSH hBrush){
	return m_brNormalEleBrush.Attach(hBrush);
}
BOOL BaseTEDrawer::setNormalEleBrush(COLORREF crColor){
	return m_brNormalEleBrush.CreateSolidBrush(crColor);
}

BOOL BaseTEDrawer::setSelEleBrush(HBRUSH hBrush){
	return m_brSelEleBrush.Attach(hBrush);
}
BOOL BaseTEDrawer::setSelEleBrush(COLORREF crColor){
	return m_brSelEleBrush.CreateSolidBrush(crColor);
}
void BaseTEDrawer::setDrawedLinkOwner(qdTriggerElement const* pele) const{
	m_ptrDrawedLinkOwner = pele;
}

void BaseTEDrawer::draw(CDC* pdc, TEWnd* pwnd,
						IDrawData* pdd, CRect const& rcPaint) const
{
	setPaintRect(rcPaint);
	drawNet(pdc, pwnd, rcPaint);
//	setPaintRect(CRect(0, 0, 0, 0));
}

void BaseTEDrawer::drawDraggedRect(HDC dc, CRect const&r, HPEN hpen) const
{
	int iMode = SetROP2(dc, R2_NOTXORPEN);
	HGDIOBJ obr = SelectObject(dc, GetStockObject(NULL_BRUSH));
	HGDIOBJ oldp = SelectObject(dc, hpen);
	Rectangle(dc, r.left, r.top, r.right, r.bottom);

	SelectObject(dc, obr);
	SelectObject(dc, oldp);
	SetROP2(dc, iMode);
}

void BaseTEDrawer::drawNet(CDC* pdc, TEWnd* pwnd, CRect const& rcPaint) const
{
	CRect r(rcPaint);

	CSize const cell_sizes(pwnd->GetNetCellDims());
	CSize const among_cell_szs(pwnd->GetNetAmongCellDims());

	int const union_cell_sx = cell_sizes.cx + among_cell_szs.cx;
	int const union_cell_sy = cell_sizes.cy + among_cell_szs.cy;

	//для случая, когда LEFT становиться отрицательным
	//страхуемся и вычитаем -1, чтобы сетка рисовалась 
	//из-за левого края окна. То же для TOP
	r.left = (r.left/union_cell_sx - 1)* union_cell_sx;
	r.right = r.right/union_cell_sx * union_cell_sx + union_cell_sx;

	r.top = (r.top/union_cell_sy - 1)* union_cell_sy;
	r.bottom = r.bottom/union_cell_sy * union_cell_sy + union_cell_sy;

	HGDIOBJ hOldPen = pdc->SelectObject(m_NetPen);

	int i = r.left;
	CPoint p;
	while (i < r.right) {
		p.x = i;
		p.y = r.top;
		pwnd->log2scr(&p);

		pdc->MoveTo(p);

		p.x = i;
		p.y = r.bottom;
		pwnd->log2scr(&p);

		pdc->LineTo(p);
		i += cell_sizes.cx;

		p.x = i;
		p.y = r.top;
		pwnd->log2scr(&p);

		pdc->MoveTo(p);

		p.x = i;
		p.y = r.bottom;
		pwnd->log2scr(&p);

		pdc->LineTo(p);

		i += among_cell_szs.cx;
	}

	i = r.top;
	while (i < r.bottom) {
		p.x = r.left;
		p.y = i;
		pwnd->log2scr(&p);
		pdc->MoveTo(p);

		p.x = r.right;
		p.y = i;
		pwnd->log2scr(&p);
		pdc->LineTo(p);

		i += cell_sizes.cy;

		p.x = r.left;
		p.y = i;
		pwnd->log2scr(&p);
		pdc->MoveTo(p);

		p.x = r.right;
		p.y = i;
		pwnd->log2scr(&p);
		pdc->LineTo(p);

		i += among_cell_szs.cy;
	}

	pdc->SelectObject(hOldPen);
}

void BaseTEDrawer::DrawElement(TEWnd* pwnd, CDC* pdc, 
							   qdTriggerElementConstPtr const& ptrEle) const
{
	CRect r = ptrEle->bounding_rect();
	r.OffsetRect(m_szTriggerOffset);
	pwnd->log2scr(r);

	HGDIOBJ oldbr = NULL;
	if (ptrEle->selected()) 
		oldbr = pdc->SelectObject(m_brSelEleBrush);
	else
		oldbr = pdc->SelectObject(m_brNormalEleBrush);

	POINT ELLIPSE = {6, 6};
//	pdc->Rectangle(&r);
	pdc->RoundRect(&r, ELLIPSE);

	/// Делаем отступ, чтобы получилась рамка, которая закрашивается в 
	/// зависимости от выбранности элемента
	int idef = pwnd->scale_x(RECT_INCREASE);
	r.DeflateRect(idef, idef);

	/// Рисуем внутренность элемента
	pdc->SelectObject(SelectEleBackBrush(ptrEle));
//	pdc->Rectangle(&r);
	pdc->RoundRect(&r, ELLIPSE);

	pdc->SetBkMode(TRANSPARENT);

	idef = pwnd->scale_x(2);
	r.left += idef? idef : 1;

	if (ptrEle->ID() == qdTriggerElement::ROOT_ID) 
	{	/// отрисовываем корневой элемент.
		/// у него спец заголовок
		m_strDrawedEleTitle = ptrEle->title().c_str();
		m_strDrawedEleTitle += _T(" - ");
		m_strDrawedEleTitle += m_ptrCurrentDrawedChain->name();

		int const old = pdc->SetTextColor(START_ELE_TITLE_COLOR);
		pdc->DrawText(m_strDrawedEleTitle, m_strDrawedEleTitle.GetLength(),r,
			DT_WORD_ELLIPSIS|DT_VCENTER|DT_CENTER);
		pdc->SetTextColor(old);
	}
	else
	{
		if (ptrEle->object()&&
			ptrEle->object()->named_object_type() == QD_NAMED_OBJECT_SCENE)
		{
			/// Отрисовываем сцену. У ней цвет текста другой (крансный)
			COLORREF const old = pdc->SetTextColor(SCENE_ELE_TITLE_COLOR);
			pdc->DrawText(ptrEle->title().c_str(), -1,
				r, DT_WORD_ELLIPSIS|DT_VCENTER|DT_CENTER);
			pdc->SetTextColor(old);
		}
		else
			pdc->DrawText(ptrEle->title().c_str(), -1,
			r, DT_WORD_ELLIPSIS|DT_VCENTER|DT_CENTER);
	}

	pdc->SelectObject(oldbr);
}

HBRUSH BaseTEDrawer::SelectEleBackBrush(qdTriggerElementConstPtr const& pele) const
{
	return reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
}
HBRUSH	BaseTEDrawer::SelectLinkBrColor(qdTriggerLink const* plink) const
{
	return reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
}
HPEN	BaseTEDrawer::SelectLinkPenColor(qdTriggerLink const* plink) const
{
	return reinterpret_cast<HPEN>(GetStockObject(BLACK_PEN));
}

void BaseTEDrawer::DrawLink(HDC dc, const CPoint& pt_from, const CPoint& pt_to, 
			  CPoint* ptrg, HPEN pen, HGDIOBJ brush) 
{
	/// рисуем линию
	HGDIOBJ old = SelectObject(dc, pen);
	MoveToEx(dc, pt_from.x, pt_from.y, NULL);
	LineTo(dc, pt_to.x, pt_to.y);
	SelectObject(dc, old);

	if (ptrg)
	{
		/// Рисуем треугольник
		old = SelectObject(dc, brush);
		Polygon(dc, ptrg, 3);
		SelectObject(dc, old);
	}
}

/**
	Для начала проверяет попадает ли треугольник линка в видимую область.
	Если да, то рисуем с треугльником, иначе без
 */
void BaseTEDrawer::DrawLink(TEWnd* pwnd, HDC dc, CPoint pf, 
					 CPoint pt,qdTriggerLink const* pl) const
{
	CRect rc(pt, pt);
	rc.InflateRect(15, 15);
	if (rc.IntersectRect(rc, getPaintRect()))
	{
		te_h::TRIANGLE linkTrg;
		te_h::get_link_triangle(pf, pt, linkTrg);

		pwnd->log2scr(&pf);
		pwnd->log2scr(&pt);
		linkTrg[0] = pt;//присваиваем здесь, чтобы избежать повторного пересчета 
		pwnd->log2scr(linkTrg + 1, 2);
		if (pl->auto_restart())
			DrawLink(dc, pf, pt, linkTrg, m_penAutoRestart, GetStockObject(NULL_BRUSH));

		DrawLink(dc, pf, pt, linkTrg, SelectLinkPenColor(pl), SelectLinkBrColor(pl));
	}
	else
	{
		pwnd->log2scr(&pf);
		pwnd->log2scr(&pt);
		if (pl->auto_restart())
			DrawLink(dc, pf, pt, NULL, m_penAutoRestart, GetStockObject(NULL_BRUSH));

		DrawLink(dc, pf, pt, NULL, SelectLinkPenColor(pl), SelectLinkBrColor(pl));
	}
}

/**
	Пробегает во всем линкам. Вычисляет начало и конец и отрисовывает
 */
void BaseTEDrawer::DrawElementLinks(TEWnd* pwnd, CDC* pdc, const RECT& rcEle, 
							 qdTriggerElementConstPtr const& ptrEle) const
{
	CPoint center = te_h::get_center(rcEle);
	qdTriggerLinkList const& children = ptrEle->children();
	qdTriggerLinkList::const_iterator	_itr = children.begin(),
		_end = children.end();
	for(; _itr != _end; ++_itr)
	{
		qdTriggerLink const* ptrTL = &*_itr;

		CPoint beg(center);
		beg.Offset(ptrTL->get_owner_offset());

		CPoint end(te_h::get_center(ptrTL->element()->bounding_rect()));
		end.Offset(m_szTriggerOffset);
		end.Offset(ptrTL->get_child_offset());

		DrawLink(pwnd, *pdc, beg, end, ptrTL);
	}
}

/**
	Пробегает по всем элементам. Если элемент перескается с областью прорисовки,
	то рисуем его
 */
void BaseTEDrawer::DrawElements(
							TEWnd* pwnd, 
							CDC* pdc, 
							const CRect& rcPaint, 
							qdTriggerChain const* ptrChain) const
{
	CRect itrs(ptrChain->root_element()->bounding_rect());
	itrs.OffsetRect(m_szTriggerOffset);

	if (itrs.IntersectRect(&rcPaint,itrs))
		DrawElement(pwnd, pdc, ptrChain->root_element());

	const qdTriggerElementList& tel = ptrChain->elements_list();
	qdTriggerElementList::const_iterator _itr = tel.begin(), _end = tel.end();
	for(;_itr!=_end; ++_itr){
		qdTriggerElementPtr const& ptrElement = *_itr;
		itrs = ptrElement->bounding_rect();

		itrs.OffsetRect(m_szTriggerOffset);

		if (itrs.IntersectRect(&rcPaint,itrs)) 
			DrawElement(pwnd, pdc, ptrElement);
	}
}

/**
	Если цепочка пересекается с областью прорисовки, то рисуем ее элементы и линки
 */
void BaseTEDrawer::DrawChain(TEWnd* pwnd, CDC* pdc, const CRect& rcPaint, 
					  qdTriggerChain const* ptrChain) const
{
	CRect intersect;
	if (!intersect.IntersectRect(&rcPaint, &ptrChain->bounding_rect())) {
		return;
	}
	setCurrentDrawedChain(ptrChain);

	DrawElements(pwnd, pdc, intersect, ptrChain);
	DrawLinks(pwnd, pdc, ptrChain);

	setCurrentDrawedChain(NULL);
}

/**
	пробегается по всем элементам и отрисовывает их линки
 */
void BaseTEDrawer::DrawLinks(TEWnd* pwnd, CDC* pdc, qdTriggerChain const* ptrChain) const
{
	CRect rc(ptrChain->root_element()->bounding_rect());
	rc.OffsetRect(m_szTriggerOffset);
	DrawElementLinks(pwnd, pdc, rc, ptrChain->root_element());
	qdTriggerElementList const& tel = ptrChain->elements_list();
	qdTriggerElementList::const_iterator _itr = tel.begin(), _end = tel.end();
	for(;_itr!=_end; ++_itr)
	{
		qdTriggerElementPtr const& ptrElement = *_itr;
		rc = ptrElement->bounding_rect();
		rc.OffsetRect(m_szTriggerOffset);
		setDrawedLinkOwner(ptrElement.get());
		DrawElementLinks(pwnd, pdc, rc, ptrElement);
	}
	setDrawedLinkOwner(NULL);
}