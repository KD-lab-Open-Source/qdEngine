#pragma once

class qdTriggerElement;
class qdTriggerChain;
class TEBaseWorkMode;

/**
	Добавляет связь между элементам
 */
class AddLink
{
public:
	AddLink(
		qdTriggerChain* pchain,
		qdTriggerElementPtr pOwner, 
		qdTriggerElementPtr pChild,
		CSize const& szChildOffset,
		CSize const& szOwnerOffset,
		int iLinkType);
	~AddLink(void);
	static bool run(
					qdTriggerChain* pchain,
					qdTriggerElementPtr pOwner, 
					qdTriggerElementPtr pChild,
					CSize const& szChildOffset,
					CSize const& szOwnerOffset,
					int iLinkType);
	bool operator()();
private:
	/// Цепочка, в которой все происходит
	qdTriggerChain		*m_pChain;
	/// Элемент, от которого идет связь
	qdTriggerElementPtr	m_pOwner;
	/// Элемент, к которому идет связь
	qdTriggerElementPtr	m_pChild;
	/// Сколько нужно отступить от центра, что получить место присоединения связи
	CSize				m_szChildOffset;
	/// Сколько нужно отступить от центра, что получить место присоединения связи
	CSize				m_szOwnerOffset;
	/// Тип связи
	int					m_iLinkType;
};
