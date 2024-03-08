// reg_setting.h: interface for the reg_setting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REG_SETTING_H__4BF154A7_79F3_42AC_86DE_3D65B63F85B4__INCLUDED_)
#define AFX_REG_SETTING_H__4BF154A7_79F3_42AC_86DE_3D65B63F85B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Setting.h"

/**
	Класс сохраняет помимо значения параметра его положение в реестре
	Загрузка значений происходит в CQuestEditorApp::load_settings_list()
 */
class reg_setting : public Setting  
{
private:
	/// Имя ключа
	CString m_strSectionName;
	/// Имя значения
	CString m_strValueName;
public:
	reg_setting();

	reg_setting(const CString &strSecName, const CString& strValueName, int iDefVal);
	reg_setting(const CString &strSecName, const CString& strValueName, const CString& strDefVal);

	reg_setting(const reg_setting& rs);
	const reg_setting& operator=(const reg_setting& rs);

	virtual ~reg_setting();

	const CString& GetSecName() const{
		return m_strSectionName;
	}
	const CString& GetValName() const {
		return m_strValueName;
	}
};

#endif //!defined(AFX_REG_SETTING_H__4BF154A7_79F3_42AC_86DE_3D65B63F85B4__INCLUDED_)
