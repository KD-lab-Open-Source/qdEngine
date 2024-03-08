/********************************************************************
	created:	2003/07/30
	created:	30:7:2003   17:24
	filename: 	d:\Projects\Quest\QuestEditor\DeleteRows.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	DeleteRows
	file ext:	h
	powerd by:	�����
	
	purpose:	�������� ����� ����� ���������� � ��������� ���������
*********************************************************************/
#pragma once
class qdTriggerChain;
class TEWnd;
/**
	������� ������ ����� ���������� � ��������� ���������
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
	/// ���������� ��������� �����
	size_t			m_nRowsCount;
	/// ���� ��������� ���������
	TEWnd			*m_pWnd;
	/// �������, � ������� �������
	qdTriggerChain	*m_pChain;
	/// ������ ��� ���� ������ � ���� � ����� �������
	CPoint			m_basePoint;
};
