/********************************************************************
	created:	2002/09/06
	created:	6:9:2002   11:23
	filename: 	D:\Projects\Quest\QuestEditor\ptree_loading.h
	file path:	D:\Projects\Quest\QuestEditor
	file base:	ptree_loading
	file ext:	h
	Powerd by:	�����
	
	purpose:	���������� ���������� �������, ����������� � ��������
*********************************************************************/
#ifndef _PTREE_LOADING_H_
#define _PTREE_LOADING_H_

namespace ptree_loading
{
//! ��������� �� �������� �������� ��� ����� � ������
bool LoadBitmaps(CImageList* ptrIL);
//! ��������� ����������� ��� ������ ���������.
bool LoadStateBitmaps(CImageList* pil);

//! ������� ������� ��������� ����
HTREEITEM CrtTypedNode(CTreeCtrl& wndTree, HTREEITEM hPar, DWORD id, DWORD type, 
						UINT uiImage, HTREEITEM hItemAfter = TVI_LAST);
//! ���������, ��� ������ � ��������
BOOL SetInTrigger(CTreeCtrl& tree, HTREEITEM hItem, BOOL bInTrg, int id);

//! �������� ���� ��������� ��� �������������� �������
bool aoLoadStates(CTreeCtrl& tree, HTREEITEM hObjItem, qdGameObjectAnimated* pobj);

//! �������� ��������� ��� ���������
bool moLoadStates(CTreeCtrl& tree, HTREEITEM hObjItem, qdGameObjectMoving* pobj);
				  
//! ��������� ��������� ��� �������
bool LoadStates(CTreeCtrl& wndTree, HTREEITEM hObjItem, qdGameObject* pgo);

//! ��������� ��������� � ������
HTREEITEM InsertState(CTreeCtrl& tree, HTREEITEM hPar, 
					  const qdGameObjectAnimated *pobj,
					  qdGameObjectState* ptrState, UINT uiImage, 
					  HTREEITEM hInsertAfter = TVI_LAST);

//! ��������� ����� ������ � ������.
HTREEITEM InsertObject(CTreeCtrl& tree, HTREEITEM hParent, const qdNamedObjectBase* po,
									 UINT uiImage, HTREEITEM hInsertAfter = TVI_LAST);

//! ��������� ������� ����������� �� qdNamedObject
HTREEITEM InsertNamedObject(CTreeCtrl& wndTree, HTREEITEM hPar, 
			const qdNamedObject* pgo, UINT uiImage, HTREEITEM hInsertAfter = TVI_LAST);

//! ������������� ��������� ��� ������������� �������
void aoReloadStates(CTreeCtrl& tree, HTREEITEM hObjItem, qdGameObjectAnimated* pobj);
//! ������������� ��������� ��� ���������
void moReloadStates(CTreeCtrl& tree, HTREEITEM hObjItem,qdGameObjectMoving* pobj);

//! ������������� ���������� ��������� ��� ���������
void moReloadWalkStates(CTreeCtrl& tree, HTREEITEM hCat, const qdGameObjectMoving* pobj);
//! ������������� ������������ ��������� ��� ���������
void moReloadNotWalkStates(CTreeCtrl& tree, HTREEITEM hCat,
						   const qdGameObjectMoving* pobj);

//! ������������� ������ ��������� � �������, ���������� ����
void ReloadStateCatalog(CTreeCtrl& tree, HTREEITEM hCat,
						qdGameObjectAnimated* pobj, bool bIsWalk);
//! ������������� ������������ ��������� �������
void ReloadOwnStates(CTreeCtrl& tree, HTREEITEM hObjItem, qdGameObjectAnimated* pobj);

//! ������������� ���������� ��������� �������
void ReloadGlobalStates(CTreeCtrl& tree, HTREEITEM hObjItem, qdGameObjectAnimated* pobj);

//! ������������� ��������� �������
void ReloadStates(CTreeCtrl& tree, HTREEITEM hObjItem,qdGameObjectAnimated* pobj);

//! �������, ������������ �������� �� ��������� ������������ ����������
class own_walk{
	qdGameObjectAnimated const* const powner;
public:
	own_walk(const qdGameObjectAnimated* po):powner(po){}
	bool operator()(qdGameObjectState const* ps) const;
};

//! �������, ������������ �������� �� ��������� ������������ ������������
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

