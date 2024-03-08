#pragma once
#include ".\conditionalobjecttreenode.h"

/// Класс узла, соответствующего сцене
class GameSceneTreeNode : public ConditionalObjectTreeNode
{
public:
	GameSceneTreeNode(qdNamedObject* object);
	~GameSceneTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Добавляет недостающие части
	virtual bool reload(ITreeNodeFactory* factory, 
						TreeLogic& logic,
						ITreeNode const* parent, 
						qdNamedObjectBase const* owner);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);

	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
/*
	class InsertObject;
	class InsertGameObject;
*/
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;

	/// Загружает в дерево содержимое сцены
	void loadContent(ITreeNodeFactory* factory, TreeLogic& treeLogic);
	/// Загружает зоны
	bool loadZones(ITreeNodeFactory* factory, TreeLogic& treeLogic);
	/// Загружает музыкальные треки
	bool loadMusicTracks(ITreeNodeFactory* factory, TreeLogic& treeLogic);
	/// Загружает список объектов
	bool loadGameObjects(ITreeNodeFactory* factory, TreeLogic& treeLogic);

	/// Перезагружает список каких-лобо объектов в дерево
	template<typename Range>
	bool reloadSimpleCatalog(ITreeNodeFactory* factory, 
							TreeLogic& logic,
							ITreeNode * folder,
							ITreeNode* insertAfter,
							Range r);

	/// Перезагружает содержимое сцены в дерево
	bool reloadContent(ITreeNodeFactory* factroy, TreeLogic& logic);
	/// Перезагружает список зон
	bool reloadZones(ITreeNodeFactory* factory, TreeLogic& logic);
	/// Перезагружает список музыкальных треков
	bool reloadMusicTracks(ITreeNodeFactory* factory, TreeLogic& logic);
	/// Перезагружает список игровых объектов
	bool reloadGameObjects(ITreeNodeFactory* factory, TreeLogic& logic);

	/// \brief Сцену выбрали и снали выделение при активном редакторе сцены
	bool onSelectInSceneView(TreeLogic& logic, bool bSelect);
	/// \brief Сцену выбрали и снали выделение при активном редакторе триггеров
	bool onSelectInTriggerView(TreeLogic& logic, bool bSelect);

	/// Удаляет зоны
	void deleteZones(TreeLogic& logic);
	/// Удаляет игровые объекты
	void deleteObjects(TreeLogic& logic);
	/// Удаляет музыкальные треки
	void deleteMusicTracks(TreeLogic& logic);
	/// Удаляет объект сцены
	void deleteSubObject(TreeLogic& logic, qdNamedObject const* object);

	/// Возвращает true, если newName уникально
	bool nameTest(TreeLogic& logic, LPCTSTR newName);

	/// Обновляет заголовки триггеров для всех объектов сцены
	void updateTriggerElementTitlesForSceneObjects(qdGameDispatcher* gameDisp);
};
