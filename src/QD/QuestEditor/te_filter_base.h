/********************************************************************
	created:	2002/09/17
	created:	17:9:2002   18:18
	filename: 	D:\Projects\Quest\QuestEditor\te_filter_base.h
	file path:	D:\Projects\Quest\QuestEditor
	file base:	te_filter_base
	file ext:	h
	Powerd by:	�����
	
	purpose:	������� ����� ��� ��������, ��������������� �����������
				�������� � �������� ���������.
*********************************************************************/

#ifndef __TE_FILTER_BASE__
#define __TE_FILTER_BASE__

#include "trigger_element_fwd.h"
//class qdTriggerElementConstPtr;
//! ������� ����� ��� �������� ��������� ������� ���������
class te_filter_base{
public:
	virtual bool operator()(qdTriggerElementConstPtr const& pe) const = 0;
};

#endif//__TE_FILTER_BASE__
