#include "stdafx.h"
#include "resource.h"

#include "curdirsafer.h"
#include "disk_op.h"
#include "interf_disk_op.h"

namespace{
	CurDirSafer& INTERFAnimCurDirSafer(){
		static CurDirSafer _safer_;
		return _safer_;
	}
	CurDirSafer& INTERFSoundCurDirSafer(){
		static CurDirSafer _safer_;
		return _safer_;
	}
}

const CString GetIntrfAnimFileName(){
	CString strFilter;
	strFilter.LoadString(IDS_OPEN_QDA_TGA_FILTER);
	CString strTitle;
	strTitle.LoadString(IDS_OSD_CAP_QDA_TGA_SELECT);
	return disk_op::Get1DiskFileName(
		INTERFAnimCurDirSafer, strFilter, _T(""),strTitle);
}

const CString GetIntrfSoundFileName(){
	CString strFilter;
	strFilter.LoadString(IDS_OPEN_INTERF_SOUND_FILTER);
	CString strTitle;
	strTitle.LoadString(IDS_OSD_CAP_INTERF_SOUND_SELECT);
	return disk_op::Get1DiskFileName(
		INTERFSoundCurDirSafer, strFilter, _T("*.wav"),strTitle);
}
