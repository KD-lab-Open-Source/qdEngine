#pragma once

class CTreeListCtrl;
class CTreeListItem;

struct CONTROL_CUSTOM_DRAW_INFO {
	HDC hdc;///< ������ ��������
	RECT rc;///< ������������� �����������
	COLORREF clrBckgrnd;///< ���� ����
	HBITMAP hbmpBckgrnd;///< ��������, ������� ������� �� ���
};

struct ITEM_CUSTOM_DRAW_INFO {
	HDC hdc;				///< ������ ��������
	RECT rc;				///< ������������� �����������
	COLORREF clrTextBk;		///< ���� ���� ��� �������
	COLORREF clrText;		///< ���� ������
	CTreeListItem* item;	///< ����, ������� ���������������
	int iRow;				///< ����� ������
	bool useBackgrndClr;	///< ������������ �� clrTextBk.���� � �������� ���� 
							///< ����� TLC_BKGNDIMAGE, �� false
};

struct SUBITEM_CUSTOM_DRAW_INFO {
	HDC hdc;				///< ������ ��������
	RECT rc;				///< ������������� �����������
	RECT rcText;			///< �������������, � ������� ����� ���������� �����
	COLORREF clrText;		///< ���� ������
	COLORREF clrTextBk;		///< ���� ���� ��� �������
	UINT uTextFormat;			///< ������������ �����
	CTreeListItem* item;	///< ����, ������� ���������������
	int iSubItem;			///< Subitem
	bool useBackgrndClr;	///< ������������ �� clrTextBk.���� � �������� ���� 
							///< ����� TLC_BKGNDIMAGE, �� false
};

class ICustomDrawNotifyListener
{
public:
/*!
 	��� ������� ���������� �������� ����������������� ��� ���������� ��������� �
	Windows. ��. ������������ �� NM_CUSTOMDRAW
*/
	virtual DWORD onPrepaint(CTreeListCtrl& source, CONTROL_CUSTOM_DRAW_INFO* pcdi) = 0;
	virtual DWORD onPostpaint(CTreeListCtrl& source, CONTROL_CUSTOM_DRAW_INFO* pcdi) = 0;
	virtual DWORD onPreerase(CTreeListCtrl& source, CONTROL_CUSTOM_DRAW_INFO* pcdi) = 0;
	virtual DWORD onPosterase(CTreeListCtrl& source, CONTROL_CUSTOM_DRAW_INFO* pcdi) = 0;

	virtual DWORD onItemPrepaint(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi) = 0;
	virtual DWORD onItemPostpaint(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi) = 0;
	virtual DWORD onItemPreerase(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi) = 0;
	virtual DWORD onItemPosterase(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi) = 0;

	virtual DWORD onSubitemPrepaint(CTreeListCtrl& source, SUBITEM_CUSTOM_DRAW_INFO* pscdi) = 0;
	virtual DWORD onSubitemPostpaint(CTreeListCtrl& source, SUBITEM_CUSTOM_DRAW_INFO* pscdi) = 0;

	virtual ~ICustomDrawNotifyListener();
};

inline ICustomDrawNotifyListener::~ICustomDrawNotifyListener(){}
