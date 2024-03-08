#include "stdafx.h"
#include "../ExtBitmap.h"
#include "bm_bkgnd.H"
#include "bm_bkgnd2.H"
#include "bm_checkbox.h"
#include "bm_lockbox.h"
#include "bm_def_head.h"
#include "bm_def_sort.h"

namespace{
	class ExtBitampLoader{
		CExtBitmap bitmap_;
	public:
		ExtBitampLoader(void* data){
			bitmap_.LoadFromMemory(data);
		}
		CBitmap* GetBitmap(){
			return &bitmap_;
		}
	};
	class CursorLoader{
		HCURSOR cursor_;
	public:
		CursorLoader(LPCTSTR cursorResName):cursor_(NULL)
		{
			cursor_ = AfxGetApp()->LoadCursor(cursorResName);
			if (!cursor_)
				cursor_ = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		}
		~CursorLoader(){
			if (cursor_)
				DestroyCursor(cursor_);
			cursor_ = NULL;
		}
		HCURSOR Get(){
			return cursor_;
		}
	};
}
namespace treelist_res{
	CBitmap* GetBackgroundBitmap(){
		static ExtBitampLoader bitmap(bm_bkgnd);
		return bitmap.GetBitmap();
	}
	CBitmap* GetBackgroundBitmap2(){
		static ExtBitampLoader bitmap(bm_bkgnd2);
		return bitmap.GetBitmap();
	}
	CBitmap* GetCheckboxBitmap(){
		static ExtBitampLoader bitmap(bm_checkbox);
		return bitmap.GetBitmap();
	}
	CBitmap* GetLockboxBitmap(){
		static ExtBitampLoader bitmap(bm_lockbox);
		return bitmap.GetBitmap();
	}
	CBitmap* GetDefheadBitmap(){
		static ExtBitampLoader bitmap(bm_def_head);
		return bitmap.GetBitmap();
	}
	CBitmap* GetDefsortBitmap(){
		static ExtBitampLoader bitmap(bm_def_sort);
		return bitmap.GetBitmap();
	}
	HCURSOR GetHeaderSplitCursor(){
		static CursorLoader cursor(_T("IDC_TREELIST_CURSOR_SPLIT"));
		return cursor.Get();
	}
	HCURSOR GetHeaderSizeCursor(){
		static CursorLoader cursor(_T("IDC_TREELIST_CURSOR_SIZE"));
		return cursor.Get();
	}
}