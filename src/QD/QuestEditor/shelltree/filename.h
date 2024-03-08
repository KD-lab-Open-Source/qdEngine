// FileName.h: interface for the CFileName class.
//
//////////////////////////////////////////////////////////////////////
//
// This source is part of CShellTree - Selom Ofori
// 
// Version: 1.02 (any previously unversioned copies are older/inferior
//
// This code is free for all to use. Mutatilate it as much as you want
// See MFCENUM sample from microsoft

#if !defined(AFX_FILENAME_H__DD386565_9863_11D1_B10E_40F603C10000__INCLUDED_)
#define AFX_FILENAME_H__DD386565_9863_11D1_B10E_40F603C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CFileName  
{
public:
	CFileName(CString szFileName="");
	virtual ~CFileName();

	void		SetFileName(CString szFileName);
	CString		GetFileName();
	CString		GetRoot();
	CString		GetFileTitle();
	CString		GetDescription();
	bool		Exist();

private:
	CString		m_szFileName;
	char		m_szDrive[_MAX_DRIVE];
	char		m_szDir[_MAX_DIR];
	char		m_szFname[_MAX_FNAME];
	char		m_szExt[_MAX_EXT];
};

#endif //!defined(AFX_FILENAME_H__DD386565_9863_11D1_B10E_40F603C10000__INCLUDED_)
