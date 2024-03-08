/********************************************************************
created:	2002/09/11
created:	11:9:2002   16:12
filename: 	D:\Projects\Quest\QuestEditor\ptree_hlprs.hpp
file path:	D:\Projects\Quest\QuestEditor
file base:	ptree_hlprs
file ext:	hpp
Powerd by:	�����

  purpose:	��������������� ������� ��� ������ � �������
*********************************************************************/
#ifndef _PTREE_HELPERS_HPP_
#define _PTREE_HELPERS_HPP_

#include <utility>
#include <vector>
#include "obj_type_detectors.h"
#include "ptree_consts.h"
class qdGameObjectState;
namespace ptree_helpers{

//! ������� ���� �� ���������� ������ � ������������� ����
HTREEITEM FindTreeItemByText(CTreeCtrl& tree, HTREEITEM hPar, LPCTSTR strText);
//! ������� ��� ������� ����
void DeleteSubItems(CTreeCtrl& tree, HTREEITEM hItem);
//! ������� ��� ������� ����, �������� ���� ���������
void DeleteSubItemsExlude(CTreeCtrl& tree, HTREEITEM hPar, HTREEITEM hExcl);
//! ��������� �� ���� ������� ��� ����� ����� � ������
HTREEITEM IsObjUnderScene(CTreeCtrl& tree, HTREEITEM hItem);
//! ��������� �� ���� ������� ��� ����� "���������" � ������
HTREEITEM IsItemUnderInterface(CTreeCtrl& tree, HTREEITEM hItem);
//! ��������� �� ���� ��� ����� �������� ������������ ��������
HTREEITEM IsUnderAnimatedCatalog(CTreeCtrl& tree, HTREEITEM hItem);
//! ��������� �� ���� ��� ����� ����������
HTREEITEM IsUnderMovingCatalog(CTreeCtrl& tree, HTREEITEM hItem);


//! ���������� ��������� � ���������� ���� ���� ����������(�����, gamedispatcher)
HTREEITEM GetDispatcherNode(CTreeCtrl& tree, HTREEITEM hItem);
//! ���������� ��� ����-�������� �������� ����������� ����
CString GetFolderNodeName(qdNamedObjectType __t);
//! ���������� ���� �������� ��� ���������. ���� ��� ��� ��� - �������
HTREEITEM GetStateCatalog(CTreeCtrl& tree, HTREEITEM hObjItem, int type, int obj_type);

//! ���������� ������������� ������� ����
DWORD GetCorrspondingMenuResID(bool bIsCont, qdNamedObjectType type, 
															 bool bIsCopyOfGlobal);
//! ���������� ������������� ������� ���� ��� ������������ ��������
DWORD GetCorrspondingMenuResID(const qdInterfaceObjectBase* pobj);
//! ���������� ����� �������� ��� ���� ������������� �������
int SelAnimObjPic(const qdGameObject* pa);
//! ���������� ����� �������� ��� ���� ���������
int SelStatePic(const qdGameObjectState* ps, const qdGameObjectAnimated* pobj);
//! ���������� ����� �������� ��� ���� ������������ �������
int SelStaticObjPic(const qdGameObject* ps);
//! ���������� ����� �������� ��� ���� ����
int SelGZonePic(qdGridZone const* pgz);
//!���������� ����� �������� ��� ���� �������
int SelPic4MouseState(const qdGameObjectMouse* pmouse, const qdGameObjectState* pstate);

//! ���������� �����, ��� ������� ��������� ����
qdGameScene* GetSceneByItem(CTreeCtrl& tree, HTREEITEM hItem);

//! ������� ���� �� ���� ������� � ��������� ���� � ������ ��� ������. �������
std::auto_ptr<CStringArray>	CreateTreePath(const qdInterfaceObjectBase* pobj);
//! ������� ���� �� ���� ������� � ��������� ���� � ������ ��� ��������. ������
std::auto_ptr<CStringArray>	CreateTreePath(const qdNamedObject* pobj);

//! ����� �� ��������� ���� qdGameObjectState::QD_OBJ_STATE_FLAG_GLOBAL_OWNER
bool HasGlobalFlag(const qdNamedObject* ps);

//! �������� �� ��������� ��������� ���������� ��� �������
bool IsGlobalState4Obj(const qdGameObjectState* pstate,
							const qdGameObjectAnimated* pobj);

//! ���������� ������ ��������� � ������ ���������
int get_state_index(const qdGameObjectStateVector& v, const qdGameObjectState*ps);
//! �������� �� ���� ����� "���������"
bool is_interface_root(CTreeCtrl& tree, HTREEITEM hItem);

//! ������ �� �������
inline bool IsKeyPressed(int keyID){
	return (GetAsyncKeyState(keyID)>>15 != 0);
}

//! �������� dwData ��������� �� ������ ��� ��� ���� � ������
inline bool is_obj(DWORD_PTR dwData){
	return (dwData > QE_FIRST);
}	

//! �������� �� ������ ����������
inline bool IsGlobalObj(const qdNamedObjectBase* ptrObj){
	if (const qdNamedObject* pno = dynamic_cast<const qdNamedObject*>(ptrObj))
		return (!pno->owner()||
		pno->owner()->named_object_type() == QD_NAMED_OBJECT_DISPATCHER);
	return false;
}

//! �������� �� ��������� �����
bool is_dlg_state(const qdNamedObject* p);

//! �������� �� ��������� �����������
bool is_state_static(const qdGameObjectState* pstate);

//! �������� �� ������ �����-���� ������. ��������.
std::pair<qdNamedObjectBase*, bool> is_any_interf_obj(DWORD_PTR data);

//! �������� �� ���� ������������ �������
bool is_interface_screen(CTreeCtrl& tree, HTREEITEM hItem);
//! �������� �� ���� ������ ���������� ����
bool is_node_type(CTreeCtrl& tree, HTREEITEM hItem, DWORD dwType);

//! �������� �� ���� ����� ���������
inline bool is_state_node(CTreeCtrl& tree, HTREEITEM hItem){
	return is_node_type(tree, hItem, QD_NAMED_OBJECT_OBJ_STATE);
}

//! �������� �� ����-������� ��������� �������� ���������� ����
inline bool is_container_type(CTreeCtrl& tree, HTREEITEM hItem, DWORD dwType){
	DWORD dwData = tree.GetItemData(hItem);
	if(is_obj(dwData))
		return false;
	return (dwData == dwType);
}

////! �������� �� ���� ����� �����
inline bool is_scene(CTreeCtrl& tree, HTREEITEM hItem){
	return is_node_type(tree, hItem, QD_NAMED_OBJECT_SCENE);
}
//! �������� �� ���� ����� ������� ���������
inline bool is_trigger_item(CTreeCtrl& tree, HTREEITEM hItem){
	return is_node_type(tree, hItem, QD_NAMED_OBJECT_TRIGGER_CHAIN);
}
//! �������� �� ���� ����� "�����"
inline bool is_net(CTreeCtrl& tree, HTREEITEM hItem){
	return is_container_type(tree, hItem, QD_NAMED_OBJECT_GRID_ZONE);
}

//! �������� �� ���� ����� ��������
inline bool is_minigame_item(CTreeCtrl& tree, HTREEITEM hItem){
	return is_node_type(tree, hItem, QD_NAMED_OBJECT_MINIGAME);
}

//! �������� �� ���� ��������� ���������� ���������
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

//! �������� �� ���� ����� �� ��������� ���������
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

//! �������� �� ��������� ���������� ����������
bool IsMovementState(const qdGameObjectState* pno);

//! //! �������� �� ���� ����� ����������� ���������
inline bool IsMovementState(CTreeCtrl& tree, HTREEITEM hItem)
{
	return IsMovementState(
		reinterpret_cast<qdGameObjectState*>(tree.GetItemData(hItem)));
}
//! �������� �� ���� ����� ����
inline bool is_gz_state_node(CTreeCtrl& tree, HTREEITEM hItem){
	ASSERT(is_obj(tree.GetItemData(hItem)));
	return is_node_type(tree, hItem, QD_NAMED_OBJECT_GRID_ZONE_STATE);
}

//! �������� �� ���� ������������ "���������� ����", "������������ �������" � �.�.
bool IsContainerObj(CTreeCtrl& tree, HTREEITEM hItem);
//! �������� �� ������ ������ ��� ���������� �����������
bool is_dispacher(qdNamedObject* pno);

//! �������-������ �� �����������
BOOL cfALL(qdGameObject*);

//! ���� �� ���������� ������ � ������ \a name
inline qdGameObject* IsCopyOfGlobal(qdGameDispatcher* pd, LPCTSTR name){
	return pd->get_global_object(name);
}

//! ������� ����� ������� ������, �� ���������� �������
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

//! ���� �� � ������� ���������� ���������
bool HasWalkState(const qdGameObjectAnimated* pobj);

//! ������� � ������ ������� �������, � ��������� ������
qdTriggerChain* findTriggerChainByName(qdTriggerChainList const& lst, LPCSTR name);

}//namespace ptree_helpers

#endif//_PTREE_HELPERS_HPP_