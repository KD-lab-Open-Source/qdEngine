#include "stdafx.h"
#include "resource.h"
#include "editinterfmode.h"

#include "childview.h"

#include "qd_interface_dispatcher.h"
#include "qd_interface_button.h"
#include "qd_interface_screen.h"
#include "qd_interface_slider.h"
#include "qd_interface_save.h"
#include "qd_interface_text_window.h"

#include <climits>

#include "GR_dispatcher.h"

#include <boost/mem_fn.hpp>

#include "sv_helpers.h"
#include "ptree_api.h"

#include "MainFrm.h"
using namespace sv_helpers;

void SetProjChange();

const int INTERF_ELE_ANCHOR_SIZE = 10;

void clear_selection(qdInterfaceScreen* pscr){
	pscr->clear_selection();
}

namespace{
class hit_tester
{
public:
	hit_tester(int x, int y): x_(x), y_(y), res_(NULL), min_d_(INT_MAX){}
	void operator()(qdInterfaceElement* p) 
	{
		if (p->hit_test(x_, y_))
		{
			if (p->screen_depth()<min_d_)
			{
				res_ = p;
				min_d_ = p->screen_depth();
			}
		}
	}
	qdInterfaceElement* result() const{
		return res_;
	}
private:
	int x_;
	int y_;
	qdInterfaceElement * res_;
	int min_d_;
};

class change_iele_scr_p
{
public:
	change_iele_scr_p(int dx, int dy):dl_(dx,dy){
	}
	void operator()(qdInterfaceElement* const p){
		p->set_r(p->r()+dl_);
	}
private:
	Vect2i dl_;
};

class change_iele_d{
public:
	change_iele_d(int d):d_(d){}
	void operator()(qdInterfaceElement* const p){
		p->set_screen_depth(p->screen_depth() + d_);
	}
private:
	int d_;
};


void GetInteractRect(qdInterfaceSlider const* psldr, std::pair<Vect2i, Vect2i>& rect)
{
	rect.first = psldr->r();
	rect.second = psldr->r();
	Vect2i const half(psldr->active_rectangle().x>>1, psldr->active_rectangle().y>>1);

	rect.first -= half;
	rect.second += half;
}

void GetInteractRect(qdInterfaceSave const* psave, std::pair<Vect2i, Vect2i>& rect)
{
	rect.first = psave->r();
	rect.second = psave->r();
	Vect2i const half(psave->thumbnail_size_x()>>1, 
					  psave->thumbnail_size_y()>>1);

	rect.first -= half;
	rect.second += half;
}

void GetInteractRect(qdInterfaceTextWindow const* textWindow, 
					 std::pair<Vect2i, Vect2i>& rect)
{
	rect.first = textWindow->r();
	rect.second = textWindow->r();
	Vect2i const half(textWindow->text_size().x>>1, textWindow->text_size().y>>1);

	rect.first -= half;
	rect.second += half;
}

bool GetInteractRect(qdInterfaceElement* const pele, std::pair<Vect2i, Vect2i> &rect)
{
	switch(pele->get_element_type()) {
	case qdInterfaceElement::EL_SLIDER:
		GetInteractRect(static_cast<qdInterfaceSlider*>(pele), rect);
		return true;
	case qdInterfaceElement::EL_SAVE:
		GetInteractRect(static_cast<qdInterfaceSave*>(pele), rect);
		return true;
	case qdInterfaceElement::EL_TEXT_WINDOW:
		GetInteractRect(static_cast<qdInterfaceTextWindow*>(pele), rect);
		return true;
	default:
		return false;
	}
}


}

EditInterfMode::EditInterfMode(void):
 m_ptrView(NULL)
 , m_bCanDrag(false)
 , m_iActiveAnchor(-1)
{

}

EditInterfMode::~EditInterfMode(void)
{
}

qdInterfaceScreen* EditInterfMode::selected_screen() const
{
	qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
	return pd->selected_screen();
}

qdInterfaceElement* EditInterfMode::get_leading_selected() const
{
	qdInterfaceScreen* const pscr = selected_screen();
	if (pscr->selected_element_list().empty())
		return NULL;
	else
		return pscr->selected_element_list().front();
}

bool EditInterfMode::Init(CChildView* pview, void *data)
{
	m_ptrView = pview;
	return true;
}

bool EditInterfMode::Deinit()
{
	return true;
}

void EditInterfMode::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	qdInterfaceScreen* const pscr = selected_screen();
	if (!pscr)
		return;
	const qdInterfaceScreen::element_list_t &lst = 
		pscr->selected_element_list();
	if (lst.empty())
		return;
	if (IsKeyPressed(VK_SHIFT))
	{
		int dx = 0;
		int dy = 0;
		switch(nChar) {
		case VK_UP:
			dy = -1;
			break;
		case VK_LEFT:
			dx = -1;
			break;
		case VK_DOWN:
			dy = 1;
			break;
		case VK_RIGHT:
			dx = 1;
			break;
		default:
			return;
		}
		if (IsKeyPressed(VK_CONTROL))
		{
			std::for_each(lst.begin(), lst.end(), change_iele_d(-dy));
			pscr->build_visible_elements_list();
		}
		else
			std::for_each(lst.begin(), lst.end(), change_iele_scr_p(dx, dy));
		m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
	}
	else
	{
		switch(nChar) {
		case VK_DELETE:
			ptree_api::DeleteObject(get_leading_selected(), true);
			break;
		}
	}
}

void EditInterfMode::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

void EditInterfMode::OnLButtonDown(UINT nFlags, const CPoint& point)
{
	if (nFlags&MK_RBUTTON)
		return;
	qdInterfaceScreen* const ps = selected_screen();
	if (!ps)
		return;
	if (ps->selected_element_list().size()==1)
	{
		DispatchOnLBtnDown(nFlags, point);
	}
	if (nFlags&MK_SHIFT&&m_iActiveAnchor == -1)
		m_bCanDrag = !ps->selected_element_list().empty();

	m_bWasMouseMove = false;
	m_base_point = point;
}

void EditInterfMode::OnLButtonUp(UINT nFlags, const CPoint& point)
{
	if (nFlags&MK_RBUTTON)
		return;

	m_bCanDrag = false;
	m_iActiveAnchor = -1;

	if (m_bWasMouseMove)
		return;
	
	if (qdInterfaceScreen* const pscr = selected_screen())
	{
		const qdInterfaceScreen::element_list_t& ele_lst = pscr->element_list();
		hit_tester ht = std::for_each(ele_lst.begin(), ele_lst.end(), 
			hit_tester(point.x, point.y));

		clear_selection(pscr);
		if (ht.result())
			pscr->select_element(ht.result(), true);

		if (pscr->selected_element_list().empty())
			ptree_api::ShowObjectInTree(pscr);
		else
			ptree_api::ShowObjectInTree(pscr->selected_element_list().front());
	}
	m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
}

void EditInterfMode::OnLButtonDblClk(UINT nFlags, const CPoint& point)
{
	qdInterfaceScreen* const pscr = selected_screen();
	if (!pscr)
		return ;
	qdInterfaceElement* const pele = get_leading_selected();
	if (pele)
		ptree_api::ShowProperties(pele);
	m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
}

void EditInterfMode::OnRButtonDown(UINT nFlags, const CPoint& point)
{
	if (nFlags&MK_LBUTTON)
		return;
}

void EditInterfMode::OnRButtonUp(UINT nFlags, const CPoint& point)
{
	if (nFlags&MK_LBUTTON)
		return;
	qdInterfaceScreen* const pscr = selected_screen();
	if (!pscr)
		return;
	const qdInterfaceScreen::element_list_t& lst = pscr->element_list();
	hit_tester ht = std::for_each(lst.begin(), lst.end(), hit_tester(point.x, point.y));
	if (ht.result())
	{
		ptree_api::ShowObjectInTree(ht.result());
		m_ptrView->UpdateWindow();
		ptree_api::ShowCorrespondingMenu(ht.result());
	}
}

void EditInterfMode::OnMouseMove(UINT nFlags, const CPoint &point)
{
	if (!(nFlags&MK_SHIFT))
		return;
	if (m_bCanDrag)
	{
		if (!(nFlags&MK_LBUTTON))
			return;
		m_bWasMouseMove = true;
		qdInterfaceScreen* const pscr = selected_screen();
		if (!pscr)
			return;

		int x = point.x - m_base_point.x;
		int y = point.y - m_base_point.y;

		typedef qdInterfaceScreen::element_list_t ele_lst_t;
		const ele_lst_t& sel_ele = pscr->selected_element_list();
		ele_lst_t::const_iterator i = sel_ele.begin(), e = sel_ele.end();

		if (nFlags&MK_CONTROL)
		{
			std::for_each(i, e, change_iele_d(-y));
			pscr->build_visible_elements_list();
		}
		else
			std::for_each(i, e, change_iele_scr_p(x, y));
		SetProjChange();
		m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
	}
	else if (nFlags&MK_LBUTTON&&m_iActiveAnchor != -1)
		DispatchOnMM(nFlags, point);
	m_base_point = point;
}

bool EditInterfMode::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (IsKeyPressed(VK_SHIFT))
	{
		CPoint p;
		m_ptrView->GetLogCurPos(p);
		qdInterfaceElement* const pele = get_leading_selected();
		int anchor = m_iActiveAnchor;
		if (anchor == -1)
		{
			if (pele)
			{
				std::pair<Vect2i, Vect2i> rect;
				if (GetInteractRect(pele, rect))
					anchor = FindAnchorFromRect(p, rect.first, 
							rect.second, INTERF_ELE_ANCHOR_SIZE);
			}
		}
		switch(anchor)
		{
		case -1:
			if (pele&&pele->hit_test(p.x, p.y))
			{
				if (IsKeyPressed(VK_LBUTTON))
					m_ptrView->SetCursor(AfxGetApp()->LoadCursor(IDC_CUR_DRAG_HAND));
				else
					m_ptrView->SetCursor(AfxGetApp()->LoadCursor(IDC_CUR_HAND));
			}
			else
				m_ptrView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			break;
		case 0:
		case 4:
			m_ptrView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE));
			break;
		case 1:
		case 5:
			m_ptrView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
			break;
		case 2:
		case 6:
			m_ptrView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENESW));
			break;
		case 3:
		case 7:
			m_ptrView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
			break;
		}
	}
	else
		m_ptrView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	return true;
}

void EditInterfMode::Draw(HDC dc)
{
	if (qdInterfaceScreen* ps = selected_screen())
	{
		typedef qdInterfaceScreen::element_list_t ele_list_t;
		const ele_list_t& lst = ps->selected_element_list();
		std::for_each(lst.begin(), lst.end(),
			std::bind1st(boost::mem_fn(EditInterfMode::DrawSelectedEle), this));
	}
}

void EditInterfMode::DrawSelectedEle(const qdInterfaceElement* pele) const
{
	Vect2i top_left(pele->r());
	Vect2i bottom_right(pele->r());
	int const sx = (pele->size_x()>>1);
	int const sy = (pele->size_y()>>1);
	top_left.x -= sx;
	top_left.y -= sy;
	bottom_right.x += sx;
	bottom_right.y += sy;
	DrawRectangle(top_left, bottom_right, 0, 4,IM_INVERSE_ON);
	CustomDraw(pele);
}

void EditInterfMode::CustomDraw(qdInterfaceElement const* pele) const
{
	switch(pele->get_element_type()) {
	case qdInterfaceElement::EL_SLIDER:
		CustomDrawSlider(static_cast<qdInterfaceSlider const*>(pele));
		break;
	case qdInterfaceElement::EL_SAVE:
		CustomDrawSave(static_cast<qdInterfaceSave const*>(pele));
		break;
	case qdInterfaceElement::EL_TEXT_WINDOW:
		CustomDrawTextWindow(static_cast<qdInterfaceTextWindow const*>(pele));
		break;
	}
}

void EditInterfMode::CustomDrawTextWindow(qdInterfaceTextWindow const* textWindow) const
{
	std::pair<Vect2i, Vect2i> rect;
	GetInteractRect(textWindow, rect);
	DrawRectangle(rect.first, rect.second, RGB(200, 200, 0), 4,IM_INVERSE_ON);
	DrawAnchors(rect.first, rect.second, INTERF_ELE_ANCHOR_SIZE, 0x00ffffff);
}

void EditInterfMode::CustomDrawSlider(qdInterfaceSlider const* psldr) const
{
	std::pair<Vect2i, Vect2i> rect;
	GetInteractRect(psldr, rect);
	DrawRectangle(rect.first, rect.second, RGB(200, 200, 0), 4,IM_INVERSE_ON);
	DrawAnchors(rect.first, rect.second, INTERF_ELE_ANCHOR_SIZE, 0x00ffffff);
}

void EditInterfMode::CustomDrawSave(qdInterfaceSave const* psave) const
{
	std::pair<Vect2i, Vect2i> rect;
	GetInteractRect(psave, rect);
	DrawRectangle(rect.first, rect.second, RGB(200, 200, 0), 4,IM_INVERSE_ON);
	DrawAnchors(rect.first, rect.second, INTERF_ELE_ANCHOR_SIZE, 0x00ffffff);
}

void EditInterfMode::OnLBtnDownBckgrnd(UINT nFlags, const CPoint& point)
{

}

void EditInterfMode::OnLBtnDownSlider(UINT nFlags, const CPoint& point)
{
	qdInterfaceSlider *const psldr = 
		static_cast<qdInterfaceSlider*>(get_leading_selected());
	std::pair<Vect2i, Vect2i> rect;
	GetInteractRect(psldr, rect);
	m_iActiveAnchor = 
		FindAnchorFromRect(point, rect.first, rect.second, INTERF_ELE_ANCHOR_SIZE);
}

void EditInterfMode::OnLBtnDownBtn(UINT nFlags, const CPoint& point)
{
}

void EditInterfMode::OnLBtnDownSave(UINT nFlags, const CPoint& point)
{
	qdInterfaceSave *const psave = 
		static_cast<qdInterfaceSave*>(get_leading_selected());
	std::pair<Vect2i, Vect2i> rect;
	GetInteractRect(psave, rect);
	m_iActiveAnchor = 
		FindAnchorFromRect(point, rect.first, rect.second, INTERF_ELE_ANCHOR_SIZE);
}

void EditInterfMode::OnLBtnDownTextWindow(UINT nFlags, const CPoint& point)
{
	qdInterfaceTextWindow *const textWindow = 
		static_cast<qdInterfaceTextWindow*>(get_leading_selected());
	std::pair<Vect2i, Vect2i> rect;
	GetInteractRect(textWindow, rect);
	m_iActiveAnchor = 
		FindAnchorFromRect(point, rect.first, rect.second, INTERF_ELE_ANCHOR_SIZE);
}

void EditInterfMode::DispatchOnLBtnDown(UINT nFlags, const CPoint& point)
{
	qdInterfaceElement* const pele = get_leading_selected();
	switch(pele->get_element_type()) {
	case qdInterfaceElement::EL_BACKGROUND:
		OnLBtnDownBckgrnd(nFlags, point);
		break;
	case qdInterfaceElement::EL_SLIDER:
		OnLBtnDownSlider(nFlags, point);
		break;
	case qdInterfaceElement::EL_SAVE:
		OnLBtnDownSave(nFlags, point);
		break;
	case qdInterfaceElement::EL_BUTTON:
		OnLBtnDownBtn(nFlags, point);
		break;
	case qdInterfaceElement::EL_TEXT_WINDOW:
		OnLBtnDownTextWindow(nFlags, point);
		break;
	}
}

void EditInterfMode::DispatchOnMM(UINT nFlags, CPoint const& point)
{
	m_bWasMouseMove = true;
	qdInterfaceElement* const pele = get_leading_selected();
	switch(pele->get_element_type()) {
	case qdInterfaceElement::EL_BACKGROUND:
		OnMMBckgrnd(nFlags, point);
		break;
	case qdInterfaceElement::EL_SLIDER:
		OnMMSlider(nFlags, point);
		break;
	case qdInterfaceElement::EL_SAVE:
		OnMMSave(nFlags, point);
		break;
	case qdInterfaceElement::EL_BUTTON:
		OnMMBtn(nFlags, point);
		break;
	case qdInterfaceElement::EL_TEXT_WINDOW:
		OnMMTextWindow(nFlags, point);
		break;
	}
}

void EditInterfMode::OnMMBckgrnd(UINT nFlags,	const CPoint& point)
{

}

void EditInterfMode::OnMMSave(UINT nFlags,		const CPoint& point)
{
	ASSERT(m_iActiveAnchor != -1);
	CPoint dl(point - m_base_point);
	AdjustDl(dl);

	qdInterfaceSave* const psave = 
		static_cast<qdInterfaceSave*>(get_leading_selected());
	ASSERT(psave);
	Vect2i ar(psave->thumbnail_size_x(), psave->thumbnail_size_y());
	Vect2i r(psave->r());

	CRect rect(r.x - (ar.x>>1), r.y - (ar.y>>1), r.x + (ar.x>>1), r.y + (ar.y>>1));
	ChangeAnchorRect(rect, m_iActiveAnchor, dl);
	ar.x = rect.Width();
	ar.y = rect.Height();

	psave->set_thumbnail_size_x(ar.x);
	psave->set_thumbnail_size_y(ar.y);
	psave->set_r(r);
	m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
	SetProjChange();
}

void EditInterfMode::OnMMSlider(UINT nFlags,	const CPoint& point)
{
	ASSERT(m_iActiveAnchor != -1);

	CPoint dl(point - m_base_point);
	AdjustDl(dl);

	qdInterfaceSlider* const psldr = 
		static_cast<qdInterfaceSlider*>(get_leading_selected());
	ASSERT(psldr);
	Vect2i ar(psldr->active_rectangle());
	Vect2i r(psldr->r());
	CRect rect(r.x - (ar.x>>1), r.y - (ar.y>>1), r.x + (ar.x>>1), r.y + (ar.y>>1));
	ChangeAnchorRect(rect, m_iActiveAnchor, dl);
	ar.x = rect.Width();
	ar.y = rect.Height();

	psldr->set_active_rectangle(ar);
	psldr->set_r(r);
	m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
	SetProjChange();
}

void EditInterfMode::OnMMBtn(UINT nFlags,		const CPoint& point)
{

}

void EditInterfMode::OnMMTextWindow(UINT nFlags,const CPoint& point)
{
	ASSERT(m_iActiveAnchor != -1);
	CPoint dl(point - m_base_point);
	AdjustDl(dl);

	qdInterfaceTextWindow* const textWindow = 
		static_cast<qdInterfaceTextWindow*>(get_leading_selected());
	ASSERT(textWindow);
	Vect2i ar(textWindow->text_size().x, textWindow->text_size().y);
	Vect2i r(textWindow->r());

	CRect rect(r.x - (ar.x>>1), r.y - (ar.y>>1), r.x + (ar.x>>1), r.y + (ar.y>>1));
	ChangeAnchorRect(rect, m_iActiveAnchor, dl);
	ar.x = rect.Width();
	ar.y = rect.Height();

	textWindow->set_text_size(Vect2i(ar.x, ar.y));
	textWindow->set_r(r);
	m_ptrView->Redraw(sv_api::RM_REDRAW_CONTENT);
	SetProjChange();
}
