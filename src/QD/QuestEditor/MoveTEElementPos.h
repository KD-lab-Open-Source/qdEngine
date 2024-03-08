/********************************************************************
	created:	2003/07/29
	created:	29:7:2003   12:36
	filename: 	d:\Projects\Quest\QuestEditor\MoveTEElementPos.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	MoveTEElementPos
	file ext:	h
	powerd by:	Илюха
	
	purpose:	Смена положения элемента на экране и выравнивание области просмотра
*********************************************************************/
#pragma once

class TEBaseWorkMode;
class qdTriggerElement;
class qdTriggerChain;
class TEWnd;

/**
	Смена положения элемента на экране и выравнивание области просмотра
 */
class MoveTEElementPos
{
public:
	MoveTEElementPos(TEBaseWorkMode* pMode,
		TEWnd* pwnd, 
		qdTriggerElementPtr pEle, 
		qdTriggerChain* pchain,
		CPoint const& offset);
	~MoveTEElementPos(void);

	bool operator()();
	static bool run(TEBaseWorkMode* pMode, 
					TEWnd* pwnd,
					qdTriggerElementPtr pEle, 
					qdTriggerChain* pchain,
					CPoint const& offset);
private:
	/// РР
	TEBaseWorkMode		*m_pMode;
	/// Сдвигаемый элемент 
	qdTriggerElementPtr	m_pEle;
	/// Цепочка, владеющая элементом
	qdTriggerChain		*m_pChain;
	/// Окно редактора
	TEWnd				*m_pWnd;
	/// На сколько сдвигаем элемент
	CPoint				m_ptOffset;
};
