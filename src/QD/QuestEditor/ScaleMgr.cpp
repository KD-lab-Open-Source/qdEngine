#include "stdafx.h"
#include "resource.h"
#include "scalemgr.h"
#include "ScaleBar.h"


ScaleMgr::ScaleMgr(void):
m_pFrameWnd(NULL)
{
}

ScaleMgr::~ScaleMgr(void)
{
}

void ScaleMgr::SetScalable(IScalable* pscalable)
{
	m_ptrScaleBar->SetScalable(pscalable);
}
//обновляем информацию о масштабе
void ScaleMgr::UpdateScaleInfo()
{
	m_ptrScaleBar->UpdateScaleInfo();
}

bool ScaleMgr::Create(CFrameWnd* pParent, DWORD id)
{
	m_pFrameWnd = pParent;

	m_ptrScaleBar.reset(new CScaleBar);
	if (!m_ptrScaleBar->Create(m_pFrameWnd, id))
		return false;
	m_ptrScaleBar->EnableDocking(CBRS_ALIGN_ANY);
	pParent->DockControlBar(m_ptrScaleBar.get(), AFX_IDW_DOCKBAR_TOP);
	return true;
}

bool ScaleMgr::IsVisible() const{
	return IsWindowVisible(m_ptrScaleBar->m_hWnd);
}

void ScaleMgr::Show() const
{
	m_pFrameWnd->ShowControlBar(m_ptrScaleBar.get(), TRUE, FALSE); 
}

void ScaleMgr::Hide() const
{
	m_pFrameWnd->ShowControlBar(m_ptrScaleBar.get(), FALSE, FALSE); 
}

	//возвращает указатель на окно
CWnd* ScaleMgr::GetView()
{
	return m_ptrScaleBar.get();
}
