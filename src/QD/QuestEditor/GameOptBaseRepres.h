/********************************************************************
	created:	2003/06/20
	created:	20:6:2003   13:16
	filename: 	d:\Projects\Quest\QuestEditor\GameOptRepres.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	GameOptRepres
	file ext:	h
	Powerd by:	�����
	
	purpose:	������������� ����� ����.
*********************************************************************/
#pragma once

#include "IGameOptBaseRepres.h"

/**
	��������� ��������� ����� ����
	������������ ��������� � GameOptionRepresMngr
 */
class GameOptionBaseRepres : public IGameOptionRepres
{
	/// ������ ��������
	CString const m_strDescrp;
	/// �������������
	qdInterfaceElement::option_ID_t const m_eOptionId;
public:
	GameOptionBaseRepres(qdInterfaceElement::option_ID_t oId, UINT_PTR descrId)
		:m_strDescrp(reinterpret_cast<LPTSTR>(descrId))
		,m_eOptionId(oId)
	{}

	inline virtual ~GameOptionBaseRepres(){};
	/// �������� �����
	CString const	description() const{
		return m_strDescrp;
	}
	/// �������������
	qdInterfaceElement::option_ID_t const optionId() const{
		return m_eOptionId;
	}
};