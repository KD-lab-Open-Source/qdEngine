#pragma once

#include "ObjInStateCond.h"
// MouseStatePhraseClickCondDlg dialog

/**
	���� �� �����
 */
class MouseStatePhraseClickCondDlg : public ObjInStateCond
{
	DECLARE_DYNAMIC(MouseStatePhraseClickCondDlg)

public:
	MouseStatePhraseClickCondDlg(DWORD id);   // standard constructor
	virtual ~MouseStatePhraseClickCondDlg();

protected:
	bool IsCondDataComplete();

	/// ��������� ������ ���������
	afx_msg virtual void OnCbnSelchangeComboObject();
	/// ������ ��������� � �������
	afx_msg virtual void OnCbnSelchangeComboStates();
	/// ������������� ������ ��������
	afx_msg virtual void OnCbnSelchangeComboScenes();

private:

};
