/********************************************************************
	created:	2002/09/06
	created:	6:9:2002   11:23
	filename: 	D:\Projects\Quest\QuestEditor\ptree_loading.h
	file path:	D:\Projects\Quest\QuestEditor
	file base:	ptree_loading
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	Декларация глобальных функций, участвующих в загрузке
*********************************************************************/
#ifndef _PTREE_LOADING_H_
#define _PTREE_LOADING_H_

namespace ptree_loading
{
//! Загружает из ресурсов картинки для узлов в дереве
bool LoadBitmaps(CImageList* ptrIL);
//! Загружает изображения для иконок состояния.
bool LoadStateBitmaps(CImageList* pil);

//! Создаем каталог заданного типа
HTREEITEM CrtTypedNode(CTreeCtrl& wndTree, HTREEITEM hPar, DWORD id, DWORD type, 
						UINT uiImage, HTREEITEM hItemAfter = TVI_LAST);
//! Обозначам, что объект в триггере
BOOL SetInTrigger(CTreeCtrl& tree, HTREEITEM hItem, BOOL bInTrg, int id);

//! Загрузка всех состояний для анимированного объекта
bool aoLoadStates(CTreeCtrl& tree, HTREEITEM hObjItem, qdGameObjectAnimated* pobj);

//! Загрузка состояний для персонажа
bool moLoadStates(CTreeCtrl& tree, HTREEITEM hObjItem, qdGameObjectMoving* pobj);
				  
//! Загружаем состояния для объекта
bool LoadStates(CTreeCtrl& wndTree, HTREEITEM hObjItem, qdGameObject* pgo);

//! Вставляет состояние в дерево
HTREEITEM InsertState(CTreeCtrl& tree, HTREEITEM hPar, 
					  const qdGameObjectAnimated *pobj,
					  qdGameObjectState* ptrState, UINT uiImage, 
					  HTREEITEM hInsertAfter = TVI_LAST);

//! Вставляет любой объект в дерево.
HTREEITEM InsertObject(CTreeCtrl& tree, HTREEITEM hParent, const qdNamedObjectBase* po,
									 UINT uiImage, HTREEITEM hInsertAfter = TVI_LAST);

//! Вставляет объекты порожденные от qdNamedObject
HTREEITEM InsertNamedObject(CTreeCtrl& wndTree, HTREEITEM hPar, 
			const qdNamedObject* pgo, UINT uiImage, HTREEITEM hInsertAfter = TVI_LAST);

//! Перезагружает состояния для динамического объекта
void aoReloadStates(CTreeCtrl& tree, HTREEITEM hObjItem, qdGameObjectAnimated* pobj);
//! Перезагружает состояния для персонажа
void moReloadStates(CTreeCtrl& tree, HTREEITEM hObjItem,qdGameObjectMoving* pobj);

//! Перезагружает походочные состояния для персонажа
void moReloadWalkStates(CTreeCtrl& tree, HTREEITEM hCat, const qdGameObjectMoving* pobj);
//! Перезагружает непоходочные состояния для персонажа
void moReloadNotWalkStates(CTreeCtrl& tree, HTREEITEM hCat,
						   const qdGameObjectMoving* pobj);

//! Перезагружает список состояний в каталог, указанного типа
void ReloadStateCatalog(CTreeCtrl& tree, HTREEITEM hCat,
						qdGameObjectAnimated* pobj, bool bIsWalk);
//! Перезагружает неглобальные состояния объекта
void ReloadOwnStates(CTreeCtrl& tree, HTREEITEM hObjItem, qdGameObjectAnimated* pobj);

//! Перезагружает глобальные состояния объекта
void ReloadGlobalStates(CTreeCtrl& tree, HTREEITEM hObjItem, qdGameObjectAnimated* pobj);

//! Перезагружает состояния объекта
void ReloadStates(CTreeCtrl& tree, HTREEITEM hObjItem,qdGameObjectAnimated* pobj);

//! функтор, определяющий является ли состояние неглобальным походочным
class own_walk{
	qdGameObjectAnimated const* const powner;
public:
	own_walk(const qdGameObjectAnimated* po):powner(po){}
	bool operator()(qdGameObjectState const* ps) const;
};

//! функтор, определяющий является ли состояние неглобальным непоходочным
class own_not_walk{
	qdGameObjectAnimated const* const powner;
public:
	own_not_walk(qdGameObjectAnimated const* po):powner(po){}
	
	bool operator()(qdGameObjectState const* ps) const;
};

}//namepsace ptree_loading

using ptree_loading::CrtTypedNode;
using ptree_loading::InsertNamedObject;

using ptree_loading::InsertState;
using ptree_loading::LoadStates;
using ptree_loading::ReloadStates;
using ptree_loading::ReloadGlobalStates;
using ptree_loading::ReloadOwnStates;
using ptree_loading::ReloadStateCatalog;

using ptree_loading::LoadStateBitmaps;

using ptree_loading::SetInTrigger;
using ptree_loading::LoadBitmaps;

#endif//_PTREE_LOADING_H_

