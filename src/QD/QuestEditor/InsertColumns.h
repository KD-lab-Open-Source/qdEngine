/********************************************************************
	created:	2003/07/30
	created:	30:7:2003   13:01
	filename: 	d:\Projects\Quest\QuestEditor\InsertColumns.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	InsertColumns
	file ext:	h
	powerd by:	Илюха
	
	purpose:	Вставляет столбцы  между элементами в редакторе триггеров
*********************************************************************/
#pragma once

class TEWnd;
class qdTriggerChain;

/**
	Вставляет столбцы  между элементами в редакторе триггеров
 */
class InsertColumns
{
public:
	InsertColumns(size_t iColumnsCount, 
					TEWnd* pwnd,
					qdTriggerChain* pchain,
					CPoint const& basePoint);
	~InsertColumns(void);
	bool operator()();
	static bool run(size_t iColumnsCount, 
					TEWnd* pwnd,
					qdTriggerChain* pchain, CPoint const& basePoint);
private:
	/// Сколько вставляем
	int				m_iColumnsCount;
	/// Окно редактора
	TEWnd			*m_pWnd;
	/// Цепочка
	qdTriggerChain	*m_pChain;
	/// Точка, куда будем вставлять элементы
	CPoint			m_basePoint;
};
