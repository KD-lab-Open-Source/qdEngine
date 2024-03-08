#include "stdafx.h"
#include "resource.h"
#include ".\profilerdatatofile.h"
#include "CurDirSafer.h"
#include "disk_op.h"
#include "qd_trigger_profiler.h"
#include <fstream>
#include <iterator>
#include <boost/functional.hpp>

namespace{
	/// Хранитель директории, куда последний раз сохраняли
	CurDirSafer& GetCurDirSafer(){
		static CurDirSafer safer;
		return safer;
	}
}
ProfilerDataToFile::ProfilerDataToFile(void)
{
}

ProfilerDataToFile::~ProfilerDataToFile(void)
{
}

bool ProfilerDataToFile::run(qdTriggerProfiler const& profiler)
{
	CString strFilter;
	strFilter.LoadString(IDS_SAVE_PROFILER_DATA_FILTER);
	CString strTitle;
	strTitle.LoadString(IDS_OSD_CAP_SELECT_TXT_FILE);
	CString strFileName = disk_op::Get1DiskFileName(
													GetCurDirSafer, strFilter, 
													_T("txt"), strTitle, FALSE
													);
	if (strFileName.IsEmpty())
		return false;
	std::ofstream output;
	output.open(strFileName,std::ios_base::out | std::ios_base::trunc);
	if (!output) 
		return false;
	CWaitCursor wait;
	std::ostream_iterator<char const*> outItr(output, _T("\n"));
	std::transform(profiler.records_begin(), profiler.records_end(), outItr,
		boost::bind2nd(&qdTriggerProfiler::record_text, '\t'));
	return true;
}