// Setting.h: interface for the Setting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTING_H__4FF71905_DEFF_42AA_818C_69B84CC29968__INCLUDED_)
#define AFX_SETTING_H__4FF71905_DEFF_42AA_818C_69B84CC29968__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
	Класс описывающий значение какого-либо параметра.
	Хорошая замена этому класс появилась гораздо позже и называется boost::variant
	Все очень тупо. Поэтому дополнительные комменты по запросу
 */
class Setting  
{
	
public:
	enum eSettingType{TYPE_UNKNOWN, TYPE_INT, TYPE_STRING, TYPE_FLOAT};

	Setting();
	virtual ~Setting();

	Setting(const CString& val);
	Setting(int val);
	Setting(float val);
	Setting(const Setting& _s);
	const Setting& operator = (const Setting& _s);

	operator int() const{
		ASSERT(m_type == TYPE_INT);
		return m_iVal;
	}
	operator CString() const{
		ASSERT(m_type == TYPE_STRING);
		return m_strVal;
	}

	operator LPCTSTR() const{
		ASSERT(m_type == TYPE_STRING);
		return LPCTSTR(m_strVal);
	}
	
	operator float() const{
		ASSERT(m_type == TYPE_FLOAT);
		return m_fVal;
	}
	eSettingType type() const {return m_type;}

	bool set_value(const CString& str){
		m_strVal = str;
		return true;
	}
	bool set_value(int iVal){
		m_iVal = iVal;
		return true;
	}
	bool set_value(float fVal){
		m_fVal = fVal;
		return true;
	}
private:
	/// Строковое значение
	CString m_strVal;
	/// Место под значения типа int или float
	union{
		int		m_iVal;
		float m_fVal;
	};
	/// Какой тип данных храним
	eSettingType m_type;
};

#endif //!defined(AFX_SETTING_H__4FF71905_DEFF_42AA_818C_69B84CC29968__INCLUDED_)
