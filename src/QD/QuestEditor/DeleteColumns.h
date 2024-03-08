/********************************************************************
	created:	2003/07/30
	created:	30:7:2003   13:00
	filename: 	d:\Projects\Quest\QuestEditor\DeleteColumns.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	DeleteColumns
	file ext:	h
	powerd by:	�����
	
	purpose:	������� ������� ����� ���������� � ��������� ���������
*********************************************************************/
#pragma once
class TEWnd;
class qdTriggerChain;

/**
	������� �������.
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
	/// ��������� �������� ��������
	size_t			m_nColumnsCount;
	/// ���� ��������� ���������
	TEWnd			*m_pWnd;
	/// �������, � ������� ���������� ��������
	qdTriggerChain	*m_pChain;
	/// �������, ������� ��� � ������ ����� �������
	CPoint			m_basePoint;
};
