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

typedef const char* (*MpegFuncGetNextFile)();

//���������� ������ � �������� (�� �� ����� �����, 
//����� ��������� �� 26 ��)
//���� ��� ������� � ����������, ������������ �� �� 
//�������������. � ��������� ���������� ���� ���� 
//����� ������� �� �����
double MpegGetLen(const char* fname);

#pragma pack(push,4)
struct MpegPos
{
	DWORD PrevBitsRead;
	DWORD N;
	DWORD NewBeginSample;//�����, �� ������� ���������� ������������������
};
#pragma pack(pop)

class MpegSound
{
	class MppLoad* pMppLoad;
	MpegSound *prev,*next;

	DWORD sizeDSBuffer;
	bool b_cycled;
	long volume;
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

	MpegFuncGetNextFile pMpegFuncGetNextFile;

	bool b_deferred_seek;
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

	//�� ��������� � �������� ��������� � �������

	//���������� ������ � ������� (�� �� ����� �����, 
	//����� ��������� �� 1152 ������,26 �� � ������� �������)
	//-1 - ������
	int GetLen();

	//GetCurPos - ���������� ������� ���������
	//���� *prevfile==true, �� ������������ ���������� ����,
	//�� ��� ����� ��������������� �������
	int GetCurPos(bool* prevfile=NULL);

	//GetCurPos - ���������� ������� ��������� ������ � ������
	//���� *prevfile==true, �� ������������ ���������� ����,
	//�� ��� ����� ��������������� �������
	int GetCurWritePos(bool* prevfile=NULL);

	//���������� �������� ������ � �������
	inline int GetBufferLen(){return sizeDSBuffer/4;}

	//��� ��� ���������� ������ ���������� �� ���������
	//�� SetGetNextFile,DeferredSeek
	//������ ���������� �������  -
	//�������������� �� GetBufferLen() ������.
	//!!! ��� callback ������� ���������� �� ������� ������

	//����� �� ����� ����� ������� ��������� �����
	//� ���� ������������ ������� MpegFuncGetNextFile,
	//�� ��� ����������� ��� ����, ����� ������,
	//����� ���� ����� �������� ���������
	void SetGetNextFile(MpegFuncGetNextFile p);

	//sample - ���������� ������� �� ������ �����, ���� ������������.
	//prev_sample - �����, ����� �������� ���������� �������������.
	//name - ����, ������� ���������� ������ ������.
	//���� ����� ������ ���� ��� �� �����������
	//prev_sample==-1 - ������������� ����� ���������� ������
	//�� ������������� ������ Seek �� ��������� ������� �����.
	//���� ���� ��� ������� ���������� true, ��� �� ������,
	//��� �������� ����� ���������.
	//���� prev_sample<GetCurWritePos; //GetCurPos()+GetBufferLen(),
	//�� ������� ����� false.
	//��� ������� �� ��������� ������� � ������������ ����� fname.
	bool DeferredSeek(MpegPos seek_sample,int prev_sample,LPCSTR fname=NULL);

	//�������� DeferredSeek, ��������� prev_sample �����������
	bool Seek(MpegPos seek_sample,LPCSTR fname=NULL);

	//���� ������� � ����������, �� ��� �� 
	//��������� DeferredSeek,
	//���-�� ��������� ��� ������ OpenToPlay
	bool IsDeferredSeek();

	static MpegPos null_sample;//����� ������ �����

	//���������� �������� ��������� � ������� �� new_volume �� ����� time
	//��������� ��� ����� ����� ��� ������ SetVolume
	//����� �������� � DeferredSeek
	bool FadeVolume(float time,int new_volume=0);
protected:
	void InternalMpegSetVolume(int _volume);
	bool InitSoundBuffer();
	void InternalMpegStop();
	bool InternalMpegOpenToPlay(const char* _fname,bool cycled);
	bool InternalDeferredSeek(MpegPos seek_sample,int prev_sample,LPCSTR fname);

	int InternalGetCurPos(DWORD dwPlayCursor,bool* prevfile);//���������� �������� � ������
	int InternalGetCurPos(bool* prevfile);//���������� �������� � ������

	void TimeCallbackTrue();

	//� ����� ����� ������ ���� ����� ������, 
	//���� � ���� �������� offset ����
	void AddWriteOffset(DWORD offset);

	void PostSeek();//���������� �� ��������� �������� Seek

	friend DWORD WINAPI MpegThreadProc(LPVOID lpParameter);
	friend void MpegDeinitLibrary();

	bool DefferredSeek(DWORD cur_pos,DWORD cur_write_byte);

	void ClearFade();
	void FadeQuant();
};

//���� ����� �������� ��������� �� MpegSound, ��� ��� 
//������ ������� ����� ����� �� �������� �����,
//�� � ���� �������� ����� mpeg ��� ��������� ���������
//����� ��������� �� ������ �����, ����� �����.
class MpegSamplePoint
{
	int mpegsize;
	MpegPos* mpegpos;
	void Destroy();
public:
	MpegSamplePoint();
	~MpegSamplePoint();
	bool Load(LPCSTR fname);
	inline int size(){return mpegsize;}
	const MpegPos& operator[](int pos){return mpegpos[pos];};

};

#endif // _PLAYMPP_H_
