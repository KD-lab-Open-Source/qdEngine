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
	����� ���� ��������. �������� ��� � ����� �������������.
	������������� ������ �����.
	������������� ������������� ������� � � ������� �������. ��� ����������
	���� ���������� ������ ����� ��� �� 2 ������/������ � ����
	����������� ������� �������, ����� ��� ���������� ���������� �����.
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
	*	����������, ����� ����� ��������� ������� ������� �������
	*  ��������� ���� ����������� �������� ���� �� �� �����
	*  
	*  ��� ����� ������� ������� ������� �������������, ������� ���
	*  �� ������� ��� ������� ������� � ������� EnsureVisible
	*/
	CRect const rcNewEleBound(pEle->bounding_rect());
	//���� ������� ���� ��������, �� ������� ���� ���������
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