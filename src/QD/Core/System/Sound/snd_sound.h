
#ifndef __SND_SOUND_H__
#define __SND_SOUND_H__

class wavSound;
class qdNamedObject;

//! ����� ��� ���������� �������.
class sndHandle
{
public:
	sndHandle(){ };
	virtual ~sndHandle(){ };
};

//! ������� ����� ��� ������.
class sndSound
{
public:
	explicit sndSound(const wavSound* snd,const sndHandle* h = NULL) : sound_(snd), handle_(h) { }
	sndSound(const sndSound& snd) : sound_(snd.sound_), handle_(snd.handle_) { }
	virtual ~sndSound(){ };

	sndSound& operator = (const sndSound& s){
		if(this == &s) return *this;

		sound_ = s.sound_;
		handle_ = s.handle_;

		return *this;
	}

	//! ��������� �����.
	enum status_t {
		//! ���� �� �������������
		SOUND_STOPPED,
		//! ���� �������������
		SOUND_PAUSED,
		//! ���� ������������
		SOUND_PLAYING
	};

	//! ���������� ��������� �����.
	virtual status_t status() const { return SOUND_STOPPED; }

	//! ���������� ��������� �� ������ �����.
	const wavSound* sound() const { return sound_; }
	//! ���������� ��������� �� ����� �����.
	const sndHandle* handle() const { return handle_; }

private:
	//! ��������� �� ������.
	const wavSound* sound_;
	//! ��������� �� ����� �����.
	const sndHandle* handle_;
};

#endif /* __SND_SOUND_H__ */