#include "stdafx.h"
#include ".\gridzoneutils.h"

#include "qd_game_scene.h"
#include "qd_grid_zone.h"
#include "qd_game_object.h"

#include <boost/rtl.hpp>

void GridZoneUtils::reselectZones(qdGameScene const& scene)
{
	boost::rtl::crange<qdGridZoneList const> r(scene.grid_zone_list());
	for(; r; ++r)
	{
		qdGridZone * ptrZone = *r;
		if(!ptrZone->check_flag(QD_OBJ_HIDDEN_FLAG))
			ptrZone->select(true);
	}
}
