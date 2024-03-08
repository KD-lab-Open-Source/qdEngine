/********************************************************************
	created:	2002/10/23
	created:	23:10:2002   18:13
	filename: 	d:\projects\quest\questeditor\sv_api.h
	file path:	d:\projects\quest\questeditor
	file base:	sv_api
	file ext:	h
	Powerd by:	Èëþõà
	
	purpose:	scene_view(CChildView) api
*********************************************************************/

#ifndef _SV_API_HEADER_
#define _SV_API_HEADER_
#include "ISceneViewFacade.h"
namespace sv_api{

	using ::eRedrawMode;
	using ::RM_REDRAW_CONTENT;
	using ::RM_NO_CONTENT_REDRAW;
//	enum eRedrawMode {RM_REDRAW_CONTENT, RM_NO_CONTENT_REDRAW};
	void Redraw(eRedrawMode mode);
	void AdjustBitmapSize();
}//end of sv_api

#endif//_SV_API_HEADER_
