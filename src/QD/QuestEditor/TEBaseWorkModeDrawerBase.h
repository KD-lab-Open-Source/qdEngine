/********************************************************************
	created:	2003/05/24
	created:	24:5:2003   16:49
	filename: 	d:\Projects\Quest\QuestEditor\TEBaseWorkModeDrawerBase.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TEBaseWorkModeDrawerBase
	file ext:	h
	Powerd by:	����� ��� ������� ����������� � ��������� ��������� of Scmorr
	
	purpose:	������� ����� ��� ���������� �������� ������ ������
*********************************************************************/
#pragma once
#include "TEConditionColoredEleDrawer.h"

/**
	������� ����� ��� ���������� �������� ������ ������
	��������� ����� ����������� �������, ������� ��������� �������� �� ���������
	���������
 */
class TEBaseWorkModeDrawerBase :
	public TEConditionColoredEleDrawer
{
public:
	TEBaseWorkModeDrawerBase(void);
	~TEBaseWorkModeDrawerBase(void);

	/// ������ ���� � �������, ������������ �������� ����� ��������� �����
	virtual void drawAnchoredLink(TEWnd* pwnd, HDC dc, 
		qdTriggerElementConstPtr const& link_owner, qdTriggerLink const* link) const = 0;
	/// ������ ����, ����� �� ��� ����� �� ������ �������� � �������
	virtual void drawDragedLink(TEWnd* pwnd, HDC dc, const CPoint& pt_from, 
		const CPoint& pt_to, int type) const = 0;
	/// ������ ������������� �������
	virtual void drawDragedTitle(TEWnd* pwnd, 
		std::string const& strTitle, CPoint pos) const = 0;
	/// ���������� ����� ��������������� ����� ������� ���
	virtual HBRUSH getBrushByLinkType(int type) const = 0;
	/// ������ ����� ��� �����
	virtual void drawLinkAnchors(TEWnd* pwnd, HDC dc, 
		qdTriggerElementConstPtr const& link_owner, qdTriggerLink const* link) const = 0;
};
