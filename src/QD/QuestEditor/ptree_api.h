/********************************************************************
	created:	2002/08/15
	created:	15:8:2002   13:39
	filename: 	D:\Projects\Quest\QuestEditor\ptree_helpers.h
	file path:	D:\Projects\Quest\QuestEditor
	file base:	ptree_helpers
	file ext:	h
	author:		�����
	
	purpose:	����� ���������� ������� ��� ������ � �������
*********************************************************************/
#ifndef __PTREE_API_H__
#define __PTREE_API_H__

#include "Main/IProjectFacade.h"

class SceneTreeView;
class qdInterfaceScreen;
class qdInterfaceBackground;
class qdInterfaceButton;
class qdInterfaceSlider;
class qdInterfaceSave;
class qdInterfaceElementState;
class qdInterfaceTextWindow;
class qdGameEnd;

class qdGameObjectStateStatic;
class qdGameObjectStateWalk;
class qdGameObjectState;

class qdGameObjectAnimated;
class qdGameObjectMoving;
class qdGameObjectStatic;

class qdGameScene;
class qdGameDispatcher;

interface ITreeNode;
class CTreeListItem;
namespace ptree_api
{
//! ��������� �������������� ������� ������� ��������� 
qdGameObjectStateStatic* AddEmptyStaticState(qdGameObjectAnimated* poa);
//! ��������� ������� ��������� ����� ���������
qdGameObjectStateStatic* InsertEmptyStaticState(qdGameObjectAnimated* poa, int iBefore);

//! ��������� ��������� ���������� ��������� 
qdGameObjectStateWalk* AddEmptyMovementState(qdGameObjectMoving* pom);
//! ��������� ���������� ��������� ����� ���������
qdGameObjectStateWalk* InsertEmptyMovementState(qdGameObjectMoving* pom, int iBefore);

//! ��������� � ����� � � ������ ������������� ������
qdGameObjectAnimated* AddAnimated(qdGameScene* ptrScene, CString& strName, bool bShow);
//! ��������� � ����� � � ������ ��������
qdGameObjectMoving* AddPersonage(qdGameScene* ptrScene, CString &strName, bool bShow);
//! ��������� � ������ � � ������ ���������� ��������
qdGameObjectMoving* AddPersonage(qdGameDispatcher* ptrDisp,CString &strName, bool bShow);
//! ��������� � ����� � � ������ ����������� ������
qdGameObjectStatic* AddStatic(qdGameScene* ptrScene, CString& strName, bool bShowInTree);
//! ��������� � ������ � � ������ �����
bool AddScene(CString &strName, bool bShowInTree);

//! ������� ������ �� ������ � �� �������
bool DeleteObject(class qdNamedObjectBase* ptrObj, bool bAsk);


//! ��������� �� ������ � ��������� � ������ ������ ��������
void UpdateCatalogsInScenes(qdNamedObjectType type);
//! ��������� ������� ������������� �������� � ��������� �����
void UpdateObjCatalogInScene(qdNamedObjectType obj_type,qdGameScene const* pscene);


//! ������� ���������
bool RemoveState(qdGameObjectState* ptrObj, bool bAsk);
//! ������������� ��������� ������� � ������
void ReloadStates(qdGameObjectAnimated* pobj);

#ifndef USE_NEW_PROJ
//! ��������� �� ����� �������� ������
SceneTreeView* GetProjTree();
#endif // USE_NEW_PROJ
//! ���������� ���� ��� �������
void ShowCorrespondingMenu(qdNamedObjectBase* pno);

/************************************************************************/
/* ����� ��� ��������� ���������                                        */
/************************************************************************/
//! ���������� �� �������� ������� �� ��������. ������ ��������� ����������� � ������
void ptOnRemoveObjFromTrigger(qdNamedObject* pno);
//! ����� ���������� ������� � �������, ��������� ��� ����������� � ������
void OnAddObjToTrigger(qdNamedObject* pno);
//! �������� ������ ������� ��� �������
bool ShowCondDlg(qdNamedObject* pobj);
//! �������� ������ ������� ��� �������
bool ShowProperties(qdNamedObjectBase* pno);
/************************************************************************/
/* �������� � ������������ ��������                                     */
/************************************************************************/
//! ������ ��������� �������������� ���������
//enum eEndEditStateMode {
//	EESTM_CANCEL,///< ��������� ��� ���������� ������
//	EESTM_OK///< ��������� � ����������� ������
//};
using ::eEndEditStateMode;
using ::EESTM_CANCEL;
using ::EESTM_OK;

//! ���������� �������� ���������
void EndStateStack(eEndEditStateMode mode);
//! ���������� �������������� ��
bool EndEditCoordAnimationMode(eEndEditStateMode mode);
//! ����������� �������������� ��������
void EndResizeState(eEndEditStateMode mode);

//! ������� ����� ����� �� ���������
bool DeleteInventoryCellset(class qdInventory* pinv, int icellset);
	
//! ������ ������� ����
//enum eRunMode{
//		RM_NORMAL,		///< ��������� ������� �����
//		RM_TRIGGER_TEST, ///< ��������� ����� ��� �������� ���������
//		RM_RUN_SCRIPT,	///< ��������� ����, ��� ������ �� ��� ���� �������� �������
//};
using IProjectFacade::eRunMode;
using IProjectFacade::RM_NORMAL;
using IProjectFacade::RM_TRIGGER_TEST;
using IProjectFacade::RM_RUN_SCRIPT;

//! ��������� �����
void RunScene(const CString& strSceneName, eRunMode emode);

//! ���������� ��������� �������
class qdGameDispatcher* GetProjGD();

//! �������� ������������ ���
qdInterfaceBackground* AddInterfBackgrnd(const CString& name, qdInterfaceScreen* pscr);
//! �������� ������������ �������
qdInterfaceSlider* AddInterfSldr(const CString& name, qdInterfaceScreen* pscr);
//! �������� ���� �������
qdInterfaceTextWindow* AddInterfTextWindow(const CString& name, qdInterfaceScreen* pscr);
//! �������� ������������ ������
qdInterfaceSave* AddInterfSave(const CString& name, qdInterfaceScreen* pscr);
//! �������� ��������� ������. ������
int AddInterfBtnState(const CString& name, qdInterfaceButton* pbtn);
//! �������� ������������ ������
qdInterfaceButton* AddInterfBtn(const CString& name, qdInterfaceScreen* pscr);
//! �������� �������� �����
#ifdef USE_NEW_PROJ
qdGameEnd* AddGameEnd(ITreeNode*, CString const&, CString const&);
#else
qdGameEnd* AddGameEnd(HTREEITEM, CString const&, CString const&);
#endif // USE_NEW_PROJ

//! ����� � �������� ������ � ������
bool ShowObjectInTree(const qdNamedObjectBase* p);

//! ��������� ����� � ������
#ifdef USE_NEW_PROJ
__declspec(deprecated) CTreeListItem* LoadScene(qdGameScene* pgs);
#else
HTREEITEM LoadScene(qdGameScene* pgs);
#endif // USE_NEW_PROJ
}//namespace ptree_api

#endif //__PTREE_API_H__