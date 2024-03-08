/********************************************************************
	created:	2003/07/30
	created:	30:7:2003   13:01
	filename: 	d:\Projects\Quest\QuestEditor\InsertColumns.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	InsertColumns
	file ext:	h
	powerd by:	�����
	
	purpose:	��������� �������  ����� ���������� � ��������� ���������
*********************************************************************/
#pragma once

class TEWnd;
class qdTriggerChain;

/**
	��������� �������  ����� ���������� � ��������� ���������
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
	/// ������� ���������
	int				m_iColumnsCount;
	/// ���� ���������
	TEWnd			*m_pWnd;
	/// �������
	qdTriggerChain	*m_pChain;
	/// �����, ���� ����� ��������� ��������
	CPoint			m_basePoint;
};
