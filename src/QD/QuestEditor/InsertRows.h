#pragma once

class TEWnd;
class qdTriggerChain;

/**
	��������� ������� ����� ��������� ������� ���������
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
	/// ���������� ����������� �����
	size_t			m_nRowsCount;
	/// ���� ���������
	TEWnd			*m_pWnd;
	/// �������
	qdTriggerChain	*m_pChain;
	/// �����, ������������ ����� ������� �����
	CPoint			m_basePoint;
};
