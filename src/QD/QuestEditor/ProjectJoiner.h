#pragma once
#include <vector>
class qdGameDispatcher;
class qdGameScene;


/// \brief ����� ������������ ��� ������� ���� ��������. ��� ������� ����� ������
///	�������� ����� join � ������� ��������� �������� �������
class ProjectJoiner
{
	typedef std::vector<qdGameScene*> GameSceneList;
public:
	ProjectJoiner(void);
	~ProjectJoiner(void);
	/// ������� �������. ��������� ���� ��������� ������� �������
	void join(qdGameDispatcher* ptrGD);
private:
	/// ���������� ��� ����� �������, � ������� ����� ���������
	CString const getProjFile() const;
	/// ��������� ������ �� �����
	bool initAnotherGameDisp(CString const& str,
								qdGameDispatcher* ptrAnotherDisp) const;
	/// ���������� ����� ���� ��� ��������
	bool selectSceneForJoin(qdGameDispatcher * ptrAnotherDisp, 
				GameSceneList& lstOut) ;
	/// ��������� ����� � m_ptrGameDisp
	bool merge(GameSceneList const& lst, qdGameDispatcher* ptrAnotherDisp);

	class scene_injector;
private:
	qdGameDispatcher *m_ptrGameDisp;
};
