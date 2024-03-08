#include "stdafx.h"
#include "QuestEditor.h"
#include ".\gameobjectfactory.h"
#include "resource.h"

#include "name_op.h"

#include "qd_game_object_state.h"
#include "qd_game_object_moving.h"
#include "qd_game_object_static.h"
#include "qd_minigame.h"
#include "qd_trigger_chain.h"
#include "qd_music_track.h"
#include "qd_font_info.h"
#include "qd_game_end.h"
#include "qd_counter.h"
#include "qd_video.h"

#include "qd_interface_screen.h"
#include "qd_interface_background.h"
#include "qd_interface_slider.h"
#include "qd_interface_text_window.h"
#include "qd_interface_save.h"
#include "qd_interface_button.h"
#include "qd_interface_counter.h"

#include "qd_setup.h"

#include <boost/rtl.hpp>

/************************************************************************/
/* Внешние определения                                                  */
/************************************************************************/
namespace
{
//! Загружает заначения по умолчанию для сетки
void SetSceneNetDefaults(qdGameScene* ptrScene)
{
	qdCamera* ptrCam = ptrScene->get_camera();

	CQuestEditorApp* app = GetMyApp();
	ptrCam->set_focus(static_cast<float>(app->GetIntSetting(SETTING_NAME(NET_FOCUS))));
	ptrCam->rotate_and_scale(
		static_cast<float>(app->GetIntSetting(SETTING_NAME(NET_ANGLE_X))),
		static_cast<float>(app->GetIntSetting(SETTING_NAME(NET_ANGLE_Y))),
		static_cast<float>(app->GetIntSetting(SETTING_NAME(NET_ANGLE_Z))),1.f,1.f,1.f);

	Vect3f pos = ptrCam->get_grid_center();
	pos.z = static_cast<float>(app->GetIntSetting(SETTING_NAME(NET_VERT_OFFSET)));
	ptrCam->set_grid_center(pos);

	ptrCam->scale_grid(app->GetIntSetting(SETTING_NAME(NET_SX)),
		app->GetIntSetting(SETTING_NAME(NET_SY)), 
		app->GetIntSetting(SETTING_NAME(NET_CELL_SZ)),
		app->GetIntSetting(SETTING_NAME(NET_CELL_SZ)));

}

}

GameObjectFactory::GameObjectFactory(void)
{
}

GameObjectFactory::~GameObjectFactory(void)
{
}

std::auto_ptr<qdGameObjectStateStatic> GameObjectFactory::createStaticState()
{
	return std::auto_ptr<qdGameObjectStateStatic>(new qdGameObjectStateStatic);
}

std::auto_ptr<qdGameObjectStateWalk> GameObjectFactory::createWalkState(){
	return std::auto_ptr<qdGameObjectStateWalk>(new qdGameObjectStateWalk);
}

template< class Object>
std::auto_ptr<Object> GameObjectFactory::createNamedObject(CString const& name){
	std::auto_ptr<Object> res(new Object);
	if (res.get())
		res->set_name(name);
	return res;
}

std::auto_ptr<qdGameScene> GameObjectFactory::createScene(CString const& strName)
{
	std::auto_ptr<qdGameScene> ptrs(createNamedObject<qdGameScene>(strName));
	if(!ptrs.get())
		return ptrs;
	SetSceneNetDefaults(ptrs.get());
	return ptrs;
}

/*!
Создает объект "GameObj" и назначает ему позицию (0, 0, 0)
\return Указатель на созданный объект
*/

template< class GameObj>
std::auto_ptr<GameObj> GameObjectFactory::createGameObject(CString const& name)
{
	std::auto_ptr<GameObj> pobj (createNamedObject<GameObj>(name));
	if(!pobj.get())
		return pobj;

	pobj->set_pos(Vect3f(0, 0, 0));
	return pobj;
}

/*!
	Передает управление GameObjectFactory::createGameObject(DWORD dwErrMsg)
*/
std::auto_ptr<qdGameObjectMoving> GameObjectFactory::createPersonage(CString const& name)
{
	return createGameObject<qdGameObjectMoving>(name);
}

/*!
	Передает управление GameObjectFactory::createGameObject(DWORD dwErrMsg)
*/
std::auto_ptr<qdGameObjectAnimated> GameObjectFactory::createAnimated(CString const& name)
{
	return createGameObject<qdGameObjectAnimated>(name);
}

/*!
	Передает управление GameObjectFactory::createGameObject(DWORD dwErrMsg)
*/
std::auto_ptr<qdGameObjectStatic> GameObjectFactory::createStatic(CString const& name)
{
	return createGameObject<qdGameObjectStatic>(name);
}


/*!
Создает объект инвентори. Если режим создания(\a mode) ICM_USE_EXISTING_CELL_TYPES,
то в инвентори для каждого типа ячеек добавляется набор ячеек, состоящий из одной
ячейки.	Наборы располагаются, так чтобы не перескаться
\param mode - рижим создания.
\return	Указатель на вновь созданый объект, либо NULL в случае неудачи
*/
std::auto_ptr<qdInventory> GameObjectFactory::createInventory(
										qdGameDispatcher* gameDisp,
										eInventoryCrtMode mode)
{
	std::auto_ptr<qdInventory> p(new qdInventory);
	if (mode == ICM_USE_EXISTING_CELL_TYPES){
		typedef qdInventoryCellTypeVector::const_iterator ci_t;
		const Vect2s init_pos(0, 0);
		const Vect2s init_size(1,1);

		Vect2s scr_pos(init_pos);
		int bottom = 0;
		boost::rtl::crange<
			qdInventoryCellTypeVector const
		> r(gameDisp->inventory_cell_types());
		for(; r; ++r)
		{
			const qdInventoryCellType& ct = *r;
			qdInventoryCellSet* ps = p->add_cell_set(init_pos, init_size, ct);
			Vect2i size = ps->cell_size();
			if (scr_pos.x + size.x > qdGameConfig::get_config().screen_sx())
			{
				//типа переходим на новую строчку
				scr_pos.x = 0;
				scr_pos.y = bottom;
			}

			ps->set_screen_pos(Vect2s(scr_pos.x + (size.x>>1),
				scr_pos.y + (size.y>>1)));
			size = ps->screen_size();
			scr_pos.x += size.x;
			bottom = (std::max)(bottom, scr_pos.y + size.y);
		}
	}
	return p;
}

std::auto_ptr<qdInventory> GameObjectFactory::createInventory(qdInventory const* source)
{
	std::auto_ptr<qdInventory> p(new qdInventory(*source));
	return p;
}

/*!
\param strName			- Имя нового объекта
\param strDllName		- Имя файла длл
\param strConfigName	- Имя файла конфиг-файла
\return Укзатель на созданный объект. 	
*/
std::auto_ptr<qdMiniGame> GameObjectFactory::createMiniGame(const CString& strName)
{
	std::auto_ptr<qdMiniGame> res(createNamedObject<qdMiniGame>(strName));
	return res;
}

/*!
Создает объект qdTriggerChain и делает корневой узел активным
\return Указатель на созданный объект.
*/
std::auto_ptr<qdTriggerChain> GameObjectFactory::createTriggerChain(CString const& name)
{
	std::auto_ptr<qdTriggerChain> pobj (createNamedObject<qdTriggerChain>(name));
	if(pobj.get())
		pobj->root_element()->make_active(true);

	return pobj;
}

/*!
\param strName - имя музыкального трека
\return Указатель на созданный объект
*/
std::auto_ptr<qdMusicTrack> GameObjectFactory::createMusicTrack(const CString& strName){
	return createNamedObject<qdMusicTrack>(strName);
}

/*!
Создает объект qdFontInfo с указанным именем
\return NULL - если не удалось создать
*/
std::auto_ptr<qdFontInfo> GameObjectFactory::createFont(CString const& strName)
{

	return createNamedObject<qdFontInfo>(strName);
}

/*!
Получает уникальное имя на основе \a name. Создает объект и назначает имя
интерфейсного экрана(\a scrName).
\param name		- имя конечного экрана
\param scrName	- задает имя интерфейсного экрана
\return указатель на вновь созданный объект. NULL в случае неудачи
*/
std::auto_ptr<qdGameEnd> GameObjectFactory::createGameEnd(CString const& name)
{
	std::auto_ptr<qdGameEnd> ptr(createNamedObject<qdGameEnd>(name));
	return ptr;
}

/*!
Создает новый объект и назначает ему имя.
\param name - имя счетчика
\return указатель на новый объект. NULL в случае неудачи
*/
std::auto_ptr<qdCounter> GameObjectFactory::createCounter(CString const& name)
{
	return createNamedObject<qdCounter>(name);
}


std::auto_ptr<qdInterfaceBackground> 
	GameObjectFactory::createInterfaceBackground(CString const& name)
{
	return createNamedObject<qdInterfaceBackground>(name);
}

std::auto_ptr<qdInterfaceScreen>
	GameObjectFactory::createInterfaceScreen(CString const& name)
{
	return createNamedObject<qdInterfaceScreen>(name);
}

std::auto_ptr<qdInterfaceSlider> 
	GameObjectFactory::createInterfaceSlider(CString const& name)
{
	return createNamedObject<qdInterfaceSlider>(name);
}

std::auto_ptr<qdInterfaceTextWindow> 
	GameObjectFactory::createInterfaceTextWindow(CString const& name)
{
	return createNamedObject<qdInterfaceTextWindow>(name);
}

std::auto_ptr<qdInterfaceCounter> 
	GameObjectFactory::createInterfaceCounter(CString const& name)
{
	return createNamedObject<qdInterfaceCounter>(name);
}

std::auto_ptr<qdInterfaceSave> 
	GameObjectFactory::createInterfaceSave(CString const& name)
{
	return createNamedObject<qdInterfaceSave>(name);
}

std::auto_ptr<qdInterfaceButton> 
	GameObjectFactory::createInterfaceButton(CString const& name)
{
	return createNamedObject<qdInterfaceButton>(name);
}

std::auto_ptr<qdVideo>	
	GameObjectFactory::createVideo(CString const& name)
{
	return createNamedObject<qdVideo>(name);
}

std::auto_ptr<qdGameObjectMoving> 
	GameObjectFactory::createPersonage(qdGameObjectMoving const* source)
{
	return std::auto_ptr<qdGameObjectMoving>(new qdGameObjectMoving(*source));
}

std::auto_ptr<qdGameObjectMoving>  
	GameObjectFactory::createPersonage(qdGameObjectAnimated const* source)
{
	return std::auto_ptr<qdGameObjectMoving>(new qdGameObjectMoving(*source));
}

std::auto_ptr<qdGameObjectAnimated> 
	GameObjectFactory::createAnimated(qdGameObjectAnimated const* source)
{
	return std::auto_ptr<qdGameObjectAnimated>(new qdGameObjectAnimated(*source));
}
