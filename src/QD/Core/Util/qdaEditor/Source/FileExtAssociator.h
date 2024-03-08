#ifndef __FILE_EXT_ASSOCIATOR_H__
#define __FILE_EXT_ASSOCIATOR_H__

/**
	����� ����������� � ������� ����� ���������� ����������� ������ ����������.
	������� �������� ��� ������ � �����-�� ������ �� MSDN

	���������� � ������� ����� ���������� ������ � ������ ����������������
	IDS_APPS_SUB_KEY - ��� exe-����� ���������� 
	IDS_PROG_ID_REG_KEY	- � ��� ���������� ������. ��� ������ ���������� QuestEditor.exe
	� ��������� � ��� "qdaEditor.1"
	IDS_PROG_ID_KEY_VALUE - ��� �������� ��� ����������� �����. ����� ��������� ��������
							����, ��� �� ����� ���������
*/
class FileExtAssociator  
{
public:
	FileExtAssociator();
	~FileExtAssociator();

	bool associate(const char* ext_string);

private:

	bool setShellSubKeys(HKEY key);
};

#endif // __FILE_EXT_ASSOCIATOR_H__
