#pragma once
#include <afxext.h>

#include "IRectChangeListener.h"

/// ������������ ��������� �������������
/**
	��� ����� �� � MSDN CRectTracker. ������ ������ ��� ����, ����� ���� �����������
	����������� �� ������� �� ��������� �������
 */
class CustomRectTracker : public CRectTracker
{
public:
	CustomRectTracker(IRectChangeListener* rectChangeListener);
	~CustomRectTracker(void);

	/// �������� �����������
	virtual void OnChangedRect(const CRect& rectOld);
	/// ������ �� ������
	virtual void DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd);
private:
	/// ��������� �� ������� "��������� ������� ��������������"
	IRectChangeListener* rectChangeListener_;
};
