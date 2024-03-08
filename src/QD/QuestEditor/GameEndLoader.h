/********************************************************************
	created:	2003/05/12
	created:	12:5:2003   14:58
	filename: 	d:\Projects\Quest\QuestEditor\GameEndLoader.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	GameEndLoader
	file ext:	h
	Powerd by:	�����
	
	purpose:	�������� � ������ ������ ��������� �������
*********************************************************************/
#pragma once
#include <list>

class qdGameEnd;
typedef std::list<qdGameEnd*> qdGameEndList;


/// ������ � ������ �������� ������. ����������,��� ��� �� ������������
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
	/// \brief �������, ������� ����������� ���� � ������ � �������� loadList
	/// ��� �������� ������
	static bool load(CTreeCtrl& tree, HTREEITEM hParent, qdGameEndList const& lst, 
				HTREEITEM hAfter);
	/// ��������� ������ 
	static bool loadList(CTreeCtrl& tree, HTREEITEM hParent, qdGameEndList const& lst);
};
