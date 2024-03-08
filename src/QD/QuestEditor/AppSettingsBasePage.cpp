#include "stdafx.h"
#include "AppSettingsBasePage.h"
#include <algorithm>


AppSettingsBasePage::AppSettingsBasePage(LPCTSTR lpszTemplateName, 
										 CWnd* pParentWnd )
: CPropertyPage(lpszTemplateName/*, pParentWnd*/)
, m_dlgTemplateID(lpszTemplateName)
{

}
AppSettingsBasePage::AppSettingsBasePage(UINT nIDTemplate, CWnd* pParentWnd)
: CPropertyPage(nIDTemplate/*, pParentWnd*/)
, m_dlgTemplateID(MAKEINTRESOURCE(nIDTemplate))
{

}
AppSettingsBasePage::~AppSettingsBasePage(void)
{

}
BOOL AppSettingsBasePage::Create(CWnd* parent)
{
	return __super::Create(m_dlgTemplateID, parent);
}

BOOL AppSettingsBasePage::OnApply()
{
	if (m_bModified)
	{
		std::for_each(m_listeners.begin(), m_listeners.end(), 
			std::mem_fun(IAppSettingsChangeListener::SettingsChanged));
	}
	return TRUE;
}

void AppSettingsBasePage::setModified(bool bModified)
{
	m_bModified = bModified;
}
bool AppSettingsBasePage::getModified() const
{
	return m_bModified;
}

bool AppSettingsBasePage::AddListener(IAppSettingsChangeListener* plistener)
{
	m_listeners.push_front(plistener);
	return true;
}
