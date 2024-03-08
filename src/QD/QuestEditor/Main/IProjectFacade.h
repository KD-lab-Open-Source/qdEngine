#pragma once

class qdInterfaceScreen;
class qdInterfaceBackground;
class qdInterfaceButton;
class qdInterfaceSlider;
class qdInterfaceSave;
class qdInterfaceCounter;
class qdInterfaceElementState;
class qdInterfaceTextWindow;
class qdGameEnd;

class qdGameObjectStateStatic;
class qdGameObjectStateWalk;
class qdGameObjectState;
class qdGameObjectStateMask;

class qdGameObjectAnimated;
class qdGameObjectMoving;
class qdGameObjectStatic;

class qdGameScene;
class qdGameDispatcher;

class qdVideo;
class qdGridZone;

class CTreeListItem;
interface ITreeNode;

//! ������ ��������� �������������� ���������
enum eEndEditStateMode {
	EESTM_CANCEL,///< ��������� ��� ���������� ������
	EESTM_OK///< ��������� � ����������� ������
};

interface IProjectFacade
{
	virtual qdGameDispatcher* getGameDispatcher() const = 0;

	//! ��������� �������������� ������� ������� ��������� 
	virtual qdGameObjectStateStatic* AddEmptyStaticState(qdGameObjectAnimated* poa) = 0;
	//! ��������� ������� ��������� ����� ���������
	virtual qdGameObjectStateStatic* InsertEmptyStaticState(qdGameObjectAnimated* poa, 
										int iBefore) = 0;

	//! ��������� ��������� ���������� ��������� 
	virtual qdGameObjectStateWalk* AddEmptyMovementState(qdGameObjectMoving* pom) = 0;
	//! ��������� ���������� ��������� ����� ���������
	virtual qdGameObjectStateWalk* InsertEmptyMovementState(qdGameObjectMoving* pom, 
																int iBefore) = 0;

	//! ��������� � ����� � � ������ ������������� ������
	virtual qdGameObjectAnimated* AddAnimated(qdGameScene* ptrScene, 
									CString const& strName, bool bShow) = 0;

	//! ��������� � ����� � � ������ ��������
	virtual qdGameObjectMoving* AddPersonage(qdGameScene* ptrScene, 
											CString &strName, bool bShow) = 0;
	//! ��������� � ������ � � ������ ���������� ��������
	virtual qdGameObjectMoving* AddPersonage(qdGameDispatcher* ptrDisp,
											CString &strName, bool bShow) = 0;
	//! ��������� � ����� � � ������ ����������� ������
	virtual qdGameObjectStatic* AddStatic(qdGameScene* ptrScene, 
											CString& strName, bool bShowInTree) = 0;
	//! ��������� � ������ � � ������ �����
	virtual qdGameScene* AddScene(CString &strName, bool bShowInTree) = 0;

	//! ������� ������ �� ������ � �� �������
	virtual bool DeleteObject(class qdNamedObjectBase* ptrObj, bool bAsk) = 0;

	//! ��������� �� ������ � ��������� � ������ ������ ��������
	virtual void UpdateCatalogsInScenes(qdNamedObjectType type) = 0;

	//! ��������� ������� ������������� �������� � ��������� �����
	virtual void UpdateObjCatalogInScene(qdNamedObjectType obj_type,
										qdGameScene const* pscene) = 0;


	//! ������� ���������
	virtual bool RemoveState(qdGameObjectState* ptrObj, bool bAsk) = 0;
	//! ������������� ��������� ������� � ������
	virtual void ReloadStates(qdGameObjectAnimated* pobj) = 0;

	//! ���������� ���� ��� �������
	virtual void ShowCorrespondingMenu(qdNamedObjectBase* pno) = 0;

	/************************************************************************/
	/* ����� ��� ��������� ���������                                        */
	/************************************************************************/
	//! ���������� �� �������� ������� �� ��������. ������ ��������� ����������� � ������
	virtual void ptOnRemoveObjFromTrigger(qdNamedObject* pno) = 0;
	//! ����� ���������� ������� � �������, ��������� ��� ����������� � ������
	virtual void OnAddObjToTrigger(qdNamedObject* pno) = 0;
	//! �������� ������ ������� ��� �������
	virtual bool ShowCondDlg(qdNamedObject* pobj) = 0;
	//! �������� ������ ������� ��� �������
	virtual bool ShowProperties(qdNamedObjectBase* pno) = 0;
	/************************************************************************/
	/* �������� � ������������ ��������                                     */
	/************************************************************************/

	//! ���������� �������� ���������
	virtual void EndStateStack(eEndEditStateMode mode) = 0;
	//! ���������� �������������� ��
	virtual bool EndEditCoordAnimation(eEndEditStateMode mode) = 0;
	//! ����������� �������������� ��������
	virtual void EndResizeState(eEndEditStateMode mode) = 0;

	//! ������� ����� ����� �� ���������
	virtual bool DeleteInventoryCellset(class qdInventory* pinv, int icellset) = 0;

	//! ������ ������� ����
	enum eRunMode{
		RM_NORMAL,		///< ��������� ������� �����
		RM_TRIGGER_TEST, ///< ��������� ����� ��� �������� ���������
		RM_RUN_SCRIPT,	///< ��������� ����, ��� ������ �� ��� ���� �������� �������
	};
	//! ��������� �����
	virtual void RunScene(const CString& strSceneName, eRunMode emode) = 0;

	//! �������� ������������ ���
	virtual qdInterfaceBackground* AddInterfBackgrnd(const CString& name, 
										qdInterfaceScreen* pscr) = 0;
	//! �������� ������������ �������
	virtual qdInterfaceSlider* AddInterfSldr(const CString& name, 
										qdInterfaceScreen* pscr) = 0;
	//! �������� ���� �������
	virtual qdInterfaceTextWindow* AddInterfTextWindow(const CString& name, 
										qdInterfaceScreen* pscr) = 0;
	//! �������� ������������ �������
	virtual qdInterfaceCounter* AddInterfCounter(const CString& name, 
										qdInterfaceScreen* pscr) = 0;
	//! �������� ������������ ������
	virtual qdInterfaceSave* AddInterfSave(const CString& name, 
										qdInterfaceScreen* pscr) = 0;
	//! �������� ��������� ������. ������
	virtual int AddInterfBtnState(const CString& name, qdInterfaceButton* pbtn) = 0;
	//! �������� ������������ ������
	virtual qdInterfaceButton* AddInterfBtn(const CString& name, 
										qdInterfaceScreen* pscr) = 0;
	//! �������� �������� �����
	virtual qdGameEnd* AddGameEnd(ITreeNode* parent, CString const&, CString const&) = 0;

	//! ����� � �������� ������ � ������
	virtual bool ShowObjectInTree(const qdNamedObjectBase* p) = 0;

	//! ��������� ����� � ������
	virtual CTreeListItem* LoadScene(qdGameScene* pgs) = 0;

	//! �������� �������� �� �������� �������� �����
	virtual bool isSceneViewActive() const = 0;
	/// ������ �������� ���� ��������/����������
	virtual void setSceneViewActive(bool bActive) = 0;
	/// ���������� ������� ����������� �������
	virtual void SetModified(bool bModified = true) = 0;
	/// �������� ��������� �����
	virtual bool SelectScene(qdNamedObject* scene) = 0;
	/// ���������� ���� ���� ���� �� ����������� ���������
	virtual void CollapseSceneNodes(qdNamedObject const* exclude) = 0;
	/// ���������� ������ ��������� � �������� ������� ���������,
	/// ��� ���� �������� �������� � qdTriggerChain
	virtual void setActiveTriggerIcon(qdTriggerChain const* chain, BOOL bSet) = 0;
	/// �������� ��������� ������������ �����
	virtual bool selectInterfaceScreen(LPCTSTR screenName) = 0;

	/// ���������� �� ������ ��������� �������� �����
	virtual bool OnCompleteCreateZone(qdGridZone* zone) = 0;
	/// ���������� �� ������ ��������� �������� �����
	virtual bool OnEndStateMaskCreate(qdGameObjectStateMask* state) = 0;
};