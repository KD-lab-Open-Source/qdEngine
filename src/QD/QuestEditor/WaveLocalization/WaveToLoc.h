#pragma once
#include "qd_fwd.h"

class CProgressDlg;

/// ������, ���������� ����-�� � ������-�� �����. �� �����, ��� ���� ��� ���� �������
class WaveToLoc
{
public:
	WaveToLoc(void);
	~WaveToLoc(void);
	bool run(CWnd* parent, qdGameDispatcher& gameDisp, bool copyToResource);
protected:
private:
	CString baseDir_;
};

