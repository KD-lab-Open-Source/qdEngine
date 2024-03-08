// RecentFilesMgr.cpp: implementation of the RecentFilesMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "questeditor.h"
#include "RecentFilesMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RecentFilesMgr::RecentFilesMgr()
{
	m_iMaxCount = DEFAULT_MAX_COUNT;
}

RecentFilesMgr::~RecentFilesMgr()
{

}

bool RecentFilesMgr::SetFilesCount(int _cont)
{
	m_iMaxCount = _cont;
	int size = m_FileNames.size();
	if(size > m_iMaxCount)
		KillLastNFiles(size - m_iMaxCount);
	return true;
}

bool RecentFilesMgr::AddFile(const CString & strFile)
{

	return true;
}

bool RecentFilesMgr::Load()
{
	CWinApp* papp = AfxGetApp();

	return true;
}

bool RecentFilesMgr::Save()
{
	return true;
}

//вставляет список файлов в меню
//принимает меню и позицию элемента меню,
//в место которого будет вставлен список
bool RecentFilesMgr::ToMenu(CMenu* hMenu, int iInPlace)
{
	return true;
}

bool RecentFilesMgr::KillLastNFiles(int N)
{
	int size = m_FileNames.size();
	FileNamesDEQ_t::iterator _itr = m_FileNames.begin();
	_ASSERT((size - N)>0);
	std::advance(_itr, (size - N + 1));
	m_FileNames.erase(_itr, m_FileNames.end());
	return true;
}
