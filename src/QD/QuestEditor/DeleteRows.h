/********************************************************************
	created:	2003/07/30
	created:	30:7:2003   17:24
	filename: 	d:\Projects\Quest\QuestEditor\DeleteRows.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	DeleteRows
	file ext:	h
	powerd by:	Илюха
	
	purpose:	удаление строк между элементами в редакторе триггеров
*********************************************************************/
#pragma once
class qdTriggerChain;
class TEWnd;
/**
	Удаляет строки между элементами в редакторе триггеров
 */
class DeleteRows
{
public:
	DeleteRows(size_t nRowsCount, 
		TEWnd* pwnd, 
		qdTriggerChain* pchain, 
		CPoint const& basePoint);
	~DeleteRows(void);
	bool operator()();
	static bool run(size_t nRowsCount, 
		TEWnd* pwnd, 
		qdTriggerChain* pchain, 
		CPoint const& basePoint);
private:
	/// Количество удаляемых строк
	size_t			m_nRowsCount;
	/// Окно редактора триггеров
	TEWnd			*m_pWnd;
	/// Цепочка, в которой удаляем
	qdTriggerChain	*m_pChain;
	/// Строки под этой точкой и ниже её будут удалены
	CPoint			m_basePoint;
};
