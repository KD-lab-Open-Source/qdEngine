/********************************************************************
	created:	2003/05/12
	created:	12:5:2003   14:58
	filename: 	d:\Projects\Quest\QuestEditor\GameEndLoader.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	GameEndLoader
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	Загрузка в дерево списка последних экранов
*********************************************************************/
#pragma once
#include <list>

class qdGameEnd;
typedef std::list<qdGameEnd*> qdGameEndList;


/// Грузит в дерево конечные экраны. Подозреваю,что уже не используется
class GameEndLoader
{
	class insert2tree
	{
	public:
		insert2tree(CTreeCtrl& tree, HTREEITEM hPar);
		void operator()(qdGameEnd* const pobj);
	private:
		CTreeCtrl& m_tree;
		HTREEITEM m_hParent;
	};
protected:
	GameEndLoader();
public:
	~GameEndLoader();
	/// \brief главная, создает необходимый узел в дереве и вызывает loadList
	/// для загрузки списка
	static bool load(CTreeCtrl& tree, HTREEITEM hParent, qdGameEndList const& lst, 
				HTREEITEM hAfter);
	/// загружает список 
	static bool loadList(CTreeCtrl& tree, HTREEITEM hParent, qdGameEndList const& lst);
};
