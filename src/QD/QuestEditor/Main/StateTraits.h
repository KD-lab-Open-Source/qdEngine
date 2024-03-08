#pragma once

#include "qd_fwd.h"

class StateTraits
{
public:
	//! явл€етс€ дл€ состо€ние глобальным дл€ этого объекта
	static bool isGlobalForObj(qdNamedObject const& state, qdNamedObject const& object);
	static int getStateIndex(qdGameObjectStateVector const& states, 
								qdNamedObject const* state);
	static bool hasGlobalFlag(qdGameObjectState const* state);
	static bool isMaskState(qdGameObjectState const* state);
	static bool isStaticState(qdGameObjectState const* state);
	static bool isWalkState(qdGameObjectState const* state);
};
