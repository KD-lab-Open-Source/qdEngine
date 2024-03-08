#pragma once
class qdGameDispatcher;
class qdTriggerChain;

/**
	ƒл€ всех цепочек переставл€ет признак активности на корневой элемент
 */
class ActiveToBeginForAllTriggers
{
	ActiveToBeginForAllTriggers(void);
	~ActiveToBeginForAllTriggers(void);
public:
	/**
		¬спомогательный классс, который обрабатывает конкретнуй цепочку
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
