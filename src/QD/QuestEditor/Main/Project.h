#pragma once

#include <map>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "IViewMgr.h"
#include "Main/IProjectFacade.h"
#include "Main/ViewActivator.h"
#include "ScaleInterfaces.h"
#include "Object List/IObjectList.h"
#include "Stat Dependencies/filterobjectdependencies.h"
#include "IAppSettingsChangeListener.h"
#include "IAppSettingsUser.h"


interface ISceneViewFacade;
interface ITreeNodeFactory;
interface ITreeNode;
interface IScaleMgr;
interface ITriggerView;
interface IRequestingStateEdit;
interface IProjectSettings;

class StructureView;
class qdGameDispatcher;
class TreeLogic;
class TreeNodeFactory;
class GameObjectFactory;
class ProjectModifier;
// Project document

typedef std::list<qdNamedObject const*> NamedObjectList;


/*!
 �������� �����. �������� �� ��������/���������� �������. ����������/��������
 � ������ ��������
*/
class Project : public CDocument
				, public IProjectFacade
				, public IViewMgr
				, public IAppSettingsUser
				, public IAppSettingsChangeListener
{
	DECLARE_DYNCREATE(Project)

	friend class ViewActivator;
public:

	//! ��������� ���� �������� ��������� � ��������� ���������
	enum eFilterScheme{
		FS_NONE, ///< ��� ����������
		FS_ENTRY_FOR_DYN_OBJ, ///< ���������� ��������� ������������ ��������
		FS_ENTRY_FOR_MOUSE, ///< ���������� ��������� ����
		FS_ENTRY_FOR_PERS, ///< ���������� ��������� ����������
		FS_ENTRY_FOR_DIALOGS,///< ���������� ���������� ���������
	};

	Project();
	virtual ~Project();

	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
	void setSceneView(ISceneViewFacade* sceneView);
	ISceneViewFacade* getSceneView() const;

	void setTriggerEditorView(ITriggerView* teView);
	ITriggerView* getTriggerEditorView() const;

	void setStructureView(StructureView* structureView);
	StructureView* getStructureView() const;
	
	void setScaleMngr(IScaleMgr* mngr);
	IScaleMgr* getScaleMngr() const;

	TreeLogic& getTreeLogic() const;

	qdGameDispatcher* getGameDispatcher() const;

	ITreeNodeFactory& getTreeNodeFactory() const;

	IObjectList* getObjectList() const;
	void setObjectList(IObjectList* ol);

	void OnTimer(UINT nIDEvent);
public:
	//! ��������� �������������� ������� ������� ��������� 
	virtual qdGameObjectStateStatic* AddEmptyStaticState(qdGameObjectAnimated* poa);
	//! ��������� ������� ��������� ����� ���������
	virtual qdGameObjectStateStatic* InsertEmptyStaticState(qdGameObjectAnimated* poa, 
		int iBefore);

	//! ��������� ��������� ���������� ��������� 
	virtual qdGameObjectStateWalk* AddEmptyMovementState(qdGameObjectMoving* pom);
	//! ��������� ���������� ��������� ����� ���������
	virtual qdGameObjectStateWalk* InsertEmptyMovementState(qdGameObjectMoving* pom, 
		int iBefore);

	//! ��������� � ����� � � ������ ������������� ������
	virtual qdGameObjectAnimated* AddAnimated(qdGameScene* ptrScene, 
		CString const& strName, bool bShow);

	//! ��������� � ����� � � ������ ��������
	virtual qdGameObjectMoving* AddPersonage(qdGameScene* ptrScene, 
		CString &strName, bool bShow);
	//! ��������� � ������ � � ������ ���������� ��������
	virtual qdGameObjectMoving* AddPersonage(qdGameDispatcher* ptrDisp,
		CString &strName, bool bShow);
	//! ��������� � ����� � � ������ ����������� ������
	virtual qdGameObjectStatic* AddStatic(qdGameScene* ptrScene, 
		CString& strName, bool bShowInTree);
	//! ��������� � ������ � � ������ �����
	virtual qdGameScene* AddScene(CString &strName, bool bShowInTree);

	//! ������� ������ �� ������ � �� �������
	virtual bool DeleteObject(class qdNamedObjectBase* ptrObj, bool bAsk);

	//! ��������� �� ������ � ��������� � ������ ������ ��������
	virtual void UpdateCatalogsInScenes(qdNamedObjectType type);

	//! ��������� ������� ������������� �������� � ��������� �����
	virtual void UpdateObjCatalogInScene(qdNamedObjectType obj_type,
		qdGameScene const* pscene);


	//! ������� ���������
	virtual bool RemoveState(qdGameObjectState* ptrObj, bool bAsk);
	//! ������������� ��������� ������� � ������
	virtual void ReloadStates(qdGameObjectAnimated* pobj);

	//! ���������� ���� ��� �������
	virtual void ShowCorrespondingMenu(qdNamedObjectBase* pno);

	/************************************************************************/
	/* ����� ��� ��������� ���������                                        */
	/************************************************************************/
	//! ���������� �� �������� ������� �� ��������. ������ ��������� ����������� � ������
	virtual void ptOnRemoveObjFromTrigger(qdNamedObject* pno);
	//! ����� ���������� ������� � �������, ��������� ��� ����������� � ������
	virtual void OnAddObjToTrigger(qdNamedObject* pno);
	//! �������� ������ ������� ��� �������
	virtual bool ShowCondDlg(qdNamedObject* pobj);
	//! �������� ������ ������� ��� �������
	virtual bool ShowProperties(qdNamedObjectBase* pno);
	/************************************************************************/
	/* �������� � ������������ ��������                                     */
	/************************************************************************/
	//! ���������� �������� ���������
	virtual void EndStateStack(eEndEditStateMode mode);
	//! ���������� �������������� ��
	virtual bool EndEditCoordAnimation(eEndEditStateMode mode);
	//! ����������� �������������� ��������
	virtual void EndResizeState(eEndEditStateMode mode);

	//! ������� ����� ����� �� ���������
	virtual bool DeleteInventoryCellset(class qdInventory* pinv, int icellset);

	//! ��������� �����
	virtual void RunScene(const CString& strSceneName, eRunMode emode);

	//! �������� ������������ ���
	virtual qdInterfaceBackground* AddInterfBackgrnd(const CString& name, 
		qdInterfaceScreen* pscr);
	//! �������� ������������ �������
	virtual qdInterfaceSlider* AddInterfSldr(const CString& name, 
		qdInterfaceScreen* pscr);
	//! �������� ���� �������
	virtual qdInterfaceTextWindow* AddInterfTextWindow(const CString& name, 
		qdInterfaceScreen* pscr);
	//! �������� ������������ �������
	virtual qdInterfaceCounter* AddInterfCounter(const CString& name, 
		qdInterfaceScreen* pscr);
	//! �������� ������������ ������
	virtual qdInterfaceSave* AddInterfSave(const CString& name, 
		qdInterfaceScreen* pscr);
	//! �������� ��������� ������. ������
	virtual int AddInterfBtnState(const CString& name, qdInterfaceButton* pbtn);
	//! �������� ������������ ������
	virtual qdInterfaceButton* AddInterfBtn(const CString& name, 
		qdInterfaceScreen* pscr);
	//! �������� �������� �����
	virtual qdGameEnd* AddGameEnd(ITreeNode* parent, CString const&, CString const&);

	//! ����� � �������� ������ � ������
	virtual bool ShowObjectInTree(const qdNamedObjectBase* p);

	//! ��������� ����� � ������
	virtual CTreeListItem* LoadScene(qdGameScene* pgs);

	//! �������� �������� �� �������� �������� �����
	virtual bool isSceneViewActive() const;
	virtual void setSceneViewActive(bool bActive);

	virtual void SetModified(bool bModified);

	virtual bool SelectScene(qdNamedObject* scene);
	virtual void CollapseSceneNodes(qdNamedObject const* exclude);
	virtual void setActiveTriggerIcon(qdTriggerChain const* chain, BOOL bSet);
	virtual bool selectInterfaceScreen(LPCTSTR screenName);

	virtual bool OnCompleteCreateZone(qdGridZone* zone);
	virtual bool OnEndStateMaskCreate(qdGameObjectStateMask* state);

	void setInventoryToAllPersonages(LPCTSTR inventoryName);
	LRESULT OnMenuCommand(WPARAM wParam, LPARAM lParam);

	//! �������� ������������� ������ � ������������� ������
	bool copyAnimatedToAnimated(ITreeNode* dropTarget, 
		qdGameObjectAnimated const* source,
		qdGameScene* scene);
	//! �������� ������������� ������ � ��������
	bool copyAnimatedToMoving(ITreeNode* dropTarget, 
		qdGameObjectAnimated const* source,
		qdGameScene* scene);
	//! �������� �������� � ��������
	bool copyMovingToMoving(ITreeNode* dropTarget, 
		qdGameObjectMoving const* source,
		qdGameScene* scene);

	//! �������������� ����� � ProjectModifier::moveState
	qdGameObjectState* moveState(qdGameObjectAnimated* source, 
		qdGameObjectAnimated* target,
		qdGameObjectState* movedState, 
		ITreeNode* targetNode,
		ITreeNode* insertAfter,
		int insertBefore);

	//! �������������� ����� � ProjectModifier::copyState
	qdGameObjectState* copyState(qdGameObjectAnimated* source, 
		qdGameObjectAnimated* target,
		qdGameObjectState* copyState, 
		ITreeNode* targetNode,
		ITreeNode* insertAfter,
		int insertBefore);

	//! �������������� ����� � ProjectModifier::moveZone
	qdGridZone* moveZone(qdGameScene* scene, 
						qdGridZone* moved, 
						qdGridZone const* zoneBefore, 
						ITreeNode* insertAfter,
						ITreeNode* parent);

	//! �������������� ����� � ProjectModifier::moveVideo
	qdVideo* moveVideo(qdVideo* source, qdVideo const* before, 
					ITreeNode* insertAfter, ITreeNode* parent);
	//! �������������� ������ � ProjectModifier::copyInventory
	qdInventory* copyInventory(qdInventory const* source, ITreeNode* parent);

	eFilterScheme getTriggerElementFilterScheme() const;
	bool getShowTriggerElement() const;

	//! ���������� �����,��� ������� ��������� ����
	qdGameScene* getSceneByNode(ITreeNode const* node) const;

	//! ��������� ������ ������ �������
	void showNewProjWiz();


	virtual boost::shared_ptr<AppSettingsBasePage> getPage();
	virtual void SettingsChanged();

	void autoSave();
protected:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, 
							void* pExtra, 
							AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnProperies();
	afx_msg void onConditions();
	afx_msg void onEditTriggers();

	afx_msg void OnCrtAnimatedObj();
	afx_msg void OnCrtMovingObj();
	afx_msg void OnCrtStaticObj();
	afx_msg void OnCreateStatics();
	afx_msg void OnWaveToLocal();
	afx_msg void OnLocalToWave();
	afx_msg void OnCrtScene();
	afx_msg void OnAddInterfBackground();
	afx_msg void OnAddInterfBtnState();
	afx_msg void OnAddInterfSave();
	afx_msg void OnCreateIscreen();
	afx_msg void OnAddInterfSlider();
	afx_msg void OnAddInterfTextWindow();
	afx_msg void OnAddInterfCounter();
	afx_msg void OnAddIntrfButton();
	afx_msg void OnAddGameEnd();
	afx_msg void OnAddFont();
	afx_msg void OnCreateFont();
	afx_msg void OnAddVideo();
	afx_msg void OnCreateMinigame();
	afx_msg void OnCrtNewInventory();
	afx_msg void OnCreateCounter();
	afx_msg void OnCreateState();
	afx_msg void OnCrtTrigger();
	afx_msg void OnCrtAnimation();
	afx_msg void OnAnimationList();
	afx_msg void OnSoundList();
	afx_msg void OnEditSound();
	afx_msg void OnEditStateAnimation();
	afx_msg void OnMenuMovingsStops();
	afx_msg void OnGameSettings();
	afx_msg void OnManageCds();
	afx_msg void OnStatDependencies();
	afx_msg void OnAddCellSet();
	afx_msg void OnRemoveCellSet();
	afx_msg void OnAddMusicTrack();
	afx_msg void OnCreateMask();
	afx_msg void OnCrtGridZone();
	afx_msg void OnMusicTrackCycled();
	afx_msg void OnCrtAnimObjByMask();
	afx_msg void OnAdjustGameArea();
	afx_msg void OnRemoveEdges();
	afx_msg void OnRemoveEdgesForAllObjs();
	afx_msg void OnRemoveEdgesInStates();
	afx_msg void OnDisableMouse();
	afx_msg void OnSelectMovementType();
	afx_msg void OnStateDims();
	afx_msg void OnBtnAutoBnd();
	afx_msg void OnStateHasBound();
	afx_msg void OnMakeBoundAsFirst();
	afx_msg void OnStackState();
	afx_msg void OnCoordsAnimation();
	afx_msg void OnCancelRemovingEdges();
	afx_msg void OnPersDim();
	afx_msg void OnPersDir();
	afx_msg void OnNotControledPers();
	afx_msg void OnRunAnimationEditor();
	afx_msg void OnEditDialogs();
	afx_msg void OnLastNail();
	afx_msg void OnJoinProject();
	afx_msg void OnSelectScene();
	afx_msg void OnEditGaits();
	afx_msg void OnNetProperties();
	afx_msg void OnRunScene();
	afx_msg void OnRunScript();
	afx_msg void OnShowAllNetProjs();
	afx_msg void OnDrawAllBounds();
	afx_msg void OnToolbarDropDown(NMHDR* pNMHDR, LRESULT *plr);
	afx_msg void OnDelete();
	afx_msg void OnMgParameters();
	afx_msg void OnAppSettings();
	afx_msg void OnCollapseAll();
	afx_msg void OnExpandAll();

	afx_msg void onUpdateCmdUIScalePlus(CCmdUI* pCmdUI);
	afx_msg void onUpdateCmdUIScaleMinus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWaveToLocal(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLocalToWave(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePlayStopVideo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateStopVideo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCrtMovingObj(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNetProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCrtAnimObjs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCreateStatics(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCreateBack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditAnimation(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSound(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLastNail(CCmdUI* pCmdUI);
	afx_msg void OnUpdateJoinProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectScene(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditGaits(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRunScene(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRunScript(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowAllNetProjs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawAllBounds(CCmdUI* pCmdUI);

	afx_msg void OnTbtnEntryForDialogs();
	afx_msg void OnTbtnEntryForDynObj();
	afx_msg void OnTbtnEntryForMouse();
	afx_msg void OnTbtnEntryForPers();
	afx_msg void OnTbtnShowElement();
	afx_msg void OnUpdateTbtnEntryForDialogs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTbtnEntryForDynObj(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTbtnEntryForMouse(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTbtnEntryForPers(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTbtnShowElement(CCmdUI* pCmdUI);


	DECLARE_MESSAGE_MAP();
protected:

	void removeTmps();
	CString getTemporaryFileName();
	//! �������� ��� ���������� �����, ������� ������������ ��� ��������������
	void makeTempFileName();

	void reset();
	void ShowAnimation();

	void setInTriggerIcon(qdTriggerChain const* chain, int command);

	//! ���������� ����� ������ ���� ��������� ��������� � ��������� �����.
	bool onChangingViews(bool bIsSVActiveOld, bool bIsSVActiveNew);
	//! ���������� ����� ����� ���� ��������� ��������� � ��������� �����.
	void onChangeViews(bool bIsSVActiveOld, bool bIsSVActiveNew);

	//! ����� IViewMgr
	virtual BOOL ShowSceneEditor();
	//! ����� IViewMgr
	virtual BOOL ShowTriggerEditor();

	//! ��������(������������) �������� �����
	void showSceneView();
	//! �������(������������) �������� ���������
	void showTEView();

	GameObjectFactory& getGameObjectFactory();
	ProjectModifier& getProjectModifier();

	void showAnimationCatalogDlg(ITreeNode* focused, int eAction, bool bUniqueList);
	void showSoundCatalogDlg(ITreeNode* dispNode, bool bUniqueList);

	//! ���������� ������ ������������ �������(��������� � ������� � ������� ������ ��)
	bool statObjectDependencies(qdNamedObject const* object);
	//! ����������� ��� ������ �� ����������� �������(��������� ��� �������������, 
	//! ��� ������� ��� �����), �� ��������� �� ������� �������
	FilterObjectDependencies::eDependenciesBound 
		filterDepedenciesBound(qdNamedObject const* object);
	//! ���������� ������ �������� �� ptrObjectsDependencies_
	void fillObjectList();

	CString const& getTempFileName() const;
	void setTempFileName(CString const& str);
	
	/* ������� ����� ���� ������ ��������� TreeLogic::saveFullStruct(...)
	//! ��������� ��������� ��� ����� ��������
	void saveSceneSubNodesState(IProjectSettings* pps);
	//! ��������� ��������� ��� ������������ �������
	void saveInterfaceScreensState(IProjectSettings* pps);
	*/
	//! ��������� ������, ����� �������������� �����
	bool safeSaveScript(qdGameDispatcher* pdisp, LPCTSTR lpszFileName);
	//! ����������� ��������� ������� (��������� ������ �������, ��������� ��������� ��������� � �.�.)
	void saveProjSettings();
	void readProjSettings();
	//! ��������� ��� ��� �������� �������� ��������� ��� �������� �����
	void saveActiveView(IProjectSettings* pps);
	//! ��������� ��� ������ ����� ����� ����������� �����
	void saveNetDrawMode(IProjectSettings* pps);
	//! ��������� � ���������� ������� ���� showTirgger_
	void saveShowTriggerElement(IProjectSettings* pps);

	//! ��������� �� �������� ������� ���� "���������� �� �������� � ��������"
	void readShowTriggerElement(IProjectSettings* pps);
	//! ��������� � ������������� �������� ��������
	void readActiveView(IProjectSettings* pps);
	//! ��������� ������ ���������� ����� ��� ����
	void readNetDrawMode(IProjectSettings* pps);
	//! ��������� �� �������� ������� ��������� ����� ��� ����� � ������������� ���
	void adjustNetDrawMode(qdGameScene* ptrScene, LPCTSTR lpszItemName, 
		IProjectSettings* pps);
	//! ��������� ��������� ��� ����� ��������
	void readSceneSubNodesState(IProjectSettings* pps);
	//! ��������� ��������� ��� ������������ �������
	void readInterfaceScreensState(IProjectSettings* pps);

	//! �������� �������
	void removeEdges();

	LRESULT beginEditStateDims(qdGameObjectState* pstate);
	void endEditStateDims(bool bSave);

	LRESULT beginStackState(qdGameObjectState* pstate);
	void endStateStack(bool bCancel);

	LRESULT beginCAEdit(qdGameObjectState* pstate);
	bool endEditCoordAnimationMode(bool bCancel);

	bool OnEditAnimatedDir(qdGameObjectAnimated* ptrObj);
	bool OnEditAnimatedDims(qdGameObjectAnimated* ptrObj);

	void activateScene(qdGameScene const* scene);

	//! ��������� ���� ���������� ��� ������� �� ��������� � ������ "�".
	void FillMenuForTriggers(CMenu*pm);
	//! ������������ ������� �� ���� ���������� FillMenuForTriggers
	void HandleEditTriggersMenu(HMENU hm, DWORD pos);

	//! ������� ���� � ������ ����� ���� 
	bool deleteNode(ITreeNode* node, bool ask);
	//! ���������������� ������ ��������������
	void startAutoSavePeriod();
private:
	ISceneViewFacade* sceneView_;
	ITriggerView* triggerEditorView_;
	StructureView* structureView_;
	boost::scoped_ptr<qdGameDispatcher> gameDispatcher_;
	boost::scoped_ptr<TreeLogic> treeLogic_;
	boost::scoped_ptr<TreeNodeFactory> treeNodeFactory_;
	//! ����� �� ������������ �������� �����
	bool canActivateTriggerView_;
	ViewActivator viewActivator_;
	IScaleMgr* scaleManager_;

	//! ���� ����� �������� �� �������� �������� ������
	boost::scoped_ptr<GameObjectFactory> gameObjectFactory_;

	//! ���� ����� �������� �� ������� � ������ ����� ��������
	boost::scoped_ptr<ProjectModifier> projectModifier_;
	//! ����� �� ������� ��������
	IObjectList* objectList_;

	//! ������ ������������. ���� ������.� ������, ������� �� ������� �������
	std::map<qdNamedObject const*, NamedObjectList> *ptrObjectsDependencies_;

	///\brief ��������� ��������� �� ������,
	///�� �������� ���� ����������� ���� �������������� ��
	///���� ��������, ���� �������������� ������ ���������
//	IRequestingStateEdit *m_ptrIReqStateEdit;
	//! ������ ������������� ��
	boost::scoped_ptr<class CoordAnimDlg>	coordAnimDlg_;
	//! ������ ���������� � �������� ���������
	boost::scoped_ptr<class StateCoordsStacker>  stateStackerDlg_;
	//! ������ ������������� ������� ���������
	boost::scoped_ptr<class StateBoundDlg> stateBoundDlg_;

	//! ���������� �������� SceneView
	/*!
	��������� ��� ����� �������� �������������� ������������ ��������, ��������
	��� ��������, ����� ����� ������������ ���
	*/
	int oldViewState_;
	/*! \brief �������� ����� ��������� ������, ������� ���� ����� �������
	� ����� ������ ���������
	*/
	std::vector<CString> m_TmpFiles;
	//! ����������, ����� �������� � �������� ��������� ����� ��������������
	eFilterScheme triggerElementFilterScheme_;
	//! ���������� �� �� �������� ���� �������� ���������
	bool showTriggerElement_;

	UINT autoSaveTimer_;
	//! ��� �����, � ������� ����� ������������� ��������
	CString tempFileName_;
};
