/********************************************************************
	created:	2003/05/23
	created:	23:5:2003   18:11
	filename: 	d:\Projects\Quest\QuestEditor\TEBaseWorkModeDrawer.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TEBaseWorkModeDrawer
	file ext:	h
	Powerd by:	Илюха при идейном руководстве и моральной поддержке of Scmorr
	
	purpose:	Прорисовка базового режима
*********************************************************************/
#pragma once
#include "tebaseworkmodedrawerbase.h"

class TEBaseWorkMode;

/**
	Дровер для базового режима работы
 */
class TEBaseWorkModeDrawer :
	public TEBaseWorkModeDrawerBase
{
public:
	TEBaseWorkModeDrawer(TEBaseWorkMode* ptrWorkMode);
	~TEBaseWorkModeDrawer(void);

	virtual void draw(CDC* pdc, TEWnd* pwnd, IDrawData* pdd, CRect const& rcPaint) const;

	/// Отрисовывает линк с якорями
	virtual void drawAnchoredLink(TEWnd* pwnd, HDC dc, 
		qdTriggerElementConstPtr const& link_owner, qdTriggerLink const* link) const;
	/// Отрисовывает линк, который тянем от одного элемента к другому
	virtual void drawDragedLink(TEWnd* pwnd, HDC dc, 
		const CPoint& pt_from, const CPoint& pt_to, int type) const;
	/// Отрисовывает заголовок перетягиваемого элемента
	virtual void drawDragedTitle(TEWnd* pwnd, 
				std::string const& strTitle, CPoint pos) const;
	/// Отрисовывает якоря для линка
	void drawLinkAnchors(TEWnd* pwnd, HDC dc, 
		qdTriggerElementConstPtr const& link_owner, qdTriggerLink const* link) const;
	/// По типу линка возвращает кисть
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
	/// Карандаш для исходящих связей
	CPen m_linkOutPen;
	/// Карандаш для входщих связей
	CPen m_linkInPen;
	/// Карандаш для остальных связей
	CPen m_otherLinksPen;

	/// Кисть для исходящих связей
	CBrush m_linkOutBrush;
	/// Кисть для входящих связей
	CBrush m_linkInBrush;
	/// Кисть для остальных связей
	CBrush m_otherLinksBrush;

	/// Рабочий режим, на который мы работаем
	TEBaseWorkMode *m_ptrWorkMode;
};
