#pragma once

class qdTriggerChain;
class qdTriggerElement;

/**
	Удаляет линк
 */
class DeleteLink
{
public:
	DeleteLink(qdTriggerChain* pchain, 
				qdTriggerElementPtr pOwner, 
				qdTriggerElementPtr pChild);
	~DeleteLink(void);
	bool operator()();
	/// Удаляет элемент
	static bool run(qdTriggerChain* pchain, 
					qdTriggerElementPtr pOwner, 
					qdTriggerElementPtr pChild);
private:
	/// Цепочка, в которой удаляется линк
	qdTriggerChain		*m_pChain;
	/// Элемет, владеющий связью
	qdTriggerElementPtr	m_pOwner;
	/// Подичиненный элемент
	qdTriggerElementPtr	m_pChild;
};
