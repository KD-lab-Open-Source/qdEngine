/********************************************************************
	created:	2003/04/30
	created:	30:4:2003   12:18
	filename: 	d:\Projects\Quest\QuestEditor\ITriggerJoinCondition.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	ITriggerJoinCondition
	file ext:	h
	Powerd by:	�����
	
	purpose:	��������� �������� ����������� ����������� 2� ���������
*********************************************************************/
#pragma once

class qdTriggerChain;

/**
	��������� �������� ����������� ����������� 2� ���������
 */
interface ITriggerJoinCondition
{
	virtual bool doCheck(qdTriggerChain const *from, qdTriggerChain const* to) = 0;
};