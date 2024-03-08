/********************************************************************
created:	2002/04/30
created:	30:4:2002   16:33
filename: 	D:\Projects\Quest\QuestEditor\name_op.h
file path:	D:\Projects\Quest\QuestEditor
file base:	name_op
file ext:	h
author:		Илюха

  purpose:	операции с именами: Тест на уникальность и получение уникального
  имени
*********************************************************************/


#ifndef __NAME_OPERATION_HEADER__

#define __NAME_OPERATION_HEADER__
#pragma once
#include <algorithm>

/// Сравнивает имя объекта со строкой.
template<class _Obj>
struct eq_str : std::binary_function<_Obj, LPCTSTR, bool>{
	bool operator()(_Obj s1, LPCTSTR s2) const {
		return (s1->name()&&strcmp(s1->name(), s2) == 0);
	}
};
/// тест на уникальность имени
template<class _Cont>
bool uniq_name(LPCTSTR strText, const _Cont& _c)
{
	_Cont::const_iterator _res = std::find_if(_c.begin(), _c.end(),
		std::bind2nd(eq_str<_Cont::value_type>(), strText));
	return (_res == _c.end());
}
/// Возвращает уникальное имя
/**
	Имя получается путем добавление к исходному имени числа. Если такое имя уже есть,
	то число увеличивается на единицу
 */
template<class _Cont>
CString get_unique_name(const CString& src_name, const _Cont& cnt)
{
	CString str = src_name;
	int i = 0;
	//пытаемся получить уникальное имя
	while(!uniq_name(str, cnt))
		str.Format("%s%d", (LPCTSTR)src_name, ++i);
	
	return str;
}

#endif//__NAME_OPERATION_HEADER__