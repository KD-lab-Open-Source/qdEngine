
#ifndef __PLAYMPP_API_H__
#define __PLAYMPP_API_H__

//! ������������� mp+.
class MpegSound;

//! ��������� � ������������� mp+.
class mpegPlayer
{
public:
	//! ���������.
	enum mpeg_status_t {
		//! ������ ���������
		MPEG_STOPPED,
		//! ������ �� �����
		MPEG_PAUSED,
		//! ������ ������
		MPEG_PLAYING
	};

	~mpegPlayer();

	//! ��������� ������������ mp+ ����� file.
	/**
	���� �������� loop == true, �� ������ ���������.
	*/
	bool play(const char* file,bool loop = false,int vol = 256);
	//! ������������� ������������ mp+ �����.
	bool stop();
	//! ���������������� ������������ mp+ �����.
	bool pause();
	//! ���������� ������������ mp+ �����.
	bool resume();

	//! ���������� true, ���� ������������ ������ ���������.
	bool is_enabled() const { return is_enabled_; }
	//! ��������� ������������ ������.
	void enable(bool need_resume = true){ is_enabled_ = true; if(need_resume) resume(); }
	//! ��������� ������������ ������.
	void disable(){ is_enabled_ = false; pause(); }

	//! ���������� ������� ��������� �������������.
	mpeg_status_t status() const;
	//! ���������� true, ���� ������ ������.
	bool is_playing() const { return (status() == MPEG_PLAYING); }

	//! ���������� ��������� ������.
	unsigned int volume() const { return volume_; }
	//! ������������� ��������� ������.
	void set_volume(unsigned int vol);

	//! ������������� mp+ ����������.
	static bool init_library(void* dsound_device);
	//! ��������������� mp+ ����������.
	static void deinit_library();

	//! ���������� �������������, ����� ������� ���� ��� ���������� �������.
	static mpegPlayer& instance();

private:

	mpegPlayer();

	bool isOGG_;
	MpegSound* oggPlayer_;

	//! ������ ���������, ���� false.
	bool is_enabled_;

	//! ��������� ������, [0, 255].
	unsigned int volume_;
	unsigned int cur_track_volume_;

	//! ���� ����� true, ������ ���������� ��� ����������������.
	static bool is_library_initialized_;
};

#endif /* __PLAYMPP_API_H__ */
