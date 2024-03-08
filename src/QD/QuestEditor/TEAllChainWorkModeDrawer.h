#pragma once
#include "teconditioncoloredeledrawer.h"
class TEWnd;
interface IDrawData;

/**
	������ ��� ��������� ���� ������� �����
 */
class TEAllChainWorkModeDrawer :
	public TEConditionColoredEleDrawer
{
public:
	TEAllChainWorkModeDrawer(void);
	~TEAllChainWorkModeDrawer(void);
	virtual void draw(CDC* pdc, TEWnd* pwnd, IDrawData* pdd, CRect const& rcPaint) const;
protected:
	/// ������������ ��� �������
	void drawAllChains(CDC* pdc, TEWnd* pwnd, 
					IDrawData* pdd, CRect const& rcPaint) const;
	/// ������������ ��������� �������
	void drawChain(CDC* pdc, TEWnd* pwnd, const CRect& rcPaint, 
					qdTriggerChain const* ptrChain) const;
};
