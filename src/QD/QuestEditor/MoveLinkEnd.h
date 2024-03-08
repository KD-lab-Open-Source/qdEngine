#pragma once

class qdTriggerLink;

/**
	Передвигает одно из окончаний связи
 */
class MoveLinkEnd
{
public:
	/// Какое окончание двигаем
	enum MLESelectEnd{
		MLE_CHILD_OFFSET,///< То что у подчиненного элемента
		MLE_OWNER_OFFSET///< То что у владельца
	};
	MoveLinkEnd(qdTriggerLink* plink, CSize const& offset, MLESelectEnd eval);
	~MoveLinkEnd(void);
	bool operator()();
	static bool run(qdTriggerLink* plink, CSize const& offset, MLESelectEnd eval);
private:
	/// Подопытная связь
	qdTriggerLink* m_pLink;
	/// Сдвиг
	CSize m_szOffset;
	/// Какое окончание двигаем
	MLESelectEnd m_eMLESelectEnd;
};
