// ChangeAutoRestart.h: interface for the ChangeAutoRestart class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANGEAUTORESTART_H__CCC9B917_025D_4E07_BB71_6FA552D2E55C__INCLUDED_)
#define AFX_CHANGEAUTORESTART_H__CCC9B917_025D_4E07_BB71_6FA552D2E55C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class qdTriggerLink;

/**
	������ ���� ����� "�����������"
 */
class ChangeAutoRestart  
{
public:
	ChangeAutoRestart(qdTriggerLink* ptrLink, bool autoRestart);
	~ChangeAutoRestart();
	bool operator()();
	static bool run(qdTriggerLink* ptrLink, bool autoRestart);
private:
	/// �����, ��� ������� ������ ����
	qdTriggerLink* ptrLink_;
	/// ���������, ������� ����� �����������
	bool autoRestart_;
};

#endif //!defined(AFX_CHANGEAUTORESTART_H__CCC9B917_025D_4E07_BB71_6FA552D2E55C__INCLUDED_)
