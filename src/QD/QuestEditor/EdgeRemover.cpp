#include "stdafx.h"
#include "resource.h"
#include "edgeremover.h"

#include "qd_sprite.h"
#include "qd_game_object_animated.h"
#include "qd_game_object_static.h"
#include "qd_game_object_state.h"

#include "qd_interface_object_base.h"
#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"
#include "qd_interface_save.h"
#include "qd_interface_button.h"
#include "qd_interface_slider.h"
#include "qd_interface_background.h"
#include "qd_interface_element_state.h"

#include "Main/Utils/AllAnimationFinder.h"
#include "Main/Utils/stateoffsetapplyer.h"
#include "Main/Utils/MoveObjectCenter.h"

#include "Main/ObjectTraits.h"
#include "Main/StateTraits.h"

#include <boost/rtl.hpp>

EdgeRemover::EdgeRemover(void)
{
}

EdgeRemover::~EdgeRemover(void)
{
}

disk_op::eFileType EdgeRemover::file_type(CString const& strFile) {
	return disk_op::GetImageFileType(strFile);
}

Vect2i const EdgeRemover::remove_edges_tga(CString const& strFile) 
{
	qdSprite sprite;
	if (!sprite.load(strFile))
		return Vect2i();

	Vect2i const res = sprite.remove_edges();
	sprite.save();

	return res;
}

Vect2i const EdgeRemover::remove_edges_qda(CString const& strFile) 
{
	qdAnimation animation;
	if (!animation.qda_load(strFile))
		return Vect2i();

	Vect2i const res = animation.remove_edges();
	animation.qda_save(strFile);
	return res;
}

Vect2i const EdgeRemover::remove_edges(CString const& strFile, bool bAsk) {
	if (!bAsk||AfxMessageBox(IDS_ASK_REMOVE_EDGES, MB_YESNO) == IDYES)
	{
		if (strFile.IsEmpty())
			return Vect2i();
		switch(file_type(strFile)) {
		case disk_op::FT_QDA:
			return remove_edges_qda(strFile);
		case disk_op::FT_TGA:
			return remove_edges_tga(strFile);
		case disk_op::FT_UNDEFINED:
			ASSERT(0);
			break;
		}
	}
	return Vect2i();
}

Vect2i const EdgeRemover::operator()(CString const& strFile) const{
	return remove_edges(strFile);
}

#ifdef USE_NEW_PROJ
void EdgeRemover::remove_edges(qdGameDispatcher* gameDisp, qdNamedObjectBase* obj)
{
	if (dynamic_cast<qdInterfaceObjectBase*>(obj))
	{
		remove_edges_interface(static_cast<qdInterfaceObjectBase*>(obj));
	}
	else
		remove_edges(gameDisp, static_cast<qdNamedObject*>(obj));
}

bool EdgeRemover::remove_edges(qdGameDispatcher* gameDisp,
									   qdGameObjectStatic* object)
{
	return object->remove_sprite_edges();
}


/*!
	Пробегает по всем состояниям. Для статических делает обрезку.Если обрезка произведена
	успешно, то для всех состояний, использующих ту же анимацию, выравниваем смещения). 
	Первое обрезанное состояние 
	запоминается. Также запоминается, насколько сместилось изображение при обрезке.
	Если было хоть что-то обрезано и объект не имеет походочных состояний, смещает
	центр объекта в центр анимации первого обрезанного состояния
	\param pobj - объект, над состояниями которого работаем
	\retval true - всегда
*/

bool EdgeRemover::remove_edges(qdGameDispatcher* gameDisp,
									   qdGameObjectAnimated* object)
{
	Vect2i offset(0, 0);//хранит смещение первого годного к обрезке состояния
	Vect2i offset2;//рабочая для цикла
	Vect2i offset3;//рабочая для цикла
	bool bFirstRemove = true;
	qdGameObjectState* pLeadingState = NULL;
	boost::rtl::filtered_range<
		qdGameObjectStateVector const,
		bool (*)(qdGameObjectState const*)
	> fr(object->state_vector(), StateTraits::isStaticState);
	for (; fr; ++fr)
	{
		qdGameObjectState* const pstate = *fr;
		object->set_state(pstate);
		if (object->remove_animation_edges(offset2, offset3))
		{
			if (offset3.x||offset3.y)
				applayStateOffsetToOtherUsers(gameDisp,
					static_cast<qdGameObjectStateStatic*>(pstate),
					offset3);
			if (bFirstRemove&&pstate->coords_animation()->is_empty())
			{
				bFirstRemove = false;
				offset = offset2;
				pLeadingState = pstate;
			}
		}
	}
	object->set_default_state();

	if(pLeadingState&&!ObjectTraits::hasWalkState(object)){
		qdCamera const* camera = static_cast<qdGameScene*>(object->owner())->get_camera();
		MoveObjectCenter action;
		action(object, pLeadingState, offset, camera);
		object->set_default_state();
	}
	return true;
}

bool EdgeRemover::remove_edges(qdGameDispatcher* gameDisp,
									   qdGameObjectStateStatic* object)
{
	qdGameObjectAnimated* const pao = 
		static_cast<qdGameObjectAnimated*>(object->ref_owner());
	pao->set_state(object);
	Vect2i offset;
	if (pao->remove_animation_edges(Vect2i(), offset))
	{
		if (offset.x||offset.y)
			applayStateOffsetToOtherUsers(gameDisp, object, offset);
		return true;
	}
	return false;
}

/*!
	Для каталогов анимации всех сцен и глобального диспетчера формируются индекс списков
	анимаций, которые опираются на тот же файл, что и исходная(
	FindAllAnimation()). Затем перобегает по этому индексу и 
	если список сформирован для сцены вызывает 
	StateOffsetInOtherAnimUses4Scene().
	Если список был сформирован для глобального каталога(дальше глобальный список), то:
	 - пробегает по списку сцен.
	 - для каждой сцены из глобального списка формируется список анимаций, имя которых
		не повторяется в каталоге анимации сцены.
	 - Для сцены вызывает StateOffsetInOtherAnimUses4Scene() и передает
	  туда сформированный для сцены список
	\param query_src	- Исходное состояние. Это состояние уже было смещено и его 
							трогать нельзя
	\param offset		-  Смещение на которое надо сместить другие состояния
*/

void EdgeRemover::applayStateOffsetToOtherUsers(qdGameDispatcher* gameDisp,
							qdGameObjectStateStatic* query_src,
										  Vect2i const& offset)
{
	qdGameScene* query_src_scene = static_cast<qdGameScene*>(query_src->owner());
	CString strAnimFileName(query_src->animation()->qda_file());
	AnimIndex_t anim_indx;
	AllAnimationFinder finder;
	finder.find(*gameDisp, strAnimFileName, anim_indx);

	AnimIndex_t::const_iterator ix = anim_indx.begin(), ex = anim_indx.end();
	for(;ix != ex; ++ix)
	{
		qdNamedObject* pdisp = ix->first;
		if (pdisp->named_object_type() == QD_NAMED_OBJECT_SCENE)
		{
			if (!ix->second.empty())
			{
				StateOffsetApplyer offsetApllayer;
				offsetApllayer.apply(query_src, offset, 
						static_cast<qdGameScene*>(pdisp), ix->second);
			}
		}
		else //в данном случае это будет глобальный диспетчер
		{
			boost::rtl::crange<qdGameSceneList const> r(gameDisp->scene_list());
			std::vector<LPCTSTR> vec;
			for(; r; ++r)
			{
				qdGameScene* const pscene = *r;
				std::vector<LPCTSTR>::const_iterator j = ix->second.begin(),
					e = ix->second.end();
				//если в сцене есть анимация с таким именем,
				//то пропускаем эту сцену
				for(; j != e; ++j)
					if (!pscene->get_animation(*j))
						vec.push_back(*j);
				if (!vec.empty())
				{
					StateOffsetApplyer offsetApllayer;
					offsetApllayer.apply(query_src, offset, pscene, vec);
					vec.clear();
				}
			}
		}
	}
}

void EdgeRemover::remove_edges_interface(qdInterfaceObjectBase* pobj)
{
	std::type_info const& ti = typeid(*pobj);
	if (ti == typeid(qdInterfaceBackground))
		remove_edges_back(static_cast<qdInterfaceBackground*>(pobj));
	else if (ti == typeid(qdInterfaceElementState))
		remove_edges_element_state(static_cast<qdInterfaceElementState*>(pobj));
	else if (ti == typeid(qdInterfaceSave))
		remove_edges_save(static_cast<qdInterfaceSave*>(pobj));
	else if (ti == typeid(qdInterfaceSlider))
		remove_edges_slider(static_cast<qdInterfaceSlider*>(pobj));
	else if (ti == typeid(qdInterfaceButton))
		remove_edges_button(static_cast<qdInterfaceButton*>(pobj));
	else if (ti == typeid(qdInterfaceScreen))
		remove_edges_screen(static_cast<qdInterfaceScreen*>(pobj));
}

/*!
	Достает из объекта имя файла анимации и вызывает EdgeRemover::remove_edges для
	обрезки. Смещает положение объекта на расстояние, на которое сместилась анимация
	после обрезки. Реинициализирует объект.
*/
void EdgeRemover::remove_edges_back(qdInterfaceBackground* pobj)
{
	Vect2i const res = remove_edges(pobj->animation_file(), false);
	pobj->set_r(pobj->r() + res);
	pobj->init();
}

/*!
	Достает из состояния имя файлы анимации для всех режимов и вызывает 
	EdgeRemover::remove_edges дляобрезки. 
*/
void EdgeRemover::remove_edges_element_state(qdInterfaceElementState* pobj)
{
	remove_edges(pobj->animation_file(), false);
	remove_edges(pobj->animation_file(qdInterfaceElementState::MOUSE_HOVER_MODE), false);
	remove_edges(pobj->animation_file(qdInterfaceElementState::EVENT_MODE), false);
}

/*!
	Достает из объекта имя файла анимации и вызывает EdgeRemover::remove_edges для
	обрезки. Реинициализирует объект.
*/
void EdgeRemover::remove_edges_save(qdInterfaceSave* pobj)
{
	remove_edges(pobj->frame_animation_file(), false);
	pobj->init();
}

/*!
	Достает из объекта имя файла анимации и вызывает EdgeRemover::remove_edges для
	обрезки. Реинициализирует объект.
*/
void EdgeRemover::remove_edges_slider(qdInterfaceSlider* pobj)
{
	remove_edges(pobj->background()->animation_file(), false);
	remove_edges(pobj->slider()->animation_file(), false);
	pobj->init();
}

/*!
	Пробегает по всем элементам и для каждого вызывает 
	EdgeRemover::RemoveEdgesInInterface. Реинициализирует объект.
*/
void EdgeRemover::remove_edges_screen(qdInterfaceScreen* pobj)
{
	boost::rtl::rng::for_each(pobj->element_list(), &remove_edges_interface);
	pobj->init();
}
/*!
	Пробегает по всем состояниям кнопки и вызыавет 
	EdgeRemover::RemoveEdgesInInterfaceElementState(). Реинициализирует объект.
*/
void EdgeRemover::remove_edges_button(qdInterfaceButton* pobj)
{
	int const num = pobj->num_states();
	for(int i = 0; i < num; ++i)
		remove_edges_element_state(pobj->get_state(i));
	pobj->init();
}

bool EdgeRemover::remove_edges(qdGameDispatcher* gameDisp, qdGameScene* scene)
{
	boost::rtl::crange<qdGameObjectList const> r(scene->object_list());
	for (; r; ++r)
	{
		if (ObjectTraits::is_animated(*r))
			remove_edges(gameDisp, static_cast<qdGameObjectAnimated*>(*r));
		else
			remove_edges(gameDisp, static_cast<qdGameObjectStatic*>(*r));
	}
	return true;
}

//! Обработка игровых объектов
void EdgeRemover::remove_edges(qdGameDispatcher* gameDisp, qdNamedObject* pobj)
{
	switch(pobj->named_object_type())
	{
	case QD_NAMED_OBJECT_SCENE:
		remove_edges(gameDisp, static_cast<qdGameScene*>(pobj));
		break;
	case QD_NAMED_OBJECT_STATIC_OBJ:
		remove_edges(gameDisp, static_cast<qdGameObjectStatic*>(pobj));
		break;
	case QD_NAMED_OBJECT_OBJ_STATE:
		remove_edges(gameDisp, static_cast<qdGameObjectStateStatic*>(pobj));
		break;
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
	case QD_NAMED_OBJECT_MOVING_OBJ:
		remove_edges(gameDisp, static_cast<qdGameObjectAnimated*>(pobj));
		break;
	}
}
#endif // USE_NEW_PROJ
