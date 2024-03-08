/********************************************************************
	created:	2002/10/23
	created:	23:10:2002   18:15
	filename: 	d:\projects\quest\questeditor\sv_api.cpp
	file path:	d:\projects\quest\questeditor
	file base:	sv_api
	file ext:	cpp
	Powerd by:	Илюха
	
	purpose:	реализация апи
*********************************************************************/
#include "stdafx.h"

#include "sv_api.h"

#include "childview.h"

namespace sv_api{

	void Redraw(eRedrawMode mode)
	{
		GetSceneView()->Redraw(mode);
	}
	void AdjustBitmapSize()
	{
		GetSceneView()->AdjustBitmapSize();
	}

}//end of namespace sv_api