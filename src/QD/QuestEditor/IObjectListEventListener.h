#pragma once

class qdNamedObjectReference;
///  \brief ��������� ������������� ��� ���������� ������, ����������� �������� 
/// ����������� � �������� ������������ � ������ ��������
class IObjectListEventListener
{
	IObjectListEventListener();
public:
	/// ����������� � ������� �����
	virtual void onObjectDblClick(qdNamedObjectReference const* obj)
};
