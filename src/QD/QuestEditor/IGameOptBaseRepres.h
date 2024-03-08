/********************************************************************
	created:	2003/06/20
	created:	20:6:2003   13:31
	filename: 	d:\Projects\Quest\QuestEditor\IGameOptBaseRepres.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	IGameOptBaseRepres
	file ext:	h
	Powerd by:	�����
	
	purpose:	�������� ������������� ����� ����
*********************************************************************/

#pragma once

#include "qd_interface_element.h"

/**
	���������, ������� ��������� �����-������ ��� ����� ����
 */
interface IGameOptionRepres
{
	/// �������� ����� ����
	virtual CString const							description() const = 0;
	/// ID ����� ����
	virtual qdInterfaceElement::option_ID_t const	optionId() const = 0;
	inline virtual ~IGameOptionRepres() = 0;
};

inline IGameOptionRepres::~IGameOptionRepres()
{}

inline bool operator == (IGameOptionRepres const& lhs, IGameOptionRepres const& rhs){
	return (lhs.optionId() == rhs.optionId());
}
inline bool operator == (IGameOptionRepres const& lhs, 
						 qdInterfaceElement::option_ID_t const& rhs){
	return (lhs.optionId() == rhs);
}