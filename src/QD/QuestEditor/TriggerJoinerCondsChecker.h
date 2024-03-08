#pragma once
#include "ITriggerView.h"
#include "ITriggerJoinCondition.h"
#include "IViewMgr.h"

interface ITriggerJoinCondition;

class TriggerJoinerCondsChecker	:
	public ITriggerJoinCondition
{
public:
	TriggerJoinerCondsChecker(ITriggerView*, IViewMgr* );
	~TriggerJoinerCondsChecker(void);

	bool doCheck(qdTriggerChain const* pfrom, qdTriggerChain const* pto);
private:
	class is_in;
private:
	/// ������� ���������
	ITriggerView* m_ptrITriggerView;
	/// �������� ���������� ���� ����������
	IViewMgr* m_ptrViewMgr;
};
