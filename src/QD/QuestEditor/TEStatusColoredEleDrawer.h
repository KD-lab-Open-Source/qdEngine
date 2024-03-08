#pragma once
#include "basetedrawer.h"

interface IDrawData;
class TEWnd;

/**
	Выбор цвета производится в зависимости от статуса в профайлере
 */
class TEStatusColoredEleDrawer :
	public BaseTEDrawer
{
public:
	TEStatusColoredEleDrawer(void);
	~TEStatusColoredEleDrawer(void);
	virtual void draw(CDC* pdc, TEWnd* pwnd, IDrawData* pdd, CRect const& rcPaint) const;
protected:
	virtual HBRUSH	SelectEleBackBrush(qdTriggerElementConstPtr const& pele) const;
	virtual HBRUSH	SelectLinkBrColor(qdTriggerLink const* pl) const;
	virtual HPEN	SelectLinkPenColor(qdTriggerLink const* pl) const;
private:
};
