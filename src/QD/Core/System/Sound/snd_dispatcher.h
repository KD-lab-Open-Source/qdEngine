#ifndef __SND_DISPATCHER_H__
#define __SND_DISPATCHER_H__

#include "snd_sound.h"

class wavSound;

//! ��������� �����.
class sndDispatcher
{
public:
	sndDispatcher();
	virtual ~sndDispatcher();

	//! ���������� �����.
	virtual void quant() = 0;
	//! ��������� ������������ �����.
	virtual bool play_sound(const sndSound* snd,bool loop,float start_position = 0.0f,int vol = 255) = 0;
	//! ������������� ������������ �����.
	virtual bool stop_sound(const sndSound* snd) = 0;
	//! ������������� ������������ �����.
	virtual bool stop_sound(const sndHandle* handle) = 0;
	//! ���������� ��������� ����� (��������/���������� � �.�.).
	virtual sndSound::status_t sound_status(const sndHandle* handle) const = 0;
	//! ���������� ��������� ����� (��������/���������� � �.�.).
	virtual sndSound::status_t sound_status(const sndSound* snd) const = 0;
	//! ���������� ������� ������� �����, �� 0.0 (������) �� 1.0 (�����).
	virtual float sound_position(const sndHandle* snd) const = 0;
	//! ��������� ������� �����.
	virtual bool set_sound_frequency(const sndHandle* snd,float coeff) = 0;

	//! ��������� ���������, �������� �������� - [0, 255].
	void set_volume(unsigned int vol);
	//! ���������� ������������� ���������, �������� �������� - [0, 255].
	unsigned int volume() const { return volume_; }

	//! ���������� ������������� ��������� � ���������.
	int volume_dB() const { return volume_dB_; }

	void set_frequency_coeff(float coeff){ frequency_coeff_ = coeff; update_frequency(); }
	float frequency_coeff() const { return frequency_coeff_; }

	//! �������� ��������� � ��������.
	static int convert_volume_to_dB(int vol);

	//! ������������� ��� �����.
	virtual void stop_sounds() = 0;
	//! ������ ��� �������� � ������ ������ ����� �� �����.
	virtual void pause_sounds() = 0;
	//! ������������ ������������ ���� ������, ������� ���� ���������� �� �����.
	virtual void resume_sounds() = 0;

	//! ������ ��� ����� �� ����� �� ������ resume().
	void pause(){
		is_paused_ = true;
		pause_sounds();
	}
	//! ������������ ������������ ���� ������.
	void resume(){
		is_paused_ = false;
		resume_sounds();
	}
	//! ���������� true, ���� ����� ���������� �� �����.
	bool is_paused() const { return is_paused_; }

	//! ���������� true, ���� ���� ��������.
	bool is_enabled() const { return is_enabled_; }
	//! �������� ����.
	void enable(){ is_enabled_ = true; }
	//! ��������� ����.
	void disable(){ is_enabled_ = false; stop_sounds(); }

	//! ���������� ��������� �� ������� ���������.
	static inline sndDispatcher* get_dispatcher(){ return dispatcher_ptr_; }
	//! ������������� ��������� �� ������� ���������.
	static inline sndDispatcher* set_dispatcher(sndDispatcher* p){ sndDispatcher* old_p = dispatcher_ptr_; dispatcher_ptr_ = p; return old_p; }

#ifdef _QUEST_EDITOR
	//! \brief � ����������� ���� �����������(���� ��� �������). � ����������� ����������,
	//! ���� ��� ��������
	class SoundDisabler{
		bool soundEnabled_;
	public:
		SoundDisabler();
		~SoundDisabler();
	};
#endif // _QUEST_EDITOR
protected:

	//! ���������� ��������� ���������.
	virtual bool update_volume() = 0;
	virtual bool update_frequency() = 0;

private:

	//! ���� ��������, ���� false.
	bool is_enabled_;

	//! ���������, �������� �������� - [0, 255].
	/**
	0 - ���� ��������� �������
	255 - ���� �������� � ������ ���������
	*/
	unsigned int volume_;

	//! ��������� � ���������, �������� �������� - [-10000, 0].
	/**
	-10000 - ���� ��������� �������
	0 - ���� �������� � ������ ���������
	*/
	int volume_dB_;

	float frequency_coeff_;

	//! �����.
	bool is_paused_;

	//! ������� ���������.
	static sndDispatcher* dispatcher_ptr_;
};

#endif /* __SND_DISPATCHER_H__ */

