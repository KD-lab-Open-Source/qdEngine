#pragma once
class TEWnd;
class TEBaseWorkMode;
class qdTriggerChain;
class qdTriggerElement;

/**
	Удаляет элемент из цепочки триггеров
 */
class DeleteElement
{
public:
	DeleteElement(TEWnd* pwnd, 
				qdTriggerChain* pchain,
				qdTriggerElementPtr pele);
	bool operator()();
	~DeleteElement(void);
	static bool run(TEWnd* pwnd, 
		qdTriggerChain* pchain,
		qdTriggerElementPtr pele);
private:
	/// Окно редактора
	TEWnd* m_pWnd;
	/// Цепочка, из которой удаляем
	qdTriggerChain* m_pChain;
	/// Удаляемый элемент
	qdTriggerElementPtr m_pEle;
};
