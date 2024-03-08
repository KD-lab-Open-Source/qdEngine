/********************************************************************
	created:	2003/07/29
	created:	29:7:2003   19:35
	filename: 	d:\Projects\Quest\QuestEditor\AddElement.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	AddElement
	file ext:	h
	powerd by:	�����
	
	purpose:	��������� ������� � �������
*********************************************************************/
#pragma once

class TEWnd;
class qdTriggerElement;
class qdTriggerChain;
class qdNamedObject;

/**
	��������� ������� � �������
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
	/// ���� ���������
	TEWnd				*m_pWnd;
	/// �������, � ������� ���������
	qdTriggerChain		*m_pChain;
	/// �������, ������� ���������
	qdTriggerElementPtr	m_pElement;
	/// �����, � ������� ���������
	CPoint				m_basePoint;
	/// ������, ������� �������� � ��������
	qdNamedObject		*m_pElementObject;
};
