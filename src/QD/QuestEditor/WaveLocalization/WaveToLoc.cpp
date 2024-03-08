#include "stdafx.h"
#include "resource.h"
#include ".\wavetoloc.h"
#include "disk_op.h"

#include <boost/rtl.hpp>

#include "ProgDlg.h"

#include "qd_game_dispatcher.h"
#include "qd_sound.h"

namespace
{

void loadingProgressFnc(int percents_loaded,void* data)
{
	CProgressDlg* prog = reinterpret_cast<CProgressDlg*>(data);
	prog->SetPos(percents_loaded);
}

}

WaveToLoc::WaveToLoc(void)
{
}

WaveToLoc::~WaveToLoc(void)
{
}

bool WaveToLoc::run(CWnd* parent, qdGameDispatcher& gameDisp, bool copyToResource)
{
	CString res = disk_op::GetDiskFolder(parent->m_hWnd, NULL, !copyToResource, NULL);
	if (res.IsEmpty())
		return false;

	CProgressDlg dlg;
	dlg.Create(parent);

	TCHAR const EXT[] = _T(".wav");
	if (copyToResource)
		gameDisp.copy_resources_from_folder(res, EXT, loadingProgressFnc, &dlg);
	else
		gameDisp.copy_resources_to_folder(res, EXT, loadingProgressFnc, &dlg);
	return true;
}
