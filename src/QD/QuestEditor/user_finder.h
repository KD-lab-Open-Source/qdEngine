/********************************************************************
created:	2002/07/01
created:	1:7:2002   13:14
filename: 	D:\Projects\Quest\QuestEditor\user_finder.h
file path:	D:\Projects\Quest\QuestEditor
file base:	user_finder
file ext:	h
author:		�����

  purpose:	���� �������, ������� ���������� �������
  ��� ��������
*********************************************************************/
#ifndef __USER_FINDER_HEADER__
#define __USER_FINDER_HEADER__

#include <list>
class qdGameObjectStateStatic;
class qdGameObjectStateWalk;
class qdGameObjectState;
class qdGameObject;
class qdGameScene;


typedef std::list<qdGameObjectState*> GameObjectStateList_t;

/**
	� ������ �������� ������ ����� ���������, ���������� ������, ��������� ������.
	��� ��������� ��������� ���������� _NamesEqual. ����� ������� ����� ��� 
	�������� ��������� ���������� _ObjSelector
 */
template<class _ObjSelector, ///< ������� ��� ������ ������ ��������
		class _NamesEqual///< ������� ��� ��������� �����
		>
bool find_in_obj_list(const qdGameObjectList& gol, LPCTSTR lpszName, 
					  GameObjectStateList_t* ptrList,
					  _ObjSelector os, _NamesEqual ne)
{
	qdGameObjectList::const_iterator _itr = gol.begin(), _end = gol.end();
	
	bool bRes = false;
	while((_itr = std::find_if(_itr, _end, _ObjSelector())) != _end)
	{
		const qdGameObjectStateVector& vec = 
			static_cast<qdGameObjectAnimated*>(*_itr)->state_vector();
		qdGameObjectStateVector::const_iterator res =  vec.begin(),
												end = vec.end();
		while((res = std::find_if(res, end, std::bind2nd(ne, lpszName)))!= end)
		{
			ptrList->push_back(*res);
			bRes = true;
			++res;
		}
		++_itr;
	}
	return bRes;
}

/**
	� ����� ������ ����� ���������, ���������� ������, ��������� ������.
	��� ��������� ��������� ���������� _NamesEqual. ����� ������� ����� ��� 
	�������� ��������� ���������� _ObjSelector
 */

template<class _ObjSelector, ///< ������� ��� ������ ������ ��������
		class _NamesEqual///< ������� ��� ��������� �����
		>
class find_users_in_scene{
	/// ��� �������� �������
	LPCTSTR lpszName;
	/// �������� ������ ���������, � ������� ��� ������ ������� ������
	GameObjectStateList_t* m_ptrList;
public:
	find_users_in_scene(LPCTSTR lpszNm, GameObjectStateList_t* ptrList)
		: lpszName(lpszNm), m_ptrList(ptrList){}
	
	bool operator()(qdGameScene* ptrScene){
		return find_in_obj_list(ptrScene->object_list(), lpszName, 
									m_ptrList,_ObjSelector(), _NamesEqual());
	}
};

/**
	�� ���� ������� ������ ����� ���������, ���������� ������, ��������� ������.
	��� ��������� ��������� ���������� _NamesEqual. ����� ������� ����� ��� 
	�������� ��������� ���������� _ObjSelector
 */
template<class _NamesEqual,///< ������� ��� ��������� �����
		class _ObjSelector///< ������� ��� ������ ������ ��������
		>
class user_finder {
public:
	typedef _ObjSelector os;
	/// ��� �������� �������
	LPCTSTR lpszName;
	/// �������� ������ ���������, � ������� ��� ������ ������� ������
	GameObjectStateList_t* m_ptrList;

	/// ������ � ������� ��������� ������������ ������� ������
	CString strUsers;
		
		user_finder(LPCTSTR lpszNm, GameObjectStateList_t* ptrList)
			:lpszName(lpszNm), m_ptrList(ptrList){};
		
		void operator()(qdGameDispatcher* ptrDisp) {
			const qdGameSceneList& gol = ptrDisp->scene_list();
			qdGameSceneList::const_iterator _itr = gol.begin(), _end = gol.end();
			while((_itr = std::find_if(_itr, _end,
						find_users_in_scene<os, _NamesEqual>
								(lpszName, m_ptrList)))!= _end)
			{
				if(!strUsers.IsEmpty())
					strUsers += _T(",\n");
				
				strUsers += (*_itr)->name();
				++_itr;
			}
		}
};

/// ������� ��� �������, ������� ���������� ��������� �������
class gait_user_finder {
public:
	/// ��������� ��������� �� ��� ������� � ��������� � ��������� ���
	struct animation_set_names_eqal: 
		std::binary_function<qdGameObjectState*, LPCTSTR, bool>{
		bool operator()(qdGameObjectState* ptrState, LPCTSTR lpszName) const{
			if (ptrState->state_type() != qdGameObjectState::STATE_STATIC)
				return false;
			
			qdGameObjectStateWalk* ptrStateWalk = static_cast<qdGameObjectStateWalk*>(ptrState);
			return (ptrStateWalk->animation_set_info()->name()&&
				strcmp(ptrStateWalk->animation_set_info()->name(), lpszName) == 0);
		}
	};
	
	/// ��������� �������� �� ������ ����������
	struct is_moving{
		bool operator()(const qdGameObject* ptrObj){
			return (ptrObj->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ);
		}
	};

	/// ��������� ������ ����������� �����
	user_finder<animation_set_names_eqal, is_moving> m_uf;

	gait_user_finder(LPCTSTR lpszNm, GameObjectStateList_t* ptrList):m_uf(lpszNm, ptrList){};
	
	/// ���������� ������, � ������� ���� ��� ���������� ������� �������
	CString get_strUsers(){
		return m_uf.strUsers;
	}
	/// ���������� �����
	void operator()(qdGameDispatcher* ptrDisp){
		m_uf(ptrDisp);
	}
};

/// ������� ��� �������, ������� ���������� ��������� ��������
class anim_user_finder {
public:
	/// ���������� ��� �������� � ��������� � ��������� ������
	struct animation_names_eqal: std::binary_function<qdGameObjectState*, LPCTSTR, bool>{

		bool operator()(qdGameObjectState* ptrState, LPCTSTR lpszName) const{
			if (ptrState->state_type() != qdGameObjectState::STATE_STATIC)
				return false;

			qdGameObjectStateStatic* ptrSS = static_cast<qdGameObjectStateStatic*>(ptrState);
			return (ptrSS->animation_info()->animation_name()
				&&(strcmp(ptrSS->animation_info()->animation_name(), lpszName) == 0));
		}
	};
	
	/// ������� ��������� �������� �� ������ ������������� �������� ��� ����������
	struct is_animated{
		bool operator()(const qdGameObject* ptrObj){
			return (ptrObj->named_object_type() == QD_NAMED_OBJECT_ANIMATED_OBJ
				||ptrObj->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ);
		}
	};
	/// ��������� ������ ����������� �����
	user_finder<animation_names_eqal, is_animated> m_uf;
	
	anim_user_finder(LPCTSTR lpszNm, GameObjectStateList_t* ptrList):m_uf(lpszNm, ptrList){};

	/// ���������� ������, � ������� ���� ��� ���������� ������� ��������
	CString get_strUsers(){
		return m_uf.strUsers;
	}
	/// ���������� �����
	void operator()(qdGameDispatcher* ptrDisp){
		m_uf(ptrDisp);
	}
};

/**
	������� ��� �������, ������� ���������� ��������� ����
 */
class sound_user_finder {
public:
	/// ���������� ��� ����� � ��������� � ��������� ������
	struct sound_names_eqal: std::binary_function<qdGameObjectState*, LPCTSTR, bool>{
		bool operator()(qdGameObjectState* ptrState, LPCTSTR lpszName) const{
			return (ptrState->sound_name()
				&&(strcmp(ptrState->sound_name(), lpszName) == 0));
		}
	};
	
	typedef anim_user_finder::is_animated is_animated;

	/// ��������� ������ ����������� �����
	user_finder<sound_names_eqal, is_animated> m_uf;
	
	sound_user_finder(LPCTSTR lpszNm, GameObjectStateList_t* ptrList)
		:m_uf(lpszNm, ptrList){};
	/// ���������� ������, � ������� ���� ��� ���������� ������� ����
	CString get_strUsers(){
		return m_uf.strUsers;
	}
	/// ���������� �����
	void operator()(qdGameDispatcher* ptrDisp){
		m_uf(ptrDisp);
	}
};
#endif//__USER_FINDER_HEADER__