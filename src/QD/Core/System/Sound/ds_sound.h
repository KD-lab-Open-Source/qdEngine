#ifndef __DS_SOUND_H__
#define __DS_SOUND_H__

#include <dsound.h>

#include "snd_sound.h"

class wavSound;

//! DirectSound ����.
class dsSound : public sndSound
{
public:
	dsSound(const sndSound& snd,LPDIRECTSOUND sound_device);
	dsSound(const dsSound& snd);
	~dsSound();

	//! ��������� ������������ �����.
	bool play();
	//! ������������� ������������ �����.
	bool stop();
	//! ������ ���� �� �����.
	void pause();
	//! ������������ ������������.
	void resume();
	//! ���������� true, ���� ���� �� �����.
	bool is_paused() const { if(flags_ & SOUND_FLAG_PAUSED) return true; else return false; }

	//! ���������� true, ���� ���� �� �������������.
	bool is_stopped() const;
	//! ���������� ��������� �����.
	sndSound::status_t status() const;

	//! ������������� ��������� �����, �������� - � ���������.
	/**
	�������� �������� ��������� - [-10000, 0]

	-10000 - ���� ������ �� ������,
	0 - ��������� ������ ����� �� ���������.
	*/
	bool set_volume(int vol);

	bool change_frequency(float coeff = 1.0f);

	//! ���������� ������� ������� �����, �������� ������������ �������� - [0.0, 1.0].
	float position() const;
	//! ������������� ������� ������� �����, �������� �������� ��������� - [0.0, 1.0].
	bool set_position(float pos);

	//! ������� DirectSoundBuffer.
	bool create_sound_buffer();
	//! ������� DirectSoundBuffer.
	bool release_sound_buffer();

	//! ��������/��������� ������������ �����.
	void toggle_looping(){ flags_ ^= SOUND_FLAG_LOOPING; }

private:
	//! ��������� �� ������ DirectSound.
	const LPDIRECTSOUND sound_device_;
	//! ��������� �� DirectSoundBuffer.
	LPDIRECTSOUNDBUFFER  sound_buffer_;

	//! �����
	enum {
		SOUND_FLAG_LOOPING	= 0x01,
		SOUND_FLAG_PAUSED	= 0x02
	};

	//! �����
	int flags_;
};

#endif /* __DS_SOUND_H__ */

