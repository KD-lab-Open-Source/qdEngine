// Setting.cpp: implementation of the Setting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "questeditor.h"
#include "Setting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Setting::Setting():m_type(TYPE_UNKNOWN), m_iVal(0)
{

}

Setting::Setting(const CString& val):m_type(TYPE_STRING), m_strVal(val)
{}

Setting::Setting(float val):m_type(TYPE_FLOAT), m_fVal(val)
{}

Setting::Setting(int val):m_type(TYPE_INT), m_iVal(val)
{}

Setting::Setting(const Setting& _s)
{
	operator=(_s);
}
const Setting& Setting::operator = (const Setting& _s)
{
	m_type = _s.m_type;
	switch(m_type) {
	case TYPE_STRING:
		m_strVal = _s.m_strVal;
		break;
	case TYPE_INT:
		m_iVal = _s.m_iVal;
		break;
	case TYPE_FLOAT:
		m_fVal = _s.m_fVal;
	}
	return *this;
}

Setting::~Setting()
{
}
