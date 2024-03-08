// ChangeLinkType.h: interface for the ChangeLinkType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANGELINKTYPE_H__2CB04223_20B9_4F0C_86EF_C5D1F2796DB3__INCLUDED_)
#define AFX_CHANGELINKTYPE_H__2CB04223_20B9_4F0C_86EF_C5D1F2796DB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class qdTriggerLink;

/**
	������ ��� �����
 */
class ChangeLinkType  
{
public:
	ChangeLinkType(qdTriggerLink* ptrLinkPar2Child,
					qdTriggerLink* ptrLinkChild2Par,
					int type);
	~ChangeLinkType();

	bool operator()();
	/// ������������� ���
	static bool run(qdTriggerLink* ptrLinkPar2Child,
					qdTriggerLink* ptrLinkChild2Par,
					int type);
private:
	/// ���� �� �������� � ������(�� ��������������)
	qdTriggerLink* ptrLinkPar2Child_;
	/// ���� �� ������ � ��������
	qdTriggerLink* ptrLinkChild2Par_;
	/// ��������������� ��������� ����
	int type_;
};

#endif //!defined(AFX_CHANGELINKTYPE_H__2CB04223_20B9_4F0C_86EF_C5D1F2796DB3__INCLUDED_)
