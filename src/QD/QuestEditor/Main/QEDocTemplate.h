#pragma once

#include "afxwin.h"

/**
	�������� ������ ���� ����������. ��������� �������� � ������� ��������������
 */
class QEDocTemplate : public CSingleDocTemplate
{
	DECLARE_DYNAMIC(QEDocTemplate)
public:
	QEDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
	~QEDocTemplate(void);

	/// ������� ������� ���� � �������� ������� ����������� ���������
	CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther);
	/// ������. ����� �����, ����� ��� ���������� ��������� ����
	void SetDefaultTitle(CDocument* pDocument);
};
