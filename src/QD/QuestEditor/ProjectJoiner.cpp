#include "stdafx.h"
#include "resource.h"

#include <memory>
#include <boost/function_output_iterator.hpp>

#include "projectjoiner.h"
#include "name_op.h"
#include "disk_op.h"
#include "CurDirSafer.h"
#include "NamedObjectSelectorDlg.h"
#include "INamedObjListTo.h"

#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"

#include "ptree_api.h"

namespace
{
	/// Текущий каталог для операции открытия проекта для слияния
	inline CurDirSafer& GetCurDirSafer()
	{
		static CurDirSafer safer;
		return safer;
	}

	///	Вставляет сцену в CListCtrl 
	class insert2lst
	{
		CListCtrl& m_lst;
	public:
		insert2lst(CListCtrl& lst):m_lst(lst){}
		void operator()(qdGameScene* pscene)
		{
			int item = m_lst.InsertItem(0, pscene->name());
			if (item != -1)
				m_lst.SetItemData(item, reinterpret_cast<DWORD_PTR>(pscene));
		}
	};

}
///	Проводит слияние проектов. Сцены одного проекта перекидываются в другой
class ProjectJoiner::scene_injector 
{
public:
	scene_injector(qdGameDispatcher* const ptrDest,
		qdGameDispatcher* const ptrSrc,
		GameSceneList& addedScenes): 
	  m_ptrSrc(ptrSrc)
	, m_ptrDest(ptrDest)
	, addedScenes_(addedScenes)
	{

	}
	/// Получает сцену и перекидывает её в другой диспетчер
	void operator()(qdGameScene* ptrScene)
	{
		//! удаляем для того, чтобы при закрытии диспетчера исходников
		//!  сцена не удалялась. 

		//! Мы можем производить любые действия надо исходными материалами, 
		// потому что потом ничего не сохраняем
		ptrScene->split_global_objects();
		if (m_ptrSrc->remove_scene(ptrScene))
		{
			if (!uniq_name(ptrScene->name(), m_ptrDest->scene_list()))
				renameScene(ptrScene);

			if (m_ptrDest->add_scene(ptrScene))
			{
				ptrScene->merge_global_objects();
				addedScenes_.push_back(ptrScene);
				ptree_api::LoadScene(ptrScene);
			}
		}
	}
private:
	void renameScene(qdGameScene* ptrScene)
	{
		CString const strUniqName = 
			get_unique_name(ptrScene->name(), m_ptrDest->scene_list());
		m_ptrSrc->rename_scene(ptrScene, strUniqName);
	}
private:
	/// Диспетчер текщего проекта
	qdGameDispatcher* const m_ptrDest;
	/// Диспетчер проекта открытого для слияния
	qdGameDispatcher* const m_ptrSrc;
	/// Ссылка! на список добавленных сцен.
	GameSceneList& addedScenes_;
};

ProjectJoiner::ProjectJoiner(void)
{
}

ProjectJoiner::~ProjectJoiner(void)
{
}

void ProjectJoiner::join(qdGameDispatcher* ptrGD)
{
	CWaitCursor wait;
	if (!ptrGD)
		return;
	m_ptrGameDisp = ptrGD;
	CString const res = getProjFile();
	if (res.IsEmpty())
		return;
	qdGameDispatcher qdAnotherDisp;
	if (!initAnotherGameDisp(res, &qdAnotherDisp))
		return;
	GameSceneList lstScenes;
	if (!selectSceneForJoin(&qdAnotherDisp, lstScenes))
		return ;

	merge(lstScenes, &qdAnotherDisp);
}

CString const ProjectJoiner::getProjFile() const
{
	CString str;
	str.LoadString(IDS_OPEN_SCRIPT_EXT);
	CString str2;
	str2.LoadString(IDS_OPEN_SCRIPT_FILTER);
	CString strTitle(reinterpret_cast<LPTSTR>(IDS_OPEN_PROJECT_FOR_JOIN_TITLE));
	return disk_op::Get1DiskFileName(GetCurDirSafer, str2, str, strTitle);
}

bool ProjectJoiner::initAnotherGameDisp(CString const& str, 
										qdGameDispatcher* ptrAnotherDisp) const
{
	try
	{
		ptrAnotherDisp->load_script(str);
	}
	catch (...) {
		return false;
	}
	return true;
}

/// производим выбор сцен для переноса
bool ProjectJoiner::selectSceneForJoin(qdGameDispatcher * ptrAnotherDisp, 
										GameSceneList& lstOut) 
{
	///	Загружает список сцен
	class ListLoader : public INamedObjListToList
	{
		qdGameSceneList const& m_lst;
	public:
		ListLoader(qdGameSceneList const& lst):m_lst(lst)
		{
		}
		bool load(CListCtrl& lstCtrl) const
		{
			std::copy(m_lst.begin(), m_lst.end(), 
				boost::make_function_output_iterator(insert2lst(lstCtrl)));
			return true;
		}
	};

	///	Возвращает список помеченных сцен
	class ListReader : public INamedObjListReader
	{
		GameSceneList& m_lstOut;
	public:
		ListReader(GameSceneList& lstOut):m_lstOut(lstOut){}
		bool read(CListCtrl& lstCtrl) {
			int const count = lstCtrl.GetItemCount();
			for(int i = 0; i < count; ++i)
			{
				if (lstCtrl.GetCheck(i))
				{
					m_lstOut.push_back(
						reinterpret_cast<qdGameScene*>(lstCtrl.GetItemData(i)));
				}
			}
			return true;
		}
	};

	NamedObjectSelectorDlg dlg;
	dlg.setCapId(IDS_SELECT_SCENES);
	ListLoader ldr(ptrAnotherDisp->scene_list());
	lstOut.clear();
	ListReader rdr(lstOut);
	dlg.setListLoader(&ldr);
	dlg.setListReader(&rdr);
	if (dlg.DoModal() == IDOK)
		return true;
	return false;
}

bool ProjectJoiner::merge(GameSceneList const& lst, qdGameDispatcher* ptrAnotherDisp)
{
	GameSceneList addedScenes;
	std::for_each(lst.begin(), lst.end(), 
		scene_injector(m_ptrGameDisp, ptrAnotherDisp, addedScenes));
	std::for_each(addedScenes.begin(), addedScenes.end(), 
		std::mem_fun(qdGameScene::init));	

	return true;
}
