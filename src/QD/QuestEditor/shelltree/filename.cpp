// FileName.cpp: implementation of the CFileName class.
//
//////////////////////////////////////////////////////////////////////
//
// This source is part of CShellTree - Selom Ofori
// 
// Version: 1.02 (any previously unversioned copies are older/inferior
//
// This code is free for all to use. Mutatilate it as much as you want
// See MFCENUM sample from microsoft

#include "stdafx.h"
#include "FileName.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileName::CFileName(CString szFileName)
{
	m_szFileName = szFileName;
}

CFileName::~CFileName()
{

}

void CFileName::SetFileName(CString szFileName)
{
	m_szFileName = szFileName;
}

/*****************************************************************
*
*	Function:	GetFileName()
*
*	Purpose:	Retrieves current filename minus the path
*
*	Remarks:	if the filename is "c:\incoming\hello.txt", this
*				function returns "hello.txt".
*
******************************************************************/
CString CFileName::GetFileName()
{
	CString szFileName;

	_splitpath(m_szFileName, m_szDrive, m_szDir, m_szFname, m_szExt);
	szFileName = m_szFname;
	szFileName += m_szExt;

	return szFileName;
}

/*****************************************************************
*
*	Function:	GetRoot()
*
*	Purpose:	Retrieves the path only of the current filename.
*
*	Remarks:	if the filename is "c:\incoming\hello.txt", this
*				function returns "c:\incoming\".
*
******************************************************************/
CString CFileName::GetRoot()
{
	CString szFileName;

	_splitpath(m_szFileName, m_szDrive, m_szDir, m_szFname, m_szExt);
	szFileName = m_szDrive;
	szFileName += m_szDir;

	return szFileName;
}

/*****************************************************************
*
*	Function:	GetFileTitle()
*
*	Purpose:	Retrieves the title of the filename excluding
*				the path and extension.
*
*	Remarks:	if the filename is "c:\incoming\hello.txt", this
*				function returns "hello".
*
******************************************************************/
CString CFileName::GetFileTitle()
{
	CString szFileName;

	_splitpath(m_szFileName, m_szDrive, m_szDir, m_szFname, m_szExt);
	szFileName = m_szFname;

	return szFileName;
}

/*****************************************************************
*
*	Function:	GetDescription()
*
*	Purpose:	Returns the description of the file
*
******************************************************************/
CString CFileName::GetDescription()
{
	CString		szTypeName;
	SHFILEINFO	sfi;

	SHGetFileInfo(m_szFileName,
                 0,
                 &sfi, 
                 sizeof(SHFILEINFO), 
                 SHGFI_TYPENAME);

   szTypeName = sfi.szTypeName;

   return szTypeName;
}

/*****************************************************************
*
*	Function:	Exists()
*
*	Purpose:	Determines whether a file or directory exists.
*
******************************************************************/
bool CFileName::Exist()
{
	WIN32_FIND_DATA fd;

	CString	szFindPath=m_szFileName;
	int nSlash = szFindPath.ReverseFind('\\');

	if( nSlash == szFindPath.GetLength()-1)
	{
		szFindPath = szFindPath.Left(nSlash);
	}

	HANDLE hFind = FindFirstFile( szFindPath, &fd );

	if ( hFind != INVALID_HANDLE_VALUE )
	{
		FindClose( hFind );
	}

	return hFind != INVALID_HANDLE_VALUE;
}


