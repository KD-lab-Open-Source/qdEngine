#ifndef __PLAY_OGG_H__
#define __PLAY_OGG_H__

enum OggState
{
	OGG_STOPPED = 0,
	OGG_PLAYING,
	OGG_PAUSED
};

struct OggCallbacks
{
	int		(*open_func)	(void *datasource, const char* file_name);
	size_t	(*read_func)	(void *ptr, size_t size, size_t nmemb, void *datasource);
	int		(*seek_func)	(void *datasource, __int64 offset, int dir);
	int		(*close_func)	(void *datasource);
	long	(*tell_func)	(void *datasource);
};

class OggPlayerImpl;

class OggPlayer
{
public:

	OggPlayer();
	~OggPlayer();

	bool play(const char* fname, bool cycled = true, void* data_handle = 0);
	void stop();
	void pause();
	void resume();

	/// имя проигрываемого файла
	const char* fileName() const;

	OggState state() const;

	/// громкость от 0 до 255
	void setVolume(int volume);
	int volume() const;
	/// плавно изменить громкость 
	/// new_volume - от 0 до 1, тишина и текущая выставленная громкость
	bool fadeVolume(float time, float new_volume = 0.f);
	/// если установить в true, то переключение треков будет через fade
	void setAutoFade(bool enable, float fade_time);
	/// останавливать проигрывание если выставлена нулевая громкость 
	/// по умолчанию - продолжает играться
	void setPauseIfZeroVolume(bool set = true);

	/// длительность проигрываемого файла в секундах
	float length();
	/// текущая позиция в секундах
	float tell();

	static void setCallbacks(OggCallbacks* callbacks);
	static OggCallbacks* callbacks();

	static double getLength(const char* fname);

	static bool initLibrary(void* dsound);
	static void finitLibrary();

private:

	OggPlayerImpl* player_;
};

class OggStreamImpl;

class OggStream
{
public:
	OggStream(const char* file_name = 0, void* data_handle = 0);
	~OggStream();

	bool open(const char* file_name, void* data_handle = 0);
	bool isOpen() const;
	/// возвращает сколько байт было прочитано
	int read_frame(char* buffer, int buffer_size);
	void close();

	/// размер распакованного звука в байтах
	int uncompressedSize() const;

	/// длительность звука в секундах
	float length() const;
	/// текущая позиция в секундах
	float tell() const;
	
	/// количество каналов, моно/стерео - 1 или 2
	int channels() const;
	/// частота дискретизации (количество сэмплов в секунду)
	int samplingRate() const;

private:
	OggStreamImpl* stream_;
};


#define _LIB_NAME "PlayOgg"
#include "AutomaticLink.h"

#endif // __PLAY_OGG_H__
