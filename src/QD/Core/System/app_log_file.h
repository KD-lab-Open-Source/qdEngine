#ifndef __APP_LOG_FILE_H__
#define __APP_LOG_FILE_H__

#ifndef _QUEST_EDITOR
#define __APP_LOG_ENABLE__
#endif

class XStream;

//! ���������� ���-����.
/**
	���������� ������ � ���� (�� � ���������) ������ /log � ��������� ������.
	�� ��������� ����� � ���� qd_engine.log, ��� ����������� ���� ��� ����������.
*/
class appLogFile : public appLog
{
public:
	explicit appLogFile(const char* fname = NULL);
	~appLogFile();

	//! ���������� ���-���� � ������ fname.
	bool open(const char* fname = NULL);
	//! ���������� true, ���� ���-���� ������.
	bool is_opened() const { if(file_) return true; else return false; }
	//! ��������� ���-����.
	bool close();

	//! ����� � ���-���� ������ ������.
	appLog& operator << (const char* str);
	//! ����� � ���-���� ����� �����.
	appLog& operator << (int data);
	//! ����� � ���-���� ������������� ����� �����.
	appLog& operator << (unsigned int data);
	//! ����� � ���-���� ����� � ��������� ������.
	appLog& operator << (float data);

private:
	//! ����� ��� ������ � ���-����.
	XStream* file_;
	//! ��� ���-�����.
	std::string file_name_;

	XStream* file(){ if(!is_opened()) open(); return file_; }
};

#endif /* __APP_LOG_FILE_H__ */
