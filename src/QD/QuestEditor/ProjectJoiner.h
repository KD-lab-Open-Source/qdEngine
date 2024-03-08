#pragma once
#include <vector>
class qdGameDispatcher;
class qdGameScene;


/// \brief Класс предназначен для слияния двух проектов. Для слияния нужно только
///	вызывать метод join и передат диспетчер текущего проекта
class ProjectJoiner
{
	typedef std::vector<qdGameScene*> GameSceneList;
public:
	ProjectJoiner(void);
	~ProjectJoiner(void);
	/// Входная функция. Принимает гейм диспетчер текщего проекта
	void join(qdGameDispatcher* ptrGD);
private:
	/// Возвращает имя файла проекта, с которым буедм сливаться
	CString const getProjFile() const;
	/// Загружает проект из файла
	bool initAnotherGameDisp(CString const& str,
								qdGameDispatcher* ptrAnotherDisp) const;
	/// производим выбор сцен для переноса
	bool selectSceneForJoin(qdGameDispatcher * ptrAnotherDisp, 
				GameSceneList& lstOut) ;
	/// вставляем сцены в m_ptrGameDisp
	bool merge(GameSceneList const& lst, qdGameDispatcher* ptrAnotherDisp);

	class scene_injector;
private:
	qdGameDispatcher *m_ptrGameDisp;
};
