#pragma once

class Project;
class GameObjectFactory;
class TreeLogic;
interface ITreeNodeFactory;
interface ITreeNode;

class qdGameObjectStatic;
class qdInterfaceBackground;
class qdInterfaceScreen;
class qdInterfaceElement;
class qdInterfaceSlider;
class qdInterfaceTextWindow;
class qdInterfaceCounter;
class qdInterfaceSave;

class qdFontInfo;
class qdVideo;
class qdMiniGame;
class qdMusicTrack;

class ProjectModifier
{
public:
	ProjectModifier(GameObjectFactory& gameObjectFactory, Project* project);
	~ProjectModifier(void);

	qdGameObjectAnimated* addAnimated(qdGameScene* ptrScene, 
									CString const& strName, 
									bool bShowInTree);
	//! Создает объект копируя его с исходного
	qdGameObjectAnimated* addAnimated(ITreeNode* parent, 
				qdGameObjectAnimated const* source, qdGameScene* ptrScene);
	qdGameObjectMoving* addPersonage(qdGameScene* ptrScene,
									CString const& strName,
									bool bShowInTree);
	//! Создает объект копируя его с исходного
	qdGameObjectMoving* addPersonage(ITreeNode* parent, 
		qdGameObjectAnimated const* source, qdGameScene* ptrScene);
	//! Создает объект копируя его с исходного
	qdGameObjectMoving* addPersonage(ITreeNode* parent, 
		qdGameObjectMoving const* source, qdGameScene* ptrScene);

	qdGameObjectMoving* addGlobalPersonage(qdGameDispatcher* ptrDisp,
									CString &strName, bool bShow);
	qdGameObjectStatic* addStatic(qdGameScene* ptrScene,
									CString const& strName,
									bool bShowInTree);
	bool				addStatics(ITreeNode* sceneNode);

	qdGameScene*		addScene(CString& name, bool bShow);

	bool				addBackground(ITreeNode* node, qdGameScene* scene);

	qdInterfaceBackground* addInterfaceBackground(CString const& name, 
												qdInterfaceScreen* pscr);
	qdInterfaceSlider* addInterfaceSlider(CString const& name, 
								qdInterfaceScreen* pscr);
	qdInterfaceScreen*	addInterfaceScreen(ITreeNode* parent, CString const& name);
	qdInterfaceTextWindow* addInterfaceTextWindow(CString const& name, 
													qdInterfaceScreen* pscr);
	qdInterfaceCounter* addInterfaceCounter(CString const& name, 
													qdInterfaceScreen* pscr);
	qdInterfaceSave*	addInterfaceSave(CString const&name, qdInterfaceScreen* pscr);
	qdInterfaceButton*	addInterfaceButton(CString const&name, qdInterfaceScreen* pscr);
	int addInterfButtonState(qdInterfaceButton* pb, CString const& name);
	qdGameEnd*	addGameEnd(ITreeNode* parent, 
							CString const& name, 
							CString const& screenName);
	qdFontInfo*	addFont(ITreeNode* parent);
	qdFontInfo*	createFont(ITreeNode* parent);
	//! Добавляет несколько видео роликов подряд.
	/*!
		Возвращает true, если хотя бы один ролик был добавлен успешно
	*/
	bool	addVideos(ITreeNode* parent);

	qdMiniGame* addMinigame(ITreeNode* parent);
	qdCounter* addCounter(ITreeNode* parent);
	qdInventory* addInventory(ITreeNode* parent);
	bool addInventoryCellTypes();
	qdTriggerChain* addTriggerChain(ITreeNode* parent, CString const& name);

	bool addCellSet(qdInventory& inventory);

	ITreeNode* addMusicTrack(qdGameScene* scene, ITreeNode* musicFolder);
	//! Добавляет зону в дерево
	ITreeNode* addGridZone(ITreeNode* sceneNode, qdGridZone* zone);
	ITreeNode* addMaskState(qdGameObjectAnimated* poa,
							qdGameObjectStateMask* state);

	//! Переставляет состояние в списке и вставляет его в дерево
	qdGameObjectState* moveState(qdGameObjectAnimated* source, 
		qdGameObjectAnimated* target,
		qdGameObjectState* movedState, 
		ITreeNode* targetNode,
		ITreeNode* insertAfter,
		int insertBefore);

	//! \brief Копирует состояние. Вставляет его в список состояний перед указанным
	//! и загружает его в дерево
	qdGameObjectState* copyState(qdGameObjectAnimated* source, 
		qdGameObjectAnimated* target,
		qdGameObjectState* copyState, 
		ITreeNode* targetNode,
		ITreeNode* insertAfter,
		int insertBefore);

	//! Переставляет зону в списке зон и вставляет её в дерево
	qdGridZone* moveZone(qdGameScene* scene, qdGridZone* moved, 
		qdGridZone const* zoneBefore, ITreeNode* insertAfter, ITreeNode* parent);
	//! Переставляет видео в списке видео и вставляет его в дерево
	qdVideo* moveVideo(qdVideo* source, 
						qdVideo const* before, 
						ITreeNode* insertAfter,
						ITreeNode* parent);
	//! Копирует исходный инветори и вставляет копию в дерево
	qdInventory* copyInventory(qdInventory const* source, ITreeNode* parent);
protected:
	qdGameObject* addGlobalObj2Scene(qdGameScene* pscene, qdGameObject const* pobj);
	void globalObjToScenes(qdGameObject const* pobj);
	
	ITreeNode* addInterfaceElement(qdInterfaceElement* element, 
		qdInterfaceScreen* screen, int defaultDepth);
	qdVideo* addVideo(const CString &strFileName, ITreeNode* parent);
protected:
	GameObjectFactory& getGameObjectFactory();
	TreeLogic& getTreeLogic() const;
	ITreeNodeFactory& getTreeNodeFactory() const;
	qdGameDispatcher* getGameDispatcher() const;
	Project* getProject() const;


private:
	Project* project_;
	GameObjectFactory& gameObjectFactory_;
};
