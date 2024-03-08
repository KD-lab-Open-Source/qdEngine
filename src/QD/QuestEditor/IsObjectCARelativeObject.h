#pragma once

#include <list>
typedef std::list<qdNamedObject const*> NamedObjectList;

class qdGameDispatcher;
class qdGameObject;

/// ��������� �������� �� ������ ������������� ��� �����-���� ��
/**
	������ �������, � ������� �� ����� ���� QD_COORDS_ANM_RELATIVE_FLAG �
	object �������� ��������� ��������
 */ 
class IsObjectCARelativeObject
{
public:
	IsObjectCARelativeObject(void);
	~IsObjectCARelativeObject(void);
	/// ��������� �� ������ ���� � �������� ��� ������ ����� test
	static bool run(qdGameDispatcher const* gameDisp, 
		qdGameObject const* object, NamedObjectList& lst);
protected:
	///  \brief ����������� �� ���� ������������� �������� � ������� ������ �������
	/**
		@param scene - �����, �� ���������� ������� ���������
		@param object - ����������� ������
		@param lst	- ������, � ������� ������������ ������� ��, ������� 
					����� ���� QD_COORDS_ANM_RELATIVE_FLAG � ��������� ������ object
	*/
	static void test(qdGameScene const*scene, 
					qdGameObject const* object, 
					NamedObjectList& lst);
	/// ��������� ����� �� �������� ���� QD_COORDS_ANM_RELATIVE_FLAG
	static bool stateHasRelativeCA(qdGameObjectState const* state);
};
