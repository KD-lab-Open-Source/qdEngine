#pragma once

class qdTriggerLink;

/**
	����������� ���� �� ��������� �����
 */
class MoveLinkEnd
{
public:
	/// ����� ��������� �������
	enum MLESelectEnd{
		MLE_CHILD_OFFSET,///< �� ��� � ������������ ��������
		MLE_OWNER_OFFSET///< �� ��� � ���������
	};
	MoveLinkEnd(qdTriggerLink* plink, CSize const& offset, MLESelectEnd eval);
	~MoveLinkEnd(void);
	bool operator()();
	static bool run(qdTriggerLink* plink, CSize const& offset, MLESelectEnd eval);
private:
	/// ���������� �����
	qdTriggerLink* m_pLink;
	/// �����
	CSize m_szOffset;
	/// ����� ��������� �������
	MLESelectEnd m_eMLESelectEnd;
};
