// SPMenu.h: interface for the SPMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPMENU_H__ED3AA277_D131_4CE5_B9C6_78534CBF35BC__INCLUDED_)
#define AFX_SPMENU_H__ED3AA277_D131_4CE5_B9C6_78534CBF35BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ImageList;
/**
	 ласс отображает меню, в котором наход€тс€ все изображени€ из ImageList
 */
class SPMenu : public CMenu
{
public:
	// Operations
	/// ƒобавл€ет элмент меню
	void AppendPicMenuItem(UINT nID);
	
	void set_image_list(ImageList* pil){
		m_ptrImageList = pil;
	}

	VOID SetMinWidth(int w){
		m_iMinWidth = w;
	}
	// Implementation
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	SPMenu();
	virtual ~SPMenu();
private://methods
	//использовать только в случае, когда точно известно, что такой ID есть
	int GetItemPos(UINT uiID);
private:
	ImageList* m_ptrImageList;
	int m_iMinWidth;
};

#endif //!defined(AFX_SPMENU_H__ED3AA277_D131_4CE5_B9C6_78534CBF35BC__INCLUDED_)
