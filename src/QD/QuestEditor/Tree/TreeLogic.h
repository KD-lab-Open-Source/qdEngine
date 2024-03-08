#pragma once

#include <afxole.h>

#include <map>

#include "Main/IProjectFacade.h"
#include "Tree/ITreeNode.h"

#include <boost/iterator/transform_iterator.hpp>
#include <boost/rtl.hpp>
#include <boost/scoped_ptr.hpp>

#include <functional>

#include <TreeListCtrl/Null Listeners/NullItemDragdropNotifyListener.h>
#include <TreeListCtrl/Null Listeners/NullChangeItemLabelNotifyListener.h>
#include <TreeListCtrl/Null Listeners/NullItemOperationNotifyListener.h>
#include <TreeListCtrl/Null Listeners/NullGeneralNotifyListener.h>
#include <TreeListCtrl/Null Listeners/NullItemChangeNotifyListener.h>

interface ISceneViewFacade;
interface ITreeNodeFactory;
interface ITriggerView;
interface IObjectList;
interface IViewMgr;
interface IProjectSettings;

class Project;
class QETreeListCtrl;
class CTreeListItem;
class TreeCustomDrawer;

struct compareNoCase{
	bool operator()(CString const& lhs, CString const& rhs) const{
		return (lhs.CompareNoCase(rhs) < 0);
	}
};
typedef std::map<CString, CTreeListItem*, compareNoCase> TreeItemsIndex;

//! ������� ��� ��������� ������ �������� � ����
enum eTriggerIconCommand{
	TIC_REMOVE, ///< �������
	TIC_SET_NOT_ACTIVE, ///< ��������� ����������
	TIC_SET_ACTIVE///< ��������� ��������
};

/// ��������� ��������� ����� ������� ������� � TreeListCtrl'm
class TreeLogic	: public NullItemDragdropNotifyListener
				, public NullChangeItemLabelNotifyListener
				, public NullItemOperationNotifyListener
				, public NullGeneralNotifyListener
				, public NullItemChangeNotifyListener
{
	typedef std::multimap<DWORD_PTR, ITreeNode*> TreeNodeIndex;
	
public:
	//! ��������� ���� �������� ��������� � ��������� ���������
	enum eFilterScheme{
		FS_NONE, ///< ��� ����������
		FS_ENTRY_FOR_DYN_OBJ, ///< ���������� ��������� ������������ ��������
		FS_ENTRY_FOR_MOUSE, ///< ���������� ��������� ����
		FS_ENTRY_FOR_PERS, ///< ���������� ��������� ����������
		FS_ENTRY_FOR_DIALOGS,///< ���������� ���������� ���������
	};

	typedef boost::transform_iterator<
		std::select2nd<
			TreeNodeIndex::value_type
		>,
		TreeNodeIndex::const_iterator
	> NodeIterator;
	typedef boost::rtl::irange<NodeIterator> Nodes;
	TreeLogic(Project* project, ITreeNodeFactory* nodeFactory);
	~TreeLogic(void);
	//! ������ ������ �������
	bool loadStructure();
	/// ��������� sceneView
	void setSceneView(ISceneViewFacade* sceneView);

	/// ���������� ��������, � ������� ��������
	QETreeListCtrl& getTreeListCtrl() const;
	/// ��������� ��������, � ������� ��������
	void setTreeListCtrl(QETreeListCtrl* treeList);

	/// ���������� ������ �� ������ ����
	TreeItemsIndex& getSceneItemsIndex();
	/// ��������� �� ������� ����
	CWnd* getBaseWnd() const;
	/// ������������� ��������� �� ������� ����
	void setBaseWnd(CWnd* wnd);
	/// ���������� ��������� �� ��� �����
	ISceneViewFacade* getSceneView() const;
	/// �������� ������
	void reset();

	/// ��������� � ������ � ������ ����� ������
	bool addTreeNodeIndexEntry(DWORD_PTR key, ITreeNode* value);
	/// ������� ������ �� ������� ����� ������
	void removeTreeNodeIndexEntry(DWORD_PTR key, ITreeNode* value);
	//! ��������, �������� �� �������� �������� �����
	bool isSceneViewActive() const;
	/// ������������� ����������/������������ ��������� �����
	void setActiveSceneView(bool bActive);
	/// ���������� ��������� �� ��������� ����
	qdGameDispatcher* getGameDispatcher() const;

	/// ����� �� ���� ��� ����� �������������� �������.
	/** pfIsMoving(��������������) - �������� true ���� ��� ��� � ��������*/
	ITreeNode* isNodeUnderAnimated(ITreeNode const* node, bool *pfIsMoving) const;
	/// ����� �� ���� ��� ����� �����
	ITreeNode* isNodeUnderScene(ITreeNode const* node) const;
	/// ����� �� ���� ��� ��������� ���������.
	/** pfIsWalk (��������������) �������� true, ���� ��� ������� �������*/
	ITreeNode* isNodeUnderStateCatalog(ITreeNode const* node, bool *pfIsWalk) const;
	//! ��������� �� ���� ��� ����� ������������� ��������
	ITreeNode* isNodeUnderAnimatedCatalog(ITreeNode const* node);
	//! ��������� �� ���� ��� ����� ����������
	ITreeNode* isNodeUnderMovingCatalog(ITreeNode const* node);

	/// ��������� ���� �����, � ������� �������� � ��������� �����
	ITreeNode* getCurrentSceneNode() const;

	/// ��������� ����, �� ������� ���������� �����
	ITreeNode* getFocusedNode() const;
	/// ���������� ����, �� ������� ����� ��������� �����
	void setFocusedNode(ITreeNode* node);


	/// �������� ��� ���������� ���� ������ �������. ����� ������� �������� ��������
	bool showProperties(ITreeNode* node, int activePage);
	/// �������� ��� ���������� ���� ������ �������
	bool showConditions(ITreeNode* node);
	/// �� ���� �������� ���������� ���� ITreeNode
	ITreeNode* getTreeNode(CTreeListItem const* item) const;
	/// ��������� ������������ ���� ���������������� ����
	ITreeNode* getDraggedParentNode() const;
	/// ���������� ��������� ��������� ���������
	ITriggerView* getTriggerView() const;
	/// ��������� ��������� �� �������� ���������
	void setTriggerView(ITriggerView* view);

	//! ��� ��������� �������� � ��������� ���������
	eFilterScheme getFilterScheme() const;
	//! ����� �� ����������������� �� �������� � �� ��� ����� �� ����
	bool getShowTriggerElement() const;

	//! �� ������� ���������� ��� ����
	Nodes const getObjectNodes(qdNamedObjectBase const* obj) const;
	//! �� ����� ���������� ��� ����
	Nodes const getKeyNodes(DWORD_PTR key) const;

	//! ���������� ���� �������� ���������� ����, ����������� ��� ������ �����
	ITreeNode* getSubfolder(ITreeNode const* parent, DWORD_PTR type) const;
	/// ���������� ������������ ����
	ITreeNode* getParentNode(ITreeNode const* child) const;
	/// ���������� ������ ��� ���� 
	ITreeNode* getChildNode(ITreeNode const* parent) const;

	//! ���������� �������������� �������� ���������
	void endEditStateDims(bool bSave);

	/// ��������� �������� ��� ���������
	/**����� �������� ���������� � �������� �����*/
	void showAnimation(qdGameObjectState* state);
	/// ������������� �������� ��� ���������
	/**
		����� �������� ���������� � �������� �����
	 */
	void stopAnimation(qdGameObjectState* state);
	/// ��������� �����.
	/**
		�������� ����������, ���� ����. ����� �������� ���������� � �������� �����
	 */
	void playStopVideo(qdVideo* video);
	/// ������������� �����.
	/**
		����� �������� ���������� � �������� �����
	 */
	void stopVideo(qdVideo* video);
	//! ���������� ��� �������� �� ������� ������ ��������� ��������
	void setActiveTriggerIcon(qdTriggerChain const* chain, BOOL bSet);
	//! �������������, ��������������� ������ ��� ����
	void setInTriggerIcon(ITreeNode* node, 	eTriggerIconCommand command);

	//! �� ��������� ��������� ���� � ������
	void onItemStateChanged(ITreeNode* node, DWORD oldState, DWORD newState);

	bool selectInterfaceScreen(LPCTSTR screenName);

	//! �������� ���� �� ������
	ITreeNode* getSubNodeByName(ITreeNode* parent, LPCTSTR subNodeText) const;

	//! �������� ���� � ���������� ��� �� ������
	bool showNode(ITreeNode* node) const;
	/// ������������� ����� ���� � ������
	void setNodeText(ITreeNode* node, LPCTSTR text) const;
	/// ��������� ����� ���� � ������
	CString getNodeText(ITreeNode* node) const;

	//! ������� ��� �������� ����
	void removeSubnodes(ITreeNode* node) const;

	//! ������� ��� ������ expand
	enum eExpandCommand{
		EC_COLLAPSE,//! �������� ���� � ������
		EC_EXPAND,//! ���������� ���� � ������
		EC_TOGGLE//! ����������� ��������� ���� � ������
	};
	/// �����������/������������� ���� � ������
	void expand(ITreeNode* node, eExpandCommand command) const;
	//! �����������/������������� ���� � ��� �������
	void expandNodeAndChilds(ITreeNode* node, eExpandCommand command);
	//! ��������� �� ����
	bool isExpanded(ITreeNode const* node) const;

	//! ������� ���� � ������ � ��� ������ �� ������
	void deleteNode(ITreeNode* node);

	/// ���������� ��������� �� ������ ��������
	IObjectList* getObjectList() const;

	/// ���������� ��������� �� ������� ����� ������
	ITreeNodeFactory* getTreeNodeFactory() const;
	/// ��������� �� �����, ����������� ���������� ��������� ��������� � �����
	IViewMgr* getViewMgr() const;

	//! ����� �� ������� 
	DROPEFFECT canDrop(const CPoint& point);
	//! ���������� �������: ������� ������
	BOOL OnDropData(void* pData, DROPEFFECT dropEffect, const CPoint& point);
	/// �������� ���� ��� drop traget(���� �� ������� ����� ������ ��������������)
	void selectDropTarget(ITreeNode const* node);
	/// ��������� drop targets
	ITreeNode* getDropTarget() const;
	/// ��������� ����� �������
	IProjectFacade* getProject() const;

	//! �������������� ����� � Project::copyAnimatedToAnimated
	bool copyAnimatedToAnimated(ITreeNode* dropTarget, 
								qdGameObjectAnimated const* source,
								qdGameScene* scene);
	//! �������������� ����� � Project::copyAnimatedToMoving
	bool copyAnimatedToMoving(ITreeNode* dropTarget, 
								qdGameObjectAnimated const* source,
								qdGameScene* scene);
	//! �������������� ����� � Project::copyMovingToMoving
	bool copyMovingToMoving(ITreeNode* dropTarget, 
							qdGameObjectMoving const* source,
							qdGameScene* scene);

	//! �������������� ����� � Prject::moveState
	qdGameObjectState* moveState(qdGameObjectAnimated* source, 
					qdGameObjectAnimated* target,
					qdGameObjectState* movedState, 
					ITreeNode* targetNode,
					ITreeNode* insertAfter,
					int insertBefore);
					
	//! �������������� ����� � Prject::copyState
	qdGameObjectState* copyState(qdGameObjectAnimated* source, 
		qdGameObjectAnimated* target,
		qdGameObjectState* copyState, 
		ITreeNode* targetNode,
		ITreeNode* insertAfter,
		int insertBefore);

	//! �������������� ����� � Prject::moveZone
	qdGridZone* moveZone(qdGameScene* scene, 
			qdGridZone* movedZone, 
			qdGridZone const* zoneBefore,
			ITreeNode* nodeAfter,
			ITreeNode* parent);
	//! �������������� ����� � Prject::moveVideo
	qdVideo* moveVideo(qdVideo* source, qdVideo const* before, 
					ITreeNode* insertAfter, ITreeNode* parent);

	//! �������������� ����� � Project::copyInventory
	qdInventory* copyInventory(qdInventory const* source, ITreeNode* parent);

	/// ��������� ���������� ������� ����
	ITreeNode* getPrevVisibleNode(ITreeNode* node) const;
	/// ��������� ���������� �������� ����
	ITreeNode* getPrevSiblingNode(ITreeNode const* node);
	/// ��������� ��������� �������� ����
	ITreeNode* getNextSiblingNode(ITreeNode const* node);

	/// ������/������� ������� � ����.
	/** ������ �� ���������, ���� ������� �� �����. \sa showNodeCheck*/
	void setNodeCheck(ITreeNode const* node, bool check);
	/// ���������� ������� ����� � �����.
	void showNodeCheck(ITreeNode const* node, bool show);

	/* ������� ����� ���� ������ ��������� TreeLogic::saveFullStruct(...) � TreeLogic::loadFullStruct(...)
	/// ��������� ���������
	void saveIni(IProjectSettings* projectSettins);
	/// ��������� ���������
	void readIni(IProjectSettings* projectSettings);
	*/

	//! ������������ ���� ��������� ������ � ����
	void saveFullStruct(const char* fname);
	void loadFullStruct(const char* fname);

protected:
	//! �������� ������ ���������� �������� ����� ������� ����
	virtual bool onBeginDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! ���������� � ���, ��� ����� ������ ��� �����
	virtual bool onDragOver(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! ���������� � ���, ��� ������ ������ ����
	virtual bool onDrop(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);

	//! ���������� ����� ������� �������������� ������.
	virtual bool onBeginLabelEdit(CTreeListCtrl& source,CHANGE_LABEL_NOTIFY_INFO* pclns);

	//! ���������� � ���, ��� �������������� ���������
	virtual bool onEndLabelEdit(CTreeListCtrl& source, CHANGE_LABEL_NOTIFY_INFO* pclns);

	/// ������� ��������������� ���� �������� ITreeNode
	virtual void onDeleteItem(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info);

	/// ���������� ������ ������� ��� �������
	virtual bool onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	/// �������� ��������� � ITreeNode ���������������� ���� ��������
	virtual void onRClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	//! ����� ����, ��� ����������
	virtual void onItemExpanded(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ����� ���������� ���������
	virtual bool onItemStateChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ����� ��������� ���������
	virtual void onItemStateChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ���� ���� �����	TLC_CHECKBOX, �� �������� ����� ��������� ���������
	virtual void onItemCheckChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	
	/* ������� ����� ���� ������ ��������� TreeLogic::saveFullStruct(...) � TreeLogic::loadFullStruct(...)
	//! ��������� �� ������ ��������� �����
	void loadExpandData(std::istream& in);
	//! ��������� �� ������ ��������� �����
	void saveExpandData(std::ostream& out);
	*/ 

	//! ��������� ����, ������� �����
	void loadFocused(IProjectSettings* projectSettings);
	//! ��������� ����, ������� �����
	void saveFocused(IProjectSettings* projectSettings);

	friend class TreeNodeStube;
	//! ��������� ITreeNode � CTreeListItem
	static CTreeListItem* nodeToItem(ITreeNode const *node);
	//! ��������� CTreeListItem � ITreeNode
	static ITreeNode* itemToNode(CTreeListItem* item);
protected:

	/* ������� ����� ���� ������ ��������� TreeLogic::saveFullStruct(...) � TreeLogic::loadFullStruct(...)
	class ExpandDataLoader;
	class ExpandDataSaver;
	*/

	/// ������������� ��������� �� ���� ������� ����� 
	void setCurrentSceneNode(ITreeNode* node);

	/// �������� �����. ����� �� �������� ���������� � IProjectFacade::SelectScene
	bool selectScene(ITreeNode* sceneNode);
	/// \brief � ����������� �� ������� � ���������� ���������� ����������� ��� 
	/// ����� �� ����������� ���������. ��������� �������������
	void changeSceneNodesExpanding(ITreeNode const* exclude);

	/// ����� �� ���� � ��������, ���������� ����
	ITreeNode* isNodeUnderCatalog(ITreeNode const* node, DWORD catalogId);

	//! ���������� ����� ������-���� ������.
	TreeNodeIndex& getTreeNodeIndex();
	//! ���������� ����� ������-���� ������.
	TreeNodeIndex const& getTreeNodeIndex() const;
private:
	//! ����������� ����� expandNodeAndChilds
	void expandRecur(ITreeNode* node, eExpandCommand command);
	//! ����������� ������� ��� ������������ ���� ��������� ������
	void saveFullStructRecur(std::ofstream& out, ITreeNode* child);
	void loadFullStructRecur(std::ifstream& in, ITreeNode* child);

	/// ��������� �� ������� ������
	Project* project_;
	/// ��������� �� �������� �����
	ISceneViewFacade* sceneView_;
	/// ������� ����� ������
	ITreeNodeFactory* treeNodeFactory_;
	/// ������� ���������
	ITriggerView* triggerView_;
	/// Tree-�������
	QETreeListCtrl* tree_;
	/// ������ ����. ��������� ��� ��������� �����  � ������ ������������
	TreeItemsIndex sceneItemsIndex_;
	/// ������� ������������ �������� ������ �������� ������
	TreeNodeIndex treeNodeIndex_;
	/// ���� ������� �����
	ITreeNode*	currentSceneNode_;
	/// ����, � �������� �������� ����-�������
	CWnd*		baseWnd_;
//	//! ������� ������ ��������� � ��������� ���������
//	eFilterScheme filterScheme_;
//	bool showTriggerElement_;

	/// ��������������� ����
	mutable ITreeNode* draggedNode_;
	/// ������������ ���� ���������������� ����
	mutable ITreeNode* draggedNodeParent_;

	friend class TreeLogicDropTarget;
	//! ��������� drag-and-drop
	std::auto_ptr<TreeLogicDropTarget>		dropTarget_;

	/// ���������� ����� � ������
	boost::scoped_ptr<TreeCustomDrawer> customDrawer_;

	//! ���� ������ ����� �������� ������
	bool loadingTree_;
};


/*!
	���������� ��� ����������� \a COleDropTarget. �����, ����� ����� ����������
	������� �� � ���� ������ ������� ��� ���
*/
class TreeLogicDropTarget : public COleDropTarget
{
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pData, 
		DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, 
		DWORD dwKeyState, CPoint point );
	virtual DROPEFFECT OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, 
		DWORD dwKeyState, CPoint point );

	virtual DROPEFFECT OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);
public:
	TreeLogicDropTarget(TreeLogic& logic):logic_(logic){}
	/// ������� �� �������������� ������ � ����-�������
	bool DropedInMe()const{
		return m_bDropInMe;
	}
	void ResetDropInMe(){
		m_bDropInMe = false;
	}
private:
	bool m_bDropInMe;
	TreeLogic& logic_;
};
