#include "stdafx.h"
#include ".\cursoridupdater.h"

#include "qd_game_object_animated.h"

#include "obj_type_detectors.h"

#include <boost/rtl.hpp>
#include <boost/functional.hpp>

namespace {
	/**
		ѕровер€ет, что состо€ние не глобальное.
	 */
	bool StateHasNoGlobalFlag(qdGameObjectState const* state){
		return !state->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_GLOBAL_OWNER);
	}
}

CursorIdUpdater::CursorIdUpdater(void)
{
}

CursorIdUpdater::~CursorIdUpdater(void)
{
}

void CursorIdUpdater::run(qdGameDispatcher& gameDispatcher)
{
	CWaitCursor wait;
	processGlobalObjects(gameDispatcher);
	processScenes(gameDispatcher);
}

void CursorIdUpdater::processGlobalObjects(qdGameDispatcher& gameDispatcher)
{
	boost::rtl::crange<qdGameObjectList const> r(gameDispatcher.global_object_list());
	while (r){
		processObject(*static_cast<qdGameObjectAnimated*>(*r), gameDispatcher);
		++r;
	}
}

void CursorIdUpdater::processScenes(qdGameDispatcher& gameDispatcher)
{
	boost::rtl::rng::for_each(gameDispatcher.scene_list(),
		boost::bind2nd(boost::ptr_fun(CursorIdUpdater::processScene), gameDispatcher));
}

void CursorIdUpdater::processScene(qdGameScene* scene, qdGameDispatcher& gameDispatcher)
{
	boost::rtl::filtered_range<
		qdGameObjectList const, 
		bool (*)(qdNamedObject const*)>
		r(scene->object_list(),ptree_helpers::is_animated);
	for(; r; ++r)
		processObject(*static_cast<qdGameObjectAnimated*>(*r), gameDispatcher);
}

void CursorIdUpdater::processObject(qdGameObjectAnimated& object, 
									qdGameDispatcher& gameDispatcher)
{
	  processStates(object.state_vector(), gameDispatcher);
}

void CursorIdUpdater::processStates(qdGameObjectStateVector const& states, 
									qdGameDispatcher& gameDispatcher)
{
	boost::rtl::filtered_range<
		qdGameObjectStateVector const, 
		bool (*)(qdGameObjectState const*)
	> r(states, StateHasNoGlobalFlag);
						
	boost::rtl::rng::for_each(r, boost::bind2nd(boost::mem_fun(
						qdGameObjectState::update_cursor_id), gameDispatcher));
}
