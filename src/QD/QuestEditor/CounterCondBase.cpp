#include "stdafx.h"
#include "resource.h"
#include "countercondbase.h"
#include "qd_counter.h"

IMPLEMENT_DYNAMIC(CounterCondBase, CondDlgBase);

VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);
CounterCondBase::AddCounterToCombo::AddCounterToCombo(CCJFlatComboBox& box)
:m_wndCombo(box)
{
}

void CounterCondBase::AddCounterToCombo::operator()(qdCounter* pc)
{
	int const iItem = m_wndCombo.AddString(pc->name());
	if (iItem != CB_ERR)
		m_wndCombo.SetItemDataPtr(iItem, pc);
}

CounterCondBase::CounterCondBase(UINT nIDTemplate, DWORD condID, UINT counterComboId)
: CondDlgBase(condID, condID)
, m_uiCounterComboId(counterComboId)
, m_ptrGameDisp(qd_get_game_dispatcher())
{
}

CounterCondBase::~CounterCondBase(void)
{
}

BEGIN_MESSAGE_MAP(CounterCondBase, CondDlgBase)
END_MESSAGE_MAP();

void CounterCondBase::DoDataExchange(CDataExchange* pDX)
{
	CondDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, m_uiCounterComboId, m_wndCounters);
}

BOOL CounterCondBase::OnInitDialog()
{
	BOOL res = CondDlgBase::OnInitDialog();
	res = loadCounters(m_wndCounters);
	onSelChangeInCounterCombo();
	return res;
}
//загружаем список счетчиков
bool CounterCondBase::loadCounters(CCJFlatComboBox& box)
{
	ASSERT(m_ptrGameDisp);
	qdCounterList const& lst = m_ptrGameDisp->counter_list();
	std::for_each(lst.begin(), lst.end(), AddCounterToCombo(box));
	AdjustListWidth(&box);
	box.SetCurSel(0);
	return true;
}

void CounterCondBase::onSelChangeInCounterCombo()
{
	m_qdCondition.put_object(0, getCounterFromCombo(m_wndCounters));
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

qdCounter* CounterCondBase::getCounterFromCombo(CCJFlatComboBox& box)
{
	int const curSel = box.GetCurSel();
	if (curSel == CB_ERR)
		return NULL;
	return reinterpret_cast<qdCounter*>(m_wndCounters.GetItemDataPtr(curSel));
}

LRESULT CounterCondBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_COMMAND
		&&HIWORD(wParam) == CBN_SELCHANGE
		&&LOWORD(wParam) == m_uiCounterComboId)
	{
		onSelChangeInCounterCombo();
	}
	return CondDlgBase::WindowProc(message, wParam, lParam);
}
