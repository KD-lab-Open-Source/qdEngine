#include "stdafx.h"
#include "resource.h"
#include "playvideoeventrepres.h"
#include "qd_video.h"

PlayVideoEventRepres::PlayVideoEventRepres(void)
:InterfaceEventWithDataRepres(qdInterfaceEvent::EVENT_PLAY_VIDEO,
							  IDS_EVENT_PLAY_VIDEO)
{
}

PlayVideoEventRepres::~PlayVideoEventRepres(void)
{
}

LPCTSTR PlayVideoEventRepres::getDefaultValue() const
{
	qdGameDispatcher* const pgd = qdGameDispatcher::get_dispatcher();
	if (!pgd->video_list().empty())
		return pgd->video_list().front()->name();
	return NULL;
}

void PlayVideoEventRepres::loadData(CWnd* pwnd) const
{
	qdGameDispatcher* ptrGameDisp = qd_get_game_dispatcher();
	qdVideoList const& lst = ptrGameDisp->video_list();
	CCJFlatComboBox* const pbox = static_cast<CCJFlatComboBox*>(pwnd);
	std::for_each(lst.begin(), lst.end(), PutObjectNameToCombo(*pbox));
	pbox->SetCurSel(0);
}
