#pragma once

#include "qd_fwd.h"

class qdGameObjectAnimated;
/// �������� ����� ������� ��� ���������� ���������� �������� ������� ���������
class UpdateTriggerElementTitles
{
	UpdateTriggerElementTitles(void);
	~UpdateTriggerElementTitles(void);
public:
	/*!
 		��������� �� ���� �������� � ���� ��������� � ���. ���� ������� �������� 
		���������� ������, �� �������� qdTriggerElement::update_object_reference()
		\param chainList - ������ ������� ���������
		\param pno	- ������, ��� �������� ��������� �������� ��������� ���������
	*/
	static void update(qdTriggerChainList const& chainList, qdNamedObject const* obj);

	/// �������� update ��� ��������� �������
	static void updateForAnimated(qdTriggerChainList const& chainList, 
		qdGameObjectAnimated const* obj);
	/// �������� update ��� ��������� ����
	static void updateForZone(qdTriggerChainList const& chainList, qdGridZone* zone);
};
