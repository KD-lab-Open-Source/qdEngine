#include "stdafx.h"

#include ".\moveobjectcenter.h"

#include "qd_game_object_animated.h"
#include "qd_game_object_state.h"
#include "qd_camera.h"

#include "Main/StateTraits.h"

#include <boost/rtl.hpp>

MoveObjectCenter::MoveObjectCenter(void)
{
}

MoveObjectCenter::~MoveObjectCenter(void)
{
}

void MoveObjectCenter::operator()(qdGameObjectAnimated* pobj, 
				qdGameObjectState const* pLeadingState,
				const Vect2i& off, qdCamera const* pcam) const
{
	//сдвинули объект для начала
	//в z храниться глубина
	if (off.x||off.y) 
	{
		ASSERT(pLeadingState->coords_animation()->is_empty());
		Vect3f vpos = pcam->global2camera_coord(pobj->R());
		Vect2s scr_pos = pcam->camera_coord2scr(vpos);
		scr_pos += off;
		scr_pos = pcam->scr2rscr(scr_pos);
		pobj->set_pos(pcam->rscr2global(scr_pos, vpos.z));

		pobj->set_default_pos(pobj->R());

		const_cast<qdGameObjectState*>(pLeadingState)->set_center_offset(Vect2s(0, 0));

		//сдвигаем остальные состояния
		boost::rtl::filtered_range
			<
			qdGameObjectStateVector const,
			bool (*)(qdGameObjectState const*)
			> fr(pobj->state_vector(), StateTraits::isStaticState);
		for (; fr; ++fr)
		{
			qdGameObjectState* pstate = *fr;
			if (pstate != pLeadingState&&pstate->coords_animation()->is_empty() && pstate != pLeadingState) 
				pstate->set_center_offset(pstate->center_offset()-off);
		}
	}
}
