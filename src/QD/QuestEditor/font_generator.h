#ifndef __FONT_GENERATOR_H__
#define __FONT_GENERATOR_H__

class FontGenerator
{
public:
	FontGenerator();
	~FontGenerator();

	bool generate(const char* output_prefix = 0);

	bool clear();

private:

	bool isInited_;
	LOGFONT font_;

	int charRange_[2];

	Vect2i bitmapSize_;
	Vect2i bitmapSizeScaled_;

	struct BITMAPINFOX 
	{
		BITMAPINFOHEADER header_;
		RGBQUAD colors_[256];
	};

	BITMAPINFOX bitmapInfo_;
	BYTE* bitmapData_;
	HBITMAP bitmap_;

	bool updateBitmapSize(HDC hdc);
	bool createBitmap();
	bool saveBitmap(const char* file_name);
};

#endif // __FONT_GENERATOR_H__
