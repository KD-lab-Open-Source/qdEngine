#include "stdafx.h"
#include "qd_trigger_element.h"
#include "moveteelementpos.h"
#include "TEBaseWorkMode.h"
#include "tewnd.h"

MoveTEElementPos::MoveTEElementPos(TEBaseWorkMode* pMode, 
								   TEWnd* pwnd,
								   qdTriggerElementPtr pEle, 
								   qdTriggerChain* pchain,
								   CPoint const& offset):
  m_pMode(pMode)
, m_pWnd(pwnd)
, m_pEle(pEle)
, m_pChain(pchain)
, m_ptOffset(offset)
{
}

MoveTEElementPos::~MoveTEElementPos(void)
{
}

bool MoveTEElementPos::operator()(){
	return run(m_pMode, m_pWnd, m_pEle, m_pChain, m_ptOffset);
}

/**
	Берет рект элемента. Сдвигает его и затем переназначает.
	Пересчитывает номера ячеек.
	Пересчитывает прямоугольник цепочки и её рабочую область. Она измениться
	если перетянуть элемет ближе чем на 2 стобца/строки к краю
	Выравнивает видимую область, чтобы она отображала правильное место.
 */
bool MoveTEElementPos::run(TEBaseWorkMode* pMode, 
							TEWnd* pwnd,
							qdTriggerElementPtr pEle, 
							qdTriggerChain* pchain,
							CPoint const& offset)
{
	CRect br = pEle->bounding_rect();
	br.OffsetRect(offset);
	pEle->set_bounding_rect(br);

	pwnd->RemakeCellIdx(pEle, br.TopLeft());

//	qdTriggerChain * const ptrChain = pMode->getChain();
	pwnd->RemakeChainRect(pchain);
	pwnd->UpdateChainWorkArea(pchain);

	/*
	*	необходимо, чтобы после изменения размреа рабочей области
	*  положение окна относитльно триггера было то же самое
	*  
	*  для этого возьмем текущий видимый прямоугольник, сдвинем его
	*  на сколько был сдвинут триггер и сделаем EnsureVisible
	*/
	CRect const rcNewEleBound(pEle->bounding_rect());
	//если цепочка была сдвинута, то элемент тоже сдвинется
	if (rcNewEleBound.TopLeft() != br.TopLeft()) 
	{
		CRect rcClient;
		pwnd->GetClientRect(rcClient);
		pwnd->scr2log(rcClient);
		rcClient.OffsetRect(rcNewEleBound.TopLeft() - br.TopLeft());
		pwnd->EnsureVisibleRect(rcClient);
	}
	return true;
}