#pragma once
class qdGameDispatcher;
class qdTriggerChain;

/**
	��� ���� ������� ������������ ������� ���������� �� �������� �������
 */
class ActiveToBeginForAllTriggers
{
	ActiveToBeginForAllTriggers(void);
	~ActiveToBeginForAllTriggers(void);
public:
	/**
		��������������� ������, ������� ������������ ���������� �������
	 */
	class MakeRootActive
	{
		const bool m_bActive;
	public:
		MakeRootActive(bool bActive);
		void operator()(qdTriggerChain *pc) const;
		static void DO(bool bActive, qdTriggerChain *pc);
	};
	static bool DO(qdGameDispatcher* ptrDisp);
};
