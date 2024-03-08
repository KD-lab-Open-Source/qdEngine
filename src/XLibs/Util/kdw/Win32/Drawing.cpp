#include "StdAfx.h"
#include <windows.h>
#include "kdw/Win32/Drawing.h"
#include "kdw/Win32/Window.h"
#include "kdw/Win32/Handle.h"

enum{
	OBM_CHECK = 32760
};

static HBITMAP checkBitmap = ::LoadBitmap(0, (LPCTSTR)OBM_CHECK);

namespace Win32{

void drawEdit3D(HDC dc, const RECT& rect, const char* text, HFONT font)
{
	RECT rt = rect;
	::FillRect(dc, &rt, GetSysColorBrush(COLOR_WINDOW));
	::DrawEdge(dc, &rt, EDGE_SUNKEN, BF_RECT);
	HFONT oldFont = (HFONT)::SelectObject(dc, font);
	::InflateRect(&rt, -3, -2);
	::DrawText(dc, text, strlen(text), &rt, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	::SelectObject(dc, oldFont);
}

void drawEditWhiteRect(HDC dc, const RECT& rect, const char* text, HFONT font)
{
	RECT rt = rect;
	::InflateRect(&rt, -1, -1);
	::FillRect(dc, &rt, GetSysColorBrush(COLOR_WINDOW));

	HFONT oldFont = (HFONT)::SelectObject(dc, font);
	::InflateRect(&rt, -5, -2);
	rt.bottom -= 1;
	::DrawText(dc, text, strlen(text), &rt, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	::SelectObject(dc, oldFont);
}


void drawGrayedCheck(HDC dc, const RECT& checkRect)
{
	int size = 16;
	RECT rect(checkRect);
	int offsetY = ((rect.bottom - rect.top) - size) / 2;
	int offsetX = ((rect.right - rect.left) - size) / 2;
	{
		Win32::StockSelector brush(dc, GetSysColorBrush(COLOR_BTNFACE));
		Win32::AutoSelector  pen(dc, CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW)));
		::RoundRect(dc, rect.left + offsetX, rect.top + offsetY, rect.left + offsetX + size, rect.top + offsetY + size, 3, 3);
	}
	ASSERT(checkBitmap);
	DrawState(dc, 0, 0, (LPARAM)checkBitmap, 0, rect.left + offsetX + 2, rect.top + offsetY + 1, size - 2, size - 2, DST_BITMAP | DSS_DISABLED);
}

void drawCheck(HDC dc, const RECT& checkRect, bool checked)
{
	int size = 16;
	RECT rect(checkRect);

	HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, GetSysColorBrush(COLOR_WINDOW));
	HPEN pen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
	HPEN oldPen = (HPEN)::SelectObject(dc, pen);
	int offsetY = ((rect.bottom - rect.top) - size) / 2;
	int offsetX = ((rect.right - rect.left) - size) / 2;
	::RoundRect(dc, rect.left + offsetX, rect.top + offsetY, rect.left + offsetX + size, rect.top + offsetY + size, 3, 3);
	::SelectObject(dc, oldPen);
	::DeleteObject(pen);
	::SelectObject(dc, oldBrush);

	if(checked){
		ASSERT(checkBitmap);
		DrawState(dc, 0, 0, (LPARAM)checkBitmap, 0, rect.left + offsetX + 2, rect.top + offsetY + 1, size - 2, size - 2, DST_BITMAP);
	}
}

void drawNotCheck(HDC dc, const RECT& checkRect, bool checked)
{
	int size = 16;
	RECT rect(checkRect);
	HBRUSH redBrush = CreateSolidBrush(RGB(255, 220, 220));
	HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, checked ? redBrush : GetSysColorBrush(COLOR_WINDOW));
	HPEN pen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
	HPEN oldPen = (HPEN)::SelectObject(dc, pen);
	int offsetY = ((rect.bottom - rect.top) - size) / 2;
	int offsetX = ((rect.right - rect.left) - size) / 2;
	::RoundRect(dc, rect.left + offsetX, rect.top + offsetY, rect.left + offsetX + size, rect.top + offsetY + size, 3, 3);
	::SelectObject(dc, oldPen);
	::DeleteObject(pen);
	::SelectObject(dc, oldBrush);
	::DeleteObject(redBrush);

	if(checked){
		HGDIOBJ oldFont = ::SelectObject(dc, Win32::defaultFont());
		COLORREF oldColor = ::SetTextColor(dc, RGB(128, 0, 0));
		int oldBkMode = ::SetBkMode(dc, TRANSPARENT);
			
		const char* text = "не";
		rect.top -= 1;
		rect.bottom -= 1;
		rect.left += 1;
		DrawText(dc, text, strlen(text), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		::SetBkMode(dc, oldBkMode);
		::SetTextColor(dc, oldColor);
		::SelectObject(dc, oldFont);
	}
}

void drawButton(HDC dc, const RECT& rect, const char* text, HFONT font)
{
	RECT rt = rect;
	::InflateRect(&rt, -1, -1);
	HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, GetSysColorBrush(COLOR_BTNFACE));
	HPEN pen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
	HPEN oldPen = (HPEN)::SelectObject(dc, pen);
	::RoundRect(dc, rt.left, rt.top, rt.right, rt.bottom, 5, 5);
	::SelectObject(dc, oldPen);
	::DeleteObject(pen);
	::SelectObject(dc, oldBrush);
	HFONT oldFont = (HFONT)::SelectObject(dc, font);
	::InflateRect(&rt, -5, -2);
	rt.bottom -= 1;
	int oldBkMode = ::SetBkMode(dc, TRANSPARENT);
	::DrawText(dc, text, strlen(text), &rt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	::SetBkMode(dc, oldBkMode);
	::SelectObject(dc, oldFont);
}

void drawEdit(HDC dc, const RECT& rect, const char* text, HFONT font, bool pathEllipsis, bool grayBackground)
{
	RECT rt = rect;
	rt.top += 1;
	rt.left += 1;
	rt.right -= 1;
	rt.bottom -= 1;
	HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, GetSysColorBrush(grayBackground ? COLOR_BTNFACE : COLOR_WINDOW));
	HPEN pen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
	HPEN oldPen = (HPEN)::SelectObject(dc, pen);
	::RoundRect(dc, rt.left, rt.top, rt.right, rt.bottom, 5, 5);
	::SelectObject(dc, oldPen);
	::DeleteObject(pen);
	::SelectObject(dc, oldBrush);
	HFONT oldFont = (HFONT)::SelectObject(dc, font);
	::InflateRect(&rt, -5, -1);
	rt.bottom -= 1;
	int oldBkMode = ::SetBkMode(dc, TRANSPARENT);
	::DrawText(dc, text, strlen(text), &rt, DT_LEFT | DT_SINGLELINE | DT_VCENTER | (pathEllipsis ? DT_PATH_ELLIPSIS : DT_END_ELLIPSIS));
	::SetBkMode(dc, oldBkMode);
	::SelectObject(dc, oldFont);
}
	
void drawComboBox(HDC dc, const RECT& rect, const char* text, HFONT font)
{
	drawEdit(dc, rect, text, font);
	/*
	RECT rt = rect;
	::FillRect(dc, &rt, GetSysColorBrush(COLOR_WINDOW));
	::DrawEdge(dc, &rt, EDGE_SUNKEN, BF_RECT);
	HFONT oldFont = (HFONT)::SelectObject(dc, font);
	::InflateRect(&rt, -3, -2);
	rt.top += 1;
	rt.left += 1;
	rt.right -= GetSystemMetrics(SM_CXVSCROLL);
	::DrawText(dc, text, strlen(text), &rt, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	RECT buttonRect = rect;
	::InflateRect(&buttonRect, -GetSystemMetrics(SM_CXEDGE), -GetSystemMetrics(SM_CYEDGE));
	buttonRect.left = buttonRect.right - GetSystemMetrics(SM_CXVSCROLL);
	::DrawFrameControl(dc, &buttonRect, DFC_SCROLL, DFCS_SCROLLDOWN);
	::SelectObject(dc, oldFont);
	*/
}


void drawColorBlend(HDC dc, const Recti& rect, const Color4f& color1, const Color4f& color2, bool alphaOnly)
{
	Win32::StockSelector pen(dc, GetStockObject(DC_PEN));
	for(int i = rect.left(); i <= rect.right (); ++i){
		float pos = float(i - rect.left()) / float(rect.width());

        Color4f solidColor, result;
		solidColor.interpolate(color1, color2, pos);

		for(int j = 0; j < 2; ++j){
			Color4f backColor = ((i / rect.height () + j) % 2) ? Color4f (1.0f, 1.0f, 1.0f) : Color4f (0.0f, 0.0f, 0.0f);
			result.interpolate3 (backColor, solidColor, solidColor.a);
			
			RECT r    = { i,      rect.top() + j * rect.height() / 2, 
						  i + 1, (rect.top() + j * rect.height() / 2) + (j ? 0 : (rect.height () / 2))
						};
			int y = rect.top() + j * rect.height() / 2;
			::MoveToEx(dc, i, y, 0);
			COLORREF color = toColorRef(alphaOnly ? Color4f(solidColor.a, solidColor.a, solidColor.a, 1.0f) : result);
			::SetDCPenColor(dc, color);
			::LineTo(dc, i, y + rect.height() / 2);
		}
	}
}

void drawVerticalBlend(HDC dc, const RECT& rect, COLORREF color1, COLORREF color2)
{
	Win32::StockSelector colorPen(dc, GetStockObject(DC_PEN));
	int height = rect.bottom - rect.top;
	for(int i = 0; i < height; ++i){
		SetDCPenColor(dc, blendColor(color1, color2, float(i) / height));
		int y = rect.top + i;
		MoveToEx(dc, rect.left, y, 0);
		LineTo(dc, rect.right - 1, y);
	}

}

void drawSlider(HDC dc, const RECT& rect, float value, bool focused)
{
	int roundness = 5;
	::SelectObject(dc, GetStockObject(NULL_PEN));
	::SelectObject(dc, GetSysColorBrush(COLOR_3DSHADOW));
	::RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, roundness, roundness);
	
	HRGN region = CreateRoundRectRgn(rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1, roundness, roundness - 2);
	::SelectClipRgn(dc, region);

	::FillRect(dc, &rect, GetSysColorBrush(COLOR_BTNFACE));
	::SelectObject(dc, GetSysColorBrush(COLOR_WINDOW));
	RoundRect(dc, rect.left + 1 + 1, rect.top + 1 + 1, rect.right - 1 + 1, rect.bottom - 1 + 1, roundness, roundness - 2);

	RECT filledRect = { rect.left + 1, rect.top + 1, rect.left + 1 + round((rect.right - rect.left - 2) * clamp(value, 0.0f, 1.0f)), rect.bottom - 1};
	::FillRect(dc, &filledRect, focused ? GetSysColorBrush(COLOR_HIGHLIGHT) : GetSysColorBrush(COLOR_BTNFACE));
	
	RECT handleRect = { filledRect.right - 2,  filledRect.top - 2, filledRect.right + 2, filledRect.bottom + 1 };
	::SelectClipRgn(dc, 0);

	::FillRect(dc, &handleRect, GetSysColorBrush(COLOR_3DSHADOW));
	handleRect.left += 1;
	handleRect.top += 1;
	handleRect.bottom -= 1;
	handleRect.right -= 1;
	::FillRect(dc, &handleRect, GetSysColorBrush(COLOR_BTNFACE));
	

	DeleteObject(region);
}

}
