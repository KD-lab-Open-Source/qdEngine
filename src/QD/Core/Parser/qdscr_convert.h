#ifndef __QDSCR_CONVERT_H__
#define __QDSCR_CONVERT_H__

//! �������������� ������� ������� � ����� XML ������.
bool qdscr_convert_to_XML(const char* file_name,const char* new_file_name = NULL);
//! ���������� true, ���� ������ � ����� ������� (��������� ������ ������������ ���������� ����� �����). 
bool qdscr_is_XML(const char* file_name);
//! ������ ���������� ����� ����� (�� .qds).
const char* qdscr_get_XML_file_name(const char* file_name);

#endif /* __QDSCR_CONVERT_H__ */
