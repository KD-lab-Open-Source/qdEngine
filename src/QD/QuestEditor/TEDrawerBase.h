/********************************************************************
	created:	2003/05/23
	created:	23:5:2003   13:19
	filename: 	d:\Projects\Quest\QuestEditor\TEDrawerBase.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TEDrawerBase
	file ext:	h
	Powerd by:	����� ��� ������� ����������� � ��������� ��������� of Scmorr
	
	purpose:	������� ���������� ���������� ITEDrawer
*********************************************************************/

#pragma once
#include "itedrawer.h"

/**
	������� ���������� ���������� ITEDrawer
	������� ������ �� ������
 */
class TEDrawerBase : public ITEDrawer
{
public:
	TEDrawerBase(void);
	~TEDrawerBase(void);
	virtual void draw(CDC* pdc, TEWnd* pwnd, CRect const& rcPaint) const;
	virtual void drawDraggedRect(HDC dc, CRect const&r, HPEN hpen) const;
};
