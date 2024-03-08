// RecentFilesMgr.h: interface for the RecentFilesMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECENTFILESMGR_H__5BF117EB_0054_45CD_907C_E68AF9492294__INCLUDED_)
#define AFX_RECENTFILESMGR_H__5BF117EB_0054_45CD_907C_E68AF9492294__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <deque>
//упаравляет списком недавно открытых файлов

const int  DEFAULT_MAX_COUNT = 5;
class RecentFilesMgr  
{
	typedef  std::deque<CString> FileNamesDEQ_t;
	FileNamesDEQ_t m_FileNames;
	int m_iMaxCount;
public:
	RecentFilesMgr();
	virtual ~RecentFilesMgr();

	//влияет на количество сохраненных в реестре файлов
	bool SetFilesCount(int _cont);
	bool AddFile(const CString & strFile);

	bool Load();
	bool Save();
private:
	bool KillLastNFiles(int N);
	//вставляет список файлов в меню
	//принимает меню и позицию элемента меню,
	//в место которого будет вставлен список
	bool ToMenu(CMenu* hMenu, int iInPlace);
};

#endif // !defined(AFX_RECENTFILESMGR_H__5BF117EB_0054_45CD_907C_E68AF9492294__INCLUDED_)
