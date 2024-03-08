#pragma once
#include "teconditioncoloredeledrawer.h"
class TEWnd;
interface IDrawData;

/**
	Дровер для отрисовки всех цепочек сразу
 */
class TEAllChainWorkModeDrawer :
	public TEConditionColoredEleDrawer
{
public:
	TEAllChainWorkModeDrawer(void);
	~TEAllChainWorkModeDrawer(void);
	virtual void draw(CDC* pdc, TEWnd* pwnd, IDrawData* pdd, CRect const& rcPaint) const;
protected:
	/// отрисовывает все цепочки
	void drawAllChains(CDC* pdc, TEWnd* pwnd, 
					IDrawData* pdd, CRect const& rcPaint) const;
	/// отрисовывает отдельную цепочку
	void drawChain(CDC* pdc, TEWnd* pwnd, const CRect& rcPaint, 
					qdTriggerChain const* ptrChain) const;
};
