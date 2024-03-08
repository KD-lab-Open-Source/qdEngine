#pragma once

class qdGameObjectMoving;
class qdGameObjectAnimated;
class qdGameObjectStatic;
class qdGameObjectStateStatic;
class qdGameScene;
class qdInventory;
class qdMiniGame;
class qdTriggerChain;
class qdMusicTrack;
class qdFontInfo;
class qdCounter;

class qdInterfaceBackground;
class qdInterfaceSlider;
class qdInterfaceTextWindow;
class qdInterfaceSave;
class qdInterfaceButton;
class qdInterfaceCounter;

class GameObjectFactory
{
public:
	//! Способы создания инвентори
	enum eInventoryCrtMode{
		ICM_USE_EXISTING_CELL_TYPES,///< При создании используются существующие наборы ячеек
		ICM_EMPTY///< Создается пустым
	};
	GameObjectFactory(void);
	~GameObjectFactory(void);

	std::auto_ptr<qdGameObjectStateStatic> createStaticState();
	std::auto_ptr<qdGameObjectStateWalk> createWalkState();
	std::auto_ptr<qdGameScene> createScene( CString const& strName);
	std::auto_ptr<qdGameObjectMoving> createPersonage(CString const& name);
	std::auto_ptr<qdGameObjectAnimated> createAnimated(CString const& name);
	std::auto_ptr<qdGameObjectMoving> createPersonage(qdGameObjectMoving const* source);
	std::auto_ptr<qdGameObjectMoving> createPersonage(qdGameObjectAnimated const* source);
	std::auto_ptr<qdGameObjectAnimated> createAnimated(qdGameObjectAnimated const* source);
	std::auto_ptr<qdGameObjectStatic> createStatic(CString const& name);
	std::auto_ptr<qdInventory> createInventory(qdGameDispatcher* gameDisp,
												eInventoryCrtMode mode);
	std::auto_ptr<qdInventory> createInventory(qdInventory const* source);

	std::auto_ptr<qdMiniGame> createMiniGame(const CString& strName);
	std::auto_ptr<qdTriggerChain> createTriggerChain(CString const& name);
	std::auto_ptr<qdMusicTrack> createMusicTrack(const CString& strName);
	std::auto_ptr<qdFontInfo> createFont(CString const& strName);
	std::auto_ptr<qdCounter> createCounter(CString const& name);
	std::auto_ptr<qdGameEnd> createGameEnd(CString const& name);
	std::auto_ptr<qdVideo>	createVideo(CString const& name);

	std::auto_ptr<qdInterfaceBackground> createInterfaceBackground(CString const& name);
	std::auto_ptr<qdInterfaceScreen> createInterfaceScreen(CString const& name);
	std::auto_ptr<qdInterfaceSlider> createInterfaceSlider(CString const& name);
	std::auto_ptr<qdInterfaceTextWindow> createInterfaceTextWindow(CString const& name);
	std::auto_ptr<qdInterfaceCounter> createInterfaceCounter(CString const& name);
	std::auto_ptr<qdInterfaceSave> createInterfaceSave(CString const& name);
	std::auto_ptr<qdInterfaceButton> createInterfaceButton(CString const& name);
protected:
	template< class Object>
		std::auto_ptr<Object> createNamedObject(CString const& name);
	template< class GameObj>
		std::auto_ptr<GameObj> createGameObject(CString const& name);
};
