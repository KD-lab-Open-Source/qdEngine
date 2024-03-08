// SPMenu.cpp: implementation of the SPMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SPMenu.h"
#include "imagelist.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SPMenu::SPMenu()
{
	VERIFY(CreatePopupMenu());
	m_ptrImageList = NULL;
	m_iMinWidth = 0;
}

SPMenu::~SPMenu()
{
	Detach();
	ASSERT(m_hMenu == NULL);    // defaul CMenu::~CMenu will destroy
}

void SPMenu::AppendPicMenuItem(UINT nID)
{
	VERIFY(AppendMenu(MF_ENABLED | MF_OWNERDRAW, nID, (LPCTSTR)nID));
}

/////////////////////////////////////////////////////////////////////////////

void SPMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	// all items are of fixed size
	lpMIS->itemWidth = (std::max)(m_ptrImageList->get_max_x(),
		static_cast<LONG>(m_iMinWidth));
	lpMIS->itemHeight = m_ptrImageList->get_max_y();
}

void SPMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	COLORREF cr = GetSysColor(COLOR_MENU);
	
	if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		// Paint the color item in the color requested
		CBrush br(cr);
		pDC->FillRect(&lpDIS->rcItem, &br);
		m_ptrImageList->DrawImage(lpDIS->hDC, &lpDIS->rcItem, 
			GetItemPos(lpDIS->itemID), cr);
	}
	
	if ((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		// item has been selected - hilite frame
		COLORREF crHilite = RGB(255-(cr&0xff),
			255-((cr>>8)&0xff), 255-((cr>>16)&0xff));
		CBrush br(crHilite);
		pDC->FrameRect(&lpDIS->rcItem, &br);
	}
	
	if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
	{
		// Item has been de-selected -- remove frame
		CBrush br(cr);
		pDC->FrameRect(&lpDIS->rcItem, &br);
	}
}

int SPMenu::GetItemPos(UINT uiID){
	for(int i = 0; ; ++i){
		if (GetMenuItemID(i)==uiID)
			return i;
	}
	return -1;
}