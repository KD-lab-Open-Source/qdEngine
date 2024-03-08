// reg_setting.cpp: implementation of the reg_setting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "questeditor.h"
#include "reg_setting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

reg_setting::reg_setting()
{

}

reg_setting::reg_setting(const CString &strSecName, const CString& strValueName, int iDefVal)
:Setting(iDefVal), m_strSectionName(strSecName), m_strValueName(strValueName)
{}
reg_setting::reg_setting(const CString &strSecName, const CString& strValueName, const CString& strDefVal)
:Setting(strDefVal), m_strSectionName(strSecName), m_strValueName(strValueName)
{}

reg_setting::~reg_setting()
{

}

reg_setting::reg_setting(const reg_setting& rs):Setting(rs)
{
	m_strSectionName	= rs.m_strSectionName;
	m_strValueName		= rs.m_strValueName;
}

const reg_setting& reg_setting::operator=(const reg_setting& rs)
{
	Setting::operator = (rs);

	m_strSectionName	= rs.m_strSectionName;
	m_strValueName		= rs.m_strValueName;

	return *this;
}
