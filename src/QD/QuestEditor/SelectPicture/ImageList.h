// ImageList.h: interface for the ImageList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGELIST_H__2D62FE0A_24E1_41F9_8227_4CC5E3649AD3__INCLUDED_)
#define AFX_IMAGELIST_H__2D62FE0A_24E1_41F9_8227_4CC5E3649AD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>


/**
	Список изображений. При добавлении имиджа использует свойства его(имиджа) типа,
	чтобы сконвертировать его в битмап
 */
class ImageList  
{
public:
	ImageList()
	{
		m_max_pic_size.cx = m_max_pic_size.cy = 0;
	}
	
	~ImageList()
	{
		bitmap_vec_t::iterator i = m_pictures.begin(), e = m_pictures.end();
		for(; i != e; ++i)
			DeleteObject((*i).hbmp);
	}
	
public:
	template<class _ImageType>
	inline bool add_image(LPCTSTR lpszFileName, _ImageType)
	{
		IMAGE_INFO ii;
		ii.hbmp = ITTraits<_ImageType>::Convert(lpszFileName, ii.dims, &ii.bUseAlpha);
		if (!ii.hbmp)
			return false;
		
		m_max_pic_size.cx = (std::max)(ii.dims.cx, m_max_pic_size.cx);
		m_max_pic_size.cy = (std::max)(ii.dims.cy, m_max_pic_size.cy);
		
		m_pictures.push_back(ii);
		return true;
	}	
	HBITMAP get_image(int idx)const{
		return m_pictures[idx].hbmp;
	}
	BOOL use_alpha(int idx)const{
		return m_pictures[idx].bUseAlpha;
	}

	LONG get_max_x() const{
		return m_max_pic_size.cx;
	}
	LONG get_max_y() const{
		return m_max_pic_size.cy;
	}
	LONG get_image_sx(int idx) const{
		return m_pictures[idx].dims.cx;
	}
	LONG get_image_sy(int idx) const{
		return m_pictures[idx].dims.cy;
	}
	DWORD get_image_data(int idx) const{
		return m_pictures[idx].dwData;
	}
	VOID set_image_data(int idx, DWORD dwData){
		m_pictures[idx].dwData = dwData;
	}

	/// Отрисовывает изображение в DC
	VOID DrawImage(HDC dc, RECT* rc, int idx, COLORREF dest_color);

	size_t size() const{
		return m_pictures.size();
	}
	int remove_image(int iImage);
private:
	typedef struct tagIMAGE_INFO {
		HBITMAP hbmp;
		SIZE dims;
		BOOL bUseAlpha;
		DWORD dwData;
		tagIMAGE_INFO():hbmp(NULL), bUseAlpha(FALSE), dwData(0){
			dims.cx = dims.cy = NULL;
		}
	}IMAGE_INFO;

	typedef std::vector<IMAGE_INFO> bitmap_vec_t;
	/// Максимальный размер изображений
	CSize m_max_pic_size;
	/// Список картинок
	bitmap_vec_t m_pictures;
private:
	VOID DrawPicWithAlpha(HDC dc, RECT* rc, const ImageList::IMAGE_INFO& ii, COLORREF dest_color);
	VOID DrawPic(HDC dc, RECT *rc, const IMAGE_INFO& ii);
	void update_pic_size();
};

#endif //!defined(AFX_IMAGELIST_H__2D62FE0A_24E1_41F9_8227_4CC5E3649AD3__INCLUDED_)
