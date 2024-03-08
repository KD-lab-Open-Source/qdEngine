#include "stdafx.h"
#include "resource.h"
#include "converttgatoqda.h"
#include "AnimByCadr.h"
#include <Shlwapi.h>
#include "disk_op.h"


ConvertTGAtoQDA::ConvertTGAtoQDA(void)
{
}

ConvertTGAtoQDA::~ConvertTGAtoQDA(void)
{
}

CString const ConvertTGAtoQDA::convert(CString const& strFileName) const
{
	switch(disk_op::GetImageFileType(strFileName)) {
	case disk_op::FT_TGA:
		{
			CString res(strFileName);
			PathRemoveExtension(res.GetBuffer());
			res.ReleaseBuffer();
			res += CString(reinterpret_cast<LPTSTR>(IDS_ANIMATION_EXT));

			AnimByCadr abc;
			boost::shared_ptr<qdAnimation> ptrAnim = abc.make(strFileName, 0.11f);
			if (!ptrAnim)
				return CString();
			ptrAnim->qda_save(res);
			return res;
		}
	case disk_op::FT_QDA:
		return strFileName;
	}
	return CString();
}
