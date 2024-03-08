#pragma once

#include "NamedObjectTreeNode.h"

/// Класс узла, соответствующего диспетчеру проекта. Это корневой узел
class GameDispTreeNode : public NamedObjectTreeNode
{
public:
	GameDispTreeNode(qdNamedObject* object);
	~GameDispTreeNode(void);

	virtual bool load(ITreeNodeFactory* factory,
						TreeLogic& treeLogic, 
						ITreeNode * parent, 
						qdNamedObjectBase const* owner,
						ITreeNode* insertAfter);
	virtual bool reload(ITreeNodeFactory* factory,
						TreeLogic& treeLogic, 
						ITreeNode const* parent, 
						qdNamedObjectBase const* owner);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	bool showProperties(TreeLogic& logic, int activePage);
	bool canRename(TreeLogic& logic);
protected:
	/// Загружает в дерево сцены
	bool loadScenes(ITreeNodeFactory* factory, TreeLogic& logic);
	/// Загружает в дерево глобальные объекты
	bool loadGlobalGameObjects(ITreeNodeFactory* factory, TreeLogic& logic);
	/// Загружает в дерево триггера
	bool loadTriggers(ITreeNodeFactory* factory, TreeLogic& logic);
	/// Загружает в дерево мышь
	bool loadMouse(ITreeNodeFactory* factory, TreeLogic& logic);
	/// Загружает в дерево инвентори
	bool loadInventory(ITreeNodeFactory* factory, TreeLogic& logic);
	/// Загружает в дерево счетчики
	bool loadCounters(ITreeNodeFactory* factory, TreeLogic& logic);
	/// Загружает в дерево фонты
	bool loadFonts(ITreeNodeFactory* factory, TreeLogic& logic);
	/// Загружает в дерево видео
	bool loadVideo(ITreeNodeFactory* factory, TreeLogic& logic);
	/// Загружает в дерево миниигры
	bool loadMinigames(ITreeNodeFactory* factory, TreeLogic& logic);

	/// Загружает в дерево интерфейс
	bool loadInterface(ITreeNodeFactory* factory, TreeLogic& logic);
	/// Загружает в дерево последние экраны
	bool loadGameEnds(ITreeNodeFactory* factory, TreeLogic& logic, ITreeNode * parent);
	/// \brief Перезагружает (что было удалено удаляется, 
	/// что добавлено - добавляется) в дерево сцены
	bool reloadScenes(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief Перезагружает (что было удалено удаляется, 
	/// что добавлено - добавляется) в дерево глобальные объекты
	bool reloadGlobalGameObjects(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief Перезагружает (что было удалено удаляется, 
	/// что добавлено - добавляется) в дерево триггера
	bool reloadTriggers(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief Перезагружает (что было удалено удаляется, 
	/// что добавлено - добавляется) в дерево мышь
	bool reloadMouse(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief Перезагружает (что было удалено удаляется, 
	/// что добавлено - добавляется) в дерево инвентори
	bool reloadInventory(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief Перезагружает (что было удалено удаляется, 
	/// что добавлено - добавляется) в дерево счетчики
	bool reloadCounters(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief Перезагружает (что было удалено удаляется, 
	/// что добавлено - добавляется) в дерево фонты
	bool reloadFonts(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief Перезагружает (что было удалено удаляется, 
	/// что добавлено - добавляется) в дерево видео
	bool reloadVideo(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief Перезагружает (что было удалено удаляется, 
	/// что добавлено - добавляется) в дерево миниигры
	bool reloadMinigames(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief Перезагружает (что было удалено удаляется, 
	/// что добавлено - добавляется) в дерево интерфейсные объекты
	bool reloadInterface(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief Перезагружает (что было удалено удаляется, 
	/// что добавлено - добавляется) в дерево конечные экраны
	bool reloadGameEnds(ITreeNodeFactory* factory, TreeLogic& logic, ITreeNode * parent);
};
