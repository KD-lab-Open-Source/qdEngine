#pragma once

//! ����������� ����� ����� ���� ��������� ��������� � ��������� �����
/*!
	������ ���������� � ��������� ���� ��������� ��������� � ���������� �����, 
	� ������������� � ��� ������.
	��������� �������� ������������ ����� ������
*/
class Project;

class ViewActivator
{
	//! ������ �� ���� ��������� �����
	/*!
		���� true, �� �� ������ �������� �����
		false - �������� ���������
	*/
	bool isSceneViewActive_;
	//! ��������� �� ������ �������
	Project* project_;

	const ViewActivator& operator = (const ViewActivator&);
	ViewActivator(const ViewActivator&);
public:
	ViewActivator(Project* p):isSceneViewActive_(true), project_(p){}

	bool getSceneViewActive() const;
	//! ����������� ����� �������� � � ������������ � ��� ������ ��������� ����
	void operator = (bool b);
};

