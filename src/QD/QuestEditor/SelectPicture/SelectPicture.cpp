// SelectPicture.cpp : implementation file
//

#include "stdafx.h"
#include "SelectPicture.h"

#include <ASSERT.h>
#include <additional//auto_object.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/*
*/

/////////////////////////////////////////////////////////////////////////////
// SelectPicture

SelectPicture::SelectPicture():m_ptrImageList(new ImageList)
{
	m_eMouseLayout = ML_NONE;
	m_iCurrImage = -1;	
	m_menu.set_image_list(m_ptrImageList.get());
}

SelectPicture::~SelectPicture()
{
}


BEGIN_MESSAGE_MAP(SelectPicture, CButton)
	//{{AFX_MSG_MAP(SelectPicture)
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectPicture message handlers

BOOL SelectPicture::SetImageData(int idx, DWORD dwData)
{
	m_ptrImageList->set_image_data(idx, dwData);
	return TRUE;
}

DWORD SelectPicture::GetImageData(int idx) const
{
	return m_ptrImageList->get_image_data(idx);
}

VOID SelectPicture::DrawItem(LPDRAWITEMSTRUCT lpdi)
{
	UINT uStyle = DFCS_BUTTONPUSH;

	bool bDisabled = lpdi->itemState&ODS_DISABLED;
	// This code only works with buttons.
	ASSERT(lpdi->CtlType == ODT_BUTTON);

	//рисуем левую кнопку
	if (lpdi->itemState & ODS_SELECTED&&m_eMouseLayout == ML_LEFT)
		uStyle |= DFCS_PUSHED;
	if (m_iCurrImage <= 0||bDisabled)
	{
		uStyle |= DFCS_INACTIVE;
		uStyle &= ~DFCS_PUSHED;
	}


	::DrawFrameControl(lpdi->hDC, m_left_btn, DFC_BUTTON, uStyle);

	//рисуем центральную кнопку
	uStyle = DFCS_BUTTONPUSH;
	
	if (lpdi->itemState & ODS_SELECTED&&m_eMouseLayout == ML_BUTTON
		&&!(m_iCurrImage < 0||lpdi->itemState&ODS_DISABLED))
		uStyle |= DFCS_PUSHED;
	
	::DrawFrameControl(lpdi->hDC, m_btn_rect, DFC_BUTTON, uStyle);

	//рисуем правую конпку

	uStyle = DFCS_BUTTONPUSH;
	
	if (lpdi->itemState & ODS_SELECTED&&m_eMouseLayout == ML_RIGHT&&
		!(m_iCurrImage == (m_ptrImageList->size()-1)||bDisabled))
		uStyle |= DFCS_PUSHED;
	
	::DrawFrameControl(lpdi->hDC, m_right_btn, DFC_BUTTON, uStyle);
	
	int imode = SetBkMode(lpdi->hDC, TRANSPARENT);

	const TCHAR LEFT_CAP[] = _T("<<");
	const TCHAR RIGHT_CAP[] = _T(">>");
	const TCHAR NO_IMAGE_SELECTED[] = _T("Нет выбранного\nизображения");

	//заголовок на кнопку "влево"
	if (m_iCurrImage <= 0||bDisabled)
	{
		int oldClr = SetTextColor(lpdi->hDC, GetSysColor(COLOR_GRAYTEXT));
		::DrawText(lpdi->hDC, LEFT_CAP, 2,m_left_btn, DT_CENTER|DT_VCENTER);
		SetTextColor(lpdi->hDC, oldClr);
	}
	else
		::DrawText(lpdi->hDC, LEFT_CAP, 2,m_left_btn, DT_CENTER|DT_VCENTER);

	//загловок на кнопку "вправо"
	if (m_iCurrImage == (m_ptrImageList->size()-1)||bDisabled)
	{
		int oldClr = SetTextColor(lpdi->hDC, GetSysColor(COLOR_GRAYTEXT));
		::DrawText(lpdi->hDC, RIGHT_CAP, 2,m_right_btn, DT_CENTER|DT_VCENTER);
		SetTextColor(lpdi->hDC, oldClr);
	}
	else
		::DrawText(lpdi->hDC, RIGHT_CAP, 2,m_right_btn, DT_CENTER|DT_VCENTER);
		
	//заголовок на центральную кнопку
	CString strText;
	GetWindowText(strText);
	if (m_iCurrImage < 0||bDisabled)
	{
		int oldClr = SetTextColor(lpdi->hDC, GetSysColor(COLOR_GRAYTEXT));
		::DrawText(lpdi->hDC,strText, -1, m_btn_rect,DT_CENTER|DT_VCENTER);
		SetTextColor(lpdi->hDC, oldClr);
	}
	else
		::DrawText(lpdi->hDC,strText, -1, m_btn_rect,DT_CENTER|DT_VCENTER);

	CRect rcPicture(lpdi->rcItem);
	rcPicture.top = m_btn_rect.bottom + 1;
	if (m_iCurrImage != -1)
		m_ptrImageList->DrawImage(lpdi->hDC, rcPicture, m_iCurrImage, 
													GetSysColor(COLOR_WINDOW));
	else
	{
		if (bDisabled){
			int oldClr = SetTextColor(lpdi->hDC, GetSysColor(COLOR_GRAYTEXT));
			DrawText(lpdi->hDC, NO_IMAGE_SELECTED,-1,&rcPicture, DT_CENTER|DT_VCENTER);
			SetTextColor(lpdi->hDC, oldClr);
		}
		else
			DrawText(lpdi->hDC, NO_IMAGE_SELECTED,-1,&rcPicture, DT_CENTER|DT_VCENTER);
	}

	SetBkMode(lpdi->hDC, imode);
}

UINT SelectPicture::OnNcHitTest(CPoint point) 
{
	UINT res = CButton::OnNcHitTest(point);
	if (res == HTCLIENT)
	{
		ScreenToClient(&point);
		if (m_btn_rect.PtInRect(point))
			m_eMouseLayout = ML_BUTTON;
		else if (m_left_btn.PtInRect(point))
			m_eMouseLayout = ML_LEFT;
		else if (m_right_btn.PtInRect(point))
			m_eMouseLayout = ML_RIGHT;
		else
			m_eMouseLayout = ML_PICTURE;
	}
	else
		m_eMouseLayout = ML_NONE;
	return res;
}

void SelectPicture::PreSubclassWindow() 
{
	const int NON_PICTURE_AREA_HEIGHT = 20;
	const int BUTTON_AREA_WIDTH = 30;
	const int NAVIG_BTN_WIDTH = 18;

	CRect r;
	GetClientRect(r);
	r.bottom = r.top + NON_PICTURE_AREA_HEIGHT;
	
	m_btn_rect.left = r.right;
	m_btn_rect.bottom = m_btn_rect.top + NON_PICTURE_AREA_HEIGHT;

	m_left_btn = r;
	m_left_btn.right = NAVIG_BTN_WIDTH;
	m_right_btn = r;
	m_right_btn.left = m_right_btn.right - NAVIG_BTN_WIDTH;

	m_btn_rect = r;
	m_btn_rect.left = m_left_btn.right;
	m_btn_rect.right = m_right_btn.left;
		
	CButton::PreSubclassWindow();
}

void SelectPicture::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CButton::OnLButtonDown(nFlags, point);
}

void SelectPicture::OnClicked() 
{
	if(!IsWindowEnabled())
		return;
	if (m_eMouseLayout == ML_BUTTON)
	{
		CPoint p(m_left_btn.left, m_left_btn.bottom);
		ClientToScreen(&p);
		m_menu.SetMinWidth(m_btn_rect.Width());
		m_menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_LEFTBUTTON, p.x, p.y, this);
	}
	else if (m_eMouseLayout == ML_LEFT){
		if (m_iCurrImage > 0)
			SelectImage(m_iCurrImage - 1);

	}
	else if (m_eMouseLayout == ML_RIGHT) {
		if (m_iCurrImage < m_ptrImageList->size()-1)
			SelectImage(m_iCurrImage + 1);
	}

}

int SelectPicture::RemoveImage(int iImage)
{
	if (iImage == -1)
		return -1;
	int res = m_ptrImageList->remove_image(iImage);

	m_menu.DeleteMenu(iImage, MF_BYPOSITION);
	if (res == -1){
		SelectImage(m_ptrImageList->size()-1);
		return m_ptrImageList->size()-1;
	}


	SelectImage(res);
	return res;
}

BOOL SelectPicture::SelectImage(int idx)
{
	ASSERT(-1 <= idx&&idx< static_cast<int>(m_ptrImageList->size()));
	if (idx==-1&&m_iCurrImage == -1)
		return FALSE;

	SP_NOTIFY_INFO ni;
	ni.nmhdr.hwndFrom = m_hWnd;
	ni.nmhdr.idFrom = GetDlgCtrlID();
	
	ni.nmhdr.code = NM_SB_IMAGE_CHANGING;
	
	ni.iNewItem = idx;
	ni.iOldItem = m_iCurrImage;
	
	if(GetParent()->SendMessage(WM_NOTIFY,NM_SB_IMAGE_CHANGING, (LPARAM)&ni))
		return TRUE;
	
	m_iCurrImage = idx;
	RedrawWindow();

	ni.nmhdr.code = NM_SB_IMAGE_CHANGED;
	GetParent()->SendMessage(WM_NOTIFY,NM_SB_IMAGE_CHANGED, (LPARAM)&ni);
	
	return TRUE;
}

BOOL SelectPicture::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	ASSERT(HIWORD(wParam) == 0);
	DWORD low = LOWORD(wParam);
	MENUITEMINFO mii;
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_DATA;
	m_menu.GetMenuItemInfo(low,&mii);

	if (m_iCurrImage != low) 
		SelectImage(low);

	return CButton::OnCommand(wParam, lParam);
}

#if _MSC_VER < 1310
template<>
#endif // _MSC_VER < 1310
HBITMAP ITTraits<ImageType<IT_TGA_IMAGE> >::
Convert(LPCTSTR lpszFileName, SIZE& dims, BOOL* pUseAlpha)
{
	
	int i,sx,sy,flags,idx,ssx,colors;
	unsigned char header[18];
	
	char* data;
	
	XStream fh(lpszFileName,XS_IN);
	fh.read(header,18);
	
	if(header[0]) // Length of Image ID field
		fh.seek(header[0],XS_CUR);
	
	if(header[1]) // Color map type (0 is no color map)
		ErrH.Abort("Paletted TGA not supported");
	
	if(header[2] != 2) // TGA file type
		ErrH.Abort("Compressed TGA not supported");
	
	sx = header[12] + (header[13] << 8); 
	sy = header[14] + (header[15] << 8); 
	
	colors = header[16];
	flags = header[17];
	
	ssx = sx * colors/8;
	*pUseAlpha = (colors == 32);
	
	BITMAPINFO bi;
	
	
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biBitCount = colors;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biWidth = sx;
	bi.bmiHeader.biHeight = -sy;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biSizeImage = sx * sy;
	bi.bmiHeader.biXPelsPerMeter = sx;
	bi.bmiHeader.biYPelsPerMeter = sy;
	bi.bmiHeader.biClrImportant = 0;
	
	HDC hdc = GetDC(NULL);
	
	axl::auto_object<HBITMAP> hBmp(
		CreateDIBSection(hdc,&bi,DIB_RGB_COLORS,(void**)&(data),NULL,NULL));
	
	ReleaseDC(NULL,hdc);
	if (!hBmp.get())
		return NULL;
	
	int wx = ssx;
	if(wx % 4)
		wx = ssx - (ssx % 4) + 4;
	
	memset(data,0,wx * sy);
	
	if(!(flags & 0x20)){
		idx = (sy - 1) * wx;
		for(i = 0; i < sy; i ++){
			fh.read(data + idx,ssx);
			idx -= wx;
		}
	}
	else {
		idx = 0;
		for(i = 0; i < sy; i ++){
			fh.read(data + idx,ssx);
			idx += wx;
		}
	}
	
	fh.close();
	
	dims.cx = sx;
	dims.cy = sy;
	
	return hBmp.detach();
};
