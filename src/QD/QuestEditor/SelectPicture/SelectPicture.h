#if !defined(AFX_SELECTPICTURE_H__41D800A9_19B6_4D33_8205_A986E9FF03FD__INCLUDED_)
#define AFX_SELECTPICTURE_H__41D800A9_19B6_4D33_8205_A986E9FF03FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectPicture.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SelectPicture window
#include <memory>
#include "imagelist.h"
#include "spmenu.h"

template<int _Image>
class ImageType{
};

/**
	Свойства типа изображения. Определяет функции для конвертирования своего типа 
	в битмап
 */
template<class _ImageType>
class ITTraits{
	public:
		/// конвертирует изображение, находящееся в файле, в битмап
		static HBITMAP Convert(LPCTSTR lpszFileName, SIZE& dims, BOOL* pUseAlpha){
			ASSERT(0);
			return NULL;
		}
		/// конвертирует изображение в битмап
		static HBITMAP Convert(VOID *lpData, size_t size, SIZE& dims, BOOL* pUseAlpha){
			ASSERT(0);
			return NULL;
		}
};

const int IT_TGA_IMAGE = 0;
template<>
class ITTraits<ImageType<IT_TGA_IMAGE> >{
	public:
		static HBITMAP Convert(LPCTSTR lpszFileName, SIZE& dims, BOOL* pUseAlpha);
		static HBITMAP Convert(VOID *lpData, size_t size, SIZE& dims, BOOL* pUseAlpha);
};


//посылаются перед сменой картинки и после
const unsigned NM_SB_IMAGE_CHANGING = (NM_FIRST + 1U);
const unsigned NM_SB_IMAGE_CHANGED = (NM_FIRST + 2U);

typedef struct tagSP_NOTIFY_INFO {
	NMHDR nmhdr;
	UINT iNewItem;
	UINT iOldItem;
}SP_NOTIFY_INFO;

/**
	Класс позволяет загрузить список картинок. Чтобы затем их поодной показывать
 */
class SelectPicture : public CButton
{
// Construction
public:
	SelectPicture();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SelectPicture)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SelectPicture();

	virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	template<class _ImageType>
	bool AddImage(LPCTSTR lpszFileName, DWORD dwData, _ImageType t)
	{
		if(m_ptrImageList->add_image(lpszFileName, t))
		{
			UINT __t = m_ptrImageList->size()-1;
			m_ptrImageList->set_image_data(__t, dwData);
			m_menu.AppendPicMenuItem(__t);

			RedrawWindow();
			return true;
		}
		return false;
	}

	int GetSelectedImage() const{
		return m_iCurrImage;
	}
	BOOL SelectImage(int idx);

	BOOL SetImageData(int idx, DWORD dwData);
	DWORD GetImageData(int idx) const;
	UINT ImagesCount() const{
		return m_ptrImageList->size();
	}

	//возвращает изображение следующее за этим или -1
	int RemoveImage(int iImage);
	// Generated message map functions
protected:
	//{{AFX_MSG(SelectPicture)
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP();

private:
	CRect m_btn_rect;
//	CRect m_cap_rect;
	CRect m_left_btn;
	CRect m_right_btn;

	enum eMouseLayout{ML_NONE, ML_PICTURE, ML_BUTTON, ML_LEFT, ML_RIGHT};
	eMouseLayout m_eMouseLayout;

	/// Список картинок
	std::auto_ptr<class ImageList> m_ptrImageList;
	/// Выпадающее по правой кнопке меню
	SPMenu m_menu;

	int m_iCurrImage;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_SELECTPICTURE_H__41D800A9_19B6_4D33_8205_A986E9FF03FD__INCLUDED_)
