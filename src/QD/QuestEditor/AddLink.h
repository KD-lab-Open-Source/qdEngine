#pragma once

class qdTriggerElement;
class qdTriggerChain;
class TEBaseWorkMode;

/**
	��������� ����� ����� ���������
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
	/// �������, � ������� ��� ����������
	qdTriggerChain		*m_pChain;
	/// �������, �� �������� ���� �����
	qdTriggerElementPtr	m_pOwner;
	/// �������, � �������� ���� �����
	qdTriggerElementPtr	m_pChild;
	/// ������� ����� ��������� �� ������, ��� �������� ����� ������������� �����
	CSize				m_szChildOffset;
	/// ������� ����� ��������� �� ������, ��� �������� ����� ������������� �����
	CSize				m_szOwnerOffset;
	/// ��� �����
	int					m_iLinkType;
};
