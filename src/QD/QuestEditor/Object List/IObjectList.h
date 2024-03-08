
#ifndef __OBJECT_LIST_HEADER
#define __OBJECT_LIST_HEADER

class qdNamedObjectReference;

DECLARE_HANDLE(HITEM);
HITEM const OL_ROOT = reinterpret_cast<HITEM>(0x0000ffff);

/// ��������� ������ ������������� ������ ��������.
struct IObjectList
{
	/// �������� ������
	virtual void show() = 0;
	/// ������ ������
	virtual void hide() = 0;
	/// �������� ������ � ������
	virtual bool showObjectInList(qdNamedObjectReference const* ) = 0;
	/// �������� ������ � ������.
	/**
		@param hRootItem - ����, ��� ������� ����� �������� ������
	 */
	virtual HITEM addObjectToList(qdNamedObjectReference const*, 
									HITEM hRootItem = OL_ROOT) = 0;
	/// ������� �� ������
	virtual bool removeObjectFromList(qdNamedObjectReference const* ) = 0;
	/// �������� ������
	virtual void clearList() = 0;
	/// ��������� ��������� ��� ������ ��������
	virtual void setListCaption(CString const& str) = 0;
};
#endif//__OBJECT_LIST_HEADER