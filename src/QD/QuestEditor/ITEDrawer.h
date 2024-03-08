/********************************************************************
	created:	2003/05/21
	created:	21:5:2003   13:46
	filename: 	d:\Projects\Quest\QuestEditor\ITEDrawer.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	ITEDrawer
	file ext:	h
	Powerd by:	�����
	
	purpose:	������� ��������� ��� �����������
*********************************************************************/

#pragma once

interface IDrawData;

class TEWnd;
/**
	������� ��������� ������� ��� ��
 */
interface ITEDrawer
{
	/// ������������ ������ ���������� ����� \a pdd
	/**
		@param pdc	- dc ����, � ������� ����� ��������
		@param pwnd - ���� ��������� ���������
		@param pdd	- ������ ��� ���������
		@param rcPaint - �������������, ������� ���� ����������
	 */
	virtual void draw(CDC* pdc, TEWnd* pwnd, 
						IDrawData* pdd, CRect const& rcPaint) const = 0;
	/**
		������������� ��� ��������� ���������������� ��������������
		@param dc	- dc ����, � ������� ����� ��������
		@param r - �������������, ������� ���� ����������
		@param pen - �������� ��� ���������
	 */
	virtual void drawDraggedRect(HDC dc, CRect const&r, HPEN pen) const = 0;
protected:
	inline ~ITEDrawer();
};
inline ITEDrawer::~ITEDrawer()
{}
