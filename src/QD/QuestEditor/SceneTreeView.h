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
/* Предварительные объявления типов                                     */
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

//! обслуживание дерева объектов проекта
/*!
 	Класс несет на себе ответственность :
	- за создание и удаление объектов
	- отработку событий от всплывающих меню
	- прокрутка анимации
	- добавление/удаление объектов
*/
class SceneTreeView :	public CTreeView,
						public IViewMgr
{
	class view_activator;

	typedef std::map<CString, HTREEITEM> TreeItemIndex_t;
	typedef std::map<qdNamedObject*, std::vector<LPCTSTR> > AnimIndex_t;

	//! Возможные типы фильтров элементов в редакторе триггеров
	enum eFilterScheme{
		FS_NONE, ///< Нет фильтрации
		FS_ENTRY_FOR_DYN_OBJ, ///< Выделяются состояния динамических объектов
		FS_ENTRY_FOR_MOUSE, ///< Выделяются состояния мыши
		FS_ENTRY_FOR_PERS, ///< Выделяются состояния персонажей
		FS_ENTRY_FOR_DIALOGS,///< Выделяются диалоговые состояния
	};

	//! Показывать триггер для узла в древе
	bool showTriggerElement_;

	//! Текущий фильтр элементов в редакторе триггеров
	eFilterScheme m_eFilterScheme;

	//! Главное окно приложения
	CMainFrame			*m_ptrMainFrame;
	//! Класс, отображающий сцену
	CChildView			*m_ptrSceneView;
	//! Интерфейс редаткора триггеров
	ITriggerView		*m_ptrITView;

	//! индекс узлов дерева принадлежащих сценам
	/*!
	 	Необходим для правильной сортировки по алфавиту узлов сцен в дереве
	 */
	TreeItemIndex_t		m_mapSceneIndex;
	//! индекс узлов дерева,принадлежащих минииграм,
	/*!
		Необходим для правильной сортировки по алфавиту узлов миниигр в дереве
	*/
	TreeItemIndex_t		m_mapMinigameIndex;

	//! Набор изображений для узлов в дереве
	std::auto_ptr<CImageList> m_ptrTreeImageList;

	//! используется для того, чтобы загружать в меню главного окна свои менюхи
	CMenu m_menu;
	//! текст узла, для которого было вставлено меню
	CString m_strMenuOwnedItem;
	/*! \brief содержит имена временных файлов, которые надо будет удалить
		в конце работы программы
	*/
	CStringVect_t m_TmpFiles;

	//! поддержка drag-and-drop
	COleDataSource m_DataSource;
	friend class SceneTreeDropTarget;
	//! поддержка drag-and-drop
	std::auto_ptr<SceneTreeDropTarget>		m_ptrDT;

	std::map<qdNamedObject const*, NamedObjectList> *ptrObjectsDependencies_;
	//! узел, который перетягиваем. 
	HTREEITEM m_hDraggedItem;
	//! Узел объкта, если m_hDraggedItem состояние, либо просто родительский узел
	HTREEITEM m_hDraggedItemParent;
	
	//! Идентификатор видео таймера. 
	/*!
		Хранит ид таймера с тем, чтобы потом его можно было убить
	 	По приходу видео-таймера провереяем закончился ли видео ролик.
	 */
	UINT m_uiShowVideoTimer;

	//! содержит узел текущей сцены
	/*!
		нужен для того, чтобы быстро управлять доступностью кнопок
		и не искать его каждый раз
	 */
	HTREEITEM m_hCurrScene;

	//! Контейнер страничек со свойствами объектов проекта
	/*!
	 	В нужный момент напихиваем соответствующими страницами и показываем.
	 */
	std::auto_ptr<CMyPropertySheet>	m_ptrPropSheet;

	//! содержит объект, анимация которого прокручивается
	/*!
	 	При присваивании m_ptrPlayedObject указателя на новый объект
		для старого восстанавливается состояние по умолчанию, 
		либо ни какое состояние, если невозможно установить состояние по умолчанию
	 */
	state_guard					m_ptrPlayedObject;
	//! Состояние, анимация которого прокручивается в данный момент
	qdGameObjectState*			m_ptrPlayedState;
	//! Управляет доступом к m_ptrPlayedObject и m_ptrPlayedState
	/*!
	 	Разграничивает время доступа потока прокрутки анимации 
		и основоного потока приложения
		к m_ptrPlayedObject и m_ptrPlayedState
	 */
	CCriticalSection			m_csAnimationGurad;
	//! Описатель потока прокрутки анимации
	axl::auto_object<HANDLE>	m_hAnimThread;
	//! Ид потока прокрутки анимации
	DWORD						m_dwAnimThreadId;
	//! Указатель копию выбранной в дереве зоны
	/*!
	 	Редактирование свойств и контура происходит у копии зоны.
		Копия создается в момент выбора узла зоны в дереве и удаляется,
		когда выбирается другой узел
	 */
	std::auto_ptr<qdGridZone>	m_ptrEditedGZ;
	//! Указатель на менеджера маштаба
	IScaleMgr*					m_ptrIScaleMgr;

	///\brief сохраняем указатель на диалог,
	///из которого было затребовано либо редактирование КА
	///либо стыковка, либо редактирование боунда состояния
	IRequestingStateEdit *m_ptrIReqStateEdit;
	//! Диалог редактирующий КА
	std::auto_ptr<class CoordAnimDlg>	m_ptrCoordAnimDlg;
	//! Диалог помогающий в стыковке состояния
	std::auto_ptr<class StateCoordsStacker>  m_ptrStateStacker;
	//! Диалог редактирующий размеры состояния
	std::auto_ptr<class StateBoundDlg> m_ptrStateBoundDlg;

	//! Предыдущее состяния SceneView
	/*!
		Сохраняем его перед запуском редактирования координатной анимации, размеров
		или стыковки, чтобы затем восстановить его
	 */
	int m_iOldViewState;

	//! узкатель на диспетчер игры
	qdGameDispatcher* m_ptrGameDisp;

	//! Связь со списком объектов
	IObjectList* objectList_;

	class thread_suspender;
	friend thread_suspender;
	//! Тормозит и расстормаживает поток прокрутки анимации
	/*!
		Если нужно на время выполнения некоторой функции остановить 
		поток прокрутки анимации, то это то что нужно. См. OnProperies() для примера
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
	//! Переключает между собой окна редактора триггеров и редактора сцены
	/*!
	 	Хранит информацию о видимости окон редактора триггеров и редакотора сцены, 
		и предоставляет к ней доступ.
		Реализует механизм переключения между окнами
	 */
	class view_activator{
		//! Видимо ли окно редактора сцены
		/*!
		 	если true, то на экране редактор сцены
			false - редактор триггеров
		 */
		bool m_bIsSceneViewActive;
		//! указатель на дерево проекта
		SceneTreeView* m_ptr;
		
		const view_activator& operator = (const view_activator&);
		view_activator(const view_activator&);
	public:
		view_activator(SceneTreeView* p):m_bIsSceneViewActive(true), m_ptr(p){}
		//! доступ к информации о том, кто активен
		operator bool() const{
			return m_bIsSceneViewActive;
		}

		//! присваиваем новое значение и в соответствии с ним меняем видимость окон
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
	//! Активен ли редактор сцены
	/*!
		Хранит информацию об активности редактора сцены и управляет видимостью
		редактора сцены и редактора триггеров
	*/	
	view_activator m_IsSceneViewActive;

	//! Показывать или нет редактор триггеров
	/*!
		в момент уведомления о том, что выбран узел "Триггеры"
		показывать или нет редактор триггеров.
	*/
	bool m_bActivateTgrView;
protected:
	SceneTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(SceneTreeView);

	//! назначает указатель на CMainFrame
	void SetMainFrame(CMainFrame* ptrMainFrame){
		m_ptrMainFrame = ptrMainFrame;
	}
	//! Сеттер для m_ptrIScaleMgr
	void SetScaleMgr(IScaleMgr* psm);

	//! Загружает объекты сцены в дерево
	bool LoadGameObjects(CTreeCtrl& wndTree, HTREEITEM hParentItem,
							const qdGameObjectList& gol);
	//! Загружает объекты сцены в дерево
	bool LoadGameObjects(CTreeCtrl& wndTree, const qdGameObjectList& gol,
			HTREEITEM hStaticObj, HTREEITEM hAnimatedObj, HTREEITEM hMovingObj);
	//! Создает условия для загрузки глобальных объектов
	bool LoadGlobalGameObjects(HTREEITEM hParent);

	//! Загружает миниигры в дерево
	bool LoadMinigames(HTREEITEM hParent, const qdMiniGameList& mgl);
	//! Загружает миниигру в дерево
	bool LoadMinigame(HTREEITEM hParent, qdMiniGame const* pg);

	//! Загружает список музыкальных треков в дерево.
	bool LoadMusicTracks(HTREEITEM hParent, const qdMusicTrackList& mtl);
	//! Вставляет музыкальный трек в дерево.
	bool LoadMusicTrack(HTREEITEM hParent, class qdMusicTrack * pm);

	//! Подготовка почвы для загрузки списка видероликов
	bool LoadVideo(HTREEITEM hParent);
	//! Загружает списко видео роликов
	bool LoadVideo(HTREEITEM hPar, const qdVideoList& vl);

	//! Подготовка почвы для загрузки списка фонтов
	bool LoadFonts(HTREEITEM hParent);
	//! Загружает список фонтов
	bool LoadFonts(HTREEITEM hParent, qdFontInfoList const& l);

	//! загружает список сцен
	bool LoadScenes(HTREEITEM hParentItem, qdGameDispatcher* ptrDisp);
	//! загружает содержимое сцены: музыкальные треки, объекты, сетку и зоны на ней
	bool LoadSceneObjs(HTREEITEM hParentItem, qdGameScene* ptrGS);

	//! загружает в дерево список конечных экранов
	bool LoadGameEnds(HTREEITEM hParent);

	//! загружает счетчики в дерево
	bool LoadCounters(HTREEITEM hParent);

	//! вставляет 2  узла в дерево для состояний зоны
	bool InsertGZStates(CTreeCtrl& tree, HTREEITEM hZoneItem, qdGridZone* pz);

	//! вставляет зону в дерево
	HTREEITEM InsertGZ2Tree(CTreeCtrl& tree, HTREEITEM hParentItem, 
						qdGridZone* pz, int iItemImage, HTREEITEM hInsertAfter);
	//! Загружает список зон из сцены
	bool LoadGridZones(HTREEITEM hParentItem, qdGameScene const* ptrScene);

	//! вспомогательная функция для загрузки списка объектов
	template<class _List>
		bool lioLoadHelper(HTREEITEM hItemPar, const _List &lst, DWORD strId, 
				qdNamedObjectType ot, UINT uiObjsImage, UINT uiObjImage,
				HTREEITEM hInsertAfter/* = TVI_LAST*/);

	//! Используется ли тип ячеек в наборах ячеек
	bool IsInventoryTypeUsed(int type);
	//! Назначен ли тип ячеек какому-либо объекту
	bool IsInventoryTypeUsedInObjs(int type);
	//! Добавляет тип ячеек в инвентори
	void AddInventoryCellTypes();
	//! Устанавливает имя инвентори всем персонажам проекта
	void SetInvToPers(LPCTSTR name);
	//! загружаем список доступных инвенторей
	bool LoadInventory(HTREEITEM hParent);
	
	//! Загружает мышь и её состояния
	bool LoadMouse(HTREEITEM hPar, qdGameObjectMouse* ptrMouse);

	//! Управляет загрузкой всего интерфейса
	bool LoadInterface(HTREEITEM hParent);
	//! Загружает интерфейсные экраны
	bool LoadInterfaceScreens(HTREEITEM hParent,const qdInterfaceDispatcher* pIDisp);
	//! Загружает содержимое интерфейсного экрана
	bool LoadInterfaceScreen(HTREEITEM hParent, qdInterfaceScreen* psi);
	//! Загружает в дерево элемент интерфейса(кнопку, слайдер, фон)
	bool LoadInterfaceElement(HTREEITEM hParent, qdInterfaceElement* pele);
	//! Загружает в дерево состояния интерфейсных кнопок 
	bool LoadInterfaceBtnStates(HTREEITEM hParent, qdInterfaceButton const* pbtn);

	//! показывает всплывающее меню соответственно типу объекта
	void ShowCorrespondingMenu(HTREEITEM hItem, bool bIsCatalog,
					const qdNamedObjectBase* pno,bool bIsGlobal);

	//! показывает всплывающее меню для узла
	void ShowCorrespondingMenu(HTREEITEM hItem);

	//! Модифицирует меню в соответствии с \a pobj и другими внешними условиями
	void ModifyMenu(qdNamedObjectBase const* pobj, HTREEITEM hItem, 
								DWORD dwMenuId, CMenu* pm);
	//! Модифицирует меню для qdGameObjectState* в \a pobj
	void mmStateNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! Модифицирует меню для qdGameObjectMoving* в \a pobj
	void mmMovingObjNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! Модифицирует меню для qdGameObjectAnimated* в \a pobj
	void mmAnimatedObjNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! Модифицирует меню для qdGameObjectStatic* в \a pobj
	void mmStaticObjNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! Модифицирует меню для qdInvetory* в \a pobj
	void mmInventoryObjNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! Модифицирует меню для qdMusicTrack* в \a pobj
	void mmMusicTrackNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! Модифицирует меню для каталога статических объектов
	void mmStaticObjsNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! Модифицирует меню для каталога анимированных объектов
	void mmAnimatedOjbsNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! Модифицирует меню для каталога персонажей
	void mmMovingObjsNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! Модифицирует меню для qdGridZone* в \a pobj
	void mmGridZoneNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! Модифицирует меню для qdVideo* в \a pobj
	void mmVideoNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! Модифицирует меню для каталога инвентори
	void mmInventoryObjsNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	//! Модифицирует меню для qdGameScene* в \a pobj
	void mmSceneNode(HTREEITEM hItem, const qdNamedObject* pobj, CMenu* pm);
	
	//! показывает свойства для объекта
	bool ShowCorrespondingPropDlg(qdNamedObjectBase* ptrObj, int iActivePage, bool bIsObj);
	//! показывает свойства для объектов унаследованных от qdNamedObject
	bool ShowCorrespondingPropDlg(qdNamedObject* ptrObj, int iActive, bool bIsObj);
	//! показывает свойства для объектов унаследованных от qdInterfaceObjectBase
	bool ShowCorrespondingPropDlg(qdInterfaceObjectBase* ptrObj, int iActive);
	
	//! выравниваем размер bitmap"а в редаткоре сцены, так чтобы влезало все изображение
	void AdjustBitmapSize();

	//! создаёт набор строк, из имен узлов, по которому можно найти узел соответствующий объекту
	std::auto_ptr<CStringArray> CreateTreePath(qdNamedObjectBase const * pobj);

	//! Создаёт сцену
	qdGameScene* CreateScene(qdGameDispatcher* ptrDisp, CString& strName);

	//! Создает конечный экран
	qdGameEnd* CreateGameEnd(CString const& name, CString const& scrName);
	//! Создает счетчик
	qdCounter* CreateCounter(CString const& name);
	
	//! Способы создания инвентори
	enum eInventoryCrtMode{
		ICM_USE_EXISTING_CELL_TYPES,///< При создании используются существующие наборы ячеек
		ICM_EMPTY///< Создается пустым
	};
	//! Создает новый инвентори
	qdInventory* CreateInventory(eInventoryCrtMode mode);
	
	//! Вставляет объект инвентори в дерево
	HTREEITEM InsertInventoryToTree(qdInventory* pinv);
	//! Устанавливает уникально имя и вставляет инвентори в диспетчер игры
	bool InsertInvToGD(qdInventory* pinv, CString strName);

	//! создаёт и добавляет сцену в дерево
	HTREEITEM AddScene(HTREEITEM hItemParent, CString& strName);

	//! Показывает узел в дереве
	bool ShowItem(HTREEITEM hItem);

	//! добавляет объект и в сцену и в дерево
	bool AddSceneObj(qdGameObject* ptr, HTREEITEM hCatalogItem, 
					const CString& strName, UINT uiImage);

	//! \brief Для объектов унаследованных от qdNamedObject определяет тип 
	//! объекта и производит тест на уникальность имени в соответствующем списке.
	bool CorrespondingTestName(LPCTSTR strName, qdNamedObject* ptrObj);
	//! \brief Для объектов унаследованных от qdInterfaceObjectBase определяет тип 
	//! объекта и производит тест на уникальность имени в соответствующем списке.
	bool CorrespondingTestName(LPCTSTR strName, qdInterfaceObjectBase* pobj);
	
	//! Назначает выбранным в редакторе сцены выбранный в дереве объект.
	void SelectObjectInSceneView(qdGameObject* pno);

	//! отработка уведомления о том, что узел стал выделенным
	void OnSelchanged4New(HTREEITEM hItem);
	//! отработка уведомления о том, что узел перестал быть выделенным
	void OnSelchanged4Old(HTREEITEM hItem);
		
	//! Выбирает сцену
	bool SelectScene(CTreeCtrl& tree, HTREEITEM hItem);

	//! проверяем выбранный экран и, если не совпадает, выбираем его)
	bool SelectInterfaceScreen(LPCTSTR name);

	//! Отображает содержимое каталога анимации
	void ShowAnimationCatalogDlg(HTREEITEM hItem, int eAction, bool bUniqueList);
	//! Отображает содержимое каталога звуков
	void ShowSoundCatalogDlg(HTREEITEM hItem, bool bUniqueList);

	//! \brief Ищет в сценах пустые объекты-копии глобального объекта
	//! и удаляет их
	void RemoveEmptyCopyOfGlobal(qdGameObject* pobj);
	//! Ищет в сценах пустые объекты-копии для анимированных объектов и персонажей
	void aoRemoveEmptyCopyOfGlobal(qdGameObjectAnimated* pobj);

	//! Переименовывает в сценах копии глобального объекта
	void RenameCopyOfGlobal(LPCTSTR lpszOld, LPCTSTR lpszNew);

	//! Создает объекты-копии всех глобальных объектов в сценах, где копий еще нет
	void GlobalObjsToScenes();
	//! Создает объекты-копии \a pobj во всех сценах, где его копии еще нет
	void GlobalObjToScenes(qdGameObject const* pobj);

	//! Создаёт список объектов-копий глобального объекта
	void GetCopyOfGlobalList(LPCTSTR name, CHECK_FUN f, qdGameObjectList* pres);
	
	//! \brief удаляет объект из соответсвующего ему контейнера
	//! и выполняет действия, необходимые для безопастного удаления
	bool DelCorrespondingObj(qdNamedObjectBase* ptrObj, bool bAsk);

	//! \brief Удаляет все объекты из указанного списка 
	//  путём вызова DelCorrespondingObj
	template<class ObjectContainer>
	void cleanupObjects(ObjectContainer& container);
//	//! \brief Удаляет все состояния динамических объектов и персонажей. 
//	void CleanupAnimatedObjectStates(qdGameObjectAnimated* pobj);

	//! Редактирует свойства состояния 
	bool EditState(HTREEITEM hItem);
	//! Редактирует свойства состояния 
	bool EditState(qdGameObjectState* pstate);

	//! удаляет все созданные временные файлы
	void RemoveTmps();

	//! \brief получает имя временного файла для скрипта и кладет полный путь 
	//! в список временных файлов
	CString GetTemporaryFileName();

	//! Запускает показ анимации для состояния
	void ShowStateAnimation(qdGameObjectState * pno);
	//! Останавливает проигрываение состояний
	void StopAnimation(qdGameObjectState* ptrState);

	//! Вызывается перед сменой окон редактора триггеров и редактора сцены.
	bool OnChangingViews(bool bIsSVActiveOld, bool bIsSVActiveNew);
	//! Вызывается после смены окон редактора триггеров и редактора сцены.
	void OnChangeViews(bool bIsSVActiveOld, bool bIsSVActiveNew);

	//! Показать(активировать) редактор сцены
	void ShowSceneView();
	//! Показть(активировать) редактор триггеров
	void ShowTEView();

	//! Можно ли удалить узел
	bool CanRemoveItem(HTREEITEM hItem);

	//! удаляет узел из дерева и соответствующий узлу объект из проекта
	void RemoveItem(HTREEITEM hItem, bool bAsk);

	//! дополнительная работа связанная с удалением состояния
	bool riStateSuppl(qdNamedObject *pno, HTREEITEM hItem);
	//! дополнительная работа связанная с удалением состояния интерфейсной кнопки
	bool riIBtnStateSuppl(qdInterfaceObjectBase *piob, HTREEITEM hItem);

	//! Удаляет состояние
	bool RemoveState(qdGameObjectState* ptrObj, bool bAsk);

	//! Можно ли удалить инвентори
	bool CanRemoveInventory(bool bAsk, qdInventory* pInv, 
							qdGameObjectList* plist4Remove);
	//! Удаляет инвентори
	bool RemoveInventory(qdInventory* pInv, bool bAsk);

	//! \brief Удаляет указанный набор ячеек из инвентори. Проверяет используется ли 
	//! вид ячеек из этого набора и если нет, то удаляет его.
	bool DeleteInventoryCellset(qdInventory* pinv, int icellset);

	//! Является ли состояние глобальным
	bool IsGlobalState(const qdNamedObject* pst, HTREEITEM hItem);

	//! Запускает и останавливает видео
	void PlayStopVideo(qdVideo * pv);
	//! Останавливает видео
	void StopVideo(qdVideo* pv);
// Operations
public:
	//! Назначает указатель на список объектов
	void SetObjectList(IObjectList* objectList);
	//! возвращает диспетчер игры
	qdGameDispatcher* GetGameDispatcher() const{
		return m_ptrGameDisp;
	}

	//! вставляет узел в дерево, а потом загружает объекты сцены
	HTREEITEM LoadScene(HTREEITEM hPar, qdGameScene* pscene);

	//! Сохраняет в ини файле настройки проекта
	void SaveIni();
	//! Читает из ини файла настройки проекта
	void ReadIni();

	/*!
	 	Задает редактор сцены и редактор триггеров
		\param ptrView		- редактор сцены
		\param ptrTEView	- редактор триггеров
		\return всегда true
	*/
	bool set_views(CChildView* ptrView, ITriggerView* ptrTEView){
		m_ptrSceneView = ptrView;
		m_ptrITView = ptrTEView;
		return true;
	}
	//! Загружает структуру проекта в treectrl
	bool LoadStructTree(qdGameDispatcher* ptrGD);

	//! Показывает редактор сцены
	BOOL ShowSceneEditor();
	//! Показывает редатор триггеров
	BOOL ShowTriggerEditor();

	//! Ищет соответствующий объекту узел в дереве
	HTREEITEM FindObjectItem(const qdNamedObjectBase* ptrObj);

	//! показывает всплывающее меню для данного объекта
	void ShowCorrespondingMenu(const qdNamedObjectBase* pobj);

	//! выбирает в дереве соответствующий объекту узел
	bool ShowObjectInTree(qdNamedObjectBase const* ptrObj);
	//! Показывает диалог свойств объекта
	bool ShowProperties(qdNamedObjectBase* ptrObj, int iActivePage);
	//! Удаляет объект
	bool DeleteObject(qdNamedObjectBase* ptrObj, bool bAsk);

	//! Прекращает процедуру редактирования границ состояния
	void EndEditStateDims(bool bSave);

	//! Приводит все в начальное состояние.
	bool reset();
	
	//! Выбирает узел сцены в дереве
	BOOL ActivateScene(qdGameScene const* ptrScene);

	//! Добавить сцену
	bool AddScene(CString &strName, bool bShowNew);

	//! Добавляет персонаж с первой походкой из списка походок
	qdGameObjectMoving*		AddPersWith1stGait(qdGameScene* ptrScene, 
												const CString& strName);
	//! Добавить копию глобального объекта в сцену
	qdGameObject*			AddGlobalObj2Scene(qdGameScene* pscene, 
												qdGameObject const* pobj);

	//! Добавить глобального персонажа
	qdGameObjectMoving*		AddGlobalPersonage(qdGameDispatcher* ptrDisp, 
												CString &strName, bool bShow);
	//! Добавить персонажа
	qdGameObjectMoving*		AddPersonage(qdGameScene* ptrScene, 
										const CString &strName, 
										bool bShow, bool bAdd2Tree);
	//! Добавить динамический объект
	qdGameObjectAnimated*	AddAnimated(qdGameScene* ptrScene, 
										const CString &strName, bool bShow);
	//! Добавить статический объект
	qdGameObjectStatic*		AddStatic(qdGameScene* ptrScene, 
										const CString &strName, bool bShow);
	//! Добавить зону сетки в дерево
	bool					AddGridZone2Tree(qdGridZone* ptrZone);

	//! Создает миниигру
	qdMiniGame*				CreateMiniGame(const CString& strName,
								const CString& strDllName, const CString& strConfigName);
	//! объеденяет в себе дейтсвия направленнные на добавление новой миниигры
	bool					AddMiniGame();

	//! Вставляет миниигру в дерево
	HTREEITEM InsertMGToTree(HTREEITEM hParent, const qdMiniGame* pmg);

	//! вставляет в миниигру состояния "проигрыш" и "выигрыш"
	bool AddSimpleMGState(qdMiniGame* pgame);

	//! Создает музыкальный трек
	std::auto_ptr<class qdMusicTrack> CreateMusicTrack(const CString& strName);
	//! Создает муз. трек и добавляет его в сцену
	bool AddMusicTrack(qdGameScene* pscene, HTREEITEM hMTCatalog);

	//! Создает объект, описывающий фонт
	std::auto_ptr<class qdFontInfo> CreateFont(CString const& strName);
	//! Добавляет существующий на диске фонт в дерево
	bool AddFont(HTREEITEM hCatalog);
	//! Создает фонт и добавляет его в дерево
	bool CreateFont(HTREEITEM hCatalog);
	//! Запускает визард создания нового проекта
	void ShowNewProjWiz();

	//! Функция прокрутки анимации
	int ShowAnimation();

	//! Обработчик окончания создания состояния-маски
	bool OnEndStateMaskCreate(qdGameObjectStateMask* ptrStateMask);
	//! Сохраняет/отменяет изменения для зоны на сетке
	bool OnSaveGZChanges(bool bSave);

	//! Пробегает по сценам и обновляет в дереве списки объектов, указанного типа
	void UpdateCatalogsInScenes(qdNamedObjectType type);
	//! обновляет в дереве список объектов указанного типа для указанной сцены
	void UpdateObjCatalogInScene(qdNamedObjectType type, qdGameScene const* pscene);
	
	//! Начали редактировать размеры динамического объекта
	bool OnEditAnimatedDims(qdGameObjectAnimated* ptrObj);
	//! Начали редактировать направление анимированного объекта
	bool OnEditAnimatedDir(qdGameObjectAnimated* ptrObj);

	//! После добавления объекта в триггер делаем что-нибудь
	void OnAddObjToTrigger(qdNamedObject* pno);
	//! После удаление объекта из триггер делаем что-нибудь
	void OnRemoveObjFromTrigger(qdNamedObject* pno);

	//! Запускает игру с указанной сценой и в указанном режиме
	void RunScene(const CString& strSceneName, ptree_api::eRunMode emode);
	//! запускаем миниигру
	void RunMiniGame(qdMiniGame* pmg);

	//! Запускает редактор диалогов
	void RunDialogEditor();

	/************************************************************************/
	/* Стыковка и координатная анимация                                     */
	/************************************************************************/

	//! Прекращает редактирование координатной анимации
	bool EndEditCoordAnimationMode(bool bCancel);
	//! Прекращает стыковку состояния
	void EndStateStack(bool bCancel);

	//! Сообщает можно ли закрыть приложение
	CString CanClose();

	/* Функции добавления интерфейсных элементов */

	//! Добавляет интерфейсный экран
	qdInterfaceScreen* AddInterfScr(HTREEITEM hItemInterf, const CString& name);
	//! Добавляет интерфейсную кнопку
	qdInterfaceButton* AddInterfBtn(qdInterfaceScreen* pscr, 
		HTREEITEM hItemScr, const CString& name);
	//! Добавляет сейв
	qdInterfaceSave* AddInterfSave(qdInterfaceScreen* pscr, 
		HTREEITEM hItemScr, const CString& name);
	//! Добавляет фон
	qdInterfaceBackground* AddInterfBack(qdInterfaceScreen* pscr, 
		HTREEITEM hItemScr, const CString& name);
	//! Добавляет регулятор
	qdInterfaceSlider* AddInterfSldr(qdInterfaceScreen* pscr, 
		HTREEITEM hItemScr, const CString& name);
	//! Добавляет окно текстов
	qdInterfaceTextWindow* AddInterfTextWindow(qdInterfaceScreen* pscr, 
		HTREEITEM hItemScr, const CString& name);
	//! Добавляет состояние интерфейсной кнопке
	int AddInterfBtnState(qdInterfaceButton* pb, 
		HTREEITEM hItemBtn, const CString& name);

	//! Добавляет конечный экран
	qdGameEnd* AddGameEnd(HTREEITEM hParent, 
		CString const& name, CString const& scrName);
	//! Добавляет счетчик
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

	//! Переименовывает объект
	bool RenameObject(qdNamedObject* pno, LPCTSTR lpszNewName);
	//! Переименовывает интерфейсный объект
	bool RenameObject(qdInterfaceObjectBase* piob, LPCTSTR lpszNewName);
	//! Переименование инвентори
	bool RenameInventory(qdGameDispatcher* disp, 
		qdInventory* inventory, LPCTSTR lpszNewName);
	//! Обновляет заголовки элементов триггеров,в которые входит данный объект
	void UpdateObjTrgEleTitles(qdNamedObject* pno);
	//! Обнвовляет все загловоки элементов триггеров
	void UpdateAllTitles();
	//! Обновляет заголовки элементов триггеров, для состояний объекта
	void UpdateTrgEleTitle4ObjStates(qdGameObjectAnimated* pobj);

	//! Обновляет заголовки элементов триггеров, для объектов из сцены.
	void UpdateTrgEleTitle4SceneObjs(qdGameScene* pscene);
		
	//! Создает фон для сцены
	bool CreateBackground(HTREEITEM hSceneItem, qdGameScene* ptrScene);
	//! Добавляет статический объект
	bool AddStatics(HTREEITEM hSceneItem);

	//! Создает объекты игры
	template<class GameObj>
		GameObj* CreateGameObject(DWORD dwErrMsg);
	//! Создает персонажа
	qdGameObjectMoving*		CreatePersonage();
	//! Создает динамический объект
	qdGameObjectAnimated*	CreateAnimated();
	//! Создает статический объект
	qdGameObjectStatic*		CreateStatic();

	//! \brief Добавляет объект в сцену с указанным именем.
	//! если надо подставляет имя по умолчанию
	bool AddToScene(qdGameScene* ptrScene, qdGameObject* ptrObj, 
				const CString& strName);

	
	//! Добавить в дерево GameObject
	HTREEITEM AddGOToTree(HTREEITEM hItemParent, qdGameObject* ptr, UINT uiImage);

	//! Добавить в дерево GameObject, при этом находит соответствующий каталог 
	HTREEITEM AddGOToTree2(HTREEITEM hSceneItem, UINT uiCatNameID, 
											qdGameObject* ptr, UINT uiImage);

	//! Удаляет дочерние узлы
	void DeleteChildern(HTREEITEM hPar);
	//! Обвновляет содержимое каталога объектов указанного типа
	void UpdateSceneObjsCatalog(qdNamedObjectType type, 
							HTREEITEM hCatalog, qdGameScene const* ptrScene);
	//! Обновляет содержимое каталога объектов указанного типа
	void UpdateObjsCatalog(qdNamedObjectType type, HTREEITEM hCatalog, 
										const qdGameObjectList& gol);

	//! Синхронизируем каталог состояний глобального объекта с его копиями в сценах
	void SyncGlobalStatesCatalog(qdGameObjectAnimated* ptrObj);

	//! Создатет цепочку триггеров.
	qdTriggerChain* CreateTriggerChain();
	//! Добавить цепочку триггеров
	qdTriggerChain* AddTriggerChain(HTREEITEM hPar, const CString& strName, 
																bool bShow);
	//! Вставляет в персонаж первую походку
	bool InsertFirstGaitToPersonage(qdGameObjectMoving* ptrPers);

	//! Обноволяет изображение узла объекта
	void UpdateObjImage(const qdGameObject* pobj);
	//! Обновляет изображение узла состояния
	void UpdateStateImage(qdGameObjectState* pstate, HTREEITEM hItem);

	//! Обновляет изображения узлов для всех состояний объекта
	void UpdateObjStatesImages(qdGameObjectAnimated const* pobj, HTREEITEM hItem);

	//! Свернуть все сцены
	void CollapseAllScenes(qdGameScene* psceneEx);

	//! Убирает чек бокс у узла
	BOOL SetNoCheck(HTREEITEM hItem){
		return GetTreeCtrl().SetItemState(hItem, 0, TVIS_STATEIMAGEMASK);
	}
	//! Устанавливает состояние чекбокса(отмеченный или нет)
	BOOL SetCheck(HTREEITEM hItem, BOOL bCheck){
		return GetTreeCtrl().SetItemState(hItem, 
			INDEXTOSTATEIMAGEMASK((bCheck ? 2 : 1)), TVIS_STATEIMAGEMASK);
	}

	//! Возвращает состояние чекбокса(отмеченный или нет)
	int GetCheck(HTREEITEM hItem)
	{
		UINT uState = GetTreeCtrl().GetItemState(hItem, TVIS_STATEIMAGEMASK);
		return (static_cast<BOOL>((uState >> 12) -1));
	}
	//! На изменения состояния чекбокса у узал сеточной зоны
	void OnCheckChangeGZone(int iCheck, qdGridZone* pzone);
	//! На изменение состояния чекбокса у узла сетки
	void OnCheckChangeNet(int iCheck, HTREEITEM hItem);
		
	//! Для узла состояния выбирает его владельца в редакторе сцены.
	bool SelectOwner(HTREEITEM hItem);

	//! Возвращает узел-каталог для видео
	HTREEITEM GetVideoCatNode();

	//! Выбирает и добавляет видео-ролик в проект
	bool AddVideo();
	//! Добавляет видо-ролик в проект
	bool AddVideoFile(const CString &strFileName);

	//! Заполняет меню выпадающее при нажатии на стрелочку у кнопки "Т".
	void FillMenuForTriggers(CMenu*pm);
	//! Обрабатывает команды от меню сделанного FillMenuForTriggers
	void HandleEditTriggersMenu(HMENU hm, DWORD pos);

	//! Останавливает поток проигрывания анимации
	bool SuspendThread();
	//! Останавливает поток проигрывания анимации и очищает проигрываемое состояние
	bool SuspendAndClear();
	
	//! Запускает поток проигрывания анимации
	bool ResumeThread();

	//! Устанавливает проигрываемое состояние и запускает поток проигрывания анимации
	bool SetAndResume(qdGameObjectState* ptrState);

	//! Обработчик события "бросили" при drag and drop
	BOOL OnDropData(void* pData, DROPEFFECT dropEffect, const CPoint& point);

	//! Бросили собственное состояние под персонажа
	BOOL oddOwnStateMovingObj(HTREEITEM hObjItem, HTREEITEM hDropTarget,
		qdGameObjectState* dragged_state, DROPEFFECT dropEffect);
	//! Бросили собственное состояние под динамический объект
	BOOL oddOwnStateAnimObj(HTREEITEM hObjItem, HTREEITEM hDropTarget,
		qdGameObjectState* dragged_state, DROPEFFECT dropEffect);
	//! Бросили собственное состояние под мышь
	BOOL oddOwnStateMouseObj(HTREEITEM hObjItem, HTREEITEM hDropTarget,
		qdGameObjectState* dragged_state, DROPEFFECT dropEffect);

	//DropData для отдельных типов
	//! Бросили состояние
	BOOL oddOwnState(qdGameObjectState* pstate, DROPEFFECT dropEffect, 
		const CPoint& point);
	//! Бросили глобальный объект
	BOOL oddGlobalObject(qdGameObject* pobj, DROPEFFECT dropEffect, 
		const CPoint& point);
	//! Бросили динамический объект
	BOOL oddAnimatedObject(qdGameObjectAnimated* pobj, DROPEFFECT dropEffect,
		const CPoint& point);
	//! Бросили персонаж
	BOOL oddMovingObject(qdGameObjectMoving* pobj, DROPEFFECT dropEffect,
		const CPoint& point);
	//! Брослили сеточную зону
	BOOL oddGridZone(qdGridZone* pobj, DROPEFFECT dropEffect, const CPoint& point);
	//! Бросили цепочку триггеров
	BOOL oddTriggerChain(qdTriggerChain* pch, DROPEFFECT dropEffect, const CPoint& pnt);
	//! Бросили видео ролик
	BOOL oddVideo(qdVideo* video, DROPEFFECT dropEffect, const CPoint& pnt);
	//! Бросили инвентори
	BOOL oddInventory(qdInventory* inventory, DROPEFFECT dropEffect, CPoint const& point);

	//! Можем ли кинуть то что тащиили(drag and drop)
	DROPEFFECT	CanDrop(const CPoint& point, COleDataObject* pDataObject);
	//частные случаи для CanDrop(cd)

	//! Можем ли бростиь глобальный объект
	DROPEFFECT  cdGlobalObj(HTREEITEM hItem, const CPoint& point, 
							qdGameObject* pobj);
	//! Можем ли бростиь персонаж
	DROPEFFECT  cdMovingObj(HTREEITEM hItem, const CPoint& point, 
						qdGameObjectMoving* pobj);
	//! Можем ли бростиь динамический объект
	DROPEFFECT  cdAnimatedObj(HTREEITEM hItem, const CPoint& point, 
						qdGameObjectAnimated* pobj);
	//! Можем ли бростиь глобальное состояние
	DROPEFFECT  cdGlobalState(HTREEITEM hItem, const CPoint& point, 
												qdGameObjectState* pstate);
	//! Можем ли бростиь собственное(неглобальное) состояние
	DROPEFFECT  cdOwnState(HTREEITEM hItem, const CPoint& point, 
											qdGameObjectState* draged_state);
	//! Можем ли бростиь собственное состояние под персонаж
	DROPEFFECT cdOwnStateUnderMoving(HTREEITEM hItem, HTREEITEM hObj,
												qdGameObjectState* draged_state);
	//! Можем ли бростиь сеточную зону
	DROPEFFECT cdGridZone(HTREEITEM hItem,  const CPoint& point, qdGridZone* pgz);
	//! Можем ли бростиь цепочку триггеров
	DROPEFFECT cdTriggerChain(HTREEITEM hItem, const CPoint& point, qdTriggerChain* pch);
	//! Можем ли бросить видео
	DROPEFFECT cdVideo(HTREEITEM hItem, CPoint const& point, qdVideo* pvideo);
	//! Можем ли бросить инвентори
	DROPEFFECT cdInventory(HTREEITEM hItem, CPoint const& point, qdInventory* pinvetory);
	//! Находится ли узел под динамическим объектом или персонажем
	HTREEITEM	IsUnderAnimated(HTREEITEM hItem, bool *pfIsMoving = NULL);
	//! Находтится ли узел под одним из каталогов состояний персонажа
	HTREEITEM	IsUnderStateCatalog(HTREEITEM hItem, bool *pfIsWalk = NULL);
	//! Можно ли бросить состояние под персонаж
	bool		CanDropUnderMoving(HTREEITEM hMovingObjNode);
	//! Можно ли бросить состояние под динамический объект
	bool		CanDropUnderAnim(HTREEITEM hAnimObjNode);

	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! статического объекта
	bool OnSelectStaticObjNode(qdNamedObject* ptrObj, 
					bool bIsObj, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! анимированного объекта
	bool OnSelectAnimatedObjNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	bool OnSelectAnimatedObjNode2(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! персонажа
	bool OnSelectMovingObjNode(qdNamedObject* ptrObj, 
					bool bIsObj, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! сцены
	bool OnSelectSceneNode(qdGameScene* ptrScene, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! сцены. Обработчик действует, когда активен редактор триггеров.
	bool OnSelectSceneNode2(qdGameScene* ptrScene, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! состояния
	bool OnSelectStateNode(qdNamedObject* pno, bool bIsObj, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! состояния. Обработчик действует, когда активен редактор триггеров.
	bool OnSelectStateNode2(qdNamedObject* pno, bool bIsObj, bool bSelect);

	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! сеточной зоны
	bool OnSelectGridZoneNode(qdNamedObject* ptrObj, 
					bool bIsObj, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! мыши
	bool OnSelectMouseNode(qdNamedObject* ptrObj, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! видое-ролика
	bool OnSelectVideoNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для корневого 
	//! узла.(Узел с текстом "Глобальные объекты")
	bool OnSelectGlobalNode(qdNamedObject* ptrObj, bool bSelect);

	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! цепочки триггеров
	bool OnSelectTriggerNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! цепочки триггеров. Обработчик действует, когда активен редактор триггеров
	bool OnSelectTriggerNode2(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! инвентори
	bool OnSelectInventoryNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! миниигры
	bool OnSelectMinigameNode(qdNamedObject* pobj, bool bIsObj, bool bSelect);

	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! состояния сеточной зоны
	bool OnSelectGZStateNode(HTREEITEM hItem, bool bSelect);

	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! "Интерфейс"
	bool OnSelectInterfaceNode(qdInterfaceObjectBase* pobj, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! интерфейсного экрана
	bool OnSelectInterfaceScreenNode(qdInterfaceObjectBase* pobj, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! интерфейсного элемента
	bool OnSelectInterfaceElementNode(qdInterfaceObjectBase* pobj, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! состояния интерфейсного элемента
	bool OnSelectInterfaceElementStateNode(qdInterfaceObjectBase* pobj, bool bSelect);

	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! музыкального трека
	bool OnSelectMusicNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);
	//! \brief Обработчик события Получил/Потерял состояние "Выбран" для узла 
	//! финального(конечного) экрана
	bool OnSelectGameEndNode(qdNamedObject* ptrObj, bool bIsObj, bool bSelect);

	void ReselectZones();

	BOOL MergeTriggers(qdTriggerChain const* from, qdTriggerChain* to);
	/************************************************************************/
	/* Сохранение в ини-файл состояния дерева                               */
	/************************************************************************/
	//! Сохраняет в настройках проекта выбранный узел
	void SaveSelectedItem(interface IProjectSettings* pps);
	//! Сохраняет в настройках проекта текущую сцену
	void SaveCurrentScene(interface IProjectSettings* pps);
	//! //! Сохраняет в настройках проекта состояния узлов сцен. Открытые они или закрытые
	void SaveSceneItemsState(interface IProjectSettings* pps);
	//! //! Сохраняет в настройках проекта  режим прорисовки сцены
	void SaveNetDrawMode(interface IProjectSettings* pps);
	//! Сохраняет в настройках проекта активный редактор
	void SaveActiveView(interface IProjectSettings* pps);
	//! Сохраняет в настройках проекта флаг showTriggerElement_
	void SaveShowTriggerElement(interface IProjectSettings* pps);
	/************************************************************************/
	/* Чтение из ини-файла                                                  */
	/************************************************************************/
	//! \brief Считывает состояния узла из ини файла проекта и приводит его сотояние
	//! в соответсвии с считаным значением
	void AdjustItemState(const CString&strSecName, LPCTSTR lpszItemName, 
		interface IProjectSettings* pps, HTREEITEM hPar);
	//! Считывает из настроек проекта текущую сцену
	void ReadCurrentScene(interface IProjectSettings* pps);
	//! Считывает из настроек проекта состояние узлов в сцене
	void ReadSceneItemState(interface IProjectSettings* pps);
	//! Считывает из настроек проекта состояние сетки для сцены и устанавливает его
	void AdjustNetDrawMode(qdGameScene* ptrScene, LPCTSTR lpszItemName, 
		interface IProjectSettings* pps);
	//! Для сеток всех сцен устанавливает режим их прорисовки
	void ReadNetDrawMode(interface IProjectSettings* pps);
	//! Считывает из настроек проекта активный редактор и устанавливает его
	void ReadActiveView(interface IProjectSettings* pps);
	//! Считывает из настроек проекта флаг showTriggerElement_
	void ReadShowTriggerElement(interface IProjectSettings* pps);
	
	/************************************************************************/
	/* работа с редактром триггеров                                         */
	/************************************************************************/
	//! \brief Устанавливает букву "Т" рядом с узлами объектов, которые входят в 
	//! триггера
	void SetTgrIcon4All(BOOL bSet);
	//! Устанавливает указанную(черную или красную) букву "Т".Либо убирает ее
	void SetInTgrIcon(qdTriggerChain* pchain, BOOL bSet, int image_id);
	//! Показывает все триггера в редакторе триггеров
	void ShowAllTriggers();
	/************************************************************************/
	/* Стыковка и координатная анимация                                     */
	/************************************************************************/
	//! Инициирует стыковку состояния
	LRESULT BeginStackState(qdGameObjectState* pstate);
	//! Инициирует редактирование КА
	LRESULT BeginCAEdit(qdGameObjectState* pstate);
	//! Инициирует редактирование размеров состояния
	LRESULT BeginEditStateDims(qdGameObjectState* pstate);

	/*
	 *	Удаление пустот в изображении(анимации) с сохранением позиции на экране
	 */
	//! Удаление пустот из анимации состояния
	bool RemoveImageEdges(qdGameObjectState* pst);
	//! Удаляет пустоты для всех статических состояний объекта и сдвигает его центр
	bool RemoveEdgesForObj(qdGameObjectAnimated* pobj);
	//! Удаление пустот для интефейсных объектов
	void RemoveEdgesInInterface(qdNamedObjectBase* pobj);

	//! Удаление пустот для интерфейсного фона
	void RemoveEdgesInInterfaceBackground(qdInterfaceBackground* pobj);
	//! Удаление пустот для состояния интерфейсного элемента
	void RemoveEdgesInInterfaceElementState(qdInterfaceElementState* pobj);
	//! Удаление пустот для сейва
	void RemoveEdgesInInterfaceSave(qdInterfaceSave* pobj);
	//! Удаление пустот бегунка
	void RemoveEdgesInInterfaceSlider(qdInterfaceSlider* pobj);
	//! Удаление пустот экрана
	void RemoveEdgesInInterfaceScreen(qdInterfaceScreen* pobj);
	//! Удаление пустот интерфейсной кнопки
	void RemoveEdgesInInterfaceButton(qdInterfaceButton* pobj);

	//! Конвертируем tga в qda
	void ConvertTGAtoQDAInInterface(qdNamedObjectBase* pobj);

	//! Ищет, где еще используется анимация из состояния и сдвигает эти состояния
	void StateOffsetInOtherAnimUses(qdGameObjectStateStatic* query_src, 
		Vect2i const& offset);
	//! Производит сдвиг состояний, которые используют анимацию из \a anim_lst
	void StateOffsetInOtherAnimUses4Scene(qdGameObjectStateStatic* query_src, 
		Vect2i const& offset, qdGameScene* pscene, std::vector<LPCTSTR> const& anim_lst);
	//! \brief Для каждого диспетчера(сцены или глобального диспетчера) формирует 
	//! список анимаций, опирающихся на файл \a strFileName
	void FindAllAnimation(const CString& strFileName, AnimIndex_t& anim_idx);

	enum eDependenciesBound{
		ACROSS_OBJECT, ///< зависимости подобъектов выходят за пределы объекта
					///< Пр.:(состояния - подобъекты, сцена объект)
		IN_OBJECT};///< зависимости подобъектов не выходят за пределы
	//! Отбрасывает все ссылки на подъобъекты объекта(состояния для анимированных, 
	//! все объекты для сцены), не выходящие за пределы объекта
	eDependenciesBound filterDepedenciesBound(qdNamedObject const* object);

	//! Составляет список зависимостей объекта(вхождение в условия и базовый объект КА)
	bool statObjectDependencies(qdNamedObject const* object);

	//! Заполнение списка объектов из ptrObjectsDependencies_
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
 	Надстройка над стандартным \a COleDropTarget. Нужен, чтобы легко определить
	бросили ли в окно дерева проекта или нет
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
