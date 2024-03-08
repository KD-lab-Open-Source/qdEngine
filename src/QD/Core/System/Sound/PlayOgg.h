#ifndef _PLAYMPP_H_
#define _PLAYMPP_H_
#pragma once
//Balmer

#include <dsound.h>

#define MPP_STAT
#ifdef MPP_STAT
double MpegCPUUsing();//���������� ������������ �� ������������ Mpeg ����� (1 - �� ����� ��������)
#endif MPP_STAT

//
bool MpegInitLibrary(void* LPDIRECTSOUND_pDS);
void MpegDeinitLibrary();

enum MpegState
{
	MPEG_STOP=0,
	MPEG_PLAY=1,
	MPEG_PAUSE=2,
};

//���������� ������ � �������� (�� �� ����� �����, 
//����� ��������� �� 26 ��)
//���� ��� ������� � ����������, ������������ �� �� 
//�������������. � ��������� ���������� ���� ���� 
//����� ������� �� �����
double MpegGetLen(const char* fname);

typedef double MpegPos;

class MpegSound
{
	class MppLoad* pMppLoad;
	MpegSound *prev,*next;

	DWORD sizeDSBuffer;
	bool b_cycled;
	long volume;

	WAVEFORMATEX  wave_format;
	LPDIRECTSOUNDBUFFER pDSBuffer;
	DWORD dwWriteOffset;
	DWORD Wraps;//������� ��� ��� ������� �������� ������
	DWORD BeginBufferOffset;
	DWORD OldWraps,OldBeginBufferOffset;
	DWORD OffsetBeginPlayFile;//� ������ ����� ����� �������� ����

	bool bOldFrame;//����������, ���� �������� ������ ������, � ������� �����

	char fname[260];

	bool clear_end_buffer;
	MpegState mpeg_state;

	DWORD deferred_prev_sample;
	DWORD SeekSkipByte;//������� ���� ���������� ���������� � ������ ������
	MpegPos deferred_sample;
	char deferred_fname[260];

	enum {block_size=36*32};//block_size==BLK_SIZE
	short last_signal[block_size*2];
	bool last_signal_is_full;
	int  last_signal_offset;

	void HammingCorrect(short* pData,int len);

	bool enable_fade;
	double fade_begin_time,fade_time;
	long fade_begin_volume,fade_end_volume;
public:
	MpegSound();
	~MpegSound();

	bool OpenToPlay(const char* fname,bool cycled=true);
	void Stop();
	void Pause();
	void Resume();

	const char* GetFileName();
	//�� ������ ������, ���� ��������� ������ ����� 0
	//(�� ��������� ������ ��������)
	void SetPauseIfNullVolume(bool set=true);

	MpegState IsPlay();

	bool DebugRealPlay();//������������� �� ������������� ������

	void SetVolume(int volume);//0..255
	int GetVolume();

	inline int GetSamplePerSecond(){return 44100;}
	//����� ��������� � ��������

	//���������� �������� ������ � �������
	inline int GetBufferLen(){return sizeDSBuffer/4;}

	//���������� �������� ��������� � ������� �� new_volume �� ����� time
	//��������� ��� ����� ����� ��� ������ SetVolume
	//����� �������� � DeferredSeek
	bool FadeVolume(float time,int new_volume=0);

	float GetLen();//� ��������
	float GetCurPos();//� �������� (�������)
protected:
	void InternalMpegSetVolume(int _volume);
	bool InitSoundBuffer();
	void InternalMpegStop();
	bool InternalMpegOpenToPlay(const char* _fname,bool cycled);

	void TimeCallbackTrue();

	//� ����� ����� ������ ���� ����� ������, 
	//���� � ���� �������� offset ����
	void AddWriteOffset(DWORD offset);

	friend DWORD WINAPI MpegThreadProc(LPVOID lpParameter);
	friend void MpegDeinitLibrary();

	bool DefferredSeek(DWORD cur_pos,DWORD cur_write_byte);

	void ClearFade();
	void FadeQuant();
};

#endif // _PLAYMPP_H_
