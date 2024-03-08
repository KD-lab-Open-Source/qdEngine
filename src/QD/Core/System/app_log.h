#ifndef __APP_LOG_H__
#define __APP_LOG_H__

//! ���������� ��� - ������� �����.
/**
���������� ������ � ���� (�� � ���������) ������ /log � ��������� ������.
�� ��������� ����� � ���� qd_engine.log, ��� ����������� ���� ��� ����������.
*/
class appLog
{
public:
	appLog();
	virtual ~appLog() = 0;

	//! ���������� ������ �� ��� �� ��������� (qd_engine.log).
	static appLog& default_log();

	//! ���������� true, ���� ������ � ��� ���������.
	bool is_enabled() const { return is_enabled_; }
	//! ��������� ������ � ���.
	void enable(){ is_enabled_ = true; }

	//! ����� � ��� ������ ������.
	virtual appLog& operator << (const char* str){ return *this; }
	//! ����� � ��� ����� �����.
	virtual appLog& operator << (int data){ return *this; }
	//! ����� � ��� ������������� ����� �����.
	virtual appLog& operator << (unsigned int data){ return *this; }
	//! ����� � ��� ����� � ��������� ������.
	virtual appLog& operator << (float data){ return *this; }

	void set_time(unsigned tm){ time_ = tm; }
	const char* time_string() const;

private:
	//! ����� true, ���� ������ � ���-���� ���������.
	bool is_enabled_;

	//! ������� �����.
	unsigned time_;
};

#endif /* __APP_LOG_H__ */
