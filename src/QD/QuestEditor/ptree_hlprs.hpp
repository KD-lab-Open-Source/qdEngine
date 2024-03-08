/********************************************************************
created:	2002/09/11
created:	11:9:2002   16:12
filename: 	D:\Projects\Quest\QuestEditor\ptree_hlprs.hpp
file path:	D:\Projects\Quest\QuestEditor
file base:	ptree_hlprs
file ext:	hpp
Powerd by:	Илюха

  purpose:	Вспомогательные функции для работы с деревом
*********************************************************************/
#ifndef _PTREE_HELPERS_HPP_
#define _PTREE_HELPERS_HPP_

#include <utility>
#include <vector>
#include "obj_type_detectors.h"
#include "ptree_consts.h"
class qdGameObjectState;
namespace ptree_helpers{

//! Находит узел по указанному тексту и родительскому узлу
HTREEITEM FindTreeItemByText(CTreeCtrl& tree, HTREEITEM hPar, LPCTSTR strText);
//! Удаляет все подузлы узла
void DeleteSubItems(CTreeCtrl& tree, HTREEITEM hItem);
//! Удаляет все подузлы узла, исключая один указанный
void DeleteSubItemsExlude(CTreeCtrl& tree, HTREEITEM hPar, HTREEITEM hExcl);
//! Находится ли узел объекта под узлом сцены в дереве
HTREEITEM IsObjUnderScene(CTreeCtrl& tree, HTREEITEM hItem);
//! Находится ли узел объекта под узлом "Интерфейс" в дереве
HTREEITEM IsItemUnderInterface(CTreeCtrl& tree, HTREEITEM hItem);
//! Находится ли узел под узлом каталога динамических объектов
HTREEITEM IsUnderAnimatedCatalog(CTreeCtrl& tree, HTREEITEM hItem);
//! Находится ли узел под узлом персонажей
HTREEITEM IsUnderMovingCatalog(CTreeCtrl& tree, HTREEITEM hItem);


//! Возвращает ближайщий к указанному узлу узел диспетчера(Сцена, gamedispatcher)
HTREEITEM GetDispatcherNode(CTreeCtrl& tree, HTREEITEM hItem);
//! Возвращает имя узла-каталога объектов указнанного типа
CString GetFolderNodeName(qdNamedObjectType __t);
//! Возвращает узел каталога для состояний. Если его еще нет - создает
HTREEITEM GetStateCatalog(CTreeCtrl& tree, HTREEITEM hObjItem, int type, int obj_type);

//! Возвращает идентификатор ресурса меню
DWORD GetCorrspondingMenuResID(bool bIsCont, qdNamedObjectType type, 
															 bool bIsCopyOfGlobal);
//! Возвращает идентификатор ресурса меню для интерфейсных объектов
DWORD GetCorrspondingMenuResID(const qdInterfaceObjectBase* pobj);
//! Возвращает номер картинки для узла динамического объекта
int SelAnimObjPic(const qdGameObject* pa);
//! Возвращает номер картинки для узла состояния
int SelStatePic(const qdGameObjectState* ps, const qdGameObjectAnimated* pobj);
//! Возвращает номер картинки для узла статического объекта
int SelStaticObjPic(const qdGameObject* ps);
//! Возвращает номер картинки для узла зоны
int SelGZonePic(qdGridZone const* pgz);
//!Возвращает номер картинки для узла курсора
int SelPic4MouseState(const qdGameObjectMouse* pmouse, const qdGameObjectState* pstate);

//! Возвращает сцену, под которой находится узел
qdGameScene* GetSceneByItem(CTreeCtrl& tree, HTREEITEM hItem);

//! Создает путь от узла объекта к корневому узлу в дереве для интерф. объекта
std::auto_ptr<CStringArray>	CreateTreePath(const qdInterfaceObjectBase* pobj);
//! Создает путь от узла объекта к корневому узлу в дереве для неинтерф. объета
std::auto_ptr<CStringArray>	CreateTreePath(const qdNamedObject* pobj);

//! Имеет ли состояние флаг qdGameObjectState::QD_OBJ_STATE_FLAG_GLOBAL_OWNER
bool HasGlobalFlag(const qdNamedObject* ps);

//! Является ли указанное состояние глобальным для объекта
bool IsGlobalState4Obj(const qdGameObjectState* pstate,
							const qdGameObjectAnimated* pobj);

//! Возвращает индекс состояния в списке состояний
int get_state_index(const qdGameObjectStateVector& v, const qdGameObjectState*ps);
//! Является ли узел узлом "Интерфейс"
bool is_interface_root(CTreeCtrl& tree, HTREEITEM hItem);

//! Нажата ли клавиша
inline bool IsKeyPressed(int keyID){
	return (GetAsyncKeyState(keyID)>>15 != 0);
}

//! Содержит dwData указатель на объект или тип узла в дереве
inline bool is_obj(DWORD_PTR dwData){
	return (dwData > QE_FIRST);
}	

//! Является ли объект глобальным
inline bool IsGlobalObj(const qdNamedObjectBase* ptrObj){
	if (const qdNamedObject* pno = dynamic_cast<const qdNamedObject*>(ptrObj))
		return (!pno->owner()||
		pno->owner()->named_object_type() == QD_NAMED_OBJECT_DISPATCHER);
	return false;
}

//! Содержит ли состояние текст
bool is_dlg_state(const qdNamedObject* p);

//! Является ли состояние статическим
bool is_state_static(const qdGameObjectState* pstate);

//! Является ли объект каким-либо интерф. объектом.
std::pair<qdNamedObjectBase*, bool> is_any_interf_obj(DWORD_PTR data);

//! Является ли узел интерфейсным экраном
bool is_interface_screen(CTreeCtrl& tree, HTREEITEM hItem);
//! Содержит ли узел объект указанного типа
bool is_node_type(CTreeCtrl& tree, HTREEITEM hItem, DWORD dwType);

//! Является ли узел узлом состояния
inline bool is_state_node(CTreeCtrl& tree, HTREEITEM hItem){
	return is_node_type(tree, hItem, QD_NAMED_OBJECT_OBJ_STATE);
}

//! Является ли узел-каталог каталогом объектов указанного типа
inline bool is_container_type(CTreeCtrl& tree, HTREEITEM hItem, DWORD dwType){
	DWORD dwData = tree.GetItemData(hItem);
	if(is_obj(dwData))
		return false;
	return (dwData == dwType);
}

////! Является ли узел узлом сцены
inline bool is_scene(CTreeCtrl& tree, HTREEITEM hItem){
	return is_node_type(tree, hItem, QD_NAMED_OBJECT_SCENE);
}
//! Является ли узел узлом цепочки триггеров
inline bool is_trigger_item(CTreeCtrl& tree, HTREEITEM hItem){
	return is_node_type(tree, hItem, QD_NAMED_OBJECT_TRIGGER_CHAIN);
}
//! Является ли узел узлом "Сетка"
inline bool is_net(CTreeCtrl& tree, HTREEITEM hItem){
	return is_container_type(tree, hItem, QD_NAMED_OBJECT_GRID_ZONE);
}

//! Является ли узел узлом миниигры
inline bool is_minigame_item(CTreeCtrl& tree, HTREEITEM hItem){
	return is_node_type(tree, hItem, QD_NAMED_OBJECT_MINIGAME);
}

//! Является ли узел каталогом походочных состояний
inline bool IsWalkStateCatalog(CTreeCtrl& tree, HTREEITEM hItem)
{
	DWORD dwData = tree.GetItemData(hItem);
	return (dwData == QE_MOVEMENT_STATE_CATALOG);
}

inline bool IsVideoCatalog(CTreeCtrl& tree, HTREEITEM hItem)
{
	DWORD dwData = tree.GetItemData(hItem);
	return (dwData == QD_NAMED_OBJECT_VIDEO);
}

//! Является ли узел одним из каталогов состояний
inline bool IsStateCatalog(CTreeCtrl& tree, HTREEITEM hItem)
{
	DWORD dwData = tree.GetItemData(hItem);
	switch(dwData) {
	case QE_STATE_CATALOG:
	case QE_MOVEMENT_STATE_CATALOG:
		return true;
	}
	return false;
}

//! Является ли состояние походочным состоянием
bool IsMovementState(const qdGameObjectState* pno);

//! //! Является ли узел узлом походочного состояния
inline bool IsMovementState(CTreeCtrl& tree, HTREEITEM hItem)
{
	return IsMovementState(
		reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hItem)));
}
//! Является ли узел узлом зоны
inline bool is_gz_state_node(CTreeCtrl& tree, HTREEITEM hItem){
	ASSERT(is_obj(tree.GetItemData(hItem)));
	return is_node_type(tree, hItem, QD_NAMED_OBJECT_GRID_ZONE_STATE);
}

//! Является ли узел объединяющим "Глобальный узел", "Динамические объекты" и т.д.
bool IsContainerObj(CTreeCtrl& tree, HTREEITEM hItem);
//! Является ли объект сценой или глобальным диспетчером
bool is_dispacher(qdNamedObject* pno);

//! Функция-фильтр не отсеивающая
BOOL cfALL(qdGameObject*);

//! Есть ли глобальный объект с именем \a name
inline qdGameObject* IsCopyOfGlobal(qdGameDispatcher* pd, LPCTSTR name){
	return pd->get_global_object(name);
}

//! Находит самый дальний объект, из подходящих фильтру
template <class _TEST>
qdGameObject* GetFarthestGO(qdGameScene* pscene, _TEST f)
{
	float fD = - 100000.f;
	qdGameObject* res = 0;
	qdCamera* pcam = pscene->get_camera();
	
	const qdGameObjectList& lst = pscene->object_list();
	qdGameObjectList::const_iterator _i = lst.begin(), _E = lst.end();
	while ((_i = std::find_if(_i, _E, f)) != _E) {
		qdGameObject* p = *_i;
		Vect3f v = pcam->global2camera_coord(p->R());
		if (v.z > fD) {
			fD = v.z;
			res	= p;
		}
		++_i;
	}
	return res;
}

//! Есть ли у объекта походочные состояния
bool HasWalkState(const qdGameObjectAnimated* pobj);

//! Находит в списке цепочек цепочку, с указанным именем
qdTriggerChain* findTriggerChainByName(qdTriggerChainList const& lst, LPCSTR name);

}//namespace ptree_helpers

#endif//_PTREE_HELPERS_HPP_