#ifndef __INFOZIP_API_H__
#define __INFOZIP_API_H__

//! ��������� � Info-ZIP ��� �������� ����������� ��������.
/**
��������� �� Info-ZIP v2.3.
*/
namespace infozip_api
{
	//! ������������� Info-ZIP. �� ��������� ������� ������ ���������.
	bool init();
	//! ��������� �����.
	//! ���������� - �� 0 (���.) �� 9 (����.).
	bool set_options(int compression = 0);
	//! ���������� ����� � ������ file_name � ����� � ������ zip_file_name.
	bool add_to_zip(const char* zip_file_name, const char* file_name, int compression = 0);
};

#endif // __INFOZIP_API_H__

