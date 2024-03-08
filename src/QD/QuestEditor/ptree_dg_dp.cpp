/********************************************************************
	created:	2002/09/11
	created:	11:9:2002   17:38
	filename: 	D:\Projects\Quest\QuestEditor\ptree_dg_dp.cpp
	file path:	D:\Projects\Quest\QuestEditor
	file base:	ptree_dg_dp
	file ext:	cpp
	Powerd by:	Илюха
	
	purpose:	Всё что касается drag and drop
*********************************************************************/

#include "stdafx.h"
#include "questeditor.h"

#include "qd_game_object_static.h"
#include "qd_game_object_animated.h"
#include "qd_game_object_moving.h"
#include "qd_game_object_mouse.h"
#include "qd_trigger_chain.h"
#include "qd_video.h"
#include "qd_inventory.h"

#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"

#include "qd_grid_zone.h"

#include "SceneTreeView.h"
#include "mainfrm.h"
#include "childview.h"
#include "ITriggerView.h"

#include "ptree_hlprs.hpp"
#include "ptree_loading.h"

#include <Additional/qls.hpp>
#include <Additional/itr_traits_spec.hpp>
#include <boost/type_traits.hpp>

#include "name_op.h"
#include "obj_type_detectors.h"
#include "CursorIdUpdater.h"

class qdGameObjectState;
BOOST_BROKEN_COMPILER_TYPE_TRAITS_SPECIALIZATION(qdGameObjectState)
BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(qdGameObjectState)


/************************************************************************/
/* Глобальные функции                                                   */
/************************************************************************/
namespace{
/*!
	Можем ли драгендропить объект
	\param dwItemData - данные, привязанные к тому узлу в дереве, который пытаемся
		тащить
*/
bool CanDrag(DWORD dwItemData)
{
	if (!ptree_helpers::is_obj(dwItemData))
		return false;

	std::pair<qdNamedObjectBase*, bool> r = 
		ptree_helpers::is_any_interf_obj(dwItemData);
	if (r.second)
	{
		return false;
	}
	else
	{
		qdNamedObject* pno = static_cast<qdNamedObject*>(r.first);
		if(ptree_helpers::IsGlobalObj(pno)&&ptree_helpers::IsGameObject(pno))
			return true;

		int type = pno->named_object_type();
		return (type == QD_NAMED_OBJECT_OBJ_STATE
			||type == QD_NAMED_OBJECT_SCENE
			||type == QD_NAMED_OBJECT_VIDEO
			||type == QD_NAMED_OBJECT_MINIGAME
			||type == QD_NAMED_OBJECT_MUSIC_TRACK
			||type == QD_NAMED_OBJECT_GRID_ZONE_STATE
			||type == QD_NAMED_OBJECT_GRID_ZONE
			||type == QD_NAMED_OBJECT_TRIGGER_CHAIN
			||type == QD_NAMED_OBJECT_GAME_END
			||type == QD_NAMED_OBJECT_INVENTORY
			||ptree_helpers::is_animated(pno));
	}
}

/*!
 	Получает последний видимый узел в дереве
*/
HTREEITEM GetLastVisible(CTreeCtrl& tree)
{
	RECT r;
	tree.GetClientRect(&r);
	SHORT sItemHeight = tree.GetItemHeight();
	
	POINT p = {20,r.bottom - (sItemHeight>>1)};
	return tree.HitTest(p,NULL);
}

/*!
 	Возвращает позицию состояния в списке состояний объекта
*/
int GetStatePos(qdGameObjectAnimated* pobj, qdGameObjectState* pstate)
{
	const qdGameObjectStateVector& v = pobj->state_vector();
	typedef qdGameObjectStateVector::const_iterator ci_t;
	ci_t _res = qls::find(v.begin(), v.end(), pstate);
	
	ASSERT(_res != v.end());
	
	return std::distance(v.begin(), _res);
}

/*!
 	Находит первое состояние, удовлетворяющее фильтру
*/
template<class _Filter>
qdGameObjectState* FindFirstState(qdGameObjectAnimated* pobj, _Filter _f)
{
	const qdGameObjectStateVector& v = pobj->state_vector();
	typedef qdGameObjectStateVector::const_iterator ci_t;
	ci_t res = std::find_if(v.begin(), v.end(), _f);
	if (res != v.end()) {
		return *res;
	}
	return NULL;
}

/*!
 	Находит первое собственное(неглобальное) походочное состояние в объекте
*/
qdGameObjectState* FindFirstOwnWalk(qdGameObjectAnimated* pobj)
{
	return FindFirstState(pobj, ptree_loading::own_walk(pobj));
}

/*!
 	Находит первое собственное(неглобальное) непоходочное состояние в объекте
*/
qdGameObjectState* FindFirstOwnNotWalk(qdGameObjectAnimated* pobj)
{
	return FindFirstState(pobj, ptree_loading::own_not_walk(pobj));
}

/*!
 	Восстанавливает положение изображения при копировании(переносе) состояния из
	одного объекта в другой.
	Получает экранные координаты исходного объекта с установленным исходным состоянием.
	Получает экранные координаты второго объекта с установленным новым состоянием.
	Добавляет к смещению нового состояния разницу между положениями двух объектов.
	\remarks \a pnew является копией \a psource
	\param psource	- Исходное состояние
	\param po_src	- Исходный объект
	\param pmo		- Объект, в который вставляем состояние
	\param pnew		- Состояние, которое вставлено
*/
void RestoreImageLayout(qdGameObjectState* psource, qdGameObjectAnimated * po_src,
						qdGameObjectAnimated* pmo, qdGameObjectState* pnew)
{
	if (ptree_helpers::IsMovementState(psource)||psource->coords_animation()->is_empty())
		return;

	qdGameObjectState* pCurState = po_src->get_cur_state();
	if (pCurState != psource)
	{
		po_src->set_state(psource);
		po_src->update_screen_pos();
	}
	Vect2i const scr_pos = po_src->screen_pos();
	if (pCurState != psource)
		po_src->set_state(pCurState);//восстанавливаем текущее

	pCurState = pmo->get_cur_state();
	pmo->set_state(pnew);
	pmo->update_screen_pos();
	Vect2i const scr_pos2 = pmo->screen_pos();

	pmo->set_state(pCurState);
	pmo->update_screen_pos();

	pnew->set_center_offset(pnew->center_offset()+(scr_pos - scr_pos2));
}

/*!
 	Получает владельца состояния копии(параметр \a pnew) и передает управление
	RestoreImageLayout(qdGameObjectState* psource, qdGameObjectAnimated * po_src,
	qdGameObjectAnimated* pmo, qdGameObjectState* pnew)
*/
void RestoreImageLayout(qdGameObjectState* psource, 
						qdGameObjectAnimated* pmo, qdGameObjectState* pnew)
{
	ASSERT(psource);
	qdGameObjectAnimated* pobj = static_cast<qdGameObjectAnimated*>(psource->owner());
	RestoreImageLayout(psource, pobj, pmo, pnew);
}

/*!
	Выравнивает положение изображений для состояний при копировании динамического 
	объекта в персонаж
	\remarks \a pobj - должен быть копией \a psrc
*/
void SyncObjectsImageLayout(qdGameObjectAnimated * psrc, qdGameObjectMoving *pobj)
{
	qdGameObjectStateVector const& v = psrc->state_vector();
	qdGameObjectStateVector const& v2 = pobj->state_vector();
	qdGameObjectStateVector::const_iterator i = v.begin(), e = v.end();
	qdGameObjectStateVector::const_iterator out = v2.begin();
	for(; i != e; ++i, ++out)
	{
		RestoreImageLayout(*i, psrc, pobj, *out);
	}
}

/*!
 	Копирует динаический объект в динамический объект.
	Создает копии исходного объекта. Назначает ему уникальное имя. Добавляет в проект.
	Вставляет в дерево. Загружает состояния
	\param tree		- Дерево, куда вствляется объект
	\param hParent	- Родительский узел
	\param pobj		- Исходный объект
	\param pscene	- Сцена, куда добавляется объект  
*/
BOOL CopyAnimatedToAnimated(CTreeCtrl& tree, HTREEITEM hParent, 
							qdGameObjectAnimated const * pobj, qdGameScene* pscene)
{
	std::auto_ptr<qdGameObjectAnimated> ptr(new qdGameObjectAnimated(*pobj));
	CString const name = get_unique_name(pobj->name(), pscene->object_list());
	ptr->set_name(name);
	if (!pscene->add_object(ptr.get()))
	{
		tree.SelectDropTarget(NULL);
		return FALSE;
	}

	HTREEITEM hObj = ptree_loading::InsertNamedObject(tree, hParent, 
		ptr.get(), II_ANIMATED_OBJ_NODE);
	if (!hObj)
	{
		pscene->remove_object(ptr.get());
		tree.SelectDropTarget(NULL);
		return FALSE;
	}
	ptree_loading::LoadStates(tree, hObj, ptr.get());
	tree.Expand(hObj, TVE_EXPAND);

	ptr.release();
	tree.SelectDropTarget(NULL);
	return TRUE;
}

/*!
 	Копирует динаический объект в персонаж.
	Создает копии исходного объекта. Назначает ему уникальное имя. Добавляет в проект.
	Вставляет в дерево. Выравнивает положение изображений. Загружает состояния
	\param tree		- Дерево, куда вствляется объект
	\param hParent	- Родительский узел
	\param pobj		- Исходный объект
	\param pscene	- Сцена, куда добавляется объект  
*/
BOOL CopyAnimatedToMoving(CTreeCtrl& tree, HTREEITEM hParent, 
						  qdGameObjectAnimated const * pobj, qdGameScene* pscene)
{
	std::auto_ptr<qdGameObjectMoving> ptr(new qdGameObjectMoving(*pobj));
	CString const name = get_unique_name(pobj->name(), pscene->object_list());
	ptr->set_name(name);
	ptr->adjust_z();
	if (!pscene->add_object(ptr.get()))
	{
		tree.SelectDropTarget(NULL);
		return FALSE;
	}

	HTREEITEM hObj = ptree_loading::InsertNamedObject(tree, hParent, 
		ptr.get(), II_MOVING_OBJ_NODE);
	if (!hObj)
	{
		pscene->remove_object(ptr.get());
		tree.SelectDropTarget(NULL);
		return FALSE;
	}
	SyncObjectsImageLayout(const_cast<qdGameObjectAnimated*>(pobj), ptr.get());

	ptree_loading::LoadStates(tree, hObj, ptr.get());
	tree.Expand(hObj, TVE_EXPAND);

	ptr.release();
	tree.SelectDropTarget(NULL);
	return TRUE;
}

/*!
 	Копирует персонаж в персонаж
	Создает копии исходного объекта. Назначает ему уникальное имя. Добавляет в проект.
	Вставляет в дерево. Загружает состояния
	\param tree		- Дерево, куда вствляется объект
	\param hParent	- Родительский узел
	\param pobj		- Исходный объект
	\param pscene	- Сцена, куда добавляется объект  
*/
BOOL CopyMovingToMoving(CTreeCtrl& tree, HTREEITEM hParent, 
						qdGameObjectMoving const* pobj, qdGameScene* pscene)
{
	std::auto_ptr<qdGameObjectMoving> ptr(new qdGameObjectMoving(*pobj));
	CString const name = get_unique_name(pobj->name(), pscene->object_list());
	ptr->set_name(name);
	if (!pscene->add_object(ptr.get()))
	{
		tree.SelectDropTarget(NULL);
		return FALSE;
	}

	HTREEITEM hObj = 
		ptree_loading::InsertNamedObject(tree, hParent, 
											ptr.get(), II_MOVING_OBJ_NODE);
	if (!hObj)
	{
		pscene->remove_object(ptr.get());
		tree.SelectDropTarget(NULL);
		return FALSE;
	}
	ptree_loading::LoadStates(tree, hObj, ptr.get());
	tree.Expand(hObj, TVE_EXPAND);

	ptr.release();
	tree.SelectDropTarget(NULL);
	return TRUE;
}

}
/************************************************************************/
/* Драг и дроп для класса SceneTreeView                                 */
/************************************************************************/
BOOL SceneTreeDropTarget::OnDrop(CWnd* pWnd, 
								 COleDataObject* pData, 
								 DROPEFFECT dropEffect, CPoint point)
{
	UINT uiDataFormat = RegisterClipboardFormat(CString((LPTSTR)IDS_CB_FORMAT_NAME));
	ASSERT (pData->IsDataAvailable(uiDataFormat)); 
	
	HGLOBAL hData = pData->GetGlobalData(uiDataFormat);
	void* pdata = GlobalLock(hData);
	GlobalUnlock(hData);

	m_bDropInMe = true;

	return static_cast<SceneTreeView*>(pWnd)->OnDropData(pdata, dropEffect, point);
}

DROPEFFECT SceneTreeDropTarget::OnDragEnter( CWnd* pWnd, 
											COleDataObject* pDataObject, 
											DWORD dwKeyState, CPoint point )
{
	UINT uiId = RegisterClipboardFormat(CString((LPTSTR)IDS_CB_FORMAT_NAME));
	if (pDataObject->IsDataAvailable(uiId))
		return ((SceneTreeView*)pWnd)->CanDrop(point, pDataObject);
	return DROPEFFECT_NONE;
}

DROPEFFECT SceneTreeDropTarget::OnDragOver( CWnd* pWnd, 
										   COleDataObject* pDataObject, 
										   DWORD dwKeyState, CPoint point )
{
	UINT uiId = RegisterClipboardFormat(CString((LPTSTR)IDS_CB_FORMAT_NAME));
	if (pDataObject->IsDataAvailable(uiId))
	{
		return ((SceneTreeView*)pWnd)->CanDrop(point, pDataObject);
	}
	return DROPEFFECT_NONE;
}
DROPEFFECT SceneTreeDropTarget::OnDragScroll(CWnd* pWnd, 
													 DWORD dwKeyState, 
													 CPoint point)
{
	return COleDropTarget::OnDragScroll(pWnd, dwKeyState, point);
}

void SceneTreeView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* lpnmtv = (NM_TREEVIEW*)pNMHDR;
	
	CTreeCtrl& tree = GetTreeCtrl();
	DWORD dwItemData = tree.GetItemData(lpnmtv->itemNew.hItem);
	if(!CanDrag(dwItemData))
		return;
	m_hDraggedItem = lpnmtv->itemNew.hItem;
	//m_hDraggedItemParent нам нужен, когда перетакскиваем
	//состояния под другие объекты. Так что если он будет NULL
	//ничего страшного.
	m_hDraggedItemParent = IsUnderAnimated(m_hDraggedItem);
	if (!m_hDraggedItemParent)
		m_hDraggedItemParent = tree.GetParentItem(m_hDraggedItem);
	
	std::auto_ptr<COleDataSource> pSource(new COleDataSource);
	
	CString m_strText;
	m_strText = tree.GetItemText(lpnmtv->itemNew.hItem);
	
	int n = m_strText.GetLength();
	
	CString strCFName((LPTSTR)IDS_CB_FORMAT_NAME);
	UINT clFromatId = RegisterClipboardFormat(strCFName);
	
	HGLOBAL h = ::GlobalAlloc( GMEM_SHARE, 2*sizeof(qdNamedObjectBase*));
	LPVOID p = ::GlobalLock( h );
	LPBYTE pT = (LPBYTE)p;

	memcpy( pT , &dwItemData, sizeof(DWORD) );

	qdNamedObject* owner = 
		reinterpret_cast<qdNamedObject*>(tree.GetItemData(m_hDraggedItemParent));
	memcpy( pT + sizeof(qdNamedObject*), &owner, sizeof(qdNamedObject*) );
	::GlobalUnlock( h );
	
	pSource->CacheGlobalData( clFromatId, h );
	
	m_ptrDT->ResetDropInMe();

	DWORD res = pSource->DoDragDrop();
	if (res == DROPEFFECT_COPY&&!m_ptrDT->DropedInMe()) {
		//значит кинули в окно редактора триггеров
		//больше мы никуда кинуть не можем

//		SetInTrigger(GetTreeCtrl(), lpnmtv->itemNew.hItem, TRUE, II_ACTIVE_TRIGGER);
	}
	GetTreeCtrl().SelectDropTarget(NULL);	
	*pResult = 0;
}

/************************************************************************/
/* Поддержка drag and drop для состояний                                */
/************************************************************************/
/*!
 	Глобальный объект можно только скопировать, перенеся его под сцену
	\param hItem	- Узел, над которым бросили
	\param point	- Точка, в которой бросили
	\param pobj		- Объект, который хотим бросить
	\retval DROPEFFECT_COPY - можем скопировать
	\retval DROPEFFECT_NONE - нельзя скопировать
*/
DROPEFFECT SceneTreeView::cdGlobalObj(HTREEITEM hItem, const CPoint& point, 
															qdGameObject* pobj)
{
	CTreeCtrl& tree = GetTreeCtrl();

	if (HTREEITEM hScene = ptree_helpers::IsObjUnderScene(tree, hItem)) {
		qdGameScene* pscene = 
			reinterpret_cast<qdGameScene*>(tree.GetItemData(hScene));
		if (!pscene->get_object(pobj->name())) {
			tree.SelectDropTarget(hScene);
			return DROPEFFECT_COPY;
		}
	}
	tree.SelectDropTarget(NULL);
	return DROPEFFECT_NONE;
}
/*!
	Персонажи можно только скопировать. Если тащат над каталогом персонажей в сцене,
	то разрешаем копировать.
	\param hItem	- Узел, над которым бросили
	\param point	- Точка, в которой бросили
	\param pobj		- Объект, который хотим бросить
	\retval DROPEFFECT_COPY - можем скопировать
	\retval DROPEFFECT_NONE - нельзя скопировать
*/
DROPEFFECT  SceneTreeView::cdMovingObj(HTREEITEM hItem, 
									   const CPoint& point, 
										qdGameObjectMoving* pobj)
{
	CTreeCtrl& tree = GetTreeCtrl();
	tree.SelectDropTarget(NULL);
	if (HTREEITEM hCatalog = ptree_helpers::IsUnderMovingCatalog(tree, hItem)) 
	{
		HTREEITEM hScene = tree.GetParentItem(hCatalog);
		if (ptree_helpers::is_scene(tree, hScene))
		{
			qdGameScene* pscene = 
				reinterpret_cast<qdGameScene*>(tree.GetItemData(hScene));
			if (pscene == pobj->owner()) {
				tree.SelectDropTarget(hCatalog);
//				if (ptree_helpers::IsKeyPressed(VK_CONTROL))
					return DROPEFFECT_COPY;
//				else
//					return DROPEFFECT_MOVE;
			}
		}
		else
		{

		}
	}
	return DROPEFFECT_NONE;
}
 
/*!
	Динамический объект можно перенести или скопировать. Если тащим над каталогам
	анимированных объектов или каталогом персонажей сцены, которой принадлежит объект
	то позовляем переносить и копировать
	\param hItem	- Узел, над которым бросили
	\param point	- Точка, в которой бросили
	\param pobj		- Объект, который хотим бросить
	\retval DROPEFFECT_COPY - можем скопировать
	\retval DROPEFFECT_MOVE - можем перенести
	\retval DROPEFFECT_NONE - нельзя скопировать
*/
DROPEFFECT  SceneTreeView::cdAnimatedObj(HTREEITEM hItem, 
										 const CPoint& point, 
											qdGameObjectAnimated* pobj)
{
	CTreeCtrl& tree = GetTreeCtrl();

	HTREEITEM hCatalog = NULL;
	if ((hCatalog = ptree_helpers::IsUnderAnimatedCatalog(tree, hItem))
		||(hCatalog = ptree_helpers::IsUnderMovingCatalog(tree, hItem))) 
	{
		HTREEITEM hScene = tree.GetParentItem(hCatalog);
		if (ptree_helpers::is_scene(tree, hScene))
		{
			qdGameScene* pscene = 
				reinterpret_cast<qdGameScene*>(tree.GetItemData(hScene));
			if (pscene == pobj->owner()) {
				tree.SelectDropTarget(hCatalog);
				if (ptree_helpers::IsUnderAnimatedCatalog(tree, hItem)
					||ptree_helpers::IsKeyPressed(VK_CONTROL))
					return DROPEFFECT_COPY;
				else
					return DROPEFFECT_MOVE;
			}
		}
		else
		{

		}
	}
	tree.SelectDropTarget(NULL);
	return DROPEFFECT_NONE;
}

/*!
 	Позволяет только копировать состояние. Если не нажата VK_CONTROL, то выходим, иначе
	передаём управление SceneTreeView::cdOwnState()
	\param hItem	- Узел, над которым бросили
	\param point	- Точка, в которой бросили
	\param pstate	- Объект, который хотим бросить
	\retval DROPEFFECT_COPY - можем скопировать
	\retval DROPEFFECT_NONE - нельзя скопировать
*/
DROPEFFECT SceneTreeView::cdGlobalState(HTREEITEM hItem, const CPoint& point, 
										qdGameObjectState* pstate)
{
	if (!ptree_helpers::IsKeyPressed(VK_CONTROL)) {
		return DROPEFFECT_NONE;
	}
	
	return cdOwnState(hItem, point, pstate);
}

/*!
 	Так как у персонажа 2 каталога состояний, то приходится сравнивать тип состояния
	(походочное или нет) с типом каталога. Они должны совпадать
	\param hItem			- Узел, над которым бросили
	\param point			- Точка, в которой бросили
	\param dragged_state	- Объект, который хотим бросить
	\retval DROPEFFECT_COPY - можем скопировать
	\retval DROPEFFECT_MOVE - можем перенести
	\retval DROPEFFECT_NONE - нельзя скопировать
*/
DROPEFFECT SceneTreeView::cdOwnStateUnderMoving(HTREEITEM hItem, 
						HTREEITEM hObj, qdGameObjectState* draged_state)
{
	CTreeCtrl& tree = GetTreeCtrl();
	if(!CanDropUnderMoving(hObj))
	{
		tree.SelectDropTarget(NULL);
		return DROPEFFECT_NONE;
	}
	qdGameObjectAnimated* pobj = 
		reinterpret_cast<qdGameObjectAnimated*>(tree.GetItemData(hObj));
	if (hObj != m_hDraggedItemParent&&draged_state->ref_owner() == pobj) {
		tree.SelectDropTarget(NULL);
		return DROPEFFECT_NONE;
	}
				
	bool bIsWalkCatalog = false;
	if (HTREEITEM hStateCatalog = IsUnderStateCatalog(hItem, &bIsWalkCatalog)) 
	{
		if (ptree_helpers::IsMovementState(draged_state)){
			if (bIsWalkCatalog) {
				if (hItem == hStateCatalog) {
					tree.SelectDropTarget(hItem);
				}
				else {
					//значит у нас есть реальный объект состояние
					qdGameObjectState* pstate = 
						reinterpret_cast<qdGameObjectState*>
							(tree.GetItemData(hItem));

					//не можем кидать на глобальные состония
					if (ptree_helpers::IsGlobalState4Obj(pstate, pobj)) {
						tree.SelectDropTarget(hStateCatalog);
					}
					else
						tree.SelectDropTarget(hItem);
				}
				if (ptree_helpers::IsKeyPressed(VK_CONTROL)) {
					return DROPEFFECT_COPY;
				}
				return DROPEFFECT_MOVE;
			}
		}
		else if (!bIsWalkCatalog)
		{
			if (hItem == hStateCatalog) {
				tree.SelectDropTarget(hItem);
			}
			else {
				
				qdGameObjectState* pstate = 
					reinterpret_cast<qdGameObjectState*>
					(tree.GetItemData(hItem));
				
				//не можем кидать на глобальные состония
				if (ptree_helpers::IsGlobalState4Obj(pstate, pobj)) {
					tree.SelectDropTarget(hStateCatalog);
				}
				else
					tree.SelectDropTarget(hItem);
			}
			if (ptree_helpers::IsKeyPressed(VK_CONTROL)) {
				return DROPEFFECT_COPY;
			}
			return DROPEFFECT_MOVE;
		}
	}
	tree.SelectDropTarget(NULL);
	return DROPEFFECT_NONE;
}

/*!
 	Бросить можем только под динамический объект или персонаж. Нельзя перетащить 
	курсор мыши под объект сцены
	\param hItem			- Узел, над которым бросили
	\param point			- Точка, в которой бросили
	\param dragged_state	- Объект, который хотим бросить
	\retval DROPEFFECT_COPY - можем скопировать
	\retval DROPEFFECT_MOVE - можем перенести
	\retval DROPEFFECT_NONE - нельзя скопировать
*/
DROPEFFECT SceneTreeView::cdOwnState(HTREEITEM hItem, const CPoint& point, 
									 qdGameObjectState* draged_state)
{
	CTreeCtrl& tree = GetTreeCtrl();
	if (hItem&&hItem != m_hDraggedItem) {
		bool IsMovingObj = false;
		//кинуть можем только под анимированный объект
		//(Это в том числе и персонаж)
		HTREEITEM hAnimObj = IsUnderAnimated(hItem, &IsMovingObj);
		if (hAnimObj) {
			
			if ((!IsMovingObj&&CanDropUnderAnim(hAnimObj))) {
				
				tree.SelectDropTarget(hItem);
				
				if (ptree_helpers::IsKeyPressed(VK_CONTROL)) {
					return DROPEFFECT_COPY;
				}
				return DROPEFFECT_MOVE;
			}
			else if(IsMovingObj)
			{
				return cdOwnStateUnderMoving(hItem, hAnimObj, draged_state);
			}
		}
	}
	tree.SelectDropTarget(NULL);
	return DROPEFFECT_NONE;
}

/*!
 	Определяет над каким узлом тащим. Достает из m_hDraggedItem объект, который тащим
	и в зависимости от типа этого объекта  вызываем функцию проверки.
	\param point		- точка, в которой находтся курсор
	\param pDataObject	- Объект переносчик данных для drag and drop
	\return Можно ли бросать данные, если можно, то что с ними делать(копировать 
		или переносить)
*/
DROPEFFECT SceneTreeView::CanDrop(const CPoint& point, 
												COleDataObject* pDataObject)
{
	CTreeCtrl& tree = GetTreeCtrl();
	UINT uiFlags = 0;
	HTREEITEM hItem = tree.HitTest(point, &uiFlags);
	if(!hItem)
		return DROPEFFECT_NONE;
	
	DWORD_PTR dwData = tree.GetItemData(m_hDraggedItem);
	if (!ptree_helpers::is_obj(dwData))
		return DROPEFFECT_NONE;

	std::pair<qdNamedObjectBase*, bool> res = ptree_helpers::is_any_interf_obj(dwData);
	if (res.second) 
	{
		//интерфейсные объекты таскать и брость не разрешаем
	}
	else
	{
		qdNamedObject* pobj = static_cast<qdNamedObject*>(res.first);

		qdNamedObject* par = 
			reinterpret_cast<qdNamedObject*>(tree.GetItemData(m_hDraggedItemParent));
		
		switch(pobj->named_object_type()) {
		case QD_NAMED_OBJECT_OBJ_STATE:
			if(ptree_helpers::IsGlobalState4Obj(
				static_cast<qdGameObjectState*>(pobj),
				static_cast<qdGameObjectAnimated*>(par)))
				return cdGlobalState(hItem, point, 
								static_cast<qdGameObjectState*>(pobj));
			else
				return cdOwnState(hItem, point, static_cast<qdGameObjectState*>(pobj));
			break;
		case QD_NAMED_OBJECT_MOVING_OBJ:
		case QD_NAMED_OBJECT_ANIMATED_OBJ:
			if (ptree_helpers::IsGlobalObj(pobj))
				return cdGlobalObj(hItem, point, static_cast<qdGameObject*>(pobj));
			else
				if (ptree_helpers::is_obj_animated(pobj))
					return cdAnimatedObj(hItem, point, 
						static_cast<qdGameObjectAnimated*>(pobj));
				else if (ptree_helpers::is_obj_moving(pobj))
					return cdMovingObj(hItem, point,
						static_cast<qdGameObjectMoving*>(pobj));
			break;
		case QD_NAMED_OBJECT_GRID_ZONE:
			return cdGridZone(hItem, point, static_cast<qdGridZone*>(pobj));
		case QD_NAMED_OBJECT_TRIGGER_CHAIN:
			return cdTriggerChain(hItem, point, static_cast<qdTriggerChain*>(pobj));
		case QD_NAMED_OBJECT_VIDEO:
			return cdVideo(hItem, point, static_cast<qdVideo*>(pobj));
		case QD_NAMED_OBJECT_INVENTORY:
			return cdInventory(hItem, point, static_cast<qdInventory*>(pobj));
		}
	}
	return DROPEFFECT_NONE;
}

/*!
 	Находится ли узел под каталогом состояний, если находится, то уточняем тип
	каталога. Несколько каталогов у нас у персонажа. Для него эта функция
	\param hItem	- Узел для теста
	\param pfIsWalk	- Является ли каталог каталогом походк. Может быть NULL
	\return Описатель узла каталога,если найден
*/
HTREEITEM SceneTreeView::IsUnderStateCatalog(HTREEITEM hItem, 
											 bool *pfIsWalk /* = NULL*/)
{
	CTreeCtrl &tree = GetTreeCtrl();
	HTREEITEM hres = NULL;
	if (ptree_helpers::IsStateCatalog(tree, hItem)) {
		hres = hItem;
	}
	else
	{
		hItem = tree.GetParentItem(hItem);
		if (hItem&&ptree_helpers::IsStateCatalog(tree, hItem)) 
			hres = hItem;
	}
	if (hres) {
		if (pfIsWalk) {
			*pfIsWalk = ptree_helpers::IsWalkStateCatalog(tree, hres);
		}
	}
	return hres;
}

/*!
 	Находится ли узел под узлом динамического объекта.
	\param hItem		- Узел
	\param pfIsMoving	- Через этот параметер возвращается признак того, что узел
						находится под персонажем. Может быть NULL
	\return Описатель узла динамического объекта, если он найден
*/
HTREEITEM SceneTreeView::IsUnderAnimated(HTREEITEM hItem, bool *pfIsMoving)
{
	CTreeCtrl &tree = GetTreeCtrl();
	do {
		DWORD_PTR dwData = tree.GetItemData(hItem);
		if (ptree_helpers::is_obj(dwData)) {
			qdNamedObject* pno = reinterpret_cast<qdNamedObject*>(dwData);
			const int type = pno->named_object_type();
			switch(type) {
			case QD_NAMED_OBJECT_SCENE:
			case QD_NAMED_OBJECT_DISPATCHER:
			case QD_NAMED_OBJECT_GAME_END:
				return NULL;

			case QD_NAMED_OBJECT_MOVING_OBJ:
				if (pfIsMoving) 
					*pfIsMoving = true;
			case QD_NAMED_OBJECT_ANIMATED_OBJ:
			case QD_NAMED_OBJECT_MOUSE_OBJ:
				return hItem;
			}
		}
	} while((hItem = tree.GetParentItem(hItem)) != NULL);
	return NULL;
}

/*!
 	Нельзя кинуть только курсор.
	\param hMovingObjNode - Объект, под который пытаются бросить
	\retval true - если можно
*/
bool SceneTreeView::CanDropUnderMoving(HTREEITEM hMovingObjNode)
{
	if (hMovingObjNode != m_hDraggedItemParent) 
	{
		CTreeCtrl& tree = GetTreeCtrl();
		qdGameObjectMoving* pa = 
			reinterpret_cast<qdGameObjectMoving*>(
				tree.GetItemData(m_hDraggedItemParent));
		if (ptree_helpers::is_obj_mouse(pa))
			return false;
	}
	return true;
}

/*!
 	Нельзя кинуть курсор и походочное состояние
	\param hAnimObjNode - узел анимированного объекта
	\retval true - если можно
*/
bool SceneTreeView::CanDropUnderAnim(HTREEITEM hAnimObjNode)
{
	CTreeCtrl& tree = GetTreeCtrl();
	if (ptree_helpers::IsMovementState(tree, m_hDraggedItem)) {
		return false;
	}
	if (hAnimObjNode != m_hDraggedItemParent) 
	{
		qdGameObjectAnimated* pa = 
			reinterpret_cast<qdGameObjectAnimated*>(
				tree.GetItemData(m_hDraggedItemParent));
		if (ptree_helpers::is_obj_mouse(pa))
			return false;
	}
	return true;
}

/*!
 	Достает из данных объект, который тащили и в зависимисти от типа объекта 
	вызываем одну из функции с префиксом odd...()
*/
BOOL SceneTreeView::OnDropData(void* pData, DROPEFFECT dropEffect, 
							   const CPoint& point)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hDropTarget = tree.GetDropHilightItem();
	if(!hDropTarget)
		return FALSE;

	qdNamedObject* pno = *static_cast<qdNamedObject**>(pData);
	switch(pno->named_object_type()) {
	case QD_NAMED_OBJECT_OBJ_STATE:
		oddOwnState(static_cast<qdGameObjectState*>(pno),dropEffect, point);
		break;
	case QD_NAMED_OBJECT_MOVING_OBJ:
		if (ptree_helpers::IsGlobalObj(pno))
			oddGlobalObject(static_cast<qdGameObject*>(pno), dropEffect, point);
		else
			oddMovingObject(static_cast<qdGameObjectMoving*>(pno), dropEffect, point);
		break;
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
		oddAnimatedObject(static_cast<qdGameObjectAnimated*>(pno), dropEffect, point);
		break;
	case QD_NAMED_OBJECT_GRID_ZONE:
		oddGridZone(static_cast<qdGridZone*>(pno), dropEffect, point);
		break;
	case QD_NAMED_OBJECT_TRIGGER_CHAIN:
		oddTriggerChain(static_cast<qdTriggerChain*>(pno), dropEffect, point);
		break;
	case QD_NAMED_OBJECT_VIDEO:
		oddVideo(static_cast<qdVideo*>(pno), dropEffect, point);
		break;
	case QD_NAMED_OBJECT_INVENTORY:
		oddInventory(static_cast<qdInventory*>(pno), dropEffect, point);
		break;
	}
	tree.SelectDropTarget(NULL);
	return TRUE;
}

/*!
 	Меняет порядок состояний, переносит в другой объект или копирует состояния брошенные
	под мышь
	\param hObjItem			- Узел объекта, под который бросли
	\param hDropTarget		- Узел, на который бросили
	\param dragged_state	- Перетаскиваемое состояние
	\param dropEffect		- Переностить или копировать
	\retval TRUE			- если операция закончилась успешно
*/
BOOL SceneTreeView::oddOwnStateMouseObj(HTREEITEM hObjItem, HTREEITEM hDropTarget,
								qdGameObjectState* dragged_state, DROPEFFECT dropEffect)
{
	CTreeCtrl& tree = GetTreeCtrl();
	qdGameObjectMouse* ptrMouse = 
		reinterpret_cast<qdGameObjectMouse*>(tree.GetItemData(hObjItem));

	qdGameObjectState* const pCursor4DynObj = 
		ptrMouse->default_cursor(qdGameObjectMouse::OBJECT_CURSOR)
		?ptrMouse->get_state(ptrMouse->default_cursor(qdGameObjectMouse::OBJECT_CURSOR))
		:NULL;
	qdGameObjectState* const pCursor4Zones = 
		ptrMouse->default_cursor(qdGameObjectMouse::ZONE_CURSOR)
		?ptrMouse->get_state(ptrMouse->default_cursor(qdGameObjectMouse::ZONE_CURSOR))
		:NULL;

	int iCurState = ptrMouse->cur_state();
	//если iCurState != 0, тогда ptrRestoreState будет восстановлено у объекта
	//иначе переустановим нулевое, каким бы оно не было
	qdGameObjectState* ptrRestoreState = ptrMouse->get_state(iCurState);

	//вставляем в новое
	//в объект
	std::auto_ptr<qdGameObjectState> ptrInsState;
	//сдвигаем ли мы состояние. Т.е. меняем его местоположение внутри
	//того же объекта
	bool bShift = (dropEffect == DROPEFFECT_MOVE
								&&hObjItem==m_hDraggedItemParent);

	if (bShift)
		ptrInsState.reset(dragged_state);
	else
		ptrInsState.reset(dragged_state->clone());

	if (!ptrInsState.get()) 
		return FALSE;

	if (dropEffect != DROPEFFECT_MOVE)
		ptrInsState->remove_all_conditions();

	if(ptree_helpers::HasGlobalFlag(ptrInsState.get()))
		ptrInsState->drop_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_GLOBAL_OWNER);

	//если не переносим внутри одного и того же объекта, 
	//то проверяем уникальность имени
	if (!bShift) {
		CString strName = get_unique_name(ptrInsState->name(),ptrMouse->state_vector());
		ptrInsState->set_name(strName);
	}

	//вставляем первым, если цель объект
	HTREEITEM hInsertAfter = TVI_FIRST;
	int iInsBefore = 0;

	//иначе вставляем после цель-состояния
	if (hDropTarget != hObjItem) {
		hInsertAfter = hDropTarget;

		qdGameObjectState* ptrInsAfterState = 
			reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hInsertAfter));
		iInsBefore = GetStatePos(ptrMouse, ptrInsAfterState) + 1;
		if (bShift) {
			//если вставляем состояние в позицию после его текущей,
			//то надо уменьшить iInsBefore, потому что после удаления
			//все состояния, находящиеся после dragged_state, сдвинуться
			int state_pos = GetStatePos(ptrMouse, dragged_state);
			if (iInsBefore - 1 > state_pos) 
				--iInsBefore;
		}
	}

	if (bShift){
		//при удалении неприходит уведомление
		//о том, что узел перестал быть выбранным. от этого баги
		//		HTREEITEM hItem4Del = FindObjectItem(ptrInsState.get());
		ASSERT(m_hDraggedItem == FindObjectItem(ptrInsState.get()));
		if (m_hDraggedItem == tree.GetSelectedItem()) 
			tree.SelectItem(tree.GetParentItem(m_hDraggedItem));

		tree.DeleteItem(m_hDraggedItem);
		m_hDraggedItem = NULL;
		ptrMouse->remove_state(ptrInsState.get());
	}

	HTREEITEM hInsertedState = InsertState(tree, hObjItem, ptrMouse,
		ptrInsState.get(), static_cast<UINT>(-1), hInsertAfter);

	if(!hInsertedState)
		return FALSE;

	if(!ptrMouse->insert_state(iInsBefore, ptrInsState.get())){
		tree.DeleteItem(hInsertedState);
		return FALSE;
	}
	ptrInsState.release();

	bool bNeedRedraw = true;

	if (iCurState) 
		ptrMouse->set_state(ptrRestoreState);
	else
		ptrMouse->set_default_state();

	if (dropEffect == DROPEFFECT_MOVE) {
		//Удаляем из старого места
		//у объекта
		if(!bShift&&CanRemoveItem(m_hDraggedItem))
		{
			RemoveItem(m_hDraggedItem, false);
			bNeedRedraw = false;
		}
	}

	CursorIdUpdater::run(*m_ptrGameDisp);

	UpdateObjStatesImages(ptrMouse, m_hDraggedItemParent);
	if (bNeedRedraw) 
		m_ptrSceneView->Redraw(sv_api::RM_REDRAW_CONTENT);
	return TRUE;
}

/*!
 	Меняет порядок состояний, переносит в другой объект или копирует состояния брошенные
	под динамический объект(в дереве)
	\param hAnimObjItem		- Узел объекта, под который бросли
	\param hDropTarget		- Узел, на который бросили
	\param dragged_state	- Перетаскиваемое состояние
	\param dropEffect		- Переностить или копировать
	\retval TRUE			- если операция закончилась успешно
*/
BOOL SceneTreeView::oddOwnStateAnimObj(HTREEITEM hAnimObjItem, 
									   HTREEITEM hDropTarget, 
									   qdGameObjectState* dragged_state,
									   DROPEFFECT dropEffect)
{
	CTreeCtrl& tree = GetTreeCtrl();
	//объект, под который бросили
	qdGameObjectAnimated* ptrAnimObj = 
		reinterpret_cast<qdGameObjectAnimated*>(tree.GetItemData(hAnimObjItem));
	
	int iCurState = ptrAnimObj->cur_state();
	//если iCurState != 0, тогда ptrRestoreState будет восстановлено у объекта
	//иначе переустановим нулевое, каким бы оно не было
	qdGameObjectState* ptrRestoreState = ptrAnimObj->get_state(iCurState);

	qdGameObjectAnimated* pDraggedStateOwner =
		static_cast<qdGameObjectAnimated*>(dragged_state->ref_owner());
	
	//вставляем в новое
	//в объект
	std::auto_ptr<qdGameObjectState> ptrInsState;
	//сдвигаем ли мы состояние. Т.е. меняем его местоположение внутри
	//того же объекта
	bool bShift = (dropEffect == DROPEFFECT_MOVE&&hAnimObjItem==m_hDraggedItemParent);
	
	if (bShift)
		ptrInsState.reset(dragged_state);
	else
		ptrInsState.reset(dragged_state->clone());

	if (!ptrInsState.get()) 
		return FALSE;

	if (dropEffect != DROPEFFECT_MOVE)
		ptrInsState->remove_all_conditions();

	if(ptree_helpers::HasGlobalFlag(ptrInsState.get()))
		ptrInsState->drop_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_GLOBAL_OWNER);
	
	//если не переносим внутри одного и того же объекта, 
	//то проверяем уникальность имени
	if (!bShift) {
		CString strName=get_unique_name(ptrInsState->name(),ptrAnimObj->state_vector());
		ptrInsState->set_name(strName);
	}
	
	//вставляем первым, если цель объект
	HTREEITEM hInsertAfter = TVI_FIRST;
	int iInsBefore = 0;
	
	//иначе вставляем после цель-состояния
	if (hDropTarget != hAnimObjItem) {
		hInsertAfter = hDropTarget;
		
		qdGameObjectState* ptrInsAfterState = 
			reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hInsertAfter));
		iInsBefore = GetStatePos(ptrAnimObj, ptrInsAfterState) + 1;
		if (bShift) {
			//если вставляем состояние в позицию после его текущей,
			//то надо уменьшить iInsBefore, потому что после удаления
			//все состояния, находящиеся после dragged_state, сдвинуться
			int state_pos = GetStatePos(ptrAnimObj, dragged_state);
			if (iInsBefore - 1 > state_pos) 
				--iInsBefore;
		}
	}
	
	if (bShift){
		//при удалении неприходит уведомление
		//о том, что узел перестал быть выбранным. от этого баги
		ASSERT(m_hDraggedItem == FindObjectItem(ptrInsState.get()));
		if (m_hDraggedItem == tree.GetSelectedItem()) 
			tree.SelectItem(tree.GetParentItem(m_hDraggedItem));

		tree.DeleteItem(m_hDraggedItem);
		m_hDraggedItem = NULL;
		//теперь состоянием никто не владеет. Значит его надо удалять даже при 
		//сдвиге внутри одного объекта
		ptrAnimObj->remove_state(ptrInsState.get());
	}
	
	HTREEITEM hInsertedState = InsertState(tree, hAnimObjItem, ptrAnimObj,
		ptrInsState.get(), static_cast<UINT>(-1), hInsertAfter);

	if(!hInsertedState)
		return FALSE;
	
	if(!ptrAnimObj->insert_state(iInsBefore, ptrInsState.get())){
		tree.DeleteItem(hInsertedState);
		return FALSE;
	}
	if (!bShift&&ptree_helpers::is_obj_moving(pDraggedStateOwner))
		RestoreImageLayout(dragged_state, ptrAnimObj, ptrInsState.get());
	ptrInsState.release();
	
	bool bNeedRedraw = true;
	
	if (iCurState) 
		ptrAnimObj->set_state(ptrRestoreState);
	else
		ptrAnimObj->set_default_state();
	
	if (dropEffect == DROPEFFECT_MOVE) {
		//Удаляем из старого места
		//у объекта
		if(!bShift&&CanRemoveItem(m_hDraggedItem))
		{
			RemoveItem(m_hDraggedItem, false);
			bNeedRedraw = false;
		}
	}
	if (bNeedRedraw) 
		m_ptrSceneView->Redraw(sv_api::RM_REDRAW_CONTENT);

	return TRUE;
}

/*!
 	Меняет порядок состояний, переносит в другой объект или копирует состояния брошенные
	под персонажа(в дереве)
	\param hObjItem			- Узел объекта, под который бросли
	\param hDropTarget		- Узел, на который бросили
	\param dragged_state	- Перетаскиваемое состояние
	\param dropEffect		- Переностить или копировать
	\retval TRUE			- если операция закончилась успешно
	\ramarks Непоходочные состояния могут свободно переноситься(копироваться) между 
	динамическими объектами и персонажами. Походочными состояниями могут обмениваться
	только персонажи
*/
BOOL SceneTreeView::oddOwnStateMovingObj(HTREEITEM hObjItem, 
										 HTREEITEM hDropTarget,
										 qdGameObjectState* dragged_state,
										 DROPEFFECT dropEffect)
{
	CWaitCursor __wait__;
	CTreeCtrl& tree = GetTreeCtrl();

	qdGameObjectMoving* ptrMovingObj = 
		reinterpret_cast<qdGameObjectMoving*>(tree.GetItemData(hObjItem));

	bool bIsWalkCatalog = false;
	HTREEITEM hStateCatalog = IsUnderStateCatalog(hDropTarget, &bIsWalkCatalog);

	int iCurState = ptrMovingObj->cur_state();
	//если iCurState != 0, тогда ptrRestoreState будет восстановлено у объекта
	//иначе переустановим нулевое, каким бы оно не было
	qdGameObjectState* ptrRestoreState = ptrMovingObj->get_state(iCurState);

	//вставляем в новое
	//в объект
	std::auto_ptr<qdGameObjectState> ptrInsState;

	//сдвигаем ли мы состояние. Т.е. меняем его местоположение внутри
	//того же объекта
	bool bShift = (dropEffect == DROPEFFECT_MOVE&&
					hObjItem == m_hDraggedItemParent//внутри одного и того же объкта
					&&!ptree_helpers::IsGlobalState4Obj(dragged_state, ptrMovingObj));

	if (bShift)
		ptrInsState.reset(dragged_state);
	else
		ptrInsState.reset(dragged_state->clone());

	if (!ptrInsState.get()) 
		return FALSE;

	if (dropEffect != DROPEFFECT_MOVE)
		ptrInsState->remove_all_conditions();

	//сбрасываем флаг глобальности
	if(ptree_helpers::HasGlobalFlag(ptrInsState.get()))
		ptrInsState->drop_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_GLOBAL_OWNER);

	if (ptree_helpers::IsGlobalObj(ptrMovingObj)) {
		qdCoordsAnimation* pca = ptrInsState->coords_animation();
		if (!pca->is_empty()) 
			pca->clear();
	}
	//если не переносим внутри одного и того же объекта, 
	//то проверяем уникальность имени
	if (!bShift) {
		CString strName = get_unique_name(ptrInsState->name(), 
							ptrMovingObj->state_vector());
		ptrInsState->set_name(strName);
	}

	//вставляем первым НЕглобальным состоянием, если цель объект
	HTREEITEM hInsertAfter = TVI_FIRST;
	int iInsBefore = 0;

	//иначе вставляем после цель-состояния
	if (hDropTarget != hStateCatalog) {
		hInsertAfter = hDropTarget;

		qdGameObjectState* ptrInsAfterState = 
			reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hInsertAfter));

		iInsBefore = GetStatePos(ptrMovingObj, ptrInsAfterState) + 1;
		if (bShift) {
			//если вставляем состояние в позицию после его текущей,
			//то надо уменьшить iInsBefore, потому что после удаления
			//все состояния, находящиеся после dragged_state, сдвинуться
			int state_pos = GetStatePos(ptrMovingObj, dragged_state);
			if (iInsBefore - 1 > state_pos) 
				--iInsBefore;
		}
	}
	else if (!ptree_helpers::IsGlobalObj(ptrMovingObj)
		&&ptree_helpers::IsCopyOfGlobal(m_ptrGameDisp, ptrMovingObj->name()))
	{//раз копия значит у него есть глобальные состояния
		//находим первое неглобальное состоняние(походку или непоходку)
		qdGameObjectState* pFirstOwnState = NULL;
		if (bIsWalkCatalog) 
			 pFirstOwnState = FindFirstOwnWalk(ptrMovingObj);

		if (!pFirstOwnState) {
			//находим первое непоходочное и вставляем перед ним
			pFirstOwnState = FindFirstOwnNotWalk(ptrMovingObj);
		}

		if (!pFirstOwnState) {
			//если нет своих или непоходчных состояний, 
			//тогда вставлем последним
			iInsBefore = ptrMovingObj->state_vector().size();
			hInsertAfter = TVI_LAST;
		}
		else
		{
			iInsBefore = GetStatePos(ptrMovingObj, pFirstOwnState);

			hInsertAfter = 
				tree.GetPrevSiblingItem(FindObjectItem(pFirstOwnState));
			if (!hInsertAfter) {
				//значит не было у персонажа глобальных состояний
				//такого же типа как и здесь вставляемое.
				hInsertAfter = TVI_FIRST;
			}
		}
	}

	if (ptree_helpers::IsGlobalObj(ptrMovingObj)) {
		m_ptrGameDisp->split_global_objects(ptrMovingObj);
	}
	if (bShift) 
	{
		//при удалении неприходит уведомление
		//о том, что узел перестал быть выбранным. от этого баги
		ASSERT(m_hDraggedItem == FindObjectItem(ptrInsState.get()));
		if (m_hDraggedItem == tree.GetSelectedItem()) 
			tree.SelectItem(tree.GetParentItem(m_hDraggedItem));

		tree.DeleteItem(m_hDraggedItem);
		m_hDraggedItem = NULL;
		ptrMovingObj->remove_state(ptrInsState.get());
	}

	//в дерево
	HTREEITEM hInsertedState = 
		InsertState(tree, hStateCatalog, ptrMovingObj,ptrInsState.get(),
										static_cast<UINT>(-1), hInsertAfter);
	if(!hInsertedState)
		return FALSE;

	if(!ptrMovingObj->insert_state(iInsBefore, ptrInsState.get())){
		tree.DeleteItem(hInsertedState);
		return FALSE;
	}
	if (!bShift)
		RestoreImageLayout(dragged_state, ptrMovingObj, ptrInsState.get());

	ptrInsState.release();

	if (ptree_helpers::IsGlobalObj(ptrMovingObj)) 
		m_ptrGameDisp->merge_global_objects(ptrMovingObj);
	
	bool bNeedRedraw = true;
	
	if (iCurState) 
		ptrMovingObj->set_state(ptrRestoreState);
	else
		ptrMovingObj->set_default_state();
	
	if (dropEffect == DROPEFFECT_MOVE) {
		//Удаляем из старого места
		//у объекта
		if(!bShift&&CanRemoveItem(m_hDraggedItem))
		{
			RemoveItem(m_hDraggedItem, false);
			bNeedRedraw = false;
		}
	}

	if (ptree_helpers::IsGlobalObj(ptrMovingObj)) 
		SyncGlobalStatesCatalog(ptrMovingObj);

	if (bNeedRedraw) 
			m_ptrSceneView->Redraw(sv_api::RM_REDRAW_CONTENT);
	return TRUE;
}

/*!
 	Определяет под кого бросли и вызывает соответствующую функцию обработчик
*/
BOOL SceneTreeView::oddOwnState(qdGameObjectState* ptrState, 
								DROPEFFECT dropEffect, const CPoint& point)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hDropTarget = tree.GetDropHilightItem();

	bool bIsMoving = false;

	HTREEITEM hAnimObjItem = IsUnderAnimated(hDropTarget, &bIsMoving);
	ASSERT(hAnimObjItem);
	
	BOOL res = FALSE;
	if (bIsMoving) 
		res = oddOwnStateMovingObj(hAnimObjItem, hDropTarget, ptrState, dropEffect);
	else 
	{
		qdGameObjectAnimated* ptrAnimObj = 
			reinterpret_cast<qdGameObjectAnimated*>(tree.GetItemData(hAnimObjItem));
		if (ptree_helpers::is_obj_mouse(ptrAnimObj)) 
			res = oddOwnStateMouseObj(hAnimObjItem, hDropTarget, ptrState, dropEffect);
		else
			res = oddOwnStateAnimObj(hAnimObjItem, hDropTarget, ptrState, dropEffect);
	}
	SetProjChange();
	Invalidate();
	return res;
}

/*!
 	Определяет под какую сцену бросили и добавляет копию этого объекта в сцену
	(SceneTreeView.oddGlobalObject::AddGlobalObj2Scene())
*/
BOOL SceneTreeView::oddGlobalObject(qdGameObject* pobj, DROPEFFECT dropEffect, 
									const CPoint& point)
{
	CTreeCtrl& tree = GetTreeCtrl();
	
	HTREEITEM hDropTarget = tree.GetDropHilightItem();
	qdGameScene* pscene = 
		reinterpret_cast<qdGameScene*>(tree.GetItemData(hDropTarget));

	AddGlobalObj2Scene(pscene, pobj);

	SetProjChange();
	Invalidate();
	tree.SelectDropTarget(NULL);
	return TRUE;
}

/*!
 	Если персонаж - копия глобального пероснажа, то убираем у него глобальные состояния.
	Затем созадем копию (CopyAnimatedToMoving()). Восстанавливаем глобальные состояния
	\retval TRUE - если процесс завершился удачно
*/
BOOL SceneTreeView::oddMovingObject(qdGameObjectMoving* pobj, DROPEFFECT dropEffect,
					 const CPoint& point)
{
	CTreeCtrl& tree = GetTreeCtrl();

	HTREEITEM hDropTarget = tree.GetDropHilightItem();
	HTREEITEM hParent = tree.GetParentItem(hDropTarget);
	if (ptree_helpers::is_scene(tree, hParent))
	{
		qdGameScene* const pscene = 
			reinterpret_cast<qdGameScene*>(tree.GetItemData(hParent));
		bool bSplitting = false;
		if (ptree_helpers::IsCopyOfGlobal(m_ptrGameDisp, pobj->name()))
		{
			pscene->split_global_objects(m_ptrGameDisp->get_global_object(pobj->name()));
			bSplitting = true;
		}
		BOOL const res = CopyAnimatedToMoving(tree, hDropTarget, pobj, pscene);
		if (bSplitting)
			pscene->merge_global_objects(m_ptrGameDisp->get_global_object(pobj->name()));
		return res;
	}
	tree.SelectDropTarget(NULL);
	return FALSE;
}

/*!
 	Проверяет куда кинули. Если кинули в каталог анимированных объектов, то
	создает копию(CopyAnimatedToAnimated()), если в каталог персонажей, 
	то создает персонаж и копирует в него состояния(CopyAnimatedToMoving())
*/
BOOL SceneTreeView::oddAnimatedObject(qdGameObjectAnimated* pobj, DROPEFFECT dropEffect, 
									  const CPoint& point)
{
	CTreeCtrl& tree = GetTreeCtrl();

	HTREEITEM hDropTarget = tree.GetDropHilightItem();
	HTREEITEM hParent = tree.GetParentItem(hDropTarget);
	if (ptree_helpers::is_scene(tree, hParent))
	{
		qdGameScene* const pscene = 
			reinterpret_cast<qdGameScene*>(tree.GetItemData(hParent));
		if (ptree_helpers::IsUnderAnimatedCatalog(tree, hDropTarget))
			return CopyAnimatedToAnimated(tree, hDropTarget, pobj, pscene);
		else if (ptree_helpers::IsUnderMovingCatalog(tree, hDropTarget))
		{
			BOOL const res = CopyAnimatedToMoving(tree, hDropTarget, pobj, pscene);
			if (dropEffect == DROPEFFECT_MOVE)
				DeleteObject(pobj, false);
			return res;

		}
	}
	tree.SelectDropTarget(NULL);
	return FALSE;
}

/*!
 	Сеточные зоны можем только переносить внутри их каталога, т.е. менять зоны местами.
	Переностиь зоны в другую сцену нельзя
	\param hItem			- Узел, над которым бросили
	\param point			- Точка, в которой бросили
	\param pgz				- Объект, который хотим бросить
	\retval DROPEFFECT_MOVE - можем перенести
	\retval DROPEFFECT_NONE - нельзя скопировать
*/
DROPEFFECT SceneTreeView::cdGridZone(HTREEITEM hItem,  
									 const CPoint& point, qdGridZone* pgz)
{
	CTreeCtrl& tree = GetTreeCtrl();
	tree.SelectDropTarget(NULL);
	if (tree.GetParentItem(hItem) == m_hDraggedItemParent
		||hItem == m_hDraggedItemParent)
	{
		tree.SelectDropTarget(hItem);
		return DROPEFFECT_MOVE;
	}
	else
		return DROPEFFECT_NONE;
}

/*!
 	Зоны не копируются. Меняется только их порядок в дереве.
	Сначала добавляет копию перетаскиваемой зоны, а затем удаляет исходную
	\retval TRUE - если процесс завершился удачно
*/
BOOL SceneTreeView::oddGridZone(qdGridZone* pobj, DROPEFFECT dropEffect, 
								const CPoint& point)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hDropTarget = tree.GetDropHilightItem();
	qdGameScene* pscene = static_cast<qdGameScene*>(pobj->owner());


	qdGridZone* before = *pscene->grid_zone_list().begin();

	HTREEITEM hInsertAfter = hDropTarget;
	if (!ptree_helpers::is_net(tree, hDropTarget))//навели не на узел "Сетка"
	{
		HTREEITEM hBefore = tree.GetNextSiblingItem(hDropTarget);
		if (!hBefore)
			before = NULL;
		else
			before = reinterpret_cast<qdGridZone*>(tree.GetItemData(hBefore));
	}
	else
		hInsertAfter = TVI_FIRST;

	qdGridZone* zone = reinterpret_cast<qdGridZone*>(tree.GetItemData(m_hDraggedItem));
	if (zone == before)
		return TRUE;

	if (!pscene->remove_grid_zone(zone))
		return FALSE;
	
	tree.DeleteItem(m_hDraggedItem);

	pscene->insert_grid_zone(zone, before);
	InsertGZ2Tree(tree, m_hDraggedItemParent, zone, II_ZONE_NODE, hInsertAfter);
	return TRUE;
}

/*!
	Цепочку триггеров можно копировать только на другую цепочку
	\param hItem			- Узел, над которым бросили
	\param point			- Точка, в которой бросили
	\param pch				- Объект, который хотим бросить
	\retval DROPEFFECT_COPY - можем скопировать
	\retval DROPEFFECT_MOVE - можем перенести
	\retval DROPEFFECT_NONE - нельзя скопировать
 	
*/
DROPEFFECT SceneTreeView::cdTriggerChain(HTREEITEM hItem, 
										 const CPoint& point, qdTriggerChain* pch)
{
	CTreeCtrl& tree = GetTreeCtrl();

	//кинуть можем только на другой триггер.
	if (hItem == m_hDraggedItem
		||tree.GetParentItem(hItem) != m_hDraggedItemParent)
	{
		tree.SelectDropTarget(NULL);
		return DROPEFFECT_NONE;
	}
	tree.SelectDropTarget(hItem);
	return DROPEFFECT_COPY;
}

/*!
 	Определяет на какую цепочку бросли и делает объединение 2х цепочек(
	SceneTreeView::MergeTriggers())
*/
BOOL SceneTreeView::oddTriggerChain(qdTriggerChain* pch, 
											DROPEFFECT dropEffect, const CPoint& pnt)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hDropTarget = tree.GetDropHilightItem();
	qdTriggerChain* ptarget = 
		reinterpret_cast<qdTriggerChain*>(tree.GetItemData(hDropTarget));
	tree.SelectDropTarget(NULL);

	return MergeTriggers(pch, ptarget);
}

DROPEFFECT SceneTreeView::cdVideo(HTREEITEM hItem, CPoint const& point, qdVideo* pvideo)
{
	CTreeCtrl& tree = GetTreeCtrl();
	if (hItem != m_hDraggedItem
		&&(ptree_helpers::IsVideoCatalog(tree, hItem)
		||ptree_helpers::IsVideoCatalog(tree, tree.GetParentItem(hItem)))
		)
	{
		tree.SelectDropTarget(hItem);
		return DROPEFFECT_MOVE;
	}
	return DROPEFFECT_NONE;
}

BOOL SceneTreeView::oddVideo(qdVideo* video, DROPEFFECT dropEffect, CPoint const& point)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hDropTarget = tree.GetDropHilightItem();
	HTREEITEM hInsertAfter = TVI_FIRST;
	HTREEITEM hVideoCatalog = NULL;
	if (!m_ptrGameDisp->remove_video(video))
		return FALSE;
	tree.DeleteItem(m_hDraggedItem);

	qdVideo const* before = NULL;
	if (ptree_helpers::IsVideoCatalog(tree, hDropTarget))
	{
		hVideoCatalog = hDropTarget;
		//Вставляем первым
		if (!m_ptrGameDisp->video_list().empty())
			before = m_ptrGameDisp->video_list().front();
	}
	else
	{
		hVideoCatalog = tree.GetParentItem(hDropTarget);
		hInsertAfter = hDropTarget;
		if (HTREEITEM hBefore = tree.GetNextSiblingItem(hInsertAfter)) 
			before = reinterpret_cast<qdVideo*>(tree.GetItemData(hBefore));
	}
	m_ptrGameDisp->add_video(video, before);
	HTREEITEM hNewItem = ptree_loading::InsertObject(tree, 
						hVideoCatalog, video, II_VIDEO_NODE, hInsertAfter);
	SetProjChange();
	tree.SelectDropTarget(NULL);
	return TRUE;
}

//! Можем ли бросить инвентори
DROPEFFECT SceneTreeView::cdInventory(HTREEITEM hItem, 
									  CPoint const& point, 
									  qdInventory* pinvetory)
{
	CTreeCtrl& tree = GetTreeCtrl();
	if (ptree_helpers::IsKeyPressed(VK_CONTROL)
		&&(ptree_helpers::is_container_type(tree, hItem, QD_NAMED_OBJECT_INVENTORY)
		||ptree_helpers::is_container_type(tree, 
										tree.GetParentItem(hItem),
										QD_NAMED_OBJECT_INVENTORY))
		)
	{
		if (ptree_helpers::is_container_type(tree, hItem, QD_NAMED_OBJECT_INVENTORY))
			tree.SelectDropTarget(hItem);
		else if (ptree_helpers::is_container_type(tree, 
												tree.GetParentItem(hItem),
												QD_NAMED_OBJECT_INVENTORY))
		{
			tree.SelectDropTarget(tree.GetParentItem(hItem));
		}
		return DROPEFFECT_COPY;
	}
	return DROPEFFECT_NONE;
}

//! Бросили инвентори
BOOL SceneTreeView::oddInventory(qdInventory* inventory, 
								 DROPEFFECT dropEffect, 
								 CPoint const& point)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hDropTarget = tree.GetDropHilightItem();
	if (!ptree_helpers::IsKeyPressed(VK_CONTROL))
		return FALSE;
	std::auto_ptr<qdInventory> ptr(new qdInventory(*inventory));
	if (!ptr.get())
		return FALSE;
	ptr->set_name(get_unique_name(inventory->name(), m_ptrGameDisp->inventory_list()));

	if (!m_ptrGameDisp->add_inventory(ptr.get()))
		return FALSE;
	HTREEITEM const hNewItem = ptree_loading::InsertObject(tree, 
		hDropTarget, ptr.get(), II_INVENTORY_OBJ_NODE, TVI_LAST);
	if (!hNewItem)
	{
		m_ptrGameDisp->remove_inventory(ptr.get());
		return FALSE;
	}
	ptr.release();
	SetProjChange();
	tree.SelectDropTarget(NULL);
	return TRUE;
}
