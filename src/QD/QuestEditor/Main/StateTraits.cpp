#include "stdafx.h"
#include ".\statetraits.h"
#include "qd_game_object_state.h"

#include <algorithm>

bool StateTraits::isGlobalForObj(qdNamedObject const& state, qdNamedObject const& object)
{
	return (hasGlobalFlag(static_cast<qdGameObjectState const*>(&state))
				&& state.ref_owner() != &object);
}

int StateTraits::getStateIndex(qdGameObjectStateVector const& states, 
						 qdNamedObject const* state)
{
	qdGameObjectStateVector::const_iterator r = 
		std::find(states.begin(), states.end(), state);
	if (r == states.end())
		return -1;
	return std::distance(states.begin(), r);
}

bool StateTraits::hasGlobalFlag(qdGameObjectState const* state){
	return state->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_GLOBAL_OWNER);
}

bool StateTraits::isMaskState(qdGameObjectState const* state){
	return (state->state_type() == qdGameObjectState::STATE_MASK);
}

bool StateTraits::isStaticState(qdGameObjectState const* state){
	return (state->state_type() == qdGameObjectState::STATE_STATIC);
}

bool StateTraits::isWalkState(qdGameObjectState const* state){
	return (state->state_type() == qdGameObjectState::STATE_WALK);
}
