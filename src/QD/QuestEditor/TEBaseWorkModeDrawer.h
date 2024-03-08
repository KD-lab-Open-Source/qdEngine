/********************************************************************
	created:	2003/05/23
	created:	23:5:2003   18:11
	filename: 	d:\Projects\Quest\QuestEditor\TEBaseWorkModeDrawer.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TEBaseWorkModeDrawer
	file ext:	h
	Powerd by:	����� ��� ������� ����������� � ��������� ��������� of Scmorr
	
	purpose:	���������� �������� ������
*********************************************************************/
#pragma once
#include "tebaseworkmodedrawerbase.h"

class TEBaseWorkMode;

/**
	������ ��� �������� ������ ������
 */
class TEBaseWorkModeDrawer :
	public TEBaseWorkModeDrawerBase
{
public:
	TEBaseWorkModeDrawer(TEBaseWorkMode* ptrWorkMode);
	~TEBaseWorkModeDrawer(void);

	virtual void draw(CDC* pdc, TEWnd* pwnd, IDrawData* pdd, CRect const& rcPaint) const;

	/// ������������ ���� � �������
	virtual void drawAnchoredLink(TEWnd* pwnd, HDC dc, 
		qdTriggerElementConstPtr const& link_owner, qdTriggerLink const* link) const;
	/// ������������ ����, ������� ����� �� ������ �������� � �������
	virtual void drawDragedLink(TEWnd* pwnd, HDC dc, 
		const CPoint& pt_from, const CPoint& pt_to, int type) const;
	/// ������������ ��������� ��������������� ��������
	virtual void drawDragedTitle(TEWnd* pwnd, 
				std::string const& strTitle, CPoint pos) const;
	/// ������������ ����� ��� �����
	void drawLinkAnchors(TEWnd* pwnd, HDC dc, 
		qdTriggerElementConstPtr const& link_owner, qdTriggerLink const* link) const;
	/// �� ���� ����� ���������� �����
	virtual HBRUSH getBrushByLinkType(int type) const{
		return getLinkTypeBrush(type);
	}
protected:
	BOOL setPenCant(HPEN hpen);
	HPEN getPenCant() const{
		return m_penCant;
	}

	virtual HBRUSH	SelectLinkBrColor(qdTriggerLink const* pl) const;
	virtual HPEN	SelectLinkPenColor(qdTriggerLink const* pl) const;

	BOOL setLinkOutPen(HPEN hpen);
	BOOL setLinkInPen(HPEN hpen);
	BOOL setOtherLinksPen(HPEN hpen);

	BOOL setLinkOutBrush(HPEN hbrush);
	BOOL setLinkInBrush(HPEN hbrush);
	BOOL setOtherLinksBrush(HPEN hbrush);

	HPEN getLinkOutPen() const{
		return m_linkOutPen;
	}
	HPEN getLinkInPen() const{
		return m_linkInPen;
	}
	HPEN getOtherLinksPen() const{
		return m_otherLinksPen;
	}
private:
	CPen m_penCant;
	/// �������� ��� ��������� ������
	CPen m_linkOutPen;
	/// �������� ��� ������� ������
	CPen m_linkInPen;
	/// �������� ��� ��������� ������
	CPen m_otherLinksPen;

	/// ����� ��� ��������� ������
	CBrush m_linkOutBrush;
	/// ����� ��� �������� ������
	CBrush m_linkInBrush;
	/// ����� ��� ��������� ������
	CBrush m_otherLinksBrush;

	/// ������� �����, �� ������� �� ��������
	TEBaseWorkMode *m_ptrWorkMode;
};
