/********************************************************************
	created:	2003/05/24
	created:	24:5:2003   16:49
	filename: 	d:\Projects\Quest\QuestEditor\TEBaseWorkModeDrawerBase.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TEBaseWorkModeDrawerBase
	file ext:	h
	Powerd by:	Илюха при идейном руководстве и моральной поддержке of Scmorr
	
	purpose:	Базовый класс для художников базового режима работы
*********************************************************************/
#pragma once
#include "TEConditionColoredEleDrawer.h"

/**
	Базовый класс для художников базового режима работы
	Добавляет набор виртуальных методов, которые требуются базовому РР редактора
	триггеров
 */
class TEBaseWorkModeDrawerBase :
	public TEConditionColoredEleDrawer
{
public:
	TEBaseWorkModeDrawerBase(void);
	~TEBaseWorkModeDrawerBase(void);

	/// рисует линк с якорями, позволяющими изменять место положение связи
	virtual void drawAnchoredLink(TEWnd* pwnd, HDC dc, 
		qdTriggerElementConstPtr const& link_owner, qdTriggerLink const* link) const = 0;
	/// рисует линк, когда мы его тянем от одного элемента к другому
	virtual void drawDragedLink(TEWnd* pwnd, HDC dc, const CPoint& pt_from, 
		const CPoint& pt_to, int type) const = 0;
	/// рисует передвигаемый элемент
	virtual void drawDragedTitle(TEWnd* pwnd, 
		std::string const& strTitle, CPoint pos) const = 0;
	/// возвращает кисть соответствующую связи данного тип
	virtual HBRUSH getBrushByLinkType(int type) const = 0;
	/// рисуем якоря для линка
	virtual void drawLinkAnchors(TEWnd* pwnd, HDC dc, 
		qdTriggerElementConstPtr const& link_owner, qdTriggerLink const* link) const = 0;
};
