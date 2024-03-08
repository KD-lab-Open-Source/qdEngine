/********************************************************************
	created:	2003/07/29
	created:	29:7:2003   12:20
	filename: 	d:\Projects\Quest\QuestEditor\MakeLinkActive.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	MakeLinkActive
	file ext:	h
	author:		�����
	
	purpose:	������ ��������� ���� ��������
*********************************************************************/
#pragma once

class qdTriggerLink;
class qdTriggerElement;
class TEBaseWorkMode;

/**
	������ ��������� ���� ��������. �������� ���� ����� ����� �� ������
 */
class MakeLinkActive
{
public:
	MakeLinkActive(TEBaseWorkMode* pmode, 
					qdTriggerElementPtr const& pLinkChild,
					qdTriggerElementPtr const& pLinkOwner);
	~MakeLinkActive();
	bool operator()();
	static bool run(TEBaseWorkMode* pmode, 
					qdTriggerElementPtr const& pLinkChild,
					qdTriggerElementPtr const& pLinkOwner);
private:
	/// ��������
	qdTriggerElementPtr m_pLinkOwner;
	/// �����������
	qdTriggerElementPtr m_pLinkChild;
	/// ��, � ������� ��� �������� �����������
	TEBaseWorkMode* m_pMode;
};
