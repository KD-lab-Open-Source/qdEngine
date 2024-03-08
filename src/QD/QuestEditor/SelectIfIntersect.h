#ifndef SELECT_IFINTERSECT_H
#define SELECT_IFINTERSECT_H

#pragma once

#include "qd_trigger_element.h"

class SelectionManager;
/**
	�������, ������� �������� �������� �������� �� ��������� ��������������
 */
class SelectIfIntersect
{
	/// �������� ������������� ������
	CRect const& rect_;
	/// ����� ����������� ��������� ����������� ��� ��������� ��������
	SelectionManager& selectionManager_;
public:
	SelectIfIntersect(CRect const& rect, SelectionManager& selectionManager);
	/// ���� ������� ��������� � �������������, �� �� ����������
	void operator()(qdTriggerElementPtr const& ptr) const;
};

#endif