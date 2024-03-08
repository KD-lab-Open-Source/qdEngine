#include "stdafx.h"
#include "resource.h"
#include "triggerjoinercondschecker.h"
#include "qd_trigger_chain.h"

/**
	��������� ���� �� ��� �������, ����������� � ��������, ������� � 
	��������� �������
 */
class TriggerJoinerCondsChecker::is_in
{
public:
	is_in(qdTriggerChain const* pchain): m_pTriggerChain(pchain)
	{}
	bool operator()(qdTriggerElementConstPtr pele) const
	{
		ASSERT(pele->object());
		return m_pTriggerChain->is_element_in_list(pele->object());
	}
private:
	/// ����������� �������
	qdTriggerChain const* m_pTriggerChain;
};


TriggerJoinerCondsChecker::TriggerJoinerCondsChecker(ITriggerView* ptv, IViewMgr* pvm)
: m_ptrITriggerView(ptv)
, m_ptrViewMgr(pvm)
{
}

TriggerJoinerCondsChecker::~TriggerJoinerCondsChecker()
{
}

/**
	���������� ���� �� ������� ��������� ��� �������� �� ������� pfrom.
	���� ����, �� ��������� ������. ���������� � ������������ ��� ������
 */
bool TriggerJoinerCondsChecker::doCheck(qdTriggerChain const* pfrom, 
										qdTriggerChain const* pto)
{
	qdTriggerElementList const& lst = pfrom->elements_list();
	qdTriggerElementList::const_iterator itr = 
		std::find_if(lst.begin(), lst.end(), is_in(pto));
	if (itr != lst.end())
	{
		if (AfxMessageBox(IDS_STRING152, MB_YESNO) == IDYES)
		{
			if (m_ptrITriggerView->IsVisible()||m_ptrViewMgr->ShowTriggerEditor())
			{
				if (m_ptrITriggerView->ShowedTrigger() != pfrom)
					m_ptrITriggerView->ShowTriggers(
							const_cast<qdTriggerChain**>(&pfrom), 1);
				m_ptrITriggerView->EnsureVisible(&*itr, 1);
			}
		}
		return false;
	}
	return true;
}
