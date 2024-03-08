#include "stdafx.h"
#include "QuestEditor.h"
#include "triggerproflist.h"
#include "CoolDialogBar.h"
#include "TriggerDbgDlg.h"

TriggerProfList::TriggerProfList(void):
m_pFrameWnd(NULL)
{
}

TriggerProfList::~TriggerProfList(void)
{
}

bool TriggerProfList::Create(CFrameWnd* pParent, ITriggerView* ptrITView, DWORD dwStyle)
{
	m_pFrameWnd = pParent;

	m_ptrTrigDbgDlg.reset(new TriggerDbgDlg);
	if (!m_ptrTrigDbgDlg.get())
		return false;

	if (!m_ptrTrigDbgDlg->SetTriggerView(ptrITView))
		return true;

	m_ptrDlgBar.reset(new CCoolDialogBar);
	if (!m_ptrDlgBar.get())
		return false;
	
	if (!m_ptrDlgBar->Create(pParent, m_ptrTrigDbgDlg.get(), 
							dwStyle, TriggerDbgDlg::IDD))
		return false;

	m_ptrDlgBar->EnableDocking(CBRS_ALIGN_ANY);
	pParent->DockControlBar(m_ptrDlgBar.get(), AFX_IDW_DOCKBAR_TOP);
	return true;
}

/**
	Показывает/скрывает панель. Уведомляет диалог о том, что его показали/скрыли
 */
bool TriggerProfList::Show(eShowHide e) const 
{
	if (e == TPL_SHOW)
	{
		m_pFrameWnd->ShowControlBar(m_ptrDlgBar.get(), TRUE, FALSE); 
		m_ptrTrigDbgDlg->OnShow();
	}
	else 
	{
		m_pFrameWnd->ShowControlBar(m_ptrDlgBar.get(), FALSE, FALSE); 
		m_ptrTrigDbgDlg->OnHide();
	}
	return true;
}

/// Видима ли панель
bool TriggerProfList::IsVisible() const {
	return IsWindowVisible(m_ptrDlgBar->m_hWnd);
}

/// Передает управление в TriggerDbgDlg::Load
bool TriggerProfList::Load()
{
	return m_ptrTrigDbgDlg->Load();
}

/// Передает управление в TriggerDbgDlg::Next
bool TriggerProfList::Next() const
{
	return m_ptrTrigDbgDlg->Next();
}

/// Передает управление в TriggerDbgDlg::Prev
bool TriggerProfList::Prev() const
{
	return m_ptrTrigDbgDlg->Prev();
}

/// Передает управление в TriggerDbgDlg::Activate
bool TriggerProfList::Activate()
{
	if (m_ptrTrigDbgDlg->Activate())
		return Load();
	return false;
}
