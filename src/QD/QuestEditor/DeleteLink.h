#pragma once

class qdTriggerChain;
class qdTriggerElement;

/**
	������� ����
 */
class DeleteLink
{
public:
	DeleteLink(qdTriggerChain* pchain, 
				qdTriggerElementPtr pOwner, 
				qdTriggerElementPtr pChild);
	~DeleteLink(void);
	bool operator()();
	/// ������� �������
	static bool run(qdTriggerChain* pchain, 
					qdTriggerElementPtr pOwner, 
					qdTriggerElementPtr pChild);
private:
	/// �������, � ������� ��������� ����
	qdTriggerChain		*m_pChain;
	/// ������, ��������� ������
	qdTriggerElementPtr	m_pOwner;
	/// ������������ �������
	qdTriggerElementPtr	m_pChild;
};
