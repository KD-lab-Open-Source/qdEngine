#pragma once

class TEWnd;
class qdTriggerChain;

/**
	Вставляет строчки между элементов цепочки триггеров
 */
class InsertRows
{
public:
	InsertRows(size_t nRowsCount, 
		TEWnd* pwnd, 
		qdTriggerChain* pchain, 
		CPoint const& basePoint);
	~InsertRows(void);
	bool operator()();
	static bool run(size_t nRowsCount, 
		TEWnd* pwnd,
		qdTriggerChain* pchain, CPoint const& basePoint);
private:
	/// Количество вставляемых строк
	size_t			m_nRowsCount;
	/// Окно редактора
	TEWnd			*m_pWnd;
	/// Цепочка
	qdTriggerChain	*m_pChain;
	/// Точка, определяющая место вставки строк
	CPoint			m_basePoint;
};
