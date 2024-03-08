#pragma once

#include "IObjectList.h"
#include <boost/scoped_ptr.hpp>

class CCoolDialogBar;
class ObjectListDlg;
class ObjectListImpl;

/// ������� ����������, ����������� ����� ���������� ������ � ��������� ����
class ObjectListMngr : public IObjectList
{
public:
	ObjectListMngr(void);
	~ObjectListMngr(void);

	/// ������� ��� ����.
	bool create(CFrameWnd* pParent, DWORD dwStyle = WS_CHILD |CBRS_FLYBY | 
								CBRS_RIGHT | CBRS_SIZE_DYNAMIC);

	/// ����� ��
	bool isVisible() const;
	virtual void show();
	virtual void hide();
	virtual bool showObjectInList(qdNamedObjectReference const* );
	virtual HITEM addObjectToList(qdNamedObjectReference const*, HITEM hItemRoot);
	virtual bool removeObjectFromList(qdNamedObjectReference const* );
	virtual bool addSubObjectFromList(HITEM hObjectItem, qdNamedObjectReference const* );
	virtual void clearList();
	virtual void setListCaption(CString const& str);
private:
	/// ���� ����������
	CFrameWnd* parentFrame_;
	/// ������, ����������� ����������� ����
	boost::scoped_ptr<CCoolDialogBar> dlgBar_;
	/// ������, ���������� ����������� �������� ��� ����������� ������
	boost::scoped_ptr<ObjectListDlg> dlg_;
	/// ����� ����������� ������ ������ �� ��������
	boost::scoped_ptr<ObjectListImpl> objectListImpl_;
};
