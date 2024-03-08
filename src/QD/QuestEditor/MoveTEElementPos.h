/********************************************************************
	created:	2003/07/29
	created:	29:7:2003   12:36
	filename: 	d:\Projects\Quest\QuestEditor\MoveTEElementPos.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	MoveTEElementPos
	file ext:	h
	powerd by:	�����
	
	purpose:	����� ��������� �������� �� ������ � ������������ ������� ���������
*********************************************************************/
#pragma once

class TEBaseWorkMode;
class qdTriggerElement;
class qdTriggerChain;
class TEWnd;

/**
	����� ��������� �������� �� ������ � ������������ ������� ���������
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
	/// ��
	TEBaseWorkMode		*m_pMode;
	/// ���������� ������� 
	qdTriggerElementPtr	m_pEle;
	/// �������, ��������� ���������
	qdTriggerChain		*m_pChain;
	/// ���� ���������
	TEWnd				*m_pWnd;
	/// �� ������� �������� �������
	CPoint				m_ptOffset;
};
