#pragma once

#include "itriggerproflist.h"
#include <memory>

class CCoolDialogBar;
class TriggerDbgDlg;
interface ITriggerView;

/**
	��������� ��������� ������ ������� ����������.
	�������� ��� �����������, ����� �������� ������
 */
class TriggerProfList :
	public ITriggerProfList
{
public:
	TriggerProfList(void);
	~TriggerProfList(void);

	/// ������� ���� ������� � ������. ��������� �� ����� �����
	bool Create(CFrameWnd* pParent, ITriggerView* ptrITView, 
								DWORD dwStyle = WS_CHILD |CBRS_FLYBY | 
								CBRS_RIGHT | CBRS_SIZE_DYNAMIC);

	virtual bool Show(eShowHide e)const;
	virtual bool IsVisible()const;

	virtual bool Load();
	virtual bool Next()const;
	virtual bool Prev()const;
	virtual bool Activate();
private:
	/// ������ ���������� ������ � ��������
	std::auto_ptr<TriggerDbgDlg> m_ptrTrigDbgDlg;
	/// ������, ����������� ���������� ������ � ����� ����
	std::auto_ptr<CCoolDialogBar> m_ptrDlgBar;
	/// ������� ���� ����������
	CFrameWnd* m_pFrameWnd;
};
