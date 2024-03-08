#ifndef __DS_SND_DISPATCHER_H__
#define __DS_SND_DISPATCHER_H__

#include "snd_dispatcher.h"
#include "ds_sound.h"

//! ��������� ������ �� DirectSound.
class ds_sndDispatcher : public sndDispatcher
{
public:
	ds_sndDispatcher();
	~ds_sndDispatcher();

	//! ���������� �����.
	void quant();
	//! ��������� ������������ �����.
	bool play_sound(const sndSound* snd,bool loop,float start_position = 0.0f,int vol = 255);
	//! ������������� ������������ �����.
	bool stop_sound(const sndSound* snd);
	//! ������������� ������������ �����.
	bool stop_sound(const sndHandle* handle);
	//! ���������� ��������� ����� (��������/���������� � �.�.).
	sndSound::status_t sound_status(const sndHandle* handle) const;
	//! ���������� ��������� ����� (��������/���������� � �.�.).
	sndSound::status_t sound_status(const sndSound* snd) const;
	//! ���������� ������� ������� �����, �� 0.0 (������) �� 1.0 (�����).
	float sound_position(const sndHandle* snd) const;
	//! ��������� ������� �����.
	bool set_sound_frequency(const sndHandle* snd,float coeff);

	//! ������������� ��� �����.
	void stop_sounds();
	//! ������ ��� �������� � ������ ������ ����� �� �����.
	void pause_sounds();
	//! ������������ ������������ ���� ������, ������� ���� ���������� �� �����.
	void resume_sounds();

protected:

	//! ���������� ��������� ���������.
	bool update_volume();

	bool update_frequency();

private:

	typedef std::list<dsSound> sound_list_t;
	//! ������ �������� ������.
	sound_list_t sounds_;

	//! ��������� �� DirectSound ���������.
	LPDIRECTSOUND sound_device_;
};

#endif /* __DS_SND_DISPATCHER_H__ */
