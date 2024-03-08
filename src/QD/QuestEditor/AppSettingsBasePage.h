#pragma once

#include <slist>
#include "IAppSettingsChangeListener.h"

class AppSettingsBasePage : public CPropertyPage
{
	LPCTSTR m_dlgTemplateID;
	bool m_bModified;
	std::slist<IAppSettingsChangeListener*> m_listeners;
public:

	explicit AppSettingsBasePage(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	explicit AppSettingsBasePage(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~AppSettingsBasePage(void);
	BOOL Create(CWnd* parent);
	virtual BOOL OnApply();
	void setModified(bool bModified);
	bool getModified() const;

	bool AddListener(IAppSettingsChangeListener* plistener);
};
