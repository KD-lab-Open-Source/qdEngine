
#pragma once

/// ������ ��� �������. 
/**
	������ ���� ������ �� ������. 
	������� ��� ������������� �� ����� ���������
 */
class DirMakeWizardData{
public:
	/// �����, � ������ ����� ��������� ��������� �����
	CString m_strDir;
	/// ��������� ����,  ����� ��� ������� ����� �������
	int scenes_count;
	DirMakeWizardData():scenes_count(0){}
};