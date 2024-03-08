#if !defined(AFX_SCENETREEVIEW_H__5E5C1963_3670_4651_AF79_BD5720CDEB56__INCLUDED_)
#define AFX_SCENETREEVIEW_H__5E5C1963_3670_4651_AF79_BD5720CDEB56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SceneTreeView.h : header file
//

#include <afxcview.h>
#include <afxole.h>
#include "curdirsafer.h"
#include <afxmt.h>
#include <map>
#include <list>
#include <vector>
#include "ScaleInterfaces.h"
#include "IViewMgr.h"
#include "Object List/IObjectList.h"

/////////////////////////////////////////////////////////////////////////////
// SceneTreeView view
/************************************************************************/
/* ��������������� ���������� �����                                     */
/************************************************************************/
class qdGameDispatcher;
class qdGameScene;
class qdGameObject;
class qdGameDispatcherBase;
class qdGameObjectStatic;
class CMainFrame;
class CMyPropertySheet;
class CChildView;
class qdGameObjectState;
class qdGridZone;
class qdGameObjectMouse;
class qdTriggerChain;
class qdVideo;
class qdMiniGame;
class qdMusicTrack;
class qdCounter;
class qdNamedObject;

class qdInterfaceDispatcher;
class qdInterfaceScreen;
class qdInterfaceButton;
class qdInterfaceElement;
class qdInterfaceElementState;
class qdInterfaceSlider;
class qdInterfaceBackground;
class qdInterfaceTextWindow;

interface ITriggerView;
interface IRequestingStateEdit;

class InterfaceButtonStateModeNode;
//////////////////////////////////////////////////////////////////////////

#include "qd_fwd.h"
/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/
#include <boost/intrusive_ptr.hpp>
class qdTriggerElement;
typedef boost::intrusive_ptr<qdTriggerElement> qdTriggerElementPtr;
typedef boost::intrusive_ptr<qdTriggerElement const> qdTriggerElementConstPtr;

typedef BOOL (*CHECK_FUN)(qdGameObject* p);
//////////////////////////////////////////////////////////////////////////
typedef std::list<qdNamedObject const*> NamedObjectList;
typedef std::vector<CString> CStringVect_t;

#include "auto_object.h"
#include "state_guard.h"
#include "ptree_api.h"
#include "ptree_consts.h"

//! ������������ ������ �������� �������
/*!
 	����� ����� �� ���� ��������������� :
	- �� �������� � �������� ��������
	- ��������� ������� �� ����������� ����
	- ��������� ��������
	- ����������/�������� ��������
*/
class SceneTreeView :	public CTreeView,
						public IViewMgr
{
	class view_activator;

	typedef std::map<CString, HTREEITEM> TreeItemIndex_t;
	typedef std::map<qdNamedObject*, std::vector<LPCTSTR> > AnimIndex_t;

	//! ��������� ���� �������� ��������� � ��������� ���������
	enum eFilterScheme{
		FS_NONE, ///< ��� ����������
		FS_ENTRY_FOR_DYN_OBJ, ///< ���������� ��������� ������������ ��������
		FS_ENTRY_FOR_MOUSE, ///< ���������� ��������� ����
		FS_ENTRY_FOR_PERS, ///< ���������� ��������� ����������
		FS_ENTRY_FOR_DIALOGS,///< ���������� ���������� ���������
	};

	//! ���������� ������� ��� ���� � �����
	bool showTriggerElement_;

	//! ������� ������ ��������� � ��������� ���������
	eFilterScheme m_eFilterScheme;

	//! ������� ���� ����������
	CMainFrame			*m_ptrMainFrame;
	//! �����, ������������ �����
	CChildView			*m_ptrSceneView;
	//! ��������� ��������� ���������
	ITriggerView		*m_ptrITView;

	//! ������ ����� ������ ������������� ������
	/*!
	 	��������� ��� ���������� ���������� �� �������� ����� ���� � ������
	 */
	TreeItemIndex_t		m_mapSceneIndex;
	//! ������ ����� ������,������������� ���������,
	/*!
		��������� ��� ���������� ���������� �� �������� ����� ������� � ������
	*/
	TreeItemIndex_t		m_mapMinigameIndex;

	//! ����� ����������� ��� ����� � ������
	std::auto_ptr<CImageList> m_ptrTreeImageList;

	//! ������������ ��� ����, ����� ��������� � ���� �������� ���� ���� ������
	CMenu m_menu;
	//! ����� ����, ��� �������� ���� ��������� ����
	CString m_strMenuOwnedItem;
	/*! \brief �������� ����� ��������� ������, ������� ���� ����� �������
		� ����� ������ ���������
	*/
	CStringVect_t m_TmpFiles;

	//! ��������� drag-and-drop
	COleDataSource m_DataSource;
	friend class SceneTreeDropTarget;
	//! ��������� drag-and-drop
	std::auto_ptr<SceneTreeDropTarget>		m_ptrDT;

	std::map<qdNamedObject const*, NamedObjectList> *ptrObjectsDependencies_;
	//! ����, ������� ������������. 
	HTREEITEM m_hDraggedItem;
	//! ���� ������, ���� m_hDraggedItem ���������, ���� ������ ������������ ����
	HTREEITEM m_hDraggedItemParent;
	
	//! ������������� ����� �������. 
	/*!
		������ �� ������� � ���, ����� ����� ��� ����� ���� �����
	 	�� ������� �����-������� ���������� ���������� �� ����� �����.
	 */
	UINT m_uiShowVideoTimer;

	//! �������� ���� ������� �����
	/*!
		����� ��� ����, ����� ������ ��������� ������������ ������
		� �� ������ ��� ������ ���
	 */
	HTREEITEM m_hCurrScene;

	//! ��������� ��������� �� ���������� �������� �������
	/*!
	 	� ������ ������ ���������� ���������������� ���������� � ����������.
	 */
	std::auto_ptr<CMyPropertySheet>	m_ptrPropSheet;

	//! �������� ������, �������� �������� ��������������
	/*!
	 	��� ������������ m_ptrPlayedObject ��������� �� ����� ������
		��� ������� ����������������� ��������� �� ���������, 
		���� �� ����� ���������, ���� ���������� ���������� ��������� �� ���������
	 */
	state_guard					m_ptrPlayedObject;
	//! ���������, �������� �������� �������������� � ������ ������
	qdGameObjectState*			m_ptrPlayedState;
	//! ��������� �������� � m_ptrPlayedObject � m_ptrPlayedState
	/*!
	 	�������������� ����� ������� ������ ��������� �������� 
		� ���������� ������ ����������
		� m_ptrPlayedObject � m_ptrPlayedState
	 */
	CCriticalSection			m_csAnimationGurad;
	//! ��������� ������ ��������� ��������
	axl::auto_object<HANDLE>	m_hAnimThread;
	//! �� ������ ��������� ��������
	DWORD						m_dwAnimThreadId;
	//! ��������� ����� ��������� � ������ ����
	/*!
	 	�������������� ������� � ������� ���������� � ����� ����.
		����� ��������� � ������ ������ ���� ���� � ������ � ���������,
		����� ���������� ������ ����
	 */
	std::auto_ptr<qdGridZone>	m_ptrEditedGZ;
	//! ��������� �� ��������� �������
	IScaleMgr*					m_ptrIScaleMgr;

	///\brief ��������� ��������� �� ������,
	///�� �������� ���� ����������� ���� �������������� ��
	///���� ��������, ���� �������������� ������ ���������
	IRequestingStateEdit *m_ptrIReqStateEdit;
	//! ������ ������������� ��
	std::auto_ptr<class CoordAnimDlg>	m_ptrCoordAnimDlg;
	//! ������ ���������� � �������� ���������
	std::auto_ptr<class StateCoordsStacker>  m_ptrStateStacker;
	//! ������ ������������� ������� ���������
	std::auto_ptr<class StateBoundDlg> m_ptrStateBoundDlg;

	//! ���������� �������� SceneView
	/*!
		��������� ��� ����� �������� �������������� ������������ ��������, ��������
		��� ��������, ����� ����� ������������ ���
	 */
	int m_iOldViewState;

	//! �������� �� ��������� ����
	qdGameDispatcher* m_ptrGameDisp;

	//! ����� �� ������� ��������
	IObjectList* objectList_;

	class thread_suspender;
	friend thread_suspender;
	//! �������� � ��������������� ����� ��������� ��������
	/*!
		���� ����� �� ����� ���������� ��������� ������� ���������� 
		����� ��������� ��������, �� ��� �� ��� �����. ��. OnProperies() ��� �������
	 */
	class thread_suspender
	{
	public:
		explicit thread_suspender(SceneTreeView* pstv):m_ptr(pstv){
			m_ptr->SuspendThread();
		}
		~thread_suspender(){
			m_ptr->ResumeThread();
		}
	private:
		SceneTreeView* m_ptr;
	};


	friend class view_activator;
	//! ����������� ����� ����� ���� ��������� ��������� � ��������� �����
	/*!
	 	������ ���������� � ��������� ���� ��������� ��������� � ���������� �����, 
		� ������������� � ��� ������.
		��������� �������� ������������ ����� ������
	 */
	class view_activator{
		//! ������ �� ���� ��������� �����
		/*!
		 	���� true, �� �� ������ �������� �����
			false - �������� ���������
		 */
		bool m_bIsSceneViewActive;
		//! ��������� �� ������ �������
		SceneTreeView* m_ptr;
		
		const view_activator& operator = (const view_activator&);
		view_activator(const view_activator&);
	public:
		view_activator(SceneTreeView* p):m_bIsSceneViewActive(true), m_ptr(p){}
		//! ������ � ���������� � ���, ��� �������
		operator bool() const{
			return m_bIsSceneViewActive;
		}

		//! ����������� ����� �������� � � ������������ � ��� ������ ��������� ����
		void operator = (bool b){
			if (b != m_bIsSceneViewActive) {
				if (!m_ptr->OnChangingViews(m_bIsSceneViewActive, b)) 
					return;
			}

			if (b) 
				m_ptr->ShowSceneView();
			else
				m_ptr->ShowTEView();

			bool old = m_bIsSceneViewActive;
			m_bIsSceneViewActive = b;

			if (old != b) 
				m_ptr->OnChangeViews(m_bIsSceneViewActive, b);
		}
	};


	class is_object_in_te;
	//! ������� �� �������� �����
	/*!
		������ ���������� �� ���������� ��������� ����� � ��������� ����������
		��������� ����� � ��������� ���������
	*/	
	view_activator m_IsSceneViewActive;

	//! ���������� ��� ��� �������� ���������
	/*!
		� ������ ����������� � ���, ��� ������ ���� "��������"
		���������� ��� ��� �������� ���������.
	*/
	bool m_bActivateTgrView;
protected:
	SceneTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(SceneTreeView);

	//! ��������� ��������� �� CMainFrame
	void SetMainFrame(CMainFrame* ptrMainFrame){
		m_ptrMainFrame = ptrMainFrame;
	}
	//! ������ ��� m_ptrIScaleMgr
	void SetScaleMgr(IScaleMgr* psm);

	//! ��������� ������� ����� � ������
	bool LoadGameObjects(CTreeCtrl& wndTree, HTREEITEM hParentItem,
							const qdGameObjectList& gol);
	//! ��������� ������� ����� � ������
	bool LoadGameObjects(CTreeCtrl& wndTree, const qdGameObjectList& gol,
			HTREEITEM hStaticObj, HTREEITEM hAnimatedObj, HTREEITEM hMovingObj);
	//! ������� ������� ��� �������� ���������� ��������
	bool LoadGlobalGameObjects(HTREEITEM hParent);

	//! ��������� �������� � ������
	bool LoadMinigames(HTREEITEM hParent, const qdMiniGameList& mgl);
	//! ��������� �������� � ������
	bool LoadMinigame(HTREEITEM hParent, qdMiniGame const* pg);

	//! ��������� ������ ����������� ������ � ������.
	bool LoadMusicTracks(HTREEITEM hParent, const qdMusicTrackList& mtl);
	//! ��������� ����������� ���� � ������.
	bool LoadMusicTrack(HTREEITEM hParent, class qdMusicTrack * pm);

	//! ���������� ����� ��� �������� ������ �����������
	bool LoadVideo(HTREEITEM hParent);
	//! ��������� ������ ����� �������
	bool LoadVideo(HTREEITEM hPar, const qdVideoList& vl);

	//! ���������� ����� ��� �������� ������ ������
	bool LoadFonts(HTREEITEM hParent);
	//! ��������� ������ ������
	bool LoadFonts(HTREEITEM hParent, qdFontInfoList const& l);

	//! ��������� ������ ����
	bool LoadScenes(HTREEITEM hParentItem, qdGameDispatcher* ptrDisp);
	//! ��������� ���������� �����: ����������� �����, �������, ����� � ���� �� ���
	bool LoadSceneObjs(HTREEITEM hParentItem, qdGameScene* ptrGS);

	//! ��������� � ������ ������ �������� �������
	bool LoadGameEnds(HTREEITEM hParent);

	//! ��������� �������� � ������
	bool LoadCounters(HTREEITEM hParent);

	//! ��������� 2  ���� � ������ ��� ��������� ����
	bool InsertGZStates(CTreeCtrl& tree, HTREEITEM hZoneItem, qdGridZone* pz);

	//! ��������� ���� � ������
	HTREEITEM InsertGZ2Tree(CTreeCtrl& tree, HTREEITEM hParentItem, 
						qdGridZone* pz, int iItemImage, HTREEITEM hInsertAfter);
	//! ��������� ������ ��� �� �����
	bool LoadGridZones(HTREEITEM hParentItem, qdGameScene const* ptrScene);

	//! ��������������� ������� ��� �������� ������ ��������
	template<class _List>
		bool lioLoadHelper(HTREEITEM hItemPar, const _List &lst, DWORD strId, 
				qdNamedObjectType ot, UINT uiObjsImage, UINT uiObjImage,
				HTREEITEM hInsertAfter/* = TVI_LAST*/);

	//! ������������ �� ��� ����� � ������� �����
	bool IsInventoryTypeUsed(int type);
	//! �������� �� ��� ����� ������-���� �������
	bool IsInventoryTypeUsedInObjs(int type);
	//! ��������� ��� ����� � ���������
	void AddInventoryCellTypes();
	//! ������������� ��� ��������� ���� ���������� �������
	void SetInvToPers(LPCTSTR name);
	//! ��������� ������ ��������� ����������
	bool LoadInventory(HTREEITEM hParent);
	
	//! ��������� ���� � � ���������
	bool LoadMouse(HTREEITEM hPar, qdGameObjectMouse* ptrMouse);

	//! ��������� ��������� ����� ����������
	bool LoadInterface(HTREEITEM hParent);
	//! ��������� ������������ ������
	bool LoadInterfaceScreens(HTREEITEM hParent,const qdInterfaceDispatcher* pIDisp);
	//! ��������� ���������� ������������� ������
	bool LoadInterfaceScreen(HTREEITEM hParent, qdInterfaceScreen* psi);
	//! ��������� � ������ ������� ����������(������, �������, ���)
	bool LoadInterfaceElement(HTREEITEM hParent, qdInterfaceElement* pele);
	//! ��������� � ������ ��������� ������������ ������ 
	bool LoadInterfaceBtnStates(HTREEITEM hParent, qdInterfaceButton const* pbtn);

	//! ���������� ����������� ���� �������������� ���� �������
	void ShowCorrespondingMenu(HTREEITEM hItem, bool bIsCatalog,
					const qdNamedObjectBase* pno,bool bIsGlobal);

	//! ���������� ����������� ���� ��� ����
	void ShowCorrespondingMenu(HTREEITEM hItem);

	//! ������������ ���� � ������������ � \a pobj � ������� �������� ���������
	void ModifyMenu(qdNamedObjectBase const* pobj, HTREEITEM hItem, 
								DWORD dwMenuId, CMenu* pm);
	//! ������������ ���� ��� qdGameObjectState* � \a pobj
	void mmStateNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! ������������ ���� ��� qdGameObjectMoving* � \a pobj
	void mmMovingObjNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! ������������ ���� ��� qdGameObjectAnimated* � \a pobj
	void mmAnimatedObjNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! ������������ ���� ��� qdGameObjectStatic* � \a pobj
	void mmStaticObjNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! ������������ ���� ��� qdInvetory* � \a pobj
	void mmInventoryObjNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! ������������ ���� ��� qdMusicTrack* � \a pobj
	void mmMusicTrackNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! ������������ ���� ��� �������� ����������� ��������
	void mmStaticObjsNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! ������������ ���� ��� �������� ������������� ��������
	void mmAnimatedOjbsNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! ������������ ���� ��� �������� ����������
	void mmMovingObjsNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! ������������ ���� ��� qdGridZone* � \a pobj
	void mmGridZoneNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! ������������ ���� ��� qdVideo* � \a pobj
	void mmVideoNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! ������������ ���� ��� �������� ���������
	void mmInventoryObjsNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! ������������ ���� ��� qdGameScene* � \a pobj
	void mmSceneNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	
	//! ���������� �������� ��� �������
	bool ShowCorrespondingPropDlg(qdNamedObjectBase* ptrObj, int iActivePage, bool bIsObj);
	//! ���������� �������� ��� �������� �������������� �� qdNamedObject
	bool ShowCorrespondingPropDlg(qdNamedObject* ptrObj, int iActive, bool bIsObj);
	//! ���������� �������� ��� �������� �������������� �� qdInterfaceObjectBase
	bool ShowCorrespondingPropDlg(qdInterfaceObjectBase* ptrObj, int iActive);
	
	//! ����������� ������ bitmap"� � ��������� �����, ��� ����� ������� ��� �����������
	void AdjustBitmapSize();

	//! ������ ����� �����, �� ���� �����, �� �������� ����� ����� ���� ��������������� �������
	std::auto_ptr<CStringArray> CreateTreePath(qdNamedObjectBase const * pobj);

	//! ������ �����
	qdGameScene* CreateScene(qdGameDispatcher* ptrDisp, CString& strName);

	//! ������� �������� �����
	qdGameEnd* CreateGameEnd(CString const& name, CString const& scrName);
	//! ������� �������
	qdCounter* CreateCounter(CString const& name);
	
	//! ������� �������� ���������
	enum eInventoryCrtMode{
		ICM_USE_EXISTING_CELL_TYPES,///< ��� �������� ������������ ������������ ������ �����
		ICM_EMPTY///< ��������� ������
	};
	//! ������� ����� ���������
	qdInventory* CreateInventory(eInventoryCrtMode mode);
	
	//! ��������� ������ ��������� � ������
	HTREEITEM InsertInventoryToTree(qdInventory* pinv);
	//! ������������� ��������� ��� � ��������� ��������� � ��������� ����
	bool InsertInvToGD(qdInventory* pinv, CString strName);

	//! ������ � ��������� ����� � ������
	HTREEITEM AddScene(HTREEITEM hItemParent, CString& strName);

	//! ���������� ���� � ������
	bool ShowItem(HTREEITEM hItem);

	//! ��������� ������ � � ����� � � ������
	bool AddSceneObj(qdGameObject* ptr, HTREEITEM hCatalogItem, 
					const CString& strName, UINT uiImage);

	//! \brief ��� �������� �������������� �� qdNamedObject ���������� ��� 
	//! ������� � ���������� ���� �� ������������ ����� � ��������������� ������.
	bool CorrespondingTestName(LPCTSTR strName, qdNamedObject* ptrObj);
	//! \brief ��� �������� �������������� �� qdInterfaceObjectBase ���������� ��� 
	//! ������� � ���������� ���� �� ������������ ����� � ��������������� ������.
	bool CorrespondingTestName(LPCTSTR strName, qdInterfaceObjectBase* pobj);
	
	//! ��������� ��������� � ��������� ����� ��������� � ������ ������.
	void SelectObjectInSceneView(qdGameObject* pno);

	//! ��������� ����������� � ���, ��� ���� ���� ����������
	void OnSelchanged4New(HTREEITEM hItem);
	//! ��������� ����������� � ���, ��� ���� �������� ���� ����������
	void OnSelchanged4Old(HTREEITEM hItem);
		
	//! �������� �����
	bool SelectScene(CTreeCtrl& tree, HTREEITEM hItem);

	//! ��������� ��������� ����� �, ���� �� ���������, �������� ���)
	bool SelectInterfaceScreen(LPCTSTR name);

	//! ���������� ���������� �������� ��������
	void ShowAnimationCatalogDlg(HTREEITEM hItem, int eAction, bool bUniqueList);
	//! ���������� ���������� �������� ������
	void ShowSoundCatalogDlg(HTREEITEM hItem, bool bUniqueList);

	//! \brief ���� � ������ ������ �������-����� ����������� �������
	//! � ������� ��
	void RemoveEmptyCopyOfGlobal(qdGameObject* pobj);
	//! ���� � ������ ������ �������-����� ��� ������������� �������� � ����������
	void aoRemoveEmptyCopyOfGlobal(qdGameObjectAnimated* pobj);

	//! ��������������� � ������ ����� ����������� �������
	void RenameCopyOfGlobal(LPCTSTR lpszOld, LPCTSTR lpszNew);

	//! ������� �������-����� ���� ���������� �������� � ������, ��� ����� ��� ���
	void GlobalObjsToScenes();
	//! ������� �������-����� \a pobj �� ���� ������, ��� ��� ����� ��� ���
	void GlobalObjToScenes(qdGameObject const* pobj);

	//! ������ ������ ��������-����� ����������� �������
	void GetCopyOfGlobalList(LPCTSTR name, CHECK_FUN f, qdGameObjectList* pres);
	
	//! \brief ������� ������ �� ��������������� ��� ����������
	//! � ��������� ��������, ����������� ��� ������������ ��������
	bool DelCorrespondingObj(qdNamedObjectBase* ptrObj, bool bAsk);

	//! \brief ������� ��� ������� �� ���������� ������ 
	//  ���� ������ DelCorrespondingObj
	template<class ObjectContainer>
	void cleanupObjects(ObjectContainer& container);
//	//! \brief ������� ��� ��������� ������������ �������� � ����������. 
//	void CleanupAnimatedObjectStates(qdGameObjectAnimated* pobj);

	//! ����������� �������� ��������� 
	bool EditState(HTREEITEM hItem);
	//! ����������� �������� ��������� 
	bool EditState(qdGameObjectState* pstate);

	//! ������� ��� ��������� ��������� �����
	void RemoveTmps();

	//! \brief �������� ��� ���������� ����� ��� ������� � ������ ������ ���� 
	//! � ������ ��������� ������
	CString GetTemporaryFileName();

	//! ��������� ����� �������� ��� ���������
	void ShowStateAnimation(qdGameObjectState * pno);
	//! ������������� ������������� ���������
	void StopAnimation(qdGameObjectState* ptrState);

	//! ���������� ����� ������ ���� ��������� ��������� � ��������� �����.
	bool OnChangingViews(bool bIsSVActiveOld, bool bIsSVActiveNew);
	//! ���������� ����� ����� ���� ��������� ��������� � ��������� �����.
	void OnChangeViews(bool bIsSVActiveOld, bool bIsSVActiveNew);

	//! ��������(������������) �������� �����
	void ShowSceneView();
	//! �������(������������) �������� ���������
	void ShowTEView();

	//! ����� �� ������� ����
	bool CanRemoveItem(HTREEITEM hItem);

	//! ������� ���� �� ������ � ��������������� ���� ������ �� �������
	void RemoveItem(HTREEITEM hItem, bool bAsk);

	//! �������������� ������ ��������� � ��������� ���������
	bool riStateSuppl(qdNamedObject *pno, HTREEITEM hItem);
	//! �������������� ������ ��������� � ��������� ��������� ������������ ������
	bool riIBtnStateSuppl(qdInterfaceObjectBase *piob, HTREEITEM hItem);

	//! ������� ���������
	bool RemoveState(qdGameObjectState* ptrObj, bool bAsk);

	//! ����� �� ������� ���������
	bool CanRemoveInventory(bool bAsk, qdInventory* pInv, 
							qdGameObjectList* plist4Remove);
	//! ������� ���������
	bool RemoveInventory(qdInventory* pInv, bool bAsk);

	//! \brief ������� ��������� ����� ����� �� ���������. ��������� ������������ �� 
	//! ��� ����� �� ����� ������ � ���� ���, �� ������� ���.
	bool DeleteInventoryCellset(qdInventory* pinv, int icellset);

	//! �������� �� ��������� ����������
	bool IsGlobalState(const qdNamedObject* pst, HTREEITEM hItem);

	//! ��������� � ������������� �����
	void PlayStopVideo(qdVideo * pv);
	//! ������������� �����
	void StopVideo(qdVideo* pv);
// Operations
public:
	//! ��������� ��������� �� ������ ��������
	void SetObjectList(IObjectList* objectList);
	//! ���������� ��������� ����
	qdGameDispatcher* GetGameDispatcher() const{
		return m_ptrGameDisp;
	}

	//! ��������� ���� � ������, � ����� ��������� ������� �����
	HTREEITEM LoadScene(HTREEITEM hPar, qdGameScene* pscene);

	//! ��������� � ��� ����� ��������� �������
	void SaveIni();
	//! ������ �� ��� ����� ��������� �������
	void ReadIni();

	/*!
	 	������ �������� ����� � �������� ���������
		\param ptrView		- �������� �����
		\param ptrTEView	- �������� ���������
		\return ������ true
	*/
	bool set_views(CChildView* ptrView, ITriggerView* ptrTEView){
		m_ptrSceneView = ptrView;
		m_ptrITView = ptrTEView;
		return true;
	}
	//! ��������� ��������� ������� � treectrl
	bool LoadStructTree(qdGameDispatcher* ptrGD);

	//! ���������� �������� �����
	BOOL ShowSceneEditor();
	//! ���������� ������� ���������
	BOOL ShowTriggerEditor();

	//! ���� ��������������� ������� ���� � ������
	HTREEITEM FindObjectItem(const qdNamedObjectBase* ptrObj);

	//! ���������� ����������� ���� ��� ������� �������
	void ShowCorrespondingMenu(const qdNamedObjectBase* pobj);

	//! �������� � ������ ��������������� ������� ����
	bool ShowObjectInTree(qdNamedObjectBase const* ptrObj);
	//! ���������� ������ ������� �������
	bool ShowProperties(qdNamedObjectBase* ptrObj, int iActivePage);
	//! ������� ������
	bool DeleteObject(qdNamedObjectBase* ptrObj, bool bAsk);

	//! ���������� ��������� �������������� ������ ���������
	void EndEditStateDims(bool bSave);

	//! �������� ��� � ��������� ���������.
	bool reset();
	
	//! �������� ���� ����� � ������
	BOOL ActivateScene(qdGameScene const* ptrScene);

	//! �������� �����
	bool AddScene(CString &strName, bool bShowNew);

	//! ��������� �������� � ������ �������� �� ������ �������
	qdGameObjectMoving*		AddPersWith1stGait(qdGameScene* ptrScene, 
												const CString& strName);
	//! �������� ����� ����������� ������� � �����
	qdGameObject*			AddGlobalObj2Scene(qdGameScene* pscene, 
												qdGameObject const* pobj);

	//! �������� ����������� ���������
	qdGameObjectMoving*		AddGlobalPersonage(qdGameDispatcher* ptrDisp, 
												CString &strName, bool bShow);
	//! �������� ���������
	qdGameObjectMoving*		AddPersonage(qdGameScene* ptrScene, 
										const CString &strName, 
										bool bShow, bool bAdd2Tree);
	//! �������� ������������ ������
	qdGameObjectAnimated*	AddAnimated(qdGameScene* ptrScene, 
										const CString &strName, bool bShow);
	//! �������� ����������� ������
	qdGameObjectStatic*		AddStatic(qdGameScene* ptrScene, 
										const CString &strName, bool bShow);
	//! �������� ���� ����� � ������
	bool					AddGridZone2Tree(qdGridZone* ptrZone);

	//! ������� ��������
	qdMiniGame*				CreateMiniGame(const CString& strName,
								const CString& strDllName, const CString& strConfigName);
	//! ���������� � ���� �������� ������������� �� ���������� ����� ��������
	bool					AddMiniGame();

	//! ��������� �������� � ������
	HTREEITEM InsertMGToTree(HTREEITEM hParent, const qdMiniGame* pmg);

	//! ��������� � �������� ��������� "��������" � "�������"
	bool AddSimpleMGState(qdMiniGame* pgame);

	//! ������� ����������� ����
	std::auto_ptr<class qdMusicTrack> CreateMusicTrack(const CString& strName);
	//! ������� ���. ���� � ��������� ��� � �����
	bool AddMusicTrack(qdGameScene* pscene, HTREEITEM hMTCatalog);

	//! ������� ������, ����������� ����
	std::auto_ptr<class qdFontInfo> CreateFont(CString const& strName);
	//! ��������� ������������ �� ����� ���� � ������
	bool AddFont(HTREEITEM hCatalog);
	//! ������� ���� � ��������� ��� � ������
	bool CreateFont(HTREEITEM hCatalog);
	//! ��������� ������ �������� ������ �������
	void ShowNewProjWiz();

	//! ������� ��������� ��������
	int ShowAnimation();

	//! ���������� ��������� �������� ���������-�����
	bool OnEndStateMaskCreate(qdGameObjectStateMask* ptrStateMask);
	//! ���������/�������� ��������� ��� ���� �� �����
	bool OnSaveGZChanges(bool bSave);

	//! ��������� �� ������ � ��������� � ������ ������ ��������, ���������� ����
	void UpdateCatalogsInScenes(qdNamedObjectType type);
	//! ��������� � ������ ������ �������� ���������� ���� ��� ��������� �����
	void UpdateObjCatalogInScene(qdNamedObjectType type, qdGameScene const* pscene);
	
	//! ������ ������������� ������� ������������� �������
	bool OnEditAnimatedDims(qdGameObjectAnimated* ptrObj);
	//! ������ ������������� ����������� �������������� �������
	bool OnEditAnimatedDir(qdGameObjectAnimated* ptrObj);

	//! ����� ���������� ������� � ������� ������ ���-������
	void OnAddObjToTrigger(qdNamedObject* pno);
	//! ����� �������� ������� �� ������� ������ ���-������
	void OnRemoveObjFromTrigger(qdNamedObject* pno);

	//! ��������� ���� � ��������� ������ � � ��������� ������
	void RunScene(const CString& strSceneName, ptree_api::eRunMode emode);
	//! ��������� ��������
	void RunMiniGame(qdMiniGame* pmg);

	//! ��������� �������� ��������
	void RunDialogEditor();

	/************************************************************************/
	/* �������� � ������������ ��������                                     */
	/************************************************************************/

	//! ���������� �������������� ������������ ��������
	bool EndEditCoordAnimationMode(bool bCancel);
	//! ���������� �������� ���������
	void EndStateStack(bool bCancel);

	//! �������� ����� �� ������� ����������
	CString CanClose();

	/* ������� ���������� ������������ ��������� */

	//! ��������� ������������ �����
	qdInterfaceScreen* AddInterfScr(HTREEITEM hItemInterf, const CString& name);
	//! ��������� ������������ ������
	qdInterfaceButton* AddInterfBtn(qdInterfaceScreen* pscr, 
		HTREEITEM hItemScr, const CString& name);
	//! ��������� ����
	qdInterfaceSave* AddInterfSave(qdInterfaceScreen* pscr, 
		HTREEITEM hItemScr, const CString& name);
	//! ��������� ���
	qdInterfaceBackground* AddInterfBack(qdInterfaceScreen* pscr, 
		HTREEITEM hItemScr, const CString& name);
	//! ��������� ���������
	qdInterfaceSlider* AddInterfSldr(qdInterfaceScreen* pscr, 
		HTREEITEM hItemScr, const CString& name);
	//! ��������� ���� �������
	qdInterfaceTextWindow* AddInterfTextWindow(qdInterfaceScreen* pscr, 
		HTREEITEM hItemScr, const CString& name);
	//! ��������� ��������� ������������ ������
	int AddInterfBtnState(qdInterfaceButton* pb, 
		HTREEITEM hItemBtn, const CString& name);

	//! ��������� �������� �����
	qdGameEnd* AddGameEnd(HTREEITEM hParent, 
		CString const& name, CString const& scrName);
	//! ��������� �������
	qdCounter* AddCounter(HTREEITEM hParent, CString name);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SceneTreeView)
	public:
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~SceneTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(SceneTreeView)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnProperies();
	afx_msg void OnAnimationList();
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCrtScene();
	afx_msg void OnCrtMovingObj();
	afx_msg void OnCrtStaticObj();
	afx_msg void OnUpdateButtonRunScene(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonRunScript(CCmdUI* pCmdUI);
	afx_msg void OnRemove();
	afx_msg void OnCrtAnimation();
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuMovingsStops();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCreateStatics();
	afx_msg void OnUpdateCreateStatics(CCmdUI* pCmdUI);
	afx_msg void OnCreateBack();
	afx_msg void OnUpdateCreateBack(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNetProperties();
	afx_msg void OnEditAnimation();
	afx_msg void OnEditGaits();
	afx_msg void OnCrtAnimObjs();
	afx_msg void OnUpdateCrtMovingObj(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNetProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCrtAnimObjs(CCmdUI* pCmdUI);
	afx_msg void OnGameSettings();
	afx_msg void OnUpdateEditAnimation(CCmdUI* pCmdUI);
	afx_msg void OnCreateState();
	afx_msg void OnDestroy();
	afx_msg void OnAddVideo();
	afx_msg void OnDelVideo();
	afx_msg void OnPlayStopVideo();
	afx_msg void OnStopVideo();
	afx_msg void OnStateCondition();
	afx_msg void OnButtonRunScene();
	afx_msg void OnButtonRunScript();
	afx_msg void OnCrtGridZone();
	afx_msg void OnCreateMask();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSoundList();
	afx_msg void OnEditSound();
	afx_msg void OnUpdateEditSound(CCmdUI* pCmdUI);
	afx_msg void OnSelScene();
	afx_msg void OnPersDim();
	afx_msg void OnPersDir();
	afx_msg void OnTbtnAllStates2Editor();
	afx_msg void OnCrtTrigger();
	afx_msg void OnEditTriggers();
	afx_msg void OnUpdateTbtnAllStates2Editor(CCmdUI* pCmdUI);
	afx_msg void OnNotControledPers();
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdjustGameArea();
	afx_msg void OnUpdateEditGaits(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelScene(CCmdUI* pCmdUI);
	afx_msg void OnStateDims();
	afx_msg void OnStateHasBound();
	afx_msg void OnBtnAutoBnd();
	afx_msg void OnStackState();
	afx_msg LRESULT OnStackState(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEditStateDims(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetCoordAnimationMode(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCoordsAnimation();
	afx_msg void OnCrtNewInventory();
	afx_msg void OnRemoveCellSet();
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
	afx_msg void OnCreateMinigame();
	afx_msg void OnCOCondition();
	afx_msg void OnAddCellSet();
	afx_msg void OnDisableMouse();
	afx_msg void OnAddMusicTrack();
	afx_msg void OnMusicTrackCycled();
	afx_msg void OnMusicTrackPlay();
	afx_msg void OnMusicTrackStop();
	afx_msg void OnShowAllNetProjs();
	afx_msg void OnUpdateShowAllNetProjs(CCmdUI* pCmdUI);
	afx_msg void OnDrawAllBounds();
	afx_msg void OnUpdateDrawAllBounds(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	//! ��������������� ������
	bool RenameObject(qdNamedObject* pno, LPCTSTR lpszNewName);
	//! ��������������� ������������ ������
	bool RenameObject(qdInterfaceObjectBase* piob, LPCTSTR lpszNewName);
	//! �������������� ���������
	bool RenameInventory(qdGameDispatcher* disp, 
		qdInventory* inventory, LPCTSTR lpszNewName);
	//! ��������� ��������� ��������� ���������,� ������� ������ ������ ������
	void UpdateObjTrgEleTitles(qdNamedObject* pno);
	//! ���������� ��� ��������� ��������� ���������
	void UpdateAllTitles();
	//! ��������� ��������� ��������� ���������, ��� ��������� �������
	void UpdateTrgEleTitle4ObjStates(qdGameObjectAnimated* pobj);

	//! ��������� ��������� ��������� ���������, ��� �������� �� �����.
	void UpdateTrgEleTitle4SceneObjs(qdGameScene* pscene);
		
	//! ������� ��� ��� �����
	bool CreateBackground(HTREEITEM hSceneItem, qdGameScene* ptrScene);
	//! ��������� ����������� ������
	bool AddStatics(HTREEITEM hSceneItem);

	//! ������� ������� ����
	template<class GameObj>
		GameObj* CreateGameObject(DWORD dwErrMsg);
	//! ������� ���������
	qdGameObjectMoving*		CreatePersonage();
	//! ������� ������������ ������
	qdGameObjectAnimated*	CreateAnimated();
	//! ������� ����������� ������
	qdGameObjectStatic*		CreateStatic();

	//! \brief ��������� ������ � ����� � ��������� ������.
	//! ���� ���� ����������� ��� �� ���������
	bool AddToScene(qdGameScene* ptrScene, qdGameObject* ptrObj, 
				const CString& strName);

	
	//! �������� � ������ GameObject
	HTREEITEM AddGOToTree(HTREEITEM hItemParent, qdGameObject* ptr, UINT uiImage);

	//! �������� � ������ GameObject, ��� ���� ������� ��������������� ������� 
	HTREEITEM AddGOToTree2(HTREEITEM hSceneItem, UINT uiCatNameID, 
											qdGameObject* ptr, UINT uiImage);

	//! ������� �������� ����
	void DeleteChildern(HTREEITEM hPar);
	//! ���������� ���������� �������� �������� ���������� ����
	void UpdateSceneObjsCatalog(qdNamedObjectType type, 
							HTREEITEM hCatalog, qdGameScene const* ptrScene);
	//! ��������� ���������� �������� �������� ���������� ����
	void UpdateObjsCatalog(qdNamedObjectType type, HTREEITEM hCatalog, 
										const qdGameObjectList& gol);

	//! �������������� ������� ��������� ����������� ������� � ��� ������� � ������
	void SyncGlobalStatesCatalog(qdGameObjectAnimated* ptrObj);

	//! �������� ������� ���������.
	qdTriggerChain* CreateTriggerChain();
	//! �������� ������� ���������
	qdTriggerChain* AddTriggerChain(HTREEITEM hPar, const CString& strName, 
																bool bShow);
	//! ��������� � �������� ������ �������
	bool InsertFirstGaitToPersonage(qdGameObjectMoving* ptrPers);

	//! ���������� ����������� ���� �������
	void UpdateObjImage(const qdGameObject* pobj);
	//! ��������� ����������� ���� ���������
	void UpdateStateImage(qdGameObjectState* pstate, HTREEITEM hItem);

	//! ��������� ����������� ����� ��� ���� ��������� �������
	void UpdateObjStatesImages(qdGameObjectAnimated const* pobj, HTREEITEM hItem);

	//! �������� ��� �����
	void CollapseAllScenes(qdGameScene* psceneEx);

	//! ������� ��� ���� � ����
	BOOL SetNoCheck(HTREEITEM hItem){
		return GetTreeCtrl().SetItemState(hItem, 0, TVIS_STATEIMAGEMASK);
	}
	//! ������������� ��������� ��������(���������� ��� ���)
	BOOL SetCheck(HTREEITEM hItem, BOOL bCheck){
		return GetTreeCtrl().SetItemState(hItem, 
			INDEXTOSTATEIMAGEMASK((bCheck ? 2 : 1)), TVIS_STATEIMAGEMASK);
	}

	//! ���������� ��������� ��������(���������� ��� ���)
	int GetCheck(HTREEITEM hItem)
	{
		UINT uState = GetTreeCtrl().GetItemState(hItem, TVIS_STATEIMAGEMASK);
		return (static_cast<BOOL>((uState >> 12) -1));
	}
	//! �� ��������� ��������� �������� � ���� �������� ����
	void OnCheckChangeGZone(int iCheck, qdGridZone* pzone);
	//! �� ��������� ��������� �������� � ���� �����
	void OnCheckChangeNet(int iCheck, HTREEITEM hItem);
		
	//! ��� ���� ��������� �������� ��� ��������� � ��������� �����.
	bool SelectOwner(HTREEITEM hItem);

	//! ���������� ����-������� ��� �����
	HTREEITEM GetVideoCatNode();

	//! �������� � ��������� �����-����� � ������
	bool AddVideo();
	//! ��������� ����-����� � ������
	bool AddVideoFile(const CString &strFileName);

	//! ��������� ���� ���������� ��� ������� �� ��������� � ������ "�".
	void FillMenuForTriggers(CMenu*pm);
	//! ������������ ������� �� ���� ���������� FillMenuForTriggers
	void HandleEditTriggersMenu(HMENU hm, DWORD pos);

	//! ������������� ����� ������������ ��������
	bool SuspendThread();
	//! ������������� ����� ������������ �������� � ������� ������������� ���������
	bool SuspendAndClear();
	
	//! ��������� ����� ������������ ��������
	bool ResumeThread();

	//! ������������� ������������� ��������� � ��������� ����� ������������ ��������
	bool SetAndResume(qdGameObjectState* ptrState);

	//! ���������� ������� "�������" ��� drag and drop
	BOOL OnDropData(void* pData, DROPEFFECT dropEffect, const CPoint& point);

	//! ������� ����������� ��������� ��� ���������
	BOOL oddOwnStateMovingObj(HTREEITEM hObjItem, HTREEITEM hDropTarget,
		qdGameObjectState* dragged_state, DROPEFFECT dropEffect);
	//! ������� ����������� ��������� ��� ������������ ������
	BOOL oddOwnStateAnimObj(HTREEITEM hObjItem, HTREEITEM hDropTarget,
		qdGameObjectState* dragged_state, DROPEFFECT dropEffect);
	//! ������� ����������� ��������� ��� ����
	BOOL oddOwnStateMouseObj(HTREEITEM hObjItem, HTREEITEM hDropTarget,
		qdGameObjectState* dragged_state, DROPEFFECT dropEffect);

	//DropData ��� ��������� �����
	//! ������� ���������
	BOOL oddOwnState(qdGameObjectState* pstate, DROPEFFECT dropEffect, 
		const CPoint& point);
	//! ������� ���������� ������
	BOOL oddGlobalObject(qdGameObject* pobj, DROPEFFECT dropEffect, 
		const CPoint& point);
	//! ������� ������������ ������
	BOOL oddAnimatedObject(qdGameObjectAnimated* pobj, DROPEFFECT dropEffect,
		const CPoint& point);
	//! ������� ��������
	BOOL oddMovingObject(qdGameObjectMoving* pobj, DROPEFFECT dropEffect,
		const CPoint& point);
	//! �������� �������� ����
	BOOL oddGridZone(qdGridZone* pobj, DROPEFFECT dropEffect, const CPoint& point);
	//! ������� ������� ���������
	BOOL oddTriggerChain(qdTriggerChain* pch, DROPEFFECT dropEffect, const CPoint& pnt);
	//! ������� ����� �����
	BOOL oddVideo(qdVideo* video, DROPEFFECT dropEffect, const CPoint& pnt);
	//! ������� ���������
	BOOL oddInventory(qdInventory* inventory, DROPEFFECT dropEffect, CPoint const& point);

	//! ����� �� ������ �� ��� �������(drag and drop)
	DROPEFFECT	CanDrop(const CPoint& point, COleDataObject* pDataObject);
	//������� ������ ��� CanDrop(cd)

	//! ����� �� ������� ���������� ������
	DROPEFFECT  cdGlobalObj(HTREEITEM hItem, const CPoint& point, 
							qdGameObject* pobj);
	//! ����� �� ������� ��������
	DROPEFFECT  cdMovingObj(HTREEITEM hItem, const CPoint& point, 
						qdGameObjectMoving* pobj);
	//! ����� �� ������� ������������ ������
	DROPEFFECT  cdAnimatedObj(HTREEITEM hItem, const CPoint& point, 
						qdGameObjectAnimated* pobj);
	//! ����� �� ������� ���������� ���������
	DROPEFFECT  cdGlobalState(HTREEITEM hItem, const CPoint& point, 
												qdGameObjectState* pstate);
	//! ����� �� ������� �����������(������������) ���������
	DROPEFFECT  cdOwnState(HTREEITEM hItem, const CPoint& point, 
											qdGameObjectState* draged_state);
	//! ����� �� ������� ����������� ��������� ��� ��������
	DROPEFFECT cdOwnStateUnderMoving(HTREEITEM hItem, HTREEITEM hObj,
												qdGameObjectState* draged_state);
	//! ����� �� ������� �������� ����
	DROPEFFECT cdGridZone(HTREEITEM hItem,  const CPoint& point, qdGridZone* pgz);
	//! ����� �� ������� ������� ���������
	DROPEFFECT cdTriggerChain(HTREEITEM hItem, const CPoint& point, qdTriggerChain* pch);
	//! ����� �� ������� �����
	DROPEFFECT cdVideo(HTREEITEM hItem, CPoint const& point, qdVideo* pvideo);
	//! ����� �� ������� ���������
	DROPEFFECT cdInventory(HTREEITEM hItem, CPoint const& point, qdInventory* pinvetory);
	//! ��������� �� ���� ��� ������������ �������� ��� ����������
	HTREEITEM	IsUnderAnimated(HTREEITEM hItem, bool *pfIsMoving = NULL);
	//! ���������� �� ���� ��� ����� �� ��������� ��������� ���������
	HTREEITEM	IsUnderStateCatalog(HTREEITEM hItem, bool *pfIsWalk = NULL);
	//! ����� �� ������� ��������� ��� ��������
	bool		CanDropUnderMoving(HTREEITEM hMovingObjNode);
	//! ����� �� ������� ��������� ��� ������������ ������
	bool		CanDropUnderAnim(HTREEITEM hAnimObjNode);

	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ������������ �������
	bool OnSelectStaticObjNode(qdNamedObject* ptrObj, 
					bool bIsObj, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! �������������� �������
	bool OnSelectAnimatedObjNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	bool OnSelectAnimatedObjNode2(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ���������
	bool OnSelectMovingObjNode(qdNamedObject* ptrObj, 
					bool bIsObj, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! �����
	bool OnSelectSceneNode(qdGameScene* ptrScene, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! �����. ���������� ���������, ����� ������� �������� ���������.
	bool OnSelectSceneNode2(qdGameScene* ptrScene, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ���������
	bool OnSelectStateNode(qdNamedObject* pno, bool bIsObj, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ���������. ���������� ���������, ����� ������� �������� ���������.
	bool OnSelectStateNode2(qdNamedObject* pno, bool bIsObj, bool bSelect);

	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! �������� ����
	bool OnSelectGridZoneNode(qdNamedObject* ptrObj, 
					bool bIsObj, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ����
	bool OnSelectMouseNode(qdNamedObject* ptrObj, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! �����-������
	bool OnSelectVideoNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ��������� 
	//! ����.(���� � ������� "���������� �������")
	bool OnSelectGlobalNode(qdNamedObject* ptrObj, bool bSelect);

	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ������� ���������
	bool OnSelectTriggerNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ������� ���������. ���������� ���������, ����� ������� �������� ���������
	bool OnSelectTriggerNode2(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ���������
	bool OnSelectInventoryNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ��������
	bool OnSelectMinigameNode(qdNamedObject* pobj, bool bIsObj, bool bSelect);

	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ��������� �������� ����
	bool OnSelectGZStateNode(HTREEITEM hItem, bool bSelect);

	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! "���������"
	bool OnSelectInterfaceNode(qdInterfaceObjectBase* pobj, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ������������� ������
	bool OnSelectInterfaceScreenNode(qdInterfaceObjectBase* pobj, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ������������� ��������
	bool OnSelectInterfaceElementNode(qdInterfaceObjectBase* pobj, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ��������� ������������� ��������
	bool OnSelectInterfaceElementStateNode(qdInterfaceObjectBase* pobj, bool bSelect);

	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ������������ �����
	bool OnSelectMusicNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	//! \brief ���������� ������� �������/������� ��������� "������" ��� ���� 
	//! ����������(���������) ������
	bool OnSelectGameEndNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);

	void ReselectZones();

	BOOL MergeTriggers(qdTriggerChain const* from, qdTriggerChain* to);
	/************************************************************************/
	/* ���������� � ���-���� ��������� ������                               */
	/************************************************************************/
	//! ��������� � ���������� ������� ��������� ����
	void SaveSelectedItem(interface IProjectSettings* pps);
	//! ��������� � ���������� ������� ������� �����
	void SaveCurrentScene(interface IProjectSettings* pps);
	//! //! ��������� � ���������� ������� ��������� ����� ����. �������� ��� ��� ��������
	void SaveSceneItemsState(interface IProjectSettings* pps);
	//! //! ��������� � ���������� �������  ����� ���������� �����
	void SaveNetDrawMode(interface IProjectSettings* pps);
	//! ��������� � ���������� ������� �������� ��������
	void SaveActiveView(interface IProjectSettings* pps);
	//! ��������� � ���������� ������� ���� showTriggerElement_
	void SaveShowTriggerElement(interface IProjectSettings* pps);
	/************************************************************************/
	/* ������ �� ���-�����                                                  */
	/************************************************************************/
	//! \brief ��������� ��������� ���� �� ��� ����� ������� � �������� ��� ��������
	//! � ����������� � �������� ���������
	void AdjustItemState(const CString&strSecName, LPCTSTR lpszItemName, 
		interface IProjectSettings* pps, HTREEITEM hPar);
	//! ��������� �� �������� ������� ������� �����
	void ReadCurrentScene(interface IProjectSettings* pps);
	//! ��������� �� �������� ������� ��������� ����� � �����
	void ReadSceneItemState(interface IProjectSettings* pps);
	//! ��������� �� �������� ������� ��������� ����� ��� ����� � ������������� ���
	void AdjustNetDrawMode(qdGameScene* ptrScene, LPCTSTR lpszItemName, 
		interface IProjectSettings* pps);
	//! ��� ����� ���� ���� ������������� ����� �� ����������
	void ReadNetDrawMode(interface IProjectSettings* pps);
	//! ��������� �� �������� ������� �������� �������� � ������������� ���
	void ReadActiveView(interface IProjectSettings* pps);
	//! ��������� �� �������� ������� ���� showTriggerElement_
	void ReadShowTriggerElement(interface IProjectSettings* pps);
	
	/************************************************************************/
	/* ������ � ��������� ���������                                         */
	/************************************************************************/
	//! \brief ������������� ����� "�" ����� � ������ ��������, ������� ������ � 
	//! ��������
	void SetTgrIcon4All(BOOL bSet);
	//! ������������� ���������(������ ��� �������) ����� "�".���� ������� ��
	void SetInTgrIcon(qdTriggerChain* pchain, BOOL bSet, int image_id);
	//! ���������� ��� �������� � ��������� ���������
	void ShowAllTriggers();
	/************************************************************************/
	/* �������� � ������������ ��������                                     */
	/************************************************************************/
	//! ���������� �������� ���������
	LRESULT BeginStackState(qdGameObjectState* pstate);
	//! ���������� �������������� ��
	LRESULT BeginCAEdit(qdGameObjectState* pstate);
	//! ���������� �������������� �������� ���������
	LRESULT BeginEditStateDims(qdGameObjectState* pstate);

	/*
	 *	�������� ������ � �����������(��������) � ����������� ������� �� ������
	 */
	//! �������� ������ �� �������� ���������
	bool RemoveImageEdges(qdGameObjectState* pst);
	//! ������� ������� ��� ���� ����������� ��������� ������� � �������� ��� �����
	bool RemoveEdgesForObj(qdGameObjectAnimated* pobj);
	//! �������� ������ ��� ����������� ��������
	void RemoveEdgesInInterface(qdNamedObjectBase* pobj);

	//! �������� ������ ��� ������������� ����
	void RemoveEdgesInInterfaceBackground(qdInterfaceBackground* pobj);
	//! �������� ������ ��� ��������� ������������� ��������
	void RemoveEdgesInInterfaceElementState(qdInterfaceElementState* pobj);
	//! �������� ������ ��� �����
	void RemoveEdgesInInterfaceSave(qdInterfaceSave* pobj);
	//! �������� ������ �������
	void RemoveEdgesInInterfaceSlider(qdInterfaceSlider* pobj);
	//! �������� ������ ������
	void RemoveEdgesInInterfaceScreen(qdInterfaceScreen* pobj);
	//! �������� ������ ������������ ������
	void RemoveEdgesInInterfaceButton(qdInterfaceButton* pobj);

	//! ������������ tga � qda
	void ConvertTGAtoQDAInInterface(qdNamedObjectBase* pobj);

	//! ����, ��� ��� ������������ �������� �� ��������� � �������� ��� ���������
	void StateOffsetInOtherAnimUses(qdGameObjectStateStatic* query_src, 
		Vect2i const& offset);
	//! ���������� ����� ���������, ������� ���������� �������� �� \a anim_lst
	void StateOffsetInOtherAnimUses4Scene(qdGameObjectStateStatic* query_src, 
		Vect2i const& offset, qdGameScene* pscene, std::vector<LPCTSTR> const& anim_lst);
	//! \brief ��� ������� ����������(����� ��� ����������� ����������) ��������� 
	//! ������ ��������, ����������� �� ���� \a strFileName
	void FindAllAnimation(const CString& strFileName, AnimIndex_t& anim_idx);

	enum eDependenciesBound{
		ACROSS_OBJECT, ///< ����������� ����������� ������� �� ������� �������
					///< ��.:(��������� - ����������, ����� ������)
		IN_OBJECT};///< ����������� ����������� �� ������� �� �������
	//! ����������� ��� ������ �� ����������� �������(��������� ��� �������������, 
	//! ��� ������� ��� �����), �� ��������� �� ������� �������
	eDependenciesBound filterDepedenciesBound(qdNamedObject const* object);

	//! ���������� ������ ������������ �������(��������� � ������� � ������� ������ ��)
	bool statObjectDependencies(qdNamedObject const* object);

	//! ���������� ������ �������� �� ptrObjectsDependencies_
	void fillObjectList();
protected:
	afx_msg void OnUpdateCrtStaticObj(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCrtGridZone(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlayStopVideo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateStopVideo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCrtNewInventory(CCmdUI *pCmdUI);
	afx_msg void OnPlayMinigame();
	afx_msg void OnMakeDirStruct();
	afx_msg void OnEditStateAnimation();
	afx_msg void OnRemoveEdges();
	afx_msg void OnToolbarDropDown(NMHDR* pNMHDR, LRESULT *plr);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void OnMenuCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRemoveEdgesInStates();
	afx_msg void OnRemoveEdgesForAllObjs();
	afx_msg void OnBindObjCenter();
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCreateIscreen();
	afx_msg void OnAddInterfBackground();
	afx_msg void OnAddInterfSlider();
	afx_msg void OnAddInterfTextWindow();
	afx_msg void OnAddIntrfButton();
	afx_msg void OnUpdateTbtnScaleMinus(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTbtnScalePlus(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAddVideo(CCmdUI *pCmdUI);
	afx_msg void OnAddInterfBtnState();
	afx_msg void OnAddInterfSave();
	afx_msg void OnRunAnimEditor();
	afx_msg void OnCrtAnimObjByMask();
	afx_msg void OnConvertToQda();
	afx_msg void OnJoinProject();
	afx_msg void OnUpdateJoinProject(CCmdUI *pCmdUI);
	afx_msg void OnAddGameEnd();
	afx_msg void OnCreateCounter();
	afx_msg void OnSelectMovemtnType();
	afx_msg void OnManageCds();
	afx_msg void OnEditDialogs();
	afx_msg void OnUpdateEditDialogs(CCmdUI* pCmdUI);
	afx_msg void OnMakeBoundAsFirst();
	afx_msg void OnCancelRemovingEdges();
	afx_msg void OnAddFont();
	afx_msg void OnCreateFont();
	afx_msg void OnWaveToLocal();
	afx_msg void OnLocalToWave();
	afx_msg void OnUpdateWaveToLocal(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLocalToWave(CCmdUI* pCmdUI);
	afx_msg void OnStatDependencies();
};

/*!
 	���������� ��� ����������� \a COleDropTarget. �����, ����� ����� ����������
	������� �� � ���� ������ ������� ��� ���
*/
class SceneTreeDropTarget : public COleDropTarget
{
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pData, 
							DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, 
											DWORD dwKeyState, CPoint point );
	virtual DROPEFFECT OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, 
											DWORD dwKeyState, CPoint point );

	virtual DROPEFFECT OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);
public:
	bool DropedInMe()const{
		return m_bDropInMe;
	}
	void ResetDropInMe(){
		m_bDropInMe = false;
	}
private:
	bool m_bDropInMe;
};

//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_SCENETREEVIEW_H__5E5C1963_3670_4651_AF79_BD5720CDEB56__INCLUDED_)
