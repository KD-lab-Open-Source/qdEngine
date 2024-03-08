/********************************************************************
	created:	2003/07/29
	created:	29:7:2003   19:35
	filename: 	d:\Projects\Quest\QuestEditor\AddElement.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	AddElement
	file ext:	h
	powerd by:	Илюха
	
	purpose:	Добавляет элемент в триггер
*********************************************************************/
#pragma once

class TEWnd;
class qdTriggerElement;
class qdTriggerChain;
class qdNamedObject;

/**
	Добавляет элемент в триггер
 */
class AddElement
{
public:
	AddElement(TEWnd* pwnd, 
				CPoint const& basePoint,
				qdTriggerChain* pchain, 
				qdTriggerElementPtr pElement);
	~AddElement(void);
	bool operator()();
	static bool run(TEWnd* pwnd, 
		CPoint const& basePoint,
		qdTriggerChain* pchain, 
		qdTriggerElementPtr pElement,
		qdNamedObject*	ptrElementObject);
private:
	/// окно редактора
	TEWnd				*m_pWnd;
	/// Цепочка, в которую добавляем
	qdTriggerChain		*m_pChain;
	/// Элемент, который добавляем
	qdTriggerElementPtr	m_pElement;
	/// Точка, в которую добавляем
	CPoint				m_basePoint;
	/// Объект, который хранится в элементе
	qdNamedObject		*m_pElementObject;
};
