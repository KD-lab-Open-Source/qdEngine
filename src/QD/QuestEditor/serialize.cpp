#include "stdafx.h"

#include "serialize.h"
#include <ctype.h>
#include <iostream>
#include <locale>

/// Символ используемый для разделения
const TCHAR DELIMETER = _T('@');
std::ostream& operator<<(std::ostream& out, RECT const& rect){
	out<<rect.left<<_T(' ')<<rect.right<<_T(' ')<<rect.top<<_T(' ')<<rect.bottom;
	return out;
}
std::istream& operator>>(std::istream& in, RECT& rect){
	in>>rect.left>>rect.right>>rect.top>>rect.bottom;
	return in;
}

std::ostream& operator<<(std::ostream& out, std::pair<std::string, CRect> const& value){
	out<<value.first<<DELIMETER<<value.second;
	return out;
}
std::istream& operator>>(std::istream& in, std::pair<std::string, CRect>& value){
	if (std::isspace((char)in.peek(), in.getloc())) 
		in.ignore();
	std::getline(in, value.first,DELIMETER) >>value.second;
	return in;
}