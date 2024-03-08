#pragma once
class TEWnd;
class TEBaseWorkMode;
class qdTriggerChain;
class qdTriggerElement;

/**
	������� ������� �� ������� ���������
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
	/// ���� ���������
	TEWnd* m_pWnd;
	/// �������, �� ������� �������
	qdTriggerChain* m_pChain;
	/// ��������� �������
	qdTriggerElementPtr m_pEle;
};
