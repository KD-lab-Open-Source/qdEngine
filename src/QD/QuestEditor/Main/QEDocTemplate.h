#pragma once

#include "afxwin.h"

/**
	Является своего рода менеджером. Увязывает документ с оконным представлением
 */
class QEDocTemplate : public CSingleDocTemplate
{
	DECLARE_DYNAMIC(QEDocTemplate)
public:
	QEDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
	~QEDocTemplate(void);

	/// Создает главное окно и передает проекту необходимые указатели
	CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther);
	/// Пустой. Нужен таким, чтобы был правильным заголовок окна
	void SetDefaultTitle(CDocument* pDocument);
};
