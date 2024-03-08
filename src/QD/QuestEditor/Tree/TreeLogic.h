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

//! Команды для уснатовки иконки триггера у узла
enum eTriggerIconCommand{
	TIC_REMOVE, ///< удалить
	TIC_SET_NOT_ACTIVE, ///< поставить неактивную
	TIC_SET_ACTIVE///< поставить активную
};

/// Реализует прослойку между деревом проекта и TreeListCtrl'm
class TreeLogic	: public NullItemDragdropNotifyListener
				, public NullChangeItemLabelNotifyListener
				, public NullItemOperationNotifyListener
				, public NullGeneralNotifyListener
				, public NullItemChangeNotifyListener
{
	typedef std::multimap<DWORD_PTR, ITreeNode*> TreeNodeIndex;
	
public:
	//! Возможные типы фильтров элементов в редакторе триггеров
	enum eFilterScheme{
		FS_NONE, ///< Нет фильтрации
		FS_ENTRY_FOR_DYN_OBJ, ///< Выделяются состояния динамических объектов
		FS_ENTRY_FOR_MOUSE, ///< Выделяются состояния мыши
		FS_ENTRY_FOR_PERS, ///< Выделяются состояния персонажей
		FS_ENTRY_FOR_DIALOGS,///< Выделяются диалоговые состояния
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
	//! Строит дерево проекта
	bool loadStructure();
	/// Назначает sceneView
	void setSceneView(ISceneViewFacade* sceneView);

	/// Возвращает конторол, с которым работаем
	QETreeListCtrl& getTreeListCtrl() const;
	/// Назначает конторол, с которым работаем
	void setTreeListCtrl(QETreeListCtrl* treeList);

	/// Возвращает ссылку на индекс сцен
	TreeItemsIndex& getSceneItemsIndex();
	/// Указатель на базовое окно
	CWnd* getBaseWnd() const;
	/// Устанавливает указатель на базовое окно
	void setBaseWnd(CWnd* wnd);
	/// Возвращает указатель на вью сцены
	ISceneViewFacade* getSceneView() const;
	/// Зачищает объект
	void reset();

	/// Добавляте в запись в индекс узлов дерева
	bool addTreeNodeIndexEntry(DWORD_PTR key, ITreeNode* value);
	/// Удаляет запись из индекса узлов дерева
	void removeTreeNodeIndexEntry(DWORD_PTR key, ITreeNode* value);
	//! Сообщает, является ли активным редактор сцены
	bool isSceneViewActive() const;
	/// Устанавливает активность/неактивность редактору сцены
	void setActiveSceneView(bool bActive);
	/// Возвращает указатель на диспетчер игры
	qdGameDispatcher* getGameDispatcher() const;

	/// Лежит ли узел под узлом анимированного объекта.
	/** pfIsMoving(необязательный) - содрежит true если это еще и персонаж*/
	ITreeNode* isNodeUnderAnimated(ITreeNode const* node, bool *pfIsMoving) const;
	/// Лежит ли узел под узлом сцены
	ITreeNode* isNodeUnderScene(ITreeNode const* node) const;
	/// Лежит ли узел под каталогом состояний.
	/** pfIsWalk (необязательный) содержит true, если это каталог походок*/
	ITreeNode* isNodeUnderStateCatalog(ITreeNode const* node, bool *pfIsWalk) const;
	//! Находится ли узел под узлом анимированных объектов
	ITreeNode* isNodeUnderAnimatedCatalog(ITreeNode const* node);
	//! Находится ли узел под узлом персонажей
	ITreeNode* isNodeUnderMovingCatalog(ITreeNode const* node);

	/// Возвращет узел сцены, с которой работают в редакторе сцены
	ITreeNode* getCurrentSceneNode() const;

	/// Возвращет узел, на котором установлен фокус
	ITreeNode* getFocusedNode() const;
	/// Выставляет узел, на котором будет усановлен фокус
	void setFocusedNode(ITreeNode* node);


	/// Показать для указанного узла диалог свойств. Можно выбрать активную страницу
	bool showProperties(ITreeNode* node, int activePage);
	/// Показать для указанного узла диалог условий
	bool showConditions(ITreeNode* node);
	/// По узлу контрола возвращает узел ITreeNode
	ITreeNode* getTreeNode(CTreeListItem const* item) const;
	/// Возвращет родительский узел перетаскиваемого узла
	ITreeNode* getDraggedParentNode() const;
	/// Возвращает интерфейс редактора триггеров
	ITriggerView* getTriggerView() const;
	/// Назначает указатель на редактор триггеров
	void setTriggerView(ITriggerView* view);

	//! Как фильтруем элементы в редакторе триггеров
	eFilterScheme getFilterScheme() const;
	//! Нужно ли позиционироваться на элементе в РТ при клике на узел
	bool getShowTriggerElement() const;

	//! По объекту возвращает его узлы
	Nodes const getObjectNodes(qdNamedObjectBase const* obj) const;
	//! По ключу возвращает его узлы
	Nodes const getKeyNodes(DWORD_PTR key) const;

	//! Возвращает узел каталога указанного типа, находящийся под данным узлом
	ITreeNode* getSubfolder(ITreeNode const* parent, DWORD_PTR type) const;
	/// Возвращает родительский узел
	ITreeNode* getParentNode(ITreeNode const* child) const;
	/// Возвращает первый под узел 
	ITreeNode* getChildNode(ITreeNode const* parent) const;

	//! Прекращает редактирование размеров состояния
	void endEditStateDims(bool bSave);

	/// Запускает анимацию для состояния
	/**Сразу передает управление в редактор сцены*/
	void showAnimation(qdGameObjectState* state);
	/// Останавливает анимацию для состояния
	/**
		Сразу передает управление в редактор сцены
	 */
	void stopAnimation(qdGameObjectState* state);
	/// Запускает видео.
	/**
		Тормозит предыдущее, если есть. Сразу передает управление в редактор сцены
	 */
	void playStopVideo(qdVideo* video);
	/// Останавливает видео.
	/**
		Сразу передает управление в редактор сцены
	 */
	void stopVideo(qdVideo* video);
	//! Выставляет для объектов из цепочки иконку активного триггера
	void setActiveTriggerIcon(qdTriggerChain const* chain, BOOL bSet);
	//! Устанавливает, соответствующую иконку для узла
	void setInTriggerIcon(ITreeNode* node, 	eTriggerIconCommand command);

	//! На изменение состояния узла в дереве
	void onItemStateChanged(ITreeNode* node, DWORD oldState, DWORD newState);

	bool selectInterfaceScreen(LPCTSTR screenName);

	//! дочерний узел по тексту
	ITreeNode* getSubNodeByName(ITreeNode* parent, LPCTSTR subNodeText) const;

	//! Выбирает узел и показывает его на экране
	bool showNode(ITreeNode* node) const;
	/// Устанавливает текст узла в дереве
	void setNodeText(ITreeNode* node, LPCTSTR text) const;
	/// Возвращет текст узла в дереве
	CString getNodeText(ITreeNode* node) const;

	//! Удаляет все дочерние узлы
	void removeSubnodes(ITreeNode* node) const;

	//! Команды для метода expand
	enum eExpandCommand{
		EC_COLLAPSE,//! свернуть узел в дереве
		EC_EXPAND,//! развернуть узел в дереве
		EC_TOGGLE//! переклучить состояние узла в дереве
	};
	/// Сворачивает/разворачивает узел в дереве
	void expand(ITreeNode* node, eExpandCommand command) const;
	//! Сворачивает/разворачивает узел и все подузлы
	void expandNodeAndChilds(ITreeNode* node, eExpandCommand command);
	//! развернут ли узел
	bool isExpanded(ITreeNode const* node) const;

	//! Удаляет узел и вместе с ним объект из дерева
	void deleteNode(ITreeNode* node);

	/// Возвращает указатель на список объектов
	IObjectList* getObjectList() const;

	/// Возвращает указатель на фабрику узлов дерева
	ITreeNodeFactory* getTreeNodeFactory() const;
	/// Указатель на класс, управляющий состоянием редактора триггеров и сцены
	IViewMgr* getViewMgr() const;

	//! Можем ли бросить 
	DROPEFFECT canDrop(const CPoint& point);
	//! Обработчик события: бросили данные
	BOOL OnDropData(void* pData, DROPEFFECT dropEffect, const CPoint& point);
	/// Помечает узел как drop traget(узел на который будет брошен перетягиваемый)
	void selectDropTarget(ITreeNode const* node);
	/// Возвращет drop targets
	ITreeNode* getDropTarget() const;
	/// Интерфейс класс проекта
	IProjectFacade* getProject() const;

	//! Перенаправляет вызов в Project::copyAnimatedToAnimated
	bool copyAnimatedToAnimated(ITreeNode* dropTarget, 
								qdGameObjectAnimated const* source,
								qdGameScene* scene);
	//! Перенаправляет вызов в Project::copyAnimatedToMoving
	bool copyAnimatedToMoving(ITreeNode* dropTarget, 
								qdGameObjectAnimated const* source,
								qdGameScene* scene);
	//! Перенаправляет вызов в Project::copyMovingToMoving
	bool copyMovingToMoving(ITreeNode* dropTarget, 
							qdGameObjectMoving const* source,
							qdGameScene* scene);

	//! Перенаправляет вызов в Prject::moveState
	qdGameObjectState* moveState(qdGameObjectAnimated* source, 
					qdGameObjectAnimated* target,
					qdGameObjectState* movedState, 
					ITreeNode* targetNode,
					ITreeNode* insertAfter,
					int insertBefore);
					
	//! Перенаправляет вызов в Prject::copyState
	qdGameObjectState* copyState(qdGameObjectAnimated* source, 
		qdGameObjectAnimated* target,
		qdGameObjectState* copyState, 
		ITreeNode* targetNode,
		ITreeNode* insertAfter,
		int insertBefore);

	//! Перенаправляет вызов в Prject::moveZone
	qdGridZone* moveZone(qdGameScene* scene, 
			qdGridZone* movedZone, 
			qdGridZone const* zoneBefore,
			ITreeNode* nodeAfter,
			ITreeNode* parent);
	//! Перенаправляет вызов в Prject::moveVideo
	qdVideo* moveVideo(qdVideo* source, qdVideo const* before, 
					ITreeNode* insertAfter, ITreeNode* parent);

	//! Перенаправляет вызов в Project::copyInventory
	qdInventory* copyInventory(qdInventory const* source, ITreeNode* parent);

	/// Возвращет предыдущий видимый узел
	ITreeNode* getPrevVisibleNode(ITreeNode* node) const;
	/// Возвращет предыдущий братский узел
	ITreeNode* getPrevSiblingNode(ITreeNode const* node);
	/// Возвращет следующий братский узел
	ITreeNode* getNextSiblingNode(ITreeNode const* node);

	/// Ставит/убирает галочку у узла.
	/** Ничего не изменится, если чекбокс не виден. \sa showNodeCheck*/
	void setNodeCheck(ITreeNode const* node, bool check);
	/// Показывает чекбокс рядом с узлом.
	void showNodeCheck(ITreeNode const* node, bool show);

	/* Функции этого кода теперь выполняет TreeLogic::saveFullStruct(...) и TreeLogic::loadFullStruct(...)
	/// Сохраняет настройки
	void saveIni(IProjectSettings* projectSettins);
	/// Считывает настройки
	void readIni(IProjectSettings* projectSettings);
	*/

	//! Сериализация всей структуры дерева в файл
	void saveFullStruct(const char* fname);
	void loadFullStruct(const char* fname);

protected:
	//! Начинаем тянуть выделенные элементы левой кнопкой мыши
	virtual bool onBeginDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! Уведомляет о том, что тянем курсор над окном
	virtual bool onDragOver(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! Уведомляет о том, что отжали кнопку мыши
	virtual bool onDrop(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);

	//! Вызывается перед началам редактирования текста.
	virtual bool onBeginLabelEdit(CTreeListCtrl& source,CHANGE_LABEL_NOTIFY_INFO* pclns);

	//! Уведомляет о том, что редактирование закончено
	virtual bool onEndLabelEdit(CTreeListCtrl& source, CHANGE_LABEL_NOTIFY_INFO* pclns);

	/// Удаляет соответствующий узлу контрола ITreeNode
	virtual void onDeleteItem(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info);

	/// Показывает диалог свойств или условий
	virtual bool onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	/// Передает сообщение в ITreeNode соответствующего узлу контрола
	virtual void onRClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	//! После того, как развернули
	virtual void onItemExpanded(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! Перед изменением состояния
	virtual bool onItemStateChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! После изменения состояния
	virtual void onItemStateChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! Если есть стиль	TLC_CHECKBOX, то приходит после изменения состояния
	virtual void onItemCheckChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	
	/* Функции этого кода теперь выполняет TreeLogic::saveFullStruct(...) и TreeLogic::loadFullStruct(...)
	//! Загружает из потока состояния узлов
	void loadExpandData(std::istream& in);
	//! Загружает из потока состояния узлов
	void saveExpandData(std::ostream& out);
	*/ 

	//! Загружает узел, имевший фокус
	void loadFocused(IProjectSettings* projectSettings);
	//! Сохраняет узел, имеющий фокус
	void saveFocused(IProjectSettings* projectSettings);

	friend class TreeNodeStube;
	//! переводит ITreeNode в CTreeListItem
	static CTreeListItem* nodeToItem(ITreeNode const *node);
	//! переводит CTreeListItem в ITreeNode
	static ITreeNode* itemToNode(CTreeListItem* item);
protected:

	/* Функции этого кода теперь выполняет TreeLogic::saveFullStruct(...) и TreeLogic::loadFullStruct(...)
	class ExpandDataLoader;
	class ExpandDataSaver;
	*/

	/// Устанавливает указатель на узел текущей сцены 
	void setCurrentSceneNode(ITreeNode* node);

	/// Выбирает сцену. Сразу же передает управление в IProjectFacade::SelectScene
	bool selectScene(ITreeNode* sceneNode);
	/// \brief В зависимости от галочки в настройках приложения сворачивает все 
	/// сцены за исключением указанной. Указанную разворачивает
	void changeSceneNodesExpanding(ITreeNode const* exclude);

	/// Лежит ли узел в каталоге, указанного типа
	ITreeNode* isNodeUnderCatalog(ITreeNode const* node, DWORD catalogId);

	//! Возвращает идекс объект-узел дерева.
	TreeNodeIndex& getTreeNodeIndex();
	//! Возвращает идекс объект-узел дерева.
	TreeNodeIndex const& getTreeNodeIndex() const;
private:
	//! Рекурсивная часть expandNodeAndChilds
	void expandRecur(ITreeNode* node, eExpandCommand command);
	//! Рекурсивные функции для сериализации всей структуры дерева
	void saveFullStructRecur(std::ofstream& out, ITreeNode* child);
	void loadFullStructRecur(std::ifstream& in, ITreeNode* child);

	/// Указатель на текущий проект
	Project* project_;
	/// Указатель на редактор сцены
	ISceneViewFacade* sceneView_;
	/// Фабрика узлов дерева
	ITreeNodeFactory* treeNodeFactory_;
	/// Редатор триггеров
	ITriggerView* triggerView_;
	/// Tree-контрол
	QETreeListCtrl* tree_;
	/// Индекс сцен. Позволяет нам вставлять сцены  в дерево сортированно
	TreeItemsIndex sceneItemsIndex_;
	/// Таблица соответствия объектов движка объектам дерева
	TreeNodeIndex treeNodeIndex_;
	/// Узел текущей сцены
	ITreeNode*	currentSceneNode_;
	/// Окно, к которому приделан трии-контрол
	CWnd*		baseWnd_;
//	//! Текущий фильтр элементов в редакторе триггеров
//	eFilterScheme filterScheme_;
//	bool showTriggerElement_;

	/// Перетаскиваемый узел
	mutable ITreeNode* draggedNode_;
	/// Родительский узел перетаскиваемого узла
	mutable ITreeNode* draggedNodeParent_;

	friend class TreeLogicDropTarget;
	//! поддержка drag-and-drop
	std::auto_ptr<TreeLogicDropTarget>		dropTarget_;

	/// Определяет цвета в дереве
	boost::scoped_ptr<TreeCustomDrawer> customDrawer_;

	//! Идет полным ходом загрузка дерева
	bool loadingTree_;
};


/*!
	Надстройка над стандартным \a COleDropTarget. Нужен, чтобы легко определить
	бросили ли в окно дерева проекта или нет
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
	/// бросили ли перетскиваемый объект в трии-контрол
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
