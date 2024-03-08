#include "stdafx.h"

#include "font_generator.h"
#include "tga.h"
#include "gemsiii\filter.h"

const int SCALE = 7;

inline int round_up(int x){ return ((x+SCALE-1)/SCALE)*SCALE; }

FontGenerator::FontGenerator()
{
	isInited_ = false;

	charRange_[0] = 32;
	charRange_[1] = 256;

	bitmapSize_ = bitmapSizeScaled_ = Vect2i(0,0);

	bitmapData_ = 0;
	bitmap_ = 0;
}

FontGenerator::~FontGenerator()
{
	clear();
}

bool FontGenerator::generate(const char* output_prefix)
{
	CHOOSEFONT cf;
	static DWORD rgb;

	// Initialize CHOOSEFONT
	ZeroMemory(&cf, sizeof(CHOOSEFONT));
	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = 0;
	cf.lpLogFont = &font_;
	cf.rgbColors = rgb;
	cf.Flags = CF_SCREENFONTS;

	if(!ChooseFont(&cf))
		return false;

	font_.lfHeight *= -SCALE;
	isInited_ = true;

	if(!output_prefix)
		output_prefix = strlen(cf.lpLogFont->lfFaceName) ? cf.lpLogFont->lfFaceName : "font";

	HDC hdc_screen = GetDC(NULL);
	HDC hdc = CreateCompatibleDC(hdc_screen);

	bitmapSize_ = Vect2i(128, 128);
	bitmapSizeScaled_ = bitmapSize_ * SCALE;

	if(!updateBitmapSize(hdc))
		return false;

	if(!createBitmap())
		return false;

	HBITMAP holdbitmap = (HBITMAP)SelectObject(hdc, bitmap_);
	SetTextColor(hdc, RGB(255,255,255));
	SetBkColor(hdc, RGB(0,0,0));

	char file_idx[_MAX_PATH];
	char file_tga[_MAX_PATH];

	_makepath(file_idx,"","",output_prefix,"idx");
	_makepath(file_tga,"","",output_prefix,"tga");

	FILE* f = fopen(file_idx,"wt");

	int dx = bitmapSizeScaled_.x;
	int dy = bitmapSizeScaled_.y;

	fprintf(f, "%i %i\n", dx/SCALE, dy/SCALE);
	fprintf(f, "%i\n", charRange_[1] - charRange_[0]);

	HFONT hfont = CreateFontIndirect(&font_);
	HFONT holdfont = (HFONT)SelectObject(hdc, hfont);

	RECT rc;
	rc.left = rc.top = rc.right = rc.bottom=0;
	DrawText(hdc, "A", 1, &rc, DT_LEFT | DT_NOPREFIX | DT_CALCRECT);
	int height = rc.bottom;

	rc.left = rc.top = 0;
	for(int i = charRange_[0]; i < charRange_[1]; i++){
		rc.right = rc.left;
		rc.bottom = rc.top;
		DrawText(hdc, (const char*)&i, 1, &rc, DT_LEFT | DT_NOPREFIX | DT_CALCRECT);
		rc.right = round_up(rc.right);
		rc.bottom = round_up(rc.bottom);
		
		if(rc.right > dx){
			rc.top += ((height+SCALE*2)/SCALE)*SCALE;
			rc.left = 0;
			rc.right = rc.left;
			rc.bottom = rc.top;
			DrawText(hdc, (const char*)&i, 1, &rc, DT_LEFT | DT_NOPREFIX | DT_CALCRECT);
			rc.right = round_up(rc.right);
			rc.bottom = round_up(rc.bottom);
		}

		DrawText(hdc, (const char*)&i, 1, &rc, DT_LEFT | DT_NOPREFIX);

		fprintf(f, "%i %i %i %i %i\n", i, rc.left/SCALE, rc.top/SCALE,
					(rc.right - rc.left)/SCALE, (rc.bottom - rc.top)/SCALE);

		rc.left = rc.right + SCALE*2;
	}

	SelectObject(hdc, holdfont);
	DeleteObject(hfont);

	fprintf(f,"\nFile format:\n"
		"bitmapSX bitmapSY\n"
		"numChars\n"
		"chrID0 X Y SX SY\n"
		"chrID1 X Y SX SY\n"
		"...\n"
		"chrIDN X Y SX SY\n");

	fclose(f);

	SelectObject(hdc, holdbitmap);
	DeleteDC(hdc);

	if(!saveBitmap(file_tga)){
		clear();
		return false;
	}

	clear();

	return true;
}

bool FontGenerator::clear()
{
	if(bitmap_){
		DeleteObject(bitmap_);
		bitmap_ = 0;
	}

	return true;
}

bool FontGenerator::updateBitmapSize(HDC hdc)
{
	bitmapSize_;
	bitmapSizeScaled_;

	HFONT hfont = CreateFontIndirect(&font_);
	HFONT holdfont=(HFONT)SelectObject(hdc, hfont);

	int dx = bitmapSizeScaled_.x;
	int dy = bitmapSizeScaled_.y;

	bool ret_flag = false;
	bool y_inc = false;
	while(!ret_flag){
		RECT rc;
		rc.left = rc.top = rc.right = rc.bottom=0;
		DrawText(hdc, "A", 1, &rc, DT_LEFT | DT_NOPREFIX | DT_CALCRECT);
		int height=rc.bottom;

		rc.left=0;
		rc.top=0;
		for(int i = charRange_[0]; i < charRange_[1]; i++){
			rc.right = rc.left;
			rc.bottom = rc.top;
			DrawText(hdc, (const char*)&i, 1, &rc, DT_LEFT | DT_NOPREFIX | DT_CALCRECT);
			rc.right = round_up(rc.right);
			rc.bottom = round_up(rc.bottom);

			if(rc.right > dx){
				rc.top += ((height + SCALE*2) / SCALE) * SCALE;
				rc.left = 0;
				rc.right = rc.left;
				rc.bottom = rc.top;
				DrawText(hdc, (const char*)&i, 1, &rc, DT_LEFT | DT_NOPREFIX | DT_CALCRECT);
				rc.right = round_up(rc.right);
				rc.bottom = round_up(rc.bottom);
			}

			rc.left = rc.right + SCALE*2;
		}

		if(rc.bottom <= dy && rc.right <= dx)
			ret_flag = true;
		else {
			if(!y_inc){
				dx *= 2;
				y_inc = true;
			}
			else {
				dy *= 2;
				y_inc = false;
			}
		}
	}

	SelectObject(hdc, holdfont);
	DeleteObject(hfont);

	bitmapSizeScaled_ = Vect2i(dx, dy);
	bitmapSize_ = bitmapSizeScaled_ / SCALE;

	return true;
}

bool FontGenerator::createBitmap()
{
	HDC hdc_screen = GetDC(NULL);

	for(int i = 0; i < 256; i++)
	{
		RGBQUAD& p = bitmapInfo_.colors_[i];
		p.rgbBlue = p.rgbGreen = p.rgbRed = i;
		p.rgbReserved = 0;
	}

	BITMAPINFOHEADER& bmh = bitmapInfo_.header_;
	bmh.biSize = sizeof(BITMAPINFOHEADER);
	bmh.biWidth = bitmapSizeScaled_.x;
	bmh.biHeight = -bitmapSizeScaled_.y;
	bmh.biPlanes = 1;
	bmh.biBitCount = 8;
	bmh.biCompression = BI_RGB;
	bmh.biSizeImage = 0;
	bmh.biXPelsPerMeter = 100;
	bmh.biYPelsPerMeter = 100;
	bmh.biClrUsed = 256;
	bmh.biClrImportant = 0;

	bitmap_ = CreateDIBSection(hdc_screen, (BITMAPINFO*)&bitmapInfo_, DIB_RGB_COLORS, 
		(void**)&bitmapData_, NULL, 0);

	return true;
}

bool FontGenerator::saveBitmap(const char* file_name)
{
	struct RGBA
	{
		BYTE r,g,b,a;
	};

	int dx = bitmapSizeScaled_.x;
	int dy = bitmapSizeScaled_.y;

	int cx = dx / SCALE;
	int cy = dy / SCALE;

	RGBA* buf=new RGBA[cx*cy];

	BYTE* bmin=new BYTE[cx*cy];

	resample(bitmapData_, dx, dy, bmin, cx, cy, 'm');
 
	SaveTga(file_name, cx, cy, (BYTE*)bmin, 1);
/*
	for(int y=0; y < cy; y++){
		for(int x=0; x<cx; x++){
			RGBA& out = buf[y * cx + x];
			out.r = out.g = out.b = 255;
			out.a = bmin[y * cx + x];
		}
	}
*/
	delete[] bmin;
	delete buf;

	return true;
}
