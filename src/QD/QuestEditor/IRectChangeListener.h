#ifndef RECT_CHANGE_LISTENER_H
#define RECT_CHANGE_LISTENER_H

#pragma once

class CustomRectTracker;
/**
	����� ����������� ���� ���������, ����� �������� ����������� ��
	CustomRectTracker � ���, ��� ������������� ���������
	������������ ��������� � CustomRectTracker
 */
interface IRectChangeListener
{
	/// Callback �� ������� �������� ��������������
	virtual void onRectChange(CustomRectTracker& rectTracker, CRect const& oldRect) = 0;
};

#endif//RECT_CHANGE_LISTENER_H