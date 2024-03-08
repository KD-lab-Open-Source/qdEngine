/********************************************************************
	created:	2003/07/30
	created:	30:7:2003   13:00
	filename: 	d:\Projects\Quest\QuestEditor\DeleteColumns.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	DeleteColumns
	file ext:	h
	powerd by:	Илюха
	
	purpose:	Удаляет столбцы между элементами в редаткоре триггеров
*********************************************************************/
#pragma once
class TEWnd;
class qdTriggerChain;

/**
	Удаляет столбцы.
 */
class DeleteColumns
{
public:
	DeleteColumns(size_t nColumnsCount, 
				TEWnd* pwnd, 
				qdTriggerChain* pchain, 
				CPoint const& basePoint);
	~DeleteColumns(void);
	bool operator()();
	static bool run(size_t nColumnsCount, 
		TEWnd* pwnd, 
		qdTriggerChain* pchain, 
		CPoint const& basePoint);
private:
	/// Количство удалемых столбцов
	size_t			m_nColumnsCount;
	/// Окно редактора триггеров
	TEWnd			*m_pWnd;
	/// Цепочка, в которой происходит удаление
	qdTriggerChain	*m_pChain;
	/// Столбцы, лежащие под и справа будут удалены
	CPoint			m_basePoint;
};
